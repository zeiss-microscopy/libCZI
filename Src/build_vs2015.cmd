
IF defined VS140COMNTOOLS (
	call "%VS140COMNTOOLS%\VsDevCmd.bat"
) else (
	call "%ProgramFiles(x86)%\Microsoft Visual Studio 14.0\Common7\Tools\VsDevCmd.bat"
)

msbuild.exe /p:Configuration=Release          /p:Platform="x86" /verbosity:minimal /m LibCZI-Solution.sln
msbuild.exe /p:Configuration=Release          /p:Platform="x64" /verbosity:minimal /m LibCZI-Solution.sln
msbuild.exe /p:Configuration="static Release" /p:Platform="x86" /verbosity:minimal /m LibCZI-Solution.sln
msbuild.exe /p:Configuration="static Release" /p:Platform="x64" /verbosity:minimal /m LibCZI-Solution.sln
