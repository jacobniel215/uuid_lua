#include "lua.hpp"

#ifdef CXX_MODULES_SUPPORT

#ifndef IMPORT_STD
#include <stdexcept>
#endif

#ifdef IMPORT_STD
import std;
#endif

import uuid;

#else
#include <stdexcept>
#include "uuid.hh"
#include "uuid3.hh"
#endif

namespace {
auto get_uuid(lua_State* lua) -> int {
	const char* name = lua_tostring(lua, 1);
	const char* ns = luaL_optstring(lua, 2, "51C3AF2C-0C43-410E-9F1B-CA01FF66333E");

	try {
		const UUID id = UUIDv3::GetUUID(name, UUID(ns));
		lua_pushstring(lua, id.to_string().c_str());
		return 1;
	} catch (std::invalid_argument& err) {
		return luaL_error(lua, err.what());
	}
	return 1;
}

luaL_Reg const uuid_lualib[] { { "getUUID", get_uuid }, { nullptr, nullptr } };
}

extern "C" {
LUALIB_API auto luaopen_uuid_lua(lua_State* lua) -> int {
	luaL_newlib(lua, uuid_lualib);
	return 1;
}
}
