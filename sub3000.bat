@echo off

SET SCRIPT_DIR=%~dp0

cd "%SCRIPT_DIR:~0,-1%\share\sub3000"
start "" "%SCRIPT_DIR:~0,-1%\bin\sub3000"

