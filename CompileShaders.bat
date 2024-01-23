@echo off
setlocal enabledelayedexpansion

rem Set the directory path where the files are located
set "directoryPath=C:\Projekty\Vulkan\VGraphics2\src\shaders"

rem Set the CMD command to run on each file
set "cmdCommand=C:\VulkanSDK\1.3.261.1\Bin\glslc.exe "

rem Change to the specified directory
cd /d "%directoryPath%"

rem Iterate over files with .frag or .vert extension
for %%F in (*.frag) do (
    set "filename=%%~nF"
    echo Processing file: %%F
    rem Run the CMD command on each file
    %cmdCommand% "%%F" -o !filename!Frag.spv
)
for %%F in (*.vert) do (
    set "filename=%%~nF"
    echo Processing file: %%F
    rem Run the CMD command on each file
    %cmdCommand% "%%F" -o !filename!Vert.spv
)

echo All files processed.
endlocal