SET( ${PROJECT_NAME}_TEST_HEADER_FILES
)

SET( ${PROJECT_NAME}_TEST_SOURCE_FILES
	${${PROJECT_NAME}_TEST_HEADER_FILES}
	test/src/main.cpp
	test/src/util.cpp
	test/src/player.cpp
	test/src/device.cpp
	test/src/player/timelinetimer.cpp
	test/src/player/input.cpp
	test/src/player/lua/luaplayer.cpp
	test/src/player/lua/attributionEvent.cpp
	test/src/player/lua/presentationEvent.cpp
	test/src/player/image/imageplayer.cpp
	test/src/player/image/fit.cpp
	test/src/player/image/visible.cpp
	test/src/player/image/zindex.cpp
	test/src/player/image/backgroundColor.cpp
	test/src/player/image/opacity.cpp
	test/src/player/image/size.cpp
	test/src/player/image/location.cpp
	test/src/player/image/explicitDuration.cpp
	test/src/player/image/focus.cpp
	test/src/player/sound/soundplayer.cpp
	test/src/player/sound/soundLevel.cpp
	test/src/player/sound/balanceLevel.cpp
	test/src/player/sound/bassLevel.cpp
	test/src/player/sound/trebleLevel.cpp
	test/src/player/video/videoplayer.cpp
	test/src/player/video/location.cpp
	test/src/player/video/zindex.cpp
	test/src/player/html/htmlplayer.cpp
	test/src/player/html/location.cpp
	test/src/player/text/textplayer.cpp
	test/src/player/lua/lua.cpp
	test/src/player/lua/updateSettings.cpp
)
