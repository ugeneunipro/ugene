<#
.SYNOPSIS
Use Qt's bundled Mesa renderer for a Windows UGENE build on a VM without OpenGL acceleration.
.DESCRIPTION
The 3D viewer uses native OpenGL and GLU calls. QT_OPENGL=software alone sends Qt
and these native calls to different DLLs. An application-local opengl32.dll makes
both use the same implementation, including GLU's internal OpenGL calls.
This is an opt-in workaround for a single build directory, not a packaging default.
It forces software rendering for this build and does not change system DLLs or drivers.
Close UGENE before enabling or disabling it. -Disable removes only an identical
copy of the bundled opengl32sw.dll; an unrelated opengl32.dll is never overwritten.
.EXAMPLE
./set_software_opengl.ps1 -DistPath C:/work/ugene/build/dist
.EXAMPLE
./set_software_opengl.ps1 -DistPath C:/work/ugene/build/dist -Disable
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$DistPath,
    [switch]$Disable
)

$ErrorActionPreference = 'Stop'
$dist = (Resolve-Path -LiteralPath $DistPath).Path
if (!(Test-Path -LiteralPath (Join-Path $dist 'ugeneui.exe')) -and
    !(Test-Path -LiteralPath (Join-Path $dist 'ugeneuid.exe'))) {
    throw 'DistPath must be a UGENE distribution directory.'
}
$running = Get-Process ugeneui, ugeneuid, ugenecl, ugenecld -ErrorAction SilentlyContinue |
    Where-Object { $_.Path -and (Split-Path $_.Path) -eq $dist }
if ($running) {
    throw 'Close UGENE processes using this distribution before changing the renderer.'
}

$source = Join-Path $dist 'opengl32sw.dll'
$target = Join-Path $dist 'opengl32.dll'
if (!(Test-Path -LiteralPath $source -PathType Leaf)) {
    throw 'The Qt software renderer opengl32sw.dll is missing from this distribution.'
}
if (Test-Path -LiteralPath $target) {
    if ((Get-FileHash -LiteralPath $source).Hash -ne (Get-FileHash -LiteralPath $target).Hash) {
        throw 'An unrelated or older opengl32.dll already exists; refusing to replace or remove it.'
    }
    if ($Disable) {
        Remove-Item -LiteralPath $target
    }
} elseif (!$Disable) {
    Copy-Item -LiteralPath $source -Destination $target
    if ((Get-FileHash -LiteralPath $source).Hash -ne (Get-FileHash -LiteralPath $target).Hash) {
        throw 'Software renderer copy verification failed.'
    }
}
if ($Disable) {
    Write-Output "Restored automatic OpenGL selection in $dist"
} else {
    Write-Output "Enabled application-local software OpenGL in $dist"
}
