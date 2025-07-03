include(TargetLuaModule)
include(CreateCXXModule)

if(CXX_MODULES_SUPPORTED)
	set(module_interfaces "")
	list(APPEND module_interfaces uuid.cc uuid_object.cc uuid_uuid3.cc)
	create_cxx_module(uuid-module "${module_interfaces}")

	set(uuid_module_deps "")
	list(APPEND uuid_module_deps md5::md5)

	add_library(uuid_lua MODULE module.cc)

	if(NOT HAVE_RANGES_VIEWS_CHUNK)
		list(APPEND uuid_module_deps range-v3::range-v3)
	else()
		add_compile_definitions(-DHAVE_RANGES_VIEWS_CHUNK)
	endif()

	target_link_libraries(uuid-module ${uuid_module_deps})

	target_lua_module(uuid_lua "$<LINK_LIBRARY:WHOLE_ARCHIVE,uuid-module>")
else()
	add_library(uuid_lua MODULE module.cc uuid_object.cc uuid_uuid3.cc)
	set(module_dependencies "")
	list(APPEND module_dependencies md5::md5)

	if(NOT HAVE_RANGES_VIEWS_CHUNK)
		list(APPEND module_dependencies range-v3::range-v3)
	else()
		add_compile_definitions(-DHAVE_RANGES_VIEWS_CHUNK)
	endif()

	target_lua_module(uuid_lua ${module_dependencies})

	target_include_directories(uuid_lua PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif()
