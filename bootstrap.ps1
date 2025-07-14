# ----------------------------------------
# PARAMS
# ----------------------------------------

param (
    [string]$OutputDir = "build",
    [string]$CompilerDir = "",
    
    [switch]$gcc,
    [switch]$clang,
    [switch]$msvc,
    [string]$Generator = "",
    
    [switch]$Debug,
    [switch]$Release,
    [switch]$Rebuild, # removes the output directory to do a clean build 
    [switch]$Clean, # removes the output directory and stops the configuration
    
    [switch]$CleanCache, # removes the cache file
    [switch]$NoCache, # does not use cache for current configuration but does not delete the existsing cache neither
    
    [switch]$BuildTests,
    [switch]$BuildDocs,
    
    [switch]$Static,
    [switch]$Shared,

    [switch]$Verbose,
    [switch]$Help
)

[string]$CacheFilePath = ".bootstrap_cache"

[string]$CMakeOptions = ""
[string]$BuildType = ""

[string]$Compiler = ""
[string]$C_Compiler = ""
[string]$Cpp_Compiler = ""

[string[]]$Compilers = @(
                           "gcc",
                           "msvc",
                           "clang"
                    )


[string[]]$Generators = @(
                            "Ninja",
                            "Unix Makefiles",
                            "Visual Studio 17 2022",
                            "MinGW Makefiles"
                    )

[ConsoleColor]$InfoColor = "Gray"
[ConsoleColor]$SuccessColor = "Green"
[ConsoleColor]$WarningColor = "Yellow"
[ConsoleColor]$ErrorColor = "Red"

if ($Debug -and $Release)
{
    Write-Host "You cannot specify more than one build type" -ForegroundColor $ErrorColor
    exit 1
}

if ((@($gcc, $msvc, $clang) | Where-Object { $_ }).Count -gt 1)
{
    Write-Host "You cannot specify more than one compiler option" -ForegroundColor $ErrorColor
    exit 1
}

if ($Verbose)
{
    $VerbosePreference = "Continue"
}

# ----------------------------------------
# END PARAMS
# ----------------------------------------

# ----------------------------------------
# HELP
# ----------------------------------------

if ($Help)
{
    Write-Host "Usage:" -ForegroundColor Cyan
    Write-Host "  ./bootstrap [OPTIONS]" -ForegroundColor $InfoColor
    Write-Host ""

    Write-Host "Options:" -ForegroundColor Cyan

    Write-Host "  -OutputDir <path>     " -NoNewline; Write-Host "Specify the output directory for the build (default: 'build')" -ForegroundColor $InfoColor
    Write-Host "  -CompilerDir <path>   " -NoNewline; Write-Host "Path to the custom compiler toolchain" -ForegroundColor $InfoColor

    Write-Host ""
    Write-Host "  -gcc                  " -NoNewline; Write-Host "Use GCC as the compiler" -ForegroundColor $InfoColor
    Write-Host "  -clang                " -NoNewline; Write-Host "Use Clang as the compiler" -ForegroundColor $InfoColor
    Write-Host "  -msvc                 " -NoNewline; Write-Host "Use MSVC as the compiler" -ForegroundColor $InfoColor
    Write-Host "  -Generator <string>   " -NoNewline; Write-Host "Specify the CMake generator (e.g., 'Ninja', 'Unix Makefiles', 'Visual Studio 17 2022')" -ForegroundColor $InfoColor

    Write-Host ""
    Write-Host "  -Debug                " -NoNewline; Write-Host "Configure the project in Debug mode" -ForegroundColor $InfoColor
    Write-Host "  -Release              " -NoNewline; Write-Host "Configure the project in Release mode" -ForegroundColor $InfoColor

    Write-Host ""
    Write-Host "  -Rebuild              " -NoNewline; Write-Host "Delete the output directory and perform a clean rebuild" -ForegroundColor $InfoColor
    Write-Host "  -Clean                " -NoNewline; Write-Host "Delete the output directory and exit" -ForegroundColor $InfoColor

    Write-Host ""
    Write-Host "  -CleanCache           " -NoNewline; Write-Host "Delete the cache file" -ForegroundColor $InfoColor
    Write-Host "  -NoCache              " -NoNewline; Write-Host "Ignore cache file for this run" -ForegroundColor $InfoColor

    Write-Host ""
    Write-Host "  -BuildTests           " -NoNewline; Write-Host "Build the unit tests" -ForegroundColor $InfoColor
    Write-Host "  -BuildDocs            " -NoNewline; Write-Host "Build the documentation" -ForegroundColor $InfoColor

    Write-Host ""
    Write-Host "  -Shared               " -NoNewline; Write-Host "Build shared libraries" -ForegroundColor $InfoColor
    Write-Host "  -Static               " -NoNewline; Write-Host "Build static libraries" -ForegroundColor $InfoColor

    Write-Host ""
    Write-Host "  -Verbose              " -NoNewline; Write-Host "Show detailed output" -ForegroundColor $InfoColor

    Write-Host ""
    Write-Host "Examples:" -ForegroundColor Cyan
    Write-Host "  ./bootstrap -BuildDocs" -ForegroundColor DarkGray
    Write-Host "  ./bootstrap -Rebuild -BuildTests -Verbose" -ForegroundColor DarkGray
    Write-Host "  ./bootstrap -msvc -Release -BuildTests" -ForegroundColor DarkGray
    Write-Host ""

    exit 0
}

# ----------------------------------------
# END HELP
# ----------------------------------------

# ----------------------------------------
# CLEAN
# ----------------------------------------

if ($Clean -or $Rebuild)
{
    Write-Host "Cleaning" -ForegroundColor $InfoColor
    
    if (Test-Path $OutputDir)
    {
        Remove-Item -Path $OutputDir -Recurse -Force

        if (Test-Path $OutputDir)
        {
            Write-Host "Failed to remove the output directory, continuing the build" -ForegroundColor $WarningColor
        }
    }
}

if ($CleanCache -and (Test-Path $CacheFilePath))
{
    Write-Host "Removing the cache file" -ForegroundColor $InfoColor
    Remove-Item $CacheFilePath -Force

    if (Test-Path $CacheFilePath)
    {
        Write-Host "Failed to remove the cache file" -ForegroundColor $ErrorColor
        exit 1
    }
}

if ($Clean)
{
    exit 0
}

# ----------------------------------------
# END CLEAN
# ----------------------------------------

# ----------------------------------------
# RESOTRE CACHE
# ----------------------------------------

if (-not $NoCache -and -not $CleanCache)
{
    Write-Host "Restoring options from the cache file" -ForegroundColor $InfoColor

    if (Test-Path $CacheFilePath)
    {
        $Cache = Get-Content -Path $CacheFilePath

        $CacheMap = @{}

        foreach ($Line in $Cache) 
        {
            if ($Line -match "^\s*#") { continue }  # skip comments
            if ($Line -match "^\s*$") { continue }  # skip blank lines
        
            $Key, $Value = $Line -split '=', 2
            $CacheMap[$Key.Trim()] = $Value.Trim()
        }

        if ($CacheMap.ContainsKey("OutputDir") -and -not $PSBoundParameters.ContainsKey("OutputDir"))
        {
            $OutputDir = $CacheMap["OutputDir"]
        }

        if ($CacheMap.ContainsKey("CompilerDir") -and -not $PSBoundParameters.ContainsKey("CompilerDir"))
        {
            $CompilerDir = $CacheMap["CompilerDir"]
        }

        if ($CacheMap.ContainsKey("Compiler") -and -not $gcc -and -not $clang -and -not $msvc)
        {
            $Compiler = $CacheMap["Compiler"]
        }

        if ($CacheMap.ContainsKey("Generator") -and -not $PSBoundParameters.ContainsKey("Generator"))
        {
            $Generator = $CacheMap["Generator"]
        }

        if ($CacheMap.ContainsKey("BuildType"))
        {
            $BuildType = $CacheMap["BuildType"]

            if (($BuildType -eq "Release" -and $PSBoundParameters.ContainsKey("Debug")) -or ($BuildType -eq "Debug" -and $PSBoundParameters.ContainsKey("Release")))
            {
                $Rebuild = $true
            }
        }

        if ($CacheMap.ContainsKey("BuildTests") -and -not $PSBoundParameters.ContainsKey("BuildTests"))
        {
            $BuildTests = if ($CacheMap["BuildTests"] -eq "true") { $true } else { $false }
        }

        if ($CacheMap.ContainsKey("BuildDocs") -and -not $PSBoundParameters.ContainsKey("BuildDocs"))
        {
            $BuildDocs = if ($CacheMap["BuildDocs"] -eq "true") { $true } else { $false }
        }

        if ($CacheMap.ContainsKey("Static") -and -not $PSBoundParameters.ContainsKey("Static"))
        {
            $Static = if ($CacheMap["Static"] -eq "true") { $true } else { $false }
        }

        if ($CacheMap.ContainsKey("Shared") -and -not $PSBoundParameters.ContainsKey("Shared"))
        {
            $Shared = if ($CacheMap["Shared"] -eq "true") { $true } else { $false }
        }
    }
}

# ----------------------------------------
# END RESOTRE CACHE
# ----------------------------------------

# ----------------------------------------
# CREATE DIRECTORIES
# ----------------------------------------

Write-Host "Building" -ForegroundColor $InfoColor
Write-Host "Checking for the output directory" -ForegroundColor $InfoColor


if (-not (Test-Path $OutputDir))
{
    Write-Host "Creating the output directory" -ForegroundColor $InfoColor    
    New-Item -ItemType Directory -Path $OutputDir | Out-Null

    if (!(Test-Path $OutputDir))
    {
        Write-Host "Failed to create the output directory" -ForegroundColor $ErrorColor
        exit 1
    }
}

# ----------------------------------------
# END CREATE DIRECTORIES
# ----------------------------------------

# ----------------------------------------
# CREATE CMAKE PARAMS
# ----------------------------------------

if ($gcc)
{
    $Compiler = "gcc"
    $C_Compiler = if ($CompilerDir -ne "") { Join-Path $CompilerDir "gcc" } else { "gcc" }
    $Cpp_Compiler = if ($CompilerDir -ne "") { Join-Path $CompilerDir "g++" } else { "g++" }

    if (-not ($Generators -contains $Generator))
    {
        $Generator = "Ninja"
    }
}

if ($clang)
{
    $Compiler = "clang"
    $C_Compiler = if ($CompilerDir -ne "") { Join-Path $CompilerDir "clang" } else { "clang" }
    $Cpp_Compiler = if ($CompilerDir -ne "") { Join-Path $CompilerDir "clang++" } else { "clang++" }
    
    if (-not ($Generators -contains $Generator))
    {
        $Generator = "Ninja"
    }
}

if ($msvc)
{
    $Compiler = "msvc"
    $C_Compiler = if ($CompilerDir -ne "") { Join-Path $CompilerDir "cl" } else { "cl" }
    $Cpp_Compiler = if ($CompilerDir -ne "") { Join-Path $CompilerDir "cl" } else { "cl" }
    
    if (-not ($Generators -contains $Generator))
    {
        $Generator = "Visual Studio 17 2022"
    }
}

if ($Generators -contains $Generator)
{
    $CMakeOptions += "-G `"$Generator`" "
}
else 
{
    $Generator = "default"    
}

if ($Compilers -contains $Compiler)
{
    $CMakeOptions += "-DCMAKE_C_COMPILER=`"$C_Compiler`" -DCMAKE_CXX_COMPILER=`"$Cpp_Compiler`" "
}
else 
{
    $Compiler = "default"    
}

if ($BuildType -eq "" -or ($BuildType -eq "Release" -and $Debug -and -not $Release) -or ($BuildType -eq "Debug" -and $Release -and -not $Debug))
{
    if ($Release)
    {
        $BuildType = "Release"
    }
    else 
    {
        $BuildType = "Debug"
    }
}

if ($BuildTests)
{
    $CMakeOptions += "-DHEPH_BUILD_TESTS=ON "
}

if ($BuildDocs)
{
    $CMakeOptions += "-DHEPH_BUILD_DOCS=ON "
}

if ($Static)
{
    $CMakeOptions += "-DHEPH_BUILD_STATIC=ON "
}

if ($Shared)
{
    $CMakeOptions += "-DHEPH_BUILD_SHARED=ON "
}

$CMakeOptions = $CMakeOptions.Trim()

# ----------------------------------------
# END CREATE CMAKE PARAMS
# ----------------------------------------

# ----------------------------------------
# UPDATE CACHE
# ----------------------------------------

if (-not $NoCache)
{
    Write-Host "Updating the cache" -ForegroundColor $InfoColor

    if (-not (Test-Path $CacheFilePath))
    {
        New-Item -ItemType File -Path $CacheFilePath -Force | Out-Null
    }

    if (-not (Test-Path $CacheFilePath))
    {
        Write-Host "Failed to create the cache file" -ForegroundColor $WarningColor
    }
    else 
    {
        $Cache = ""
        $Cache += "OutputDir=$OutputDir`n"
        $Cache += "CompilerDir=$CompilerDir`n"
        $Cache += "Compiler=$Compiler`n"
        $Cache += "Generator=$Generator`n"
        $Cache += "BuildType=$BuildType`n"
        $Cache += "BuildTests=$($BuildTests.ToString().ToLower())`n"
        $Cache += "BuildDocs=$($BuildDocs.ToString().ToLower())`n"
        $Cache += "Static=$($Static.ToString().ToLower())`n"
        $Cache += "Shared=$($Shared.ToString().ToLower())`n"

        $Cache | Set-Content -Path $CacheFilePath -Encoding UTF8
    }
}

# ----------------------------------------
# END UPDATE CACHE
# ----------------------------------------

# ----------------------------------------
# BUILD
# ----------------------------------------

Write-Host "Configuring cmake" -ForegroundColor $InfoColor

Write-Verbose "Running command: 'cmake -S . -B $OutputDir -DCMAKE_BUILD_TYPE=$BuildType $CMakeOptions'"
cmake -S . -B "$OutputDir" -DCMAKE_BUILD_TYPE="$BuildType" $($CMakeOptions -split " ")

Write-Verbose "Running command: 'cmake --build $OutputDir --config $BuildType"
cmake --build "$OutputDir" --config $BuildType

Write-Host "Build files have been written to `"$OutputDir`"" -ForegroundColor $SuccessColor

# ----------------------------------------
# END BUILD
# ----------------------------------------