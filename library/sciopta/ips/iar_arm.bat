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
REM * $Revision: 2.11 $                                                 **
REM * $Date: 2008/02/18 10:13:26 $                                     **
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

if "%IAR%" == "" (
  echo Must set enviroment variable %%IAR%% pointing to EWARM
  exit
)

echo Building IPS
set IPS_MODULES=device icmp igmp ips ipv4 link ppp tcp udp

REM ---------------------
REM ARM specific settings
REM ---------------------
set CC=%IAR%\bin\iccarm
set AR=%IAR%\..\common\bin\xlib

set INC=-I%SCIOPTA_HOME%\ips -I%SCIOPTA_HOME%\include -I%SCIOPTA_HOME%\include\sciopta\arm
set INC=%INC% -I%IAR%\inc

set CFLAGS= --cpu_mode thumb --interwork --diag_suppress Pa050,Pe111,Pa084 --silent
set CFLAGS= %CFLAGS%  -D__inline="" -Dstderr=9 -Dstdout=8 -Dfflush="" -Dfprintf=kprintf 

set EXT=r79
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
set O0= -z2
set OSIZE= -z9
set OSPEED= -s9
REM ----------------------
REM IPS configurations
REM ----------------------
set SMALL="-DIPS_CONF=<ips_config_small.h>"
set FULL="-DIPS_CONF=<ips_config_full.h>"
set NORMAL="-DIPS_CONF=<ips_config.h>"

REM Check if delivery
if "%1%" == "" (
    set OPT=%OSIZE% %NORMAL%
    set LIB=%LIB%.%EXT%
    set LIBHTTP=%LIBHTTP%.%EXT%
    set LIBDNS=%LIBDNS%.%EXT%
    set LIBDHCP=%LIBDHCP%.%EXT%
    set LIBTELNET=%LIBTELNET%.%EXT%
    set LIBTFTP=%LIBTFTP%.%EXT%
    set LIBSMTP=%LIBSMTP%.%EXT%
    set LIBFTP=%LIBFTP%.%EXT%
    call :doit
) ELSE (
    echo Building delivery libs

    set OPT=%O0% %NORMAL%
    set LIB=%LIB%_0.%EXT%
    set LIBHTTP=%LIBHTTP%_0.%EXT%
    set LIBDNS=%LIBDNS%_0.%EXT%
    set LIBDHCP=%LIBDHCP%_0.%EXT%
    set LIBTELNET=%LIBTELNET%_0.%EXT%
    set LIBTFTP=%LIBTFTP%_0.%EXT%
    set LIBSMTP=%LIBSMTP%_0.%EXT%
    set LIBFTP=%LIBFTP%_0.%EXT%
    CALL :doit

    set OPT=%O0% %SMALL%
    set LIB=%LIB%_0s.%EXT%
    set LIBHTTP=%LIBHTTP%_0s.%EXT%
    set LIBDNS=%LIBDNS%_0s.%EXT%
    set LIBDHCP=%LIBDHCP%_0s.%EXT%
    set LIBTELNET=%LIBTELNET%_0s.%EXT%
    set LIBTFTP=%LIBTFTP%_0s.%EXT%
    set LIBSMTP=%LIBSMTP%_0s.%EXT%
    set LIBFTP=%LIBFTP%_0s.%EXT%
    CALL :doit

    set OPT=%O0% %FULL%
    set LIB=%LIB%_0f.%EXT%
    set LIBHTTP=%LIBHTTP%_0f.%EXT%
    set LIBDNS=%LIBDNS%_0f.%EXT%
    set LIBDHCP=%LIBDHCP%_0f.%EXT%
    set LIBTELNET=%LIBTELNET%_0f.%EXT%
    set LIBTFTP=%LIBTFTP%_0f.%EXT%
    set LIBSMTP=%LIBSMTP%_0f.%EXT%
    set LIBFTP=%LIBFTP%_0f.%EXT%
    CALL :doit

    set OPT=%OSIZE% %NORMAL%
    set LIB=%LIB%_1.%EXT%
    set LIBHTTP=%LIBHTTP%_1.%EXT%
    set LIBDNS=%LIBDNS%_1.%EXT%
    set LIBDHCP=%LIBDHCP%_1.%EXT%
    set LIBTELNET=%LIBTELNET%_1.%EXT%
    set LIBTFTP=%LIBTFTP%_1.%EXT%
    set LIBSMTP=%LIBSMTP%_1.%EXT%
    set LIBFTP=%LIBFTP%_1.%EXT%
    CALL :doit

    set OPT=%OSIZE% %SMALL%
    set LIB=%LIB%_1s.%EXT%
    set LIBHTTP=%LIBHTTP%_1s.%EXT%
    set LIBDNS=%LIBDNS%_1s.%EXT%
    set LIBDHCP=%LIBDHCP%_1s.%EXT%
    set LIBTELNET=%LIBTELNET%_1s.%EXT%
    set LIBTFTP=%LIBTFTP%_1s.%EXT%
    set LIBSMTP=%LIBSMTP%_1s.%EXT%
    set LIBFTP=%LIBFTP%_1s.%EXT%
    CALL :doit

    set OPT=%OSIZE% %FULL%
    set LIB=%LIB%_1f.%EXT%
    set LIBHTTP=%LIBHTTP%_1f.%EXT%
    set LIBDNS=%LIBDNS%_1f.%EXT%
    set LIBDHCP=%LIBDHCP%_1f.%EXT%
    set LIBTELNET=%LIBTELNET%_1f.%EXT%
    set LIBTFTP=%LIBTFTP%_1f.%EXT%
    set LIBSMTP=%LIBSMTP%_1f.%EXT%
    set LIBFTP=%LIBFTP%_1f.%EXT%
    CALL :doit

    set OPT=%OSPEED% %NORMAL%
    set LIB=%LIB%_2.%EXT%
    set LIBHTTP=%LIBHTTP%_2.%EXT%
    set LIBDNS=%LIBDNS%_2.%EXT%
    set LIBDHCP=%LIBDHCP%_2.%EXT%
    set LIBTELNET=%LIBTELNET%_2.%EXT%
    set LIBTFTP=%LIBTFTP%_2.%EXT%
    set LIBSMTP=%LIBSMTP%_2.%EXT%
    set LIBFTP=%LIBFTP%_2.%EXT%
    CALL :doit

    set OPT=%OSPEED% %SMALL%
    set LIB=%LIB%_2s.%EXT%
    set LIBHTTP=%LIBHTTP%_2s.%EXT%
    set LIBDNS=%LIBDNS%_2s.%EXT%
    set LIBDHCP=%LIBDHCP%_2s.%EXT%
    set LIBTELNET=%LIBTELNET%_2s.%EXT%
    set LIBTFTP=%LIBTFTP%_2s.%EXT%
    set LIBSMTP=%LIBSMTP%_2s.%EXT%
    set LIBFTP=%LIBFTP%_2s.%EXT%
    CALL :doit

    set OPT=%OSPEED% %FULL%
    set LIB=%LIB%_2f.%EXT%
    set LIBHTTP=%LIBHTTP%_2f.%EXT%
    set LIBDNS=%LIBDNS%_2f.%EXT%
    set LIBDHCP=%LIBDHCP%_2f.%EXT%
    set LIBTELNET=%LIBTELNET%_2f.%EXT%
    set LIBTFTP=%LIBTFTP%_2f.%EXT%
    set LIBSMTP=%LIBSMTP%_2f.%EXT%
    set LIBFTP=%LIBFTP%_2f.%EXT%
    CALL :doit
)
goto :eof

:doit
%CC%  %CFLAGS% %OPT% %INC% version.c
%IAR%\..\common\bin\xar ..\lib\arm\iar\%LIB% version.%EXT%
for /D %%i in (%IPS_MODULES%) DO (
    echo Building in %%i%
    pushd %%i%
    for %%o in (*.c) do (
	echo %%o%
	set L= %%o%
	set L=!L:.c=.%EXT%!
	%CC% %CFLAGS% %OPT% %INC% %%o%
	%AR% -c "FETCH-MOD !L! ..\..\lib\arm\iar\%LIB%"
	del !L!
    )
    popd
)

%IAR%\..\common\bin\xar ..\lib\arm\iar\%LIBHTTP% version.%EXT%
echo Building in http
pushd http
for %%o in (*.c) do (
   echo %%o%
   set L= %%o%
   set L=!L:.c=.%EXT%!
   %CC% %CFLAGS% %OPT% %INC% %%o%
   %AR% -c "FETCH-MOD !L! ..\..\lib\arm\iar\%LIBHTTP%"
   del !L!
)
popd

%IAR%\..\common\bin\xar ..\lib\arm\iar\%LIBDNS% version.%EXT%
echo Building in dns
pushd dns
for %%o in (*.c) do (
   echo %%o%
   set L= %%o%
   set L=!L:.c=.%EXT%!
   %CC% %CFLAGS% %OPT% %INC% %%o%
   %AR% -c "FETCH-MOD !L! ..\..\lib\arm\iar\%LIBDNS%"
   del !L!
)
popd

%IAR%\..\common\bin\xar ..\lib\arm\iar\%LIBDHCP% version.%EXT%
echo Building in dhcp
pushd bootp
for %%o in (*.c) do (
   echo %%o%
   set L= %%o%
   set L=!L:.c=.%EXT%!
   %CC% %CFLAGS% %OPT% %INC% %%o%
   %AR% -c "FETCH-MOD !L! ..\..\lib\arm\iar\%LIBDHCP%"
   del !L!
)
popd

%IAR%\..\common\bin\xar ..\lib\arm\iar\%LIBTELNET% version.%EXT%
echo Building in telnet
pushd telnet
for %%o in (*.c) do (
   echo %%o%
   set L= %%o%
   set L=!L:.c=.%EXT%!
   %CC% %CFLAGS% %OPT% %INC% %%o%
   %AR% -c "FETCH-MOD !L! ..\..\lib\arm\iar\%LIBTELNET%"
   del !L!
)
popd

%IAR%\..\common\bin\xar ..\lib\arm\iar\%LIBTFTP% version.%EXT%
echo Building in tftp
pushd tftp
for %%o in (*.c) do (
   echo %%o%
   set L= %%o%
   set L=!L:.c=.%EXT%!
   %CC% %CFLAGS% %OPT% %INC% %%o%
   %AR% -c "FETCH-MOD !L! ..\..\lib\arm\iar\%LIBTFTP%"
   del !L!
)
popd

%IAR%\..\common\bin\xar ..\lib\arm\iar\%LIBSMTP% version.%EXT%
echo Building in smtp
pushd smtp
for %%o in (*.c) do (
   echo %%o%
   set L= %%o%
   set L=!L:.c=.%EXT%!
   %CC% %CFLAGS% %OPT% %INC% %%o%
   %AR% -c "FETCH-MOD !L! ..\..\lib\arm\iar\%LIBSMTP%"
   del !L!
)
popd

%IAR%\..\common\bin\xar ..\lib\arm\iar\%LIBFTP% version.%EXT%
echo Building in ftp
pushd ftp
for %%o in (*.c) do (
   echo %%o%
   set L= %%o%
   set L=!L:.c=.%EXT%!
   %CC% %CFLAGS% %OPT% %INC% %%o%
   %AR% -c "FETCH-MOD !L! ..\..\lib\arm\iar\%LIBFTP%"
   del !L!
)
popd

del version.%EXT%

goto :eof

:error
popd
echo Error: %ERRORLEVEL%
:exit
exit

