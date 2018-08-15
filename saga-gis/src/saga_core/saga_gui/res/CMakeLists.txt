message(STATUS "subfolder: ${PROJECT_NAME}/res")

# desktop file
install(FILES saga.desktop DESTINATION share/applications)

# pixmaps file
install(FILES saga.png DESTINATION share/pixmaps)

# sized icons
set(ICON_SIZES 8 16 22 24 32 48 64 72 80 96 128 192)

foreach(size ${ICON_SIZES})
	#message(STATUS "icon size: ${size}")
	set(_icon_source "saga_icon_${size}.png")
	set(_icon_target "share/icons/hicolor/${size}x${size}/apps/")
	install(FILES ${_icon_source}
		DESTINATION ${_icon_target}
		RENAME saga.png
	)
endforeach()

set(size 256)
set(_icon_target "share/icons/hicolor/${size}x${size}/apps/")
install(FILES saga.png
	DESTINATION ${_icon_target}
)
# clean variables
set(size)
set(_icon_source)
set(_icon_target)

# appdata
install(FILES org.saga-gis.saga-gui.appdata.xml DESTINATION share/metainfo)

# tip, srs and dic
install(FILES saga_tip.txt DESTINATION share/saga)
install(FILES saga_srs.txt DESTINATION share/saga RENAME saga_prj.srs)
install(FILES saga_dic.txt DESTINATION share/saga RENAME saga_prj.dic)

# cosmetic: add to custom target to show up in IDE
add_custom_target(saga-gui-res-otherfiles SOURCES
	saga.desktop
	org.saga-gis.saga-gui.appdata.xml
	saga_tip.txt
	saga_srs.txt
	saga_dic.txt
	)

# TODO: dist-hook
