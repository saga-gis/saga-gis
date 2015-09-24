import sys, os, glob, subprocess, arcpy, ConversionUtils, shutil

##########################################
# Globals
#_________________________________________
DIR_SELF = os.path.dirname(__file__)

DIR_LOG  = None
#DIR_LOG  = DIR_SELF # uncomment to create logs in toolbox directory

DIR_SAGA = None	# assuming SAGA directory is included in the PATH environment variable
#DIR_SAGA = 'D:\\saga\\saga-code\\trunk\\saga-gis\\bin\\saga_vc_x64'	# ...or define hard coded path to SAGA directory

dir, tail = os.path.split(DIR_SELF)
if os.path.isfile(dir + os.sep + 'saga_cmd.exe') == True:
	DIR_SAGA = dir	# use SAGA instance of this installation


##########################################
# File Tools
#_________________________________________
def File_Get_TempName(Extension):
	return os.tempnam(None, 'arc_saga_') + '.' + Extension

#_________________________________________
def File_Cmp_Extension(File, Extension):
	if File != '#' and File != None:
		File, ext = os.path.splitext(File)
		if '.' + Extension == ext:
			return True
	return False

#_________________________________________
def File_Set_Extension(File, Extension):
	if File != '#' and File != None:
		File, ext = os.path.splitext(File)
		File += '.' + Extension
		return File
	return None

#_________________________________________
def File_Remove_All(File_Name):
	if File_Name != '#' and File_Name != None:
		Files = File_Set_Extension(File_Name, '*')
		for File in glob.glob(Files):
			os.remove(File)
	return


##########################################
# The SAGA Tool Execution Class
#_________________________________________
class SAGA_Tool:

	######################################
	# Construction
	#_____________________________________
	def __init__(self, Library, Tool):
		self.Library    = Library
		self.Tool       = Tool
		self.Parameters	= None
		self.Temporary  = None
		self.Output     = None

		if DIR_SAGA == None:
			self.saga_cmd =                     ['saga_cmd']
		else:
			self.saga_cmd = [DIR_SAGA + os.sep + 'saga_cmd']


	######################################
	# Execution
	#_____________________________________
	def Run(self, bIgnoreLog = False):
		cmd_string  = '_________________________\n'
		cmd_string += 'saga_cmd ' + self.Library + ' ' + self.Tool
		for Item in self.Parameters:
			cmd_string += ' ' + Item

		cmd    = [self.saga_cmd, '-f=q', self.Library, self.Tool] + self.Parameters

		if bIgnoreLog == False and DIR_LOG != None:
			cmd_out = open(DIR_LOG + os.sep + 'arcsaga.log'      , 'w')
			cmd_err = open(DIR_LOG + os.sep + 'arcsaga.error.log', 'w')
			cmd_out.write(cmd_string)	# print to log file
			Result = subprocess.call(cmd, stdout=cmd_out, stderr=cmd_err)
		else:
			Result = subprocess.call(cmd)

		if Result != 0:
			arcpy.AddMessage(cmd_string)
			arcpy.AddError('...failed to run SAGA tool!')

		elif self.Output != None:
			for Data in self.Output:
				self.Get_Output(Data[0], Data[1], Data[2])

		if self.Temporary != None:
			for Data in self.Temporary:
				File_Remove_All(Data)

		return Result


	######################################
	# Data Lists
	#_____________________________________
	def Add_Output(self, Identifier, File, Type):
		if self.Output == None:
			self.Output  = [[Identifier, File, Type]]
		else:
			self.Output += [[Identifier, File, Type]]
		return

	#_____________________________________
	def Add_Temporary(self, File):
		if self.Temporary == None:
			self.Temporary  = [File]
		else:
			self.Temporary += [File]
		return

	#_____________________________________
	def Get_Temporary(self, Extension):
		File = File_Get_TempName(Extension)
		self.Add_Temporary(File)
		return File


	######################################
	# Parameters
	#_____________________________________
	def Set_Option(self, Identifier, Value):
		if Value != '#' and Value != None:
			Value.strip()
			if self.Parameters == None:
				self.Parameters  = ['-' + Identifier, Value]
			else:
				self.Parameters += ['-' + Identifier, Value]
		return

	#_____________________________________
	def Set_Input(self, Identifier, Value, Type):
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

					if File != Item:
						self.Add_Temporary(File)

		if Files != None:
			self.Set_Option(Identifier, Files)

		return Files

	#_____________________________________
	def Set_Output(self, Identifier, Value, Type):
		Files = None

		if Value != '#' and Value != None:
			List  = ConversionUtils.SplitMultiInputs(Value)
			for Item in List:
				if   Type == 'grid'   or Type == 'grid_list':
					if File_Cmp_Extension(Item,   'sdat') == False:
						File = self.Get_Temporary('sgrd')
					else:
						File = File_Set_Extension(Item, 'sgrd')
				elif Type == 'table'  or Type == 'table_list':
					if File_Cmp_Extension(Item,   'dbf') == False:
						File = self.Get_Temporary('dbf')
					else:
						File = Item
				elif Type == 'shapes' or Type == 'shapes_list':
					if File_Cmp_Extension(Item,   'shp') == False:
						File = self.Get_Temporary('shp')
					else:
						File = Item
				elif Type == 'points' or Type == 'points_list':
					File = self.Get_Temporary('spc')
				else:
					File = None

				if File != None:
					if Files == None:
						Files = File
					else:
						Files = Files + ';' + File

		if Files != None:
			self.Set_Option(Identifier, Files)
			self.Add_Output(Files, Value, Type)

		return Files

	#_____________________________________
	def Get_Output(self, SagaFile, ArcFile, Type):
		if ArcFile != '#' and ArcFile != None:
			List  = ArcFile.split(';')
			for Item in List:
				if   Type == 'grid'   or Type == 'grid_list':
					SAGA_To_Arc_Raster (SagaFile, Item)
				elif Type == 'table'  or Type == 'table_list':
					SAGA_To_Arc_Table  (SagaFile, Item)
				elif Type == 'shapes' or Type == 'shapes_list':
					SAGA_To_Arc_Feature(SagaFile, Item)
				elif Type == 'points' or Type == 'points_list':
					SAGA_To_Arc_Points (SagaFile, Item)
		return


##########################################
# Raster Conversion
#_________________________________________
def Arc_To_SAGA_Raster(Raster):
	Supported = ['sdat', 'tif', 'img', 'asc']
	for ext in Supported:
		if File_Cmp_Extension(Raster, ext):
			return Raster

	File   = File_Get_TempName('tif')
	Raster = ConversionUtils.ValidateInputRaster(Raster)
	ConversionUtils.CopyRasters(Raster, File, "")

	return File

#_________________________________________
def SAGA_To_Arc_Raster(File, Raster):
	if File_Cmp_Extension(Raster, 'sdat') == False: # conversion needed
		Tool = SAGA_Tool('io_gdal', '2') # 'Export Raster to GeoTIFF'
		Tool.Set_Option('GRIDS', File)

		if File_Cmp_Extension(Raster, 'tif'):
			arcpy.AddMessage('Output GeoTIFF: ' + File + ' >> ' + Raster)
			Tool.Set_Option('FILE', Raster)
			if Tool.Run(True) != 0:
				return False
		else:
			arcpy.AddMessage('Output Convert: ' + File + ' >> ' + Raster)
			GeoTIFF = File_Set_Extension(File, 'tif')
			Tool.Set_Option('FILE', GeoTIFF)
			if Tool.Run(True) != 0:
				return False
			GeoTIFF = ConversionUtils.ValidateInputRaster(GeoTIFF)
			ConversionUtils.CopyRasters(GeoTIFF, Raster, "")

	Arc_Load_Layer(Raster)
	
	return True


##########################################
# Feature Conversion
#_________________________________________
def Arc_To_SAGA_Feature(Feature):
	if File_Cmp_Extension(Feature, 'shp') == True:
		return Feature

	File = File_Get_TempName('.shp')
	ConversionUtils.CopyFeatures(Feature, File)

	return File

#_________________________________________
def SAGA_To_Arc_Feature(File, Feature):
	if File_Cmp_Extension(Feature, 'shp') == False:
		ConversionUtils.CopyFeatures(File, Feature)
		File_Remove_All(File)

	Arc_Load_Layer(Feature)

	return True


##########################################
# Table Conversion
#_________________________________________
def Arc_To_SAGA_Table(Table):
	if File_Cmp_Extension(Table, 'dbf') == True:
		return Table

	File  = File_Get_TempName('.dbf')
	ConversionUtils.CopyRows(Table, File)

	return File

#_________________________________________
def SAGA_To_Arc_Table(File, Table):
	if File_Cmp_Extension(Table, 'dbf') == False:
		ConversionUtils.CopyRows(File, Table)
		File_Remove_All(File)

	return True

	
##########################################
# ArcMap Interaction
#_________________________________________
def Arc_Load_Layer(Layer):
	if Layer == '#' or not Layer:
		return
	# ------------------------------------
	Map_Project = arcpy.mapping.MapDocument("CURRENT")
	Map_Frame   = arcpy.mapping.ListDataFrames(Map_Project)[0]
	Map_Layer   = arcpy.mapping.Layer(Layer)

	if Map_Layer.isRasterLayer:
		if os.path.isfile(DIR_SELF + os.sep + 'grid.lyr') == True:
			Src_Layer = arcpy.mapping.Layer(DIR_SELF + os.sep + 'grid.lyr')
			arcpy.mapping.UpdateLayer(Map_Frame, Map_Layer, Src_Layer, True)

	arcpy.mapping.AddLayer(Map_Frame, Map_Layer, 'AUTO_ARRANGE')
	
	return


##########################################
#_________________________________________
