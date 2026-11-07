#define lwrsName "Loswors"
#define lwrsVersion "1.0.0"
#define lwrsExeName "Loswors.exe"
#define lwrsFormat "lwrs"

#define lwrsInstallerName "Loswors_WIN_Installer_v1.0.0"

[Setup]
; In PowerShell: [guid]::NewGuid() 
AppId={{d19749b7-6b90-4afa-ac00-36fa8bc2cb47}
AppName={#lwrsName}
AppVersion={#lwrsVersion}

; Dir
DefaultDirName={localappdata}\Programs\{#lwrsName}
PrivilegesRequired=lowest

DisableProgramGroupPage=yes
OutputBaseFilename={#lwrsInstallerName}
UninstallDisplayIcon={app}\{#lwrsExeName}

; Update Windows file association information
ChangesAssociations=yes

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; Only one installation file
Source: "build_source\Loswors_v1.0.0.exe"; DestDir: "{app}"; DestName: "{#lwrsExeName}"; Flags: ignoreversion

[Icons]
; Shortcut in the Start menu
Name: "{userprograms}\{#lwrsName}"; \
    Filename: "{app}\{#lwrsExeName}"; \
    WorkingDir: "{app}"
    
; Shortcut on the Desktop
Name: "{autodesktop}\{#lwrsName}"; \
    Filename: "{app}\{#lwrsExeName}"; \
    WorkingDir: "{app}"; \
    Tasks: desktopicon
    
[InstallDelete]
; We remove everything that the program may generate (while update)
Type: filesandordirs; Name: "{app}\Data"
Type: filesandordirs; Name: "{app}\licenses"
Type: files; Name: "{app}\LICENSE"

[UninstallDelete]
; We remove everything that the program may generate (while delete)
Type: filesandordirs; Name: "{app}\Data"
Type: filesandordirs; Name: "{app}\licenses"
Type: files; Name: "{app}\LICENSE"

[Registry]
; 1. register .lwrs
Root: HKCU; Subkey: "Software\Classes\.{#lwrsFormat}"; \
    ValueType: string; ValueName: ""; \
    ValueData: "{#lwrsName}.SaveFile"; \
    Flags: uninsdeletevalue uninsdeletekeyifempty
Root: HKCU; Subkey: "Software\Classes\.{#lwrsFormat}\OpenWithProgids"; \
    ValueType: string; \
    ValueName: "{#lwrsName}.SaveFile"; \
    ValueData: ""; \
    Flags: uninsdeletevalue

; 2. Description of file type and name
Root: HKCU; Subkey: "Software\Classes\{#lwrsName}.SaveFile"; \
    ValueType: string; \
    ValueName: ""; \
    ValueData: "{#lwrsName} SaveFile"; \
    Flags: uninsdeletekey

; 3. File icon (index 1 = second icon in .exe)
Root: HKCU; Subkey: "Software\Classes\{#lwrsName}.SaveFile\DefaultIcon"; \
    ValueType: string; \
    ValueName: ""; \
    ValueData: "{app}\{#lwrsExeName},1"; \
    Flags: uninsdeletekey

; 4. Double-click open command
Root: HKCU; Subkey: "Software\Classes\{#lwrsName}.SaveFile\shell\open\command"; \
    ValueType: string; \
    ValueName: ""; \
    ValueData: """{app}\{#lwrsExeName}"" ""%1"""; \
    Flags: uninsdeletekey
    
    
