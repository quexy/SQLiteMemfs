@echo off

if "!" == "!%~1" goto :EOF

echo EXPORTS > "%~n1.def"
for /f "usebackq tokens=4" %%i in (`dumpbin "%~n1.dll" /EXPORTS ^| find " = "`) do echo %%~i>>"%~n1.def"

lib /NOLOGO /DEF:"%~n1.def" /OUT:"%~n1.lib" /MACHINE:X86
