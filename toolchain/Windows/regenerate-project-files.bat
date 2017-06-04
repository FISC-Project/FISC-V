@CD "%~dp0"\..\..\build
@ECHO OFF
@CLS

@SET PATH=%PATH:C:\MinGW\msys\1.0\bin;=%
cmake ..
@SET PATH=%PATH%;C:\MinGW\msys\1.0\bin

@CD "%~dp0"\..\..\
@ECHO.
@ECHO Regeneration successful.