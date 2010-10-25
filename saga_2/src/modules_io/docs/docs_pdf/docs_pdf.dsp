# Microsoft Developer Studio Project File - Name="docs_pdf" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=docs_pdf - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "docs_pdf.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "docs_pdf.mak" CFG="docs_pdf - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "docs_pdf - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "docs_pdf - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "docs_pdf - Win32 Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "docs_pdf - Win32 Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "docs_pdf - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\..\..\..\..\bin\saga_vc\modules"
# PROP Intermediate_Dir ".\..\..\..\..\bin\tmp\saga_vc\docs_pdf"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(SAGA)/src/saga_core" /I "$(HARU)/include" /D "NDEBUG" /D "_USRDLL" /D "_MBCS" /D "_SAGA_VC" /D "WIN32" /D "_TYPEDEF_BYTE" /D "_TYPEDEF_WORD" /D "_SAGA_MSW" /D "docs_pdf_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib saga_api.lib libhpdf.lib wxpng.lib wxzlib.lib /nologo /subsystem:windows /dll /machine:I386 /libpath:"$(SAGA)/bin/saga_vc" /libpath:"$(HARU)" /libpath:"$(WXWIN)/lib/vc6_dll"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy     *.lng.txt     ..\..\..\..\bin\saga_vc\modules\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "docs_pdf - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\..\..\..\..\bin\saga_vc_dbg\modules"
# PROP Intermediate_Dir ".\..\..\..\..\bin\tmp\saga_vc_dbg\docs_pdf"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(SAGA)/src/saga_core" /I "$(HARU)/include" /D "_DEBUG" /D "_USRDLL" /D "_MBCS" /D "_SAGA_VC" /D "WIN32" /D "_TYPEDEF_BYTE" /D "_TYPEDEF_WORD" /D "_SAGA_MSW" /D "docs_pdf_EXPORTS" /YX /FD /GZ /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib saga_api.lib libhpdf.lib wxpngd.lib wxzlibd.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"$(SAGA)/bin/saga_vc_dbg" /libpath:"$(HARU)" /libpath:"$(WXWIN)/lib/vc6_dll"

!ELSEIF  "$(CFG)" == "docs_pdf - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "docs_pdf___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "docs_pdf___Win32_Unicode_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "./../../../../bin/saga_vc_u_dbg/modules"
# PROP Intermediate_Dir "./../../../../bin/tmp/saga_vc_u_dbg/docs_pdf"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(SAGA)/src/saga_core" /D "_DEBUG" /D "WIN32" /D "_USRDLL" /D "_MBCS" /D "_TYPEDEF_BYTE" /D "_TYPEDEF_WORD" /D "_SAGA_MSW" /D "_SAGA_VC" /YX /FD /GZ /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(SAGA)/src/saga_core" /I "$(HARU)/include" /D "_DEBUG" /D "_UNICODE" /D "_SAGA_UNICODE" /D "_USRDLL" /D "_MBCS" /D "_SAGA_VC" /D "WIN32" /D "_TYPEDEF_BYTE" /D "_TYPEDEF_WORD" /D "_SAGA_MSW" /D "docs_pdf_EXPORTS" /YX /FD /GZ /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib saga_api.lib libhpdf.lib wxpngd.lib wxzlibd.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"$(SAGA)/bin/saga_vc_u_dbg" /libpath:"$(HARU)" /libpath:"$(WXWIN)/lib/vc6_dll"

!ELSEIF  "$(CFG)" == "docs_pdf - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "docs_pdf___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "docs_pdf___Win32_Unicode_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "./../../../../bin/saga_vc_u/modules"
# PROP Intermediate_Dir "./../../../../bin/tmp/saga_vc_u/docs_pdf"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "$(SAGA)/src/saga_core" /D "NDEBUG" /D "WIN32" /D "_USRDLL" /D "_MBCS" /D "_TYPEDEF_BYTE" /D "_TYPEDEF_WORD" /D "_SAGA_MSW" /D "_SAGA_VC" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(SAGA)/src/saga_core" /I "$(HARU)/include" /D "NDEBUG" /D "_UNICODE" /D "_SAGA_UNICODE" /D "_USRDLL" /D "_MBCS" /D "_SAGA_VC" /D "WIN32" /D "_TYPEDEF_BYTE" /D "_TYPEDEF_WORD" /D "_SAGA_MSW" /D "docs_pdf_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib saga_api.lib /nologo /subsystem:windows /dll /machine:I386 /libpath:"$(SAGA)/bin/saga_vc"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib saga_api.lib libhpdf.lib wxpng.lib wxzlib.lib /nologo /subsystem:windows /dll /machine:I386 /libpath:"$(SAGA)/bin/saga_vc_u" /libpath:"$(HARU)" /libpath:"$(WXWIN)/lib/vc6_dll"

!ENDIF 

# Begin Target

# Name "docs_pdf - Win32 Release"
# Name "docs_pdf - Win32 Debug"
# Name "docs_pdf - Win32 Unicode Debug"
# Name "docs_pdf - Win32 Unicode Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\doc_pdf.cpp
# End Source File
# Begin Source File

SOURCE=.\MLB_Interface.cpp
# End Source File
# Begin Source File

SOURCE=.\Polygon_Clipper.cpp
# End Source File
# Begin Source File

SOURCE=.\Profile_Cross_Sections.cpp
# End Source File
# Begin Source File

SOURCE=.\Shapes_Report.cpp
# End Source File
# Begin Source File

SOURCE=.\Shapes_Summary.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\doc_pdf.h
# End Source File
# Begin Source File

SOURCE=.\MLB_Interface.h
# End Source File
# Begin Source File

SOURCE=.\Polygon_Clipper.h
# End Source File
# Begin Source File

SOURCE=.\Profile_Cross_Sections.h
# End Source File
# Begin Source File

SOURCE=.\Shapes_Report.h
# End Source File
# Begin Source File

SOURCE=.\Shapes_Summary.h
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

SOURCE=..\..\..\saga_core\saga_api\table_value.h
# End Source File
# Begin Source File

SOURCE=..\..\..\saga_core\saga_api\tin.h
# End Source File
# End Group
# End Target
# End Project
