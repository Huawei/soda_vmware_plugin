@echo off
set buildFailed=
set result=

@echo ************************************************************
echo [%DATE:~0,10% %TIME%][Info]************************************************************ >> build.log
@echo start building libcurl...
echo [%DATE:~0,10% %TIME%][Info]start building libcurl... >> build.log
echo [%DATE:~0,10% %TIME%][Info]. >> build.log
echo [%DATE:~0,10% %TIME%][Info]-------------------compile libcurl------------------- >> build.log
echo [%DATE:~0,10% %TIME%][Info]. >> build.log
cd "%cd%"
rem setting variables
SET LIBCURL_PATH=%cd%\..\OpenSDS_Storage_Plugins\curl\source
SET SOLUTION="%LIBCURL_PATH%\projects\Windows\VC11\curl-all.sln"
SET SOLUTION_CONFIG="DLL Release - DLL OpenSSL - DLL LibSSH2|Win32"
SET ACTION=Rebuild

@echo build libcurl, please wait...
echo [%DATE:~0,10% %TIME%][Info]start compiling libcurl... >> build.log
rem compiling
"%VS110COMNTOOLS%\..\IDE\devenv" %SOLUTION% /%ACTION% %SOLUTION_CONFIG% >> build.log

@echo copying temp files...
echo [%DATE:~0,10% %TIME%][Info]copying temp files... >> build.log
xCOPY "%LIBCURL_PATH%\build\Win32\VC11\DLL Release - DLL OpenSSL - DLL LibSSH2\*.lib" "%LIBCURL_PATH%\..\lib\win32_x86_msvc\release\" /y >> build.log
xCOPY "%LIBCURL_PATH%\build\Win32\VC11\DLL Release - DLL OpenSSL - DLL LibSSH2\*.dll" "%LIBCURL_PATH%\..\bin\win32_x86_msvc\release\" /y >> build.log
xCOPY "%LIBCURL_PATH%\include\curl\*.h" "%LIBCURL_PATH%\..\include\curl\" /y >> build.log
echo [%DATE:~0,10% %TIME%][Info]build libcurl end >> build.log

for /f "delims=," %%i in ('find /c ", 0 failed" build.log') do set result=%%i
if %result:~-1%==3 (goto deepCheck) else (goto Fail)

:deepCheck
	find "0 File(s) copied" build.log | findstr "^0" > nul 2>&1 && set buildFailed=True
	if defined buildFailed (goto Fail) else (goto Success)
	
:Fail
	echo [%DATE:~0,10% %TIME%][Error]build libcurl fail >> build.log
	echo build libcurl fail
	goto Exit
	
:Success
	echo [%DATE:~0,10% %TIME%][Info]build libcurl success >> build.log
	echo build libcurl success
	goto Exit
	
:Exit
	echo [%DATE:~0,10% %TIME%][Info]************************************************************ >> build.log
	@echo ************************************************************