#include <gtest/gtest.h>

#ifdef _WIN32
#	ifdef _DEBUG
#		pragma comment(lib,"gtestd.lib")
#	else
#		pragma comment(lib,"gtest.lib")
#	endif // _DEBUG
#endif

#include <ginga_api.h>

LGESP_INTEGRATION_STUB_IMPL();


int main( int argc, char **argv ) {
	int result;
	testing::InitGoogleTest( &argc, argv );
	result = RUN_ALL_TESTS();
	return result;
}

