SAGA API - Python Interface
* MS-Windows Installation *

To load the SAGA API from a Python script you have to import the saga_api.py module
> import saga_api


- The SAGA API DLL (Dynamic Link Library) must be in the PATH
- 

Windows: Let the 'SAGA_PATH' environment variable point to
the SAGA installation folder before importing 'saga_api'!
This can be defined globally in the Windows system or
user environment variable settings, in the 'PySAGA/__init__.py'
file, or in the individual Python script itself. To do the latter
just uncomment the following line and adjust the path accordingly:
> import os; os.environ['SAGA_PATH'] = os.path.split(os.path.dirname(__file__))[0]

If you have not copied the PySAGA folder to your Python's 'Lib/site-packages/' folder
you can alternatively add the path containing PySAGA (i.e. the 'SAGA_PATH') to
the Python's package paths here:
> import sys; sys.path.insert(1, os.environ['SAGA_PATH'])

Initialize the SAGA environment (also loads all tools by default) and import the saga_api
> import PySAGA; PySAGA.Initialize(True); import PySAGA.saga_api as saga



import os; os.environ['SAGA_PATH'] = 'F:/develop/saga/saga-code/master/saga-gis/bin/saga_x64'

# import sys; sys.path.insert(1, os.environ['SAGA_PATH'])

import PySAGA; PySAGA.Initialize(); import PySAGA.saga_api as saga

print(saga.SG_Get_Tool_Library_Manager().Get_Summary(saga.SG_SUMMARY_FMT_FLAT).c_str())
