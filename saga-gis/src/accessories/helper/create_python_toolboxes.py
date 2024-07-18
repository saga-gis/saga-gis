#________________________________________
def install(package):
    import subprocess, sys; subprocess.check_call([sys.executable, '-m', 'pip', 'install', package])

#________________________________________
def Create_Python_Toolboxes(saga_directory):
    import os, sys
    saga_directory = os.path.abspath(saga_directory)
    sys.path.insert(1, saga_directory)
    print('\nPySAGA.tools creation\n>>> \'{:s}\'...'.format(saga_directory))
    from PySAGA import helper
    helper.Create_Toolboxes()
    print('ready')

#________________________________________
if __name__ == '__main__':
    import sys, os
    if len(sys.argv) == 2 and os.path.exists(sys.argv[1]):
        Create_Python_Toolboxes(sys.argv[1])
    else:
        Create_Python_Toolboxes(os.path.dirname(__file__) + '/../../../bin/saga_x64')
