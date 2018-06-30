#pragma once
#include "../../surface.h"
#include <gtk/gtk.h>
#include <pango/pangocairo.h>
#include <map>
#include <glib-object.h>
#include <cairo/cairo.h>
#include <cairo/cairo-ft.h>
#include <pango/pango.h>
#include <pango/pangoft2.h>




using namespace std;

typedef struct _cairo cairo_t;
typedef struct _cairo_surface cairo_surface_t;
typedef struct _GdkPixbuf GdkPixbuf;
typedef enum _cairo_operator cairo_operator_t;


namespace canvas {
namespace cairo {

class Canvas;

class DefaultFontData {
public:
	DefaultFontData( PangoFont *font, PangoLayout *layout ) : _font(font), _layout(layout), _refCounter(0) {}
	virtual ~DefaultFontData() { g_object_unref( _font ); g_object_unref( _layout ); }

	void ref() { _refCounter++; }
	void deref() { _refCounter--; }
	int refCount() const { return _refCounter; }
	PangoFont *font() { return _font; };
	PangoLayout *layout() { return _layout; };
private:
	PangoFont *_font;
	PangoLayout *_layout;
	int _refCounter;
};

class Surface : public canvas::Surface {
public:
	Surface( Canvas *canvas, ImageData *img );	
	Surface( Canvas *canvas, const Rect &rect );
	Surface( Canvas *canvas, const std::string &file );
	virtual ~Surface();

	virtual canvas::Canvas *canvas() const;

	virtual Size getSize() const;

	virtual bool getClip( Rect &rect );

	virtual int fontAscent();
	virtual int fontDescent();

	virtual bool setOpacity( util::BYTE alpha );
	virtual util::BYTE getOpacity() const;

	float alpha() const;

	virtual bool saveAsImage( const std::string &file );
	cairo_surface_t *getContent();

	virtual bool blitEx(const Point &targetPoint, canvas::Surface *srcSurface, const Rect &sourceRect, bool withAlpha = true, bool saveClip = false);
	cairo_t *getContext();
	GdkWindow *getWebkitWindow() const;
	void setWebkitWindow( GdkWindow *window );

	virtual void MaskCreate(bool transitionIn, void* transition, int stepCount, void* anim);
	virtual bool HasMask(void* anim);
	virtual void MaskRemove(void* anim);
	virtual void MaskStepTransition(int remainingSteps, void* anim);
	typedef map<void*, cairo_t*> Masks;
	typedef map<void*, cairo_surface_t*> Masksurfs;
	typedef map<void*, cairo_t*> BorderCRs;
    typedef map<void*, cairo_surface_t*> BorderSurfs;
	typedef map<void*,  std::string> BorderColors;
	typedef map<void*,  bool> BorderIsTransIn;

   
    BorderCRs* _borderCRs;
    BorderSurfs* _BorderSurfs;
	BorderColors* _borderColors;
	BorderIsTransIn* _borderIsTransIn;

	Masks* _masks;
	Masksurfs* _masksurfs;
	void* curr_anim;

	void* _transition;
	double _maskStep;
	double _maskA;

	cairo_t *GetMask();
	cairo_surface_t *GetMaskSurf();

	cairo_t *GetBorder();
        cairo_surface_t *GetBorderSurf();
	std::string  getBorderColor();



protected:
	//	Aux fonts
	void freeFont();
	PangoFont *loadFont();
	PangoFont *currentFont();
	void loadLayout();
	PangoLayout *currentLayout();

	virtual bool applyFont();
	virtual std::string installFontFile(const std::string &fontfile);
	void init();
	virtual void setClipImpl( const Rect &rect );

	virtual void setCompositionModeImpl( composition::mode mode );

	virtual void drawLineImpl( int x1, int y1, int x2, int y2 );

	virtual void drawRectImpl( const Rect &rect );
	virtual void fillRectImpl( const Rect &rect );

	virtual void drawRoundRectImpl( const Rect &rect, int arcW, int arcH );
	virtual void fillRoundRectImpl( const Rect &rect, int arcW, int arcH );

	// DrawPolygon helper
	void drawLineTo( const Point &lp, const Point &np );
	void drawRelLineTo( const Point &lp, const Point &np );

	virtual void drawPolygonImpl( const std::vector<Point>& vertices, bool closed );
	virtual void fillPolygonImpl( const std::vector<Point>& vertices );

	virtual void drawEllipseImpl( const Point &center, int rw, int rh, int angStart, int angStop );
	virtual void fillEllipseImpl( const Point &center, int rw, int rh, int angStart, int angStop );
    
	virtual void blitImpl( const Point &target, canvas::Surface *srcSurface, const Rect &source );
	virtual void scaleImpl( const Rect &targetRect, canvas::Surface *srcSurface, const Rect &sourceRect, bool flipw=false, bool fliph=false );
	virtual void resizeImpl( const Size &size, bool scaleContent=false );

	virtual canvas::Surface *rotateImpl( int degrees );

	virtual util::DWORD getPixel( const Point &pos );
	virtual void getPixelColorImpl( const Point &pos, Color &color );
	virtual void setPixelColorImpl( const Point &pos, const Color &color );

	virtual void drawTextImpl( const Point &pos, const std::string &text, int ascent );
	virtual void drawTextImpl( const Rect &rect, const std::string &text, Alignment alignment=alignment::None, int spacing=0 );
	virtual void measureTextImpl( const std::string &text, Size &size );

	static const int CHROMAKEY_THRESHOLD = 20;
	virtual canvas::Surface *transformWithChromakeyImpl( const std::string &chromakey );

	static DefaultFontData *_pangoDefaultFont;
private:
	Canvas *_canvas;
	cairo_t *_cr;
	cairo_surface_t *_surface;
	cairo_operator_t _op;
	float _alpha;
	PangoFont *_pangoFont;
	PangoLayout* _pangoLayout;
	GdkWindow *_webkitwindow;
};

}
}
	
