#
# $Id: Makefile.am 1277 2011-12-22 15:48:59Z reklov_w $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version 
pkglib_LTLIBRARIES = libclimate_tools.la
libclimate_tools_la_SOURCES =\
air_pressure.cpp\
bioclimatic_vars.cpp\
climate_classification.cpp\
climate_tools.cpp\
cloud_overlap.cpp\
daily_sun.cpp\
evapotranspiration.cpp\
frost_change_frequency.cpp\
grid_levels_interpolation.cpp\
growing_degree_days.cpp\
land_surface_temperature.cpp\
milankovic.cpp\
phenips.cpp\
MLB_Interface.cpp\
soil_water_balance.cpp\
snow_cover.cpp\
temperature_lapse_rates.cpp\
thermal_belts.cpp\
treeline.cpp\
windeffect_correction.cpp\
air_pressure.h\
bioclimatic_vars.h\
climate_classification.h\
climate_tools.h\
cloud_overlap.h\
daily_sun.h\
evapotranspiration.h\
frost_change_frequency.h\
grid_levels_interpolation.h\
growing_degree_days.h\
land_surface_temperature.h\
milankovic.h\
phenips.h\
MLB_Interface.h\
soil_water_balance.h\
snow_cover.h\
temperature_lapse_rates.h\
thermal_belts.h\
treeline.h\
windeffect_correction.h

libclimate_tools_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

