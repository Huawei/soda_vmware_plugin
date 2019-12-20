@echo off
set result=
set buildFailed=

@echo ************************************************************
@echo start building zlib...
cd "%cd%"
SET ZLIB_PATH=%cd%\..\OpenSDS_Storage_Plugins\zlib\source

@echo build zlib, please wait...

echo [%DATE:~0,10% %TIME%][Info]************************************************************ >> build.log
echo [%DATE:~0,10% %TIME%][Info]start building zlib... >> build.log
echo [%DATE:~0,10% %TIME%][Info]. >> build.log
echo [%DATE:~0,10% %TIME%][Info]-------------------compile zlib------------------- >> build.log
echo [%DATE:~0,10% %TIME%][Info]. >> build.log
@"%VS110COMNTOOLS%\..\IDE\devenv.com" "%ZLIB_PATH%\contrib\vstudio\vc11\zlibvc.vcxproj" /Rebuild "Release|Win32" >> build.log

@echo copying compiled zlib files...
echo [%DATE:~0,10% %TIME%][Info]copying compiled zlib files... >> build.log
xcopy /Y "%ZLIB_PATH%\contrib\vstudio\vc11\x86\ZlibDllRelease\zlibwapi.dll" "%ZLIB_PATH%\x86\dll\" >> build.log
xcopy /Y "%ZLIB_PATH%\contrib\vstudio\vc11\x86\ZlibDllRelease\zlibwapi.lib" "%ZLIB_PATH%\x86\lib\" >> build.log
 
xcopy /Y %ZLIB_PATH%\contrib\minizip\crypt.h "%ZLIB_PATH%\..\build\source\include\" >> build.log
xcopy /Y %ZLIB_PATH%\contrib\minizip\ioapi.c "%ZLIB_PATH%\..\build\source\include\" >> build.log
xcopy /Y %ZLIB_PATH%\contrib\minizip\ioapi.h "%ZLIB_PATH%\..\build\source\include\" >> build.log
xcopy /Y %ZLIB_PATH%\contrib\minizip\iowin32.h "%ZLIB_PATH%\..\build\source\include\" >> build.log
xcopy /Y %ZLIB_PATH%\contrib\minizip\mztools.h "%ZLIB_PATH%\..\build\source\include\" >> build.log
xcopy /Y %ZLIB_PATH%\contrib\minizip\unzip.h "%ZLIB_PATH%\..\build\source\include\" >> build.log
xcopy /Y %ZLIB_PATH%\contrib\minizip\zip.h "%ZLIB_PATH%\..\build\source\include\" >> build.log
xcopy /Y %ZLIB_PATH%\zconf.h "%ZLIB_PATH%\..\build\source\include\" >> build.log
xcopy /Y %ZLIB_PATH%\zlib.h "%ZLIB_PATH%\..\build\source\include\" >> build.log
echo [%DATE:~0,10% %TIME%][Info]build zlib end >> build.log

rem find "0 failed" occurs times in build.log, if occurs 1 time, then buildSucceess
for /f "delims=," %%i in ('find /c ", 0 failed" build.log') do set result=%%i
if %result:~-1%==1 (goto deepCheck) else (goto Fail)

:deepCheck
	find "0 File(s) copied" build.log | findstr "^0" > nul 2>&1 && set buildFailed=True
	if defined buildFailed (goto Fail) else (goto Success)
	
:Fail
	echo [%DATE:~0,10% %TIME%][Error]build zlib fail >> build.log
	echo build zlib fail
	goto Exit
	
:Success
	echo [%DATE:~0,10% %TIME%][Info]build zlib success >> build.log
	echo build zlib success
	goto Exit

:Exit
	echo [%DATE:~0,10% %TIME%][Info]************************************************************ >> build.log
	@echo ************************************************************