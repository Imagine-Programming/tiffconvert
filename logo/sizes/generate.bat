@echo off
setlocal 
cd %~dp0

REM enter the full path to ImageMagick convert.exe (or command) here
SET convert=convert 
SET source="..\tiffconvert.svg"
SET iconfile="tiffconvert.ico"
SET background="rgba(0,0,0,0)"
SET align=center
SET sizes=512 256 128 64 32 16
SET iconsizes=256 128 64 32 16
SET infiles=

REM convert all the individual sizes first 
for %%a in (%sizes%) do call :convert_one %%a

REM generate a list of files for the iconfile 
for %%a in (%iconsizes%) do call :generate_icon_list %%a

REM convert all sizes to ico 
echo Generating ICO %iconfile%
call :convert_all_ico

REM we're done, exit batch script 
goto end

REM convert the source SVG to a single PNG with specific dimensions
:convert_one
	SET size=%1x%1
	SET filename="tiffconvert.%size%.png"
	echo Generating image for %size% dimensions...
	%convert% -background %background% %source% -resize %size% -gravity %align% -extent %size% %filename%
	goto :eof
	
REM generate a list of files that can be converted to an icon
:generate_icon_list
	SET infiles=%infiles% "tiffconvert.%1x%1.png"
	goto :eof 
	
REM convert the image list to an icon
:convert_all_ico
	%convert% %infiles% %iconfile%
	
:end 
endlocal