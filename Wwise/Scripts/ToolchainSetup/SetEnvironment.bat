@echo off
echo.
IF -%1-==-- (
    echo Error: Platform not specified.
    echo.
    set ERRORLEVEL=1
    goto printUsage
)

IF NOT EXIST %~dp0\%1\GetToolchainEnv.py (
    echo Error: Cannot find GetToolchainEnv script at %~dp0%1\GetToolchainEnv.py
    echo.
    set ERRORLEVEL=1
    goto printUsage
)

IF -%2-==-- (
    echo Error: ToolchainVer not specified
    echo.
    set ERRORLEVEL=1
    goto printUsage
)

REM Main script. Runs GetToolchainEnv.py, fetches comma-separated list of envvars to set, and sets each one, one-by-one
set _FULL_TOOLCHAIN_ENV=
ECHO Running "py -3 %~dp0%1\GetToolchainEnv.py %2"

FOR /F "delims=" %%i IN ('py -3 %~dp0%1\GetToolchainEnv.py %2') DO set _FULL_TOOLCHAIN_ENV=%%i

IF NOT DEFINED  _FULL_TOOLCHAIN_ENV (
    ECHO Error fetching toolchain environment. 
    exit /b
)

ECHO Fetched toolchain environment list:
ECHO     %_FULL_TOOLCHAIN_ENV%
ECHO.


:repeat 
FOR /F "tokens=1* delims=," %%i IN ("%_FULL_TOOLCHAIN_ENV%") DO (
    ECHO Setting environment variable: %%i
    set %%i
)
set _PRE_TOOLCHAIN_ENV=%_FULL_TOOLCHAIN_ENV%
set _FULL_TOOLCHAIN_ENV=%_FULL_TOOLCHAIN_ENV:*,=%
IF NOT "%_PRE_TOOLCHAIN_ENV:,=%"=="%_PRE_TOOLCHAIN_ENV%" GOTO :repeat

exit /b

:printUsage

echo Sets an environment up for a specific platform and toolchain version.
echo.
echo SetEnvironment.bat [Platform] [ToolchainVer]
echo.
echo Platform        Specifies which GetToolchainEnv.py script in this directory
echo should be executed.
echo.
echo ToolchainVer    ToolchainVer to use when running GetToolchainEnv script. This
echo                 should be equivalent to one of the lines from
echo                 .\{Platform}\ToolchainVers.txt
exit /b