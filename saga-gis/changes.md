
# Backward incompatible changes

This document lists backward incompatible changes and provides hints on how these can be resolved. The document covers both the SAGA API as well as tool parameter interface changes.

___
___
## SAGA 9.3.0 API changes

#### CSG_Shapes::Destroy()

The method now invalidates the shape type (SHAPE_TYPE_Undefined). If you want to reuse the object, better use Create(), otherwise Add() returns a NULL pointer.

#### CSG_Data_Manager class

The class has been reworked and improved. The most prominent change is the removal of the Delete_All() method:

CSG_Data_Manager.Delete_All() &rarr; CSG_Data_Manager.Delete()

___
## SAGA 9.3.0 Tool interface changes

In some tools, the "extra" tool parameters have been removed, which changes the parameter interface.

#### Point Cloud Cutter ('pointcloud_tools', '0')
| old | new |
| --- | --- | 
| -USER_XMIN | -XMIN |
| -USER_XMAX | -XMAX |
| -USER_YMIN | -YMIN |
| -USER_YMAX | -YMAX |
| -GRID_GRID_D | -GRID_D |
| -GRID_GRID_X | -GRID_X |
| -GRID_GRID_Y | -GRID_Y |
| -GRID_GRID_NX | -GRID_NX |
| -GRID_GRID_NY | -GRID_NY |
| -GRID_GRID_FILE | -GRID_FILE |
| -EXTENT_EXTENT | -EXTENT |
| -POLYGONS_POLYGONS | -POLYGONS |

#### Import SRTM30 DEM ('io_grid', '9')
-TILE_PATH removed

___
___
## SAGA 9.2.0 API changes

The output datatypes for grids and attribute fields have been unified in SAGA 9.2.0. Now there is a special parameter type, previously each tool had implemented its own list of datatypes. The change removes the inconsistencies across tools, but has the drawback that the choice indices for some of the datatypes has changed. Please verify the settings of the tools listed below within your scripts:

___
## SAGA 9.2.0 Tool interface changes

#### Constant Grid ('grid_tools', '3')
New data storage types (parameter "TYPE" choices index changed)

| old choices | new choices
| --- | --- 
| [0] bit						    | [0] bit
| [1] unsigned 1 byte integer	    | [1] unsigned 1 byte integer
| [2] signed 1 byte integer		    | [2] signed 1 byte integer
| [3] unsigned 2 byte integer		| [3] unsigned 2 byte integer
| [4] signed 2 byte integer		    | [4] signed 2 byte integer
| [5] unsigned 8 byte integer		| [5] unsigned 4 byte integer
| [6] signed 8 byte integer		    | [6] signed 4 byte integer
| [7] 4 byte floating point number  | [7] unsigned 8 byte integer
| [8] 8 byte floating point number  | [8] signed 8 byte integer
| Default: 7					    | [9] 4 byte floating point number
| 								    | [10] 8 byte floating point number
| 								    | Default: 9


#### Coordinate Transformation Grid Lists / Grids ('pj_proj4', '3') / ('pj_proj4', '4')
New data storage types (parameter "KEEP_TYPE" removed, parameter "TYPE" choices index changed)

| old choices | new choices
| --- | --- 
| [0] bit							| [0] unsigned 1 byte integer
| [1] unsigned 1 byte integer		| [1] signed 1 byte integer
| [2] signed 1 byte integer			| [2] unsigned 2 byte integer
| [3] unsigned 2 byte integer		| [3] signed 2 byte integer
| [4] signed 2 byte integer			| [4] unsigned 4 byte integer
| [5] unsigned 4 byte integer		| [5] signed 4 byte integer
| [6] signed 4 byte integer			| [6] unsigned 8 byte integer
| [7] unsigned 8 byte integer		| [7] signed 8 byte integer
| [8] signed 8 byte integer			| [8] 4 byte floating point number
| [9] 4 byte floating point number	| [9] 8 byte floating point number
| [10] 8 byte floating point number	| [10] Preserve
| Default: 9						| Default: 10


#### Direct Georeferencing of Airborne Photographs ('pj_georeference', '4')
New data storage types (parameter "DATA_TYPE" choices index changed)

| old choices | new choices
| --- | --- 
| [0] 1 byte unsigned integer		| [0] unsigned 1 byte integer
| [1] 1 byte signed integer			| [1] signed 1 byte integer
| [2] 2 byte unsigned integer		| [2] unsigned 2 byte integer
| [3] 2 byte signed integer			| [3] signed 2 byte integer
| [4] 4 byte unsigned integer		| [4] unsigned 4 byte integer
| [5] 4 byte signed integer			| [5] signed 4 byte integer
| [6] 4 byte floating point			| [6] unsigned 8 byte integer
| [7] 8 byte floating point			| [7] signed 8 byte integer
| [8] same as original				| [8] 4 byte floating point number
| Default: 8						| [9] 8 byte floating point number
| 									| [10] same as original
| 									| Default: 10


#### Grid Calculator ('grid_calculus', '1')
New data storage types (parameter "TYPE" choices index changed)

| old choices | new choices
| --- | --- 
| [0] bit							| [0] bit
| [1] unsigned 1 byte integer		| [1] unsigned 1 byte integer
| [2] signed 1 byte integer			| [2] signed 1 byte integer
| [3] unsigned 2 byte integer		| [3] unsigned 2 byte integer
| [4] signed 2 byte integer			| [4] signed 2 byte integer
| [5] unsigned 4 byte integer		| [5] unsigned 4 byte integer
| [6] signed 4 byte integer			| [6] signed 4 byte integer
| [7] 4 byte floating point number	| [7] unsigned 8 byte integer
| [8] 8 byte floating point number	| [8] signed 8 byte integer
| Default: 7						| [9] 4 byte floating point number
| 									| [10] 8 byte floating point number
| 									| Default: 9


#### Import Binary Raw Data ('io_grid', '4')
New data storage types (parameter "DATA_TYPE" choices index changed)

| old choices | new choices
| --- | --- 
| [0] 8 bit unsigned integer		| [0] unsigned 1 byte integer
| [1] 8 bit signed integer			| [1] signed 1 byte integer
| [2] 16 bit unsigned integer		| [2] unsigned 2 byte integer
| [3] 16 bit signed integer			| [3] signed 2 byte integer
| [4] 32 bit unsigned integer		| [4] unsigned 4 byte integer
| [5] 32 bit signed integer			| [5] signed 4 byte integer
| [6] 32 bit floating point			| [6] unsigned 8 byte integer
| [7] 64 bit floating point			| [7] signed 8 byte integer
| Default: 0						| [8] 4 byte floating point number
| 									| [9] 8 byte floating point number
| 									| Default: 8


#### Import Grid from Table ('io_grid', '12')
New data storage types (parameter "DATA_TYPE" choices index changed)

| old choices | new choices
| --- | --- 
| [0] 1 Byte Integer (unsigned)		| [0] bit
| [1] 1 Byte Integer (signed)		| [1] unsigned 1 byte integer
| [2] 2 Byte Integer (unsigned)		| [2] signed 1 byte integer
| [3] 2 Byte Integer (signed)		| [3] unsigned 2 byte integer
| [4] 4 Byte Integer (unsigned)		| [4] signed 2 byte integer
| [5] 4 Byte Integer (signed)		| [5] unsigned 4 byte integer
| [6] 4 Byte Floating Point			| [6] signed 4 byte integer
| [7] 8 Byte Floating Point			| [7] unsigned 8 byte integer
| Default: 6						| [8] signed 8 byte integer
| 									| [9] 4 byte floating point number
| 									| [10] 8 byte floating point number
| 									| Default: 9



#### Import Grid from XYZ ('io_grid', '6')
New data storage types (parameter "TYPE" choices index changed)

| old choices | new choices
| --- | --- 
| [0] 1 bit							| [0] bit
| [1] 1 byte unsigned integer		| [1] unsigned 1 byte integer
| [2] 1 byte signed integer			| [2] signed 1 byte integer
| [3] 2 byte unsigned integer		| [3] unsigned 2 byte integer
| [4] 2 byte signed integer			| [4] signed 2 byte integer
| [5] 4 byte unsigned integer		| [5] unsigned 4 byte integer
| [6] 4 byte signed integerr		| [6] signed 4 byte integer
| [7] 4 byte floating point			| [7] unsigned 8 byte integer
| [8] 8 byte floating point			| [8] signed 8 byte integer
| Default: 7						| [9] 4 byte floating point number
| 									| [10] 8 byte floating point number
| 									| Default: 9


#### Mosaicking ('grid_tools', '3')
New data storage types (parameter "TYPE" choices index changed)
 
| old choices | new choices
| --- | --- 
| [0] 1 bit							| [0] bit
| [1] 1 byte unsigned integer		| [1] unsigned 1 byte integer
| [2] 1 byte signed integer			| [2] signed 1 byte integer
| [3] 2 byte unsigned integer		| [3] unsigned 2 byte integer
| [4] 2 byte signed integer			| [4] signed 2 byte integer
| [5] 4 byte unsigned integer		| [5] unsigned 4 byte integer
| [6] 4 byte signed integer			| [6] signed 4 byte integer
| [7] 4 byte floating point			| [7] unsigned 8 byte integer
| [8] 8 byte floating point			| [8] signed 8 byte integer
| [9] same as first grid in list	| [9] 4 byte floating point number
| Default: 9						| [10] 8 byte floating point number
| 									| [11] same as first grid in list
| 									| Default: 11


#### Point Cloud Attribute Calculator ('pointcloud_tools', '10')
New data storage types (parameter "TYPE" choices index changed)

| old choices | new choices
| --- | --- 
| [0] 1 bit							| [0] bit
| [1] 1 byte unsigned integer		| [1] unsigned 1 byte integer
| [2] 1 byte signed integer			| [2] signed 1 byte integer
| [3] 2 byte unsigned integer		| [3] unsigned 2 byte integer
| [4] 2 byte signed integer			| [4] signed 2 byte integer
| [5] 4 byte unsigned integer		| [5] unsigned 4 byte integer
| [6] 4 byte signed integerr		| [6] signed 4 byte integer
| [7] 4 byte floating point			| [7] unsigned 8 byte integer
| [8] 8 byte floating point			| [8] signed 8 byte integer
| Default: 7						| [9] 4 byte floating point number
| 									| [10] 8 byte floating point number
| 									| Default: 9


#### Reclassify Grid Values ('grid_tools', '15')
New data storage types (parameter "RESULT_TYPE" choices index changed)

| old choices | new choices
| --- | --- 
| [0] 1 bit							| [0] bit
| [1] 1 byte unsigned integer		| [1] unsigned 1 byte integer
| [2] 1 byte signed integer			| [2] signed 1 byte integer
| [3] 2 byte unsigned integer		| [3] unsigned 2 byte integer
| [4] 2 byte signed integer			| [4] signed 2 byte integer
| [5] 4 byte unsigned integer		| [5] unsigned 4 byte integer
| [6] 4 byte signed integer			| [6] signed 4 byte integer
| [7] 4 byte floating point			| [7] unsigned 8 byte integer
| [8] 8 byte floating point			| [8] signed 8 byte integer
| [9] same as input grid			| [9] 4 byte floating point number
| Default: 9						| [10] 8 byte floating point number
| 									| [11] same as first grid in list
| 									| Default: 11


#### Shapes to Grid ('grid_gridding', '0')
New data storage types (parameter "GRID_TYPE" choices index changed)

| old choices | new choices
| --- | --- 
| [0] 1 bit							| [0] bit
| [1] 1 byte unsigned integer		| [1] unsigned 1 byte integer
| [2] 1 byte signed integer			| [2] signed 1 byte integer
| [3] 2 byte unsigned integer		| [3] unsigned 2 byte integer
| [4] 2 byte signed integer			| [4] signed 2 byte integer
| [5] 4 byte unsigned integer		| [5] unsigned 4 byte integer
| [6] 4 byte signed integer			| [6] signed 4 byte integer
| [7] 4 byte floating point			| [7] unsigned 8 byte integer
| [8] 8 byte floating point			| [8] signed 8 byte integer
| [9] same as attribute				| [9] 4 byte floating point number
| Default: 9						| [10] 8 byte floating point number
| 									| [11] same as attribute
| 									| Default: 11


___
___
## SAGA 9.0.0 API changes

Some functions and enumerations have been removed or renamed. Some return values have changed.

#### CSG_Parameter_List
Removal of redundant Get_Data_Count() and Get_Data() functions

Get_Data_Count() &rarr; Get_Item_Count() or overwrites for datatypes, e.g. Get_Grid_Count()  
Get_Data() &rarr; Get_Item()

#### CSG_PointCloud
Get_Point_Count() renamed to Get_Count()

Get_Point_Count() &rarr; Get_Count()

#### CSG_Table, CSG_Shapes
Get_Record_Count() renamed to Get_Count()

Get_Record_Count() &rarr; Get_Count()

#### CSG_Table, CSG_Shapes, CSG_PointCloud
Get_Count() now returns sLong instead of int

&rarr; change iterator in for loops from int to sLong  
&rarr; change format specifiers in print statements from %d to %lld  
&rarr; check Set_Progress/SG_UI_Process_Set_Progress functions

#### TSG_Point_Z
renamed to TSG_Point_3D

TSG_Point_Z &rarr; TSG_Point_3D

#### CSG_Point_Z
renamed to CSG_Point_3D

CSG_Point_Z &rarr; CSG_Point_3D

#### CSG_Point_ZM
renamed to CSG_Point_4D

CSG_Point_ZM &rarr; CSG_Point_4D

#### CSG_Point class
"Get_X()" etc. functions removed

CSG_Point.Get_X() &rarr; CSG_Point.x

#### CSG_Rect class:
"m_rect" member removed

CSG_Rect.m_rect.xMin &rarr; CSG_Rect.xMin

#### ESG_Data_Type
renamed to TSG_Data_Type

ESG_Data_Type &rarr; TSG_Data_Type

#### ESG_UI_DataObject_Update
enumeration changed and updated

SG_UI_DATAOBJECT_UPDATE_ONLY &rarr; SG_UI_DATAOBJECT_UPDATE  
SG_UI_DATAOBJECT_SHOW &rarr; SG_UI_DATAOBJECT_SHOW_MAP  
SG_UI_DATAOBJECT_SHOW_NEW_MAP &rarr; SG_UI_DATAOBJECT_SHOW_MAP_NEW  
SG_UI_DATAOBJECT_SHOW_LAST_MAP &rarr; SG_UI_DATAOBJECT_SHOW_MAP_LAST  
SG_UI_DATAOBJECT_SHOW_MAP_ACTIVE &rarr; added


___
## SAGA 9.0.0 Tool interface changes

#### Grid Values to Points ('shapes_grid', '3')
-SHAPES parameter identifier renamed to -POINTS  
