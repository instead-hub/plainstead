@REM Директория сборки
@SET buldDir=D:\Projects\PlainInstead\build
@REM Директория выхода
@SET outDir=D:\temp\build
@REM Удаляем целиком каталог перед копированием
if exist %outDir% RD /S /Q %outDir%
MD %outDir%
XCOPY %buldDir% %outDir% /S /EXCLUDE:%buldDir%\exlude_build_list.txt
MD %outDir%\saves
MD %outDir%\games
COPY /Y %buldDir%\games\instead_games_approved.xml %outDir%\games\instead_games_approved.xml
XCOPY %buldDir%\games\cat %outDir%\games\cat /S /I
XCOPY %buldDir%\games\mirror %outDir%\games\mirror /S /I
XCOPY %buldDir%\games\tutorial3 %outDir%\games\tutorial3 /S /I