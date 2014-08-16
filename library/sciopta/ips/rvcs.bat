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
REM * $Revision: 1.9 $                                                 **
REM * $Date: 2008/02/19 13:28:38 $                                     **
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
set INC=%INC% -I%SCIOPTA_HOME%\ips -J"%RVCT21INC%"

set CFLAGS= -Wa -apcs /interwork -zo -Wb
set ARFLAGS= --create

REM --------------
REM Library names
REM --------------

set LIB=ips
set LIBHTTP=http
set LIBDNS=dns
set LIBDHCP=dhcp
set LIBTELNET=telnet
set LIBTFTP=tftp
set LIBSMTP=smtp
set LIBFTP=ftp

REM ---------------------
REM Default optimizations
REM ---------------------
set O0= -O0
set OSIZE= -Ospace
set OSPEED= -Otime

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
    echo Building delivery libs

    set OPT=%O0% %NORMAL%
    set LIB=%LIB%_0.l
    set LIBHTTP=%LIBHTTP%_0.l
    set LIBDNS=%LIBDNS%_0.l
    set LIBDHCP=%LIBDHCP%_0.l
    set LIBTELNET=%LIBTELNET%_0.l
    set LIBTFTP=%LIBTFTP%_0.l
    set LIBSMTP=%LIBSMTP%_0.l
    set LIBFTP=%LIBFTP%_0.l
    CALL :doit

    set OPT=%O0% %SMALL%
    set LIB=%LIB%_0s.l
    set LIBHTTP=%LIBHTTP%_0s.l
    set LIBDNS=%LIBDNS%_0s.l
    set LIBDHCP=%LIBDHCP%_0s.l
    set LIBTELNET=%LIBTELNET%_0s.l
    set LIBTFTP=%LIBTFTP%_0s.l
    set LIBSMTP=%LIBSMTP%_0s.l
    set LIBFTP=%LIBFTP%_0s.l
    CALL :doit

    set OPT=%O0% %FULL%
    set LIB=%LIB%_0f.l
    set LIBHTTP=%LIBHTTP%_0f.l
    set LIBDNS=%LIBDNS%_0f.l
    set LIBDHCP=%LIBDHCP%_0f.l
    set LIBTELNET=%LIBTELNET%_0f.l
    set LIBTFTP=%LIBTFTP%_0f.l
    set LIBSMTP=%LIBSMTP%_0f.l
    set LIBFTP=%LIBFTP%_0f.l
    CALL :doit

    set OPT=%OSIZE% %NORMAL%
    set LIB=%LIB%_1.l
    set LIBHTTP=%LIBHTTP%_1.l
    set LIBDNS=%LIBDNS%_1.l
    set LIBDHCP=%LIBDHCP%_1.l
    set LIBTELNET=%LIBTELNET%_1.l
    set LIBTFTP=%LIBTFTP%_1.l
    set LIBSMTP=%LIBSMTP%_1.l
    set LIBFTP=%LIBFTP%_1.l
    CALL :doit

    set OPT=%OSIZE% %SMALL%
    set LIB=%LIB%_1s.l
    set LIBHTTP=%LIBHTTP%_1s.l
    set LIBDNS=%LIBDNS%_1s.l
    set LIBDHCP=%LIBDHCP%_1s.l
    set LIBTELNET=%LIBTELNET%_1s.l
    set LIBTFTP=%LIBTFTP%_1s.l
    set LIBSMTP=%LIBSMTP%_1s.l
    set LIBFTP=%LIBFTP%_1s.l
    CALL :doit

    set OPT=%OSIZE% %FULL%
    set LIB=%LIB%_1f.l
    set LIBHTTP=%LIBHTTP%_1f.l
    set LIBDNS=%LIBDNS%_1f.l
    set LIBDHCP=%LIBDHCP%_1f.l
    set LIBTELNET=%LIBTELNET%_1f.l
    set LIBTFTP=%LIBTFTP%_1f.l
    set LIBSMTP=%LIBSMTP%_1f.l
    set LIBFTP=%LIBFTP%_1f.l
    CALL :doit

    set OPT=%OSPEED% %NORMAL%
    set LIB=%LIB%_2.l
    set LIBHTTP=%LIBHTTP%_2.l
    set LIBDNS=%LIBDNS%_2.l
    set LIBDHCP=%LIBDHCP%_2.l
    set LIBTELNET=%LIBTELNET%_2.l
    set LIBTFTP=%LIBTFTP%_2.l
    set LIBSMTP=%LIBSMTP%_2.l
    set LIBFTP=%LIBFTP%_2.l
    CALL :doit

    set OPT=%OSPEED% %SMALL%
    set LIB=%LIB%_2s.l
    set LIBHTTP=%LIBHTTP%_2s.l
    set LIBDNS=%LIBDNS%_2s.l
    set LIBDHCP=%LIBDHCP%_2s.l
    set LIBTELNET=%LIBTELNET%_2s.l
    set LIBTFTP=%LIBTFTP%_2s.l
    set LIBSMTP=%LIBSMTP%_2s.l
    set LIBFTP=%LIBFTP%_2s.l
    CALL :doit

    set OPT=%OSPEED% %FULL%
    set LIB=%LIB%_2f.l
    set LIBHTTP=%LIBHTTP%_2f.l
    set LIBDNS=%LIBDNS%_2f.l
    set LIBDHCP=%LIBDHCP%_2f.l
    set LIBTELNET=%LIBTELNET%_2f.l
    set LIBTFTP=%LIBTFTP%_2f.l
    set LIBSMTP=%LIBSMTP%_2f.l
    set LIBFTP=%LIBFTP%_2f.l
    CALL :doit
)
goto :eof

:doit

%CC% -c %CFLAGS% %OPT% %INC% version_ips.c
%AR% %ARFLAGS% ..\lib\arm\arm\%LIB% version_ips.o
for /D %%i in (%IPS_MODULES%) DO (
    echo Building in %%i%
    pushd %%i%
    set L=
    for %%o in (*.c) DO (
      set L=!L! %%o%
    )
    echo Compiling: !L!
    %CC% -c %CFLAGS% %OPT% %INC% !L!
    %AR% -r ..\..\lib\arm\arm\%LIB% *.o
    popd
)

echo Building in http
%AR% %ARFLAGS% ..\lib\arm\arm\%LIBHTTP% version_ips.o
pushd http
set L=
for %%o in (*.c) DO (
  set L=!L! %%o%
)
echo Compiling: !L!
%CC% -c %CFLAGS% %OPT% %INC% !L!
%AR% -r ..\..\lib\arm\arm\rvct21\%LIBHTTP% *.o
popd

echo Building in dns
%AR% %ARFLAGS% ..\lib\arm\arm\rvct21\%LIBDNS% version_ips.o
pushd dns
set L=
for %%o in (*.c) DO (
  set L=!L! %%o%
)
echo Compiling: !L!
%CC% -c %CFLAGS% %OPT% %INC% !L!
%AR% -r ..\..\lib\arm\arm\rvct21\%LIBDNS% *.o
popd

echo Building in bootp
%AR% %ARFLAGS% ..\lib\arm\arm\rvct21\%LIBDHCP% version_ips.o
pushd bootp
set L=
for %%o in (*.c) DO (
  set L=!L! %%o%
)
echo Compiling: !L!
%CC% -c %CFLAGS% %OPT% %INC% !L!
%AR% -r ..\..\lib\arm\arm\rvct21\%LIBDHCP% *.o
popd

echo Building in telnet
%AR% %ARFLAGS% ..\lib\arm\arm\rvct21\%LIBTELNET% version_ips.o
pushd telnet
set L=
for %%o in (*.c) DO (
  set L=!L! %%o%
)
echo Compiling: !L!
%CC% -c %CFLAGS% %OPT% %INC% !L!
%AR% -r ..\..\lib\arm\arm\rvct21\%LIBTELNET% *.o
popd

echo Building in tftp
%AR% %ARFLAGS% ..\lib\arm\arm\rvct21\%LIBTFTP% version_ips.o
pushd tftp
set L=
for %%o in (*.c) DO (
  set L=!L! %%o%
)
echo Compiling: !L!
%CC% -c %CFLAGS% %OPT% %INC% !L!
%AR% -r ..\..\lib\arm\arm\rvct21\%LIBTFTP% *.o
popd

echo Building in smtp
%AR% %ARFLAGS% ..\lib\arm\arm\rvct21\%LIBSMTP% version_ips.o
pushd smtp
set L=
for %%o in (*.c) DO (
  set L=!L! %%o%
)
echo Compiling: !L!
%CC% -c %CFLAGS% %OPT% %INC% !L!
%AR% -r ..\..\lib\arm\arm\rvct21\%LIBSMTP% *.o
popd

echo Building in ftp
%AR% %ARFLAGS% ..\lib\arm\arm\rvct21\%LIBFTP% version_ips.o
pushd ftp
set L=
for %%o in (*.c) DO (
  set L=!L! %%o%
)
echo Compiling: !L!
%CC% -c %CFLAGS% %OPT% %INC% !L!
%AR% -r ..\..\lib\arm\arm\rvct21\%LIBFTP% *.o
popd

del version_ips.o

goto :eof

:error
popd
echo Error: %ERRORLEVEL%
:exit

