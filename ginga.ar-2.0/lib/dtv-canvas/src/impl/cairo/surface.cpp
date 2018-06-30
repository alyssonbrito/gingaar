#include "surface.h"
#include "canvas.h"
#include "../../size.h"
#include "../../rect.h"
#include <util/log.h>
#include <cairo/cairo.h>


#include <ft2build.h>
#include <pango/pango.h>
#include <pango/pangofc-fontmap.h>
#include <fontconfig/fontconfig.h>

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions/round.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/foreach.hpp>
#include <exception>
#include <stdexcept>
#if defined(GL)
#include <gdk/gdkzen.h>
#include <cairo-gl-ext.h>
#endif
#ifdef _WIN32
#define _USE_MATH_DEFINES
#include <pango/pangowin32.h>
#endif
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include "transition/Transition.h"
namespace bptnt = ::br::pucrio::telemidia::ncl::transition;

#define CMP_CAIRO(c) (double(c)/255.0)
#define COLOR_TO_CAIRO(c) CMP_CAIRO(c.r), CMP_CAIRO(c.g), CMP_CAIRO(c.b), CMP_CAIRO(c.alpha)

#define FONT_DPI 72 //TODO:parametrizar: http://www.gnu.org/s/guile-gnome/docs/pangocairo/html/Cairo-Rendering.html


namespace canvas {
namespace cairo {

namespace impl {

static inline void setup( cairo_t *cr ) {
	//	Set default line width
	cairo_set_line_width( cr, 1.0 );
	cairo_set_antialias( cr, CAIRO_ANTIALIAS_NONE );
}

static inline bool create( cairo_surface_t* &surface, cairo_t* &cr, int width, int height, bool argb ) {
#if defined(GL)
	if (argb) {
#endif
		surface = cairo_image_surface_create( CAIRO_FORMAT_ARGB32, width, height );
#if defined(GL)
	} else {
		surface = cairo_gl_surface_create( gdk_get_egl_device(), CAIRO_CONTENT_COLOR_ALPHA, width, height );
	}
#endif
	if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) {
		LERROR("cairo::Surface", "Cannot create cairo surface![%d][%d]",width,height);
		return false;
	}
	
	cr = cairo_create (surface);
	if (cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
		LERROR("cairo::Surface", "Cannot create cairo context!");
		return false;
	}

	setup( cr );

	return true;
}

static inline void destroy( cairo_surface_t *surface, cairo_t *cr ) {
	cairo_destroy(cr);
	cairo_surface_destroy (surface);
}

static inline void setColor( cairo_t *cr, const Color &color ) {
	//hack: if alpha is 0 we lose the rgb color of the pixel so
	//when alpha is 0 we change it to 1
	util::BYTE a = color.alpha;
	if ((color.r + color.g + color.b)>0 && a==0) {
		a = 1;
	}
	Color c = Color(color.r, color.g, color.b, a);
	cairo_set_source_rgba( cr, COLOR_TO_CAIRO(c) );
}

void gdk_cairo_set_source_pixbuf (
	cairo_t         *cr,
	const GdkPixbuf *pixbuf,
	double           pixbuf_x,
	double           pixbuf_y)
{
	gint width = gdk_pixbuf_get_width (pixbuf);
	gint height = gdk_pixbuf_get_height (pixbuf);
	guchar *gdk_pixels = gdk_pixbuf_get_pixels (pixbuf);
	int gdk_rowstride = gdk_pixbuf_get_rowstride (pixbuf);
	int n_channels = gdk_pixbuf_get_n_channels (pixbuf);
	int cairo_stride;
	guchar *cairo_pixels;
	cairo_format_t format;
	cairo_surface_t *surface;
	static const cairo_user_data_key_t key = {0};
	int j;

	if (n_channels == 3)
		format = CAIRO_FORMAT_RGB24;
	else
		format = CAIRO_FORMAT_ARGB32;

	cairo_stride = cairo_format_stride_for_width (format, width);
	cairo_pixels = (guchar *)g_malloc (height * cairo_stride);
	surface = cairo_image_surface_create_for_data ((unsigned char *)cairo_pixels,
		format,
		width, height, cairo_stride);
	if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) {
		LERROR("cairo::Surface", "Cannot create cairo from data!");
		throw std::runtime_error( "Cannot create cairo surface/context!" );
	}

	cairo_surface_set_user_data (surface, &key,
		cairo_pixels, (cairo_destroy_func_t)g_free);

	for (j = height; j; j--)
	{
		guchar *p = gdk_pixels;
		guchar *q = cairo_pixels;

		if (n_channels == 3)
		{
			guchar *end = p + 3 * width;

			while (p < end)
			{
			#if G_BYTE_ORDER == G_LITTLE_ENDIAN
				q[0] = p[2];
				q[1] = p[1];
				q[2] = p[0];
			#else
				q[1] = p[0];
				q[2] = p[1];
				q[3] = p[2];
			#endif
				p += 3;
				q += 4;
			}
		}
		else
		{
			guchar *end = p + 4 * width;
			guint t1,t2,t3;

		#define MULT(d,c,a,t) { t = c * a + 0x7f; d = (guchar)(((t >> 8) + t) >> 8); }

			while (p < end)
			{
			#if G_BYTE_ORDER == G_LITTLE_ENDIAN
				MULT(q[0], p[2], p[3], t1);
				MULT(q[1], p[1], p[3], t2);
				MULT(q[2], p[0], p[3], t3);
				q[3] = p[3];
			#else
				q[0] = p[3];
				MULT(q[1], p[0], p[3], t1);
				MULT(q[2], p[1], p[3], t2);
				MULT(q[3], p[2], p[3], t3);
			#endif

				p += 4;
				q += 4;
			}

		#undef MULT
		}

		gdk_pixels += gdk_rowstride;
		cairo_pixels += cairo_stride;
	}

	cairo_set_source_surface (cr, surface, pixbuf_x, pixbuf_y);
	cairo_surface_destroy (surface);
}

}	//	namespace impl


Surface::Surface( Canvas *canvas, ImageData *img )
	: canvas::Surface( Point(0,0) )
{
	_canvas = canvas;

	//	Create cairo surface for data
	const Size &size = img->size;
	int stride = cairo_format_stride_for_width( CAIRO_FORMAT_ARGB32, size.w );
	if (img->length < (size.h*stride)) {
		throw std::runtime_error( "Cannot create cairo surface/context!" );
	}
#if defined(GL)
	_surface = cairo_gl_surface_create_for_data (reinterpret_cast<unsigned char*>(img->data),
		gdk_get_egl_device(),
		CAIRO_FORMAT_ARGB32,
		size.w, size.h, stride );
#else
	_surface = cairo_image_surface_create_for_data(
		img->data,
		CAIRO_FORMAT_ARGB32,
		size.w, size.h,
		stride );
#endif
	_cr = cairo_create (_surface);
	if (cairo_status(_cr) != CAIRO_STATUS_SUCCESS) {
		throw std::runtime_error( "Cannot create cairo surface/context!" );
	}

	impl::setup( _cr );

	init();
	_masks = new map<void*, cairo_t*>;
	_masksurfs = new map<void*, cairo_surface_t*>;
	_borderCRs = new map<void*, cairo_t*>;
    _BorderSurfs= new map<void*, cairo_surface_t*>;
	_borderColors= new map<void*, std::string>;
	_borderIsTransIn = new map<void*,  bool>;
	curr_anim = 0;
}

Surface::Surface( Canvas *canvas, const Rect &rect )
	: canvas::Surface( Point(rect.x,rect.y) )
{
	if (rect.w<=0 || rect.h<=0 ) {
		throw std::runtime_error("[canvas::cairo::Surface] Cannot create surface!!! Invalid bounds");
	}

	_canvas = canvas;
	if (!impl::create( _surface, _cr, rect.w, rect.h, canvas->isARGB() )) {
		throw std::runtime_error( "Cannot create cairo surface/context!" );
	}
	init();
	_masks = new map<void*, cairo_t*>;
	_masksurfs = new map<void*, cairo_surface_t*>;
	_borderCRs = new map<void*, cairo_t*>;
    _BorderSurfs= new map<void*, cairo_surface_t*>;
	_borderColors= new map<void*, std::string>;
	_borderIsTransIn = new map<void*,  bool>;
	curr_anim = 0;
}

Surface::Surface( Canvas *canvas, const std::string &file )
	: canvas::Surface( Point(0,0) )
{
	_canvas = canvas;

	GError *err=NULL;
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file( file.c_str(), &err );
	if (!pixbuf) {
		LERROR( "cairo::Surface", "Cannot load image file: file=%s, err=%s\n",
			file.c_str(), err->message );
		throw std::runtime_error( "Cannot load image file!" );
	}

	int width = gdk_pixbuf_get_width(pixbuf);
	int height = gdk_pixbuf_get_height(pixbuf);
	if (!impl::create( _surface, _cr, width, height, canvas->isARGB() )) {
		LERROR( "cairo::Surface", "Cannot create cairo surface/context!" );
		throw std::runtime_error( "Cannot create cairo surface/context!" );
	}
	init();

	impl::gdk_cairo_set_source_pixbuf( _cr, pixbuf, 0, 0 );
	cairo_paint( _cr );

	g_object_unref(pixbuf);
	_masks = new map<void*, cairo_t*>;
	_masksurfs = new map<void*, cairo_surface_t*>;
	_borderCRs = new map<void*, cairo_t*>;
    _BorderSurfs= new map<void*, cairo_surface_t*>;
	_borderColors= new map<void*, std::string>;
	_borderIsTransIn = new map<void*,  bool>;


	curr_anim = 0;
}

DefaultFontData *Surface::_pangoDefaultFont = NULL;

void Surface::init() {
	_alpha = 1.0;
	_pangoFont = NULL;
	_pangoLayout = NULL;
	setCompositionModeImpl( getCompositionMode() );
	_webkitwindow = NULL;
	if (!_pangoDefaultFont) {
		_pangoDefaultFont = new DefaultFontData( loadFont(), _pangoLayout );
	}
	assert(_pangoDefaultFont->font());
	assert(_pangoDefaultFont->layout());
	_pangoDefaultFont->ref();
}

Surface::~Surface()
{
	Masks::iterator im = _masks->begin();
	while (im != _masks->end()) {
		cairo_t *_maskcr = im->second;
		_masks->erase( im );
		cairo_destroy(_maskcr);
		++im;
	}

	Masksurfs::iterator ims = _masksurfs->begin();
	while (ims != _masksurfs->end()) {
		cairo_surface_t *_masksurface = ims->second;;
		_masksurfs->erase( ims );
		cairo_surface_destroy(_masksurface);
		++ims;
	}
	delete _masks;
        delete _masksurfs;
        _masks = NULL;
        _masksurfs = NULL;

	BorderCRs::iterator imb = _borderCRs->begin();
        while (imb != _borderCRs->end()) {
                cairo_t *_bordercr = imb->second;
                _borderCRs->erase( imb );
                cairo_destroy(_bordercr);
                ++imb;
        }
        BorderSurfs::iterator imbs = _BorderSurfs->begin();
        while (imbs != _BorderSurfs->end()) {
                cairo_surface_t *_bordersurface = imbs->second;;
                _BorderSurfs->erase( imbs );
                cairo_surface_destroy(_bordersurface);
                ++imbs;
        }
	
	BorderColors::iterator imbc = _borderColors->begin();
	while (imbc != _borderColors->end()) {
                std::string border_color = imbc->second;;
                _borderColors->erase( imbc );
                ++imbc;
        }
    
    BorderIsTransIn::iterator imbi = _borderIsTransIn->begin();
	while (imbi != _borderIsTransIn->end()) {
               // bool border_color = imbc->second;;
                _borderIsTransIn->erase( imbi );
                ++imbi;
    }


	delete  _borderCRs;
	delete 	_BorderSurfs;
	delete  _borderColors;
	delete _borderIsTransIn;
	_borderCRs = 	NULL;
	_BorderSurfs =  NULL;
	_borderColors = NULL;
	_borderIsTransIn = NULL;
	markDirty();
	impl::destroy( _surface, _cr );
	if (_pangoLayout && _pangoLayout != _pangoDefaultFont->layout()) {
		g_object_unref( _pangoLayout );
	}
	assert(_pangoDefaultFont&&_pangoDefaultFont->font());
	if (_pangoFont && _pangoFont != _pangoDefaultFont->font()) {
		g_object_unref( _pangoFont );
	}
	_pangoDefaultFont->deref();
	if (_pangoDefaultFont->refCount() == 0) {
		delete _pangoDefaultFont;
		_pangoDefaultFont = NULL;
	}
	//uninstall all fonts used by Ginga App
	FcConfig* config = FcConfigGetCurrent();
        FcConfigAppFontClear(config);

}

cairo_surface_t *Surface::getContent() {
	return _surface;
}

cairo_t *Surface::getContext() {
	return _cr;
}

canvas::Canvas *Surface::canvas() const {
	return _canvas;
}

GdkWindow *Surface::getWebkitWindow() const {
	return _webkitwindow;
}

void Surface::setWebkitWindow( GdkWindow *window ) {
	_webkitwindow = window;
}

/****************************** Size functions ********************************/

Size Surface::getSize() const {
	Size size;
#if defined(GL)
	if (cairo_surface_get_type(_surface) == CAIRO_SURFACE_TYPE_GL) {
		size.w = cairo_gl_surface_get_width(_surface);
		size.h = cairo_gl_surface_get_height(_surface);
	} else {
#endif
		size.w = cairo_image_surface_get_width( _surface );
		size.h = cairo_image_surface_get_height( _surface );
#if defined(GL)
	}
#endif
	return size;
}

/****************************** Clip functions ********************************/

bool Surface::getClip( Rect &rect ) {
	double x1, y1, x2, y2;

	cairo_clip_extents( _cr, &x1, &y1, &x2, &y2 );

	rect.x = (int) x1;
	rect.y = (int) y1;
	rect.w = (int) (x2 - x1);
	rect.h = (int) (y2 - y1);

	return true;
}

void Surface::setClipImpl( const Rect &rect ) {
	cairo_reset_clip( _cr );
	cairo_rectangle( _cr, rect.x, rect.y, rect.w, rect.h );
	cairo_clip( _cr );
}

/****************************** Line functions ********************************/

void Surface::drawLineImpl( int x1, int y1, int x2, int y2 ) {
	impl::setColor( _cr, getColor() );
	cairo_set_line_cap  (_cr, CAIRO_LINE_CAP_ROUND);
	cairo_move_to (_cr, (x1<x2)? x1 : x1+1, (y1<y2)? y1 : y1+1 );
	cairo_line_to (_cr, (x1<x2)? x2+1 : x2, (y1<y2)? y2+1 : y2 );
	cairo_set_line_width (_cr, 1);
	cairo_stroke (_cr);
}

/****************************** Rect functions ********************************/

void Surface::drawRectImpl( const Rect &rect ) {
	impl::setColor( _cr, getColor() );
	cairo_rectangle ( _cr, rect.x+1, rect.y+1, rect.w-1, rect.h-1 );
	cairo_stroke (_cr);
}

void Surface::fillRectImpl( const Rect &rect ) {
	impl::setColor( _cr, getColor() );
	cairo_rectangle ( _cr, rect.x, rect.y, rect.w, rect.h );
	cairo_fill(_cr);
}

/*************************** RoundRect functions ******************************/
void Surface::drawRoundRectImpl( const Rect &rect, int arcW, int arcH ) {
        impl::setColor( _cr, getColor() );

	int     x = rect.x + 1, // Adjustment position because of division by 2
                y = rect.y + 1; //  by 2 without it the position is wrong
        double width = rect.w -1;// wrong with other objects, i.e, line, rectangle,
        double height = rect.h-1;// and so on.

        double arcWidth = arcW;
        double arcHeight = arcH;

        width = width - (arcWidth*2);
        if (width <0) {
                width =0;
                arcWidth = rect.w /2;
        }
        height = height - (arcHeight*2);
        if (height < 0){
                height = 0;
                arcHeight = rect.h /2;
        }

        /* Point Map used in to draw the Round Rect below
         P2#P3---------------------------------P4#P5
         P1|                                   |P6
           |                                   |
        P12|                                   |P7
        P11#P10------------------------------P9#P8  */

        Point P1 = Point(x              , y + arcHeight);
        Point P2 = Point(x              , y);
        Point P3 = Point(x + arcWidth   , y);
        Point P4 = Point(P3.x + width   , y);
        Point P5 = Point(P4.x + arcWidth, y);
        Point P6 = Point(P5.x           , y + arcHeight);
        Point P7 = Point(P5.x           , P6.y + height);
        Point P8 = Point(P5.x           , P7.y + arcHeight);
        Point P9 = Point(P4.x           , P8.y);
        Point P10= Point(P3.x           , P8.y);
        Point P11= Point(P2.x           , P8.y);
        Point P12= Point(P1.x           , P1.y + height);
        cairo_save (_cr);
        cairo_curve_to(_cr, P1.x, P1.y, P2.x, P2.y, P3.x, P3.y);
        cairo_line_to(_cr, P4.x, P4.y);
        cairo_curve_to(_cr, P4.x, P4.y, P5.x,P5.y, P6.x, P6.y);
        cairo_line_to (_cr, P7.x, P7.y);
        cairo_curve_to (_cr, P7.x, P7.y, P8.x, P8.y, P9.x, P9.y);
        cairo_line_to (_cr, P10.x, P10.y);
        cairo_curve_to (_cr, P10.x, P10.y, P11.x, P11.y, P12.x, P12.y);
        cairo_close_path(_cr);
        cairo_stroke(_cr);
        cairo_restore(_cr);
}

void Surface::fillRoundRectImpl( const Rect &rect, int arcW, int arcH ) {
	impl::setColor( _cr, getColor() );
	int     x = rect.x + 1, // Adjustment position because of division by 2
                y = rect.y + 1; //  by 2 without it the position is wrong
        double width = rect.w -1;// wrong with other objects, i.e, line, rectangle,
        double height = rect.h-1;// and so on.

	double arcWidth = arcW;
	double arcHeight = arcH;
	
	width = width - (arcWidth*2);
	if (width <0) {
		width =0;
		arcWidth = rect.w /2;
	}
	height = height - (arcHeight*2);
	if (height < 0){
		height = 0;
		arcHeight = rect.h /2;
	}

	/* Point Map used in to draw the Round Rect below
	 P2#P3---------------------------------P4#P5
	 P1|                                   |P6
	   |                                   |
	P12|                                   |P7
	P11#P10------------------------------P9#P8  */

	Point P1 = Point(x              , y + arcHeight);
	Point P2 = Point(x              , y);
	Point P3 = Point(x + arcWidth   , y);
	Point P4 = Point(P3.x + width   , y);
	Point P5 = Point(P4.x + arcWidth, y);
	Point P6 = Point(P5.x           , y + arcHeight);
	Point P7 = Point(P5.x           , P6.y + height);
	Point P8 = Point(P5.x           , P7.y + arcHeight);
	Point P9 = Point(P4.x           , P8.y);
	Point P10= Point(P3.x           , P8.y);
	Point P11= Point(P2.x           , P8.y);
	Point P12= Point(P1.x           , P1.y + height);

	cairo_save (_cr);
	cairo_curve_to(_cr, P1.x, P1.y, P2.x, P2.y, P3.x, P3.y);
	cairo_line_to(_cr, P4.x, P4.y);
	cairo_curve_to(_cr, P4.x, P4.y, P5.x,P5.y, P6.x, P6.y);
	cairo_line_to (_cr, P7.x, P7.y);
	cairo_curve_to (_cr, P7.x, P7.y, P8.x, P8.y, P9.x, P9.y);
	cairo_line_to (_cr, P10.x, P10.y);
	cairo_curve_to (_cr, P10.x, P10.y, P11.x, P11.y, P12.x, P12.y);
	cairo_close_path(_cr);
	cairo_fill_preserve(_cr);
	cairo_stroke(_cr);
	cairo_restore(_cr);
	
}

/**************************** Polygon functions ******************************/

void Surface::drawRelLineTo( const Point &lp, const Point &np ) {
	int x = lp.x;
	int y = lp.y;
	if (lp.x < np.x) {
		if (lp.y < np.y) {
			x = np.x-lp.x;	// Hacia abajo a la derecha
			y = np.y-lp.y;
		} else if (lp.y > np.y) {
			x = np.x-lp.x+1;// Hacia arriba a la derecha
			y = np.y-lp.y-1;
		} else {
			x = np.x-lp.x;// Horizontal hacia la derecha
			y = np.y-lp.y;
		}
	} else if (lp.x > np.x) {
		if (lp.y < np.y) {
			x = np.x-lp.x;	// Hacia abajo a la izquierda
			y = np.y-lp.y;
		} else if (lp.y > np.y) {
			x = np.x-lp.x-1;	// Hacia arriba a la izquierda
			y = np.y-lp.y-1;
		} else {
			x = np.x-lp.x;	// Horizontal hacia la izquierda
			y = np.y-lp.y;
		}
	} else {
		if (lp.y < np.y) {
			x = np.x-lp.x;	// Vertical hacia abajo
			y = np.y-lp.y+1;
		} else if (lp.y > np.y) {
			x = np.x-lp.x; 	// Vertical hacia arriba
			y = np.y-lp.y+1;
		}
	}
	cairo_rel_line_to(_cr, x, y );
}

void Surface::drawLineTo( const Point &lp, const Point &np ) {
	int x= lp.x;
	int y= lp.y;
	if (lp.x < np.x) {
		if (lp.y < np.y) {
			x = np.x-lp.x;	// Hacia abajo a la derecha
			y = np.y-lp.y;
		} else if (lp.y > np.y) {
			x = np.x-lp.x-1;// Hacia arriba a la derecha
			y = np.y-lp.y+1;
		} else {
			x = np.x-lp.x+1;// Horizontal hacia la derecha
			y = np.y-lp.y;
		}
	} else if (lp.x > np.x) {
		if (lp.y < np.y) {
			x = np.x-lp.x-1;// Hacia abajo a la izquierda
			y = np.y-lp.y+1;
		} else if (lp.y > np.y) {
			x = np.x-lp.x;	// Hacia arriba a la izquierda
			y = np.y-lp.y;
		} else {
			x = np.x-lp.x+1;// Horizontal hacia la izquierda
			y = np.y-lp.y;
		}
	} else {
		if (lp.y < np.y) {
			x = np.x-lp.x;	// Vertical hacia abajo
			y = np.y-lp.y-1;
		} else if (lp.y > np.y) {
			x = np.x-lp.x; 	// Vertical hacia arriba
			y = np.y-lp.y-1;
		}
	}
	cairo_rel_line_to(_cr, x, y );
}

void Surface::drawPolygonImpl( const std::vector<Point>& vertices, bool closed ) {
	impl::setColor( _cr, getColor() );
	cairo_set_line_cap  (_cr, CAIRO_LINE_CAP_ROUND);
	cairo_set_line_width (_cr, 1);

	const Point& fp = vertices[0];
	Point lp = fp;	// Last point drawn
	Point np = vertices[1]; // Next point to draw
	int mx, my;
	if (lp.x < np.x) {
		if (lp.y < np.y) {
			mx = lp.x+1;
			my = lp.y+1;
		} else if (lp.y > np.y) {
			mx = lp.x+1;
			my = lp.y;
		} else {
			mx = lp.x;
			my = lp.y+1;
		}
	} else if(lp.x > np.x) {
		if (lp.y < np.y) {
			mx = lp.x+1;
			my = lp.y;
		} else if (lp.y > np.y) {
			mx = lp.x+1;
			my = lp.y+1;
		} else {
			mx = lp.x;
			my = lp.y+1;
		}
	} else {
		mx = lp.x+1;
		my = lp.y+1;
	}
	cairo_move_to( _cr, mx, my );
	for (size_t i=1; i<vertices.size()-1; i++) {
		np = vertices[i];
		drawLineTo(lp, np);
		lp = np;
	}
	np = vertices[vertices.size()-1];
	if (np!=fp) {
		drawLineTo(lp, np);
	}

	if (closed || (!closed && np == fp)) {
		cairo_close_path( _cr );
	}

	cairo_stroke (_cr);
}

void Surface::fillPolygonImpl(const std::vector<Point>& vertices ) {
	impl::setColor( _cr, getColor() );
	cairo_set_line_cap  (_cr, CAIRO_LINE_CAP_ROUND);
	cairo_set_line_width (_cr, 1);

	Point fp = vertices[0];
	Point lp = fp; // Last point drawn
	Point np = vertices[1]; // Next point to draw
	int mx = lp.x;
	int my = lp.y;
	if (lp.x < np.x) {
		if (lp.y < np.y) {
			mx = lp.x+1;
			my = lp.y;
		} else if (lp.y > np.y) {
			mx = lp.x;
			my = lp.y+1;
		} else {
			mx = lp.x+1;
			my = lp.y;
		}
	} else if(lp.x > np.x) {
		if (lp.y < np.y) {
			mx = lp.x+1;
			my = lp.y+1;
		} else if (lp.y > np.y) {
			mx = lp.x+1;
			my = lp.y+1;
		} else {
			mx = lp.x+1;
			my = lp.y+1;
		}
	} else {
		if (lp.y < np.y) {
			mx = lp.x+1;
			my = lp.y;
		} else if (lp.y > np.y) {
			mx = lp.x;
			my = lp.y;
		}
	}

	cairo_move_to( _cr, mx, my );
	for (size_t i=1; i<vertices.size()-1; i++) {
		np = vertices[i];
		drawRelLineTo(lp, np);
		lp = np;
	}
	np = vertices[vertices.size()-1];
	if (np!=fp) {
		drawRelLineTo(lp, np);
	}
	cairo_close_path( _cr );
	cairo_fill (_cr);
}

/**************************** Ellipse functions ******************************/

void Surface::drawEllipseImpl( const canvas::Point &center, int rw, int rh, int angStart, int angStop ) {
	double angle1 =(angStart  * (M_PI/180.0));   /* angles are specified */
        double angle2 =(angStop * (M_PI/180.0));     /* in radians           */

	cairo_save(_cr);
	cairo_translate(_cr,center.x+0.5,center.y+0.5);
	cairo_scale(_cr,rw  , rh );
	cairo_arc(_cr,0.0, 0.0, 1.0, angle1, angle2);
	cairo_restore(_cr);
	impl::setColor( _cr, getColor() );
	cairo_stroke(_cr);
}


void Surface::fillEllipseImpl( const canvas::Point &center, int rw, int rh, int angStart, int angStop ) {
	double angle1 =((angStart)  * (M_PI/180.0));  	/* angles are specified */
	double angle2 =((angStop) * (M_PI/180.0));  	/* in radians           */
	cairo_set_line_width (_cr, 1.0);
	impl::setColor( _cr, getColor() );
        cairo_save(_cr);
	cairo_translate(_cr, center.x+0.5 , center.y+0.5);
        cairo_scale(_cr, rw , rh );
        cairo_arc (_cr, 0, 0, 1.0, angle1, angle2);
	cairo_line_to (_cr, 0, 0);
	cairo_close_path(_cr);
	impl::setColor( _cr, getColor() );
	cairo_fill_preserve(_cr);
        cairo_restore(_cr);
        cairo_stroke(_cr);

}

std::string Surface::installFontFile(const std::string &fontfile){
	FT_Face     face;
	FT_Library library;
	//FcFontSet *fonts; // use for debug
	std::string familyName="";
    	
	FcConfig* config = FcConfigGetCurrent(); 
	if (!FcConfigAppFontAddFile (config,reinterpret_cast<const FcChar8*>(fontfile.c_str()))) {
		LERROR("cairo::Surface", "Failed to install app font.[%s]", fontfile.c_str());
		return familyName;
	}	
	/* use for debug only, it prints the installed font info*/
	/*fonts = FcConfigGetFonts (config, FcSetApplication);
	FcPatternPrint (fonts->fonts[0]);*/
	FT_Init_FreeType(&library);
	FT_New_Face (library, fontfile.c_str(), 0, &face);	
	if (face->family_name) familyName = face->family_name;
	
	return familyName;

}
bool Surface::applyFont() {
	freeFont();
	_pangoFont = loadFont();

	assert(_pangoFont);
	return true;
}

void Surface::freeFont() {
	assert(_pangoDefaultFont&&_pangoDefaultFont->font());
	if (_pangoFont && _pangoFont != _pangoDefaultFont->font()) {
		g_object_unref( _pangoFont );
	} 
	_pangoFont = NULL;
}
//TODO:	change to use non installed fonts
PangoFont* Surface::loadFont() {
	PangoFont *pangoFont = NULL;
	const Font &font = getFont();

	PangoFontDescription *desc = pango_font_description_new();

	//	Size
	pango_font_description_set_absolute_size( desc, font.size() * PANGO_SCALE);

	//	Weight
	if (font.bold()) {
		pango_font_description_set_weight( desc, PANGO_WEIGHT_BOLD );
	}
	else {
		pango_font_description_set_weight( desc, PANGO_WEIGHT_NORMAL );
	}

	//	Style
	if (font.italic()) {
		pango_font_description_set_style( desc, PANGO_STYLE_ITALIC );
	}
	else {
		pango_font_description_set_style( desc, PANGO_STYLE_NORMAL);
	}

	//	Variant
	if (font.smallCaps()) {
		pango_font_description_set_variant( desc, PANGO_VARIANT_SMALL_CAPS );
	}
	else {
		pango_font_description_set_variant( desc, PANGO_VARIANT_NORMAL );
	}

	//	Family
	std::vector<std::string> families  = font.families();
	BOOST_FOREACH( std::string &family, families) {

		// Family: Hack DejaVuSans!
		if (family == "DejaVuSans") {
			family = "DejaVu Sans";
		}

		pango_font_description_set_family( desc, family.c_str() );

		// Get fontmap
		PangoFontMap *fontMap = pango_cairo_font_map_get_default();

		// Create context from fontmap
		PangoContext *context = pango_font_map_create_context(fontMap);

		// Load font
		pango_context_set_font_description( context, desc );

		pango_cairo_context_set_resolution( context, FONT_DPI );

		pangoFont = pango_font_map_load_font( fontMap, context, desc );

		// Free context
		g_object_unref(context);

		// Validate that the selected font is correct
		if (pangoFont != NULL) {
			PangoFontDescription *fontDesc = pango_font_describe(pangoFont);
			char *fontString=pango_font_description_to_string(fontDesc);
			pango_font_description_free(fontDesc);

			if (strncmp(fontString,family.c_str(),family.size()) != 0) {
				
				
				// Free the erroneous font
				g_object_unref( pangoFont );
				pangoFont = NULL;

				g_free(fontString);
			} else {
				g_free(fontString);
				break;
			}

		}
	}
	
	char *strDesc=pango_font_description_to_string(desc);
	//LDEBUG("cairo::Surface", "Font to use: %s", strDesc);
	g_free(strDesc);

	//	Update layout
	if (_pangoLayout && _pangoLayout != _pangoDefaultFont->layout()) {
		g_object_unref( _pangoLayout );
	}
	_pangoLayout = pango_cairo_create_layout( _cr );
	pango_layout_set_font_description( _pangoLayout, desc );

	pango_font_description_free(desc);
	return pangoFont;
}

PangoLayout *Surface::currentLayout() {
	//	Update current font
	currentFont();
	if (!_pangoLayout) {
		_pangoLayout = _pangoDefaultFont->layout();
		assert(_pangoLayout);
	}
	return _pangoLayout;
}

PangoFont *Surface::currentFont() {
	if (!_pangoFont) {
		_pangoFont = _pangoDefaultFont->font();
		assert(_pangoFont);
	}
	return _pangoFont;
}

/****************************** Text functions ********************************/

char * normalizeText(const std::string &text){
	char * utf8_text;
	if(!g_utf8_validate(text.c_str(), -1, NULL)){
		//LDEBUG( "cairo::Surface", "The text isnt in UTF-8");
		GError *error = NULL;

		utf8_text = g_convert (text.c_str(), -1, "UTF-8", "ISO-8859-1", NULL, NULL, &error);
		if (error != NULL) {
			//LERROR( "cairo::Surface", "Couldn't convert to UTF-8");
			g_error_free (error);
			utf8_text = g_strdup(text.c_str());
		}
	}else
		utf8_text = g_strdup(text.c_str());

	return utf8_text;
}

void Surface::drawTextImpl( const Point &pos, const std::string &text, int ascent ) {
	PangoLayout *layout = currentLayout();
	impl::setColor( _cr, getColor() );
	cairo_move_to( _cr, pos.x, pos.y - ascent + 1 );

	char * utf8_text = normalizeText(text);

	pango_layout_set_text( layout, utf8_text, -1);

	g_free (utf8_text);
	pango_cairo_update_layout(_cr, layout );
	pango_cairo_show_layout(_cr, layout );
}

void Surface::drawTextImpl( const Rect &rect, const std::string &text, Alignment /*alignment*//*=alignment::None*/, int spacing/*=0*/ ) {
	PangoLayout *layout = currentLayout();

	pango_layout_set_wrap( layout, PANGO_WRAP_WORD_CHAR );
	pango_layout_set_width  ( layout, rect.w * PANGO_SCALE );
	pango_layout_set_height ( layout, rect.h * PANGO_SCALE );
	pango_layout_set_spacing ( layout, spacing * PANGO_SCALE );

	cairo_rectangle ( _cr, rect.x, rect.y, rect.w, rect.h );

	//	Set background color
	impl::setColor( _cr, getBackgroundColor() );
	if ( getTextAlign() == "justify") {
		pango_layout_set_justify (layout, true);
	} else {
		PangoAlignment align;
		pango_layout_set_justify ( layout, false );
		if (getTextAlign() == "left") {
			align = PANGO_ALIGN_LEFT;
		} else if (getTextAlign() == "right") {
			align = PANGO_ALIGN_RIGHT;
		} else if (getTextAlign() == "center") {
			align = PANGO_ALIGN_CENTER;
		} else {
			align = PANGO_ALIGN_LEFT;
		}
		pango_layout_set_alignment ( layout, align );
	}

	cairo_fill_preserve (_cr);

	cairo_clip (_cr);
	cairo_move_to( _cr, rect.x, rect.y );

	impl::setColor( _cr, getColor() );

	char * utf8_text = normalizeText(text);

	pango_layout_set_text( layout, utf8_text, -1);

	g_free (utf8_text);

	pango_cairo_update_layout(_cr, layout );
	pango_cairo_show_layout(_cr, layout );

	cairo_reset_clip (_cr);
}

void Surface::measureTextImpl( const std::string &text, Size &size ) {
	PangoLayout *layout = currentLayout();
	char * utf8_text = normalizeText(text);
	pango_layout_set_text( layout, utf8_text, -1);
	g_free (utf8_text);
	pango_layout_get_pixel_size ( layout, &size.w, &size.h);
}

int Surface::fontAscent() {
	PangoFontMetrics *metrics = pango_font_get_metrics( currentFont(), pango_language_get_default());
	int tmp = pango_font_metrics_get_ascent( metrics ) / PANGO_SCALE;
	pango_font_metrics_unref(metrics);
	return tmp;
}

int Surface::fontDescent() {
	PangoFontMetrics *metrics = pango_font_get_metrics( currentFont(), pango_language_get_default());
	int tmp = pango_font_metrics_get_descent( metrics ) / PANGO_SCALE;
	pango_font_metrics_unref(metrics);
	return tmp;
}

bool Surface::setOpacity( util::BYTE alpha ) {
	_alpha = (float) (alpha / 255.0);
	markDirty();
	return true;
}

util::BYTE Surface::getOpacity() const{
	return (util::BYTE) (_alpha * 255);
}

float Surface::alpha() const {
	return _alpha;
}

void Surface::setCompositionModeImpl( composition::mode mode ) {
	switch (mode) {
		case composition::source_over: _op = CAIRO_OPERATOR_OVER; break;
		case composition::source: _op = CAIRO_OPERATOR_SOURCE; break;
		case composition::clear: _op = CAIRO_OPERATOR_CLEAR; break;
		default: return;
	}
	cairo_set_operator( _cr, _op );
}

cairo_t *Surface::GetMask() {
	return (*_masks)[curr_anim];
}

cairo_surface_t *Surface::GetMaskSurf() {
	return (*_masksurfs)[curr_anim];
}

cairo_t *Surface::GetBorder() {
        return (*_borderCRs)[curr_anim];
}

cairo_surface_t *Surface::GetBorderSurf() {
        return (*_BorderSurfs)[curr_anim];
}
std::string  Surface::getBorderColor(){ 
        return (*_borderColors)[curr_anim] ;
}



void Surface::blitImpl( const Point &targetPoint, canvas::Surface *srcSurface, const Rect &source ) {
	Surface *src=dynamic_cast<Surface *>(srcSurface);

	if (src->getWebkitWindow()) {
		gdk_cairo_set_source_window( _cr, src->getWebkitWindow(), targetPoint.x-source.x, targetPoint.y-source.y );
	} else {
		cairo_set_source_surface ( _cr, src->getContent(), targetPoint.x-source.x, targetPoint.y-source.y );
	}

	cairo_rectangle ( _cr, targetPoint.x, targetPoint.y, source.w, source.h );
	cairo_clip (_cr);
	cairo_t* src_maskcr = src->GetMask();
	cairo_surface_t* src_masksurface = src->GetMaskSurf();
	if (src_maskcr) {
		cairo_mask_surface( _cr, src_masksurface, targetPoint.x-source.x, targetPoint.y-source.y );
	} else {
		cairo_paint_with_alpha( _cr, src->alpha() );
	}
	cairo_reset_clip (_cr);
}

bool Surface::blitEx(const Point &targetPoint, canvas::Surface *srcSurface, const Rect &srcRect, bool withAlpha /*= true*/, bool saveClip /*= false*/)
{
	if (srcRect.w <= 0 || srcRect.h <= 0) {
		const Rect &bounds=getBounds();
		LWARN("Surface", "blit fail. Invalid source rect: srcRect=(x=%d y=%d w=%d h=%d) bounds=(%d,%d,%d,%d), targetPoint=(x=%d y=%d) ",
		srcRect.x, srcRect.y, srcRect.w, srcRect.h,
		bounds.x, bounds.y, bounds.w, bounds.h,
		targetPoint.x, targetPoint.y);
		return false;
	}

	Surface *surface=dynamic_cast<Surface *>(srcSurface);

	if (surface->getWebkitWindow()) {
		gdk_cairo_set_source_window( _cr, surface->getWebkitWindow(), targetPoint.x-srcRect.x, targetPoint.y-srcRect.y );
	} else {
		cairo_set_source_surface( _cr, surface->getContent(), targetPoint.x-srcRect.x, targetPoint.y-srcRect.y );
	}

	double x1, y1, x2, y2;
	if (saveClip) {
		cairo_clip_extents( _cr, &x1, &y1, &x2, &y2 );
	}
	cairo_rectangle( _cr, targetPoint.x, targetPoint.y, srcRect.w, srcRect.h );
	cairo_clip(_cr);

	if (withAlpha) {
		cairo_paint_with_alpha( _cr, surface->alpha() );
	} else {
		cairo_paint(_cr);
	}

	cairo_reset_clip(_cr);
	if (saveClip) {
		cairo_rectangle( _cr, x1, y1, x2 - x1, y2 - y1 );
		cairo_clip(_cr);
	}

	markDirtySurface( Rect(targetPoint.x, targetPoint.y, srcRect.w, srcRect.h) );
	return true;
}

void Surface::scaleImpl( const Rect &targetRect, canvas::Surface *srcSurface, const Rect &sourceRect, bool flipw /*=false*/, bool fliph /*=false*/ ) {
	Surface *src = NULL;
	Rect r(Point(0,0),srcSurface->getSize());
	if (r != sourceRect) {
		canvas::Surface *tmp = canvas()->createSurface(Size(sourceRect.w, sourceRect.h));
		tmp->blitEx(Point(0,0), srcSurface, sourceRect, false, false);
		src = dynamic_cast<Surface *>(tmp);
	} else {
		src = dynamic_cast<Surface *>(srcSurface);
	}
	//#1 Save the clip area 
	double x1, y1, x2, y2;
        cairo_clip_extents( _cr, &x1, &y1, &x2, &y2 );

	cairo_t *cr = NULL;
	cairo_surface_t *content = NULL;

	double fx = ((double)targetRect.w / (double)sourceRect.w) * (flipw ? -1.0 : 1.0);
	double fy = ((double)targetRect.h / (double)sourceRect.h) * (fliph ? -1.0 : 1.0);
	impl::create( content, cr, targetRect.w, targetRect.h, false );
	cairo_scale( cr, fx, fy );
	if (fx<0 && fy<0) {
		cairo_translate( cr, targetRect.w/fx, targetRect.h/fy );
	} else if (fx < 0) {
		cairo_translate( cr, targetRect.w/fx, 0 );
	} else if (fy < 0) {
		cairo_translate( cr, 0, targetRect.h/fy );
	}

	if (src->getWebkitWindow()) {
		gdk_cairo_set_source_window( cr, src->getWebkitWindow(), 0, 0 );
	} else {
		cairo_set_source_surface( cr, src->getContent(), 0, 0 );
	}

	cairo_pattern_set_filter( cairo_get_source(cr), CAIRO_FILTER_GOOD );
	cairo_set_operator( cr, CAIRO_OPERATOR_SOURCE);
	cairo_paint( cr );

	cairo_set_source_surface( _cr, content, targetRect.x, targetRect.y );
	cairo_rectangle ( _cr, targetRect.x, targetRect.y, targetRect.w, targetRect.h );
	cairo_clip (_cr);
	cairo_paint_with_alpha( _cr, (dynamic_cast<Surface *>(srcSurface))->alpha() );
	cairo_reset_clip (_cr);

	//restore the clip area saved in #1, for the next blit
	cairo_rectangle( _cr, x1, y1, x2 - x1, y2 - y1 );
        cairo_clip(_cr);

	impl::destroy( content, cr );
	if (r != sourceRect) {
		canvas::Surface *tmp = dynamic_cast<canvas::Surface *>(src);
		canvas()->destroy( tmp );
	}
}

canvas::Surface *Surface::rotateImpl( int degrees ) {
	int tw=0;
	int th=0;
	canvas::Rect rect;
	canvas::Size size = getSize();
	if (degrees==90 || degrees==270) {
		canvas::Point pos = getLocation();
		rect = canvas::Rect(pos.x,pos.y,size.h,size.w);
		if (degrees==90) {
			tw = -size.h;
			th = -size.w;
		} else {
			tw = -size.h;
			th = -size.w;
		}
	} else {
		rect = getBounds();
		if (degrees==180) {
			tw = -size.w;
			th = -size.h;
		}
	}
	Surface *newSurface = dynamic_cast<Surface *>( createSimilar( rect ) );
	double radians = ((double)degrees)*boost::math::constants::pi<double>()/180.0;

	cairo_t *ctx = newSurface->getContext();
	cairo_translate( ctx, -tw/2.0, -th/2.0);
	cairo_rotate( ctx, radians );
	cairo_translate( ctx, tw/2.0, th/2.0);
	//cairo_translate( ctx, tw, th);
	cairo_set_source_surface( ctx, getContent(), 0, 0 );
	cairo_paint( ctx );
	return newSurface;
}

/*********************** Pixel manipulation functions *************************/
util::DWORD Surface::getPixel( const Point &pos ) {
	int width = 0;
	int height = 0;
	cairo_surface_t *originSurface = getContent();
	if (cairo_surface_get_type(originSurface) == CAIRO_SURFACE_TYPE_IMAGE) {
		width = cairo_image_surface_get_width(originSurface);
		height = cairo_image_surface_get_height(originSurface);
	} else
#if defined(GL)
	if (cairo_surface_get_type(_surface) == CAIRO_SURFACE_TYPE_GL) {
		width = cairo_gl_surface_get_width(_surface);
		height = cairo_gl_surface_get_height(_surface);
	} else
#endif
	{
		LWARN("cairo::Surface", "Wrong surface type for getPixel");
		return 0;
	}
	cairo_surface_t *tmpSurface = NULL;
	canvas::cairo::Surface *newSurface = NULL;
	cairo_format_t format = cairo_image_surface_get_format(originSurface);
	if (format == CAIRO_FORMAT_RGB24
#if defined(GL)
		 || cairo_surface_get_type(_surface) == CAIRO_SURFACE_TYPE_GL
#endif
		) {
		canvas()->setARGB(true);
		newSurface = static_cast<canvas::cairo::Surface*>(canvas()->createSurface( Rect( 0, 0, width, height )));
		cairo_t *cr_temp = newSurface->getContext();
		cairo_set_operator( cr_temp, CAIRO_OPERATOR_SOURCE );
		cairo_rectangle ( cr_temp, 0,  0,  width, height );
		cairo_set_source_surface( cr_temp, originSurface, 0, 0 );
		cairo_clip ( cr_temp );
		cairo_paint( cr_temp );
		cairo_reset_clip (cr_temp);
		tmpSurface = newSurface->getContent();
		if (cr_temp){
                	cairo_destroy(cr_temp);
        	}
        	cr_temp=NULL;	
		assert( cairo_image_surface_get_format(tmpSurface) == CAIRO_FORMAT_ARGB32 );
	} else if (format == CAIRO_FORMAT_ARGB32) {
		tmpSurface = originSurface;
	} else {
		LWARN("cairo::Surface", "Wrong surface format for getPexil");
		return 0;
	}

	int stride = cairo_format_stride_for_width ( CAIRO_FORMAT_ARGB32, width );
	unsigned char *data = cairo_image_surface_get_data(tmpSurface);
	util::DWORD *pixelptr;
	pixelptr = (util::DWORD *)(data + pos.y * stride); // row
	
	return pixelptr[pos.x];//pixel
}

void Surface::getPixelColorImpl( const Point &pos, Color &color ) {
	util::DWORD pixel = getPixel( pos );

	color.alpha = (unsigned char)((pixel & 0xFF000000) >> 24);
	color.r = (unsigned char)((pixel & 0x00FF0000) >> 16);
	color.g = (unsigned char)((pixel & 0x0000FF00) >> 8);
	color.b = (unsigned char)(pixel & 0x000000FF);

	if (color.alpha != 255) {
		if (color.alpha != 0) {
			double factor = 255.0/double(color.alpha);
			color.r = (unsigned char) boost::math::iround(color.r*factor);
			color.g = (unsigned char) boost::math::iround(color.g*factor);
			color.b = (unsigned char) boost::math::iround(color.b*factor);
		}
	}

}

void Surface::setPixelColorImpl( const Point &pos, const Color &color ) {
	cairo_rectangle ( _cr, pos.x, pos.y, 1, 1);
	impl::setColor( _cr, color );
	cairo_fill ( _cr );
	cairo_set_operator( _cr, _op );
}

bool Surface::saveAsImage( const std::string &file ) {
	return cairo_surface_write_to_png( _surface, file.c_str() ) == CAIRO_STATUS_SUCCESS;
}

void Surface::resizeImpl( const Size &size, bool scaleContent/*=false*/ ) {
	cairo_t *cr = NULL;
	cairo_surface_t *content = NULL;
	impl::create( content, cr, size.w, size.h, false );

	if (scaleContent) {
		Size actualSize = getSize();
		double fx = (double)size.w / (double)actualSize.w;
		double fy = (double)size.h / (double)actualSize.h;
		cairo_scale( cr, fx, fy );
		cairo_set_source_surface( cr, getContent(), 0, 0 );
		cairo_set_operator( cr, CAIRO_OPERATOR_SOURCE);
		cairo_paint( cr );
		setCompositionModeImpl(getCompositionMode());
	}
	impl::destroy( _surface, _cr );
	_surface = content;
	_cr = cr;
}

#define SNAKEWIPE_ROW_COUNT 10

void Surface::MaskCreate(bool transitionIn, void* transition, int stepCount, void* anim)
{
	if (!transition || stepCount < 1){
		return;
	}

	Size size = getSize();

	cairo_t* _borderCR = NULL;
	cairo_surface_t * _borderSurface = NULL;
	if (!impl::create(_borderSurface,_borderCR, size.w, size.h, true)){
		if (_borderCR){
			 cairo_destroy(_borderCR);
		}
		_borderCR=NULL;
		if (_borderSurface){
			cairo_surface_destroy(_borderSurface);
		}
		_borderSurface=NULL;
		return;
	}
	cairo_set_operator( _borderCR, CAIRO_OPERATOR_SOURCE);

	cairo_t* _maskcr = NULL;
	cairo_surface_t * _masksurface = NULL;
	if (!impl::create(_masksurface, _maskcr, size.w, size.h, false )) {
		if (_maskcr) {
			cairo_destroy(_maskcr);
		}
		_maskcr = NULL;
		if (_masksurface) {
			cairo_surface_destroy(_masksurface);
		}
		_masksurface = NULL;
		return;
	}

	double bgAlpha, fgAlpha;
	if (transitionIn) {
		bgAlpha = 0.0;
		fgAlpha = _alpha;
	} else {
		bgAlpha = _alpha;
		fgAlpha = 0.0;
	}

	cairo_set_operator(_maskcr, CAIRO_OPERATOR_SOURCE);

	cairo_set_source_rgba(_maskcr, 0.0, 0.0, 0.0, bgAlpha);
	cairo_rectangle(_maskcr, 0, 0, size.w, size.h);
	cairo_fill(_maskcr);

	cairo_set_source_rgba(_maskcr, 0.0, 0.0, 0.0, fgAlpha);
	_transition = transition;

	bptnt::Transition* tr = (bptnt::Transition*)_transition;
	_maskStep = (tr->endProgress - tr->startProgress) / stepCount;

	switch (tr->type) {
		case bptnt::Transition::TYPE_IRISWIPE: {
			if (tr->direction == bptnt::Transition::DIRECTION_REVERSE) {
				cairo_set_fill_rule(_maskcr, CAIRO_FILL_RULE_EVEN_ODD);
			}
			cairo_set_line_width( _borderCR, tr->borderWidth );
			break;
		}
		case bptnt::Transition::TYPE_CLOCKWIPE: {
			Size size = getSize();
			_maskA = atan2(size.w/tr->horzRepeat, size.h/tr->vertRepeat); /// angle from twelve
			break;
		}
	}
	(*_masks)[anim] = _maskcr;
	(*_masksurfs)[anim] = _masksurface;

	(*_borderCRs)[anim] = _borderCR;
	(*_BorderSurfs)[anim] = _borderSurface;

	(*_borderColors)[anim] = tr->borderColor;
	(*_borderIsTransIn)[anim] = transitionIn;

	curr_anim = anim;
}

bool Surface::HasMask(void* anim)
{
	cairo_t* _maskcr = (*_masks)[anim];
	return (_maskcr != NULL);
}

void Surface::MaskRemove(void* anim)
{
	
	cairo_t*         _borderCR = (*_borderCRs)[anim];
    cairo_surface_t* _borderSurface = (*_BorderSurfs)[anim];
	std::string _border_color = (*_borderColors)[anim];
	
	cairo_t* _maskcr = (*_masks)[anim];
	cairo_surface_t* _masksurface = (*_masksurfs)[anim];

	if (_maskcr) {
		_masks->erase( _masks->find( anim ) );
		_masksurfs->erase( _masksurfs->find( anim ) );
		cairo_destroy(_maskcr);
		_maskcr = NULL;
		cairo_surface_destroy(_masksurface);
		_masksurface = NULL;
		
		_borderCRs->erase(_borderCRs->find(anim));
		_BorderSurfs->erase(_BorderSurfs->find(anim));
		cairo_destroy(_borderCR);
		_borderCR = NULL;
		cairo_surface_destroy(_borderSurface);
		_borderSurface=NULL;
		_border_color.erase(_border_color.find((*_borderColors)[anim]));
		
		markDirty();
	}
}

/*
  top: 0; right: 1; bottom: 2; left: 3
*/
int MaskGetSide(double &angle, double &maskA, double &curW, double &curH, double &dx, double &dy)
{
	if ((angle < maskA) || (angle >= 2*M_PI - maskA)){
		dx = (curH/2)*tan(angle);
		dy = -curH/2;
		return 0;
	}
	if (angle < M_PI - maskA) {
		dx = curW/2;
		dy = (curW/2)*tan(angle-M_PI_2);
		return 1;
	}
	if (angle < M_PI + maskA) {
		dx = -(curH/2)*tan(angle);
		dy = curH/2;
		return 2;
	}
	dx = -curW/2;
	dy = -(curW/2)*tan(angle-M_PI_2);
	return 3;
}

void Surface::MaskStepTransition(int remainingSteps, void* anim)
{
	bptnt::Transition* tr = (bptnt::Transition*)_transition;
	cairo_t* _maskcr = (*_masks)[anim];
	cairo_t* _borderCR = (*_borderCRs)[anim];
 	canvas::Color color;	

	int countX = tr->horzRepeat;
	int countY = tr->vertRepeat;
	Size size = getSize();
	double posX;
	double posY = 0;
	double curW = size.w / countX;
	double curH = size.h / countY;

	double startVal, endVal;
	if ((tr->direction == bptnt::Transition::DIRECTION_FORWARD) ||
		(tr->type == bptnt::Transition::TYPE_SNAKEWIPE))
	{
		startVal = 0.0;
		endVal = tr->endProgress - (_maskStep * remainingSteps);
	} else {
		startVal = 1.0 - (tr->endProgress - _maskStep * remainingSteps);
		endVal = 1.0;
	}

	for (int i = 0; i < countY; i++) {
		posX = 0;
		for (int j = 0; j < countX; j++) {
			switch (tr->type) {
				case bptnt::Transition::TYPE_BARWIPE: {
					switch (tr->subtype) {
						case bptnt::Transition::SUBTYPE_BARWIPE_TOPTOBOTTOM: {
							cairo_rectangle(_maskcr, posX, posY + startVal*curH, curW, (endVal-startVal)*curH);
							if (tr->borderWidth >0){
								cairo_set_source_rgba (_borderCR, 0, 0, 0, 0);
								cairo_rectangle(_borderCR, posX,  posY + startVal*curH, curW, (endVal-startVal)*curH);
								cairo_fill(_borderCR);

								if (tr->borderColor.compare("blend") == 0){
									if (((*_borderIsTransIn)[anim])==true)
										cairo_set_source_surface (_borderCR ,getContent() , 0, 0);
									else
										cairo_set_source_rgba (_borderCR, 0, 0, 0, 0.2);
								}else{
									::canvas::color::get(  tr->borderColor.c_str(), color );
									impl::setColor( _borderCR, color );	
								}
								cairo_rectangle(_borderCR, posX,(posY + startVal*curH)+((endVal-startVal)*curH), curW ,tr->borderWidth);
								cairo_fill(_borderCR);
							}
							break;
						}
						case bptnt::Transition::SUBTYPE_BARWIPE_LEFTTORIGHT:
						default: {
							cairo_rectangle(_maskcr, posX + startVal*curW, posY, (endVal-startVal)*curW, curH);
							if (tr->borderWidth >0){
								cairo_set_source_rgba (_borderCR, 0, 0, 0, 0);
								cairo_rectangle(_borderCR, posX + startVal*curW, posY, (endVal-startVal)*curW, curH);
								cairo_fill(_borderCR);

								if (tr->borderColor.compare("blend") == 0){
									if (((*_borderIsTransIn)[anim])==true)
										cairo_set_source_surface (_borderCR ,getContent() , 0, 0);
									else
										cairo_set_source_rgba (_borderCR, 0, 0, 0, 0.2);
																			
								}else{
									::canvas::color::get(  tr->borderColor.c_str(), color );
									impl::setColor( _borderCR, color );	
								}
								cairo_rectangle(_borderCR, ((posX+startVal*curW)+(endVal-startVal)*curW), posY, tr->borderWidth, curH);
								cairo_fill(_borderCR);
							 //     cairo_set_source_surface (_borderCR ,getContent() , 0, 0);
							}
							break;
						}
					};
					cairo_fill(_maskcr);
					break;
				}

				case bptnt::Transition::TYPE_IRISWIPE: {
					double val;
					cairo_reset_clip(_maskcr);
					cairo_rectangle(_maskcr, posX, posY, curW, curH);
					cairo_clip(_maskcr);
					if (tr->direction == bptnt::Transition::DIRECTION_FORWARD) {
						val = endVal;
					} else {
						val = startVal;
						cairo_rectangle(_maskcr, posX, posY, curW, curH);
					}
					double w = curW * val;
					double h = curH * val;
					switch (tr->subtype) {
						case bptnt::Transition::SUBTYPE_IRISWIPE_DIAMOND: {
							cairo_move_to(_maskcr, posX+curW/2-w, posY+curH/2);
							cairo_rel_line_to(_maskcr, w, -h);
							cairo_rel_line_to(_maskcr, w, h);
							cairo_rel_line_to(_maskcr, -w, h);
							cairo_close_path(_maskcr);
							break;
						}
						case bptnt::Transition::SUBTYPE_IRISWIPE_RECTANGLE:
						default: {
							cairo_rectangle(_maskcr, posX+(curW-w)/2 , posY+(curH-h)/2, w, h);
							if (tr->borderWidth >0){
								cairo_set_source_rgba (_borderCR, 0, 0, 0, 0);
								cairo_rectangle(_borderCR, posX+(curW-w)/2 , posY+(curH-h)/2, w, h);
								cairo_fill(_borderCR);
								cairo_set_line_width( _borderCR, tr->borderWidth );				
								if ((curW - h)< tr->borderWidth) cairo_set_line_width( _borderCR, (curW - h) );
								if (tr->borderColor.compare("blend") == 0){
										if (((*_borderIsTransIn)[anim])==true)
										cairo_set_source_surface (_borderCR ,getContent() , 0, 0);
									else
										cairo_set_source_rgba (_borderCR, 0, 0, 0, 0.2);
										
								}else{
										::canvas::color::get(  tr->borderColor.c_str(), color );
										impl::setColor( _borderCR, color );
								}
								cairo_rectangle(_borderCR, posX+(curW-w)/2 , posY+(curH-h)/2, w, h);
								cairo_stroke(_borderCR);
							}//tr->broderWidth
							break;
						}
					};
					cairo_fill(_maskcr);
					break;
				} // IRISWIPE

				case bptnt::Transition::TYPE_CLOCKWIPE: {
					double d = 0;
					switch (tr->subtype) {
						case bptnt::Transition::SUBTYPE_CLOCKWIPE_CLOCKWISETHREE: {
							d = M_PI_2;
							break;
						}
						case bptnt::Transition::SUBTYPE_CLOCKWIPE_CLOCKWISESIX: {
							d = M_PI;
							break;
						}
						case bptnt::Transition::SUBTYPE_CLOCKWIPE_CLOCKWISENINE: {
							d = M_PI+M_PI_2;
							break;
						}
					};
					double startA = 2 * M_PI * startVal + d;
					if (startA >= 2 * M_PI) {
						startA -= 2 * M_PI;
					}
					double endA = 2 * M_PI * endVal + d;
					if (endA > 2 * M_PI) {
						endA -= 2 * M_PI;
					}

					double startDX, startDY, endDX, endDY;
					int startSide = MaskGetSide(startA, _maskA, curW, curH, startDX, startDY);
					int endSide = MaskGetSide(endA, _maskA, curW, curH, endDX, endDY);

					cairo_move_to(_maskcr, posX+curW/2, posY+curH/2);
					cairo_rel_line_to(_maskcr, startDX, startDY);

					if (endSide < startSide) {
						endSide += 4;
					} else if (startSide == endSide) {
						 switch (startSide) {
							 case 0: //	top
								 if (startDX > endDX)
									 endSide += 4;
								 break;
							 case 1: //	right
								 if (startDY > endDY)
									 endSide += 4;
								 break;
							 case 2: //	bottom
								 if (startDX < endDX)
									 endSide += 4;
								 break;
							 case 3: //	left
								 if (startDY < endDY)
									 endSide += 4;
								 break;
						 }
					}
					if (remainingSteps==0 && endSide==startSide){
						endSide+=4;
					}
					for (int k = startSide; k < endSide; k++) {
						 switch (k % 4) {
							 case 0: //	top
								cairo_line_to(_maskcr, posX+curW, posY);
								break;
							 case 1: //	right
								cairo_line_to(_maskcr, posX+curW, posY+curH);
								break;
							 case 2: //	bottom
								cairo_line_to(_maskcr, posX, posY+curH);
								break;
							 case 3: //	left
								cairo_line_to(_maskcr, posX, posY);
								break;
						 }
					}

					cairo_line_to(_maskcr, posX+curW/2+endDX, posY+curH/2+endDY);
					cairo_close_path(_maskcr);
					cairo_fill(_maskcr);
					break;
				} // CLOCKWIPE

				case bptnt::Transition::TYPE_SNAKEWIPE: {
					switch (tr->subtype) {
						case bptnt::Transition::SUBTYPE_SNAKEWIPE_TOPLEFTHORIZONTAL:
						default: {
							double val = endVal*SNAKEWIPE_ROW_COUNT;
							int row = val;
							double w = curW*(val-row);
							double dY = curH/SNAKEWIPE_ROW_COUNT;

							if (tr->direction == bptnt::Transition::DIRECTION_FORWARD) {
								if (row == 0) {
									cairo_rectangle(_maskcr, posX, posY, w, dY);
								} if (w == 0.0) {
									cairo_rectangle(_maskcr, posX, posY, curW, dY*row);
								} else {
									cairo_move_to(_maskcr, posX, posY);
									cairo_rel_line_to(_maskcr, curW, 0);
									if (row & 1) {
										cairo_rel_line_to(_maskcr, 0, dY*(row+1));
										cairo_rel_line_to(_maskcr, -w, 0);
										cairo_rel_line_to(_maskcr, 0, -dY);
										cairo_rel_line_to(_maskcr, w-curW, 0);
									} else {
										cairo_rel_line_to(_maskcr, 0, dY*row);
										cairo_rel_line_to(_maskcr, w-curW, 0);
										cairo_rel_line_to(_maskcr, 0, dY);
										cairo_rel_line_to(_maskcr, -w, 0);
									}
									cairo_close_path(_maskcr);
								}
							} else {
								if (row == 0) {
									cairo_rectangle(_maskcr, posX+curW-w, posY+curH-dY, w, dY);
								}
								if (w == 0.0) {
									double h = dY*row;
									cairo_rectangle(_maskcr, posX, posY+curH-h, curW, h);
								} else {
									cairo_move_to(_maskcr, posX+curW, posY+curH);
									cairo_rel_line_to(_maskcr, -curW, 0);
									if (row & 1) {
										cairo_rel_line_to(_maskcr, 0, -dY*(row+1));
										cairo_rel_line_to(_maskcr, w, 0);
										cairo_rel_line_to(_maskcr, 0, dY);
										cairo_rel_line_to(_maskcr, curW-w, 0);
									} else {
										cairo_rel_line_to(_maskcr, 0, -dY*row);
										cairo_rel_line_to(_maskcr, curW-w, 0);
										cairo_rel_line_to(_maskcr, 0, -dY);
										cairo_rel_line_to(_maskcr, w, 0);
									}
									cairo_close_path(_maskcr);
								}
							}
							cairo_fill(_maskcr);
							break;
						}
					}
					break;
				}
			}
			posX += curW;
		}
		posY += curH;
	}

	markDirty();
	canvas()->flush(true);
}

canvas::Surface *Surface::transformWithChromakeyImpl( const std::string &chromakey ) {
	canvas::Color color;
	if (!canvas::color::get( chromakey.c_str(), color )) {
		LWARN("cairo::Surface", "Wrong chromakey color");
		return NULL;
	}

	int width = 0;
	int height = 0;
	cairo_surface_t *originSurface = getContent();
	if (cairo_surface_get_type(originSurface) == CAIRO_SURFACE_TYPE_IMAGE) {
		width = cairo_image_surface_get_width(originSurface);
		height = cairo_image_surface_get_height(originSurface);
	} else
#if defined(GL)
	if (cairo_surface_get_type(_surface) == CAIRO_SURFACE_TYPE_GL) {
		width = cairo_gl_surface_get_width(_surface);
		height = cairo_gl_surface_get_height(_surface);
	} else
#endif
	{
		LWARN("cairo::Surface", "Wrong surface type for chromakey");
		return NULL;
	}

	cairo_surface_t *tmpSurface = NULL;
	canvas::cairo::Surface *newSurface = NULL;
	cairo_format_t format = cairo_image_surface_get_format(originSurface);
	if (format == CAIRO_FORMAT_RGB24
#if defined(GL)
		 || cairo_surface_get_type(_surface) == CAIRO_SURFACE_TYPE_GL
#endif
		) {
		canvas()->setARGB(true);
		newSurface = static_cast<canvas::cairo::Surface*>(canvas()->createSurface( Rect( 0, 0, width, height )));
		cairo_t *cr_temp = newSurface->getContext();
		cairo_set_operator( cr_temp, CAIRO_OPERATOR_SOURCE );
		cairo_rectangle ( cr_temp, 0,  0,  width, height );
		cairo_set_source_surface( cr_temp, originSurface, 0, 0 );
		cairo_clip ( cr_temp );
		cairo_paint( cr_temp );
		cairo_reset_clip (cr_temp);
		tmpSurface = newSurface->getContent();
		assert( cairo_image_surface_get_format(tmpSurface) == CAIRO_FORMAT_ARGB32 );
	} else if (format == CAIRO_FORMAT_ARGB32) {
		tmpSurface = originSurface;
	} else {
		LWARN("cairo::Surface", "Wrong surface format for chromakey");
		return NULL;
	}

	register guchar *bptr;
	unsigned i = 0;
	guint stride = cairo_format_stride_for_width ( CAIRO_FORMAT_ARGB32, width );
	guchar *cairo_pixels = (guchar*)cairo_image_surface_get_data(tmpSurface);
	bptr = cairo_pixels;

	while (bptr < (cairo_pixels + stride * height)) {
		for (i = 0; i < stride ; i+=4) {
			bool check = bptr[i] <= std::min(color.r + Surface::CHROMAKEY_THRESHOLD, 255) && bptr[i] >= std::max(color.r - Surface::CHROMAKEY_THRESHOLD, 0);
			check &= bptr[i+1] <= std::min(color.g + Surface::CHROMAKEY_THRESHOLD, 255) && bptr[i+1] >= std::max(color.g - Surface::CHROMAKEY_THRESHOLD, 0);
			check &= bptr[i+2] <= std::min(color.b + Surface::CHROMAKEY_THRESHOLD, 255) && bptr[i+2] >= std::max(color.b - Surface::CHROMAKEY_THRESHOLD, 0);
			if (check) {
				bptr[i] = 0;
				bptr[i + 1] = 0;
				bptr[i + 2] = 0;
				bptr[i + 3] = 0;
			}
		}
		bptr += stride;
	}

	return newSurface ? newSurface : NULL;
}

}
}
