@echo off
set var1=
set var2=
@echo Start packaging sra.exe
echo [%DATE:~0,10% %TIME%][Info]Start packaging sra.exe >> build.log
SET PACKAGE_FILE_DIR=%cd%
SET PACKAGE_DIR_INIT =%PACKAGE_FILE_DIR%\sra\PROJECT_ASSISTANT
SET PACKAGE_DIR=%PACKAGE_DIR_INIT%\SINGLE_EXE_IMAGE\DiskImages\DISK1

rem Check files in directory srcfiles£¬All are necessary when package with InstallShield
@echo Checking necessary files...
echo [%DATE:~0,10% %TIME%][Info]Checking necessary files >> build.log
IF NOT EXIST %PACKAGE_FILE_DIR%\srcfile\sra.exe                            GOTO  isFailed

IF NOT EXIST %PACKAGE_FILE_DIR%\srcfile\command.pl                         GOTO  isFailed

IF NOT EXIST %PACKAGE_FILE_DIR%\srcfile\readme.txt                         GOTO  isFailed

IF NOT EXIST %PACKAGE_FILE_DIR%\License.txt                                GOTO  isFailed

IF EXIST %PACKAGE_DIR_INIT% RMDIR  /S/Q  %PACKAGE_DIR_INIT% 

rem  make package
@echo Making package
echo [%DATE:~0,10% %TIME%][Info]Making package >> build.log
"%InstallShield%\System\IsCmdBld.exe" -p  "%PACKAGE_FILE_DIR%\sra.ism" >> build.log

find "fatal error -1014" build.log > nul 2>&1 && set var1=True
if defined var1 goto RenameError

find "0 error(s)" build.log > nul 2>&1 && set var2=True
if not defined var2 goto FinalError

echo Package Success, get package under .\sra\PROJECT_ASSISTANT\SINGLE_EXE_IMAGE\DiskImages\DISK1
echo [%DATE:~0,10% %TIME%][Info]Package Success, get package under .\sra\PROJECT_ASSISTANT\SINGLE_EXE_IMAGE\DiskImages\DISK1 >> build.log
goto Exit

:isFailed
	echo [%DATE:~0,10% %TIME%][Error]srafiles not Not Found, more details see .\build.log >> build.log
	echo %DATE% %TIME% srafiles not Not Found, more details see .\build.log
	goto Exit

:PackageNotFound
	echo [%DATE:~0,10% %TIME%][Error]Package Oceanspace SRA-0.0.1.exe not Not Found, more details see .\build.log >> build.log
	echo %DATE% %TIME% Package Oceanspace SRA-0.0.1.exe not Not Found, more details see .\build.log
	goto Exit

:RenameError
	echo [%DATE:~0,10% %TIME%][Error]Cannot rename directory .\sra\PROJECT_ASSISTANT\SINGLE_EXE_IMAGE\DiskImages\DISK1, please make sure the directory was not opened, more details see .\build.log >> build.log
	echo %DATE% %TIME% Cannot rename directory .\sra\PROJECT_ASSISTANT\SINGLE_EXE_IMAGE\DiskImages\DISK1, please make sure the directory was not opened, more details see .\build.log
	goto Exit
	
:FinalError
	echo [%DATE:~0,10% %TIME%][Error]Making package fail, more details see .\build.log >> build.log
	echo %DATE% %TIME% [Error]Making package fail, more details see .\build.log
	goto Exit
:Exit