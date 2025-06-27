cmake_minimum_required(VERSION 3.31)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if ((CMAKE_MAJOR_VERSION EQUAL 3) AND (CMAKE_MINOR_VERSION EQUAL 31))
	set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
else()
	set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")
endif()

project(uuid_lua LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_SHARED_MODULE_PREFIX "")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# C++ 20 required for std::span and C++ 23 for std::chunk
set(CMAKE_CXX_STANDARD 23)

option(BUILD_CXX_MODULE "Build with CXX Modules support" OFF)

find_package(Lua REQUIRED)
if(Lua_FOUND AND NOT TARGET Lua::Lua)
	add_library(Lua::Lua INTERFACE IMPORTED)
	set_target_properties(
		Lua::Lua
		PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES "${LUA_INCLUDE_DIR}"
		INTERFACE_LINK_LIBRARIES "${LUA_LIBRARIES}"
	)
endif()

if(BUILD_CXX_MODULE)
	set(CXX_MODULES_SUPPORTED True)
	add_compile_definitions(CXX_MODULES_SUPPORT)
endif()

if (CXX_MODULES_SUPPORTED)
	if(23 IN_LIST CMAKE_CXX_COMPILER_IMPORT_STD)
		# C++ modules import std support
		set(CMAKE_CXX_MODULE_STD 1)
		add_compile_definitions(IMPORT_STD)
		set(IMPORT_STD True)
	else()
		set(IMPORT_STD False)
	endif()
	message(STATUS "Building with import std: ${IMPORT_STD}")
endif()

if(APPLE)
	set(CMAKE_MACOSX_RPATH TRUE)
	set(CMAKE_INSTALL_RPATH "@loader_path")
elseif(UNIX)
	set(CMAKE_INSTALL_RPATH "$ORIGIN")
endif()

add_subdirectory(third_party)
add_subdirectory(src)
