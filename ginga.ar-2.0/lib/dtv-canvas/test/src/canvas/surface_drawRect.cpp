#include "surface.h"
#include "../util.h"
#include "../../../src/canvas.h"
#include "../../../src/surface.h"

class SurfaceDrawRect : public Surface {
protected:
    std::string getSubDirectory() const { return "DrawRect"; }
};

// Draw a rectangle on the middle of the surface
TEST_F( SurfaceDrawRect, basic ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));
        ASSERT_TRUE( s->drawRect(canvas::Rect(310, 238, 100, 100)) );

        ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "basic" ) ) );

        getCanvas()->destroy( s );
}

// Draw a rectangle on the top left corner of the surface
TEST_F( SurfaceDrawRect, top_left ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));
        ASSERT_TRUE( s->drawRect(canvas::Rect(0, 0, 100, 100)) );

        ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "top_left" ) ) );

        getCanvas()->destroy( s );
}

// Draw a rectangle on the top right corner of the surface
TEST_F( SurfaceDrawRect, top_right ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));
        ASSERT_TRUE( s->drawRect(canvas::Rect(620, 0, 100, 100)) );

        ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "top_right" ) ) );

        getCanvas()->destroy( s );
}

// Draw a rectangle on the bottom left corner of the surface
TEST_F( SurfaceDrawRect, bottom_left ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));
        ASSERT_TRUE( s->drawRect(canvas::Rect(0, 476, 100, 100)) );

        ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "bottom_left" ) ) );

        getCanvas()->destroy( s );
}

// Draw a rectangle on the bottom right corner of the surface
TEST_F( SurfaceDrawRect, bottom_right ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));
        ASSERT_TRUE( s->drawRect(canvas::Rect(620, 476, 100, 100)) );

        ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "bottom_right" ) ) );

        getCanvas()->destroy( s );
}

// Draw a rectangle with width greater than height
TEST_F( SurfaceDrawRect, width_gt_height ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));
        ASSERT_TRUE( s->drawRect(canvas::Rect(310, 263, 100, 50)) );

        ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "width_gt_height" ) ) );

        getCanvas()->destroy( s );
}

// Draw a rectangle with height greater than width
TEST_F( SurfaceDrawRect, height_gt_width ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));
        ASSERT_TRUE( s->drawRect(canvas::Rect(335, 238, 50, 100)) );

        ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "height_gt_width" ) ) );

        getCanvas()->destroy( s );
}

// Draw a rectangle with the same height of the surface
TEST_F( SurfaceDrawRect, same_height_surface ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));
        ASSERT_TRUE( s->drawRect(canvas::Rect(310, 0, 100, 576)) );

        ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "same_height_surface" ) ) );

        getCanvas()->destroy( s );
}

// Draw a rectangle with the same width of the surface
TEST_F( SurfaceDrawRect, same_width_surface ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));
        ASSERT_TRUE( s->drawRect(canvas::Rect(0, 238, 720, 100)) );

        ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "same_width_surface" ) ) );

        getCanvas()->destroy( s );
}

// Draw a rectangle with the same size of the surface
TEST_F( SurfaceDrawRect, same_size_surface ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));
        ASSERT_TRUE( s->drawRect(canvas::Rect(0, 0, 720, 576)) );

        ASSERT_TRUE( util::compareImages( getCanvas(),getExpectedPath( "same_size_surface" ) ) );

        getCanvas()->destroy( s );
}

// Draw a rectangle with width = 0
TEST_F( SurfaceDrawRect, width_zero ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));
        ASSERT_TRUE( s->drawRect(canvas::Rect(360, 238, 0, 100)) );

        ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ));

        getCanvas()->destroy( s );
}

// Draw a rectangle with height = 0
TEST_F( SurfaceDrawRect, height_zero ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));
        ASSERT_TRUE( s->drawRect(canvas::Rect(310, 288, 100, 0)) );

        ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ));

        getCanvas()->destroy( s );
}

// Draw a rectangle with height = 0 and width = 0
TEST_F( SurfaceDrawRect, height_and_width_zero ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));
        ASSERT_TRUE( s->drawRect(canvas::Rect(310, 288, 0, 0)) );

        ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ));

        getCanvas()->destroy( s );
}

// Draw a rectangle with width lower than 0
TEST_F( SurfaceDrawRect, width_negative ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));

        ASSERT_FALSE( s->drawRect(canvas::Rect(310, 238, -1, 100)) );

        getCanvas()->destroy( s );
}

// Draw a rectangle with height lower than 0
TEST_F( SurfaceDrawRect, height_negative ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));

        ASSERT_FALSE( s->drawRect(canvas::Rect(310, 238, 100, -1)) );

        getCanvas()->destroy( s );
}

// Draw a rectangle with width and height lower than 0
TEST_F( SurfaceDrawRect, height_and_width_negative ) {
	    canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));
        ASSERT_FALSE( s->drawRect(canvas::Rect(310, 238, -1, -1)) );

        getCanvas()->destroy( s );
}

// Draw a rectangle completely outside the surface (top)
TEST_F( SurfaceDrawRect, completely_outside_top ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));

        ASSERT_FALSE( s->drawRect(canvas::Rect(310, -100, 100, 100)) );

        getCanvas()->destroy( s );
}

// Draw a rectangle completely outside the surface (bottom)
TEST_F( SurfaceDrawRect, completely_outside_bottom ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));

        ASSERT_FALSE( s->drawRect(canvas::Rect(310, 576, 100, 100)) );

        getCanvas()->destroy( s );
}

// Draw a rectangle completely outside the surface (left)
TEST_F( SurfaceDrawRect, completely_outside_left ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));

        ASSERT_FALSE( s->drawRect(canvas::Rect(-100, 238, 100, 100)) );

        getCanvas()->destroy( s );
}

// Draw a rectangle completely outside the surface (right)
TEST_F( SurfaceDrawRect, completely_outside_right ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));

        ASSERT_FALSE( s->drawRect(canvas::Rect(720, 238, 100, 100)) );

        getCanvas()->destroy( s );
}


// Draw a rectangle with two vertices out of the surface (left)
TEST_F( SurfaceDrawRect, two_vertices_out_left ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));

        ASSERT_FALSE( s->drawRect(canvas::Rect(-1, 238, 100, 100)) );

        getCanvas()->destroy( s );
}

// Draw a rectangle with two vertices out of the surface (top)
TEST_F( SurfaceDrawRect, two_vertices_out_top ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));

        ASSERT_FALSE( s->drawRect(canvas::Rect(310, -1, 100, 100)) );

        getCanvas()->destroy( s );
}

// Draw a rectangle with two vertices out of the surface (right)
TEST_F( SurfaceDrawRect, two_vertices_out_right ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));

        ASSERT_FALSE( s->drawRect(canvas::Rect(621, 238, 100, 100)) );

        getCanvas()->destroy( s );
}

// Draw a rectangle with two vertices out of the surface (bottom)
TEST_F( SurfaceDrawRect, two_vertices_out_bottom ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));

        ASSERT_FALSE( s->drawRect(canvas::Rect(310, 477, 100, 100)) );

        getCanvas()->destroy( s );
}

// Draw a rectangle with three vertices out of the surface (top-left)
TEST_F( SurfaceDrawRect, three_vertices_out_top_left ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));

        ASSERT_FALSE( s->drawRect(canvas::Rect(-1, -1, 100, 100)) );

        getCanvas()->destroy( s );
}

// Draw a rectangle with three vertices out of the surface (top-right)
TEST_F( SurfaceDrawRect, three_vertices_out_top_right ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));

        ASSERT_FALSE( s->drawRect(canvas::Rect(720, -1, 100, 100)) );

        getCanvas()->destroy( s );
}

// Draw a rectangle with three vertices out of the surface (bottom-left)
TEST_F( SurfaceDrawRect, three_vertices_out_bottom_left ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));

        ASSERT_FALSE( s->drawRect(canvas::Rect(-1, 477, 100, 100)) );

        getCanvas()->destroy( s );
}

// Draw a rectangle with three vertices out of the surface (bottom-right)
TEST_F( SurfaceDrawRect, three_vertices_out_bottom_right ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));

        ASSERT_FALSE( s->drawRect(canvas::Rect(621, 477, 100, 100)) );

        getCanvas()->destroy( s );
}

// Draw a rectangle with four vertices out of the surface that crosses the surface horizontally
TEST_F( SurfaceDrawRect, four_vertices_out_crossing_surface_horizontally ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));

        ASSERT_FALSE( s->drawRect(canvas::Rect(-1, 238, 722, 100)) );

        getCanvas()->destroy( s );
}

// Draw a rectangle with four vertices out of the surface that crosses the surface vertically
TEST_F( SurfaceDrawRect, four_vertices_out_crossing_surface_vertically ) {
        canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

        ASSERT_TRUE( s != NULL );
        s->autoFlush(true);
        s->setColor(canvas::Color(255,0,0));

        ASSERT_FALSE( s->drawRect(canvas::Rect(310, -1, 310, 578)) );

        getCanvas()->destroy( s );
}

// Draw a rect with alpha on the middle of the surface .
TEST_F( SurfaceDrawRect, rect_with_alpha_and_solid_background ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    s->setColor( canvas::Color(0, 0, 0) );
    ASSERT_TRUE( s->fillRect(canvas::Rect(0, 0, 720, 576)) );

    s->setColor( canvas::Color(255, 0, 0) );
    s->drawLine( 0,308,719,308 );
    s->setColor( canvas::Color(255, 0, 0, 128) );
    s->drawLine( 0,268,719,268 );

    s->setColor( canvas::Color(0, 255, 0, 128) );

    canvas::Rect r(160, 188, 400, 200);
    ASSERT_TRUE( s->drawRect( r ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "rect_with_alpha_and_solid_background" ) ) );

    getCanvas()->destroy( s );
}

// Draw a rect with alpha on the middle of the surface .
TEST_F( SurfaceDrawRect, rect_with_alpha_and_transparent_background ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    s->setColor( canvas::Color(255, 0, 0) );
    s->drawLine( 0,308,719,308 );
    s->setColor( canvas::Color(255, 0, 0, 128) );
    s->drawLine( 0,268,719,268 );

    s->setColor( canvas::Color(0, 255, 0, 128) );

    canvas::Rect r(160, 188, 400, 200);
    ASSERT_TRUE( s->drawRect( r ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "rect_with_alpha_and_transparent_background" ) ) );

    getCanvas()->destroy( s );
}

// Draw a full transparent rect over a solid background.
TEST_F( SurfaceDrawRect, full_transparent_over_solid_background ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    s->setColor( canvas::Color(0, 0, 0) );
    ASSERT_TRUE( s->fillRect(canvas::Rect(0, 0, 720, 576)) );

    s->setColor( canvas::Color(0, 255, 0, 0) );

    canvas::Rect r(160, 188, 400, 200);
    ASSERT_TRUE( s->drawRect( r ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), "nothing_black_background" ) );

    getCanvas()->destroy( s );
}

// Draw a full transparent rect over a transparent background.
TEST_F( SurfaceDrawRect, full_transparent_over_transparent_background ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    s->setColor( canvas::Color(0, 255, 0, 0) );

    canvas::Rect r(160, 188, 400, 200);
    ASSERT_TRUE( s->drawRect( r ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("full_transparent_over_transparent_background") ) );

    getCanvas()->destroy( s );
}

// Test markDirty after drawing a rect
TEST_F( SurfaceDrawRect, markDirty_drawRect ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	getCanvas()->flush();

	canvas::Rect r1(0,0,1,1),
				 r2(10,10,10,5),
				 r3(30,30,5,10),
				 r4(50,50,10,1),
				 r5(70,70,1,10);

	s->drawRect( r1 ); // Point
	s->drawRect( r2 ); // Rect with w > h
	s->drawRect( r3 ); // Rect with h > w
	s->drawRect( r4 ); // Rect with h = 1
	s->drawRect( r5 ); // Rect with w = 1

	std::vector<canvas::Rect> dirtyRegions = getCanvas()->dirtyRegions();

	ASSERT_FALSE( dirtyRegions.empty() );
	ASSERT_TRUE( dirtyRegions.size() == 5 );

	ASSERT_TRUE( dirtyRegions.at(0) == r1 );
	ASSERT_TRUE( dirtyRegions.at(1) == r2 );
	ASSERT_TRUE( dirtyRegions.at(2) == r3 );
	ASSERT_TRUE( dirtyRegions.at(3) == r4 );
	ASSERT_TRUE( dirtyRegions.at(4) == r5 );

	getCanvas()->destroy( s );
}
