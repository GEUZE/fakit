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
REM * ID: S04355BS2                                                    **
REM * $Revision: 1.4 $                                                 **
REM * $Date: 2008/02/19 17:19:24 $                                     **
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

echo Building UTIL
set UTIL_MODULES=libsc logd container

set CC=tcc
set AR=armar

set INC=-I. -J%SCIOPTA_HOME%\include -I%SCIOPTA_HOME%\include\sciopta\arm -J"%RVCT22INC%"

set CFLAGS= --apcs /interwork --split_sections -g
set ARFLAGS= --create
REM --------------
REM Library names
REM --------------

set LIB=util.l
set LIB0=util_0.l
set LIB1=util_1.l
set LIB2=util_2.l

set LIBSH=sh.l
set LIBSH0=sh_0.l
set LIBSH1=sh_1.l
set LIBSH2=sh_2.l

set LIBT=utilt.l
set LIB0T=util_0t.l
set LIB1T=util_1t.l
set LIB2T=util_2t.l

set LIBSHT=sht.l
set LIBSH0T=sh_0t.l
set LIBSH1T=sh_1t.l
set LIBSH2T=sh_2t.l

REM ---------------------
REM Default optimizations
REM ---------------------
set O0= -O0
set OSIZE= -O2 -Ospace
set OSPEED= -O2 -Otime

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

    set OPT=%O0% -D__SYSCALL__
    set LIB=%LIB0T%
    set LIBSH=%LIBSH0T%
    CALL :doit

    set OPT=%OSIZE% -D__SYSCALL__
    set LIB=%LIB1T%
    set LIBSH=%LIBSH1T%
    CALL :doit

    set OPT=%OSPEED% -D__SYSCALL__
    set LIB=%LIB2T%
    set LIBSH=%LIBSH2T%
    CALL :doit

)
goto :eof

:doit
echo Options: %OPT% %LIB% %LIBSH%
%CC% -c %OSIZE% %CFLAGS% %INC% version_util.c
%AR% %ARFLAGS% ..\lib\arm\arm\rvct22\%LIB% version_util.o
for /D %%i in (%UTIL_MODULES%) DO (
    echo Building in %%i%
    pushd %%i%
    set L=
    for %%o in (*.c) DO (
      set L=!L! %%o%
    )
    echo Compiling: !L!
    %CC% -c %OPT% %CFLAGS% %INC% !L!
    %AR% -r ..\..\lib\arm\arm\rvct22\%LIB% *.o
    del *.o
    popd
)

%CC% -c %OSIZE% %CFLAGS% %INC% version_sh.c
%AR% %ARFLAGS% ..\lib\arm\arm\rvct22\%LIBSH% version_sh.o
echo Building in SH
pushd sh
set L=
for %%o in (*.c) DO (
  set L=!L! %%o%
)
echo Compiling: !L!
%CC% -c %CFLAGS% %OPT% %INC% !L!
%AR% -r ..\..\lib\arm\arm\rvct22\%LIBSH% *.o
del *.o
popd

goto :eof

:error
popd
echo Error: %ERRORLEVEL%
:exit