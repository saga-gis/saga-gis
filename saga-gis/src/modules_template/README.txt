###############################################################################
#                Using the SAGA module template on GNU/Linux                  #
###############################################################################

Autor(s): Volker Wichmann


This is a short description on how to use the source code template in order
to start the development of an own module library under GNU/Linux.

Change to the folder of the module_template (saga-gis/src/modules_template)
and execute the "create_custom_mlb_from_template.sh" script. It will prompt
you to provide a new module library name and module name.

Executed in that manner, the script will place a new module library template
in the "/saga-gis/src/modules_contrib" folder and will automatically refactor
the template sources using the names provided by the user.

For example, the call

./create_custom_mlb_from_template.sh contrib_your_name example_module

will create a new module library "contrib_your_name" with a module called
"example_module" within the "/saga-gis/src/modules_contrib" folder.

In case you are using the GNU building system, you have to edit the following two
files in order to make your template compile:

* edit the "Makefile.am" in "/saga-gis/src/modules_contrib" and add your module
library to the SUBDIRS, e.g.:

SUBDIRS = contrib_a_perego contrib_s_liersch garden contrib_your_name

* edit "configure.in" in "/saga-gis" and add your module library above the
last line of the file, e.g.:

	src/modules_contrib/contrib_your_name/Makefile \

Now you can run automake, configure and make in order to compile your
template, e.g.:

autoreconf -i
./configure --enable-unicode
make
make install

