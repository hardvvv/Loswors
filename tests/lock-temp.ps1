# Simulates another program (e.g. an antivirus scan) holding the Loswors temp file open.
# The temp file "<name>.lwrsTEMP" is opened with Read/Write sharing but WITHOUT Delete sharing:
# Loswors can still write into it, but cannot rename it afterwards.
#
# Keep this window open while saving in Loswors, then press Enter to release the file.

param([Parameter(Mandatory = $true)][string]$LwrsPath)

$ErrorActionPreference = 'Stop'

$full = [System.IO.Path]::GetFullPath($LwrsPath)
if (-not (Test-Path -LiteralPath $full)) { Write-Error "File not found: $full" }

# Loswors builds the temp path by replacing the extension: file.lwrs -> file.lwrsTEMP
$temp = [System.IO.Path]::ChangeExtension($full, 'lwrsTEMP')

$fs = [System.IO.File]::Open(
    $temp,
    [System.IO.FileMode]::OpenOrCreate,
    [System.IO.FileAccess]::ReadWrite,
    [System.IO.FileShare]::ReadWrite)   # no FileShare.Delete -> rename/delete of this file fails

try {
    "Holding: $temp"
    'Now press Save in Loswors and enter your PIN. Then press Enter here to release the file.'
    [void][Console]::ReadLine()
}
finally {
    $fs.Dispose()
    'Released.'
}
