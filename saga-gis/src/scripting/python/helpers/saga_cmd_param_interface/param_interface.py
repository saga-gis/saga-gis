#!/usr/bin/env python

# ----------------------------------------------------------------------
# saga_cmd parameter interface generator "param_interface.py"

# copyright (C) 2015 by Volker Wichmann
# released under the GNU General Public License as published by the
# Free Software Foundation; version 2 of the License.

# purpose:
# generate a simple text-based description of the saga_cmd parameter
# interface of a SAGA binary installation.
# this allows to detect changes in the parameter interface of different
# SAGA versions.
#
# usage:
# in order to run this script, you have to edit the "environment.py"
# file to match the file paths of the SAGA installation you like to
# investigate.
# the editing of the "tools.py" is usually not necessary as it contains
# all SAGA tools available and tools that not found are silently
# skipped.
#
# run the script twice on two different SAGA installations in order to
# generate a description of the saga_cmd parameter interface for each
# version.
# with a diff (compare) of both output files it is possible to easily
# detect changes in the saga_cmd parameter interface of individual
# tools.
# ----------------------------------------------------------------------

import os, subprocess, sys, environment, tools

# ----------------------------------------------------------------------
os.putenv('SAGA_MLB', environment.SAGA_MLB)

outfile = open(environment.OUTFILE, 'wb')

for tool in range(len(tools.tools)):
    bPrint = False
    
    try:
        cmd = subprocess.Popen(environment.SAGA_CMD + ' ' +
        tools.tools[tool][0] + ' ' + str(tools.tools[tool][1]),
        shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        
        for line in cmd.stdout:
            if 'Error:' in line:
                break
            if 'Usage:' in line:
                bPrint = True
                outfile.write('<TOOL>\n')
                outfile.write(tools.tools[tool][2] + '\n')
            if bPrint:
                outfile.write(' '.join(line.split()) + '\n')
                
        if bPrint:
            outfile.write('</TOOL>\n\n')
        
    except Exception, e:
        print 'ERROR: Could not execute command - %s\n' % e
        sys.exit(1)

outfile.close
