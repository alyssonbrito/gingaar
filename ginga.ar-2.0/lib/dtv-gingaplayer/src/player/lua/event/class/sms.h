#pragma once

#include "../event.h"

namespace player {
namespace event {
namespace sms {

//	Event class methods
int postEvent( lua_State *st, bool isOut, int eventPos );
void dispatchSMSReceived( Module *module, const std::string &from, const std::string &port, const std::string &value );

}
}
}

