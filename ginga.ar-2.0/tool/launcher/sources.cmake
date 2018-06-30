LIST(APPEND DVC_NAMES dvc )
FIND_LIBRARY(DVC_LIBRARY NAMES ${DVC_NAMES} )

SET( ${PROJECT_NAME}_HEADER_FILES
    include/AppInfo.h
    include/BrowserPresentationEnginer.h
    include/browser_agent.h
    include/browser_port_mw2tv.h
    include/browser_port_tv2mw.h
    include/configuration.h
    include/gingancl_bridge.h
    include/pem_bind_references.h
    include/pem_callbacks.h
    include/pem_types.h
    include/pem_util.h
    include/pem_worker_key.h
    include/wqueue.h
)

SET( ${PROJECT_NAME}_SOURCE_FILES
    src/AppInfo.cpp
    src/BrowserPresentationEngine.cpp
    src/browser_agent.cpp
    src/browser_port_mw2tv.cpp
    src/browser_port_tv2mw.cpp
    src/gingancl_bridge.cpp
    src/pem_bind_references.cpp
    src/pem_callbacks.cpp
    src/pem_launcher.cpp
    src/pem_util.cpp
    src/Thread.cpp
)

IF(NOT ${DVC_LIBRARY} MATCHES NOTFOUND)
	MESSAGE("DVC_LIBRARY = ${DVC_LIBRARY}")
	LIST(APPEND GINGA_LINK_LIBRARIES dvc )
ENDIF(NOT ${DVC_LIBRARY} MATCHES NOTFOUND)


