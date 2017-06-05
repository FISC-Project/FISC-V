@CD "%~dp0"\..\..\
@ECHO OFF
@CLS

@REM ####################################################
SET IDE_Name=CodeBlocks - MinGW Makefiles
SET Project_Filename=FISC-VM.cbp
SET Shortcut_Suffix=.cbp
SET Shortcut_Suffix_Bat=run-makefile.bat
SET Shortcut_Run_Suffix=run.bat
SET Shortcut_Clean_Suffix=run-clean.bat
SET Exec_Filename=FISC-VM
@REM ####################################################

SET TOOLS="%~dp0\Tools"

@ECHO Targeting IDE: %IDE_Name%
@ECHO.

IF EXIST "build" @SET /P ProjectType=<build\projtype.txt

IF EXIST "build" (
	IF NOT "%ProjectType%"=="%IDE_Name%" (
		@ECHO -- Removing already existing project: %ProjectType%
		@DEL "Project Solution*.*">nul 2>&1
		@RMDIR build /S /Q
		@RMDIR bin /S /Q
		@MKDIR build
		@MKDIR bin
		@MKLINK "Project Solution%Shortcut_Suffix%" build\%Project_Filename%>nul
	)
) ELSE (
	@MKDIR build
	@MKDIR bin
	@MKLINK "Project Solution%Shortcut_Suffix%" build\%Project_Filename%>nul
)

@DEL "%~dp0\run*.bat">nul 2>&1
@%TOOLS%\printf "@CD ""%%%%~dp0""\\..\\..\\\\\n@ECHO OFF\n@CLS\ncmake --build build\nIF ERRORLEVEL 2 SET ERRORLEVEL=2\n@CD ""%%%%~dp0""\\..\\..\\\\" > "%~dp0"\%Shortcut_Suffix_Bat%
@%TOOLS%\printf "@CD ""%%%%~dp0""\\..\\..\\\\\n@ECHO OFF\n@CLS\n@CALL ""%%%%~dp0""\\\regenerate-project-files.bat >nul 2>&1\n@CALL ""%%%%~dp0""\\\%Shortcut_Suffix_Bat%\nIF %%%%ERRORLEVEL%%%% NEQ 0 GOTO END\n@CD bin\n@echo ---------------------------\n.\%Exec_Filename%\n@echo.\n@echo ---------------------------\n@echo Finished program execution.\n:END\n@CD ""%%%%~dp0""\\..\\..\\\\\n@PAUSE" > "%~dp0"\%Shortcut_Run_Suffix%
@%TOOLS%\printf "@CD ""%%%%~dp0""\\..\\..\\\\\n@ECHO OFF\n@CLS\n@CD build\nmingw32-make clean\n@CD ""%%%%~dp0""\\..\\..\\\\" > "%~dp0"\%Shortcut_Clean_Suffix%

@CD build

@SET PATH=%PATH:C:\MinGW\msys\1.0\bin;=%
cmake -G "%IDE_Name%" ..
@SET PATH=%PATH%;C:\MinGW\msys\1.0\bin

@ECHO %IDE_Name%> projtype.txt

@CD "%~dp0"\..\..\
@ECHO.
@ECHO Project successfully created.
