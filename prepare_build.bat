@REM Директория сборки
@SET buldDir=D:\Projects\PlainInstead\build
@REM Директория выхода
@SET outDir=D:\temp\build
@REM Удаляем челиком каталог перед копированием
if exist %outDir% RD /S /Q %outDir%
MD %outDir%
XCOPY %buldDir% %outDir% /S /EXCLUDE:%buldDir%\exlude_build_list.txt