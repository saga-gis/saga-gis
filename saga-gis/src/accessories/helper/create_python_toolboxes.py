#________________________________________
def install(package):
    import subprocess, sys; subprocess.check_call([sys.executable, '-m', 'pip', 'install', package])

#________________________________________
def Create_Python_Toolboxes(dir_tool, dir_saga):
    print('\nPySAGA.tools creation started')

    import os, sys

    if not dir_saga:
        dir_saga = (os.path.dirname(__file__) + '/../../../bin/saga_x64')

    if not os.path.exists(dir_saga):
        print('Error: SAGA path does not exist: \'{:s}\''.format(dir_saga))
        return False

    dir_saga = os.path.abspath(dir_saga)
    print(' - SAGA directory is: \'{:s}\''.format(dir_saga))

    if not dir_tool:
        dir_tool = dir_saga + '/PySAGA/tools'

    dir_tool = os.path.abspath(dir_tool)
    print(' - Tool directory is: \'{:s}\''.format(dir_tool))

    sys.path.insert(1, dir_saga)
    from PySAGA import helper
    helper.Create_Toolboxes(dir_tool)

    print('\nPySAGA.tools creation finished')
    return True

#________________________________________
if __name__ == '__main__':
    import sys, os; dir_tool = None; dir_saga = None

    if len(sys.argv) > 2 and os.path.exists(sys.argv[2]):
        dir_saga = sys.argv[2]

    if len(sys.argv) > 1:
        dir_tool = sys.argv[1]

    Create_Python_Toolboxes(dir_tool, dir_saga)
