# enable warnings
if(NOT MSVC)
	add_compile_options(-Werror)
else()
	add_compile_options(/WX)
endif()
