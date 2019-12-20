@echo off
set buildFailed=
set result=

echo [%DATE:~0,10% %TIME%][Info]************************************************************ >> build.log
@echo ************************************************************
@echo start building jsoncpp...
echo [%DATE:~0,10% %TIME%][Info]start building jsoncpp... >> build.log
echo [%DATE:~0,10% %TIME%][Info]. >> build.log
echo [%DATE:~0,10% %TIME%][Info]-------------------compile jsoncpp------------------- >> build.log
echo [%DATE:~0,10% %TIME%][Info]. >> build.log
cd "%cd%"
rem setting variables
SET JSONCPP_PATH=%cd%\..\OpenSDS_Storage_Plugins\jsoncpp\source
SET SOLUTION="%JSONCPP_PATH%\makefiles\msvc2010\lib_json.vcxproj"
SET SOLUTION_CONFIG="Release|Win32"
SET ACTION=Rebuild
SET PROJECT="lib_json"

@echo build jsoncpp, please wait...
echo [%DATE:~0,10% %TIME%][Info]Compiling jsoncpp... >> build.log
rem Compiling
"%VS110COMNTOOLS%\..\IDE\devenv" %SOLUTION% /%ACTION% %SOLUTION_CONFIG% /project %PROJECT% >> build.log

@echo Copying comiled files..
echo [%DATE:~0,10% %TIME%][Info]Copying comiled files.. >> build.log
xCOPY "%JSONCPP_PATH%\..\..\ACE\source\lib\release\lib_json.lib" "%JSONCPP_PATH%\..\lib\win32_x86_msvc\release\" /y >> build.log
xCOPY "%JSONCPP_PATH%\..\..\ACE\source\lib\release\lib_json.dll" "%JSONCPP_PATH%\..\bin\win32_x86_msvc\release\" /y >> build.log
xCOPY "%JSONCPP_PATH%\include\json\*.h" "%JSONCPP_PATH%\..\include\json\" /y >> build.log

for /f "delims=," %%i in ('find /c ", 0 failed" build.log') do set result=%%i
if %result:~-1%==5 (goto deepCheck) else (goto Fail)

:deepCheck
	find "0 File(s) copied" build.log | findstr "^0" > nul 2>&1 && set buildFailed=True
	if defined buildFailed (goto Fail) else (goto Success)
	
:Fail
	echo [%DATE:~0,10% %TIME%][Error]build jsoncpp fail >> build.log
	echo build jsoncpp fail
	goto Exit
	
:Success
	echo [%DATE:~0,10% %TIME%][Info]build jsoncpp success >> build.log
	echo build jsoncpp success
	goto Exit

:Exit
	echo [%DATE:~0,10% %TIME%][Info]************************************************************ >> build.log
	@echo ************************************************************