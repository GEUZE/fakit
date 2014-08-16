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
REM * $Revision: 1.6 $                                                 **
REM * $Date: 2008/02/19 13:28:39 $                                     **
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

echo Building SFS
set SFS_MODULES=sh utils

set CC=tcc
set AR=armar

set INC=-I. -J%SCIOPTA_HOME%\include -I%SCIOPTA_HOME%\include\sciopta\arm -J"%RVCT21INC%" -I%SCIOPTA_HOME%\include\hcc

set CFLAGS= --thumb -Wa -apcs /interwork -zo -Wb -g
set ARFLAGS= -r -c

REM --------------
REM Library names
REM --------------
set LIB=sfs.l
set LIB0=sfs_0.l
set LIB1=sfs_1.l
set LIB2=sfs_2.l

set LIBT=sfst.l
set LIB0T=sfs_0t.l
set LIB1T=sfs_1t.l
set LIB2T=sfs_2t.l

REM ---------------------
REM Default optimizations
REM ---------------------

set O0= -O0
set OSIZE= -Ospace
set OSPEED= -Otime

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
%CC% -c %CFLAGS% %OPT% %INC% version_sfs.c
%AR% %ARFLAGS% ..\lib\arm\arm\rvct21\%LIB% version_sfs.o
for /D %%i in (%SFS_MODULES%) DO (
    echo Building in %%i%
    pushd %%i%
    set L=
    for %%o in (*.c) DO (
      set L=!L! %%o%
    )
    echo Compiling: !L!
    %CC% -c %CFLAGS% %OPT% %INC% !L!
    %AR% -r ..\..\lib\arm\arm\rvct21\%LIB% *.o
    del *.o
    popd
)

goto :eof

:error
popd
echo Error: %ERRORLEVEL%
:exit
