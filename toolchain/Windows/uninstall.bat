@CD "%~dp0"\..\..\
@ECHO OFF
@CLS
IF EXIST "build" @SET /P ProjectType=<build\projtype.txt

IF EXIST "build" (
	@ECHO Removing existing project: %ProjectType%
	@DEL "%~dp0\run*.bat">nul 2>&1
	@DEL "Project Solution*.*">nul 2>&1
	@RMDIR build /S /Q
	@RMDIR bin /S /Q
) ELSE (
	@ECHO No project found
)