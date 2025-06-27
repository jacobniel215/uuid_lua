function(create_cxx_module library_name module_interface)
	add_library(${library_name} STATIC ${ARGN})
	target_sources(${library_name} PUBLIC
		FILE_SET CXX_MODULES FILES "${module_interface}"
	)

	target_include_directories(${library_name} PRIVATE ${LUA_INCLUDE_DIR})
endfunction()
