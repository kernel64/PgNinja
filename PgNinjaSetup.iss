; Inno Setup Script
[Setup]
AppName=PgNinja
AppVersion=1.0
DefaultDirName={pf}\PgNinja
DefaultGroupName=PgNinja
UninstallDisplayIcon={app}\PgNinja.exe
OutputDir=.\Output
OutputBaseFilename=PgNinjaSetup
Compression=lzma
SolidCompression=yes

[Files]
Source: "D:\a\PgNinja\PgNinja\x64\Release\PgNinja.exe"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\PgNinja"; Filename: "{app}\PgNinja.exe"
Name: "{userstartup}\PgNinja"; Filename: "{app}\PgNinja.exe"; Tasks: autostart

[Tasks]
Name: "autostart"; Description: "Launch PgNinja at Windows startup"; GroupDescription: "Additional options:"

[Registry]
; This creates a registry entry to launch PgNinja on boot if the user selects autostart.
Root: HKCU; Subkey: "Software\Microsoft\Windows\CurrentVersion\Run"; \
Name: "PgNinja"; ValueType: string; ValueData: """{app}\PgNinja.exe"""; \
Flags: uninsdeletevalue; Tasks: autostart

[Run]
Filename: "{app}\PgNinja.exe"; Description: "Launch PgNinja"; Flags: nowait postinstall skipifsilent
