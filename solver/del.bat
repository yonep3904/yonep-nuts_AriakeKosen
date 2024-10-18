@echo off

if exist %1.exe (
    echo del /Q "%1.exe"
    del /Q "%1.exe"
)

if exist %2 (
    echo del /Q "%2\*.o"
    del /Q "%2\*.o"
)