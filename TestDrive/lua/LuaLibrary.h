#pragma once

#ifndef LUABRIDGEDEMO_LUA_VERSION
  #define LUABRIDGEDEMO_LUA_VERSION 504   // use 5.4.4
#endif

#ifndef LUALIBRARY_SOURCE

#if LUABRIDGEDEMO_LUA_VERSION >= 502
  #include "lua-5.4.6/src/lua.hpp"

#else
  #error "Unknown LUA_VERSION_NUM"

#endif

#endif
