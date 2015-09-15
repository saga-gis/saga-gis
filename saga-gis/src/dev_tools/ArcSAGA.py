import sys, os, glob, subprocess, arcpy, ConversionUtils

##########################################
# Globals
#_________________________________________
DIR_LOG  = None
#DIR_LOG  = 'C:\\_tmp'

DIR_SAGA, tail = os.path.split(os.path.dirname(__file__))
if os.path.isfile(DIR_SAGA + os.sep + 'saga_cmd.exe') == False:
	DIR_SAGA = None	# SAGA directory has to be in PATH environment variable
	DIR_SAGA = 'D:\\develop\\saga\\saga-code\\trunk\\saga-gis\\bin\\saga_vc_x64'


##########################################
# Run SAGA Tool
#_________________________________________
def Run_Tool(Library, Tool, Parameters):
	if DIR_SAGA != None:
		cmd		= [DIR_SAGA + os.sep + 'saga_cmd', '-f=q']
	else:
		cmd		= ['saga_cmd', '-f=q']
	cmd = cmd + [Library, Tool] + Parameters

	if DIR_LOG != None:
		log_std	= open(DIR_LOG + os.sep + 'arcsaga.log'      , 'a')
		log_err	= open(DIR_LOG + os.sep + 'arcsaga.error.log', 'a')
		p		= subprocess.call(cmd, stdout=log_std, stderr=log_err)
	else:
		p		= subprocess.call(cmd)

	if p != 0:
		arcpy.AddMessage('_________________________')
		arcpy.AddError('...failed to run SAGA tool!')
		s = 'saga_cmd ' + Library + ' ' + Tool
		for i in Parameters:
			s	= s + ' ' + i
		arcpy.AddMessage(s)

	return p

	
##########################################
# File Tools
#_________________________________________
def File_Get_TempName(Extension):
	return os.tempnam(None, 'arc_saga_') + '.' + Extension

#_________________________________________
def File_Cmp_Extension(File, Extension):
	if File == '#' or not File:
		return False
	File, ext = os.path.splitext(File)
	if Extension == ext:
		return True
	return False

#_________________________________________
def File_Set_Extension(File, Extension):
	if File == '#' or not File:
		return None
	File, ext = os.path.splitext(File)
	File = File + '.' + Extension
	return File

#_________________________________________
def File_Remove_All(File_Name):
	if File_Name == '#' or not File_Name:
		return
	Files = File_Set_Extension(File_Name, '*')
	for File in glob.glob(Files):
		os.remove(File)
	return


##########################################
# Parameters
#_________________________________________
def Set_Option(Parameters, Identifier, Value):
	if Value == '#' or Value == None:
		return Parameters

	Value.strip()
	Parameter = ['-' + Identifier, Value]
	if Parameters == None:
		return Parameter
	return Parameters + Parameter

#_________________________________________
def Set_Input(Parameters, Identifier, Value, Type):
	Files = None

	if Value != '#' and Value != None:
		List  = ConversionUtils.SplitMultiInputs(Value)
		for Item in List:
			if   Type == 'grid'   or Type == 'grid_list':
				File = Arc_To_SAGA_Raster (Item)
			elif Type == 'table'  or Type == 'table_list':
				File = Arc_To_SAGA_Table  (Item)
			elif Type == 'shapes' or Type == 'shapes_list':
				File = Arc_To_SAGA_Feature(Item)
			elif Type == 'points' or Type == 'points_list':
				File = Arc_To_SAGA_Points (Item)
			else:
				File = None

			if File != None:
				if Files == None:
					Files = File
				else:
					Files = Files + ';' + File

	if Files == None:
		return Parameters, None

	Parameter = ['-' + Identifier, Files]

	if Parameters == None:
		return Parameter, Files
	return Parameters + Parameter, Files

#_________________________________________
def Set_Output(Parameters, Identifier, Value, Type):
	Files = None

	if Value != '#' and Value != None:
		List  = ConversionUtils.SplitMultiInputs(Value)
		for Item in List:
			if   Type == 'grid'   or Type == 'grid_list':
				File = File_Get_TempName('sgrd')
			elif Type == 'table'  or Type == 'table_list':
				if File_Cmp_Extension(Item,  'dbf') == False:
					File = File_Get_TempName('dbf')
				else:
					File = Item
			elif Type == 'shapes' or Type == 'shapes_list':
				if File_Cmp_Extension(Item,  'shp') == False:
					arcpy.AddMessage('ext <> shp: ' + Item)
					File = File_Get_TempName('shp')
				else:
					File = Item
			elif Type == 'points' or Type == 'points_list':
				File = File_Get_TempName('spc')
			else:
				File = None

			if File != None:
				if Files == None:
					Files = File
				else:
					Files = Files + ';' + File

	if Files == None:
		return Parameters, None

	Parameter = ['-' + Identifier, Files]

	if Parameters == None:
		return Parameter, Files
	return Parameters + Parameter, Files

#_________________________________________
def Get_Output(Identifier, Value, Type):
	if Value != '#' and Value != None:
		List  = Value.split(';')
		for Item in List:
			if   Type == 'grid'   or Type == 'grid_list':
				SAGA_To_Arc_Raster (Identifier, Item)
			elif Type == 'table'  or Type == 'table_list':
				SAGA_To_Arc_Table  (Identifier, Item)
			elif Type == 'shapes' or Type == 'shapes_list':
				SAGA_To_Arc_Feature(Identifier, Item)
			elif Type == 'points' or Type == 'points_list':
				SAGA_To_Arc_Points (Identifier, Item)
	return


##########################################
# Raster Conversion
#_________________________________________
def Arc_To_SAGA_Raster(Raster):
	Raster = ConversionUtils.ValidateInputRaster(Raster)

	Supported = ['tif', 'img', 'asc']
	for ext in Supported:
		if File_Cmp_Extension(Raster, ext):
			return Raster

	arcpy.AddMessage('Export: ' + Raster)

	File   = os.tempnam(None, 'arc_saga_') + '.tif'
	ConversionUtils.CopyRasters(Raster, File, "")

	return File

#_________________________________________
def SAGA_To_Arc_Raster(File, Raster):
	arcpy.AddMessage('Import: ' + Raster)

	if File_Cmp_Extension(Raster, 'tif'):
		if Run_Tool('io_gdal', '2', ['-GRIDS' , File, '-FILE', Raster]) != 0 : # 'Export Raster to GeoTIFF'
			return 0
	else:
		GeoTIFF = File_Set_Extension(File, 'tif')
		if Run_Tool('io_gdal', '2', ['-GRIDS' , File, '-FILE', GeoTIFF]) != 0 : # 'Export Raster to GeoTIFF'
			return 0
		GeoTIFF = ConversionUtils.ValidateInputRaster(GeoTIFF)
		ConversionUtils.CopyRasters(GeoTIFF, Raster, "")

	File_Remove_All(File)
	Arc_Load_Layer(Raster)
	
	return 1


##########################################
# Feature Conversion
#_________________________________________
def Arc_To_SAGA_Feature(Feature):
	if File_Cmp_Extension(Feature, 'shp') == True:
		return Feature

	File = os.tempnam(None, 'arc_saga_') + '.shp'
	ConversionUtils.CopyFeatures(Feature, File)

	return File

#_________________________________________
def SAGA_To_Arc_Feature(File, Feature):
	if File_Cmp_Extension(Feature, 'shp') == False:
		ConversionUtils.CopyFeatures(File, Feature)
		File_Remove_All(File)

	Arc_Load_Layer(Feature)

	return 1


##########################################
# Feature Conversion
#_________________________________________
def Arc_To_SAGA_Table(Table):
	if File_Cmp_Extension(Table, 'dbf') == True:
		return Table

	File  = os.tempnam(None, 'arc_saga_') + '.dbf'
	ConversionUtils.CopyRows(Table, File)

	return File

#_________________________________________
def SAGA_To_Arc_Table(File, Table):
	if File_Cmp_Extension(Table, 'dbf') == False:
		ConversionUtils.CopyRows(File, Table)
		File_Remove_All(File)

#	Arc_Load_Layer(Table)

	return 1

	
##########################################
# ArcMap Interaction
#_________________________________________
def Arc_Load_Layer(Layer):
	if Layer == '#' or not Layer:
		return
	# ------------------------------------
	mxd = arcpy.mapping.MapDocument("CURRENT")
	df  = arcpy.mapping.ListDataFrames(mxd)[0]
	lyr = arcpy.mapping.Layer(Layer)
	arcpy.mapping.AddLayer(df, lyr, 'AUTO_ARRANGE')
	return


##########################################
#_________________________________________
