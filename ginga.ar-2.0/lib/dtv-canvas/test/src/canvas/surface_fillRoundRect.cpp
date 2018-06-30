#include "surface.h"
#include "../util.h"
#include "../../../src/canvas.h"
#include "../../../src/surface.h"

class SurfaceFillRoundRect: public Surface {
protected:
    std::string getSubDirectory() const { return "FillRoundRect"; }
};

// Draw a fill rounded rectangle on the middle of the surface
TEST_F( SurfaceFillRoundRect, middle ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->fillRoundRect( canvas::Rect(310, 263, 100, 50), 10, 10 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("middle") ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle on the top left corner of the surface
TEST_F( SurfaceFillRoundRect, top_left ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->fillRoundRect( canvas::Rect(0, 0, 100, 50), 10, 10 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("top_left") ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle on the top right corner of the surface
TEST_F( SurfaceFillRoundRect, top_right ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->fillRoundRect( canvas::Rect(620, 0, 100, 50), 10, 10 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("top_right") ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle on the bottom right corner of the surface
TEST_F( SurfaceFillRoundRect, bottom_right ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->fillRoundRect( canvas::Rect(620, 526, 100, 50), 10, 10 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("bottom_right") ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle on the bottom left corner of the surface
TEST_F( SurfaceFillRoundRect, bottom_left ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->fillRoundRect( canvas::Rect(0, 526, 100, 50), 10, 10 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("bottom_left") ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with arcW = 0
TEST_F( SurfaceFillRoundRect, arcW_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->fillRoundRect( canvas::Rect(310, 260, 100, 50), 0, 10 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("arcW_0") ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with arcH = 0
TEST_F( SurfaceFillRoundRect, arcH_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->fillRoundRect( canvas::Rect(310, 260, 100, 50), 10, 0 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("arcH_0") ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with arcW = 0 and arcH = 0
TEST_F( SurfaceFillRoundRect, arcW_and_arcH_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->fillRoundRect( canvas::Rect(310, 260, 100, 50), 0, 0 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("arcW_and_arcH_0") ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded square with arcW = w/2 and arcH = h/2. Must be a circle.
TEST_F( SurfaceFillRoundRect, arcW_and_arcH_half_w_h ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->fillRoundRect( canvas::Rect(310, 260, 100, 100), 50, 50 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("arcW_arcH_half_w_h") ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with arcW > w/2
TEST_F( SurfaceFillRoundRect, arcW_gt_w_div_2 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->fillRoundRect( canvas::Rect(310, 260, 100, 50), 60, 10 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("arcW_gt_w_div_2") ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with arcH > h/2
TEST_F( SurfaceFillRoundRect, arcH_gt_h_div_2 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->fillRoundRect( canvas::Rect(310, 260, 100, 50), 10, 35 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("arcH_gt_h_div_2") ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with arcW > w.
TEST_F( SurfaceFillRoundRect, arcW_gt_w ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->fillRoundRect( canvas::Rect(310, 260, 100, 50), 200, 25 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("arcW_gt_w") ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with arcH > h.
TEST_F( SurfaceFillRoundRect, arcH_gt_h ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->fillRoundRect( canvas::Rect(310, 260, 100, 50), 50, 100 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("arcH_gt_h") ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with arcH > h and arcW > w.
TEST_F( SurfaceFillRoundRect, arcH_gt_h_and_arcW_gt_w ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->fillRoundRect( canvas::Rect(310, 260, 100, 50), 200, 100 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("arcH_gt_h_and_arcW_gt_w") ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with w = 0.
TEST_F( SurfaceFillRoundRect, w_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->fillRoundRect( canvas::Rect(310, 260, 0, 50), 10, 10 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with h= 0.
TEST_F( SurfaceFillRoundRect, h_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->fillRoundRect( canvas::Rect(310, 260, 100, 0), 10, 10 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with h = 0 and w = 0.
TEST_F( SurfaceFillRoundRect, h_0_and_w_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->fillRoundRect( canvas::Rect(310, 260, 0, 0), 10, 10 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), "nothing" ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with the same width of the surface.
TEST_F( SurfaceFillRoundRect, same_width_surface) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->fillRoundRect( canvas::Rect(0, 238, 720, 100), 25, 25 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("same_width_surface") ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with the same height of the surface.
TEST_F( SurfaceFillRoundRect, same_height_surface) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->fillRoundRect( canvas::Rect(310, 0, 100, 576), 25, 25 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("same_height_surface") ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with the size of the surface.
TEST_F( SurfaceFillRoundRect, same_size_surface ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_TRUE( s->fillRoundRect( canvas::Rect(0, 0, 720, 576), 25, 25 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("same_size_surface") ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle completely out of the surface (top)
TEST_F( SurfaceFillRoundRect, completely_out_top ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->fillRoundRect( canvas::Rect(310, -50, 100, 50), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle completely out of the surface (bottom)
TEST_F( SurfaceFillRoundRect, completely_out_bottom ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->fillRoundRect( canvas::Rect(310, 576, 100, 50), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle completely out of the surface (left)
TEST_F( SurfaceFillRoundRect, completely_out_left ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->fillRoundRect( canvas::Rect(-100, 263, 100, 50), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle completely out of the surface (right)
TEST_F( SurfaceFillRoundRect, completely_out_right ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->fillRoundRect( canvas::Rect(720, 263, 100, 50), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with two vertices out of the surface (top)
TEST_F( SurfaceFillRoundRect, two_vertices_out_top ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->fillRoundRect( canvas::Rect(310, -1, 100, 50), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with two vertices out of the surface (bottom)
TEST_F( SurfaceFillRoundRect, two_vertices_out_bottom ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->fillRoundRect( canvas::Rect(310, 527, 100, 50), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with two vertices out of the surface (left)
TEST_F( SurfaceFillRoundRect, two_vertices_out_left ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->fillRoundRect( canvas::Rect(-1, 263, 100, 50), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with two vertices out of the surface (right)
TEST_F( SurfaceFillRoundRect, two_vertices_out ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->fillRoundRect( canvas::Rect(621, 263, 100, 50), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with three vertices out of the surface (top-left)
TEST_F( SurfaceFillRoundRect, three_vertices_out_top_left ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->fillRoundRect( canvas::Rect(-1, -1, 100, 50), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with three vertices out of the surface (top-right)
TEST_F( SurfaceFillRoundRect, three_vertices_out_top_right ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->fillRoundRect( canvas::Rect(621, 0, 100, 50), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with three vertices out of the surface (bottom-left)
TEST_F( SurfaceFillRoundRect, three_vertices_out_bottom_left ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->fillRoundRect( canvas::Rect(-1, 576, 100, 50), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with three vertices out of the surface (bottom-right)
TEST_F( SurfaceFillRoundRect, three_vertices_out_bottom_right ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->fillRoundRect( canvas::Rect(621, 576, 100, 50), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with four vertices out of the surface that crosses the surface horizontally
TEST_F( SurfaceFillRoundRect, four_vertices_out_crossing_surface_horizontally ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->fillRoundRect( canvas::Rect(-1, 238, 722, 100), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with four vertices out of the surface that crosses the surface vertically
TEST_F( SurfaceFillRoundRect, four_vertices_out_crossing_surface_vertically ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->fillRoundRect( canvas::Rect(310, -1, 100, 578), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with w < 0.
TEST_F( SurfaceFillRoundRect, w_lt_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->fillRoundRect( canvas::Rect(310, 260, -1, 50), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with h < 0.
TEST_F( SurfaceFillRoundRect, h_lt_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->fillRoundRect( canvas::Rect(310, 260, 100, -1), 10, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with arcW < 0
TEST_F( SurfaceFillRoundRect, arcW_lt_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->fillRoundRect( canvas::Rect(310, 260, 100, 50), -1, 10 ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with arcH < 0
TEST_F( SurfaceFillRoundRect, arcH_lt_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->fillRoundRect( canvas::Rect(310, 260, 100, 50), 10, -1 ) );

	getCanvas()->destroy( s );
}

// Draw a fill rounded rectangle with arcW < 0 and arcH < 0
TEST_F( SurfaceFillRoundRect, arcW_and_arcH_lt_0 ) {
	canvas::Surface* s = getCanvas()->createSurface(canvas::Rect(0, 0, 720, 576));

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	s->setColor(canvas::Color(255, 0, 0));
	ASSERT_FALSE( s->fillRoundRect( canvas::Rect(310, 260, 100, 50), -1, -1 ) );

	getCanvas()->destroy( s );
}


// Fill a round rect with alpha on the middle of the surface .
TEST_F( SurfaceFillRoundRect, round_rect_with_alpha_and_solid_background ) {
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
    ASSERT_TRUE( s->fillRoundRect( r, 20, 20 ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "round_rect_with_alpha_and_solid_background" ) ) );

    getCanvas()->destroy( s );
}

// Fill a round rect with alpha on the middle of the surface .
TEST_F( SurfaceFillRoundRect, round_rect_with_alpha_and_transparent_background ) {
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
    ASSERT_TRUE( s->fillRoundRect( r, 20, 20 ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath( "round_rect_with_alpha_and_transparent_background" ) ) );

    getCanvas()->destroy( s );
}

// Fill a full transparent round rect over a solid background.
TEST_F( SurfaceFillRoundRect, full_transparent_over_solid_background ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    s->setColor( canvas::Color(0, 0, 0) );
    ASSERT_TRUE( s->fillRect(canvas::Rect(0, 0, 720, 576)) );

    s->setColor( canvas::Color(0, 255, 0, 0) );

    canvas::Rect r(160, 188, 400, 200);
    ASSERT_TRUE( s->fillRoundRect( r, 20, 20 ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), "nothing_black_background" ) );

    getCanvas()->destroy( s );
}

// Fill a full transparent round rect over a transparent background.
TEST_F( SurfaceFillRoundRect, full_transparent_over_transparent_background ) {
    canvas::Rect rect(0, 0, 720, 576);
    canvas::Surface* s = getCanvas()->createSurface(rect);

    ASSERT_TRUE( s != NULL );
    s->autoFlush(true);

    s->setColor( canvas::Color(0, 255, 0, 0) );

    canvas::Rect r(160, 188, 400, 200);
    ASSERT_TRUE( s->fillRoundRect( r, 20, 20 ) );

    ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("full_transparent_over_transparent_background") ) );

    getCanvas()->destroy( s );
}

// Test markDirty after drawing a filled round rect
TEST_F( SurfaceFillRoundRect, markDirty_fillRoundRect ) {
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
				 r5(70,70,1,10),
				 r6(90,10,10,5),
				 r7(110,30,5,10),
				 r8(130,50,10,1),
				 r9(150,70,1,10);

	s->fillRoundRect( r1, 0, 0 );   // Point
	s->fillRoundRect( r2, 0, 0 );   // Rect with w > h
	s->fillRoundRect( r3, 0, 0 );   // Rect with h > w
	s->fillRoundRect( r4, 0, 0 );   // Rect with h = 1
	s->fillRoundRect( r5, 0, 0 );   // Rect with w = 1
	s->fillRoundRect( r6, 10, 10 ); // Rect with w > h with arcW and arcH
	s->fillRoundRect( r7, 10, 10 ); // Rect with h > w with arcW and arcH
	s->fillRoundRect( r8, 10, 10 ); // Rect with h = 1 with arcW and arcH
	s->fillRoundRect( r9, 10, 10 ); // Rect with w = 1 with arcW and arcH

	std::vector<canvas::Rect> dirtyRegions = getCanvas()->dirtyRegions();

	ASSERT_FALSE( dirtyRegions.empty() );
	ASSERT_TRUE( dirtyRegions.size() == 9 );

	ASSERT_TRUE( dirtyRegions.at(0) == r1 );
	ASSERT_TRUE( dirtyRegions.at(1) == r2 );
	ASSERT_TRUE( dirtyRegions.at(2) == r3 );
	ASSERT_TRUE( dirtyRegions.at(3) == r4 );
	ASSERT_TRUE( dirtyRegions.at(4) == r5 );
	ASSERT_TRUE( dirtyRegions.at(5) == r6 );
	ASSERT_TRUE( dirtyRegions.at(6) == r7 );
	ASSERT_TRUE( dirtyRegions.at(7) == r8 );
	ASSERT_TRUE( dirtyRegions.at(8) == r9 );

	getCanvas()->destroy( s );
}
