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
REM * Build BATch for gdd library                                      **
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

echo Building GDD
set GDD_MODULES=sdd scio tty fildev nulldev randdev pipe sem


set CC=%IAR%\bin\iccarm
set AR=%IAR%\..\common\bin\xlib

set INC=-I. -I%SCIOPTA_HOME%\include -I%SCIOPTA_HOME%\include\sciopta\arm
set INC=%INC% -I%IAR%\inc

set CFLAGS= --debug --cpu_mode thumb --interwork --diag_suppress Pa050 --silent
set CFLAGS= %CFLAGS% -DSDD_MAN_NOTIFY=1

REM --------------
REM Library names
REM --------------
set LIB=gdd
set LIB0=gdd_0
set LIB1=gdd_1
set LIB2=gdd_2
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
    CALL :doit

    set OPT=%OSIZE%
    set LIB=%LIB1%
    CALL :doit

    set OPT=%OSPEED%
    set LIB=%LIB2%
    CALL :doit
)
goto :eof

:doit
%CC%  %CFLAGS% %OSIZE% %INC% version.c
%IAR%\..\common\bin\xar ..\lib\%CPU%\iar\%LIB%%EXT% version%EXT%
for /D %%i in (%GDD_MODULES%) DO (
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
del version%EXT%
goto :eof

:error
popd
echo Error: %ERRORLEVEL%
:exit
exit
