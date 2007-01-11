if DEBUG
DBGFLAGS = -g -DDEBUG
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS) $(DBGFLAGS)
AM_LDFLAGS         = -shared $(DEP_LFLG)
pkglib_LTLIBRARIES = libsim_hydrology.la
libsim_hydrology_la_SOURCES =\
DVWK_SoilMoisture.cpp\
idw.cpp\
KinWav_D8.cpp\
MLB_Interface.cpp\
topmodel.cpp\
topmodel_values.cpp\
WaterRetentionCapacity.cpp\
DVWK_SoilMoisture.h\
idw.h\
KinWav_D8.h\
MLB_Interface.h\
topmodel.h\
topmodel_values.h\
WaterRetentionCapacity.h

libsim_hydrology_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

