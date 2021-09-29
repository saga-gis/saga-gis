#! /usr/bin/env python

import os, sys

if os.name == 'nt': # Windows
	if os.getenv('SAGA_PATH') is None:
		os.environ['SAGA_PATH'] = 'F:/develop/saga/saga-code/master/saga-gis/bin/saga_x64'
	if 'add_dll_directory' in dir(os):
		os.add_dll_directory(os.environ['SAGA_PATH'])
	else:
		os.environ['PATH'] = os.environ['SAGA_PATH'] + os.sep + ';' + os.environ['PATH']

import saga_api


#########################################
bInitialized = False

def Initialize(Verbose):
	global bInitialized
	if not bInitialized:
		bInitialized = True
		if os.name == 'nt': # Windows
			saga_api.SG_Initialize_Environment(True, True, os.environ['SAGA_PATH'])
		else:               # Linux
			saga_api.SG_Initialize_Environment(True, True)

	if Verbose == True:
		print('_______')
		print('Python - Version ' + sys.version)
		print(saga_api.SAGA_API_Get_Version())
		print('number of loaded libraries: ' + str(saga_api.SG_Get_Tool_Library_Manager().Get_Count()))
		print('_______')

	return True

	
#########################################
