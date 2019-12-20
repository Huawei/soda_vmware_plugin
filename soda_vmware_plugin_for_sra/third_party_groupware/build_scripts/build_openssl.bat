@echo off
set time_s=%time:~0,8%
if exist build.log ren build.log build_"%time_s::=%".log

set errorExists=
set buildFailed=

@echo ************************************************************
@echo start building openssl...
@echo build openssl with perl,please waiting...

echo [%DATE:~0,10% %TIME%][Info]************************************************************ >> build.log
echo [%DATE:~0,10% %TIME%][Info]start building openssl... >> build.log
echo [%DATE:~0,10% %TIME%][Info]setting environment... >> build.log
set SCRIPT_PATH=%cd%
set OPENSSL_PATH="%SCRIPT_PATH%\..\OpenSDS_Storage_Plugins\openssl\source"
set VCVARS32="%VS110COMNTOOLS%\..\..\VC\bin\vcvars32.bat"

echo [%DATE:~0,10% %TIME%][Info]. >> build.log
echo [%DATE:~0,10% %TIME%][Info]-------------------compile openssl------------------- >> build.log
echo [%DATE:~0,10% %TIME%][Info]. >> build.log
cd %OPENSSL_PATH%

echo [%DATE:~0,10% %TIME%][Info]build openssl with perl... >> build.log
perl Configure VC-WIN32 no-asm --prefix=%OPENSSL_PATH%\..\..\opensslOuput >> build.log
call %VCVARS32% >> build.log
if exist tmp32dll nmake clean >> build.log
nmake
nmake install


echo [%DATE:~0,10% %TIME%][Info]copying compiled files >> build.log
cd "%cd%"
xCOPY "%OPENSSL_PATH%\..\..\opensslOuput\bin" "%OPENSSL_PATH%\..\..\openssl\bin\win32_x86_msvc\release\" /s /e /y >> build.log
xCOPY "%OPENSSL_PATH%\..\..\opensslOuput\lib" "%OPENSSL_PATH%\..\..\openssl\lib\win32_x86_msvc\release\" /s /e /y >> build.log
xCOPY "%OPENSSL_PATH%\..\..\opensslOuput\include" "%OPENSSL_PATH%\..\..\openssl\include\" /s /e /y >> build.log

echo [%DATE:~0,10% %TIME%][Info]removing temp files... >> build.log
RMDIR /S/Q "%OPENSSL_PATH%\..\..\opensslOuput" >> build.log
RMDIR /S/Q "%OPENSSL_PATH%\..\..\..\opensslOuput" >> build.log
cd "%SCRIPT_PATH%"

echo [%DATE:~0,10% %TIME%][Info]build openssl end >> build.log


rem find "error" in log, if not found, find "0 File(s) copied" in log
find /i "error" build.log > nul 2>&1 && set errorExists=True
if defined errorExists (goto Fail) else (goto deepCheck)

:deepCheck
	find "0 File(s) copied" build.log | findstr "^0" > nul 2>&1 && set buildFailed=True
	if defined buildFailed (goto Fail) else (goto Success)
	
:Success
	echo [%DATE:~0,10% %TIME%][Info]build openssl success >> build.log
	echo build openssl success
	goto Exit

:Fail
	echo [%DATE:~0,10% %TIME%][Error]build openssl fail >> build.log
	echo build openssl fail
	goto Exit
	
:Exit
	echo [%DATE:~0,10% %TIME%][Info]************************************************************ >> build.log
	echo ************************************************************