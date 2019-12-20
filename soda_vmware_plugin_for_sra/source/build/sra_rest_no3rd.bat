@echo off
set time_s=%time:~0,8%
if exist build.log ren build.log build_"%time_s::=%".log

setlocal EnableDelayedExpansion
@echo Start building SRA...

set Drive=%~d0
set CurBatPath=%~dp0
echo CurBatPath is %CurBatPath%
if exist "%CurBatPath%build.log" (del "%CurBatPath%build.log")
if not exist "%CurBatPath%..\bin\" (md "%CurBatPath%..\bin\") >> build.log

set SOLUTION=%CurBatPath%..\sra.sln
echo SOLUTION is %SOLUTION%

set VCTOOL_HOME=
call :FindVCTOOLHome VCTOOL_HOME

echo VCTOOL_HOME is %VCTOOL_HOME%
set DEVEVN_FILE=%VCTOOL_HOME%\..\IDE\devenv.exe

cd.>"%CurBatPath%build.log"
set BUILD_LOG="%CurBatPath%build.log"

echo include is %INCLUDE%

rem clean openssl 32 lib file
rem setting the enviroment with VC command 32
set SOLUTION_CONFIG="Release|Win32"
set ACTION=Rebuild
call %VCTOOL_HOME%vsvars32.bat

cd %CurBatPath%

@echo Copy third party binaries compiled before 
echo [%DATE:~0,10% %TIME%][Info]Copy third party binaries compiled before >> build.log
COPY /Y "%CurBatPath%..\..\third_party_groupware\OpenSDS_Storage_Plugins\openssl\bin\win32_x86_msvc\release\libcrypto-1_1.dll"  "%CurBatPath%..\bin\libcrypto-1_1.dll" >> build.log

COPY /Y "%CurBatPath%..\..\third_party_groupware\OpenSDS_Storage_Plugins\openssl\bin\win32_x86_msvc\release\libssl-1_1.dll"  "%CurBatPath%..\bin\libssl-1_1.dll" >> build.log

COPY /Y "%CurBatPath%..\..\third_party_groupware\OpenSDS_Storage_Plugins\libssh2\bin\libssh2.dll"  "%CurBatPath%..\bin\libssh2.dll" >> build.log

COPY /Y "%CurBatPath%..\..\third_party_groupware\OpenSDS_Storage_Plugins\libssh2\bin\libssh2.lib"  "%CurBatPath%..\lib\libssh2.lib" >> build.log

@echo Begin to clean compile
echo [%DATE:~0,10% %TIME%][Info]Begin to clean compile >> %BUILD_LOG%
echo %DEVEVN_FILE% "%SOLUTION%" /clean
%DEVEVN_FILE% "%SOLUTION%" /clean

@echo Begin to rebuild SRA, please wait...
echo [%DATE:~0,10% %TIME%][Info]Begin to rebuild SRA >> %BUILD_LOG%
echo %DEVEVN_FILE% %SOLUTION% /%ACTION% %SOLUTION_CONFIG% /useenv
rem "D:\Program Files\Microsoft Visual Studio 8\Common7\IDE\devenv" D:\Work\APM\Code\C10_Agent\agent\vsprj\UltraAgent\UltraAgent.sln /Rebuild "Release|Win32" /useenv
%DEVEVN_FILE% "%SOLUTION%" /%ACTION% %SOLUTION_CONFIG% /useenv /Out %BUILD_LOG%

if not exist "%CurBatPath%..\bin\sra.exe" (goto CompileFailed)

@echo Copying compiled binaries...

echo [%DATE:~0,10% %TIME%][Info]Copying compiled binaries >> build.log
COPY /Y "%CurBatPath%..\bin\sra.exe"  "%CurBatPath%..\build\srcfile\sra.exe" >> build.log

COPY /Y "%CurBatPath%..\bin\*.dll"  "%CurBatPath%..\build\srcfile" >> build.log

COPY /Y "%CurBatPath%..\..\third_party_groupware\OpenSDS_Storage_Plugins\win32_x86_msvc2010\*.dll"  "%CurBatPath%..\build\srcfile" >> build.log

echo End to build x32 Agent
@echo Build SRA success
echo [%DATE:~0,10% %TIME%][Info]Build SRA success >> %BUILD_LOG%
goto :eof



rem
rem Find the Visual Studio Tool's Installed Path by query enviroment
rem  %1: return value, the path where Visual Studio Tool installed, if not found, return "n/a"
rem
:FindVCTOOLHome
    set VCTOOLTemp=n/a

    rem Query the registry to find the VC's install Path 
    set | findstr "VS110COMNTOOLS" >temp.txt 2>nul
    for /f "tokens=2,* delims==" %%i in (temp.txt) do set VCTOOLTemp=%%i
        
    if "%VCTOOLTemp%" NEQ "n/a" (set %1="%VCTOOLTemp%")
    call :DeleteFile temp.txt
goto :eof

rem
rem Delete a file
rem
:DeleteFile
    set FileName=%~1
    if exist "%FileName%" (del /f /q "%FileName%")
goto :eof

:CompileFailed
@echo  [%DATE:~0,10% %TIME%] Compile Failed, more details see .\build.log.
echo  [%DATE:~0,10% %TIME%][Error]Compile Failed, more details see .\build.log. >> %BUILD_LOG%
goto Exit

:Exit

