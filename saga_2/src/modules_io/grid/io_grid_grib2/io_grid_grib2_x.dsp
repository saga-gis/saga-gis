# Microsoft Developer Studio Project File - Name="io_grid_grib2_x" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=io_grid_grib2_x - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "io_grid_grib2_x.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "io_grid_grib2_x.mak" CFG="io_grid_grib2_x - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "io_grid_grib2_x - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "io_grid_grib2_x - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "io_grid_grib2_x - Win32 Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "io_grid_grib2_x - Win32 Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "io_grid_grib2_x - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\..\..\..\..\bin\saga_vc\modules"
# PROP Intermediate_Dir ".\..\..\..\..\bin\tmp\saga_vc\io_grid_grib2_x"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(SAGA)/src/saga_core" /I "$(JASPER)/src/libjasper/include" /D "NDEBUG" /D "_USRDLL" /D "_MBCS" /D "_SAGA_VC" /D "WIN32" /D "_TYPEDEF_BYTE" /D "_TYPEDEF_WORD" /D "_SAGA_MSW" /D "JAS_WIN_MSVC_BUILD" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 libjasper.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib saga_api.lib /nologo /subsystem:windows /dll /machine:I386 /libpath:"$(SAGA)/bin/saga_vc" /libpath:"$(JASPER)/src/msvc/Win32_Release"

!ELSEIF  "$(CFG)" == "io_grid_grib2_x - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\..\..\..\..\bin\saga_vc_dbg\modules"
# PROP Intermediate_Dir ".\..\..\..\..\bin\tmp\saga_vc_dbg\io_grid_grib2_x"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(JASPER)/src/libjasper/include" /I "$(SAGA)/src/saga_core" /D "_DEBUG" /D "USE_JPEG2000" /D "_USRDLL" /D "_MBCS" /D "_SAGA_VC" /D "WIN32" /D "_TYPEDEF_BYTE" /D "_TYPEDEF_WORD" /D "_SAGA_MSW" /D "JAS_WIN_MSVC_BUILD" /YX /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libjasper.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib saga_api.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"$(SAGA)/bin/saga_vc_dbg" /libpath:"$(JASPER)/src/msvc/Win32_Debug"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "io_grid_grib2_x - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "io_grid_grib2_x___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "io_grid_grib2_x___Win32_Unicode_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "./../../../../bin/saga_vc_u_dbg/modules"
# PROP Intermediate_Dir "./../../../../bin/tmp/saga_vc_u_dbg/io_grid_grib2_x"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(SAGA)/src/saga_core" /D "_DEBUG" /D "WIN32" /D "_USRDLL" /D "_MBCS" /D "_TYPEDEF_BYTE" /D "_TYPEDEF_WORD" /D "_SAGA_MSW" /D "_SAGA_VC" /YX /FD /GZ /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(SAGA)/src/saga_core" /I "$(JASPER)/src/libjasper/include" /D "_DEBUG" /D "_UNICODE" /D "_SAGA_UNICODE" /D "_USRDLL" /D "_MBCS" /D "_SAGA_VC" /D "WIN32" /D "_TYPEDEF_BYTE" /D "_TYPEDEF_WORD" /D "_SAGA_MSW" /D "JAS_WIN_MSVC_BUILD" /YX /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib saga_api.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"$(SAGA)/bin/saga_vc_dbg"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 libjasper.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib saga_api.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"$(SAGA)/bin/saga_vc_u_dbg" /libpath:"$(JASPER)/src/msvc/Win32_Debug"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "io_grid_grib2_x - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "io_grid_grib2_x___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "io_grid_grib2_x___Win32_Unicode_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "./../../../../bin/saga_vc_u/modules"
# PROP Intermediate_Dir "./../../../../bin/tmp/saga_vc_u/io_grid_grib2_x"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "$(SAGA)/src/saga_core" /D "NDEBUG" /D "WIN32" /D "_USRDLL" /D "_MBCS" /D "_TYPEDEF_BYTE" /D "_TYPEDEF_WORD" /D "_SAGA_MSW" /D "_SAGA_VC" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(SAGA)/src/saga_core" /I "$(JASPER)/src/libjasper/include" /D "NDEBUG" /D "_UNICODE" /D "_SAGA_UNICODE" /D "_USRDLL" /D "_MBCS" /D "_SAGA_VC" /D "WIN32" /D "_TYPEDEF_BYTE" /D "_TYPEDEF_WORD" /D "_SAGA_MSW" /D "JAS_WIN_MSVC_BUILD" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib saga_api.lib /nologo /subsystem:windows /dll /machine:I386 /libpath:"$(SAGA)/bin/saga_vc"
# ADD LINK32 libjasper.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib saga_api.lib /nologo /subsystem:windows /dll /machine:I386 /libpath:"$(SAGA)/bin/saga_vc_u" /libpath:"$(JASPER)/src/msvc/Win32_Release"

!ENDIF 

# Begin Target

# Name "io_grid_grib2_x - Win32 Release"
# Name "io_grid_grib2_x - Win32 Debug"
# Name "io_grid_grib2_x - Win32 Unicode Debug"
# Name "io_grid_grib2_x - Win32 Unicode Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\grib2_import.cpp
# End Source File
# Begin Source File

SOURCE=.\MLB_Interface.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\grib2_import.h
# End Source File
# Begin Source File

SOURCE=.\MLB_Interface.h
# End Source File
# End Group
# Begin Group "Include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\saga_core\saga_api\api_core.h
# End Source File
# Begin Source File

SOURCE=..\..\..\saga_core\saga_api\dataobject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\saga_core\saga_api\doc_html.h
# End Source File
# Begin Source File

SOURCE=..\..\..\saga_core\saga_api\doc_pdf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\saga_core\saga_api\doc_svg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\saga_core\saga_api\geo_tools.h
# End Source File
# Begin Source File

SOURCE=..\..\..\saga_core\saga_api\grid.h
# End Source File
# Begin Source File

SOURCE=..\..\..\saga_core\saga_api\mat_tools.h
# End Source File
# Begin Source File

SOURCE=..\..\..\saga_core\saga_api\module.h
# End Source File
# Begin Source File

SOURCE=..\..\..\saga_core\saga_api\module_library.h
# End Source File
# Begin Source File

SOURCE=..\..\..\saga_core\saga_api\parameters.h
# End Source File
# Begin Source File

SOURCE=..\..\..\saga_core\saga_api\saga_api.h
# End Source File
# Begin Source File

SOURCE=..\..\..\saga_core\saga_api\shapes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\saga_core\saga_api\table.h
# End Source File
# Begin Source File

SOURCE=..\..\..\saga_core\saga_api\tin.h
# End Source File
# End Group
# Begin Group "g2clib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\g2clib-1.0.4\cmplxpack.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\compack.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\comunpack.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\dec_jpeg2000.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\dec_png.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\drstemplates.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\drstemplates.h"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\enc_jpeg2000.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\enc_png.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\g2_addfield.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\g2_addgrid.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\g2_addlocal.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\g2_create.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\g2_free.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\g2_getfld.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\g2_gribend.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\g2_info.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\g2_miss.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\g2_unpack1.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\g2_unpack2.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\g2_unpack3.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\g2_unpack4.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\g2_unpack5.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\g2_unpack6.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\g2_unpack7.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\gbits.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\getdim.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\getpoly.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\grib2.h"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\gridtemplates.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\gridtemplates.h"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\int_power.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\jpcpack.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\jpcunpack.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\misspack.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\mkieee.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\pack_gp.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\pdstemplates.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\pdstemplates.h"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\pngpack.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\pngunpack.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\rdieee.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\reduce.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\seekgb.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\simpack.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\simunpack.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\specpack.c"
# End Source File
# Begin Source File

SOURCE=".\g2clib-1.0.4\specunpack.c"
# End Source File
# End Group
# End Target
# End Project
