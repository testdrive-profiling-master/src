call clean.bat .
call clean.bat TestDrive
call clean.bat TestDrive\lua
call clean.bat TestDrive\locale
call clean.bat TestDrive\locale\Korean
call clean.bat TestDrive\locale\Japanese
call clean.bat TestDrive\locale\English
call clean.bat TestDrive\locale\Chinese_Simplified
call clean.bat TestDrive\locale\Chinese_Traditional
call clean.bat TestDrive\locale\Spanish
call clean.bat TestDrive\locale\Russian
call clean.bat TestDrive\locale\Arabic
call clean.bat TestDrive\locale\Dutch
call clean.bat TestDrive\TestDriver
call clean.bat TestDrive\UpVersion
call clean.bat util\CheckInception
call clean.bat util\GPASM
call clean.bat util\iStyle
call clean.bat util\HexEdit
call clean.bat util\ToolsAutoDownload
call clean.bat util\WatchDogRun
call clean.bat util\HierarchicalMake
call clean.bat util\InvokeEclipse
call clean.bat util\Verilator_helper

del /S /Q util\HexEdit\src\lib\*.lib
del /Q Program\locale\*.pdb
del /Q TestDrive\lib\*.lib
