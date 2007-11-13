# Microsoft Developer Studio Project File - Name="saga_cmd" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=saga_cmd - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "saga_cmd.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "saga_cmd.mak" CFG="saga_cmd - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "saga_cmd - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "saga_cmd - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "saga_cmd - Win32 Unicode Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "saga_cmd - Win32 Unicode Release" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "saga_cmd - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\saga_cmd"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "./../../../bin/saga_vc"
# PROP Intermediate_Dir "./../../../bin/tmp/saga_vc/saga_cmd"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\include" /I ".\..\..\lib\vc_lib\msw" /I "." /D "WIN32" /D "__WXMSW__" /D "_CONSOLE" /D wxUSE_GUI=0 /Fd"vc_msw\saga_cmd.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W3 /GR /O2 /I "./.." /I "$(WXWIN)/include" /I "$(WXWIN)/lib/vc_dll/msw" /D "__WXMSW__" /D "_CONSOLE" /D wxUSE_GUI=0 /D "WIN32" /D "WXUSINGDLL" /D "_TYPEDEF_BYTE" /D "_TYPEDEF_WORD" /D "_SAGA_MSW" /Fd"./../../../bin/tmp/saga_vc/saga_cmd/saga_cmd.pdb" /FD /EHsc /c
# ADD BASE RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_lib\msw" /i "." /d "__WXMSW__" /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_lib\msw" /i "." /d "__WXMSW__" /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase26_net.lib wxbase26_odbc.lib wxbase26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:I386 /libpath:".\..\..\lib\vc_lib" /subsystem:saga_cmd
# ADD LINK32 wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:I386 /libpath:"$(WXWIN)/lib/vc_dll"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "saga_cmd - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\saga_cmd"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "./../../../bin/saga_vc_dbg"
# PROP Intermediate_Dir "./../../../bin/tmp/saga_vc_dbg/saga_cmd"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_lib\mswd" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_CONSOLE" /D wxUSE_GUI=0 /Fd"vc_mswd\saga_cmd.pdb" /FD /GZ /EHsc /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /ZI /Od /I "./.." /I "$(WXWIN)/include" /I "$(WXWIN)/lib/vc_dll/mswd" /D "__WXMSW__" /D "_CONSOLE" /D wxUSE_GUI=0 /D "WIN32" /D "_DEBUG" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_TYPEDEF_BYTE" /D "_TYPEDEF_WORD" /D "_SAGA_MSW" /Fd"./../../../bin/tmp/saga_vc_dbg/saga_cmd/saga_cmd.pdb" /FD /GZ /EHsc /c
# ADD BASE RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_lib\mswd" /i "." /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_lib\mswd" /i "." /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase26d_net.lib wxbase26d_odbc.lib wxbase26d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /debug /machine:I386 /libpath:".\..\..\lib\vc_lib" /subsystem:saga_cmd
# ADD LINK32 wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /debug /machine:I386 /libpath:"$(WXWIN)/lib/vc_dll"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "saga_cmd - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "saga_cmd___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "saga_cmd___Win32_Unicode_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "./../../../bin/saga_vc_u_dbg"
# PROP Intermediate_Dir "./../../../bin/tmp/saga_vc_u_dbg/saga_cmd"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GR /ZI /Od /I "$(WXWIN)/lib/vc_lib/mswd" /I "$(SAGA)/src/saga_core" /I "$(WXWIN)/include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_CONSOLE" /D wxUSE_GUI=0 /D "_TYPEDEF_BYTE" /D "_TYPEDEF_WORD" /D "_SAGA_MSW" /Fd"./../../../bin/tmp/saga_vc_dbg/saga_cmd/saga_cmd.pdb" /FD /GZ /EHsc /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /ZI /Od /I "./.." /I "$(WXWIN)/lib/vc_lib/mswud" /I "$(WXWIN)/include" /I "$(WXWIN)/lib/vc_dll/mswud" /D "_CONSOLE" /D wxUSE_GUI=0 /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_DEBUG" /D "WIN32" /D "_UNICODE" /D "_TYPEDEF_BYTE" /D "_TYPEDEF_WORD" /D "_SAGA_MSW" /D "_SAGA_UNICODE" /Fd"./../../../bin/tmp/saga_vc_dbg/saga_cmd/saga_cmd.pdb" /FD /GZ /EHsc /c
# ADD BASE RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_lib\mswd" /i "." /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_lib\mswd" /i "." /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /debug /machine:I386 /libpath:"$(WXWIN)/lib/vc_lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /debug /machine:I386 /libpath:"$(WXWIN)/lib/vc_dll"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "saga_cmd - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "saga_cmd___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "saga_cmd___Win32_Unicode_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "./../../../bin/saga_vc_u"
# PROP Intermediate_Dir "./../../../bin/tmp/saga_vc_u/saga_cmd"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /O2 /I "$(WXWIN)/lib/vc_lib/msw" /I "$(SAGA)/src/saga_core" /I "$(WXWIN)/include" /D "WIN32" /D "__WXMSW__" /D "_CONSOLE" /D wxUSE_GUI=0 /D "_TYPEDEF_BYTE" /D "_TYPEDEF_WORD" /D "_SAGA_MSW" /Fd"./../../../bin/tmp/saga_vc/saga_cmd/saga_cmd.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W3 /GR /O2 /I "./.." /I "$(WXWIN)/lib/vc_lib/mswu" /I "$(WXWIN)/include" /I "$(WXWIN)/lib/vc_dll/mswu" /D "_CONSOLE" /D wxUSE_GUI=0 /D "__WXMSW__" /D "WIN32" /D "WXUSINGDLL" /D "_UNICODE" /D "_TYPEDEF_BYTE" /D "_TYPEDEF_WORD" /D "_SAGA_MSW" /D "_SAGA_UNICODE" /Fd"./../../../bin/tmp/saga_vc/saga_cmd/saga_cmd.pdb" /FD /EHsc /c
# ADD BASE RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_lib\msw" /i "." /d "__WXMSW__" /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_lib\msw" /i "." /d "__WXMSW__" /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:I386 /libpath:"$(WXWIN)/lib/vc_lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:I386 /libpath:"$(WXWIN)/lib/vc_dll"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "saga_cmd - Win32 Release"
# Name "saga_cmd - Win32 Debug"
# Name "saga_cmd - Win32 Unicode Debug"
# Name "saga_cmd - Win32 Unicode Release"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\callback.cpp
# End Source File
# Begin Source File

SOURCE=.\module_library.cpp
# End Source File
# Begin Source File

SOURCE=.\saga_cmd.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\callback.h
# End Source File
# Begin Source File

SOURCE=.\module_library.h
# End Source File
# End Group
# End Target
# End Project
