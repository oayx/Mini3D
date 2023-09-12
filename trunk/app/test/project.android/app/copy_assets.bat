@ECHO OFF
echo begin copy assets

start CopyAsset.exe src\main\assets ..\..\..\..\bin\assets ..\..\..\..\template\data\assets
echo begin copy dlls
if not exist "libs\arm64-v8a" mkdir "libs\arm64-v8a"
if not exist "libs\armeabi-v7a" mkdir "libs\armeabi-v7a"
xcopy /Y /Q /E "..\..\..\..\bin\android\libs\*.*" "libs"
if exist "..\..\..\..\bin\android\arm64-v8a" xcopy /Y /Q /E "..\..\..\..\bin\android\arm64-v8a\*.*" "libs\arm64-v8a"
if exist "..\..\..\..\bin\android\armeabi-v7a" xcopy /Y /Q /E "..\..\..\..\bin\android\armeabi-v7a\*.*" "libs\armeabi-v7a"