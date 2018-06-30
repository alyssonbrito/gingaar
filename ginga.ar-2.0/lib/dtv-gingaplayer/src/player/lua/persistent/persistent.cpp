#include "persistent.h"
#include "../lua.h"
#include "../../luaplayer.h"
#include <string>
#include <assert.h>
#include "generated/config.h"
#include <stdio.h>

#define LUA_PERSISTENT "lua_persistent_module"
#define LUA_PERSISTENT_META "lua_persistent_meta"

#define PERSISTENT_FILE       DATA_PREFIX_PATH"/share/ginga.persistent"
#define PERSISTENT_MAX_NODES 100

namespace player {
namespace persistent {

#define ID_CHANNEL 1
#define ID_SHARED  2
#define ID_SERVICE 3

static bool persGetName( lua_State *st, std::string &name ) {
	if (!lua_isstring(st, 2)) {
		return false;
	}
	name = lua_tostring(st, 2);

	int *id = (int*)lua_touserdata(st, 1);
	switch (*id) {
		case ID_CHANNEL:
			name.append("#1");
			break;
		case ID_SHARED:
			name.append("#2");
			break;
		case ID_SERVICE:
			name.append("#3");
			break;
		default:
			return false;
	}
	return true;
}

typedef struct PersistentNode PersistentNode;
struct PersistentNode {
	PersistentNode() {
		prev = NULL;
		next = NULL;
	}

	PersistentNode *prev;
	PersistentNode *next;
	std::string name;
	std::string value;
};
static PersistentNode *_head = NULL;
static PersistentNode *_last = NULL;

static std::map<std::string, PersistentNode*> _persistent;
static bool _persNotInited = true;
static bool _persModified = false;
static int _persCount = 0;

#define BUF_SIZE 1024

static void readStr( FILE *f, char* buf, size_t &bufPos, size_t &bufCount, std::string &str ) {
	size_t l;
	do {
		if (bufPos >= bufCount)	{
			bufCount = fread(buf, sizeof(char), BUF_SIZE, f);
			if (!bufCount) {
				return;
			}
			bufPos = 0;
		}
		l = strlen(buf+bufPos);
		if (l) {
			str.append(buf+bufPos);
			bufPos += l+1;
			if (bufPos <= bufCount) {
				return;
			}
		} else {
			bufPos += 1;
			return;
		}
	} while (!feof(f));
}

static void loadPersistent() {
	_persNotInited = false;
	FILE *f;
	f = fopen(PERSISTENT_FILE, "r");
	if (!f) {
		return;
	}

	size_t bufPos = 0;
	size_t bufCount = 0;
	PersistentNode* cur = NULL;
	std::string name;
	char buf[BUF_SIZE+1];
	buf[BUF_SIZE] = 0;
	while (_persCount < PERSISTENT_MAX_NODES) {
		readStr( f, buf, bufPos, bufCount, name );
		if (name.empty()) {
			break;
		}
		_persCount++;

		PersistentNode* node = new PersistentNode();
		node->name = name;
		readStr( f, buf, bufPos, bufCount, node->value );
		if (cur) {
			cur->next = node;
			node->prev = cur;
		} else {
			_head = node;
		}
		cur = node;
		_persistent[name] = node;
		name.erase();
	}
	_last = cur;
	fclose(f);
}

static void storePersistent() {
	if (!_persModified) {
		return;
	}
	_persModified = false;
	FILE * f;
	f = fopen(PERSISTENT_FILE, "w");
	if (!f) {
		return;
	}
	PersistentNode* node = _head;
	size_t l;
	for (int i=0; (i < _persCount) && node; i++) {
		l = node->name.length();
		fwrite(node->name.c_str(), sizeof(char), l+1, f);
		l = node->value.length();
		fwrite(node->value.c_str(), sizeof(char), l+1, f);
		node = node->next;
	}
	fclose(f);
}

static void putToHead( PersistentNode* node ) {
	if (node == _head) {
		return;
	}
	node->prev->next = node->next;
	if (node->next) {
		node->next->prev = node->prev;
	} else {
		_last = node->prev;
	}
	_head->prev = node;
	node->next = _head;
	node->prev = NULL;
	_head = node;
}

int indexHandler( lua_State *st ) {
	std::string value = "";
	std::string name;
	if (persGetName( st, name )) {
		if (_persNotInited) {
			loadPersistent();
		}
		std::map<std::string, PersistentNode*>::const_iterator it = _persistent.find(name);
		if (it != _persistent.end()) {
			value = it->second->value;
			putToHead( it->second );
		}
	}
	lua_pushstring(st, value.c_str());
	return 1;
}

int newindexHandler( lua_State *st ) {
	const char *str = lua_tostring(st, 3);
	if (!str) {
		return 0;
	}
	std::string value = str;

	std::string name;
	if (persGetName( st, name )) {
		if (_persNotInited) {
			loadPersistent();
		}
		std::map<std::string, PersistentNode*>::const_iterator it = _persistent.find(name);
		if (it != _persistent.end()) {
			if (it->second->value != value) {
				it->second->value = value;
				_persModified = true;
			}
			putToHead( it->second );
		} else {
			PersistentNode* node;
			if (_persCount < PERSISTENT_MAX_NODES) {
				node = new PersistentNode();
				_persCount++;
				node->next = _head;
				if (_head) {
					_head->prev = node;
				} else {
					_last = node;
				}
				_head = node;
			} else {
				node = _last;
				_persistent.erase(node->name);
				putToHead( node );
			}
			node->name = name;
			node->value = value;
			_persistent[name] = node;
			_persModified = true;
		}
	}

	return 0;
}

static void createObject( lua_State *st, int id ) {
	int *ptr = (int*)lua_newuserdata(st, sizeof(int));
	*ptr = id;
	lua_getfield(st, LUA_REGISTRYINDEX, LUA_PERSISTENT_META);
	lua_setmetatable(st, -2);
}

/*******************************************************************************
 *	Persistent module
 *******************************************************************************/
Module::Module( LuaPlayer *player, lua_State *st ) :
		_player( player ),
		_lua( st )
{
	//	Store module into stack
	lua::storeObject( _lua, this, LUA_PERSISTENT );

	lua_newtable(_lua); // persistent metatable
	lua_pushcfunction(_lua, indexHandler);
	lua_setfield(_lua, -2, "__index");
	lua_pushcfunction(_lua, newindexHandler);
	lua_setfield(_lua, -2, "__newindex");
	lua_setfield(_lua, LUA_REGISTRYINDEX, LUA_PERSISTENT_META); // save persistent metatable

	lua_newtable(_lua);

	createObject(_lua, ID_CHANNEL);
	lua_setfield(_lua, -2, "channel");

	createObject(_lua, ID_SHARED);
	lua_setfield(_lua, -2, "shared");

	createObject(_lua, ID_SERVICE);
	lua_setfield(_lua, -2, "service");

	lua_setfield(_lua, LUA_GLOBALSINDEX, "persistent");
}

Module::~Module() {
}

void Module::stop() {
	storePersistent();
}

Module *Module::get( lua_State *st ) {
	return lua::getObject<Module>( st, LUA_PERSISTENT );
}

}
}
