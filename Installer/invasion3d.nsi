; Invasion3D.nsi
;
;--------------------------------

SetCompressor lzma

; The name of the installer
Name "Invasion3D"
Caption "Invasion3D Installer"
Icon "C:\Program Files\NSIS\Contrib\Graphics\Icons\nsis1-install.ico"
UninstallIcon "C:\Program Files\NSIS\Contrib\Graphics\Icons\nsis1-uninstall.ico"

; The file to write
OutFile "Setup.exe"

; flags
CRCCheck on
BGGradient 000000 000080 FFFFFF
InstallColors 8080FF 000030
XPStyle on

; The default installation directory
InstallDir $PROGRAMFILES\Invasion3D

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Fascination Software\Invasion3D" "Install_Dir"

; license file info
LicenseText "Read the information before continuing..."
LicenseData "License.txt"

;--------------------------------

; Pages

Page license
Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "Invasion3D (required)"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put files there
  File "..\Release\Invasion3D.exe"
  File "..\Release\Invaders.dat"
  File "..\Release\msvcr71.dll"
  File "..\Release\Readme.txt"
  File "..\Release\SDL.dll"
  File "lgpl.html"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM "SOFTWARE\Fascination Software\Invasion3D" "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Invasion3D" "DisplayName" "Invasion3D"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Invasion3D" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Invasion3D" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Invasion3D" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\Invasion3D"
  CreateShortCut "$SMPROGRAMS\Invasion3D\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Invasion3D\Invasion3D.lnk" "$INSTDIR\Invasion3D.exe" "" "$INSTDIR\Invasion3D.exe" 0
  
SectionEnd

Section "Desktop Icon"

  CreateShortCut "$DESKTOP\Invasion3D.lnk" "$INSTDIR\Invasion3D.exe"

SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Invasion3D"
  DeleteRegKey HKLM "SOFTWARE\Fascination Software\Invasion3D"

  ; Remove files and uninstaller
  Delete $INSTDIR\Invasion3D.exe
  Delete $INSTDIR\Invaders.dat
  Delete $INSTDIR\lgpl.html
  Delete $INSTDIR\msvcr71.dll
  Delete $INSTDIR\Readme.txt
  Delete $INSTDIR\SDL.dll
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Invasion3D\*.*"
  Delete "$DESKTOP\Invasion3D.lnk"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Invasion3D"
  RMDir "$INSTDIR"

SectionEnd
