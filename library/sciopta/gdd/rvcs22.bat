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
REM * ID: S04324BS2                                                    **
REM * $Revision: 1.4 $                                                 **
REM * $Date: 2008/02/19 17:19:24 $                                     **
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

echo Building GDD
set GDD_MODULES=sdd scio tty fildev nulldev randdev pipe sem

set CC=tcc
set AR=armar

set INC=-I. -J%SCIOPTA_HOME%\include -I%SCIOPTA_HOME%\include\sciopta\arm -J"%RVCT22INC%"

set CFLAGS= --apcs /interwork --split_sections -g
set CFLAGS= %CFLAGS% -DSDD_MAN_NOTIFY=1

set ARFLAGS= --create
REM --------------
REM Library names
REM --------------

set LIB=gdd.l
set LIB0=gdd_0.l
set LIB1=gdd_1.l
set LIB2=gdd_2.l

set LIBT=gddt.l
set LIB0T=gdd_0t.l
set LIB1T=gdd_1t.l
set LIB2T=gdd_2t.l

REM ---------------------
REM Default optimizations
REM ---------------------

set O0= -O0
set OSIZE= -O2 -Ospace
set OSPEED= -O2 -Otime

REM Check if delivery
if "%1%" == "" (
    set OPT=%O0%
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

    set OPT=%O0% -D__SYSCALL__
    set LIB=%LIB0T%
    CALL :doit

    set OPT=%OSIZE% -D__SYSCALL__
    set LIB=%LIB1T%
    CALL :doit

    set OPT=%OSPEED% -D__SYSCALL__
    set LIB=%LIB2T%
    CALL :doit
)
goto :eof

:doit

%CC% -c %CFLAGS% %OPT% %INC% version_gdd.c
%AR% %ARFLAGS% ..\lib\arm\arm\rvct22\%LIB% version_gdd.o
for /D %%i in (%GDD_MODULES%) DO (
    echo Building in %%i%
    pushd %%i%
    set L=
    for %%o in (*.c) DO (
      set L=!L! %%o%
    )
    echo Compiling: !L!
    %CC% -c %CFLAGS% %OPT% %INC% !L!
    %AR% -r ..\..\lib\arm\arm\rvct22\%LIB% *.o
    del *.o
    popd
)
goto :eof

:error
popd
echo Error: %ERRORLEVEL%
:exit