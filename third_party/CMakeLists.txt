include(CheckCXXSymbolExists)

check_cxx_symbol_exists(std::ranges::views::chunk "ranges" HAVE_RANGES_VIEWS_CHUNK)
if(NOT HAVE_RANGES_VIEWS_CHUNK)
	add_subdirectory(range-v3)
endif()
