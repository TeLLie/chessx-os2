; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "ChessX"
#define MyAppVersion "1.5.2"
#define MyAppPublisher "chessx.sourceforge.net"
#define MyAppURL "http://chessx.sourceforge.net"
#define MyAppExeName "chessx.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{238918C3-282E-4415-A40D-49E63E09F666}}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
DisableWelcomePage=no
AllowNoIcons=yes
LicenseFile=.\COPYING
InfoAfterFile=.\ChangeLog.txt
OutputBaseFilename=setup-chessx7-64
SetupIconFile=.\src\chessx.ico
Compression=lzma
SolidCompression=yes
ChangesAssociations = yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; OnlyBelowVersion: 0,6.1

[Files]
Source: "..\build-chessx-MinGW_64_bit-Release\release\chessx.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\build-chessx-MinGW_64_bit-Release\release\*.dll"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\build-chessx-MinGW_64_bit-Release\release\chessx.exe.local"; DestDir: "{app}"; Flags: ignoreversion
Source: ".\COPYING"; DestDir: "{app}"; Flags: ignoreversion
Source: ".\ChangeLog"; DestDir: "{app}"
Source: ".\data\engines\uci\stockfish_10_x64.exe"; DestDir: "{app}\data\engines\uci\"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ".\data\timeseal\windows\*.exe"; DestDir: "{app}\data\timeseal\windows\"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[Tasks]
Name: pgnAssociation; Description: "Associate ""pgn"" extension"; GroupDescription: File extensions:

[Registry]
Root: HKCR; Subkey: ".pgn";                             ValueData: "{#MyAppName}";          Flags: uninsdeletevalue; ValueType: string;  ValueName: ""; Tasks: pgnAssociation
Root: HKCR; Subkey: "{#MyAppName}";                     ValueData: "Program {#MyAppName}";  Flags: uninsdeletekey;   ValueType: string;  ValueName: ""; Tasks: pgnAssociation
Root: HKCR; Subkey: "{#MyAppName}\DefaultIcon";         ValueData: "{app}\{#MyAppExeName},0";                        ValueType: string;  ValueName: ""; Tasks: pgnAssociation
Root: HKCR; Subkey: "{#MyAppName}\shell\open\command";  ValueData: """{app}\{#MyAppExeName}"" ""%1""";               ValueType: string;  ValueName: ""; Tasks: pgnAssociation