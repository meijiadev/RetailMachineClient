set PrePath=%CD%
echo The current directory is %~dp0
set CurrentPath=%~dp0
cd %CurrentPath%



xcopy /Y %CurrentPath%\include\*.h %CurrentPath%\..\DDRSDK\include\ /s /i





cd %PrePath%