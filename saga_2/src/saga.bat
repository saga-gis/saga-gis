@ECHO OFF

REM -----------------------------------
REM Make SAGA using MinGW...
REM -----------------------------------

REM SET MINGW=C:\dev-cpp

PATH=%PATH%;%MINGW%\bin


REM -----------------------------------
REM SAGA location...
REM -----------------------------------

REM SET SAGA=C:\saga_2


REM -----------------------------------
REM SAGA Core...
REM -----------------------------------

SET BIN_SAGA=%SAGA%\bin\saga_mingw
SET OBJ_SAGA=%SAGA%\bin\tmp\saga_mingw

MD %SAGA%\bin
MD %SAGA%\bin\saga_mingw
MD %SAGA%\bin\tmp
MD %SAGA%\bin\tmp\saga_mingw

ECHO ###################################
ECHO SAGA API
ECHO -----------------------------------

SET OBJ_SAGA=%SAGA%\bin\tmp\saga_mingw\saga_api\
SET TMP_SAGA=
MD %OBJ_SAGA%

PUSHD %SAGA%\src\saga_core\saga_api
MOVE %OBJ_SAGA%*.o %TMP_SAGA%
make -fmakefile.mingw
MOVE %TMP_SAGA%*.o %OBJ_SAGA%
POPD

ECHO ###################################
ECHO SAGA CMD
ECHO -----------------------------------

SET OBJ_SAGA=%SAGA%\bin\tmp\saga_mingw\saga_cmd\
SET TMP_SAGA=%OBJ_SAGA%
MD %OBJ_SAGA%

PUSHD %SAGA%\src\saga_core\saga_cmd
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO SAGA GUI
ECHO -----------------------------------

SET OBJ_SAGA=%SAGA%\bin\tmp\saga_mingw\saga_gui\
SET TMP_SAGA=
MD %OBJ_SAGA%

PUSHD %SAGA%\src\saga_core\saga_gui
MOVE %OBJ_SAGA%*.o %TMP_SAGA%
MOVE %OBJ_SAGA%*.res %TMP_SAGA%
make -fmakefile.mingw
MOVE %TMP_SAGA%*.o %OBJ_SAGA%
MOVE %TMP_SAGA%*.res %OBJ_SAGA%
POPD


REM -----------------------------------
REM Module Libraries...
REM -----------------------------------

ECHO ###################################
ECHO contrib_a_perego
ECHO -----------------------------------

PUSHD .\modules_contrib\contrib_a_perego\contrib_a_perego
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO contrib_t_wutzler
ECHO -----------------------------------

PUSHD .\modules_contrib\contrib_t_wutzler\contrib_t_wutzler
rem make -fmakefile.mingw
POPD

ECHO ###################################
ECHO geostatistics_grid
ECHO -----------------------------------

PUSHD .\modules_geostatistics\geostatistics\geostatistics_grid
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO geostatistics_kriging
ECHO -----------------------------------

PUSHD .\modules_geostatistics\geostatistics\geostatistics_kriging
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO geostatistics_points
ECHO -----------------------------------

PUSHD .\modules_geostatistics\geostatistics\geostatistics_points
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO grid_analysis
ECHO -----------------------------------

PUSHD .\modules\grid\grid_analysis
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO grid_calculus
ECHO -----------------------------------

PUSHD .\modules\grid\grid_calculus
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO grid_discretisation
ECHO -----------------------------------

PUSHD .\modules\grid\grid_discretisation
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO grid_filter
ECHO -----------------------------------

PUSHD .\modules\grid\grid_filter
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO grid_gridding
ECHO -----------------------------------

PUSHD .\modules\grid\grid_gridding
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO grid_spline
ECHO -----------------------------------

PUSHD .\modules\grid\grid_spline
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO grid_tools
ECHO -----------------------------------

PUSHD .\modules\grid\grid_tools
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO grid_visualisation
ECHO -----------------------------------

PUSHD .\modules\grid\grid_visualisation
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO io_esri_e00
ECHO -----------------------------------

PUSHD .\modules_io\esri_e00\io_esri_e00
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO io_gps
ECHO -----------------------------------

PUSHD .\modules_io\gps\io_gps
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO io_grid
ECHO -----------------------------------

PUSHD .\modules_io\grid\io_grid
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO io_grid_gdal
ECHO -----------------------------------

PUSHD .\modules_io_gdal\io_grid_gdal\io_grid_gdal
rem make -fmakefile.mingw
POPD

ECHO ###################################
ECHO io_grid_image
ECHO -----------------------------------

PUSHD .\modules_io\grid\io_grid_image
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO io_shapes
ECHO -----------------------------------

PUSHD .\modules_io\shapes\io_shapes
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO io_table_odbc
ECHO -----------------------------------

PUSHD .\modules_io\table\io_table_odbc
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO lectures_introduction
ECHO -----------------------------------

PUSHD .\modules_lectures\lectures\lectures_introduction
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO pj_georeference
ECHO -----------------------------------

PUSHD .\modules_projection\pj_georeference\pj_georeference
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO pj_geotrans
ECHO -----------------------------------

PUSHD .\modules_projection\pj_geotrans\pj_geotrans
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO pj_proj4
ECHO -----------------------------------

PUSHD .\modules_projection\pj_proj4\pj_proj4
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO recreations_fractals
ECHO -----------------------------------

PUSHD .\modules_recreations\recreations\recreations_fractals
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO recreations_games
ECHO -----------------------------------

PUSHD .\modules_recreations\recreations\recreations_games
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO shapes_grid
ECHO -----------------------------------

PUSHD .\modules\shapes\shapes_grid
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO shapes_lines
ECHO -----------------------------------

PUSHD .\modules\shapes\shapes_lines
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO shapes_lines_ex
ECHO -----------------------------------

PUSHD .\modules\shapes\shapes_lines_ex
rem make -fmakefile.mingw
POPD

ECHO ###################################
ECHO shapes_points
ECHO -----------------------------------

PUSHD .\modules\shapes\shapes_points
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO shapes_polygons
ECHO -----------------------------------

PUSHD .\modules\shapes\shapes_polygons
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO shapes_tools
ECHO -----------------------------------

PUSHD .\modules\shapes\shapes_tools
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO sim_cellular_automata
ECHO -----------------------------------

PUSHD .\modules_simulation\cellular_automata\sim_cellular_automata
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO sim_ecosystems_hugget
ECHO -----------------------------------

PUSHD .\modules_simulation\ecosystems\sim_ecosystems_hugget
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO sim_fire_spreading
ECHO -----------------------------------

PUSHD .\modules_simulation\fire\fire_spreading
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO sim_hydrology
ECHO -----------------------------------

PUSHD .\modules_simulation\hydrology\sim_hydrology
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO ta_channels
ECHO -----------------------------------

PUSHD .\modules_terrain_analysis\terrain_analysis\ta_channels
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO ta_hydrology
ECHO -----------------------------------

PUSHD .\modules_terrain_analysis\terrain_analysis\ta_hydrology
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO ta_lighting
ECHO -----------------------------------

PUSHD .\modules_terrain_analysis\terrain_analysis\ta_lighting
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO ta_morphometry
ECHO -----------------------------------

PUSHD .\modules_terrain_analysis\terrain_analysis\ta_morphometry
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO ta_preprocessor
ECHO -----------------------------------

PUSHD .\modules_terrain_analysis\terrain_analysis\ta_preprocessor
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO ta_profiles
ECHO -----------------------------------

PUSHD .\modules_terrain_analysis\terrain_analysis\ta_profiles
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO ta_compound
ECHO -----------------------------------

PUSHD .\modules_terrain_analysis\terrain_analysis\ta_compound
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO table_calculus
ECHO -----------------------------------

PUSHD .\modules\table\table_calculus
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO table_tools
ECHO -----------------------------------

PUSHD .\modules\table\table_tools
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO tin_tools
ECHO -----------------------------------

PUSHD .\modules\tin\tin_tools
make -fmakefile.mingw
POPD

PAUSE
