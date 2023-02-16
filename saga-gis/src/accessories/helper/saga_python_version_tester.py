#! /usr/bin/env python

import saga_helper, saga_api, sys

if False:
	print('_______')
	print('Python-' + sys.version)
	print('SAGA-{:s} (loaded {:d} libraries, {:d} tools)'.format(saga_api.SAGA_VERSION,
		saga_api.SG_Get_Tool_Library_Manager().Get_Count(),
		saga_api.SG_Get_Tool_Library_Manager().Get_Tool_Count()
	))
else:
	saga_helper.Initialize(True)
