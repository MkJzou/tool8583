@echo off
title qmake and nmake build prompt
set VCINSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC
set QTDIR=E:\soft\Qt5.11.0\5.11.0\msvc2015
set PATH=%VCINSTALLDIR%\bin;%QTDIR%\bin;%PATH%

set project=tool8583.pro
set dir=build-Desktop_Qt_5_11_0_MSVC2015_32bit-Debug
if not exist %dir% mkdir %dir%
cd %dir%

call "%VCINSTALLDIR%\vcvarsall.bat" x86
qmake ..\%project% -spec win32-msvc "CONFIG+=debug" "CONFIG+=qml_debug"
nmake
cd debug
::windeployqt tips.exe
::tips.exe