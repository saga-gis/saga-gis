#! /usr/bin/env python

import os

if os.name == 'nt': # Windows
	if os.getenv('SAGA_PATH') is None:
		os.environ['SAGA_PATH'] = 'D:/saga/saga-code/master/saga-gis/bin/saga_vc_win32'
	os.environ['PATH'     ] = os.environ['SAGA_PATH'] + os.sep +    ';' + os.environ['PATH']
	os.environ['PATH'     ] = os.environ['SAGA_PATH'] + os.sep + 'dll;' + os.environ['PATH']
	os.environ['PROJ_LIB' ] = os.environ['SAGA_PATH'] + os.sep + 'dll' + os.sep + 'proj-data'
	os.environ['GDAL-DATA'] = os.environ['SAGA_PATH'] + os.sep + 'dll' + os.sep + 'gdal-data'

import sys, saga_api


#########################################
def Initialize(Verbose):
	if os.name == 'nt': # Windows
		Load_Tool_Libraries(os.environ['SAGA_PATH'] + os.sep + 'tools')
	else:               # Linux
		Load_Tool_Libraries('/usr/local/lib/saga')

	if os.getenv('SAGA_TLB') is not None:
		Load_Tool_Libraries(os.environ['SAGA_TLB'])

	if os.getenv('SAGA_MLB') is not None:
		Load_Tool_Libraries(os.environ['SAGA_MLB'])

	if Verbose == True:
		print('_______')
		print('Python - Version ' + sys.version)
		print(saga_api.SAGA_API_Get_Version())
		print('number of loaded libraries: ' + str(saga_api.SG_Get_Tool_Library_Manager().Get_Count()))
		print('_______')

	return True


#########################################
def Load_Tool_Libraries(Directory):
	saga_api.SG_UI_Msg_Lock(True)
	saga_api.SG_Get_Tool_Library_Manager().Add_Directory(Directory, False)
	saga_api.SG_UI_Msg_Lock(False)

	return True

	
#########################################
