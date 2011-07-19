if WITH_GUI
if WITH_ODBC
SUBDIRS = saga_api saga_odbc saga_gdi saga_cmd saga_gui
DIST_SUBDIRS = saga_api saga_odbc saga_gdi saga_cmd saga_gui
else
SUBDIRS = saga_api saga_gdi saga_cmd saga_gui
DIST_SUBDIRS = saga_api saga_gdi saga_cmd saga_gui
endif
else
if WITH_ODBC
SUBDIRS = saga_api saga_odbc saga_cmd
DIST_SUBDIRS = saga_api saga_odbc saga_cmd
else
SUBDIRS = saga_api saga_cmd
DIST_SUBDIRS = saga_api saga_cmd
endif
endif

