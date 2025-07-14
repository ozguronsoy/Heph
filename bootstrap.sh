#!/usr/bin/env bash

# ----------------------------------------
# PARAMS
# ----------------------------------------

# passed by the user
OutputDir="build"
CompilerDir=""
Generator=""

gcc=false
clang=false

Debug=false
Release=false
Rebuild=false
Clean=false

CleanCache=false
NoCache=false

BuildTests=false
BuildDocs=false

Static=false
Shared=false

Verbose=false
Help=false

# flags for checking if args are passed
seen_OutputDir=false
seen_CompilerDir=false
seen_Generator=false

seen_gcc=false
seen_clang=false

seen_Debug=false
seen_Release=false
seen_Rebuild=false
seen_Clean=false

seen_CleanCache=false
seen_NoCache=false

seen_BuildTests=false
seen_BuildDocs=false

seen_Static=false
seen_Shared=false

seen_Verbose=false
seen_Help=false

# used internally by the script
CacheFilePath=".bootstrap_cache"

CMakeOptions=""
BuildType=""

Compiler=""
C_Compiler=""
Cpp_Compiler=""

Compilers=("gcc" "clang")
Generators=("Ninja" "Unix Makefiles")

while [[ $# -gt 0 ]]; do
    case "$1" in
        --output-dir)
            OutputDir="$2"
            seen_OutputDir=true
            shift 2
            ;;
        --compiler-dir)
            CompilerDir="$2"
            seen_CompilerDir=true
            shift 2
            ;;
        --generator)
            Generator="$2"
            seen_Generator=true
            shift 2
            ;;

        --gcc)
            gcc=true
            seen_gcc=true
            shift
            ;;
        --clang)
            clang=true
            seen_clang=true
            shift
            ;;

        --debug)
            Debug=true
            seen_Debug=true
            shift
            ;;
        --release)
            Release=true
            seen_Release=true
            shift
            ;;
        --rebuild)
            Rebuild=true
            seen_Rebuild=true
            shift
            ;;
        --clean)
            Clean=true
            seen_Clean=true
            shift
            ;;

        --clean-cache)
            CleanCache=true
            seen_CleanCache=true
            shift
            ;;
        --no-cache)
            NoCache=true
            seen_NoCache=true
            shift
            ;;

        --build-tests)
            BuildTests=true
            seen_BuildTests=true
            shift
            ;;
        --build-docs)
            BuildDocs=true
            seen_BuildDocs=true
            shift
            ;;

        --static)
            Static=true
            seen_Static=true
            shift
            ;;
        --shared)
            Shared=true
            seen_Shared=true
            shift
            ;;

        --verbose)
            Verbose=true
            seen_Verbose=true
            shift
            ;;
        --help)
            Help=true
            seen_Help=true
            shift
            ;;

        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

if [ "$Debug" = true ] && [ "$Release" = true ]; then
    echo "You cannot specify more than one build type" >&2
    exit 1
fi

compiler_count=0
[ "$gcc" = true ] && ((compiler_count++))
[ "$msvc" = true ] && ((compiler_count++))
[ "$clang" = true ] && ((compiler_count++))

if [ "$compiler_count" -gt 1 ]; then
    echo "You cannot specify more than one compiler option" >&2
    exit 1
fi

echo_verbose() {
    if [ "$Verbose" = true ]; then
        echo "VERBOSE: $*"
    fi
}

# ----------------------------------------
# END PARAMS
# ----------------------------------------

# ----------------------------------------
# HELP
# ----------------------------------------

if [ "$Help" = true ]; then

    echo "Usage:"
    echo "  ./bootstrap [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --output-dir <path>     Specify the output directory for the build (default: 'build')"
    echo "  --compiler-dir <path>   Path to the custom compiler toolchain"
    echo ""
    echo "  --gcc                   Use GCC as the compiler"
    echo "  --clang                 Use Clang as the compiler"
    echo "  --generator <string>    Specify the CMake generator (e.g., 'Ninja', 'Unix Makefiles')"
    echo ""
    echo "  --debug                 Configure the project in Debug mode"
    echo "  --release               Configure the project in Release mode"
    echo ""
    echo "  --rebuild               Delete the output directory and perform a clean rebuild"
    echo "  --clean                 Delete the output directory and exit"
    echo ""
    echo "  --clean-cache           Delete the cache file"
    echo "  --no-cache              Ignore cache file for this run"
    echo ""
    echo "  --build-tests           Build the unit tests"
    echo "  --build-docs            Build the documentation"
    echo ""
    echo "  --shared                Build shared libraries"
    echo "  --static                Build static libraries"
    echo ""
    echo "  --verbose               Show detailed output"
    echo ""
    echo "Examples:"
    echo "  ./bootstrap --build-docs"
    echo "  ./bootstrap --rebuild --build-tests --verbose"
    echo "  ./bootstrap --msvc --release --build-tests"
    echo ""

fi

# ----------------------------------------
# END HELP
# ----------------------------------------

# ----------------------------------------
# CLEAN
# ----------------------------------------

if [ "$Clean" = true ] || [ "$Rebuild" = true ]; then
    echo "Cleaning"

    if [ -d "$OutputDir" ]; then
        rm -rf "$OutputDir"

        if [ -d "$OutputDir" ]; then
            echo "Failed to remove the output directory, continuing the build"
        fi
    fi
fi

if [ "$CleanCache" = true ] && [ -f "$CacheFilePath" ]; then
    echo "Removing the cache file"
    rm -f "$CacheFilePath"

    if [ -f "$CacheFilePath" ]; then
        echo "Failed to remove the cache file" >&2
        exit 1
    fi
fi

if [ "$Clean" = true ]; then
    exit 0
fi

# ----------------------------------------
# END CLEAN
# ----------------------------------------

# ----------------------------------------
# RESTORE CACHE
# ----------------------------------------

if [ "$NoCache" != true ] && [ "$CleanCache" != true ]; then
    echo "Restoring options from the cache file"

    if [ -f "$CacheFilePath" ]; then
        declare -A CacheMap=()

        while IFS= read -r Line || [ -n "$Line" ]; do
            [[ "$Line" =~ ^\s*# ]] && continue
            [[ "$Line" =~ ^\s*$ ]] && continue

            # split at first '=' into key and value
            IFS='=' read -r Key Value <<< "$Line"
            Key=$(echo "$Key" | xargs)   # trim whitespace
            Value=$(echo "$Value" | xargs)

            CacheMap["$Key"]="$Value"
        done < "$CacheFilePath"

        param_set() {
            local param=$1
            eval "[ \"\${seen_${param}:-false}\" = true ]"
        }

        if [[ -n "${CacheMap[OutputDir]}" ]] && ! param_set OutputDir; then
            OutputDir="${CacheMap[OutputDir]}"
        fi

        if [[ -n "${CacheMap[CompilerDir]}" ]] && ! param_set CompilerDir; then
            CompilerDir="${CacheMap[CompilerDir]}"
        fi

        if [[ -n "${CacheMap[Compiler]}" ]] && [ "$gcc" != true ] && [ "$clang" != true ]; then
            Compiler="${CacheMap[Compiler]}"
        fi

        if [[ -n "${CacheMap[Generator]}" ]] && ! param_set Generator; then
            Generator="${CacheMap[Generator]}"
        fi

        if [[ -n "${CacheMap[BuildType]}" ]]; then
            BuildType="${CacheMap[BuildType]}"

            if { [ "$BuildType" = "Release" ] && param_set Debug; } || { [ "$BuildType" = "Debug" ] && param_set Release; }; then
                Rebuild=true
            fi
        fi

        # helper to convert string "true"/"false" to bash booleans
        str_to_bool() {
            if [ "$1" = "true" ]; then
                echo true
            else
                echo false
            fi
        }

        if [[ -n "${CacheMap[BuildTests]}" ]] && ! param_set BuildTests; then
            BuildTests=$(str_to_bool "${CacheMap[BuildTests]}")
        fi

        if [[ -n "${CacheMap[BuildDocs]}" ]] && ! param_set BuildDocs; then
            BuildDocs=$(str_to_bool "${CacheMap[BuildDocs]}")
        fi

        if [[ -n "${CacheMap[Static]}" ]] && ! param_set Static; then
            Static=$(str_to_bool "${CacheMap[Static]}")
        fi

        if [[ -n "${CacheMap[Shared]}" ]] && ! param_set Shared; then
            Shared=$(str_to_bool "${CacheMap[Shared]}")
        fi

    fi
fi

# ----------------------------------------
# END RESTORE CACHE
# ----------------------------------------

# ----------------------------------------
# CREATE DIRECTORIES
# ----------------------------------------

echo "Building"
echo "Checking for the output directory"

if [ ! -d "$OutputDir" ]; then
    echo "Creating the output directory"
    mkdir -p "$OutputDir"

    if [ ! -d "$OutputDir" ]; then
        echo "Failed to create the output directory" >&2
        exit 1
    fi
fi

# ----------------------------------------
# END CREATE DIRECTORIES
# ----------------------------------------

# ----------------------------------------
# CREATE CMAKE PARAMS
# ----------------------------------------

if [ "$gcc" = true ]; then
    Compiler="gcc"
    if [ -n "$CompilerDir" ]; then
        C_Compiler="$CompilerDir/gcc"
        Cpp_Compiler="$CompilerDir/g++"
    else
        C_Compiler="gcc"
        Cpp_Compiler="g++"
    fi

    if [[ ! " ${Generators[*]} " =~ " $Generator " ]]; then
        Generator="Unix Makefiles"
    fi
fi

if [ "$clang" = true ]; then
    Compiler="clang"
    if [ -n "$CompilerDir" ]; then
        C_Compiler="$CompilerDir/clang"
        Cpp_Compiler="$CompilerDir/clang++"
    else
        C_Compiler="clang"
        Cpp_Compiler="clang++"
    fi

    if [[ ! " ${Generators[*]} " =~ " $Generator " ]]; then
        Generator="Unix Makefiles"
    fi
fi

if [[ " ${Generators[*]} " =~ " $Generator " ]]; then
    CMakeOptions+="-G \"$Generator\" "
else
    Generator="default"
fi

if [[ " ${Compilers[*]} " =~ " $Compiler " ]]; then
    CMakeOptions+="-DCMAKE_C_COMPILER=\"$C_Compiler\" -DCMAKE_CXX_COMPILER=\"$Cpp_Compiler\" "
else
    Compiler="default"
fi

if [ -z "$BuildType" ] || { [ "$BuildType" = "Release" ] && [ "$Debug" = true ] && [ "$Release" != true ]; } || { [ "$BuildType" = "Debug" ] && [ "$Release" = true ] && [ "$Debug" != true ]; }; then
    if [ "$Release" = true ]; then
        BuildType="Release"
    else
        BuildType="Debug"
    fi
fi

if [ "$BuildTests" = true ]; then
    CMakeOptions+="-DHEPH_BUILD_TESTS=ON "
fi

if [ "$BuildDocs" = true ]; then
    CMakeOptions+="-DHEPH_BUILD_DOCS=ON "
fi

if [ "$Static" = true ]; then
    CMakeOptions+="-DHEPH_BUILD_STATIC=ON "
fi

if [ "$Shared" = true ]; then
    CMakeOptions+="-DHEPH_BUILD_SHARED=ON "
fi

CMakeOptions="${CMakeOptions%"${CMakeOptions##*[![:space:]]}"}"

# ----------------------------------------
# END CREATE CMAKE PARAMS
# ----------------------------------------

# ----------------------------------------
# UPDATE CACHE
# ----------------------------------------

if [ "$NoCache" != true ]; then
    echo "Updating the cache"

    if [ ! -f "$CacheFilePath" ]; then
        touch "$CacheFilePath"
    fi

    if [ ! -f "$CacheFilePath" ]; then
        echo "Failed to create the cache file"
    else
        {
            echo "OutputDir=$OutputDir"
            echo "CompilerDir=$CompilerDir"
            echo "Compiler=$Compiler"
            echo "Generator=$Generator"
            echo "BuildType=$BuildType"
            echo "BuildTests=$( [ "$BuildTests" = true ] && echo true || echo false )"
            echo "BuildDocs=$( [ "$BuildDocs" = true ] && echo true || echo false )"
            echo "Static=$( [ "$Static" = true ] && echo true || echo false )"
            echo "Shared=$( [ "$Shared" = true ] && echo true || echo false )"
        } > "$CacheFilePath"
    fi
fi

# ----------------------------------------
# END UPDATE CACHE
# ----------------------------------------

echo "Configuring cmake"

echo-verbose "Running command: cmake -S . -B $OutputDir -DCMAKE_BUILD_TYPE=$BuildType $CMakeOptions"
cmake -S . -B "$OutputDir" -DCMAKE_BUILD_TYPE="$BuildType" ${CMakeOptions}

echo-verbose "Running command: cmake --build $OutputDir --config $BuildType"
cmake --build "$OutputDir" --config "$BuildType"

echo "Build files have been written to \"$OutputDir\""