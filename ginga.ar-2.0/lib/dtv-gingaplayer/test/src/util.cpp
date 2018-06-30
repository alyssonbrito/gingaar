#include "util.h"
#include "../../src/device.h"
#include "../../src/system.h"
#include <canvas/canvas.h>
#include <util/mcr.h>
#include <boost/filesystem.hpp>

namespace util {

namespace fs = boost::filesystem;

std::string getDepot() {
		const char *root=getenv( "DEPOT" );
		return root ? root : "/";
	}

std::string getTestRoot() {
	fs::path rootPath(getDepot());
	rootPath /= "lib";
	rootPath /= "dtv-gingaplayer";
	rootPath /= "test";
	return rootPath.string();
}

std::string getImageName( const std::string &name ) {
	fs::path imagesPath( util::getTestRoot() );
	imagesPath /= "images";
	imagesPath /= name;
	return imagesPath.string();
}

std::string getExpectedPath( const std::string &name, const std::string &canvas="" ) {
	fs::path expectedPath( util::getTestRoot() );
	expectedPath /= "images";
	expectedPath /= "expected";
	if (!canvas.empty()) {
		expectedPath /= canvas;
	}
	expectedPath /= name;
	return expectedPath.string();
}

bool compareImagesFromPath( canvas::Canvas *c, const std::string &file ) {
	if (!fs::exists( file )) {
 		printf( "[util] Creating image file: %s\n", file.c_str() );
 		c->saveAsImage( file );
		return false;
	}
	return c->equalsImage( file );
}

bool compareImages( canvas::Canvas *c, const std::string &file ) {
	std::string tmp = getExpectedPath( file ) + ".png";
	if (!fs::exists( tmp )) {
		tmp = getExpectedPath( file, c->name() ) + ".png" ;
	}
 	return compareImagesFromPath( c, tmp );
}

}

