@echo off
set buildFailed=
set result=

echo [%DATE:~0,10% %TIME%][Info]************************************************************ >> build.log
@echo ************************************************************
@echo start building libssh2...

echo [%DATE:~0,10% %TIME%][Info]start building libssh2... >> build.log
echo [%DATE:~0,10% %TIME%][Info]. >> build.log
echo [%DATE:~0,10% %TIME%][Info]-------------------compile libssh2------------------- >> build.log
echo [%DATE:~0,10% %TIME%][Info]. >> build.log
cd "%cd%"
rem set variables
SET LIBSSH2_PATH=%cd%\..\OpenSDS_Storage_Plugins\libssh2
SET SOLUTION="%LIBSSH2_PATH%\win32\libssh2.sln"
SET PROJECT="libssh2"
SET SOLUTION_CONFIG="OpenSSL DLL Release|Win32"
SET ACTION=Rebuild

@echo build libssh2, please wait...

echo [%DATE:~0,10% %TIME%][Info]start compiling libssh2... >> build.log
rem compile
"%VS110COMNTOOLS%\..\IDE\devenv" %SOLUTION% /%ACTION% %SOLUTION_CONFIG% /project %PROJECT% >> build.log

@echo copying temp files...

echo [%DATE:~0,10% %TIME%][Info]copying temp files... >> build.log
xCOPY "%LIBSSH2_PATH%\win32\Release_dll\*.dll" "%LIBSSH2_PATH%\bin\" /y >> build.log
xCOPY "%LIBSSH2_PATH%\win32\Release_dll\*.lib" "%LIBSSH2_PATH%\bin\" /y >> build.log
echo [%DATE:~0,10% %TIME%][Info]build libssh2 end >> build.log

for /f "delims=," %%i in ('find /c ", 0 failed" build.log') do set result=%%i
if %result:~-1%==2 (goto deepCheck) else (goto Fail)

:deepCheck
	find "0 File(s) copied" build.log | findstr "^0" > nul 2>&1 && set buildFailed=True
	if defined buildFailed (goto Fail) else (goto Success)
	
:Fail
	echo [%DATE:~0,10% %TIME%][Error]build libssh2 fail >> build.log
	echo build libssh2 fail
	goto Exit
	
:Success
	echo [%DATE:~0,10% %TIME%][Info]build libssh2 success >> build.log
	echo build libssh2 success
	goto Exit
	
:Exit
	echo [%DATE:~0,10% %TIME%][Info]************************************************************ >> build.log
	@echo ************************************************************