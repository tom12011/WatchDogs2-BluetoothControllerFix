[CmdletBinding()]
param(
    [string]$Archive = (Join-Path $PSScriptRoot '..\dist\v0.9.0-beta\WatchDogs2-BluetoothControllerFix-v0.9.0-beta.zip')
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

$expectedArchiveHash = '805DC6591B65483551CF4B4BFC13B48763F021054876AD560DDF42CC1CE8AF7D'
$expectedDllHash = 'F4112AFAB93E3A9200E2F442286A80EF76EA5BB0552CE9F0A68F2E004BED38CC'
$expectedEntries = @('README.txt', 'xinput9_1_0.dll')

$resolvedArchive = (Resolve-Path -LiteralPath $Archive).Path
$archiveHash = (Get-FileHash -LiteralPath $resolvedArchive -Algorithm SHA256).Hash
if ($archiveHash -ne $expectedArchiveHash) {
    throw "Release archive SHA-256 mismatch.`nExpected: $expectedArchiveHash`nActual:   $archiveHash"
}

Add-Type -AssemblyName System.IO.Compression.FileSystem
$zip = [IO.Compression.ZipFile]::OpenRead($resolvedArchive)
try {
    $entries = @($zip.Entries | Where-Object { $_.FullName -and -not $_.FullName.EndsWith('/') })
    $names = @($entries.FullName | Sort-Object)
    $expected = @($expectedEntries | Sort-Object)
    if ($names.Count -ne $expected.Count -or (Compare-Object $names $expected)) {
        throw "Unexpected archive contents: $($names -join ', ')"
    }

    $dllEntry = $entries | Where-Object FullName -eq 'xinput9_1_0.dll'
    $stream = $dllEntry.Open()
    try {
        $sha = [Security.Cryptography.SHA256]::Create()
        try {
            $dllHash = [BitConverter]::ToString($sha.ComputeHash($stream)).Replace('-', '')
        } finally {
            $sha.Dispose()
        }
    } finally {
        $stream.Dispose()
    }
    if ($dllHash -ne $expectedDllHash) {
        throw "Packaged DLL SHA-256 mismatch.`nExpected: $expectedDllHash`nActual:   $dllHash"
    }
} finally {
    $zip.Dispose()
}

Write-Host 'Release validation passed.'
Write-Host "Archive SHA-256: $archiveHash"
Write-Host "DLL SHA-256:     $expectedDllHash"
