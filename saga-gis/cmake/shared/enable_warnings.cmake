# enable warnings
if(NOT MSVC)
	add_compile_options(-Wall)# -Wextra -pedantic)
else()
	add_compile_options(/W3)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()
