@echo off
REM *********************************************************************
REM *  _______  _______ _________ _______  _______ _________ _______   **
REM * (  ____ \(  ____ \\__   __/(  ___  )(  ____ )\__   __/(  ___  )  **
REM * | (    \/| (    \/   ) (   | (   ) || (    )|   ) (   | (   ) |  **
REM * | (_____ | |         | |   | |   | || (____)|   | |   | (___) |  **
REM * (_____  )| |         | |   | |   | ||  _____)   | |   |  ___  |  **
REM *       ) || |         | |   | |   | || (         | |   | (   ) |  **
REM * /\____) || (____/\___) (___| (___) || )         | |   | )   ( |  **
REM * \_______)(_______/\_______/(_______)|/          )_(   |/     \|  **
REM *                                                                  **
REM * (c) 2002,2003 Sciopta Systems GmbH, Litronic AG/ Schweiz         **
REM *                                                                  **
REM *********************************************************************
REM * ID: S04355BS1                                                    **
REM * $Revision: 1.4 $                                                 **
REM * $Date: 2006/06/20 06:09:32 $                                     **
REM * Build BATch for sfs library                                      **
REM *********************************************************************

REM We need delayed execution.

set CHECK=before
if "%CHECK%" == "before" (
  set CHECK=after
  if NOT "!CHECK!" == "after" (
    cmd /V:ON /C %0 %1
    exit /B
  )
)

if "%IAR%" == "" (
  echo Must set enviroment variable %%IAR%% pointing to EWARM
  exit
)

echo Building SFS
set SFS_MODULES=sh utils


set CC=%IAR%\bin\iccarm
set AR=%IAR%\..\common\bin\xlib

set INC=-I. -I%SCIOPTA_HOME%\include -I%SCIOPTA_HOME%\include\sciopta\arm
set INC=%INC% -I%IAR%\inc -I%SCIOPTA_HOME%\include\hcc

set CFLAGS=  --silent --debug --cpu_mode thumb --interwork 
set CFLAGS=%CFLAGS% --diag_suppress Pa050,Pe111,Pe174,Pa084,Pe175 -D__inline="" 
set CFLAGS=%CFLAGS% -Dstderr=9 -Dstdout=8 -Dfflush="" -Dfprintf=kprintf
REM --------------
REM Library names
REM --------------
set LIB=sfs.r79
set LIB0=sfs_0.r79
set LIB1=sfs_1.r79
set LIB2=sfs_2.r79

set LIBSC2HCC=sc2hcc.r79
set LIBSC2HCC0=sc2hcc_0.r79
set LIBSC2HCC1=sc2hcc_1.r79
set LIBSC2HCC2=sc2hcc_2.r79

REM ---------------------
REM Default optimizations
REM ---------------------
set O0= -z2
set OSIZE= -z9
set OSPEED= -s9

REM Check if delivery
if "%1%" == "" (
    set OPT=%OSIZE%
    call :doit
) ELSE (
    echo Building delivery libs

    set OPT=%O0%
    set LIB=%LIB0%
    set LIBSC2HCC=%LIBSC2HCC0%
    CALL :doit

    set OPT=%OSIZE%
    set LIB=%LIB1%
    set LIBSC2HCC=%LIBSC2HCC1%
    CALL :doit

    set OPT=%OSPEED%
    set LIB=%LIB2%
    set LIBSC2HCC=%LIBSC2HCC2%
    CALL :doit
)
goto :eof

:doit

%CC%  %CFLAGS% %OSIZE% %INC% version.c
%IAR%\..\common\bin\xar ..\lib\arm\iar\%LIB% version.r79
for /D %%i in (%SFS_MODULES%) DO (
    echo Building in %%i%
    pushd %%i%
    for %%o in (*.c) do (
	echo %%o%
	set L= %%o%
	set L=!L:.c=.r79!
	%CC% %CFLAGS% %OPT% %INC% %%o%
	%AR% -c "FETCH-MOD !L! ..\..\lib\arm\iar\%LIB%"
        del !L!
    )
    popd
)

echo Building in hcc\src
%IAR%\..\common\bin\xar ..\lib\arm\iar\%LIBSC2HCC% version.r79
pushd hcc\src
for %%o in (*.c) do (
    echo %%o%
    set L= %%o%
    set L=!L:.c=.r79!
    %CC% %CFLAGS% %OPT% %INC% %%o%
    %AR% -c "FETCH-MOD !L! ..\..\..\lib\arm\iar\%LIBSC2HCC%"
    del !L!
)
popd

del version.r79
goto :eof

:error
popd
echo Error: %ERRORLEVEL%
:exit
exit

