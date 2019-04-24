set PrePath=%CD%
echo The current directory is %~dp0
set CurrentPath=%~dp0
cd %CurrentPath%



xcopy /Y %CurrentPath%\Interface\*.h %CurrentPath%\..\DDRSDK\include\Interface /s /i





cd %PrePath%