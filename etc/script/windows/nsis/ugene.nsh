# NSIS Script For Unipro UGENE

################################################################
# Modern UI
!include "MUI2.nsh"
!verbose 4

!define CompanyName "Unipro"
!define ProductName "UGENE"
!define FullProductName "${CompanyName} ${ProductName}"
!define TargetPlatform x64

# Compressor
    SetCompressor /SOLID /FINAL lzma
    SetCompressorDictSize 64

# Interface Settings
    !define MUI_ABORTWARNING
    !define MUI_LANGDLL_ALLLANGUAGES        
    !define MUI_HEADERIMAGE
    !define MUI_HEADERIMAGE_BITMAP "ugene\etc\script\windows\nsis\images\header.bmp"
    !define MUI_SPECIALIMAGE
    !define MUI_WELCOMEFINISHPAGE_BITMAP "ugene\etc\script\windows\nsis\images\welcome.bmp"
    !define MUI_FINISHPAGE_RUN "$INSTDIR\ugeneui.exe"

;--------------------------------
;Language Selection Dialog Settings

  ;Remember the installer language
  !define MUI_LANGDLL_REGISTRY_ROOT "HKCU" 
  !define MUI_LANGDLL_REGISTRY_KEY "Software\${CompanyName}\${ProductName}" 
  !define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

# Pages
    !insertmacro MUI_PAGE_WELCOME
    !insertmacro MUI_PAGE_LICENSE ugene\LICENSE.txt
    !define MUI_PAGE_CUSTOMFUNCTION_LEAVE checkInstDir
    !insertmacro MUI_PAGE_DIRECTORY
    !insertmacro MUI_PAGE_INSTFILES
    !insertmacro MUI_PAGE_FINISH

    !insertmacro MUI_UNPAGE_CONFIRM
    !insertmacro MUI_UNPAGE_INSTFILES

# Languages
    !insertmacro MUI_LANGUAGE "English"
    !insertmacro MUI_LANGUAGE "Russian"

;--------------------------------
;Reserve Files
  
  ;If you are using solid compression, files that are required before
  ;the actual installation should be stored first in the data block,
  ;because this will make your installer start faster.
  
  !insertmacro MUI_RESERVEFILE_LANGDLL

Function "checkInstDir"
   CreateDirectory $INSTDIR
   ClearErrors
   CreateDirectory $INSTDIR
   Iferrors 0 +3
   MessageBox MB_ICONEXCLAMATION 'The directory "$INSTDIR" is not available for writing. Please choose another directory.'
   Abort
FunctionEnd

!include ugene_extensions.nsh
!include "x64.nsh"
!define MUI_LANGDLL_WINDOWTITLE "Select Language"

Function .onInit
${IfNot} ${RunningX64}
    MessageBox MB_OK|MB_ICONEXCLAMATION "This installer can only be run on 64-bit windows"
    Abort
${EndIf}

    !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd


################################################################
# Installer options
    !define ReleaseBuildDir "${UGENE_BUNDLE_DIR}"
    !ifndef ProductVersion
    !define ProductVersion "unknown"
    !endif

    Name    "${FullProductName}"
    InstallDir "$PROGRAMFILES64\${FullProductName}"
    InstallDirRegKey HKCU "Software\${CompanyName}\${ProductName}" ""
    DirText "Please select the folder below"
    BrandingText "${FullProductName}"
    UninstallText "This will uninstall ${FullProductName} from your system"
    Icon "ugene\etc\script\windows\nsis\images\install.ico"
    UninstallIcon "ugene\etc\script\windows\nsis\images\uninstall.ico"
    ;BGGradient 000000 400040 FFFFFF
    SetFont "Tahoma" 9
    CRCCheck On

    !ifndef UGENE_DISTRIB_FILE_NAME
        OutFile "ugene_${ProductVersion}_win_${TargetPlatform}.exe"
    !else
        OutFile "${UGENE_DISTRIB_FILE_NAME}"
    !endif


Function languageUGENEIni
    CreateDirectory "$APPDATA\${CompanyName}\"
    ClearErrors

    FileOpen $4 "$APPDATA\${CompanyName}\${ProductName}.ini" a
    FileWrite $4 "[user_apps]$\r$\n"
    FileWrite $4 "translation_file=transl_"

        StrCmp $LANGUAGE ${LANG_RUSSIAN} 0 +2
            FileWrite $4 "ru"
        StrCmp $LANGUAGE ${LANG_ENGLISH} 0 +2
            FileWrite $4 "en"

    FileWrite $4 "$\r$\n"
    FileClose $4
FunctionEnd


################################################################
Section "Build"
    !include "FileFunc.nsh"

SetRegView 64
    SectionIn 1 2 RO
    SetOutPath $INSTDIR
    
    # Remove old install
    RMDir /r "$INSTDIR"

    # Include the whole bundle as is.
    File /r "${ReleaseBuildDir}\*"

    SetOutPath $INSTDIR
    
    ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
    IntFmt $0 "0x%08X" $0

    var /GLOBAL warnText #collects warnings during the installation
    Iferrors 0 +2
    StrCpy $warnText "Warning: not all files were copied successfully!"
    
    # Write the uninstall keys for Windows
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${FullProductName}" "DisplayName" "${FullProductName} ${PrintableVersion}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${FullProductName}" "UninstallString" "$INSTDIR\Uninst.exe"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${FullProductName}" "Publisher" "Unipro"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${FullProductName}" "DisplayVersion" "${PrintableVersion}"
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${FullProductName}" "EstimatedSize" "$0"

    Iferrors 0 +2
    StrCpy $warnText "$warnText$\r$\nWarning: cannot create registry entries!"

    WriteUninstaller "$INSTDIR\Uninst.exe"

    Iferrors 0 +2
    StrCpy $warnText "$warnText$\r$\nWarning: cannot create uninstaller!"

    # Write language param in ini file
    Call languageUGENEIni

SectionEnd

################################################################
; Register extensions
!insertmacro AssociateExtSectionGroup ''

################################################################
Section "Add Shortcuts"
    SectionIn 1
    SetShellVarContext all
    RMDir /r "$SMPROGRAMS\${FullProductName}"
    ClearErrors
    CreateDirectory "$SMPROGRAMS\${FullProductName}"
    CreateShortCut "$SMPROGRAMS\${FullProductName}\Launch UGENE.lnk" "$INSTDIR\ugeneui.exe" "" "$INSTDIR\ugeneui.exe" 0
    # Make sure uninstall shortcut will be last item in Start menu
#    nsisStartMenu::RegenerateFolder "${FullProductName}"
    CreateShortCut "$SMPROGRAMS\${FullProductName}\Uninstall.lnk" "$INSTDIR\Uninst.exe" "" "$INSTDIR\Uninst.exe" 0
    CreateShortCut "$DESKTOP\${FullProductName}.lnk" "$INSTDIR\ugeneui.exe" "" "$INSTDIR\ugeneui.exe" 0

    Iferrors 0 +2
    StrCpy $warnText "$warnText$\r$\nWarning: cannot create program shortcuts!"
#Display all of collected warnings  
    var /GLOBAL warnTextLen
    StrLen $warnTextLen "$warnText"
    IntCmp $warnTextLen 0 +2
    MessageBox MB_ICONEXCLAMATION "$warnText"
SectionEnd

################################################################
Section Uninstall
    # Delete shortcuts
    SetShellVarContext all
    Delete "$DESKTOP\${FullProductName}.lnk"
    Delete "$SMPROGRAMS\${FullProductName}\*.*"
    RmDir /r "$SMPROGRAMS\${FullProductName}"

    # Delete Uninstaller And Unistall Registry Entries
    Delete "$INSTDIR\Uninst.exe"
    RMDir /r "$INSTDIR"
	SetRegView 64
    DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${FullProductName}"
    DeleteRegKey HKCU "Software\${CompanyName}\${ProductName}"
SectionEnd

################################################################
; Unregister extensions
!insertmacro AssociateExtSectionGroup 'un.'


################################################################

;--------------------------------
;Uninstaller Functions

Function un.onInit

  !insertmacro MUI_UNGETLANGUAGE
  
FunctionEnd
