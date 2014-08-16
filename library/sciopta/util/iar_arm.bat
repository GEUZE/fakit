@REM -*-sh-*-
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
REM * $Revision: 1.5 $                                                 **
REM * $Date: 2006/11/28 09:59:48 $                                     **
REM * Build BATch for util library                                     **
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

echo Building UTIL
set UTIL_MODULES=libsc logd container


set CC=%IAR%\bin\iccarm
set AR=%IAR%\..\common\bin\xlib

set INC=-I. -I%SCIOPTA_HOME%\include -I%SCIOPTA_HOME%\include\sciopta\arm
set INC=%INC% -I%IAR%\inc

set CFLAGS= --debug --cpu_mode thumb --interwork --diag_suppress Pa050 --silent -D__inline=""
set CFLAGS= %CFLAGS% -Dstderr=9 -Dstdout=8 -Dfflush="" -Dfprintf=kprintf 

REM --------------
REM Library names
REM --------------
set LIB=util
set LIB0=util_0
set LIB1=util_1
set LIB2=util_2

set LIBSH=sh
set LIBSH0=sh_0
set LIBSH1=sh_1
set LIBSH2=sh_2

set EXT=.r79
set CPU=arm
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
    set LIBSH=%LIBSH0%
    CALL :doit

    set OPT=%OSIZE%
    set LIB=%LIB1%
    set LIBSH=%LIBSH1%
    CALL :doit

    set OPT=%OSPEED%
    set LIB=%LIB2%
    set LIBSH=%LIBSH2%
    CALL :doit
)
goto :eof

:doit
%CC%  %CFLAGS% %OPT% %INC% version.c
%IAR%\..\common\bin\xar ..\lib\%CPU%\iar\%LIB%%EXT% version%EXT%
for /D %%i in (%UTIL_MODULES%) DO (
    echo Building in %%i%
    pushd %%i%
    for %%o in (*.c) DO (
	echo %%o%
	set L= %%o%
	set L=!L:.c=%EXT%!
	%CC% %CFLAGS% %OPT% %INC% %%o%
	%AR% -c "FETCH-MOD !L! ..\..\lib\%CPU%\iar\%LIB%%EXT%"
        del !L!
    )
    popd
)

%IAR%\..\common\bin\xar ..\lib\%CPU%\iar\%LIBSH%%EXT% version%EXT%
echo Building in sh
pushd sh
for %%o in (*.c) do (
  echo %%o%
  set L= %%o%
  set L=!L:.c=%EXT%!
   %CC% %CFLAGS% %OPT% --diag_suppress Pe174 %INC% %%o%
   %AR% -c "FETCH-MOD !L! ..\..\lib\%CPU%\iar\%LIBSH%%EXT%"
   del !L!
)
popd
del version%EXT%
goto :eof

:error
popd
echo Error: %ERRORLEVEL%
:exit
exit









