; To build installer (for example, SetupSAGA-203.exe).
; - Unzip the SAGA binary files in their own folder (for example, saga_2.0.3_bin_mswvc.zip).
; - Place saga.iss and saga_readme.rtf in the SAGA folder (contains saga_gui.exe, etc.).
; - Open saga.iss in Inno Setup.
; - Choose Build | Compile.
; Contribute by Philipp J. Hess

[Setup]
AppName=SAGA - System for Automated Geoscientific Analyses
AppVerName=SAGA 2.2.0

DefaultDirName={pf}\SAGA-GIS
DefaultGroupName=SAGA GIS
UninstallDisplayIcon={app}\saga_gui.exe

InfoBeforeFile=saga_setup_readme.rtf

Compression=lzma
SolidCompression=yes

OutputDir=.
OutputBaseFilename=saga_2.2.0_win32_setup

VersionInfoVersion=2.2.0.0
VersionInfoCompany=SAGA User Group Association
VersionInfoCopyright=(c) 2005-2013 by O. Conrad
VersionInfoDescription=SAGA GIS single-file installer

PrivilegesRequired=admin

[Files]
; Add .exe's separately so can exclude any other .exe's (e.g., old installer).
Source: "saga_gui.exe"; DestDir: "{app}"
Source: "saga_cmd.exe"; DestDir: "{app}"
Source: "*.*"; Excludes: "*.iss,*.ini,*.cfg,*.bak,*.exe,*.bat,*.rtf"; DestDir: "{app}"
Source: "modules\*.*"; DestDir: "{app}\modules"
Source: "dll\*.*"; DestDir: "{app}\dll"

[Icons]
; Start up in modules folder so SAGA will load all modules automatically.
Name: "{commonprograms}\SAGA GIS"; Filename: "{app}\saga_gui.exe"; WorkingDir: "{app}"
Name: "{commondesktop}\SAGA GIS"; Filename: "{app}\saga_gui.exe"; WorkingDir: "{app}"

[Registry]
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\saga_gui.exe"; ValueType: string; ValueName: ""; ValueData: "{app}\saga_gui.exe"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\saga_gui.exe"; ValueType: string; ValueName: "Path"; ValueData: "{app}"

Root: HKLM; Subkey: "SOFTWARE\SAGA User Group Association\SAGA GIS\2.2.0"; ValueType: string; ValueName: "InstallDir"; ValueData: "{app}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\SAGA User Group Association\SAGA GIS\2.2.0"; ValueType: string; ValueName: "Version"; ValueData: "2.2.0.0"
