#include "canvas.h"
#include "surfacewrapper.h"
#include "../lua.h"
#include "../../luaplayer.h"
#include <util/mcr.h>
#include <util/log.h>
#include <canvas/system.h>
#include <canvas/canvas.h>
#include <canvas/surface.h>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/math/special_functions/round.hpp>
#include <string>
#include <string.h>
#include <assert.h>

#include <ginga_api.h>

// LGESP: Change Request
// Description: There's a compilation error when I invoke "player::Device".
// method. For example:
//
//      _player->device()->onFetchFile(absPath.string());
//
// The workaround is to include the header file where "player::Device" lives.
#include "../../../device.h"
// end of LGESP


#define LUA_CANVAS  "lua_canvas_module"
#define CANVAS_TYPE "lua.canvas"

namespace player {
namespace mcanvas {

namespace fs = boost::filesystem;

//	Aux implementation for a l_drawPolygon function
namespace impl {
	struct PolygonDrawer {
		PolygonDrawer( const std::string &mode, SurfaceWrapper *wrapper) : _mode(mode), _surface(wrapper->surface()) {}
		~PolygonDrawer() {}

		void addPoint( const canvas::Point &point) {
			_points.push_back(point);
		}

		int draw( lua_State *L ) const {
			assert(_surface);
			int res = 0;
			if (!_points.empty()) {
				if (_mode == "close") {
					_surface->drawPolygon(_points);
				} else if (_mode == "fill") {
					_surface->fillPolygon(_points);
				} else {
					_surface->drawPolygon(_points, false);
				}
			} else {
				res = luaL_error(L, "The polygon should have vertices.");
			}
			return res;
		}

		const std::string _mode;
		std::vector<canvas::Point> _points;
		::canvas::Surface *_surface;
		
		private:
			PolygonDrawer operator=( const PolygonDrawer &/*drawer*/ ) { return *this; }
	};
}

static void setDefaultFont(canvas::Surface *s){
	APRINT_LUA("<==>");
	bool sty  = false;
	bool bold = false;
	bool variant = false; //variant = "normal" | "small-caps"
	::canvas::Font font( "Tiresias", 16, bold, sty, variant );
	if (!s->setFont( font )){
		LDEBUG("[ERROR]", "Error setting default font\n");
	}
}

//	Aux
static inline canvas::Canvas *getCanvas( SurfaceWrapper *wrapper ) {
	return wrapper->surface()->canvas();
}

static int pushSize( lua_State *L, const canvas::Size &size ) {
	lua_pushnumber(L, size.w );
	lua_pushnumber(L, size.h );
	return 2;
}

static int pushRect( lua_State *L, const canvas::Rect &rect ) {
	lua_pushnumber( L, rect.x );
	lua_pushnumber( L, rect.y );
	lua_pushnumber( L, rect.w );
	lua_pushnumber( L, rect.h );
	return 4;
}

static void checkRect( lua_State *L, int pos, canvas::Rect &rect ) {
	rect.x = luaL_checkint(L, pos++);
	rect.y = luaL_checkint(L, pos++);
	rect.w = luaL_checkint(L, pos++);
	rect.h = luaL_checkint(L, pos++);
}

static int pushColor( lua_State *L, const canvas::Color &color ) {
	lua_pushnumber(L, color.r);
	lua_pushnumber(L, color.g);
	lua_pushnumber(L, color.b);
	lua_pushnumber(L, color.alpha);
	return 4;
}

static void checkColor( lua_State *L, int pos, canvas::Color &color ) {
	color.r = (util::BYTE) luaL_checknumber(L, pos++),
	color.g = (util::BYTE) luaL_checknumber(L, pos++),
	color.b = (util::BYTE) luaL_checknumber(L, pos++),
	color.alpha = (util::BYTE) luaL_checknumber(L, pos++);
}

static void checkPoint( lua_State *L, int pos, canvas::Point &point ) {
	point.x = luaL_checkint(L, pos++);
	point.y = luaL_checkint(L, pos++);
}

static SurfaceWrapper *checkSurface( lua_State *L, int pos=1 ) {
	void *ud = luaL_checkudata( L, pos, CANVAS_TYPE );
	luaL_argcheck(L, ud != NULL, 1, "'canvas' expected" );
	return *((SurfaceWrapperPtr *)ud);
}

static void buildLuaFontStyle( std::string &style, bool bold, bool italics ) {
	if (bold) {
		if (italics) {
			style = "bold-italic";
		} else {
			style = "bold";
		}
	} else {
		if (italics) {
			style = "italic";
		} else {
			style = "normal";
		}
	}
}

//	API
static int l_new( lua_State *L ) {
  APRINT_LUA("==>");
	//	Parameters: canvas [string | (width,height)] -> canvas
	SurfaceWrapper *wrapper=checkSurface(L);
	canvas::Surface *surface;
	
	//	Get canvas module from stack
	Module *module = Module::get( L );
	if (!module) {
    APRINT_LUA("<== ERROR");
		return luaL_error( L, "Invalid canvas module\n" );
	}

	int type = lua_type(L, 2);
	switch (type) {
		case LUA_TSTRING: {
			const char *img = luaL_checkstring(L, 2);
			if (!img) {
        APRINT_LUA("<== ERROR");
				return luaL_error(L, "The path of an image must be a string\n");
			}

			std::string path = module->getFilePath( img );
			if (!fs::exists( path )) {
				//	LG Electronics: #4298
				//	Path what we check here is path related to LUA module path 'nclroot/lua'. But some
				//	content keep paths related to 'nclroot'. Let's try to find image relatively to current module path
				//	before give up.
				//	DamasTV is a content example
				fs::path originModulePath = module->getOriginPath();
				originModulePath /= std::string(img);
				path = originModulePath.string();

				// LGESP: dsmcc hook
				// TODO: replace this direct call by calling Device->onFetchFile
				onFetchFilesFromCarousel(path);

				if (!fs::exists( path )) {
          APRINT_LUA("<== ERROR");
					return luaL_error(L, "The image does not exists\n");
				}
			}

			surface=getCanvas(wrapper)->createSurfaceFromPath( path );

			break;
		}
		case LUA_TNUMBER: {
			canvas::Rect bounds( 0, 0, luaL_checkint(L, 2), luaL_checkint(L, 3) );
			if (bounds.w <= 0 || bounds.h <= 0) {
        APRINT_LUA("<== ERROR");
				return luaL_error(L, "Invalid size of canvas\n");
			}
			surface=getCanvas(wrapper)->createSurface( bounds );
			break;
		}
		default: {
			surface=NULL;
			break;
		}
	};

	//	The below code will run the GC for GC_MILLI_SECONDS ms.
	lua_gc(L, LUA_GCSETSTEPMUL, Module::GC_MILLI_SECONDS);
	lua_gc(L, LUA_GCSTEP, 0);

	if (surface!=NULL) {
    APRINT_LUA("<== <-- createSurface");
		return module->createSurface(L, surface);
	} else {
    APRINT_LUA("<== ERROR");
		return luaL_error(L, "The parameter to create a new canvas must be the width and height of this or the path of an image\n");
	}
}

static int l_attrSize( lua_State *L ) {
	APRINT_LUA("<==>");
	//	Parameters: canvas -> width height
	SurfaceWrapper *wrapper=checkSurface(L);
	return pushSize( L, wrapper->surface()->getSize() );
}

static int l_attrColor( lua_State *L ) {
  	APRINT_LUA("==>");
	//	Method:
	//	a) SET: canvas R G B A
	//	b) SET: canvas cl_name
	//	c) GET: canvas -> R G B A
	SurfaceWrapper *wrapper=checkSurface(L);

	if (lua_gettop(L) == 1) {
		APRINT_LUA("<== <-- pushColor()");
		//	(c)
		return pushColor( L, wrapper->surface()->getColor() );
	}
	else {
		//	Set
		if (lua_type(L, 2) == LUA_TNUMBER) {
			//	(a)
			::canvas::Color color;
			checkColor( L, 2, color );
			wrapper->surface()->setColor( color );
		}
		else {
			//	(b)
			const char *textColor=luaL_checkstring(L, 2);
			if (!textColor) {
        			APRINT_LUA("<== ERROR");
				return luaL_error(L, "The color must be a string or a RGBA color\n");
			}

			//	Transform string color to RGB
			::canvas::Color color;
			if (!::canvas::color::get( textColor, color )) {
				//  LG Electronics: #4227
				//  Hack to make lua run further despite to incorrect color setting
				//  Required to run LuaRocks application
				LWARN("canvas::lua", "The string must be a NCL defined color, previous color is kept");
 				APRINT_LUA("<== WARN wrong color. Returning O0 nonetheless");
				return 0;
			}

			//	Try set color
			wrapper->surface()->setColor( color );
		}
		wrapper->surface()->setWasSetColorCalled(true);
    		APRINT_LUA("<== OK");
		return 0;
	}
}

static int l_attrFont( lua_State *L ) {
	APRINT_LUA("==>");
	//	a) canvas:attrFont() -> face: string; size: number; style: string
	//	b) canvas:attrFont( face: string; size: number; style: string )
	SurfaceWrapper *wrapper=checkSurface(L);
	 Module *module = Module::get( L );
        if (!module) {
                return luaL_error( L, "Invalid canvas module\n" );
        }

	if (lua_gettop(L) == 1) {
		//	(a)
		std::string luastyle;
		::canvas::Font font = wrapper->surface()->getFont();
		
		buildLuaFontStyle( luastyle, font.bold(), font.italic() );
		
		std::string face = font.familiesAsString();
		lua_pushstring( L, face.c_str() );
		lua_pushnumber( L, font.size() );
		if  (luastyle.compare("normal")==0)		
			lua_pushnil(L);
		else
			lua_pushstring( L, luastyle.c_str() );
		APRINT_LUA("<== [3]");
		return 3;
	}
	else {
		//	(b)
//		const char *face=luaL_checkstring(L,2);
		std::string face = luaL_checkstring(L,2);
		if (face.empty()) {
			APRINT_LUA("<== ERROR");
			return luaL_error(L, "The font must be a string\n");
		}
		
		int size = luaL_checkint(L, 3);
		
		const char *style=luaL_optstring(L,4,"normal");
		if (!style) {
			APRINT_LUA("<== ERROR");
			return luaL_error(L,"The style is not supported\n" );
		}

		//	Check style
		if (strcmp("normal",style) &&
			strcmp("bold",style) &&
			strcmp("italic",style) &&
			strcmp("bold-italic",style) &&
			//	LG Electronics: #3690
			//	Spec: ABNT NBR 15606-2:2011 -> 7.2.6 Presentation Specification functionality
			//	-> 10.3.3.3 Attributes : canvas:attrFont
			//	-> The possible style values are: 'bold', 'italic', 'bold-italic' and 'nil'.
			//	Description: Content hack, add "regular" and "bold and italic" fontStyle not allowed by specification
			strcmp("regular",style) &&
			strcmp("bold and italic",style) &&
			strcmp("Tiresias",style) &&
			strcmp("nil",style))
		{
			//LGESP
			//WORKAROUND TO AVOID PANIC
			bool sty  = false;//std::string::npos != luastyle.find("italic");
			bool bold = false;//std::string::npos != luastyle.find("bold");
			bool variant = false; //variant = "normal" | "small-caps"
			::canvas::Font font( face.c_str(), size, bold, sty, variant );
			if (!wrapper->surface()->setFont( font )) {
				APRINT_LUA("<== ERROR");
				return luaL_error(L, "The font has not been set\n");
			}
			//return luaL_error(L, "The font styles only can take one of the following values: normal, bold, bold-italic, italic, nil\n");
		}
		else	
		{	
#if 1 // install font
			std::string path = module->getFilePath( face.c_str() );
			if (!fs::exists( path )) {
				fs::path originModulePath = module->getOriginPath();
				originModulePath /= face;
				path = originModulePath.string();
				onFetchFilesFromCarousel(path);
				if (fs::exists( path )) {
					std::string familyInstalled = wrapper->surface()->installFont( path );
					if (!familyInstalled.empty()) face = familyInstalled;
				}
			}else{
				std::string familyInstalled = wrapper->surface()->installFont( path );
				if (!familyInstalled.empty()) face = familyInstalled;
			}
#endif
			//	parse font lua style 
			std::string luastyle(style);
			bool sty  = std::string::npos != luastyle.find("italic");
			bool bold = std::string::npos != luastyle.find("bold");
			bool variant = false; //variant = "normal" | "small-caps"
			::canvas::Font font( face.c_str(), size, bold, sty, variant );
			if (!wrapper->surface()->setFont( font )) {
				APRINT_LUA("<== ERROR");
				return luaL_error(L, "The font has not been set\n");
			}
		}

		APRINT_LUA("<== OK");
		return 0;
	}
}

static int l_attrClip( lua_State *L ) {
	APRINT_LUA("<==>");
	//	a) canvas:attrClip () -> x, y, width, height: number
	//	b) canvas:attrClip (x, y, width, height: number)
	SurfaceWrapper *wrapper=checkSurface(L);

	if (lua_gettop(L) == 1) {
		//	(a)
		::canvas::Rect rect;
		if (!wrapper->surface()->getClip( rect )) {
			return luaL_error(L, "There is no set clipping area\n");
		}
		return pushRect( L, rect );
	}
	else {
		//	(b)
		::canvas::Rect rect;
		checkRect( L, 2, rect );
		if (!wrapper->surface()->setClip( rect )) {
			//	LG Electronics: #4201
			//	Hack to make lua run further despite to incorrect clip setting
			//	Required to run 20090806_Gazeta_BBB9_NCL application
			LWARN("canvas::lua", "The clip was not set");
			return 0;
		}

		return 0;
	}
}

static int l_attrCrop( lua_State *L ) {
	APRINT_LUA("<==>");
	//	a) canvas:attrCrop () -> x, y, w, h: number	
	//	b) canvas:attrCrop (x, y, w, h: number)
	SurfaceWrapper *wrapper=checkSurface(L);

	if (lua_gettop(L) == 1) {
		//	(a)
		if (wrapper->isCropSet()) {
			canvas::Rect rect = wrapper->getCrop();
			return pushRect( L, rect );
		} else {
			const canvas::Rect &rect = wrapper->surface()->getBounds();
			return pushRect( L, rect );
		}
	} else {
		//	(b)		
		//	Get canvas module from stack
		Module *module = Module::get( L );
		if (!module) {
			return luaL_error( L, "Invalid canvas module\n" );
		}

    //can not set crop to main canvas
		if (wrapper->isPrimary()) {
			return luaL_error( L, "Can not set crop to main canvas\n" );
		}
		//	Check rectangle
		canvas::Rect rect;
		checkRect( L, 2, rect );
		wrapper->setCrop(rect);

		return 0;
	}	
}

static int l_attrFlip( lua_State *L ) {
	APRINT_LUA("<==>");
	//	a) canvas:attrFlip () -> horiz, vert: boolean	
	//	b) canvas:attrFlip ( horiz, vert: boolean)
	SurfaceWrapper *wrapper=checkSurface(L);
	
	//	Get canvas module from stack
	Module *module = Module::get( L );
	if (!module) {
		return luaL_error( L, "Invalid canvas module\n" );
	}

	if (lua_gettop(L) == 1) {
		//	(a)
		lua_pushboolean( L, wrapper->isFlippedH() );
		lua_pushboolean( L, wrapper->isFlippedV() );
		return 2;
	} else {
		//	(b)
		bool horiz;
		bool vert;

		if (wrapper->isPrimary()) {
			return luaL_error( L, "Invalid canvas module\n" ); 
		}

		if (lua_isboolean(L,2) && lua_isboolean(L,3)){
			horiz = lua_toboolean(L,2) != 0;
			vert = lua_toboolean(L,3) != 0;
		} else {
			return 0;
		}
		wrapper->setFlip(horiz, vert);
		return 0;
	}	
}

static int l_attrOpacity( lua_State *L ) {
	APRINT_LUA("<==>");
// 	a) canvas:attrOpacity (opacity: number)
// 	b) canvas:attrOpacity () -> opacity: number

	SurfaceWrapper *wrapper = checkSurface(L);

	if (lua_gettop(L) == 1) {
		lua_pushnumber(L, wrapper->surface()->getOpacity());
		return 1;
	} else {
		//	Get canvas module from stack
		Module *module = Module::get( L );
		if (!module) {
			return luaL_error( L, "Invalid canvas module\n" );
		}

		if (wrapper->isPrimary()) {
			return luaL_error( L, "Can not change the opacity on main canvas\n" );
		}

		int x = luaL_checkint(L, 2);
		x = (x < 0) ? 0:x;
		x = (x > 255) ? 255:x;

		wrapper->surface()->setOpacity( (util::BYTE) x );
		return 0;		
	}
}

static int l_attrRotation( lua_State *L ) {
	APRINT_LUA("<==>");
// 	a) canvas:attrRotation (degrees: number)
// 	b) canvas:attrRotation () -> degrees: number

	SurfaceWrapper *wrapper=checkSurface(L);

	if (lua_gettop(L) == 1) {
		//	(a)
		lua_pushnumber(L, wrapper->rotation());
		return 1;
	} else {
		//	(b)

		//	Get canvas module from stack
		Module *module = Module::get( L );
		if (!module) {
			return luaL_error( L, "Invalid canvas module\n" );
		}

		if (wrapper->isPrimary()) {
			return luaL_error( L, "Cannot rotate main canvas\n" );
		}

		int degrees = luaL_checkint(L, 2);
		if (degrees%90 == 0) {
			wrapper->rotation(degrees);
		}
		return 0;
	}
}

static int l_attrScale( lua_State *L ) {
	APRINT_LUA("==>");// attrScale( w, h: number | boolean )
	SurfaceWrapper *wrapper=checkSurface(L);
	if (lua_gettop(L) == 3) {
		//      Get canvas module from stack
		Module *module = Module::get( L );
		if (!module) {
			APRINT_LUA("<== ERROR");
			return luaL_error( L, "Invalid canvas module\n" );
		}
		if (wrapper->isPrimary()) {
			APRINT_LUA("<== ERROR");
			return luaL_error( L, "Cannot scale the main canvas\n" );
		}
		canvas::Size s = wrapper->surface()->getSize();
		int w, h;
		double fw, fh;

        	if (lua_isboolean(L, 2)==1) {
			if (lua_toboolean(L, 2)==0) { // attrScale (false, number) keep width size
				if (lua_isnumber(L, 3)==1){
					fw = 1;
					h = luaL_checkint(L, 3);
					if (h>=0){
						fh = ((double)h) / ((double)s.h);
					 }else {
						APRINT_LUA("<== ERROR");
						return 0; //luaL_error( L, "Height must be greater than -1\n" );
					}
				}else{ //height value is not a number
					APRINT_LUA("<== ERROR");
					return 0; //luaL_error( L, "At least one argument must be a number\n" );					
				}
            		}else{ // attrScale (true, number) indicates that the aspect ration must be kept
				if (lua_isnumber(L, 3)==1) {
					h = luaL_checkint(L, 3);
                    			if (h>=0) {
						fw = fh = ((double)h) / ((double)s.h);
                    			} else {
						APRINT_LUA("<== ERROR");
                        			return 0;//luaL_error( L, "Height must be greater than -1\n" );
                    			}
				} else {
					APRINT_LUA("<== ERROR");
                    			return 0;luaL_error( L, "At least one argument must be a number\n" );
                		}
            		}
        	} else if (lua_isboolean(L, 3)==1) {
			if (lua_toboolean(L, 3)==0) { // attrScale (number,false) keep height size
				if (lua_isnumber(L, 2)==1){
					fh = 1;
					w = luaL_checkint(L, 2);
					if (w>=0) {
						fw = ((double)w) / ((double)s.w);
					} else {
						APRINT_LUA("<== ERROR");
						return 0;// luaL_error( L, "Height must be greater than 0\n" );
					}
				}else{ //width value is not a number
					APRINT_LUA("<== ERROR");
					return 0; //luaL_error( L, "At least one argument must be a number\n" );					
				}
			}else { // attrScale (number,true) indicates that the aspect ration must be kept
                		if (lua_isnumber(L, 2)==1) {
					w = luaL_checkint(L, 2);
                    			if (w>=0) {
						fw = fh = ((double)w) / ((double)s.w);
                    			} else {
						APRINT_LUA("<== ERROR");
                        			return 0;//luaL_error( L, "Height must be greater than -1\n" );
                    			}
                		}else { //width value is not a number
					APRINT_LUA("<== ERROR");
					return 0;//luaL_error( L, "At least one argument must be a number\n" );
				}
			}
        	} else { //attrScale (number,number)
            		w = luaL_checkint(L, 2);
            		h = luaL_checkint(L, 3);
            		if (w>=0 && h>=0) {
				fw = ((double)w) / ((double)s.w);
                		fh = ((double)h) / ((double)s.h);
                	} else {
				APRINT_LUA("<== ERROR");
				return 0; //luaL_error( L, "Height and width must be greater than -1\n" );
                	}
		}	
            	wrapper->setScaledSize( canvas::Size(boost::math::iround(s.w*fw), boost::math::iround(s.h*fh)) );
            	APRINT_LUA("<== OK");
            	return 0;
		
	} else { //  attrScale() returns the current Scaled size 
		APRINT_LUA("<== <-- pushSize");
		return pushSize( L, wrapper->getScaledSize() );
    	}
}
static int l_drawLine( lua_State *L ) {
	APRINT_LUA("<==>");
	//	canvas:drawLine (x1, y1, x2, y2: number)
	if (lua_gettop(L) == 5) {
		SurfaceWrapper *wrapper=checkSurface(L);

		int x1 = luaL_checkint(L, 2);
		int y1 = luaL_checkint(L, 3);
		int x2 = luaL_checkint(L, 4);
		int y2 = luaL_checkint(L, 5);

		wrapper->surface()->drawLine( x1, y1, x2, y2 );
	}

	return 0;
}

static int l_drawRect( lua_State *L ) {
  APRINT_LUA("==>");
	//	canvas:drawRect (mode: string; x, y, width, height: number) mode = [fill|frame]
	int result = 0;
	SurfaceWrapper *wrapper=checkSurface(L);

	const char *mode = luaL_checkstring(L,2);
	if (!mode) {
    APRINT_LUA("<== ERROR");
		return luaL_error(L, "Bad argument. The first argument must be a string.");
	}

	::canvas::Rect rect;
	checkRect( L, 3, rect );

	if (!strcmp( "fill", mode )) {
		wrapper->surface()->fillRect( rect );
	}
	else if (!strcmp( "frame", mode )) {
		wrapper->surface()->drawRect( rect );
	}
	else {
		result = luaL_error(L, "The first argument is invalid. Options: fill | frame");
	}
  APRINT_LUA("<== [%d]",result);
	return result;
}

static int l_drawRoundRect( lua_State *L ) {
	APRINT_LUA("<==>");
	//	canvas:drawRoundRect (mode: string; x, y, width, height, arcWidth, arcHeight: number) mode = [fill|frame]
	int result = 0;
	SurfaceWrapper *wrapper=checkSurface(L);
	
	const char *mode = luaL_checkstring(L,2);
	if (!mode) {
		return luaL_error(L, "Bad argument. The first argument must be a string.");
	}

	::canvas::Rect rect;
	checkRect( L, 3, rect );

	int arcW = luaL_checkint(L, 7);
	int arcH = luaL_checkint(L, 8);

	if (!strcmp( "fill", mode )) {
		wrapper->surface()->fillRoundRect( rect, arcW, arcH );
	}
	else if (!strcmp( "frame", mode )) {
		wrapper->surface()->drawRoundRect( rect, arcW, arcH );
	}
	else {
		result = luaL_error(L, "The first argument is invalid. Options: fill | frame.");
	}

	return result;
}

//	Aux l_drawPolygon function
static int l_auxPolygonDrawing( lua_State *L ) {
	APRINT_LUA("<==>");
	//	gets the drawer instance from the lua stack
	impl::PolygonDrawer **drawer = (impl::PolygonDrawer **)lua_touserdata(L, lua_upvalueindex(1));

	if (lua_gettop(L) == 2) {
		::canvas::Point point;
		//	gets the point passed by parameter
		checkPoint( L, 1, point );

		(*drawer)->addPoint(point);

		// push the drawer instance in the stack
		lua_pushlightuserdata(L, drawer);

		// 	push the return function in the stack
		lua_pushcclosure( L, l_auxPolygonDrawing, 1 );
		return 1;
	} else {
		int res = (*drawer)->draw(L);
		DEL(*drawer);
		return res;
	}
}

static int l_drawPolygon( lua_State *L ) {
	APRINT_LUA("<==>");
	SurfaceWrapper *wrapper = checkSurface(L);

	if (lua_gettop(L) == 2) {
		const char *mode = luaL_checkstring(L,2);
		if (!mode) {
			return luaL_error(L, "Bad argument. The argument must be a string.");
		}

		if ((strcmp(mode, "open") != 0) && (strcmp(mode, "fill") != 0) && (strcmp(mode, "close") != 0) ) {
			return luaL_error(L, "Bad argument. The draw mode must be open, close or fill.");
		}

		impl::PolygonDrawer **drawer = (impl::PolygonDrawer **) lua_newuserdata(L, sizeof(impl::PolygonDrawer *));
		*drawer = new impl::PolygonDrawer( mode, wrapper );
		/* set its metatable */
		luaL_getmetatable(L, "Aux.Drawer");
		lua_setmetatable(L, -2);

		//	 push the return function
		lua_pushcclosure( L, l_auxPolygonDrawing, 1 );
		return 1;
	} else {
		return luaL_error(L, "No argument. To draw a polygon one of the following draw modes must be specified : fill, close, open.");
	}
}

static int l_drawEllipse( lua_State *L ) {
	APRINT_LUA("<==>");
	SurfaceWrapper *wrapper=checkSurface(L);

	const char *mode = luaL_checkstring(L,2);
	if (!mode) {
		return luaL_error(L, "Bad argument. The first argument must be a string.");
	}

	::canvas::Point point;
	checkPoint( L, 3, point );

	int w    =  boost::math::iround(luaL_checknumber(L, 5)/2);
	int h    =  boost::math::iround(luaL_checknumber(L, 6)/2);
	int arcW = luaL_checkint(L, 7);
	int arcH = luaL_checkint(L, 8);

	if (!strcmp( "fill", mode )) {
		wrapper->surface()->fillEllipse( point, w, h, arcW, arcH );
	}
	else if (!strcmp( "arc", mode )) {
		wrapper->surface()->drawEllipse( point, w, h, arcW, arcH );
	}
	else {
		return luaL_error(L, "The first argument is invalid. Options: fill | arc");
	}
	
	return 0;	
}

static int l_drawText( lua_State *L ) {
	APRINT_LUA("==>");
	//	a) canvas:drawText (text: string; x,y: number)
	//	b) canvas:drawText (x,y: number; text: string)
	SurfaceWrapper *wrapper=checkSurface(L);
	
	canvas::Point point;
	//	(a)
	//	LG Electronics
	//	old spec (2007) support
	const char *text;
	if (lua_type(L, 2) == LUA_TSTRING) {
		checkPoint( L, 3, point );
		text = luaL_checkstring(L,2);
	}
	else {
	//	b
		checkPoint( L, 2, point );
		text = luaL_checkstring(L,4);
	}
	if (!text) {
    APRINT_LUA("<== ERROR");
		return luaL_error(L, "Bad argument. The third argument must be a string.");
	}
	point.y += wrapper->surface()->fontAscent();

#ifdef HTV_NC_VER /* LGE Platform */
	point.y--;
#endif /* HTV_NC_VER */

	APRINT_LUA("text[%s]",text);
	wrapper->surface()->drawText( point, text );
  APRINT_LUA("<== OK");
	return 0;
}

static int l_clear( lua_State *L ) {
  APRINT_LUA("==>");
	//	(a) canvas:clear ()	
	//	(b) canvas:clear (x, y, w, h: number)
	SurfaceWrapper *wrapper=checkSurface(L);
	wrapper->surface()->setCompositionMode( canvas::composition::source );
	
  	canvas::Rect rect(0,0,0,0);
	if (lua_gettop(L) == 1) {
    		APRINT_LUA("---");
		//	(a)
		canvas::Size size = wrapper->surface()->getSize();
    		rect.w = size.w;
    		rect.h = size.h;
	} else {
    		APRINT_LUA("---");
		//	(b)
		//canvas::Rect rect;
		checkRect( L, 2, rect );
	}
  		//LGESP-492
  	if( !wrapper->surface()->getWasSetColorCalled()){
		//in case no font was set
    		canvas::Color DEFAULT_COLOR(0,0,0,255) ; //black opaque
		canvas::Color DEFAULT_CLEARCOLOR(0,0,0,0) ; //black transparent
		wrapper->surface()->setColor(DEFAULT_CLEARCOLOR);
		wrapper->surface()->fillRect(rect);
		//restore previous state
		wrapper->surface()->setColor(DEFAULT_COLOR);
		wrapper->surface()->setWasSetColorCalled(false);
	}else{
    		//biz as usual
    		wrapper->surface()->fillRect(rect);
  	}
 	APRINT_LUA("---");
	wrapper->surface()->setCompositionMode( canvas::composition::source_over );
  	APRINT_LUA("<==");
	return 0;
}

static int l_flush( lua_State *L ) {
	APRINT_LUA("==>");
	//	canvas:flush ()
	SurfaceWrapper *wrapper=checkSurface(L);
	if (wrapper->surface()->autoFlush()) {
		wrapper->surface()->canvas()->flush(true);
	}
	APRINT_LUA("<== OK");
	return 0;
}

static int l_compose( lua_State *L ) {
	APRINT_LUA("==>");
	//	(a) canvas:compose (x, y: number; src: canvas )	
	//	(b) canvas:compose (x, y: number; src: canvas; src_x, src_y, src_width, src_height: number )
	SurfaceWrapper *wrapper=checkSurface(L);
	
	canvas::Point point;
	checkPoint( L, 2, point );

	SurfaceWrapper *srcWrapper=checkSurface(L,4);
	
	if (!srcWrapper) {
		APRINT_LUA("<== ERROR");
		return luaL_error(L, "The source is invalid");
	}

	canvas::Size srcSize = srcWrapper->surface()->getSize();
	canvas::Rect srcRec(0, 0, srcSize.w, srcSize.h);
	bool doCrop = false;
	if (lua_gettop(L) > 4) {
		// (b)
		checkRect(L, 5, srcRec);
		srcRec.w = std::min(abs(srcSize.w - srcRec.x),srcRec.w);
        	srcRec.h = std::min((srcSize.h - srcRec.y),srcRec.h);
		doCrop = true;
	}

	if ((srcWrapper->isCropSet())&& (doCrop == false)) {
		canvas::Rect rectCrop = srcWrapper->getCrop();
		srcRec.w = std::min(srcRec.w, std::max(rectCrop.w - srcRec.x, 0));
		srcRec.h = std::min(srcRec.h, std::max(rectCrop.h - srcRec.y, 0));
		srcRec.x += rectCrop.x;
		srcRec.y += rectCrop.y;
		doCrop = true;
	}

	if (!srcWrapper->rotation() && !srcWrapper->needScale()) {
		wrapper->surface()->blitEx(point, srcWrapper->surface(), srcRec, true, true);
		APRINT_LUA("<== OK");
		return 0;
	}
	canvas::Surface *surface;
	if (doCrop) {
		if ((srcRec.w <= 0) || (srcRec.h <= 0)) {
			APRINT_LUA("<== ERROR");
			return 0;
		}

		surface = getCanvas(wrapper)->createSurface(canvas::Rect(0, 0, srcRec.w, srcRec.h));
		if (!surface) {
			APRINT_LUA("<== ERROR");
			return luaL_error( L, "Cannot create surface from rect\n" );
		}

		surface->blitEx(canvas::Point(0, 0), srcWrapper->surface(), srcRec, false, false);
	} else {
		surface = srcWrapper->surface();
	}

	if (srcWrapper->rotation() != 0) {
		canvas::Surface *tmp = surface->rotate(srcWrapper->rotation());
		if (doCrop) {
			canvas::Surface::destroy(surface);
		}
		surface = tmp;
	}

	if (surface != srcWrapper->surface()) {
		surface->setOpacity(wrapper->surface()->getOpacity());
	}

	if (srcWrapper->needScale()) {
		wrapper->surface()->scale(canvas::Rect(point, srcWrapper->getScaledSize()),
					surface,
					srcWrapper->isFlippedH(),
					srcWrapper->isFlippedV());
	} else {
		canvas::Size sz = surface->getSize();
		canvas::Rect r(0, 0, sz.w, sz.h);
		wrapper->surface()->blitEx(point, surface, r, true, true);
	}

	if (doCrop || (srcWrapper->rotation() != 0)) {
		canvas::Surface::destroy(surface);
	}

	APRINT_LUA("<== OK");
	return 0;
}

static int l_pixel( lua_State *L ) {
	APRINT_LUA("<==>");
	//	(a) canvas:pixel (x, y: number) -> R, G, B, A: number
	//	(b) canvas:pixel (x, y, R, G, B, A: number)
	SurfaceWrapper *wrapper=checkSurface(L);
	
	::canvas::Point point;
	checkPoint( L, 2, point );
	
	if (lua_gettop(L) == 3) {
		//	(a)
		::canvas::Color color;
		if (!wrapper->surface()->getPixelColor( point, color )) {
			return luaL_error(L, "Bad argument. Invalid point ");
		}
		return pushColor( L, color );
	}
	else {
		//	(b)
		::canvas::Color color;
		checkColor( L, 4, color );
		wrapper->surface()->setPixelColor( point, color );
		return 0;
	}
}

static int l_measureText( lua_State *L ) {
	APRINT_LUA("<==>");
	//	a) canvas:measureText (x, y: number; text: string) -> x1, x2, y1, y2: number
	//	b) canvas:measureText (text: string) -> dx, dy: number
	SurfaceWrapper *wrapper=checkSurface(L);
	
	//	(a)
	//	LG Electronics
	//	old spec (2007) support
	if (lua_gettop(L) == 4 && lua_type(L, 4) == LUA_TSTRING) {
		int x1 = luaL_checkint(L, 2);
		int y1 = luaL_checkint(L, 3);
		const char *text = luaL_checkstring(L,4);
		if (!text) {
			return luaL_error(L, "Bad argument. The text argument must be a string.");
		}
		::canvas::Size size;
		if (!wrapper->surface()->measureText( text, size )) {
			return luaL_error(L, "The text is invalid.");
		}
		lua_pushnumber(L, x1);
		lua_pushnumber(L, y1);
		lua_pushnumber(L, x1 + size.w);
		lua_pushnumber(L, y1 + size.h);
		return 4;
	}
	//	(b)
	const char *text = luaL_checkstring(L,2);
	if (!text) {
		return luaL_error(L, "Bad argument. The text argument must be a string.");
	}

	::canvas::Size size;
	if (!wrapper->surface()->measureText( text, size )) {
		return luaL_error(L, "The text is invalid.");

	}
	return pushSize( L, size );
}

static const struct luaL_Reg canvas_methods[] = {
	{ "new",           l_new           },
	{ "attrSize",      l_attrSize      },
	{ "attrColor",     l_attrColor     },
	{ "attrFont",      l_attrFont      },
	{ "attrClip",      l_attrClip      },
	{ "attrCrop",      l_attrCrop      },
	{ "attrFlip",      l_attrFlip      },
	{ "attrOpacity",   l_attrOpacity   },
	{ "attrRotation",  l_attrRotation  },
	{ "attrScale",     l_attrScale     },
	{ "drawLine",      l_drawLine      },
	{ "drawRect",      l_drawRect      },
	{ "drawRoundRect", l_drawRoundRect },
	{ "drawPolygon",   l_drawPolygon   },
	{ "drawEllipse",   l_drawEllipse   },
	{ "drawText",      l_drawText      },
	{ "flush",         l_flush         },
	{ "clear",         l_clear         },
	{ "compose",       l_compose       },
	{ "pixel",         l_pixel         },
	{ "measureText",   l_measureText   },
	{ NULL,            NULL            }
};

/*******************************************************************************
*	Canvas module
*******************************************************************************/

Module::Module( LuaPlayer *player, lua_State *st )
	: _player(player), _lua( st )
{
	APRINT_LUA("<==>");
	//	Store module into stack
	lua::storeObject( st, this, LUA_CANVAS );

	//	Register new type and set the metadata
	luaL_newmetatable(st, CANVAS_TYPE);
	lua_pushvalue(st, -1);
	lua_setfield(st, -2, "__index");
	luaL_register(st, NULL, canvas_methods);
	lua_pop(st, 1);
	lua_pushnil(st);

	_originPath = fs::current_path().string();
}

Module::~Module()
{
	APRINT_LUA("<==>");
	lua::storeObject( _lua, NULL, LUA_CANVAS );
}

void Module::start( canvas::Surface *surface ) {
	APRINT_LUA("<==>");
	//	Create main canvas
	createSurface(_lua, surface, true);
	lua_setglobal(_lua, "canvas");

	//	LG Electronics: #4307
	//	Persistent global variable adding with 'channel', 'shared' and 'service' groups of variables
	//	Content examples are Idolos_2012, Balacobaco, Camera_Record, Eleicoes, Reporter_Record, Idolos_Kids_2012
	//TODO:	provide possibility to export and save persistent table
	lua_newtable(_lua);

	lua_newtable(_lua);
	lua_setfield(_lua, -2, "channel");

	lua_newtable(_lua);
	lua_setfield(_lua, -2, "shared");

	lua_newtable(_lua);
	lua_setfield(_lua, -2, "service");

	lua_setfield(_lua, LUA_GLOBALSINDEX, "persistent");

	//set default font
	setDefaultFont(surface);
}

void Module::stop() {
	APRINT_LUA("<==>");
	LINFO("canvas::Module", "Stop");
	CLEAN_ALL( SurfaceWrapper* , _surfaces );
}

static int gcDestroySurface( lua_State *st )
{
	APRINT_LUA("<==>");
	luaL_checktype(st, 1, LUA_TUSERDATA);
	SurfaceWrapperPtr *wrapperptr = reinterpret_cast<SurfaceWrapperPtr*>(lua_touserdata(st, 1));
	SurfaceWrapper *wrapper = *wrapperptr;

	Module *module = Module::get( st );
	if (module) {
		module->removeWrapper(wrapper);
	}
	return 0;
}

int Module::createSurface(lua_State *L, canvas::Surface *surface, bool isPrimary/*=false*/ ) {
	APRINT_LUA("<==>");
	SurfaceWrapperPtr *newSurface = (SurfaceWrapperPtr *)lua_newuserdata(L, sizeof(SurfaceWrapperPtr) );
	SurfaceWrapper *wrapper = new SurfaceWrapper( surface, isPrimary );
	*newSurface = wrapper;
  canvas::Color DEFAULT_COLOR(0,0,0,255) ; //black opaque
	//surface->setColor(canvas::Color(0,0,0,255));
	surface->setColor(DEFAULT_COLOR);
	luaL_getmetatable(L, CANVAS_TYPE );
	lua_pushcfunction(L, gcDestroySurface);
	lua_setfield(L, -2, "__gc");
	lua_setmetatable(L, -2);
	_surfaces.push_back( wrapper );
	return 1;
}

std::string Module::getFilePath( const std::string &filepath ) {
	APRINT_LUA("<==> filepath[%s]",filepath.c_str());
	//	Make asbsolute path and create the image surface from file
	fs::path absPath( _player->rootPath() );
	absPath /= filepath;

	// LGESP: Change Request
	// Description: Lua script does not call any TV function before loading
	// image. So, this hack should invoke the implemented fetch callback.
	player::Device * dev = _player->device();
	dev->onFetchFile(absPath.string());
	// If this callback was not set during player::Device, this code should
	// do nothing because the function pont "onFetchFile" is skipped.
	// end of LGESP

	return absPath.string();
}

Module *Module::get( lua_State *st ) {
	APRINT_LUA("<==>");
	return lua::getObject<Module>(st,LUA_CANVAS);
}

std::string Module::getOriginPath() const {
  APRINT_LUA("<==>");
	return _originPath;
}

bool Module::removeWrapper( SurfaceWrapper *wrapper ) {
	APRINT_LUA("<==>");
	bool res = false;
	std::vector<SurfaceWrapper *>::iterator it=std::find(
		_surfaces.begin(), _surfaces.end(), wrapper );
	if (it != _surfaces.end()) {
		delete (*it);
		_surfaces.erase( it );
		res = true;
	}
	return res;
}

}
}


