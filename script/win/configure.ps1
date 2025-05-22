# DXUX - Copyright (C) 2025 NaokiS, spicyjpeg
# configure.ps1 - Created on 22-04-2025
# 
# DXUX is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
# 
# DXUX is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE. See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License along with
# DXUX. If not, see <https://www.gnu.org/licenses/>.

$ProjectRoot = "$PSScriptRoot\..\..\"
$ConfigRoot = "$ProjectRoot\.user"

$QEMUURL = "https://www.qemu.org/download/"

$UserSettings = @{
    QEMU = $null
}
$QEMUConfig = @{
    UseQEMU = $false
    Version = ""
    Path    = ""
}

Function Confirm-Action (
    [Parameter(Mandatory)]
    [String]$Prompt
) {
    . { 
        $Done = $false
        $Result = $false
        $Prompt = $Prompt.Replace('?', '')
        Write-Host "$Prompt`? (Y/N): " -NoNewline
        while (-not $Done) {
            $key = [System.Console]::ReadKey().KeyChar.ToString().ToUpper()
            switch ($key) {
                'Y' { 
                    $Done = $true
                    $Result = $true
                }
                'N' {
                    $Done = $true
                    $Result = $false
                }
                Default {
                    $Done = $false
                }
            }
        }
        Write-Host ""
    } | Out-Null
    return $Result
}

Function Write-ConfigFile {
    $UserSettings | ConvertTo-Json | Out-File -FilePath "$ConfigRoot\config.json"
    if ($?) {
        Write-Host "Config file written!" -ForegroundColor Green
    }
    else {
        Write-Host "Error writing config file!" -ForegroundColor Red
    }
}

function Get-QEMUPath {
    ${QEMUConfig.Path} = ""
    $Done = $false
    $path = @("$Env:ProgramFiles\qemu", "${Env:ProgramFiles(x86)}\qemu", "C:\qemu")
    $path | ForEach-Object {
        if (Test-Path -Path "$_\VERSION") {
            $ver = Get-Content -Path "$_\VERSION"
            Write-Host "Found QEMU $ver at $_"
            if (Confirm-Action -Prompt "Use this version?") {
                $QEMUConfig.Path = $_
                $QEMUConfig.Version = $ver
                $Done = $true
            }
        }
    }
        
    while (!$Done) {
        # Enter manually
        $path = Read-Host -Prompt "Please enter QEMU directory path"
        if ($null -eq $path) {
            # Assume user wants to skip
            Write-Host "Invalid QEMU path, QEMU will not be used."
            ${QEMUConfig.UseQEMU} = $false
            $Done = $true
        } 
                
        if (!(Test-Path -Path $path)) {
            Write-Host "Could not open $path!" -ForegroundColor Red
        }
        else {
            if (Test-Path -Path "$path\VERSION") {
                $ver = Get-Content -Path "$path\VERSION"
                Write-Host "Found QEMU $ver at $path"
                if (Confirm-Action -Prompt "Use this version?") {
                    $QEMUConfig.Path = $path
                    $QEMUConfig.Version = $ver
                    $Done = $true
                }
            }
            else {
                Write-Host "QEMU version file not found at $path"
                if (Confirm-Action -Prompt "Use this path anyway?") {
                    $QEMUConfig.Path = $path
                    $QEMUConfig.Version = "Unknown"
                    $Done = $true
                }
            }
        }
    }
}

Function Configure-QEMU() {
    Write-Host "= Configuring QEMU testing environment ="
    $QEMUConfig.UseQEMU = (Confirm-Action -Prompt "Use QEMU to test build output?")
    if ($QEMUConfig.UseQEMU -eq $true) {
        Get-QEMUPath
    }
    Write-Host "QEMU Config:"
    Write-Host "`tUse QEMU: ${QEMUConfig.UseQEMU}"
    Write-Host "`tQEMU Path: ${QEMUConfig.Path}"
    Write-Host ""
    if ((Confirm-Action -Prompt "Accept settings and save?")) {
        $UserSettings.QEMU = $QEMUConfig
        Write-ConfigFile
    }
    
}

if (!(Test-Path -Path "$ConfigRoot\config.json")) {
    Write-Host "Config file does not exist, creating new one now!"
    Configure-QEMU
}
else {
    $UserSettings = (Get-Content -Path "$ConfigRoot\config.json" | ConvertFrom-Json)
    $UserSettings | Format-Table
}