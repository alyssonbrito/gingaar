#include "mngviewer.h"
#include "../../size.h"
#include "../../system.h"
#include <util/log.h>
#include <gtk/gtk.h>
#include <cairo/cairo.h>
#include <boost/bind.hpp>
#include <boost/math/special_functions/round.hpp>
#include <exception>
#include <stdexcept>
#include <assert.h>
#include <gdk/gdkkeysyms.h>
#include "../cairo/surface.h"
#include <util/cfg/configregistrator.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <fcntl.h>
#if defined(GL)
#include <gdk/gdkzen.h>
#include <cairo-gl-ext.h>
#endif

#define BLOCK_SIZE  4096

namespace canvas {
namespace gtk {

static guint read_file ( const gchar *file_name, guchar **ptr ) {
	gint fd;
	guint size = 0;
	guint bytes_read = 0;

	if ((fd = open (file_name, O_RDONLY)) == -1) {
		perror (file_name);
		* ptr = NULL;
		return 0;
	}

	* ptr = g_new (guchar, BLOCK_SIZE);
	while ((bytes_read = read (fd, * ptr + size, BLOCK_SIZE))) {
		size += bytes_read;
		* ptr = (guchar *) g_realloc (* ptr, size + BLOCK_SIZE);
	}
	close (fd);

	* ptr = (guchar *) g_realloc (* ptr, size);
	return size;
}


/* MNG callbacks */
static mng_ptr mng_malloc_callback( mng_size_t how_many ) {
	return (mng_ptr) g_new0 (gchar, how_many);
}

static void mng_free_callback( mng_ptr pointer, mng_size_t number ) {
	UNUSED(number);
	g_free (pointer);
}

static mng_bool mng_open_stream_callback( mng_handle mng_h ) {
	UNUSED(mng_h);
	return MNG_TRUE;
}

static mng_bool mng_close_stream_callback( mng_handle mng_h ) {
	UNUSED(mng_h);
	return MNG_TRUE;
}

static mng_bool mngerror(mng_handle mng, mng_int32 code, mng_int8 severity,
	mng_chunkid chunktype, mng_uint32 chunkseq,
	mng_int32 extra1, mng_int32 extra2, mng_pchar text) {
	UNUSED(mng);
	UNUSED(code);
	UNUSED(severity);
	UNUSED(chunktype);
	UNUSED(chunkseq);
	UNUSED(extra1);
	UNUSED(extra2);
	fprintf(stderr, "%s\n", text);
	return 0;
}


static mng_bool mng_read_data_callback( mng_handle mng_h, mng_ptr buffer, mng_uint32 bytes_requested, mng_uint32 * bytes_read ) {
	guint available_mng_food;
	MngData * mng_view = (MngData *)mng_get_userdata (mng_h);
	available_mng_food = mng_view->bytes_to_eat - mng_view->bytes_eaten;
	if (available_mng_food > 0 && mng_view->mng_food != NULL) {
		* bytes_read = (mng_uint32) MIN ((mng_uint32) available_mng_food, bytes_requested);
		memcpy (buffer, mng_view->mng_food + mng_view->bytes_eaten, * bytes_read);
		mng_view->bytes_eaten += * bytes_read;
		return MNG_TRUE;
	} else {
		return MNG_FALSE;
	}
}

static mng_bool mng_process_header_callback( mng_handle mng_h, mng_uint32 width, mng_uint32 height ) {
	MngData * mng_view;

	mng_view = (MngData *)mng_get_userdata (mng_h);

	mng_view->width = width;
	mng_view->height = height;

	if (mng_view->MNG_drawing_buffer) {
		g_free (mng_view->MNG_drawing_buffer);
	}
	mng_view->MNG_drawing_buffer = g_new0 (guchar, 3 * width * height);

	if (mng_view->MNG_alpha_buffer) {
		g_free (mng_view->MNG_alpha_buffer);
	}
	mng_view->MNG_alpha_buffer = g_new0 (guchar, width * height);
	return MNG_TRUE;
}

static gboolean gtk_mng_view_animator( MngData *mng_view ) {
	mng_retcode retcode;
	retcode = mng_display_resume (mng_view->MNG_handle);

	if (retcode == MNG_NOERROR) {
		mng_view->timeout_ID = 0;
		return FALSE;
	} else if (retcode == MNG_NEEDTIMERWAIT) {
		return FALSE;
	} else {
		g_warning ("mng_display_resume() error");
	}

	return FALSE;
}

static mng_bool mng_set_timer_callback( mng_handle mng_h, mng_uint32 delay ) {
	MngData * mng_view;
	mng_view = (MngData *)mng_get_userdata (mng_h);
	int fnum = *mng_view->frame_number;
	if (fnum < mng_view->first) {
		delay = 0;
	}
	mng_view->timeout_ID = g_timeout_add( delay, (GSourceFunc) gtk_mng_view_animator, (gpointer)mng_view );
	return MNG_TRUE;
}

static mng_uint32 mng_get_tickcount_callback( mng_handle mng_h ) {
	gdouble seconds;
	gulong microseconds;

	MngData * mng_view;

	mng_view = (MngData *)mng_get_userdata (mng_h);
	seconds = g_timer_elapsed ( mng_view->timer, &microseconds );

	return ((mng_uint32) (seconds*1000.0 + ((gdouble) microseconds)/1000.0));
}

static mng_ptr mng_get_canvas_line_callback( mng_handle mng_h, mng_uint32 line ) {
	MngData * mng_view;

	mng_view = (MngData *)mng_get_userdata (mng_h);
	return mng_view->MNG_drawing_buffer + 3 * line * mng_view->width;
}

static mng_ptr mng_get_alpha_line_callback( mng_handle mng_h, mng_uint32 line ) {
	MngData * mng_view;
	mng_view = (MngData *)  (mng_get_userdata (mng_h));
	return mng_view->MNG_alpha_buffer + line * mng_view->width;
}

static void gtk_mng_view_paint( MngData *mng_view, GdkRectangle *area ) {
	guint stride;
	guchar * buffer;
	register guchar * ptr;
	register guchar * aptr;
	register guchar * bptr;

	if (!mng_view->surface) {
		return;
	}

	unsigned i = 0, j = 0, k = 0;
	stride = cairo_format_stride_for_width ( CAIRO_FORMAT_ARGB32, area->width );
	buffer = (guchar *)malloc( stride * area->height );

	bptr = buffer;
	ptr = mng_view->MNG_drawing_buffer + 3 * (area->y * mng_view->width + area->x);
	aptr = mng_view->MNG_alpha_buffer + (area->y * mng_view->width + area->x);

	while (bptr < (buffer + stride * area->height)) {
		// Copying with alpha
		for (i = 0, j = 0, k = 0; i < stride ; i+=4) {
			bptr[i] = ptr[j + 2];
			bptr[i+1] = ptr[j + 1];
			bptr[i+2] = ptr[j];
			bptr[i+3] = aptr[k];
			j+=3;
			k++;
		}
		bptr += stride;
		ptr += 3 * mng_view->width;
		aptr += mng_view->width;
	}

	canvas::cairo::Surface* destSurface = (canvas::cairo::Surface*)mng_view->surface;
	if (!destSurface) {
		free (buffer);
		return;
	}

#if defined(GL)
	cairo_surface_t * surface = cairo_gl_surface_create_for_data( reinterpret_cast<unsigned char*>(buffer),
		gdk_get_egl_device(),
		CAIRO_FORMAT_ARGB32,
		area->width, area->height, stride );
#else
	cairo_surface_t *surface = cairo_image_surface_create_for_data( buffer, CAIRO_FORMAT_ARGB32, area->width, area->height, stride );
#endif

	int fnum = *mng_view->frame_number;
	fnum++;
	*mng_view->frame_number = fnum;

	if (mng_view->viewer) {
		MngViewer* viewer = mng_view->viewer;
		if (!viewer->getSurface()) {
			viewer->createSurface( mng_view->width, mng_view->height );
		}

		canvas::cairo::Surface* tmpSurface = static_cast<canvas::cairo::Surface*>(viewer->getSurface());
		cairo_t *cr_temp = tmpSurface->getContext();
		cairo_set_operator( cr_temp, CAIRO_OPERATOR_OVER );
		cairo_rectangle ( cr_temp, area->x,  area->y,  area->width, area->height );
		cairo_set_source_surface( cr_temp, surface, area->x, area->y );
		cairo_clip ( cr_temp );
		cairo_paint( cr_temp );
		cairo_reset_clip (cr_temp);

		if (fnum >= mng_view->first && (!mng_view->last || (mng_view->last && fnum <= mng_view->last))) {
			canvas::Rect t;//	target
			canvas::Rect s;//	source
			viewer->getScalingSizes( t, s );

			if (!destSurface->scale( t, tmpSurface, s )) {
					LWARN("MnvViewer", "cannot scale image");
			}

			mng_view->surface->markDirty();
			mng_view->surface->flush();
		}
	}

	cairo_surface_destroy( surface );
	free (buffer);

	if (mng_view->sys && mng_view->stop_player && mng_view->last && fnum > mng_view->last) {
		mng_view->sys->enqueue(mng_view->stop_player);
	}
}

static mng_bool mng_refresh_callback( mng_handle mng_h, mng_uint32 x, mng_uint32 y, mng_uint32 width, mng_uint32 height ) {
	MngData * mng_view;
	mng_view = (MngData *) (mng_get_userdata (mng_h));

	GdkRectangle rectangle;
	rectangle.x = x;
	rectangle.y = y;
	rectangle.width = width;
	rectangle.height = height;
	gtk_mng_view_paint (mng_view, &rectangle);

	return MNG_TRUE;
}

MngViewer::MngViewer( System *sys, Surface *surface )
	: canvas::MngViewer(surface), _sys(sys)
{
	_mngView = NULL;
	_surface = NULL;
	_frameNumber = 0;
}

MngViewer::~MngViewer( void )
{
	if (_mngView) {
		stopAnim();
	}
}

void MngViewer::onResized() {
	LINFO("MngViewer", "onResized");
}

void MngViewer::stopAnim() {
	LINFO("MngViewer", "Stop animation");
	if (_mngView->MNG_drawing_buffer) {
		g_free (_mngView->MNG_drawing_buffer);
		_mngView->MNG_drawing_buffer = NULL;
	}

	if (_mngView->MNG_alpha_buffer) {
		g_free (_mngView->MNG_alpha_buffer);
		_mngView->MNG_alpha_buffer = NULL;
	}

	if (_mngView->MNG_handle) {
		mng_cleanup (&_mngView->MNG_handle);
		_mngView->MNG_handle = NULL;
	}

	if (_mngView->timeout_ID) {
		g_source_remove(_mngView->timeout_ID);
		_mngView->timeout_ID = 0;
		g_timer_stop (_mngView->timer);
		g_timer_destroy(_mngView->timer);
	}

	if (_surface) {
		_sys->canvas()->destroy( _surface );
		_surface = NULL;
	}

	free(_mngView);
	_mngView = NULL;

	_frameNumber = 0;

}

gboolean MngViewer::initLibMng( MngData * mng_view ) {

	if (mng_view->MNG_handle) {
		mng_cleanup (&mng_view->MNG_handle);
	}

	mng_view->MNG_handle = mng_initialize (mng_view,
					 mng_malloc_callback,
					 mng_free_callback,
					 MNG_NULL);

	if (mng_view->MNG_handle == MNG_NULL) {
		return FALSE;
	}

	if (mng_setcb_openstream (mng_view->MNG_handle, mng_open_stream_callback) != MNG_NOERROR ||
		mng_setcb_closestream (mng_view->MNG_handle, mng_close_stream_callback) != MNG_NOERROR ||
		mng_setcb_errorproc (mng_view->MNG_handle, mngerror) != MNG_NOERROR ||
		mng_setcb_readdata (mng_view->MNG_handle, mng_read_data_callback) != MNG_NOERROR ||
		mng_setcb_processheader (mng_view->MNG_handle, mng_process_header_callback) != MNG_NOERROR ||
		mng_setcb_settimer (mng_view->MNG_handle, mng_set_timer_callback) != MNG_NOERROR ||
		mng_setcb_gettickcount (mng_view->MNG_handle, mng_get_tickcount_callback) != MNG_NOERROR ||
		mng_setcb_getcanvasline (mng_view->MNG_handle, mng_get_canvas_line_callback) != MNG_NOERROR ||
		mng_setcb_getalphaline (mng_view->MNG_handle, mng_get_alpha_line_callback) != MNG_NOERROR ||
		mng_setcb_refresh (mng_view->MNG_handle, mng_refresh_callback) != MNG_NOERROR) {
		mng_cleanup (&mng_view->MNG_handle);
		return FALSE;
	}


	if(mng_get_alphadepth(mng_view->MNG_handle)) {
		mng_set_canvasstyle (mng_view->MNG_handle, MNG_CANVAS_RGB8_A8);
	} else {
		mng_set_canvasstyle (mng_view->MNG_handle, MNG_CANVAS_RGB8);
	}

	return TRUE;
}

gboolean MngViewer::loadMngFromMemory( MngData *mng_view, guchar *data_to_eat, guint data_size ) {
	g_return_val_if_fail (data_size > 27, FALSE);
	g_return_val_if_fail (data_to_eat != NULL, FALSE);

	if (data_to_eat[0] != 0x8a ||
			data_to_eat[1] != 'M' ||
			data_to_eat[2] != 'N' ||
			data_to_eat[3] != 'G' ||
			data_to_eat[4] != 0x0d ||
			data_to_eat[5] != 0x0a ||
			data_to_eat[6] != 0x1a ||
			data_to_eat[7] != 0x0a) {
		g_warning ("not mng format");
		return FALSE;
	}

	if (initLibMng( mng_view )) {
		mng_view->bytes_to_eat = data_size;
		mng_view->bytes_eaten = 0;
		mng_view->mng_food = data_to_eat;

		if (mng_read (mng_view->MNG_handle) != MNG_NOERROR) {
			g_warning ("libmng read error");
			mng_cleanup (&mng_view->MNG_handle);
			return FALSE;
		} else {
			return mng_display (mng_view->MNG_handle);
		}
	} else {
		g_warning ("error initializing libmng");
		return FALSE;
	}
	return TRUE;
}

bool MngViewer::load( const std::string &file, const std::string &fitMode ) {
	if (_mngView) {
		stopAnim();
	}

	_fitMode = fitMode;
	const Size &s = surface()->getSize();

	LINFO("MngViewer", "Load page: url=%s, width=%d, height=%d",
		file.c_str(), s.w, s.h);

	guchar * mng_data = NULL;
	guint mng_data_size;

	mng_data_size = read_file (file.c_str(), &mng_data);

	if (mng_data == NULL) {
		return false;
	}

	_mngView = (MngData *) calloc(1, sizeof(*_mngView));
	if (!_mngView) {
		LERROR("MngViewer", "Memory allocation error");
		return false;
	}

	_mngView->MNG_handle = NULL;
	_mngView->MNG_drawing_buffer = NULL;
	_mngView->MNG_alpha_buffer = NULL;
	_mngView->timeout_ID = 0;
	_mngView->timer = g_timer_new ();
	g_timer_start (_mngView->timer);
	_mngView->mng_food = NULL;
	_mngView->surface = this->surface();
	_mngView->viewer = this;
	_mngView->width = 0;
	_mngView->height = 0;
	_mngView->first = _first;
	_mngView->last = _last;
	_mngView->stop_player = _stopPlayer;
	_mngView->sys = _sys;
	_mngView->frame_number = &_frameNumber;

	loadMngFromMemory ( _mngView, mng_data, mng_data_size );

	g_free (mng_data);
	return true;
}

void MngViewer::stop() {
	if (_mngView) {
		stopAnim();
	}
}

void MngViewer::draw() {
	LINFO("MngViewer", "Draw");

	//	Set background color
	surface()->setColor( surface()->getBackgroundColor() );
	surface()->fillRect( canvas::Rect(0, 0, surface()->getSize().w, surface()->getSize().h) );

	canvas::cairo::Surface* tmpSurface = static_cast<canvas::cairo::Surface*>(getSurface());
	if (!tmpSurface) {
		LWARN("MngViewer", "cannot get inner surface");
		return;
	}

	canvas::Rect t;	//target
	canvas::Rect s;	//source
	if (!getScalingSizes( t, s )) {
		LDEBUG("MngViewer", "can't get scaling sizes");
		return;
	}

	if (_frameNumber < _first || (_last && _frameNumber > _last)) {
		LDEBUG("MngViewer", "frames out of range");
		return;
	}

	if (!surface()->scale( t, tmpSurface, s )) {
		LWARN("MnvViewer", "cannot scale image");
	}

	surface()->markDirty();
	surface()->flush();
}

extern GdkEventKey *g_curKeyEvent;

void MngViewer::dispatchKey(util::key::type key, bool isUp)
{
	UNUSED(key);
	UNUSED(isUp);
}

void  MngViewer::createSurface( int width, int height ) {
	_surface = _sys->canvas()->createSurface( Size( width, height ) );
}

bool MngViewer::getScalingSizes( canvas::Rect &t /*target*/, canvas::Rect &s /*source*/ ) {
	canvas::Size s_size = surface()->getSize();
	canvas::Size i_size( _mngView->width, _mngView->height );

	if (_mngView->width == 0 || _mngView->height == 0) {
		return false;
	}

	std::string mode = getFitMode();
	if (mode == "fill") {
			t = canvas::Rect(0, 0, s_size.w, s_size.h);
			s = canvas::Rect(0, 0, i_size.w, i_size.h);
	} else if (mode == "meet") {
		double fw = double(s_size.w)/double(i_size.w);
		double fh = double(s_size.h)/double(i_size.h);
		double factor = std::min( fw, fh );
		t = canvas::Rect(0, 0, boost::math::iround(i_size.w*factor), boost::math::iround(i_size.h*factor));
		s = canvas::Rect(0, 0, i_size.w, i_size.h);
	} else if (mode == "meetBest") {
		double fw = double(s_size.w)/double(i_size.w);
		double fh = double(s_size.h)/double(i_size.h);
		double factor = std::min( fw, fh );
		if ( factor > 2.0 ) {
			factor = 2.0;
		}
		t = canvas::Rect(0, 0, boost::math::iround(i_size.w*factor), boost::math::iround(i_size.h*factor));
		s = canvas::Rect(0, 0, i_size.w, i_size.h);
	} else if (mode == "slice") {
		double fw = double(s_size.w)/double(i_size.w);
		double fh = double(s_size.h)/double(i_size.h);
		double factor = std::max( fw, fh );
		t = canvas::Rect(
			0,
			0,
			std::min(boost::math::iround(i_size.w*factor),s_size.w),
			std::min(boost::math::iround(i_size.h*factor),s_size.h));
		s = canvas::Rect(0, 0, boost::math::iround(t.w/factor), boost::math::iround(t.h/factor));
	} else {
		t = s = canvas::Rect(0, 0, std::min(s_size.w, i_size.w), std::min(s_size.h, i_size.h));
	}
	return true;
}

}
}
