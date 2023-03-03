#! /usr/bin/env python

#################################################################################
# MIT License

# Copyright (c) 2023 Olaf Conrad

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#################################################################################


#_________________________________________
##########################################
import os, sys

if os.name == 'nt': # Windows
	if not os.getenv('SAGA_PATH'):
		os.environ['SAGA_PATH'] = 'F:/develop/saga/saga-code/master/saga-gis/bin/saga_x64'
	if 'add_dll_directory' in dir(os):
		os.add_dll_directory(os.environ['SAGA_PATH'])
	else:
		os.environ['PATH'] = os.environ['SAGA_PATH'] + os.sep + ';' + os.environ['PATH']

import saga_api


#_________________________________________
##########################################
bInitialized = False

def Initialize(Verbose = True):
	global bInitialized
	if not bInitialized:
		bInitialized = True
		if os.name == 'nt': # Windows
			saga_api.SG_Initialize_Environment(True, True, os.environ['SAGA_PATH'])
		else:               # Linux
			saga_api.SG_Initialize_Environment(True, True)

	if Verbose == True:
		print('_______')
		print('Python-' + sys.version)
		print('SAGA-{:s} (loaded {:d} libraries, {:d} tools)'.format(saga_api.SAGA_VERSION,
			saga_api.SG_Get_Tool_Library_Manager().Get_Count(),
			saga_api.SG_Get_Tool_Library_Manager().Get_Tool_Count()
		))
		print('_______')

	return True

	
#_________________________________________
##########################################
