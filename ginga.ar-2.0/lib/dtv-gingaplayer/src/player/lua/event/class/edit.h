#pragma once

#include "../event.h"

namespace player {
namespace event {
namespace edit {

//	Event class methods
int postEvent( lua_State *st, bool isOut, int eventPos );

}
}
}

