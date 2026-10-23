$ErrorActionPreference = 'Stop'

$vs = & "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vs) { throw "No Visual Studio installation has the x64 C++ tools" }
$loaded = cmd /c "`"$vs\VC\Auxiliary\Build\vcvars64.bat`" >nul && set"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
$changed = foreach ($line in $loaded) {
    $name, $value = $line -split '=', 2
    if ($value -ne [Environment]::GetEnvironmentVariable($name)) { "$name=$value" }
}
if (-not ($changed -match '^VCToolsInstallDir=')) { throw "vcvars64.bat did not load the MSVC environment" }
$changed | Add-Content $env:GITHUB_ENV
