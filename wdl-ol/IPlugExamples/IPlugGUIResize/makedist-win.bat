echo off

REM - batch file to build VS2010 project and zip the resulting binaries (or make installer)
REM - updating version numbers requires python and python path added to %PATH% env variable 
REM - zipping requires 7zip in %ProgramFiles%\7-Zip\7z.exe
REM - building installer requires innotsetup in "%ProgramFiles(x86)%\Inno Setup 5\iscc"
REM - AAX codesigning requires ashelper tool added to %PATH% env variable and aax.key/.crt in .\..\..\..\Certificates\

echo Making IPlugGUIResize win distribution ...

echo ------------------------------------------------------------------
echo Updating version numbers ...

call python update_version.py

echo ------------------------------------------------------------------
echo Building ...

if exist "%ProgramFiles(x86)%" (goto 64-Bit) else (goto 32-Bit)

:32-Bit
echo 32-Bit O/S detected
call "%ProgramFiles%\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"
goto END

:64-Bit
echo 64-Bit Host O/S detected
call "%ProgramFiles(x86)%\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"
goto END
:END

REM - set preprocessor macros like this, for instance to enable demo build:
REM - SET CMDLINE_DEFINES="DEMO_VERSION"

REM - Could build individual targets like this:
REM - msbuild IPlugGUIResize-app.vcxproj /p:configuration=release /p:platform=win32

msbuild IPlugGUIResize.sln /p:configuration=release /p:platform=win32 /nologo /noconsolelogger /fileLogger /v:quiet /flp:logfile=build-win.log;errorsonly 
msbuild IPlugGUIResize.sln /p:configuration=release /p:platform=x64 /nologo /noconsolelogger /fileLogger /v:quiet /flp:logfile=build-win.log;errorsonly;append

echo ------------------------------------------------------------------
echo Code sign aax binary...
call ashelper -f .\build-win\aax\bin\IPlugGUIResize.aaxplugin\Contents\Win32\IPlugGUIResize.aaxplugin -l .\..\..\..\Certificates\aax.crt -k .\..\..\..\Certificates\aax.key -o .\build-win\aax\bin\IPlugGUIResize.aaxplugin\Contents\Win32\IPlugGUIResize.aaxplugin
REM - call ashelper -f .\build-win\aax\bin\IPlugGUIResize.aaxplugin\Contents\x64\IPlugGUIResize.aaxplugin -l .\..\..\..\Certificates\aax.crt -k .\..\..\..\Certificates\aax.key -o .\build-win\aax\bin\IPlugGUIResize.aaxplugin\Contents\x64\IPlugGUIResize.aaxplugin

REM - Make Installer (InnoSetup)

echo ------------------------------------------------------------------
echo Making Installer ...

if exist "%ProgramFiles(x86)%" (goto 64-Bit-is) else (goto 32-Bit-is)

:32-Bit-is
"%ProgramFiles%\Inno Setup 5\iscc" /cc ".\installer\IPlugGUIResize.iss"
goto END-is

:64-Bit-is
"%ProgramFiles(x86)%\Inno Setup 5\iscc" /cc ".\installer\IPlugGUIResize.iss"
goto END-is

:END-is

REM - ZIP
REM - "%ProgramFiles%\7-Zip\7z.exe" a .\installer\IPlugGUIResize-win-32bit.zip .\build-win\app\win32\bin\IPlugGUIResize.exe .\build-win\vst3\win32\bin\IPlugGUIResize.vst3 .\build-win\vst2\win32\bin\IPlugGUIResize.dll .\build-win\rtas\bin\IPlugGUIResize.dpm .\build-win\rtas\bin\IPlugGUIResize.dpm.rsr .\build-win\aax\bin\IPlugGUIResize.aaxplugin* .\installer\license.rtf .\installer\readmewin.rtf
REM - "%ProgramFiles%\7-Zip\7z.exe" a .\installer\IPlugGUIResize-win-64bit.zip .\build-win\app\x64\bin\IPlugGUIResize.exe .\build-win\vst3\x64\bin\IPlugGUIResize.vst3 .\build-win\vst2\x64\bin\IPlugGUIResize.dll .\installer\license.rtf .\installer\readmewin.rtf

echo ------------------------------------------------------------------
echo Printing log file to console...

type build-win.log

pause