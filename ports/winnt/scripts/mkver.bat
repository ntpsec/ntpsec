@ECHO OFF

GOTO PROG
######################################################################
#
# Revision: mkver.bat
# Author:   Frederick Czajka
# Date:     02/10/2000
# Purpose:  Provide a NT Shell script to replace the perl script 
#           that replaced the UNIX mkver shell script.
#           
# 
#
# Notes:  I had two goals with this script one to only use native
#         NT Shell commands and two was too emulate the PERL style
#         output. This required some work for the DATE format as 
#         you will see and TIME was really tricky to get a format 
#         matching PERLs!
#
######################################################################
:PROG

IF {%1} == {} GOTO USAGE
IF {%1} == {-H} GOTO USAGE
IF {%2} == {} GOTO USAGE
IF {%1} == {-P} GOTO BEGIN

REM For any other bizarre permutation...
GOTO USAGE

:BEGIN

SET GENERATED_PROGRAM=%2

REM Reimplemented from orginal Unix Shell script
IF NOT EXIST .version ECHO 0 > .version
FOR /F %%i IN (.version) do @SET RUN=%%i
SET /A RUN=%RUN%+1
ECHO %RUN% > .version


FOR /F "TOKENS=2 DELIMS== " %%a IN ('findstr /b /l VERSION= ..\..\..\configure') DO @SET VER=%%a
FOR /F "TOKENS=5-8 DELIMS=:. " %%a IN ('echo.^|time') DO SET HH=%%a&SET MM=%%b&SET SS=%%c&SET HS=%%d
FOR /F "TOKENS=1-4 DELIMS=/ " %%a IN ('date/t') DO SET DAY=%%a&SET nmm=%%b&SET dd=%%c&SET yyyy=%%d

IF %NMM% ==01 SET MONTH=Jan
IF %NMM% ==02 SET MONTH=Feb
IF %NMM% ==03 SET MONTH=Mar
IF %NMM% ==04 SET MONTH=Apr
IF %NMM% ==05 SET MONTH=May
IF %NMM% ==06 SET MONTH=Jun
IF %NMM% ==07 SET MONTH=Jul
IF %NMM% ==08 SET MONTH=Aug
IF %NMM% ==09 SET MONTH=Sep
IF %NMM% ==10 SET MONTH=Oct
IF %NMM% ==11 SET MONTH=Nov
IF %NMM% ==12 SET MONTH=Dec

ECHO char * Version = "%GENERATED_PROGRAM% %VER% %DAY% %MONTH% %DD% %HH%:%MM%:%SS% %YYYY% (%RUN%)" ; > version.c

GOTO EOF
:USAGE

   ECHO Usage: mkver.bat [ -P <Program Name> -H ]
   ECHO   -P          Database Name
   ECHO   -H          Help on options

:EOF