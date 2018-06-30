ADD_BOOST_COMPONENTS( system filesystem thread signals math_tr1 )

ADD_INTERNAL_COMPONENTS( gingaplayer canvas util ncl30-presenter ncl30-converter ncl30 )

# I have to put "pem-tv-agent" component to force the build script to generate this library...
ADD_INTERNAL_COMPONENTS( pem-tv-agent pem-tv-util )

find_package( Lua51 REQUIRED )
find_package( CURL REQUIRED )
find_package( MNG REQUIRED )
find_package( XercesC REQUIRED )

if($ENV{FRAMEWORK} STREQUAL gtk3)
    Find_Package( GTK3 REQUIRED )
    Find_Package( Gal REQUIRED )
    ADD_DEPENDENTS( GTK3 )
else()
    Find_Package( Gtk2 REQUIRED )
    ADD_DEPENDENTS( GTK2 )
endif()

find_package( GDK-PixBuf REQUIRED )
find_package( Glib REQUIRED )
find_package( Pango REQUIRED )
find_package( Cairo REQUIRED )
find_package( Freetype2 REQUIRED )
find_package( Fontconfig REQUIRED )
Find_Package( Webkit REQUIRED )
Find_Package( LibSoup2 REQUIRED )

Find_Package( EV REQUIRED )

ADD_DEPENDENTS( LUA CURL XERCESC GTK3 )
ADD_DEPENDENTS( GDK-PIXBUF GLIB PANGO CAIRO FREETYPE2 FONTCONFIG )
if($ENV{FRAMEWORK} STREQUAL gtk3)
    ADD_DEPENDENTS( Gal )
endif()
ADD_DEPENDENTS( LIBSOUP2 WEBKIT )
ADD_DEPENDENTS( EV )
ADD_DEPENDENTS( MNG )


