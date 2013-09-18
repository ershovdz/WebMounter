!define VERSION "0.4.0"

!define MULTIUSER_EXECUTIONLEVEL Highest
;!define MULTIUSER_NOUNINSTALL ;Uncomment if no uninstaller is created
!define MULTIUSER_MUI
!include MultiUser.nsh
!include MUI2.nsh
!include LogicLib.nsh
!include x64.nsh
!include WinVer.nsh

Name "WebMounter ${VERSION}"
OutFile "WebMounter ${VERSION}.exe"

InstallDir "$PROGRAMFILES32\WebMounter"
RequestExecutionLevel admin
;LicenseData "licdata.rtf"
ShowUninstDetails show

Var StartMenuFolder

!insertmacro MUI_PAGE_WELCOME 
!insertmacro MUI_PAGE_COMPONENTS 
!insertmacro MUI_PAGE_DIRECTORY 
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
!insertmacro MUI_PAGE_INSTFILES  
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

; First is default
;;LoadLanguageFile "${NSISDIR}\Contrib\Language files\English.nlf"
;;LoadLanguageFile "${NSISDIR}\Contrib\Language files\Russian.nlf"

!insertmacro MUI_LANGUAGE "Russian"
!insertmacro MUI_LANGUAGE "English"

;LangString prevExist ${LANG_ENGLISH} "Please unstall the previous version and restart your computer before running this installer."
;LangString prevExist ${LANG_RUSSIAN} "Пожалуйста, удалите предыдущую версию и перезагрузите компьютер перед началом новой установки"
;LangString notSupportOS64 ${LANG_ENGLISH} "Your OS is not supported. WebMounter supports Windows 2003, Vista, 2008, 2008R2 and 7 for x64."
;LangString notSupportOS64 ${LANG_RUSSIAN} "Ваша операционная система не поддерживается. WebMounter поддерживает Windows 2003, Vista, 2008, 2008R2 and 7 для x64."	
;LangString notSupportOS86 ${LANG_ENGLISH} "Your OS is not supported. WebMounter supports Windows XP, 2003, Vista, 2008 and 7 for x86."
;LangString notSupportOS86 ${LANG_RUSSIAN} "Ваша операционная система не поддерживается. WebMounter поддерживает Windows XP, 2003, Vista, 2008 and 7 для x86."	
;LangString rebootReq ${LANG_ENGLISH} "A reboot is required to finish the uninstallation. Do you wish to reboot now?"
;LangString rebootReq ${LANG_RUSSIAN} "Для завершения установки требуется перезагрузка. Вы хотите перезагрузиться сейчас ?"	

UninstPage uninstConfirm
UninstPage instfiles
Icon ".\bin\drive.ico"


 
!macro WebMounter

  SetOutPath "$PROGRAMFILES32\WebMounter\bin"
    File .\bin\libcurl.dll
    File .\bin\phonon4.dll
    File .\bin\QtCore4.dll
    File .\bin\QtGui4.dll
    File .\bin\QtNetwork4.dll
    File .\bin\QtWebKit4.dll
    File .\bin\phonon4.dll
    File .\bin\QtSql4.dll
    File .\bin\wmbase.dll
    File .\bin\wmui.dll
    File ".\bin\WebMounter.exe"
	File .\bin\Help.chm
    File .\bin\msvcp100.dll
    File .\bin\msvcr100.dll
    File .\bin\drive.ico
    File .\bin\libeay32.dll
    File .\bin\libssl32.dll
    File .\bin\QtXml4.dll
    File .\bin\ssleay32.dll
    File .\bin\qjson.dll

  SetOutPath "$PROGRAMFILES32\WebMounter\lib\plugins"
    File .\lib\plugins\wm-vk-plugin.dll
    File .\lib\plugins\wm-facebook-plugin.dll
    File .\lib\plugins\wm-yandex-plugin.dll
    File .\lib\plugins\wm-ya-disk-plugin.dll
    File .\lib\plugins\wm-ya-narod-plugin.dll


  SetOutPath "$PROGRAMFILES32\WebMounter\share\webmounter"
    File .\share\webmounter\vk_wm_pl_en.qm
    File .\share\webmounter\vk_wm_pl_ru.qm
    File .\share\webmounter\yaf_wm_pl_en.qm
    File .\share\webmounter\yaf_wm_pl_ru.qm
    File .\share\webmounter\facebook_wm_pl_en.qm
    File .\share\webmounter\facebook_wm_pl_ru.qm
    File .\share\webmounter\webmounter_en.qm
    File .\share\webmounter\webmounter_ru.qm
    File .\share\webmounter\wmbase_en.qm
    File .\share\webmounter\wmbase_ru.qm
    File .\share\webmounter\wmui_en.qm
    File .\share\webmounter\wmui_ru.qm
    File .\share\webmounter\yandex_disk_wm_pl_en.qm
    File .\share\webmounter\yandex_disk_wm_pl_ru.qm
    File .\share\webmounter\yanarod_wm_pl_en.qm
    File .\share\webmounter\yanarod_wm_pl_ru.qm
	
  SetOutPath "$PROGRAMFILES32\WebMounter\bin\sqldrivers"
    File .\bin\sqldrivers\qsqlite4.dll
  SetOutPath "$PROGRAMFILES32\WebMounter\bin\imageformats"
    File .\bin\imageformats\qjpeg4.dll
	File .\bin\imageformats\qgif4.dll
	File .\bin\imageformats\qmng4.dll
!macroend


!macro X86Files os

  SetOutPath $PROGRAMFILES32\Dokan\DokanLibrary
    File .\dokan\${os}\dokanctrl\dokanctl.exe
    File .\dokan\${os}\mounter\mounter.exe

  SetOutPath $SYSDIR
    File .\dokan\${os}\dll\dokan.dll

!macroend

/*
!macro X64Files os

  SetOutPath $PROGRAMFILES32\WebMounter

    File .\WebMounter\curllib.dll
	File .\WebMounter\libeay32.dll
    File .\WebMounter\libidn-11.dll
    File .\WebMounter\libssh2.dll
    File .\WebMounter\libssl32.dll
    File .\WebMounter\msvcp100d.dll
    File .\WebMounter\msvcr100d.dll
    File .\WebMounter\openldap.dll
    File .\WebMounter\QtCored4.dll
    File .\WebMounter\QtGuid4.dll
    File .\WebMounter\QtSqld4.dll
    File .\WebMounter\ssleay32.dll
    File .\WebMounter\WebMounter.exe
    File .\WebMounter\webmounter_en.qm
    File .\WebMounter\webmounter_ru.qm

  SetOutPath $PROGRAMFILES64\Dokan\DokanLibrary

    File ..\dokan\readme.txt
    File ..\dokan\readme.ja.txt
    File ..\license.gpl.txt
    File ..\license.lgpl.txt
    File ..\license.mit.txt
    File .\dokan-0.5.3\${os}\dll\dokan.lib
    File .\dokan-0.5.3\${os}\dokanctrl\dokanctl.exe
    File .\dokan-0.5.3\${os}\mounter\mounter.exe

  ${DisableX64FSRedirection}

  SetOutPath $SYSDIR

    File .\dokan-0.5.3\${os}\dll\dokan.dll

  ${EnableX64FSRedirection}

!macroend
*/

!macro DokanSetup
  ExecWait '"$PROGRAMFILES32\Dokan\DokanLibrary\dokanctl.exe" /i a' $0
  DetailPrint "dokanctl returned $0"
  WriteUninstaller "$PROGRAMFILES32\iPrintPhoto WebDisk\WebMounterUninstall.exe"

  ;WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Run" "WebMounter" "$INSTDIR\WebMounter.exe"
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\WebMounter" "DisplayName" "WebMounter ${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\WebMounter" "UninstallString" '"$PROGRAMFILES32\WebMounter\WebMounterUninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\WebMounter" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\WebMounter" "NoRepair" 1

!macroend

!macro X86Driver os
  SetOutPath $SYSDIR\drivers
    File .\dokan\${os}\sys\dokan.sys
!macroend

!macro X64Driver os
  ${DisableX64FSRedirection}

  SetOutPath $SYSDIR\drivers
    File .\dokan\x64\${os}\sys\dokan.sys

  ${EnableX64FSRedirection}
!macroend

;--------------------------------
;Languages

	;LoadLanguageFile "${NSISDIR}\Contrib\Language files\Russian.nlf"
	;LoadLanguageFile "${NSISDIR}\Contrib\Language files\English.nlf"
  ;!insertmacro MUI_LANGUAGE "English"
  ;!insertmacro MUI_LANGUAGE "Russian"
  ;!insertmacro MUI_RESERVEFILE_LANGDLL
  
;---------------------------------
 
Section "WebMounter Files" section_webmounter
	!insertmacro WebMounter
	
	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\WebMounter"
    CreateShortCut "$SMPROGRAMS\WebMounter\WebMounter.lnk" "$PROGRAMFILES32\WebMounter\bin\WebMounter.exe" "" "$PROGRAMFILES32\WebMounter\bin\drive.ico"
    CreateShortCut "$SMPROGRAMS\WebMounter\Uninstall.lnk" "$PROGRAMFILES32\WebMounter\WebMounterUninstall.exe"
	
	SetOutPath "$PROGRAMFILES32\WebMounter"
	CreateShortCut "$DESKTOP\WebMounter.lnk" "$INSTDIR\bin\WebMounter.exe" "" "$INSTDIR\bin\drive.ico"
	
	;CreateShortCut "$SMSTARTUP\WebMounter.lnk" "$INSTDIR\bin\WebMounter.exe" "" "$INSTDIR\bin\drive.ico"
	
	
    !insertmacro MUI_STARTMENU_WRITE_END
	
SectionEnd	

Section "Dokan Library x86" section_x86
  ${If} ${IsWin7}
    !insertmacro X86Files "win7"
  ${ElseIf} ${IsWin2008R2}
    !insertmacro X86Files "win7"
  ${ElseIf} ${IsWinVista}
    !insertmacro X86Files "wlh"
  ${ElseIf} ${IsWin2008}
    !insertmacro X86Files "wlh"
  ${ElseIf} ${IsWin2003}
    !insertmacro X86Files "wnet"
  ${ElseIf} ${IsWinXp}
    !insertmacro X86Files "wxp"
  ${Else}
	!insertmacro X86Files "win7" ; may be it's win8 ?
  ${EndIf}
SectionEnd

Section "Dokan Driver x86" section_x86_driver
  ${If} ${IsWin7}
    !insertmacro X86Driver "win7"
  ${ElseIf} ${IsWinVista}
    !insertmacro X86Driver "wlh"
  ${ElseIf} ${IsWin2008}
    !insertmacro X86Driver "wlh"
  ${ElseIf} ${IsWin2003}
    !insertmacro X86Driver "wnet"
  ${ElseIf} ${IsWinXp}
    !insertmacro X86Driver "wxp"
  ${Else}
	!insertmacro X86Driver "win7" ; may be it's win8 ?
  ${EndIf}
  !insertmacro DokanSetup
SectionEnd

Section "Dokan Driver x64" section_x64_driver
  ${If} ${IsWin7}
    !insertmacro X64Driver "win7"
  ${ElseIf} ${IsWin2008R2}
    !insertmacro X64Driver "win7"
  ${ElseIf} ${IsWinVista}
    !insertmacro X64Driver "wlh"
  ${ElseIf} ${IsWin2008}
    !insertmacro X64Driver "wlh"
  ${ElseIf} ${IsWin2003}
    !insertmacro X64Driver "wnet"
  ${Else}
	!insertmacro X64Driver "win7" ; may be it's win8 ?
  ${EndIf}
  !insertmacro DokanSetup
SectionEnd

/*
Section "Dokan Library x64" section_x64
  ${If} ${IsWin7}
    !insertmacro X64Files "win7"
  ${ElseIf} ${IsWinVista}
    !insertmacro X64Files "wlh"
  ${ElseIf} ${IsWin2008}
    !insertmacro X64Files "wlh"
  ${ElseIf} ${IsWin2003}
    !insertmacro X64Files "wnet"
  ${EndIf}
SectionEnd
*/
	

Section "Uninstall"
  ExecWait '"$PROGRAMFILES32\Dokan\DokanLibrary\dokanctl.exe" /r a' $0
  DetailPrint "dokanctl.exe returned $0"

  RMDir /r "$PROGRAMFILES32\WebMounter"
  RMDir /r $PROGRAMFILES32\Dokan\DokanLibrary
  RMDir $PROGRAMFILES32\Dokan
  Delete $SYSDIR\dokan.dll

  ${If} ${RunningX64}
    ${DisableX64FSRedirection}
      Delete $SYSDIR\drivers\dokan.sys
    ${EnableX64FSRedirection}
  ${Else}
    Delete $SYSDIR\drivers\dokan.sys
  ${EndIf}

  ; Remove registry keys
 
  Delete "$SMPROGRAMS\WebMounter\WebMounter.lnk"
  Delete "$SMPROGRAMS\WebMounter\Uninstall.lnk"
  Delete "$DESKTOP\WebMounter.lnk"
  ;Delete "$SMSTARTUP\WebMounter.lnk"
  RMDir /r "$SMPROGRAMS\WebMounter"
  
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\WebMounter"
  ;DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Run\WebMounter"

  ;RMDir $SMPROGRAMS\WebMounter

	StrCmp $LANGUAGE ${LANG_ENGLISH} 0 +2
		MessageBox MB_YESNO "A reboot is required to finish the uninstallation. Do you wish to reboot now?" IDNO noreboot
	StrCmp $LANGUAGE ${LANG_RUSSIAN} 0 +2
		MessageBox MB_YESNO "Для завершения установки требуется перезагрузка. Вы хотите перезагрузиться сейчас ?" IDNO noreboot
    Reboot
  noreboot:

SectionEnd



Function initChecks 
  IntOp $0 ${SF_SELECTED} | ${SF_RO}
  ${If} ${RunningX64}
    SectionSetFlags ${section_webmounter} $0
	SectionSetFlags ${section_x86} $0
    SectionSetFlags ${section_x86_driver} ${SF_RO}  ; disable
    SectionSetFlags ${section_x64_driver} $0
  ${Else}
    SectionSetFlags ${section_webmounter} $0
    SectionSetFlags ${section_x86} $0
    SectionSetFlags ${section_x86_driver} $0
    SectionSetFlags ${section_x64_driver} ${SF_RO}  ; disable
  ${EndIf}

  ; Windows Version check

  ${If} ${RunningX64}
    ${If} ${IsWin2003}
    ${ElseIf} ${IsWinVista}
    ${ElseIf} ${IsWin2008}
    ${ElseIf} ${IsWin2008R2}
    ${ElseIf} ${IsWin7}
    ; ${Else}
      ; MessageBox MB_OK "$(notSupportOS64)"
      ; Abort
    ${EndIf}
  ${Else}
    ${If} ${IsWinXP}
    ${ElseIf} ${IsWin2003}
    ${ElseIf} ${IsWinVista}
    ${ElseIf} ${IsWin2008}
    ${ElseIf} ${IsWin7}
    ; ${Else}
      ; MessageBox MB_OK "$(notSupportOS86)"
      ; Abort
    ${EndIf}
  ${EndIf}

  ; Previous version
  ${If} ${RunningX64}
    ${DisableX64FSRedirection}
      IfFileExists $SYSDIR\drivers\dokan.sys HasPreviousVersionX64 NoPreviousVersionX64
      ; To make EnableX64FSRedirection called in both cases, needs duplicated MessageBox code. How can I avoid this?
      HasPreviousVersionX64:
        StrCmp $LANGUAGE ${LANG_ENGLISH} 0 +2
			MessageBox MB_OK "Please unstall the previous version and restart your computer before running this installer."
		StrCmp $LANGUAGE ${LANG_RUSSIAN} 0 +2
			MessageBox MB_OK "Пожалуйста, удалите предыдущую версию и перезагрузите компьютер перед началом новой установки"
        Abort
      NoPreviousVersionX64:
    ${EnableX64FSRedirection}
  ${Else}
    IfFileExists $SYSDIR\drivers\dokan.sys HasPreviousVersion NoPreviousVersion
    HasPreviousVersion:
       StrCmp $LANGUAGE ${LANG_ENGLISH} 0 +2
			MessageBox MB_OK "Please unstall the previous version and restart your computer before running this installer."
		StrCmp $LANGUAGE ${LANG_RUSSIAN} 0 +2
			MessageBox MB_OK "Пожалуйста, удалите предыдущую версию и перезагрузите компьютер перед началом новой установки"
      Abort
    NoPreviousVersion:
  ${EndIf}
FunctionEnd



Function .onInit
  !insertmacro MULTIUSER_INIT
  !insertmacro MUI_LANGDLL_DISPLAY
  
  Call initChecks
	
FunctionEnd
 
Function .onInstSuccess
	
  ;ExecShell "open" "$PROGRAMFILES32\WebMounter"
FunctionEnd

Function un.onInit
  !insertmacro MULTIUSER_UNINIT
FunctionEnd
