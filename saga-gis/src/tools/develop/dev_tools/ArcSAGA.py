#_______________________________________________________#
#########################################################
#                                                       #
#          Copyright (C) 2020 by Olaf Conrad            #
#                                                       #
#_______________________________________________________#
#                                                       #
# This file is part of 'SAGA - System for Automated     #
# Geoscientific Analyses'.                              #
#                                                       #
# This library is free software; you can redistribute   #
# it and/or modify it under the terms of the GNU Lesser #
# General Public License as published by the Free       #
# Software Foundation, either version 2.1 of the        #
# License, or (at your option) any later version.       #
#                                                       #
# This library is distributed in the hope that it will  #
# be useful, but WITHOUT ANY WARRANTY; without even the #
# implied warranty of MERCHANTABILITY or FITNESS FOR A  #
# PARTICULAR PURPOSE. See the GNU Lesser General Public #
# License for more details.                             #
#                                                       #
# You should have received a copy of the GNU Lesser     #
# General Public License along with this program; if    #
# not, see <http://www.gnu.org/licenses/>.              #
#                                                       #
#_______________________________________________________#
#                                                       #
#    contact:    Olaf Conrad                            #
#                Institute of Geography                 #
#                University of Hamburg                  #
#                Germany                                #
#                                                       #
#    e-mail:     oconrad@saga-gis.org                   #
#                                                       #
#########################################################

#########################################################
#________________________________________________________
import sys, os, glob, subprocess, arcpy, ConversionUtils, shutil


#########################################################
# Globals
#________________________________________________________
DIR_LOG  = None
DIR_LOG  = os.path.dirname(__file__) # uncomment to create logs in toolbox directory

CREATE_NO_WINDOW = 0x08000000


#########################################################
# File Tools
#________________________________________________________
def File_Get_TempName(Extension):
	if sys.version_info.major < 3: # ArcGIS Desktop
		return os.tempnam(None, 'arc_saga_') + '.' + Extension
	import tempfile
	return tempfile._get_default_tempdir() + os.sep + 'arcsaga_' + next(tempfile._get_candidate_names()) + '.' + Extension

#________________________________________________________
def File_Cmp_Extension(File, Extension):
	if File != '#' and File != None:
		File, ext = os.path.splitext(File)
		if '.' + Extension == ext:
			return True
	return False

#________________________________________________________
def File_Set_Extension(File, Extension):
	if File != '#' and File != None:
		File, ext = os.path.splitext(File)
		File += '.' + Extension
		return File
	return None

#________________________________________________________
def File_Remove_All(File_Name):
	if File_Name != '#' and File_Name != None:
		Files = File_Set_Extension(File_Name, '*')
		for File in glob.glob(Files):
			os.remove(File)
	return


#########################################################
# The SAGA Tool Execution Class
#________________________________________________________
class SAGA_Tool:

	#####################################################
	# Construction
	#____________________________________________________
	def __init__(self, Library, Tool):
		self.Library    = Library
		self.Tool       = Tool
		self.Parameters	= None
		self.Temporary  = None
		self.Output     = None

		saga_cmd = None
	#	saga_cmd = 'F:/develop/saga/saga-code/master/saga-gis/bin/saga_vc_x64/saga_cmd.exe'	# define hard coded path to saga_cmd.exe
		if saga_cmd == None:
			head, tail = os.path.split(os.path.dirname(__file__))
			saga_cmd = head + os.sep + 'saga_cmd.exe'

		if os.path.isfile(saga_cmd) == True:
			self.saga_cmd =  saga_cmd	# use SAGA instance of this installation
		else:
			self.saga_cmd = 'saga_cmd'	# assuming saga_cmd is included in the PATH environment variable


	#####################################################
	# Execution
	#____________________________________________________
	def Run(self, bIgnoreLog = False):
		cmd = '\"' + self.saga_cmd + '\" -f=q ' + self.Library + ' ' + self.Tool
		for Item in self.Parameters:
			cmd += ' ' + Item

		if bIgnoreLog == False and DIR_LOG != None:
			cmd_out = open(DIR_LOG + os.sep + 'arcsaga.log'      , 'w')
			cmd_err = open(DIR_LOG + os.sep + 'arcsaga.error.log', 'w')
			cmd_out.write('_________________________\n')
			cmd_out.write(cmd)	# print to log file
			Result = subprocess.call(cmd, creationflags=CREATE_NO_WINDOW, stdout=cmd_out, stderr=cmd_err)
		else:
			Result = subprocess.call(cmd, creationflags=CREATE_NO_WINDOW)

		if Result != 0:
			arcpy.AddMessage('_________________________\n')
			arcpy.AddMessage(cmd)
			arcpy.AddError('...failed to run SAGA tool!')

		elif self.Output != None:
			for Data in self.Output:
				self.Get_Output(Data[0], Data[1], Data[2])

		if self.Temporary != None:
			for Data in self.Temporary:
				File_Remove_All(Data)

		return Result


	#####################################################
	# Data Lists
	#____________________________________________________
	def Add_Output(self, Identifier, File, Type):
		if self.Output == None:
			self.Output  = [[Identifier, File, Type]]
		else:
			self.Output += [[Identifier, File, Type]]
		return

	#____________________________________________________
	def Add_Temporary(self, File):
		if self.Temporary == None:
			self.Temporary  = [File]
		else:
			self.Temporary += [File]
		return

	#____________________________________________________
	def Get_Temporary(self, Extension):
		File = File_Get_TempName(Extension)
		self.Add_Temporary(File)
		return File


	#####################################################
	# Parameters
	#____________________________________________________
	def Set_Option(self, Identifier, Value):
		if Value != '#' and Value != None:
			Value = '\"' + Value.strip() + '\"'
			if self.Parameters == None:
				self.Parameters  = ['-' + Identifier, Value]
			else:
				self.Parameters += ['-' + Identifier, Value]
		return

	#____________________________________________________
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

	#____________________________________________________
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

	#____________________________________________________
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


#########################################################
# Raster Conversion
#________________________________________________________
def Arc_To_SAGA_Raster(Raster):
	Supported = ['sdat', 'tif', 'img', 'asc']
	for ext in Supported:
		if File_Cmp_Extension(Raster, ext):
			return Raster

	File   = File_Get_TempName('tif')
	Raster = ConversionUtils.ValidateInputRaster(Raster)
	ConversionUtils.CopyRasters(Raster, File, "")

	return File

#________________________________________________________
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


#########################################################
# Feature Conversion
#________________________________________________________
def Arc_To_SAGA_Feature(Feature):
	if File_Cmp_Extension(Feature, 'shp') == True:
		return Feature

	File = File_Get_TempName('.shp')
	ConversionUtils.CopyFeatures(Feature, File)

	return File

#________________________________________________________
def SAGA_To_Arc_Feature(File, Feature):
	if File_Cmp_Extension(Feature, 'shp') == False:
		ConversionUtils.CopyFeatures(File, Feature)
		File_Remove_All(File)

	Arc_Load_Layer(Feature)

	return True


#########################################################
# Table Conversion
#________________________________________________________
def Arc_To_SAGA_Table(Table):
	if File_Cmp_Extension(Table, 'dbf') == True:
		return Table

	File  = File_Get_TempName('.dbf')
	ConversionUtils.CopyRows(Table, File)

	return File

#________________________________________________________
def SAGA_To_Arc_Table(File, Table):
	if File_Cmp_Extension(Table, 'dbf') == False:
		ConversionUtils.CopyRows(File, Table)
		File_Remove_All(File)

	return True

	
#########################################################
# ArcMap Interaction
#________________________________________________________
def Arc_Load_Layer(Layer):
	if Layer == '#' or not Layer:
		return

	#____________________________________________________
	if sys.version_info.major < 3: # ArcGIS Desktop
		Map_Project = arcpy.mapping.MapDocument("CURRENT")
		Map_Frame   = arcpy.mapping.ListDataFrames(Map_Project)[0]
		Map_Layer   = arcpy.mapping.Layer(Layer)
		if Map_Layer.isRasterLayer:
			File = os.path.dirname(__file__) + os.sep + 'grid.lyr'
			if os.path.isfile(File) == True:
				Src_Layer = arcpy.mapping.Layer(File)
				arcpy.mapping.UpdateLayer(Map_Frame, Map_Layer, Src_Layer, True)
		arcpy.mapping.AddLayer(Map_Frame, Map_Layer, 'AUTO_ARRANGE')
		return

	#____________________________________________________
	Project = arcpy.mp.ArcGISProject("CURRENT")
	Map     = Project.listMaps()[0]
	Layer   = Map.addDataFromPath(Layer)
	return


#########################################################
#________________________________________________________
