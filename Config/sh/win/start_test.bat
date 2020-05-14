@echo off
pause

set GridPath=%~dp0..\
cd /d %GridPath%
del /f /q .\log\*.log
pause
start "agent --name=test" "%GridPath%GridSeed.exe" --name=test --type=1 --id=1 --port=8101
