$ErrorActionPreference = "Stop"

$ucrt = "C:\msys64\ucrt64\bin"
$env:Path = "$ucrt;$env:Path"

./scripts/compile.ps1
if ($LastExitCode -ne 0) { exit $LastExitCode }

Write-Host -ForegroundColor Green "====== Running the application... ======"
./build/CppTemplate.exe
