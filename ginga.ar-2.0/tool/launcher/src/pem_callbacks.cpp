#include "pem_callbacks.h"

/******************************************************************************
 *                                                                            *
 *                          Implementing listeners                            *
 *                                                                            *
 ******************************************************************************/
/**
 * Fetch files from Carousel
 */
int onFetchFilesFromCarousel ( const std::string &url ) {
	std::string locator ( "file://" );
	std::string new_url;

	int result = FAILURE;

	PEM_LOG_DEBUG( "Fetching [%s]...", url.c_str ( ) );

	// If the URL starts with 'file://', remove it!
	// Platform does not know 'file://'...
	if ( url.find ( locator ) != std::string::npos ) {
		size_t p = locator.length ( );
		new_url = url.substr ( p );
	}
	else {
		new_url = url;
	}

	// If there're multiple and sequenced '/', they should be removed.
	// Only one '/' is acceptable.
	if ( new_url.find ( "http://" ) == std::string::npos && new_url.find ( "https://" ) == std::string::npos && new_url.find ( "rtsp://" ) == std::string::npos && new_url.find ( "rtp://" ) == std::string::npos) {
		new_url.erase ( std::unique ( new_url.begin ( ), new_url.end ( ), both_slashes ), new_url.end ( ) );
	}	

	// Check if file has already downloaded to '/tmp' directory
	result = PEM_PORT_FS_fileExist ( ( SINT8 * ) new_url.c_str ( ) );
	if ( result != SUCCESS ) { /* File does not exist in '/tmp' directory! */

		// Check if file exist in OC
		result = PORT_N2TV_ocFileExist ( ( SINT8 * ) new_url.c_str ( ) );
		if ( result == SUCCESS ) { /* File exists in OC! */
			// Copy the file to the '/tmp' directory
			result = PORT_N2TV_copyOCFile ( ( SINT8 * ) new_url.c_str ( ) );
		}

	}
	else {
		PEM_LOG_DEBUG( "[%s] was opened!", new_url.c_str() );
	}

	PEM_LOG_DEBUG( "Loading [%s] done with result [%s]", new_url.c_str ( ), (result == SUCCESS ? "SUCCESS" : "FAILURE") );
	return result;
}

/**
 * Load the main file that contains the Entry Point
 */
int onLoadMainFile ( const std::string &filename ) {
	int result = FAILURE;

	PEM_LOG_DEBUG( "Loading [%s]...", filename.c_str ( ) );

	// Check if file has already downloaded to '/tmp' directory
	result = PEM_PORT_FS_fileExist ( ( SINT8 * ) filename.c_str ( ) );
	if ( result != SUCCESS ) { /* File does not exist in '/tmp' directory! */

		// Check if file exist in OC
		result = PORT_N2TV_ocFileExist ( ( SINT8 * ) filename.c_str ( ) );
		if ( result == SUCCESS ) { /* File exists in OC! */
			// Copy the file to the '/tmp' directory
			result = PORT_N2TV_copyOCFile ( ( SINT8 * ) filename.c_str ( ) );
		}

	}
	else {
		PEM_LOG_DEBUG( "[%s] was opened!", filename.c_str() );
	}

	PEM_LOG_DEBUG( "Loading [%s] done with result [%s]", filename.c_str ( ), (result == SUCCESS ? "SUCCESS" : "FAILURE") );
	return result;
}

/**
 *
 */
int onBeforeStart ( AppInfo * appInfo ) {
	PEM_LOG_DEBUG( "Will start application" );
	appInfo->setStartControl ( 1 );          //TODO: DEFINE STATES LIKE STARTED, DESTROYED, PAUSED
	return 0;
}

/**
 *
 */
int onStoppedApp ( AppInfo * appInfo ) {
	int result = -1;
	PEM_LOG_DEBUG( "Stopped Application" );
	result = DETACH( appInfo );
	if ( result != SUCCESS ) {
		PEM_LOG_ERROR( "App could not detach" );
	}
	PORT_N2TV_notifyAppState ( appInfo->getOrgID ( ), appInfo->getAppID ( ), APP_DESTROYED );
	appInfo->setStartControl ( 0 );
	appInfo->setAttachUniqueKey ( -1 /* Invalid Value! */);
	PEM_LOG_DEBUG( "Done" );
	return 0;
}

/**/

int onGetNativeSystemProperty ( char * key, char * value ) {
	int ret = -1;
	ret = PORT_N2TV_getNativeSystemProperty ( ( SINT8* ) key, ( SINT8* ) value );
	return ret;
}

int onRequestUserProperty ( char */*key*/, char */*value*/) {
	int ret = -1;

#if 0
	ret = PORT_N2TV_requestUserProperties(UINT8 theCommand,
			const UINT8 *szTheFilter,
			UINT8 nSizeOfTheFilter,
			UINT8 PropertyValue[],
			UINT8 *nSizeOfPropertyValue,
			const UINT8 *szNewValue,
			UINT8 nSizeOfValue);
#endif

	return ret;
}

int onGetProperty ( const std::string &key, std::string */*value*/) {
	char lvalue [ USER_PROPERTY_VALUE_MAX_LENGTH ];
	UINT8 size;
	int ret = -1;

	ret = PORT_N2TV_requestUserProperties ( READ_PROPERTY, ( const UINT8* ) key.c_str ( ), key.length ( ), ( UINT8* ) lvalue,
			&size, NULL, 0 );
	return ret;
}

int onSetProperty ( const std::string &key, const std::string &value ) {
	char lvalue [ USER_PROPERTY_VALUE_MAX_LENGTH ];
	UINT8 size;

	strncpy ( lvalue, value.c_str ( ), sizeof ( lvalue ) );
	size = value.length ( );

	int ret = -1;
	ret = PORT_N2TV_requestUserProperties ( WRITE_PROPERTY, ( const UINT8* ) key.c_str ( ), key.length ( ), NULL, NULL,
			( const UINT8* ) lvalue, size );
	return ret;
}

int onDelProperty ( const std::string &key ) {
	int ret = -1;
	ret = PORT_N2TV_requestUserProperties ( REMOVE_PROPERTY, ( const UINT8* ) key.c_str ( ), key.length ( ), NULL, NULL, NULL,
			0 );
	return ret;
}

int onReserveKeys ( int /*key*/) {
	/*
	 if (appInfo->getStartControl()){
	 appInfo->setStartControl (0);
	 PORT_N2TV_notifyAppState(appInfo->getOrgID(), appInfo->getAppID(), APP_ACTIVE);
	 }
	 */
	if ( bpe.appInfo.getStartControl ( ) ) {
		bpe.appInfo.setStartControl ( 0 );
		PORT_N2TV_notifyAppState ( bpe.appInfo.getOrgID ( ), bpe.appInfo.getAppID ( ), APP_ACTIVE );
	}

	return 0;
}

/**
 * numeric_keys = "0, 1, 2, 3, 4, 5, 6, 7, 8, 9"
 * interactive_keys = "clored_keys, selection_keys, cursor_keys"
 * colored_keys = "RED, GREEN, YELLOW, BLUE"
 * selection_keys = "OK, BACK, EXIT"
 * cursor_keys = "CURSOR_LEFT, CURSOR_RIGHT, CURSOR_DOWN, CURSOR_UP
 */
int onRequestKeySet ( const std::string &name, const std::string &value ) {

	KEY_MASK_INFO cMask = ( KEY_MASK_INFO ) ( GINGA_KEY_ARROW | GINGA_KEY_CONFIRM | GINGA_KEY_COLOURED | GINGA_KEY_BACK
			| GINGA_KEY_EXIT );

	PEM_LOG_DEBUG( "Name = \"%s\", Value = \"%s\"", name.c_str ( ), value.c_str ( ) );

	if ( name != "channel.keyCapture" ) {
		PEM_LOG_DEBUG( "Property is NULL. Name = [%s]", name.c_str ( ) );
		return FALSE;
	}

	if ( value.find ( "numeric_keys" ) != std::string::npos ) {
		cMask = ( KEY_MASK_INFO ) ( cMask | GINGA_KEY_NUMERIC );
	}

	if ( value.find ( "true" ) != std::string::npos ) {
		cMask = ( KEY_MASK_INFO ) ( cMask | GINGA_KEY_NUMERIC );
	}

	PEM_LOG_DEBUG( "BEGIN - keyMask = [0x%x]", cMask );
	PORT_N2TV_registerKeySet ( ( UINT32 ) cMask );
	bpe.notifyRequestKeySet(value);
	PEM_LOG_DEBUG( "END" );
	return TRUE;
}

