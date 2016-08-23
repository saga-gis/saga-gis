#!/usr/bin/env python

# ----------------------------------------------------------------------
# environmental configuration file "environment.py" for the
# saga_cmd parameter interface generator "param_interface.py"

# copyright (C) 2015 by Volker Wichmann
# released under the GNU General Public License as published by the
# Free Software Foundation; version 2 of the License.

# purpose:
# definition of file paths for the saga_cmd executable, the SAGA_MLB
# environmental variable, and the name of the output file
#
# usage:
# edit the file paths to match the saga installation of which you like
# to generate the parameter interface
# ----------------------------------------------------------------------

import os

# ----------------------------------------------------------------------
if (os.name == 'posix'):
    SAGA_CMD = '/usr/local/bin/saga_cmd'
    SAGA_MLB = 'usr/local/lib/saga'
    OUTFILE  = '/home/vw/Desktop/saga_cmd_interface.txt'
else:
    SAGA_CMD = '"C:\\Program Files\\SAGA-GIS\\saga_cmd.exe"'
    SAGA_MLB = '"C:\\Program Files\\SAGA-GIS\\modules"'
    OUTFILE  = 'C:\\users\\vw\\Desktop\\saga_cmd_interface.txt'

