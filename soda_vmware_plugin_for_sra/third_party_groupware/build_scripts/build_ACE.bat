@echo off
set buildFailed=
set result=

@echo ************************************************************
@echo start building ACE...
echo [%DATE:~0,10% %TIME%][Info]************************************************************ >> build.log
echo [%DATE:~0,10% %TIME%][Info]start building ACE... >> build.log
echo [%DATE:~0,10% %TIME%][Info]. >> build.log
echo [%DATE:~0,10% %TIME%][Info]-------------------compile ACE------------------- >> build.log
echo [%DATE:~0,10% %TIME%][Info]. >> build.log

cd "%cd%"
rem  setting variables
SET ACE_PATH=%cd%\..\OpenSDS_Storage_Plugins\ACE\source
SET SOLUTION="%ACE_PATH%\ace\ace_vc12.sln"
SET SOLUTION_CONFIG="Release|Win32"
SET ACTION=Rebuild 

@echo build ACE, please wait...

echo [%DATE:~0,10% %TIME%][Info]Compiling ACE... >> build.log
"%VS110COMNTOOLS%\..\IDE\devenv" %SOLUTION% /%ACTION% %SOLUTION_CONFIG% >> build.log

@echo Copying comiled files..
echo [%DATE:~0,10% %TIME%][Info]Copying comiled files.. >> build.log
xCOPY "%ACE_PATH%\lib\release\ACE.lib"  "%ACE_PATH%\..\lib\win32_x86_msvc\release\" /y >> build.log
xCOPY "%ACE_PATH%\lib\release\bin\ACE.dll"  "%ACE_PATH%\..\bin\win32_x86_msvc\release\" /y >> build.log

xCOPY "%ACE_PATH%\lib\release\ACE_SSL.lib"  "%ACE_PATH%\..\lib\win32_x86_msvc\release\" /y >> build.log
xCOPY "%ACE_PATH%\lib\release\bin\ACE_SSL.dll"  "%ACE_PATH%\..\bin\win32_x86_msvc\release\" /y >> build.log

xCOPY "%ACE_PATH%\ace\os_include\arpa\*.h"  "%ACE_PATH%\..\include\ace\os_include\arpa\" /y >> build.log
xCOPY "%ACE_PATH%\ace\os_include\net\*.h"  "%ACE_PATH%\..\include\ace\os_include\net\" /y >> build.log
xCOPY "%ACE_PATH%\ace\os_include\netinet\*.h"  "%ACE_PATH%\..\include\ace\os_include\netinet\" /y >> build.log
xCOPY "%ACE_PATH%\ace\os_include\sys\*.h"  "%ACE_PATH%\..\include\ace\os_include\sys\" /y >> build.log
xCOPY "%ACE_PATH%\ace\SSL"  "%ACE_PATH%\..\include\ace\SSL\" /y >> build.log
xCOPY "%ACE_PATH%\ace\os_include"  "%ACE_PATH%\..\include\ace\os_include\" /y >> build.log
xCOPY "%ACE_PATH%\ace\*.*"  "%ACE_PATH%\..\include\ace\" /y >> build.log

@echo Removing temp files..
echo [%DATE:~0,10% %TIME%][Info]Removing temp files.. >> build.log

set logfile=
(
REM del "%ACE_PATH%\..\build\ACE\include\ace\*.opensdf
del "%ACE_PATH%\..\include\ace\*.sln
del "%ACE_PATH%\..\include\ace\*.vcxproj
del "%ACE_PATH%\..\include\ace\*.vcxproj.filters
del "%ACE_PATH%\..\include\ace\*.vcxproj.user
) >> build.log

echo [%DATE:~0,10% %TIME%][Info]build ACE end >> build.log

for /f "delims=," %%i in ('find /c ", 0 failed" build.log') do set result=%%i
if %result:~-1%==4 (goto deepCheck) else (goto Fail)

:deepCheck
	find "0 File(s) copied" build.log | findstr "^0" > nul 2>&1 && set buildFailed=True
	if defined buildFailed (goto Fail) else (goto Success)
	
:Fail
	echo [%DATE:~0,10% %TIME%][Error]build ACE fail >> build.log
	echo build ACE fail
	goto Exit
	
:Success
	echo [%DATE:~0,10% %TIME%][Info]build ACE success >> build.log
	echo build ACE success
	goto Exit

:Exit
	echo [%DATE:~0,10% %TIME%][Info]************************************************************ >> build.log
	@echo ************************************************************