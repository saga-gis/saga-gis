#! /usr/bin/env python

import sys; from PySAGA import saga_api

print('_______')
print('Python-' + sys.version)
print('SAGA-{:s} (loaded {:d} libraries, {:d} tools)'.format(saga_api.SAGA_VERSION,
	saga_api.SG_Get_Tool_Library_Manager().Get_Count(),
	saga_api.SG_Get_Tool_Library_Manager().Get_Tool_Count()
))
