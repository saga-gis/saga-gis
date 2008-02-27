# Microsoft Developer Studio Project File - Name="saga_api" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=saga_api - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "saga_api.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "saga_api.mak" CFG="saga_api - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "saga_api - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "saga_api - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "saga_api - Win32 Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "saga_api - Win32 Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "saga_api - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "./../../../bin/saga_vc"
# PROP Intermediate_Dir "./../../../bin/tmp/saga_vc/saga_api"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "saga_api_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(WXWIN)/lib/vc_dll/msw" /I "$(HARU)/include" /I "$(WXWIN)/include" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "__WXMSW__" /D "_SAGA_API_EXPORTS" /D "WIN32" /D "WXUSINGDLL" /D "_TYPEDEF_BYTE" /D "_TYPEDEF_WORD" /D "_SAGA_MSW" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 wxmsw28_core.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib libhpdf.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /dll /machine:I386 /libpath:"$(HARU)" /libpath:"$(WXWIN)/lib/vc_dll"

!ELSEIF  "$(CFG)" == "saga_api - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "./../../../bin/saga_vc_dbg"
# PROP Intermediate_Dir "./../../../bin/tmp/saga_vc_dbg/saga_api"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "saga_api_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(WXWIN)/lib/vc_dll/mswd" /I "$(HARU)/include" /I "$(WXWIN)/include" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "__WXMSW__" /D "_SAGA_API_EXPORTS" /D "WIN32" /D "_DEBUG" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_TYPEDEF_BYTE" /D "_TYPEDEF_WORD" /D "_SAGA_MSW" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wxmsw28d_core.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib libhpdf.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"$(HARU)" /libpath:"$(WXWIN)/lib/vc_dll"

!ELSEIF  "$(CFG)" == "saga_api - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "saga_api___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "saga_api___Win32_Unicode_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "./../../../bin/saga_vc_u_dbg"
# PROP Intermediate_Dir "./../../../bin/tmp/saga_vc_u_dbg/saga_api"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(WXWIN)/include" /I "$(WXWIN)/lib/vc_lib/mswd" /I "$(HARU)" /I "$(HARU)/msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "__WXMSW__" /D "__WXDEBUG__" /D "_SAGA_API_EXPORTS" /D "_TYPEDEF_BYTE" /D "_TYPEDEF_WORD" /D "_SAGA_MSW" /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(WXWIN)/lib/vc_lib/mswud" /I "$(WXWIN)/lib/vc_dll/mswud" /I "$(HARU)/include" /I "$(WXWIN)/include" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_SAGA_API_EXPORTS" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_DEBUG" /D "WIN32" /D "_UNICODE" /D "_TYPEDEF_BYTE" /D "_TYPEDEF_WORD" /D "_SAGA_MSW" /D "_SAGA_UNICODE" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28d_core.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib libharu.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"$(WXWIN)/lib/vc_lib" /libpath:"$(HARU)"
# ADD LINK32 wxmsw28ud_core.lib wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib libhpdf.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"$(HARU)" /libpath:"$(WXWIN)/lib/vc_dll"

!ELSEIF  "$(CFG)" == "saga_api - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "saga_api___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "saga_api___Win32_Unicode_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "./../../../bin/saga_vc_u"
# PROP Intermediate_Dir "./../../../bin/tmp/saga_vc_u/saga_api"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "$(WXWIN)/include" /I "$(WXWIN)/lib/vc_lib/msw" /I "$(HARU)" /I "$(HARU)/msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "__WXMSW__" /D "_SAGA_API_EXPORTS" /D "_TYPEDEF_BYTE" /D "_TYPEDEF_WORD" /D "_SAGA_MSW" /FR /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(WXWIN)/lib/vc_lib/mswu" /I "$(WXWIN)/lib/vc_dll/mswu" /I "$(HARU)/include" /I "$(WXWIN)/include" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_SAGA_API_EXPORTS" /D "__WXMSW__" /D "WIN32" /D "WXUSINGDLL" /D "_UNICODE" /D "_TYPEDEF_BYTE" /D "_TYPEDEF_WORD" /D "_SAGA_MSW" /D "_SAGA_UNICODE" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28_core.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib libharu.lib /nologo /dll /machine:I386 /libpath:"$(WXWIN)/lib/vc_lib" /libpath:"$(HARU)"
# ADD LINK32 wxmsw28u_core.lib wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib libhpdf.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /dll /machine:I386 /libpath:"$(HARU)" /libpath:"$(WXWIN)/lib/vc_dll"

!ENDIF 

# Begin Target

# Name "saga_api - Win32 Release"
# Name "saga_api - Win32 Debug"
# Name "saga_api - Win32 Unicode Debug"
# Name "saga_api - Win32 Unicode Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\api_callback.cpp
# End Source File
# Begin Source File

SOURCE=.\api_colors.cpp
# End Source File
# Begin Source File

SOURCE=.\api_core.cpp
# End Source File
# Begin Source File

SOURCE=.\api_file.cpp
# End Source File
# Begin Source File

SOURCE=.\api_history.cpp
# End Source File
# Begin Source File

SOURCE=.\api_memory.cpp
# End Source File
# Begin Source File

SOURCE=.\api_string.cpp
# End Source File
# Begin Source File

SOURCE=.\api_translator.cpp
# End Source File
# Begin Source File

SOURCE=.\dataobject.cpp
# End Source File
# Begin Source File

SOURCE=.\doc_html.cpp
# End Source File
# Begin Source File

SOURCE=.\doc_pdf.cpp
# End Source File
# Begin Source File

SOURCE=.\doc_svg.cpp
# End Source File
# Begin Source File

SOURCE=.\geo_classes.cpp
# End Source File
# Begin Source File

SOURCE=.\geo_functions.cpp
# End Source File
# Begin Source File

SOURCE=.\grid.cpp
# End Source File
# Begin Source File

SOURCE=.\grid_io.cpp
# End Source File
# Begin Source File

SOURCE=.\grid_memory.cpp
# End Source File
# Begin Source File

SOURCE=.\grid_operation.cpp
# End Source File
# Begin Source File

SOURCE=.\grid_system.cpp
# End Source File
# Begin Source File

SOURCE=.\mat_formula.cpp
# End Source File
# Begin Source File

SOURCE=.\mat_grid_radius.cpp
# End Source File
# Begin Source File

SOURCE=.\mat_indexing.cpp
# End Source File
# Begin Source File

SOURCE=.\mat_matrix.cpp
# End Source File
# Begin Source File

SOURCE=.\mat_regression.cpp
# End Source File
# Begin Source File

SOURCE=.\mat_regression_multiple.cpp
# End Source File
# Begin Source File

SOURCE=.\mat_spline.cpp
# End Source File
# Begin Source File

SOURCE=.\mat_tools.cpp
# End Source File
# Begin Source File

SOURCE=.\mat_trend.cpp
# End Source File
# Begin Source File

SOURCE=.\module.cpp
# End Source File
# Begin Source File

SOURCE=.\module_grid.cpp
# End Source File
# Begin Source File

SOURCE=.\module_grid_interactive.cpp
# End Source File
# Begin Source File

SOURCE=.\module_interactive.cpp
# End Source File
# Begin Source File

SOURCE=.\module_interactive_base.cpp
# End Source File
# Begin Source File

SOURCE=.\module_library.cpp
# End Source File
# Begin Source File

SOURCE=.\module_library_interface.cpp
# End Source File
# Begin Source File

SOURCE=.\parameter.cpp
# End Source File
# Begin Source File

SOURCE=.\parameter_data.cpp
# End Source File
# Begin Source File

SOURCE=.\parameters.cpp
# End Source File
# Begin Source File

SOURCE=.\saga_api.cpp
# End Source File
# Begin Source File

SOURCE=.\shape.cpp
# End Source File
# Begin Source File

SOURCE=.\shape_line.cpp
# End Source File
# Begin Source File

SOURCE=.\shape_point.cpp
# End Source File
# Begin Source File

SOURCE=.\shape_points.cpp
# End Source File
# Begin Source File

SOURCE=.\shape_polygon.cpp
# End Source File
# Begin Source File

SOURCE=.\shapes.cpp
# End Source File
# Begin Source File

SOURCE=.\shapes_io.cpp
# End Source File
# Begin Source File

SOURCE=.\shapes_search.cpp
# End Source File
# Begin Source File

SOURCE=.\shapes_selection.cpp
# End Source File
# Begin Source File

SOURCE=.\table.cpp
# End Source File
# Begin Source File

SOURCE=.\table_dbase.cpp
# End Source File
# Begin Source File

SOURCE=.\table_io.cpp
# End Source File
# Begin Source File

SOURCE=.\table_record.cpp
# End Source File
# Begin Source File

SOURCE=.\table_selection.cpp
# End Source File
# Begin Source File

SOURCE=.\tin.cpp
# End Source File
# Begin Source File

SOURCE=.\tin_elements.cpp
# End Source File
# Begin Source File

SOURCE=.\tin_triangulation.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\API_Core.h
# End Source File
# Begin Source File

SOURCE=.\DataObject.h
# End Source File
# Begin Source File

SOURCE=.\doc_html.h
# End Source File
# Begin Source File

SOURCE=.\doc_pdf.h
# End Source File
# Begin Source File

SOURCE=.\doc_svg.h
# End Source File
# Begin Source File

SOURCE=.\GEO_Tools.h
# End Source File
# Begin Source File

SOURCE=.\Grid.h
# End Source File
# Begin Source File

SOURCE=.\MAT_Tools.h
# End Source File
# Begin Source File

SOURCE=.\Module.h
# End Source File
# Begin Source File

SOURCE=.\module_library.h
# End Source File
# Begin Source File

SOURCE=.\Parameters.h
# End Source File
# Begin Source File

SOURCE=.\saga_api.h
# End Source File
# Begin Source File

SOURCE=.\Shapes.h
# End Source File
# Begin Source File

SOURCE=.\Table.h
# End Source File
# Begin Source File

SOURCE=.\table_dbase.h
# End Source File
# Begin Source File

SOURCE=.\table_value.h
# End Source File
# Begin Source File

SOURCE=.\TIN.h
# End Source File
# End Group
# End Target
# End Project
