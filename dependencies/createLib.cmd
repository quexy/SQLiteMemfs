@echo off
setlocal

cd /d "%~dp0"

if "!" == "!%~1" goto :EOF

set path=%path%;C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin

echo.   Copying %~1 System.Data.SQLite binaries
copy /y %~1\System.Data.SQLite.* >nul

echo.   Exporting symbols to System.Data.SQLite.def
(
    @echo EXPORTS
    @for /f "usebackq tokens=4" %%i in (`dumpbin /NOLOGO /EXPORTS %~1\System.Data.SQLite.dll ^| find "sqlite3"`) do @echo %%~i
)>System.Data.SQLite.def

lib /NOLOGO /DEF:System.Data.SQLite.def /OUT:System.Data.SQLite.lib /MACHINE:%~1
