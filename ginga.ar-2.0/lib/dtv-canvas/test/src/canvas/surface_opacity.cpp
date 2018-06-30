#include "surface.h"
#include "../util.h"
#include "../../../src/canvas.h"
#include "../../../src/surface.h"

class SurfaceOpacity : public Surface {
protected:
    std::string getSubDirectory() const { return "Opacity"; }
};

// Set opacity (128) to a surface intersected with other surface with lower zindex.
TEST_F( SurfaceOpacity, half ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "blue.jpg" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);
	s2->autoFlush(true);
	s2->setZIndex(2);
	ASSERT_TRUE( s2->setOpacity( 128 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("half_opacity") ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Set opacity (255) to a surface intersected with other surface with lower zindex.
TEST_F( SurfaceOpacity, full ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "blue.jpg" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);
	s2->autoFlush(true);
	s2->setZIndex(2);
	ASSERT_TRUE( s2->setOpacity( 255 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("full_opacity") ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Set opacity (0) to a surface intersected with other surface with lower zindex.
TEST_F( SurfaceOpacity, zero ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurfaceFromPath( util::getImageName( "blue.jpg" ) );

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);
	s2->autoFlush(true);
	s2->setZIndex(2);
	ASSERT_TRUE( s2->setOpacity( 0 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("zero_opacity") ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

/// Use setOpacity with a translucent value of the surface
/// rendered from an image.
TEST_F( SurfaceOpacity, image_compare ) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	ASSERT_TRUE( s->setOpacity( 128 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("image_with_opacity_128") ) );

	getCanvas()->destroy( s );
}

/// Use setOpacity with a translucent value of the surface
/// rendered from an image with alpha.
TEST_F( SurfaceOpacity, image_withAlpha_compare ) {
	canvas::Surface* s = getCanvas()->createSurfaceFromPath( util::getImageName( "lifia.png" ) );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	getCanvas()->flush();
	ASSERT_TRUE( s->setOpacity( 128 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("image_with_alpha_and_opacity_128") ) );
	getCanvas()->destroy( s );
}

/// Use setOpacity with a translucent value of the top surface rendered from
/// a rectangle with color property.
TEST_F( SurfaceOpacity, background_withImage_compare ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurface(canvas::Rect(420, 300, 100, 100));

	ASSERT_TRUE(s1 && s2);
	s1->autoFlush(true);
	s2->autoFlush(true);
	s2->setColor(canvas::Color(0, 0, 255));
	ASSERT_TRUE( s2->fillRect(canvas::Rect(0,0,100,100)) );
	ASSERT_TRUE( s2->setOpacity( 128 ) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("opacity") ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
}

// Two surfaces with opacity intersected over a background. The surface in the middle is
// totally transparent, the one at the front is partial transparent.
TEST_F( SurfaceOpacity, intersected ) {
	canvas::Surface* s1 = getCanvas()->createSurfaceFromPath( util::getImageName( "image.png" ) );
	canvas::Surface* s2 = getCanvas()->createSurface(canvas::Rect(100, 100, 200, 200));
	canvas::Surface* s3 = getCanvas()->createSurface(canvas::Rect(100, 100, 200, 200));

	ASSERT_TRUE(s1 && s2 && s3);
	s1->autoFlush(true);
	s2->autoFlush(true);
	s3->autoFlush(true);

	s2->setColor(canvas::Color(0, 0, 255));
    ASSERT_TRUE( s2->fillRect(canvas::Rect(0,0,200,200)) );
	s2->setZIndex(2);
	ASSERT_TRUE( s2->setOpacity(0) );

	s3->setColor(canvas::Color(0,255,0));
    ASSERT_TRUE( s3->fillRect(canvas::Rect(0,0,200,200)) );
	s3->setZIndex(3);
	ASSERT_TRUE( s3->setOpacity(128) );

	ASSERT_TRUE( util::compareImages( getCanvas(), getExpectedPath("intersected") ) );

	getCanvas()->destroy( s1 );
	getCanvas()->destroy( s2 );
	getCanvas()->destroy( s3 );
}

// Test markDirty after changing opacity
TEST_F( SurfaceOpacity, markDirty_opacity ) {
	canvas::Rect rect(0,0,100,100);
	canvas::Surface* s = getCanvas()->createSurface( rect );

	ASSERT_TRUE( s != NULL );
	s->autoFlush(true);
	getCanvas()->flush();

	ASSERT_TRUE( s->setOpacity(128) );

	std::vector<canvas::Rect> dirtyRegions = getCanvas()->dirtyRegions();

	ASSERT_FALSE( dirtyRegions.empty() );
	ASSERT_TRUE( dirtyRegions.size() == 1 );

	ASSERT_TRUE( dirtyRegions.at(0) == rect );

	getCanvas()->destroy( s );
}

