@echo off
set DEVICE_PATH=/sdcard/libProject.so
adb push "build\libs\arm64-v8a\libProject.so" %DEVICE_PATH%
@REM adb shell chmod 755 %DEVICE_PATH%
