# enable warnings
if(NOT MSVC)
	add_compile_options(-Wall)# -Wextra -pedantic)
else()
	add_compile_options(/W4)
endif()
