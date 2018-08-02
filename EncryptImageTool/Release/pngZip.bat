@echo off

set path=%1
set minQ=%2
set maxQ=%3
set filePath=%4
set outPath=%5

:start

REM echo %path%
REM echo %minQ%
REM echo %maxQ%
REM echo %filePath%
REM echo %outPath%
REM "%path%pngquant.exe" --force --verbose --quality=10-20 %1 --output ./outfiles/%1
echo out --> %outPath%
%path% --force --verbose --ordered --speed=1 --quality=%minQ%-%maxQ% %filePath% --output %outPath%

REM shift
REM if NOT x%1==x goto start

pause
