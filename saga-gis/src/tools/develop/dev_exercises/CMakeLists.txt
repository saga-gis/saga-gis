option(WITH_EXCERCISES "Check to create build targets for SAGA development tools" OFF)

if(WITH_EXCERCISES)
	project(dev_exercises)

	include(../../CMakePluginTemplate.cmake) # load common tool template
endif()
