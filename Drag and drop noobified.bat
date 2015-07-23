@echo off
color F1

setlocal EnableDelayedExpansion
set /p Input=Do you wan't to use autolevels? [yes/no (default:yes)]:
for /f "skip=1" %%x in ('wmic os get localdatetime') do if not defined mydate set mydate=%%x
set TIMESTAMP=%mydate:~4,-4%

IF EXIST "%~dp0output\%TIMESTAMP%" (
echo ERROR, folder already exists, rerun program
exit
) ELSE (
md "%~dp0output\%TIMESTAMP%"
)

echo Processing...

IF "%Input%"=="no" ( 
"%~dp0\phoenix.exe" -f "%~1" --ela --lg --hsv --lab --avgdist -q -o "%~dp0output\%TIMESTAMP%" --copymove
echo Look at the quality and press space to quit and open file in Photome
pause >nul 
) ELSE (
"%~dp0\phoenix.exe" -f "%~1" --ela --lg --hsv --lab --avgdist -a -q -o "%~dp0output\%TIMESTAMP%" --copymove
echo Look at the quality and press space to quit and open file in Photome
pause >nul
)

COPY "%~1 " "%~dp0output\%TIMESTAMP%"
cls

echo "Done!
explorer.exe "%~dp0output\%TIMESTAMP%"
REM start "" "C:\Program Files (x86)\PhotoMEBeta\photome.exe" "%~1"

exit



