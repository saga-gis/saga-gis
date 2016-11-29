project(geotrans)

# use all local c files
file(GLOB header_list *.h)
file(GLOB source_list *.cpp *.c)
# create geotrans library
add_library(${PROJECT_NAME} STATIC ${header_list} ${source_list})

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

if(NOT MSVC) #expect g++ on linux
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS -fPIC)
endif()
