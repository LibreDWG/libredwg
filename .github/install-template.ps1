#Requires -Version 5.1
<#
.SYNOPSIS
    LibreDWG Installation Script for Windows
.DESCRIPTION
    Platform: PLATFORM_PLACEHOLDER
    Version: VERSION_PLACEHOLDER
.PARAMETER Prefix
    Installation directory (default: $env:LOCALAPPDATA\libredwg)
.PARAMETER System
    Install to Program Files (requires admin)
.EXAMPLE
    .\install-windows-x64.ps1
.EXAMPLE
    .\install-windows-x64.ps1 -Prefix "C:\tools\libredwg"
.EXAMPLE
    .\install-windows-x64.ps1 -System
#>
param(
    [string]$Prefix = "$env:LOCALAPPDATA\libredwg",
    [switch]$System,
    [switch]$Help
)

$ErrorActionPreference = "Stop"

$Version = "VERSION_PLACEHOLDER"
$Platform = "PLATFORM_PLACEHOLDER"
$Repo = "rite-build/libredwg"

if ($Help) {
    Get-Help $MyInvocation.MyCommand.Path -Detailed
    exit 0
}

if ($System) {
    $Prefix = "$env:ProgramFiles\libredwg"
}

Write-Host "==> LibreDWG Installer" -ForegroundColor Cyan
Write-Host "    Version:  $Version"
Write-Host "    Platform: $Platform"
Write-Host "    Prefix:   $Prefix"
Write-Host ""

# Check if we need admin for the prefix
$NeedsAdmin = $false
if ($Prefix.StartsWith($env:ProgramFiles) -or $Prefix.StartsWith($env:ProgramW6432)) {
    $currentPrincipal = New-Object Security.Principal.WindowsPrincipal([Security.Principal.WindowsIdentity]::GetCurrent())
    if (-not $currentPrincipal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
        $NeedsAdmin = $true
    }
}

if ($NeedsAdmin) {
    Write-Host "WARNING: Installation to $Prefix requires administrator privileges." -ForegroundColor Yellow
    Write-Host "Please run this script as Administrator or choose a different prefix." -ForegroundColor Yellow
    exit 1
}

# Create temporary directory
$TmpDir = Join-Path $env:TEMP "libredwg-install-$(Get-Random)"
New-Item -ItemType Directory -Path $TmpDir -Force | Out-Null

try {
    Set-Location $TmpDir

    # Download files
    $ZipFile = "libredwg-$Version-$Platform.zip"
    $Checksum = "libredwg-$Version-$Platform.zip.sha256"
    $BaseUrl = "https://github.com/$Repo/releases/download/$Version"

    Write-Host "==> Downloading $ZipFile..." -ForegroundColor Cyan
    
    $ProgressPreference = 'SilentlyContinue'
    Invoke-WebRequest -Uri "$BaseUrl/$ZipFile" -OutFile $ZipFile -UseBasicParsing
    Invoke-WebRequest -Uri "$BaseUrl/$Checksum" -OutFile $Checksum -UseBasicParsing
    $ProgressPreference = 'Continue'

    # Verify checksum
    Write-Host "==> Verifying checksum..." -ForegroundColor Cyan
    $ExpectedHash = (Get-Content $Checksum -Raw).Trim().Split()[0]
    $ActualHash = (Get-FileHash -Algorithm SHA256 $ZipFile).Hash.ToLower()
    
    if ($ExpectedHash -ne $ActualHash) {
        Write-Host "ERROR: Checksum verification failed!" -ForegroundColor Red
        Write-Host "Expected: $ExpectedHash" -ForegroundColor Red
        Write-Host "Actual:   $ActualHash" -ForegroundColor Red
        exit 1
    }
    Write-Host "    Checksum OK" -ForegroundColor Green

    # Extract zip
    Write-Host "==> Extracting files..." -ForegroundColor Cyan
    Expand-Archive -Path $ZipFile -DestinationPath . -Force

    # Install
    Write-Host "==> Installing to $Prefix..." -ForegroundColor Cyan
    
    if (-not (Test-Path $Prefix)) {
        New-Item -ItemType Directory -Path $Prefix -Force | Out-Null
    }

    $SourceDir = "libredwg-$Version-$Platform\usr\local"
    Copy-Item -Path "$SourceDir\*" -Destination $Prefix -Recurse -Force

    Write-Host ""
    Write-Host "LibreDWG $Version installed successfully!" -ForegroundColor Green
    Write-Host ""

    # Check if binaries are in PATH
    $BinPath = Join-Path $Prefix "bin"
    $CurrentPath = [Environment]::GetEnvironmentVariable("PATH", "User")
    
    if ($CurrentPath -notlike "*$BinPath*") {
        Write-Host "WARNING: Add $BinPath to your PATH:" -ForegroundColor Yellow
        Write-Host ""
        Write-Host "    # PowerShell (current session):" -ForegroundColor Gray
        Write-Host "    `$env:PATH = `"$BinPath;`$env:PATH`"" -ForegroundColor White
        Write-Host ""
        Write-Host "    # PowerShell (permanent for current user):" -ForegroundColor Gray
        Write-Host "    [Environment]::SetEnvironmentVariable('PATH', `"$BinPath;`$([Environment]::GetEnvironmentVariable('PATH', 'User'))`", 'User')" -ForegroundColor White
        Write-Host ""
        
        $AddToPath = Read-Host "Would you like to add it to PATH now? (Y/n)"
        if ($AddToPath -ne "n" -and $AddToPath -ne "N") {
            $NewPath = "$BinPath;$CurrentPath"
            [Environment]::SetEnvironmentVariable("PATH", $NewPath, "User")
            $env:PATH = "$BinPath;$env:PATH"
            Write-Host "PATH updated successfully!" -ForegroundColor Green
            Write-Host "Note: You may need to restart your terminal for changes to take effect." -ForegroundColor Yellow
        }
    } else {
        Write-Host "LibreDWG binaries are available in your PATH." -ForegroundColor Green
    }

    Write-Host ""
    Write-Host "Available commands:" -ForegroundColor Cyan
    Write-Host "  - dwg2dxf, dxf2dwg, dwg2SVG"
    Write-Host "  - dwgread, dwgwrite, dwgadd"
    Write-Host "  - dwggrep, dwglayers, dwgrewrite"
    Write-Host ""

} finally {
    # Cleanup
    Set-Location $env:USERPROFILE
    Remove-Item -Path $TmpDir -Recurse -Force -ErrorAction SilentlyContinue
}

