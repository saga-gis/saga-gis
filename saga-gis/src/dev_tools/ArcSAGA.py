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
# File Tools
#_________________________________________
def File_Get_TempName(Extension):
	return os.tempnam(None, 'arc_saga_') + '.' + Extension

#_________________________________________
def File_Cmp_Extension(File, Extension):
	if File != '#' and File != None:
		File, ext = os.path.splitext(File)
		if Extension == ext:
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


	######################################
	# Execution
	#_____________________________________
	def Run(self):
		if DIR_SAGA != None:
			cmd = [DIR_SAGA + os.sep + 'saga_cmd', '-f=q']
		else:
			cmd =                     ['saga_cmd', '-f=q']
		cmd += [self.Library, self.Tool] + self.Parameters

		if DIR_LOG != None:
			log_std = open(DIR_LOG + os.sep + 'arcsaga.log'      , 'a')
			log_err = open(DIR_LOG + os.sep + 'arcsaga.error.log', 'a')
			Result  = subprocess.call(cmd, stdout=log_std, stderr=log_err)
		else:
			Result  = subprocess.call(cmd)

		if Result != 0:
			arcpy.AddMessage('_________________________')
			arcpy.AddError('...failed to run SAGA tool!')
			Message = 'saga_cmd ' + self.Library + ' ' + self.Tool
			for Item in self.Parameters:
				Message += ' ' + Item
			arcpy.AddMessage(Message)
			
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
	def Add_Temporary(self, File):
		if self.Temporary == None:
			self.Temporary  = [File]
		else:
			self.Temporary += [File]
		return

	#_____________________________________
	def Add_Output(self, Identifier, File, Type):
		if self.Output == None:
			self.Output  = [[Identifier, File, Type]]
		else:
			self.Output += [[Identifier, File, Type]]
		return


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
					File = File_Get_TempName('sgrd')
				elif Type == 'table'  or Type == 'table_list':
					if File_Cmp_Extension(Item,  'dbf') == False:
						File = File_Get_TempName('dbf')
					else:
						File = Item
				elif Type == 'shapes' or Type == 'shapes_list':
					if File_Cmp_Extension(Item,  'shp') == False:
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

					if File != Item:
						self.Add_Temporary(File)

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
	Supported = ['tif', 'img', 'asc']
	for ext in Supported:
		if File_Cmp_Extension(Raster, ext):
			return Raster

	File   = File_Get_TempName('tif')
	Raster = ConversionUtils.ValidateInputRaster(Raster)
	ConversionUtils.CopyRasters(Raster, File, "")

	return File

#_________________________________________
def SAGA_To_Arc_Raster(File, Raster):
	Tool = SAGA_Tool('io_gdal', '2') # 'Export Raster to GeoTIFF'
	Tool.Set_Option('GRIDS', File)

	if File_Cmp_Extension(Raster, 'tif'):
		Tool.Set_Option('FILE', Raster)
		if Tool.Run() != 0:
			return False
	else:
		GeoTIFF = File_Set_Extension(File, 'tif')
		Tool.Set_Option('FILE', GeoTIFF)
		if Tool.Run() != 0:
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
	mxd = arcpy.mapping.MapDocument("CURRENT")
	df  = arcpy.mapping.ListDataFrames(mxd)[0]
	lyr = arcpy.mapping.Layer(Layer)
	arcpy.mapping.AddLayer(df, lyr, 'AUTO_ARRANGE')
	return


##########################################
#_________________________________________
