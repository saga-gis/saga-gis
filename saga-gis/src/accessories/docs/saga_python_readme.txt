SAGA API - Python Interface

* MS-Windows Installation *

To use SAGA in your Python scripts you first have to define the path
to your SAGA installation. This can be done by a 'SAGA_PATH' environment
variable, either through the Windows' system or user environment
variable settings, or with a Python command in the individual Python
scripts itself. For the latter you can use a command like this

> import os; os.environ['SAGA_PATH'] = 'C:/saga-9.1.0_x64'

Another convenient possibility to define the SAGA installation path is
the '__init__.py' module of the PySAGA package folder, where you find
a 'SAGA_Path' variable for this purpose.

The second step is to make the PySAGA package available to your Python
scripts. Maybe the most convenient way to achieve this done by copying
the PySAGA folder to the 'Lib/site-packages/' folder of your Python
installation. This way all your Python scripts can easily import PySAGA.
If don't want to do this or if you don't have the rights to do so, you
can copy the PySAGA folder to the folder with the Python scripts in
which you want to work with PySAGA. A further alternative is to add
the path containing the PySAGA folder (e.g. the path to your SAGA
installation) to the PYTHONPATH environment variable. You also can do
this within your Python script with the following command (adjust the
path accordingly):

> import sys; sys.path.insert(1, 'C:/saga-9.1.0_x64')

With these two prerequisites in mind you can start using SAGA in your
Python scripts by importing 'saga_api' from the PySAGA package

> from PySAGA import saga_api

...and add a short test requesting the loaded tool libraries

> print(saga_api.SG_Get_Tool_Library_Manager().Get_Summary(saga_api.SG_SUMMARY_FMT_FLAT).c_str())
