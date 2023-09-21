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
# Purpose
#################################################################################

'''
The PySAGA.data.helper module provides a collection of functions supporting data access.
'''


#################################################################################
# Globals
#________________________________________________________________________________

import os; from PySAGA import saga_api


#################################################################################
#
# Downloads...
#________________________________________________________________________________

#________________________________________________________________________________
def Get_File(File, Local_Dir, Remote_Dir, Download_Retries = 4, Overwrite = False):
    '''
    If *Overwrite* is False this function checks if the requested 'File' exists in
    *Local_Dir* before it tries to download the file from *Remote_Dir* to *Local_Dir*.
    The function returns None if the file does not exist and the attempt to download
    it failed or the full file path to the local file otherwise.
    This function uses the *wget* Python module to download a requested file from
    internet. Any download will fail if *wget* is not installed.
    Installation can be done through pip:
        pip install wget
    '''

    Local_File = '{:s}/{:s}'.format(Local_Dir, File)
    if Overwrite == False and os.path.exists(Local_File):
        return Local_File

    if not os.path.exists(Local_Dir):
        os.makedirs(Local_Dir)

    Remote_File = '{:s}/{:s}'.format(Remote_Dir, File)

    Retry = Download_Retries
    if Retry < 0:
        Retry = 0
    Retry += 1

    try:
        import wget
    except:
        saga_api.SG_UI_Console_Print_StdErr('failed to import wget module! is it installed?')
        return None

    while Retry > 0:
        try:
            import wget
            Local_File = wget.download(Remote_File, Local_Dir) # returns full path to downloaded file or 'None'
            if Local_File:
                saga_api.SG_UI_Console_Print_StdOut('')
                return Local_File
            Retry -= 1
        except: # remote file might not exist or internet connection is not available
            break

    saga_api.SG_UI_Console_Print_StdErr('failed to access remote file: ' + Remote_File)
    return None


#################################################################################
#
#________________________________________________________________________________
