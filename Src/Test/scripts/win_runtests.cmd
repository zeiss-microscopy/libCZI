@echo off
IF EXIST "%ProgramFiles%\IronPython 2.7\ipy.exe" (
	SET PYTHONEXECUTABLE="%ProgramFiles%\IronPython 2.7\ipy.exe"
	goto found
) ELSE IF EXIST "%ProgramFiles%\Python 3.5\python.exe" (
	SET PYTHONEXECUTABLE="%ProgramFiles%\Python 3.5\python.exe"
	goto found
) ELSE IF EXIST "%ProgramFiles%\Python 3.5\python.exe" (
	SET PYTHONEXECUTABLE="%ProgramFiles%\Python 3.5\python.exe"
	goto found
) ELSE IF EXIST "%LOCALAPPDATA%\Programs\Python\Python35-32\python.exe" (
    SET PYTHONEXECUTABLE="%LOCALAPPDATA%\Programs\Python\Python35-32\python.exe"
	goto found
) ELSE IF EXIST "%ProgramFiles%\Microsoft Visual Studio\Shared\Python36_64\python.exe" (
	SET PYTHONEXECUTABLE="%ProgramFiles%\Microsoft Visual Studio\Shared\Python36_64\python.exe"
	goto found
)	


IF DEFINED ProgramW6432 (
	IF EXIST "%ProgramW6432%\IronPython 2.7\ipy.exe" (
		SET PYTHONEXECUTABLE="%ProgramW6432%\IronPython 2.7\ipy.exe"
		goto found
	) ELSE IF EXIST "%ProgramW6432%\Python 3.5\python.exe" (
		SET PYTHONEXECUTABLE="%ProgramW6432%\Python 3.5\python.exe"
		goto found
	) 
)

IF DEFINED ProgramFiles(x86) (
 	IF EXIST "%ProgramFiles(x86)%\IronPython 2.7\ipy.exe" (
		SET PYTHONEXECUTABLE="%ProgramFiles(x86)%\IronPython 2.7\ipy.exe"
		goto found
	) ELSE IF EXIST "%ProgramFiles(x86)%\Python 3.5\python.exe" (
		SET PYTHONEXECUTABLE="%ProgramFiles(x86)%\Python 3.5\python.exe"
		goto found
	) ELSE IF EXIST "%ProgramFiles(x86)%\Microsoft Visual Studio\Shared\Python36_64\python.exe" (
		SET PYTHONEXECUTABLE="%ProgramFiles(x86)%\Microsoft Visual Studio\Shared\Python36_64\python.exe"
		goto found
	)	
)

echo "Python interpreter not found!"
EXIT /B 1

:found

for /f "tokens=*" %%i in ('hostname') do set VAR=%%i
if /I "%VAR%" == "SESOSTRIS10" (
	SET SOURCEDIRECTORY=W:/Data/TestData/
	SET DESTINATIONDIRECTORY=W:/Data/Out/
) ELSE IF /I "%VAR%" == "SNOFRU" (
	SET SOURCEDIRECTORY=G:/Data/TestData/
	SET DESTINATIONDIRECTORY=G:/Data/Out/
) ELSE (
	SET SOURCEDIRECTORY=D:/PICTURES/NaCZIrTestData/
	SET DESTINATIONDIRECTORY=D:/TFSJBL/NaCZIr/Test/Out/
)

SET EXECUTABLE=../../Build/VS/Win32/Debug/CZIcmd.exe
REM SET EXECUTABLE=../../Build/VS/Win32/Release/CZIcmd.exe
REM SET EXECUTABLE=../../Build/VS/x64/Debug/CZIcmd.exe
REM SET EXECUTABLE=../../Build/VS/x64/Release/CZIcmd.exe


IF "%1"=="" (
%PYTHONEXECUTABLE% runtests.py -s "%SOURCEDIRECTORY%" -d "%DESTINATIONDIRECTORY%" -t ./testdata_win.txt -e %EXECUTABLE%
) ELSE (
%PYTHONEXECUTABLE% runtests.py -s "%SOURCEDIRECTORY%" -d "%DESTINATIONDIRECTORY%" -t ./testdata_win.txt -e %EXECUTABLE% --skip %1
)


