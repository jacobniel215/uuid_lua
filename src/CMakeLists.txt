include(TargetLuaModule)
include(CreateCXXModule)

if(CXX_MODULES_SUPPORTED)
	create_cxx_module(md5-module md5.cc md5_impl.cc)

	set(module_interfaces "")
	list(APPEND module_interfaces uuid.cc uuid_object.cc uuid_uuid3.cc)
	create_cxx_module(uuid-module "${module_interfaces}")

	set(uuid_module_deps "")
	list(APPEND uuid_module_deps "$<LINK_LIBRARY:WHOLE_ARCHIVE,md5-module>")

	add_library(uuid_lua MODULE module.cc)

	if(NOT HAVE_RANGES_VIEWS_CHUNK)
		list(APPEND uuid_module_deps range-v3::range-v3)
	else()
		add_compile_definitions(-DHAVE_RANGES_VIEWS_CHUNK)
	endif()

	target_link_libraries(uuid-module ${uuid_module_deps})

	target_lua_module(uuid_lua "$<LINK_LIBRARY:WHOLE_ARCHIVE,uuid-module>")
else()
	add_library(uuid_lua MODULE module.cc uuid_object.cc uuid_uuid3.cc md5_impl.cc)

	if(NOT HAVE_RANGES_VIEWS_CHUNK)
		target_lua_module(uuid_lua range-v3::range-v3)
	else()
		add_compile_definitions(-DHAVE_RANGES_VIEWS_CHUNK)
		target_lua_module(uuid_lua)
	endif()

	target_include_directories(uuid_lua PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif()
