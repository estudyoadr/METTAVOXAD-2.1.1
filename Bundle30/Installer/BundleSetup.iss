#ifndef PluginArch
 #define PluginArch "x64"
#endif
#if PluginArch == "x86"
 #define CommonFiles "{commoncf32}"
#else
 #define CommonFiles "{commoncf64}"
#endif
[Setup]
AppId=mettavoxad-bundle-3-{#PluginArch}
AppName=MettaVoxAD 3.0 Bundle ({#PluginArch})
AppVersion=3.0.0
AppPublisher=MettaVoxAD Audio Engineering
DefaultDirName={#CommonFiles}\VST2\MettaVoxAD3
DefaultGroupName=MettaVoxAD 3.0 Bundle ({#PluginArch})
#if PluginArch == "x64"
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
#endif
OutputDir=Output
OutputBaseFilename=MettaVoxAD_3.0.0_Bundle_{#PluginArch}_Setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
PrivilegesRequired=admin
UsePreviousAppDir=yes
CloseApplications=yes
RestartApplications=no
SetupLogging=yes
[Files]
Source: "..\..\build_bundle_{#PluginArch}\Legacy\Release\*.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\build_bundle_{#PluginArch}\Stage\*"; DestDir: "{#CommonFiles}\VST3\MettaVoxAD3"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\Docs\MANUAL.md"; DestDir: "{app}"; Flags: ignoreversion
[Icons]
Name: "{group}\Manual"; Filename: "{app}\MANUAL.md"
[UninstallDelete]
Type: filesandordirs; Name: "{#CommonFiles}\VST3\MettaVoxAD3"
