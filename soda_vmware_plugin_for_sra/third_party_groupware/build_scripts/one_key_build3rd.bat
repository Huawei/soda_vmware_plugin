@echo off
set var1=
set var2=
set var3=
set var4=
set var5=
set var6=

@echo start building third-party packages...
call "build_openssl.bat"
find "build openssl fail" build.log > nul 2>&1 && set var1=True
if defined var1 (goto isFailed) else (goto BUILD_ZLIB)

:BUILD_ZLIB
	call "build_zlib_86.bat"
	find "build zlib fail" build.log > nul 2>&1 && set var2=True
	if defined var2 (goto isFailed) else (goto BUILD_LIBSSH)

:BUILD_LIBSSH
	call "build_libssh2.bat"
	find "build libssh2 fail" build.log > nul 2>&1 && set var3=True
	if defined var3 (goto isFailed ) else (goto BUILD_CURL)

:BUILD_CURL
	call "build_curl.bat"
	find "build libcurl fail" build.log > nul 2>&1 && set var4=True
	if defined var4 (goto isFailed) else (goto BUILD_ACE)

:BUILD_ACE
	call "build_ACE.bat"
	find "build ACE fail" build.log > nul 2>&1 && set var5=True
	if defined var5 (goto isFailed) else (goto BUILD_JSONCPP)
	
:BUILD_JSONCPP
	call "build_jsoncpp.bat"
	find "build jsoncpp fail" build.log > nul 2>&1 && set var6=True
	if defined var6 (goto isFailed) else (goto BUILD_TINYXML)
	
:BUILD_TINYXML
	call "build_tinyxml.bat"
	echo "build Tinyxml" >> build.log
    goto SUCCESS

:SUCCESS
	@echo Build third-party packages success
	@echo [%DATE:~0,10% %TIME%][Info]Build third-party packages success >> build.log
	goto Exit

:isFailed
	@echo Build third-party packages fail, see log in .\build.log
	@echo [%DATE:~0,10% %TIME%][Error]Build third-party packages fail, see log in .\build.log >> build.log
	goto Exit

:Exit