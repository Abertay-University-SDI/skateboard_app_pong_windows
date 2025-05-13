@echo off

REM Define variables
set "solutionPath=Skateboard.sln"
set "filePath=gameapp/src/TutorialScene.cpp"
set "solutionDir=./"

REM Change to solution directory
cd /D "%solutionDir%"

REM Check file existence
if not exist "%solutionPath%" (
    echo Error: Solution not found: "%solutionPath%"
    exit /b 1
)
if not exist "%filePath%" (
    echo Error: File not found: "%filePath%"
    exit /b 1
)

REM Reset Git
echo Resetting Git...
git reset --hard
if %errorlevel% neq 0 (
    echo Error: Git reset failed.
    exit /b %errorlevel%
)

REM Open VS solution *and* the file in a single command
echo Opening Visual Studio...
start "" /b devenv "%solutionPath%" "%filePath%"

echo Done.
exit /b 0