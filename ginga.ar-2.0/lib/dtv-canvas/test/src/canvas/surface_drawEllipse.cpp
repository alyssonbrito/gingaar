#include "surface.h"
#include "../util.h"
#include "../../../src/canvas.h"
#include "../../../src/surface.h"

class SurfaceDrawEllipse : public Surface {
protected:
    std::string getSubDirectory() const { return "DrawEllipse"; }
};

// Draw an ellipse on the top left corner of the surface.
TEST_F( SurfaceDrawEllipse, top_left ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawEllipse( canvas::Point(25, 50), 25, 50, 0, 0 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "top_left" ) ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse on the top right corner of the surface.
TEST_F( SurfaceDrawEllipse, top_right ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawEllipse( canvas::Point(694, 50), 25, 50, 0, 0 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "top_right" ) ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse on the bottom right corner of the surface.
TEST_F( SurfaceDrawEllipse, bottom_right ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawEllipse( canvas::Point(694, 525), 25, 50, 0, 0 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "bottom_right") ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse on the bottom left corner of the surface.
TEST_F( SurfaceDrawEllipse, bottom_left ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawEllipse( canvas::Point(25, 525), 25, 50, 0, 0 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "bottom_left" ) ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse from a square on the middle of the surface.
TEST_F( SurfaceDrawEllipse, from_square ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawEllipse( canvas::Point(360, 288), 50, 50, 0, 0 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "from_square" ) ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse from a rectangle with height > width on the middle of the surface.
TEST_F( SurfaceDrawEllipse, from_rectangle_h_gt_w ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawEllipse( canvas::Point(360, 288), 25, 50, 0, 0 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "from_rectangle_h_gt_w" ) ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse from a rectangle with height < width on the middle of the surface.
TEST_F( SurfaceDrawEllipse, from_rectangle_w_gt_h ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawEllipse( canvas::Point(360, 288), 50, 25, 0, 0 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "from_rectangle_w_gt_h" ) ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse very small.
TEST_F( SurfaceDrawEllipse, very_small ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawEllipse( canvas::Point(360, 288), 1, 1, 0, 0 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "very_small" ) ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse from a rectangle of the same width of the surface.
TEST_F( SurfaceDrawEllipse, same_width_surface ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawEllipse( canvas::Point(360, 288), 359, 50, 0, 0 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "same_width_surface" ) ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse from a rectangle of the same height of the surface.
TEST_F( SurfaceDrawEllipse, same_height_surface ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawEllipse( canvas::Point(360, 288), 50, 287, 0, 0 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "same_height_surface" ) ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse from a rectangle of the same size of the surface.
TEST_F( SurfaceDrawEllipse, same_size_surface ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawEllipse( canvas::Point(360, 288), 359, 287, 0, 0 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "same_size_surface" ) ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse from a rectangle with height = 0.
TEST_F( SurfaceDrawEllipse, height_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawEllipse( canvas::Point(360, 288), 50, 0, 0, 0 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse from a rectangle with width = 0.
TEST_F( SurfaceDrawEllipse, width_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawEllipse( canvas::Point(360, 288), 0, 50, 0, 0 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse from a rectangle with width = 0 and height = 0.
TEST_F( SurfaceDrawEllipse, width_0_and_height_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_TRUE( s->drawEllipse( canvas::Point(360, 288), 0, 0, 0, 0 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse completely outside the surface (top).
TEST_F( SurfaceDrawEllipse, outside_top ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawEllipse( canvas::Point(360, -51), 25, 50, 0, 0 ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse completely outside the surface (bottom).
TEST_F( SurfaceDrawEllipse, outside_bottom ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawEllipse( canvas::Point(360, 626), 25, 50, 0, 0 ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse completely outside the surface (left).
TEST_F( SurfaceDrawEllipse, outside_top_left ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawEllipse( canvas::Point(-26, 288), 25, 50, 0, 0 ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse completely outside the surface (right).
TEST_F( SurfaceDrawEllipse, outside_right ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawEllipse( canvas::Point(745, 288), 25, 50, 0, 0 ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse half outside the surface (top).
TEST_F( SurfaceDrawEllipse, half_outside_top ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawEllipse( canvas::Point(360, 0), 25, 50, 0, 0 ) );

	getCanvas()->destroy( s );
}


// Draw an ellipse half outside the surface (bottom).
TEST_F( SurfaceDrawEllipse, half_outside_bottom ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawEllipse( canvas::Point(360, 576), 25, 50, 0, 0 ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse half outside the surface (left).
TEST_F( SurfaceDrawEllipse, half_outside_left ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawEllipse( canvas::Point(0, 288), 25, 50, 0, 0 ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse half outside the surface (right).
TEST_F( SurfaceDrawEllipse, half_outside_right ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawEllipse( canvas::Point(719, 288), 25, 50, 0, 0 ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse three points outside the surface (top-left).
TEST_F( SurfaceDrawEllipse, three_points_outside_top_left ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawEllipse( canvas::Point(0, 0), 25, 50, 0, 0 ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse three points outside the surface (top-right).
TEST_F( SurfaceDrawEllipse, three_points_outside_top_right ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawEllipse( canvas::Point(720, 0), 25, 50, 0, 0 ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse three points outside the surface (bottom-left).
TEST_F( SurfaceDrawEllipse, three_points_outside_bottom_left ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawEllipse( canvas::Point(0, 576), 25, 50, 0, 0 ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse three points outside the surface (bottom-right).
TEST_F( SurfaceDrawEllipse, three_points_outside ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawEllipse( canvas::Point(720, 576), 25, 50, 0, 0 ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse four points outside the surface but crossing the surface horizontally.
TEST_F( SurfaceDrawEllipse, four_points_outside_crossing_surface_horizontally ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawEllipse( canvas::Point(360, 288), 361, 100, 0, 0 ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse four points outside the surface but crossing the surface vertically.
TEST_F( SurfaceDrawEllipse, four_points_outside_crossing_surface_vertically ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(0, 255, 0));
	ASSERT_FALSE( s->drawEllipse( canvas::Point(360, 288), 100, 289, 0, 0 ) );

	getCanvas()->destroy( s );
}

// Draw an ellipse with alpha from a square on the middle of the surface .
TEST_F( SurfaceDrawEllipse, ellipse_with_alpha_and_solid_background ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    s->setColor( canvas::Color(0, 0, 0) );
    ASSERT_TRUE( s->fillRect(canvas::Rect(0, 0, 720, 576)) );

    s->setColor( canvas::Color(255, 0, 0) );
    s->drawLine( 0,308,719,308 );
    s->setColor( canvas::Color(255, 0, 0, 127) );
    s->drawLine( 0,268,719,268 );

    s->setColor( canvas::Color(0, 255, 0, 127) );

    canvas::Point point(360, 288);
    ASSERT_TRUE( s->drawEllipse( point, 50, 50, 0, 0 ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "ellipse_with_alpha_and_solid_background" ) ) );

    getCanvas()->destroy( s );
}

// Draw an ellipse with alpha from a square on the middle of the surface .
TEST_F( SurfaceDrawEllipse, ellipse_with_alpha_and_transparent_background ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    s->setColor( canvas::Color(255, 0, 0) );
    s->drawLine( 0,308,719,308 );
    s->setColor( canvas::Color(255, 0, 0, 127) );
    s->drawLine( 0,268,719,268 );

    s->setColor( canvas::Color(0, 255, 0, 127) );

    canvas::Point point(360, 288);
    ASSERT_TRUE( s->drawEllipse( point, 50, 50, 0, 0 ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "ellipse_with_alpha_and_transparent_background" ) ) );

    getCanvas()->destroy( s );
}

// Draw a full transparent ellipse over a solid background.
TEST_F( SurfaceDrawEllipse, full_transparent_over_solid_background ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    s->setColor( canvas::Color(0, 0, 0) );
    ASSERT_TRUE( s->fillRect(canvas::Rect(0, 0, 720, 576)) );

    s->setColor( canvas::Color(0, 255, 0, 0) );

    canvas::Point point(360, 288);
    ASSERT_TRUE( s->drawEllipse( point, 50, 50, 0, 0 ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), "nothing_black_background" ) );

    getCanvas()->destroy( s );
}

// Draw a full transparent ellipse over a transparent background.
TEST_F( SurfaceDrawEllipse, full_transparent_over_transparent_background ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    s->setColor( canvas::Color(0, 255, 0, 0) );

    canvas::Point point(360, 288);
    ASSERT_TRUE( s->drawEllipse( point, 50, 50, 0, 0 ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("full_transparent_over_transparent_background") ) );

    getCanvas()->destroy( s );
}


// Test markDirty after drawing an ellipse
TEST_F( SurfaceDrawEllipse, markDirty_drawEllipse ) {
	canvas::Rect rect(0,0,720,576);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255,0,0));
	getCanvas()->flush();
	canvas::Point point(10,10);
	s->drawEllipse(point, 5, 5, 0, 0);

	std::vector<canvas::Rect> dirtyRegions = getCanvas()->dirtyRegions();

	ASSERT_FALSE( dirtyRegions.empty() );
	ASSERT_TRUE( dirtyRegions.size() == 1 );

	canvas::Rect r(5,5,11,11);
	ASSERT_TRUE( dirtyRegions.front() == r );

	getCanvas()->destroy( s );
}
