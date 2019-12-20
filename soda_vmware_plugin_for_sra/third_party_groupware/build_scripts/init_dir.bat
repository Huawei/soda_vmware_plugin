@echo off
cd "%cd%"
rem init directory structure
SET THIRD_PARTY_PATH=%cd%\..
IF NOT EXIST %THIRD_PARTY_PATH%\OpenSDS_Storage_Plugins\ACE\source MD %THIRD_PARTY_PATH%\OpenSDS_Storage_Plugins\ACE\source
IF NOT EXIST %THIRD_PARTY_PATH%\OpenSDS_Storage_Plugins\curl\source MD %THIRD_PARTY_PATH%\OpenSDS_Storage_Plugins\curl\source
IF NOT EXIST %THIRD_PARTY_PATH%\OpenSDS_Storage_Plugins\jsoncpp\source MD %THIRD_PARTY_PATH%\OpenSDS_Storage_Plugins\jsoncpp\source
IF NOT EXIST %THIRD_PARTY_PATH%\OpenSDS_Storage_Plugins\libssh2 MD %THIRD_PARTY_PATH%\OpenSDS_Storage_Plugins\libssh2
IF NOT EXIST %THIRD_PARTY_PATH%\OpenSDS_Storage_Plugins\openssl\source MD %THIRD_PARTY_PATH%\OpenSDS_Storage_Plugins\openssl\source
IF NOT EXIST %THIRD_PARTY_PATH%\OpenSDS_Storage_Plugins\zlib MD %THIRD_PARTY_PATH%\OpenSDS_Storage_Plugins\zlib
IF NOT EXIST %THIRD_PARTY_PATH%\OpenSDS_Storage_Plugins\cfgmgr\lib MD %THIRD_PARTY_PATH%\OpenSDS_Storage_Plugins\cfgmgr\lib

rem copy files
xCOPY "%VS110COMNTOOLS%\..\..\VC\redist\x86\Microsoft.VC110.CRT\*.dll" "%THIRD_PARTY_PATH%\OpenSDS_Storage_Plugins\win32_x86_msvc2010\" /y
xCOPY "%WinKits%\Lib\winv6.3\um\arm\cfgmgr32.lib" %THIRD_PARTY_PATH%\OpenSDS_Storage_Plugins\cfgmgr\lib /y

:Exit