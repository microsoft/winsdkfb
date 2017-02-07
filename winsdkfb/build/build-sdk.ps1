
# If you move this script, change this environment variable to point to the root
# of the winsdkfb directory. Stuff will break if you don't!
Set-Item -force -path "env:WinsdkfbRoot" -value "$PSScriptRoot\..\.."

#.SYNOPSIS
# Grabs all environment variable set after vcvarsall.bat is called and pulls
# them into the Powershell environment.
function Set-MsbuildDevEnvironment()
{
    $path = "$env:VS140COMNTOOLS\..\.."
    pushd $path
    cmd /c "vcvarsall.bat&set" | foreach {
        if ($_ -match "=")
        {
            $s = $_.Split("=");
            Set-Item -force -path "env:\$($s[0])" -value "$($s[1])"
        }
    }
    popd
    Write-Host "Dev environment variables set" -ForegroundColor Green
}

#.SYNOPSIS
# Downloads the latest version of nuget.exe.
#
#.PARAMETER $DownloadTargetDir
# The directory to place nuget.exe in.
function Get-Nuget()
{
    [CmdletBinding()]
    Param (
        [parameter(Mandatory=$true)]
        [string]$DownloadTargetDir
    )

    $NugetDownloadUrl = 'https://dist.nuget.org/win-x86-commandline/latest/nuget.exe'
    $Downloader = New-Object System.Net.WebClient
    $Downloader.DownloadFile($NugetDownloadUrl, "$DownloadTargetDir\nuget.exe")
}

#.SYNOPSIS
# Ensures that the build tools required can be found on the system.
function Test-BuildToolsPresent()
{
    Set-MsbuildDevEnvironment
    if ((Get-Command "msbuild.exe" -ErrorAction SilentlyContinue) -eq $null)
    {
        Throw "msbuild.exe is not present in path"
    }
    if ((Get-Command "nuget.exe" -ErrorAction SilentlyContinue) -eq $null)
    {
        $NugetTargetDir = "$env:WinsdkfbRoot\winsdkfb\build\nuget"
        if (-not (Test-Path "$NugetTargetDir\nuget.exe"))
        {
            Write-Host "downloading latest nuget.exe..."
            Get-Nuget -DownloadTargetDir $NugetTargetDir
        }
        $env:Path += ";$NugetTargetDir;"
    }
    if ((Get-Command "TextTransform.exe" -ErrorAction SilentlyContinue) -eq $null)
    {
        $TextTemplateDir = "${env:CommonProgramFiles(x86)}\Microsoft Shared\TextTemplating\14.0"
        if (Test-Path "$TextTemplateDir\TextTransform.exe")
        {
            $env:Path += ";$TextTemplateDir;"
        }
        else
        {
            Throw "TextTransform.exe cannot be found."
        }
    }
}

#.SYNOPSIS
# Builds a winsdkfb solution.
#
#.PARAMETER $SolutionName
# The path to the solution to build.
#
#.PARAMETER $Platform
# The platform to target. May be Win32, x64, ARM.
#
#.PARAMETER $Configuration
# The configuration to target. May be Debug, Release.
function Invoke-OpenConsoleBuild()
{
    [CmdletBinding()]
    Param (
        [parameter(Mandatory=$true)]
        [string]$SolutionName,

        [parameter(Mandatory=$true)]
        [string]$Platform,

        [parameter(Mandatory=$true)]
        [string]$Configuration
    )

    Write-Host "building $SolutionName,$Platform,$Configuration"
    nuget.exe restore $SolutionName
    if (-not $?)
    {
        Throw "nuget restore failed"
    }
    msbuild.exe $SolutionName /p:Platform=$Platform /p:Configuration=$Configuration
    if (-not $?)
    {
        Throw "building $SolutionName,$Platform,$Configuration failed"
    }
}

#.SYNOPSIS
# Creates a winsdkfb nuget package.
function Invoke-NugetPackage()
{
    # set version number
    $Version = (Get-Content $env:winsdkfbRoot\winsdkfb\build\nuget\VERSION).ToString()
    $env:version = $Version
    # build-nupkg.cmd requires that the current working directory is the
    # directory that it sits in because it uses relative paths.
    pushd $env:winsdkfbRoot\winsdkfb\build\nuget
    & $env:WinsdkfbRoot\winsdkfb\build\nuget\build-nupkg.cmd
    if (-not (Test-Path "$env:WinsdkfbRoot\winsdkfb\bin\winsdkfb.${Version}.nupkg"))
    {
        popd
        Throw "Failed to build SDK NuGet package"
    }
    popd
}

# main
Test-BuildToolsPresent
$SolutionFiles = @("$env:WinsdkfbRoot\winsdkfb\winsdkfb.sln", "$env:winsdkfbRoot\winsdkfb\winsdkfb_uwp\winsdkfb_uwp.sln")
$Platforms = @("Win32", "x64", "ARM")
$Configurations = @("Debug", "Release")
foreach ($Solution in $SolutionFiles)
{
    foreach ($Platform in $Platforms)
    {
        foreach ($Configuration in $Configurations)
        {
            Invoke-OpenConsoleBuild -SolutionName $Solution -Platform $Platform -Configuration $Configuration
        }
    }
}
Invoke-NugetPackage
