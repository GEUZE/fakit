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
REM * $Revision: 2.4 $                                                 **
REM * $Date: 2008/02/19 17:18:33 $                                     **
REM * Build BATch for ips library                                     **
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

echo Building IPS
set IPS_MODULES=device icmp igmp ips ipv4 link ppp tcp udp

set CC=tcc
set AR=armar

set INC=-J%SCIOPTA_HOME%\include -I%SCIOPTA_HOME%\include\sciopta\arm
set INC=%INC% -I%SCIOPTA_HOME%\ips -J"%RVCT22INC%"

set CFLAGS= --thumb --apcs /interwork --split_sections -g
set ARFLAGS= --create

REM --------------
REM Library names
REM --------------

set _LIB=ips
set _LIBHTTP=http
set _LIBDNS=dns
set _LIBDHCP=dhcp
set _LIBTELNET=telnet
set _LIBTFTP=tftp
set _LIBSMTP=smtp
set _LIBFTP=ftp

REM ---------------------
REM Default optimizations
REM ---------------------
set O0= -O0
set OSIZE= -O2 -Ospace
set OSPEED= -O2 -Otime

REM ----------------------
REM IPS configurations
REM ----------------------
set SMALL="-DIPS_CONF=<ips_config_small.h>"
set FULL="-DIPS_CONF=<ips_config_full.h>"
set NORMAL="-DIPS_CONF=<ips_config.h>"

REM Check if delivery
if "%1%" == "" (
    set OPT=%OSIZE%
    set LIB=%LIB%.l
    set LIBHTTP=%LIBHTTP%.l
    set LIBDNS=%LIBDNS%.l
    set LIBDHCP=%LIBDHCP%.l
    set LIBTELNET=%LIBTELNET%.l
    set LIBTFTP=%LIBTFTP%.l
    set LIBSMTP=%LIBSMTP%.l
    set LIBFTP=%LIBFTP%.l
    call :doit
) ELSE (
    set EXT=.l
    call :all
    
    set EXT=t.l
    set CFLAGS=%CFLAGS% -D__SYSCALL__
    call :all
)
goto :eof

:all
echo Building delivery libs

set OPT=%O0% %NORMAL%
set LIB=%_LIB%_0%EXT%
set LIBHTTP=%_LIBHTTP%_0%EXT%
set LIBDNS=%_LIBDNS%_0%EXT%
set LIBDHCP=%_LIBDHCP%_0%EXT%
set LIBTELNET=%_LIBTELNET%_0%EXT%
set LIBTFTP=%_LIBTFTP%_0%EXT%
set LIBSMTP=%_LIBSMTP%_0%EXT%
set LIBFTP=%_LIBFTP%_0%EXT%
CALL :doit

set OPT=%O0% %SMALL%
set LIB=%_LIB%_0s%EXT%
set LIBHTTP=%_LIBHTTP%_0s%EXT%
set LIBDNS=%_LIBDNS%_0s%EXT%
set LIBDHCP=%_LIBDHCP%_0s%EXT%
set LIBTELNET=%_LIBTELNET%_0s%EXT%
set LIBTFTP=%_LIBTFTP%_0s%EXT%
set LIBSMTP=%_LIBSMTP%_0s%EXT%
set LIBFTP=%_LIBFTP%_0s%EXT%
CALL :doit

set OPT=%O0% %FULL%
set LIB=%_LIB%_0f%EXT%
set LIBHTTP=%_LIBHTTP%_0f%EXT%
set LIBDNS=%_LIBDNS%_0f%EXT%
set LIBDHCP=%_LIBDHCP%_0f%EXT%
set LIBTELNET=%_LIBTELNET%_0f%EXT%
set LIBTFTP=%_LIBTFTP%_0f%EXT%
set LIBSMTP=%_LIBSMTP%_0f%EXT%
set LIBFTP=%_LIBFTP%_0f%EXT%
CALL :doit

set OPT=%OSIZE% %NORMAL%
set LIB=%_LIB%_1%EXT%
set LIBHTTP=%_LIBHTTP%_1%EXT%
set LIBDNS=%_LIBDNS%_1%EXT%
set LIBDHCP=%_LIBDHCP%_1%EXT%
set LIBTELNET=%_LIBTELNET%_1%EXT%
set LIBTFTP=%_LIBTFTP%_1%EXT%
set LIBSMTP=%_LIBSMTP%_1%EXT%
set LIBFTP=%_LIBFTP%_1%EXT%
CALL :doit

set OPT=%OSIZE% %SMALL%
set LIB=%_LIB%_1s%EXT%
set LIBHTTP=%_LIBHTTP%_1s%EXT%
set LIBDNS=%_LIBDNS%_1s%EXT%
set LIBDHCP=%_LIBDHCP%_1s%EXT%
set LIBTELNET=%_LIBTELNET%_1s%EXT%
set LIBTFTP=%_LIBTFTP%_1s%EXT%
set LIBSMTP=%_LIBSMTP%_1s%EXT%
set LIBFTP=%_LIBFTP%_1s%EXT%
CALL :doit

set OPT=%OSIZE% %FULL%
set LIB=%_LIB%_1f%EXT%
set LIBHTTP=%_LIBHTTP%_1f%EXT%
set LIBDNS=%_LIBDNS%_1f%EXT%
set LIBDHCP=%_LIBDHCP%_1f%EXT%
set LIBTELNET=%_LIBTELNET%_1f%EXT%
set LIBTFTP=%_LIBTFTP%_1f%EXT%
set LIBSMTP=%_LIBSMTP%_1f%EXT%
set LIBFTP=%_LIBFTP%_1f%EXT%
CALL :doit

set OPT=%OSPEED% %NORMAL%
set LIB=%_LIB%_2%EXT%
set LIBHTTP=%_LIBHTTP%_2%EXT%
set LIBDNS=%_LIBDNS%_2%EXT%
set LIBDHCP=%_LIBDHCP%_2%EXT%
set LIBTELNET=%_LIBTELNET%_2%EXT%
set LIBTFTP=%_LIBTFTP%_2%EXT%
set LIBSMTP=%_LIBSMTP%_2%EXT%
set LIBFTP=%_LIBFTP%_2%EXT%
CALL :doit

set OPT=%OSPEED% %SMALL%
set LIB=%_LIB%_2s%EXT%
set LIBHTTP=%_LIBHTTP%_2s%EXT%
set LIBDNS=%_LIBDNS%_2s%EXT%
set LIBDHCP=%_LIBDHCP%_2s%EXT%
set LIBTELNET=%_LIBTELNET%_2s%EXT%
set LIBTFTP=%_LIBTFTP%_2s%EXT%
set LIBSMTP=%_LIBSMTP%_2s%EXT%
set LIBFTP=%_LIBFTP%_2s%EXT%
CALL :doit

set OPT=%OSPEED% %FULL%
set LIB=%_LIB%_2f%EXT%
set LIBHTTP=%_LIBHTTP%_2f%EXT%
set LIBDNS=%_LIBDNS%_2f%EXT%
set LIBDHCP=%_LIBDHCP%_2f%EXT%
set LIBTELNET=%_LIBTELNET%_2f%EXT%
set LIBTFTP=%_LIBTFTP%_2f%EXT%
set LIBSMTP=%_LIBSMTP%_2f%EXT%
set LIBFTP=%_LIBFTP%_2f%EXT%
CALL :doit
goto :eof

:doit
echo Lib: %LIB%
%CC% -c %CFLAGS% %OPT% %INC% version_ips.c
%AR% %ARFLAGS% ..\lib\arm\arm\rvct22\%LIB% version_ips.o
for /D %%i in (%IPS_MODULES%) DO (
REM    echo Building in %%i%
    pushd %%i%
    set L=
    for %%o in (*.c) DO (
      set L=!L! %%o%
    )
REM    echo Compiling: !L!
    %CC% -c %CFLAGS% %OPT% %INC% !L!
    %AR% -r ..\..\lib\arm\arm\rvct22\%LIB% *.o
    popd
)

echo Building in http
%AR% %ARFLAGS% ..\lib\arm\arm\rvct22\%LIBHTTP% version_ips.o
pushd http
set L=
for %%o in (*.c) DO (
  set L=!L! %%o%
)
REM echo Compiling: !L!
%CC% -c %CFLAGS% %OPT% %INC% !L!
%AR% -r ..\..\lib\arm\arm\rvct22\%LIBHTTP% *.o
popd

echo Building in dns
%AR% %ARFLAGS% ..\lib\arm\arm\rvct22\%LIBDNS% version_ips.o
pushd dns
set L=
for %%o in (*.c) DO (
  set L=!L! %%o%
)
REM echo Compiling: !L!
%CC% -c %CFLAGS% %OPT% %INC% !L!
%AR% -r ..\..\lib\arm\arm\rvct22\%LIBDNS% *.o
popd

echo Building in bootp
%AR% %ARFLAGS% ..\lib\arm\arm\rvct22\%LIBDHCP% version_ips.o
pushd bootp
set L=
for %%o in (*.c) DO (
  set L=!L! %%o%
)
REM echo Compiling: !L!
%CC% -c %CFLAGS% %OPT% %INC% !L!
%AR% -r ..\..\lib\arm\arm\rvct22\%LIBDHCP% *.o
popd

echo Building in telnet
%AR% %ARFLAGS% ..\lib\arm\arm\rvct22\%LIBTELNET% version_ips.o
pushd telnet
set L=
for %%o in (*.c) DO (
  set L=!L! %%o%
)
REM echo Compiling: !L!
%CC% -c %CFLAGS% %OPT% %INC% !L!
%AR% -r ..\..\lib\arm\arm\rvct22\%LIBTELNET% *.o
popd

echo Building in tftp
%AR% %ARFLAGS% ..\lib\arm\arm\rvct22\%LIBTFTP% version_ips.o
pushd tftp
set L=
for %%o in (*.c) DO (
  set L=!L! %%o%
)
REM echo Compiling: !L!
%CC% -c %CFLAGS% %OPT% %INC% !L!
%AR% -r ..\..\lib\arm\arm\rvct22\%LIBTFTP% *.o
popd

echo Building in smtp
%AR% %ARFLAGS% ..\lib\arm\arm\rvct22\%LIBSMTP% version_ips.o
pushd smtp
set L=
for %%o in (*.c) DO (
  set L=!L! %%o%
)
REM echo Compiling: !L!
%CC% -c %CFLAGS% %OPT% %INC% !L!
%AR% -r ..\..\lib\arm\arm\rvct22\%LIBSMTP% *.o
popd

echo Building in ftp
%AR% %ARFLAGS% ..\lib\arm\arm\rvct22\%LIBFTP% version_ips.o
pushd ftp
set L=
for %%o in (*.c) DO (
  set L=!L! %%o%
)
REM echo Compiling: !L!
%CC% -c %CFLAGS% %OPT% %INC% !L!
%AR% -r ..\..\lib\arm\arm\rvct22\%LIBFTP% *.o
popd

del version_ips.o

goto :eof

:error
popd
echo Error: %ERRORLEVEL%
:exit

