; -- 64Bit.iss --
; Demonstrates installation of a program built for the x64 (a.k.a. AMD64)
; architecture.
; To successfully run this installation and the program it installs,
; you must have a "x64" edition of Windows.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

[Setup]
AppName=BomChecker
AppVersion=1.0.0
AppPublisher=qiuxuenan
WizardStyle=modern
SetupIconFile=icon.ico
DefaultDirName={autopf}\BomChecker
DefaultGroupName=BomChecker
Uninstallable=yes
UninstallDisplayIcon={app}\BomChecker.exe
AllowNoIcons=yes
OutputDir=dist
OutputBaseFilename=BomChecker_Setup
Compression=lzma2
SolidCompression=yes
; "ArchitecturesAllowed=x64" specifies that Setup cannot run on
; anything but x64.
ArchitecturesAllowed=x64
; "ArchitecturesInstallIn64BitMode=x64" requests that the install be
; done in "64-bit mode" on x64, meaning it should use the native
; 64-bit Program Files directory and the 64-bit view of the registry.
ArchitecturesInstallIn64BitMode=x64

[Files]
Source: "dist\BomChecker\*"; DestDir: "{app}"; Flags: recursesubdirs createallsubdirs;

[Icons]
Name: "{group}\BomChecker"; Filename: "{app}\BomChecker.exe"; WorkingDir: "{app}"
Name: "{commondesktop}\BomChecker"; Filename: "{app}\BomChecker.exe"; WorkingDir: "{app}"
Name: "{group}\Uninstall BomChecker"; Filename: "{uninstallexe}"

[Run]
Filename: "{app}\BomChecker.exe"; Description: "Run BomChecker now"; Flags: postinstall nowait unchecked;