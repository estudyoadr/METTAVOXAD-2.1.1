param([ValidateSet('x86','x64')][string]$Architecture)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
if ($env:GITHUB_ACTIONS -ne 'true') { throw 'CI only' }
$exe=Join-Path $PSScriptRoot "../Installer/Output/MettaVoxAD_3.0.3_Bundle_${Architecture}_Setup.exe"
$common=if($Architecture -eq 'x86'){[Environment]::GetFolderPath('CommonProgramFilesX86')}else{[Environment]::GetFolderPath('CommonProgramFiles')}
$vst2=Join-Path $common 'VST2/MettaVoxAD3'
$vst3=Join-Path $common 'VST3/MettaVoxAD3'
# Keep a 2.1.1 fixture to confirm the new bundle does not remove it.
$old=Join-Path $common 'VST2/mettavoxad/METTAVOXAD21.dll'
New-Item -ItemType Directory -Force (Split-Path $old) | Out-Null
Set-Content -LiteralPath $old -Value '2.1.1 preservation fixture'
# Simulate the previous bundle names to verify upgrade removes duplicates.
New-Item -ItemType Directory -Force $vst2,$vst3 | Out-Null
foreach($name in @('DeCon','SessionStrip','TransientDynamix','SonicMatrix','SpaceWeaver','Exciter808','MaximumCeiling')){
 Set-Content -LiteralPath (Join-Path $vst2 "MV3 $name.dll") -Value '3.0 obsolete-name fixture'
 New-Item -ItemType Directory -Force (Join-Path $vst3 "MV3 $name.vst3") | Out-Null
}
foreach($pass in @(1,2)){
 $log=Join-Path $env:RUNNER_TEMP "bundle_${Architecture}_install_$pass.log"
 $p=Start-Process -FilePath $exe -ArgumentList @('/VERYSILENT','/SUPPRESSMSGBOXES','/NORESTART',"/LOG=`"$log`"") -Wait -PassThru
 if($p.ExitCode -ne 0){throw "Installer exit $($p.ExitCode)"}
 $dlls=@(Get-ChildItem $vst2 -Filter '*.dll')
 if($dlls.Count -ne 7){throw 'Expected seven installed DLLs'}
 $bundles=@(Get-ChildItem $vst3 -Directory -Filter '*.vst3')
 if($bundles.Count -ne 7){throw 'Expected seven installed VST3 bundles'}
 foreach($dll in $dlls){$source=Join-Path $PSScriptRoot "../../build_bundle_$Architecture/Legacy/Release/$($dll.Name)";if((Get-FileHash $dll.FullName).Hash -ne (Get-FileHash $source).Hash){throw 'Installed DLL mismatch'}}
 if(!(Test-Path $old)){throw '2.1.1 removed'}
}
$p=Start-Process -FilePath (Join-Path $vst2 'unins000.exe') -ArgumentList @('/VERYSILENT','/SUPPRESSMSGBOXES','/NORESTART') -Wait -PassThru
if($p.ExitCode -ne 0 -or (Test-Path $vst3)){throw 'Uninstall failed'}
if(!(Test-Path $old)){throw '2.1.1 removed by uninstall'}
Write-Host 'PASS: install, reinstall, exact DLL hashes, seven VST3, uninstall, 2.1.1 preserved.'
