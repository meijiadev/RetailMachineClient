set PrePath=%CD%
echo The current directory is %~dp0
set CurrentPath=%~dp0
cd %CurrentPath%



xcopy /Y %CurrentPath%\include\*.h %CurrentPath%\..\DDRSDK\include\ /s /i
xcopy /Y %CurrentPath%..\..\..\..\..\..\\Shared\proto\DDRModuleCmd.pb.h %CurrentPath%\..\DDRSDK\include\Shared\proto /s /i





cd %PrePath%