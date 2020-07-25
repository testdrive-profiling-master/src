rmdir /S /Q %1\.vs
rmdir /S /Q %1\Release
rmdir /S /Q %1\Debug
rmdir /S /Q %1\ReleaseDLL
rmdir /S /Q %1\DebugDLL
rmdir /S /Q %1\x64
rmdir /S /Q %1\ipch
del /Q %1\*.VC.db
del /Q /S /AH %1\*.suo
del /Q /S %1\*.user
del /Q /S %1\*.aps
del /Q /S %1\*.o
del /Q /S %1\*.d
