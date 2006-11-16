# Microsoft Developer Studio Project File - Name="shapes_tools" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=shapes_tools - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "shapes_tools.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "shapes_tools.mak" CFG="shapes_tools - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "shapes_tools - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "shapes_tools - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "shapes_tools - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\..\..\..\..\bin\saga_vc\modules"
# PROP Intermediate_Dir ".\..\..\..\..\bin\tmp\saga_vc\shapes_tools"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(SAGA)/src/saga_core" /D "NDEBUG" /D "WIN32" /D "_USRDLL" /D "_MBCS" /D "_TYPEDEF_BYTE" /D "_TYPEDEF_WORD" /D "_SAGA_MSW" /D "_SAGA_VC" /D "__WXMSW__" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib saga_api.lib /nologo /subsystem:windows /dll /machine:I386 /libpath:"$(SAGA)/bin/saga_vc"

!ELSEIF  "$(CFG)" == "shapes_tools - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\..\..\..\..\bin\saga_vc_dbg\modules"
# PROP Intermediate_Dir ".\..\..\..\..\bin\tmp\saga_vc_dbg\shapes_tools"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(SAGA)/src/saga_core" /D "_DEBUG" /D "WIN32" /D "_USRDLL" /D "_MBCS" /D "_TYPEDEF_BYTE" /D "_TYPEDEF_WORD" /D "_SAGA_MSW" /D "_SAGA_VC" /YX /FD /GZ /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib saga_api.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"$(SAGA)/bin/saga_vc_dbg"

!ENDIF 

# Begin Target

# Name "shapes_tools - Win32 Release"
# Name "shapes_tools - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CreateChartLayer.cpp
# End Source File
# Begin Source File

SOURCE=.\CreateWebContent.cpp
# End Source File
# Begin Source File

SOURCE=.\GraticuleBuilder.cpp
# End Source File
# Begin Source File

SOURCE=.\Intersection_GPC.c
# End Source File
# Begin Source File

SOURCE=.\MLB_Interface.cpp
# End Source File
# Begin Source File

SOURCE=.\NewLayerFromSelectedShapes.cpp
# End Source File
# Begin Source File

SOURCE=.\QueryBuilder.cpp
# End Source File
# Begin Source File

SOURCE=.\QueryParser.cpp
# End Source File
# Begin Source File

SOURCE=.\SearchInTable.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectByTheme.cpp
# End Source File
# Begin Source File

SOURCE=.\SeparateShapes.cpp
# End Source File
# Begin Source File

SOURCE=.\Shapes_Assign_Table.cpp
# End Source File
# Begin Source File

SOURCE=.\Shapes_Create_Empty.cpp
# End Source File
# Begin Source File

SOURCE=.\shapes_cut.cpp
# End Source File
# Begin Source File

SOURCE=.\shapes_cut_interactive.cpp
# End Source File
# Begin Source File

SOURCE=.\Shapes_Merge.cpp
# End Source File
# Begin Source File

SOURCE=.\Shapes_Report.cpp
# End Source File
# Begin Source File

SOURCE=.\shapes_split.cpp
# End Source File
# Begin Source File

SOURCE=.\ShapeSearchSelector.cpp
# End Source File
# Begin Source File

SOURCE=.\ShapeSelector.cpp
# End Source File
# Begin Source File

SOURCE=.\Summarize.cpp
# End Source File
# Begin Source File

SOURCE=.\SummaryPDFDocEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\TransformShapes.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CreateChartLayer.h
# End Source File
# Begin Source File

SOURCE=.\CreateWebContent.h
# End Source File
# Begin Source File

SOURCE=.\GraticuleBuilder.h
# End Source File
# Begin Source File

SOURCE=.\Intersection_GPC.h
# End Source File
# Begin Source File

SOURCE=.\MLB_Interface.h
# End Source File
# Begin Source File

SOURCE=.\NewLayerFromSelectedShapes.h
# End Source File
# Begin Source File

SOURCE=.\QueryBuilder.h
# End Source File
# Begin Source File

SOURCE=.\QueryParser.h
# End Source File
# Begin Source File

SOURCE=.\SearchInTable.h
# End Source File
# Begin Source File

SOURCE=.\SelectByTheme.h
# End Source File
# Begin Source File

SOURCE=.\SeparateShapes.h
# End Source File
# Begin Source File

SOURCE=.\Shapes_Assign_Table.h
# End Source File
# Begin Source File

SOURCE=.\Shapes_Create_Empty.h
# End Source File
# Begin Source File

SOURCE=.\shapes_cut.h
# End Source File
# Begin Source File

SOURCE=.\shapes_cut_interactive.h
# End Source File
# Begin Source File

SOURCE=.\Shapes_Merge.h
# End Source File
# Begin Source File

SOURCE=.\Shapes_Report.h
# End Source File
# Begin Source File

SOURCE=.\shapes_split.h
# End Source File
# Begin Source File

SOURCE=.\ShapeSearchSelector.h
# End Source File
# Begin Source File

SOURCE=.\ShapeSelector.h
# End Source File
# Begin Source File

SOURCE=.\Summarize.h
# End Source File
# Begin Source File

SOURCE=.\SummaryPDFDocEngine.h
# End Source File
# Begin Source File

SOURCE=.\TransformShapes.h
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

SOURCE=..\..\..\saga_core\saga_api\pdf_document.h
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
# End Target
# End Project
