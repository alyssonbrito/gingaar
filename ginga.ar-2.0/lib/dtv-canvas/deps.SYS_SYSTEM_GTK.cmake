CHECK_OPTION(SYS_CANVAS_USE_CAIRO 1)

if($ENV{FRAMEWORK} STREQUAL gtk3)
    Find_Package( GTK3 REQUIRED )
    ADD_DEPENDENTS( GTK3 )
else()
    Find_Package( Gtk2 REQUIRED )
    ADD_DEPENDENTS( GTK2 )
endif()

find_package( MNG REQUIRED )
ADD_DEPENDENTS( MNG )
