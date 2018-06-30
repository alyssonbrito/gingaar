ADD_BOOST_COMPONENTS( system filesystem thread signals)

ADD_INTERNAL_COMPONENTS( canvas util pem-tv-util )

add_definitions(-DNO_DUAL_VIDEO_DECODER)

find_package( Lua51 REQUIRED )
find_package( CURL REQUIRED )

ADD_DEPENDENTS( LUA CURL )
