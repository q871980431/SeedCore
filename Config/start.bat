@echo off
pause

cd /d %~dp0
start "test --name=test --agent=11401" "%~dp0NetGrid.exe" --name=test --type=1 --id=1 