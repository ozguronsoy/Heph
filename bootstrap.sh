#!/usr/bin/env bash

# ----------------------------------------
# PARAMS
# ----------------------------------------

# passed by the user
BuildDir="build"
InstallDir=""
CompilerDir=""
Generator="default"

install=false

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
seen_BuildDir=false
seen_InstallDir=false
seen_CompilerDir=false
seen_Generator=false

seen_install=false

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

CMakeOptions=()
BuildType=""

Compiler=""
C_Compiler=""
Cpp_Compiler=""

Compilers=("gcc" "clang")

while [[ $# -gt 0 ]]; do
    case "$1" in
        --build-dir)
            BuildDir="$2"
            seen_BuildDir=true
            shift 2
            ;;
        --install-dir)
            InstallDir="$2"
            seen_InstallDir=true
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

        --install)
            install=true
            seen_install=true
            shift
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

echo-verbose() {
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
    echo "  ./bootstrap.sh [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --build-dir <path>      Specify the build directory (default: 'build')"
    echo "  --install-dir <path>    Specify the install directory"
    echo "  --compiler-dir <path>   Path to the custom compiler toolchain"
    echo ""
    echo "  --install               Run the installation step after building the project"
    echo ""
    echo "  --gcc                   Use GCC as the compiler"
    echo "  --clang                 Use Clang as the compiler"
    echo "  --generator <string>    Specify the CMake generator (e.g., 'Ninja', 'Unix Makefiles')"
    echo ""
    echo "  --debug                 Configure the project in Debug mode"
    echo "  --release               Configure the project in Release mode"
    echo ""
    echo "  --rebuild               Delete the build directory and perform a clean rebuild"
    echo "  --clean                 Delete the build directory and exit"
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
# RESTORE CACHE
# ----------------------------------------

if [ "$CleanCache" = true ] && [ -f "$CacheFilePath" ]; then
    echo "Removing the cache file"
    rm -f "$CacheFilePath"

    if [ -f "$CacheFilePath" ]; then
        echo "Failed to remove the cache file" >&2
        exit 1
    fi
fi

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

        if [[ -n "${CacheMap[BuildDir]}" ]] && ! param_set BuildDir; then
            BuildDir="${CacheMap[BuildDir]}"
        fi

        if [[ -n "${CacheMap[InstallDir]}" ]] && ! param_set InstallDir; then
            InstallDir="${CacheMap[InstallDir]}"
        fi

        if [[ -n "${CacheMap[CompilerDir]}" ]] && ! param_set CompilerDir; then
            CompilerDir="${CacheMap[CompilerDir]}"
        fi

        if [[ -n "${CacheMap[Compiler]}" ]]; then
            Compiler="${CacheMap[Compiler]}"
        
            if [[ "$Compiler" == "default" && ( "$gcc" == true || "$clang" == true ) ]]; then
                Rebuild=true
            elif [[ "$Compiler" == "gcc" && "$clang" == true ]]; then
                Rebuild=true
            elif [[ "$Compiler" == "clang" && "$gcc" == true ]]; then
                Rebuild=true
            fi
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

    fi
fi

# ----------------------------------------
# END RESTORE CACHE
# ----------------------------------------

# ----------------------------------------
# CLEAN
# ----------------------------------------

if [ "$Clean" = true ] || [ "$Rebuild" = true ]; then
    echo "Cleaning"

    if [ -d "$BuildDir" ]; then
        rm -rf "$BuildDir"

        if [ -d "$BuildDir" ]; then
            echo "Failed to remove the output directory, continuing the build"
        fi
    fi

    if [ "$Clean" = true ]; then
        exit 0
    fi
fi

# ----------------------------------------
# END CLEAN
# ----------------------------------------

# ----------------------------------------
# CREATE DIRECTORIES
# ----------------------------------------

echo "Building"
echo "Checking for the output directory"

if [ ! -d "$BuildDir" ]; then
    echo "Creating the output directory"
    mkdir -p "$BuildDir"

    if [ ! -d "$BuildDir" ]; then
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

if [[ -n "$InstallDir" ]]; then
    CMakeOptions+=("-DCMAKE_INSTALL_PREFIX=$InstallDir")
fi

if [[ "$gcc" = true ]]; then
    Compiler="gcc"
    if [[ -n "$CompilerDir" ]]; then
        C_Compiler="${CompilerDir}/gcc"
        Cpp_Compiler="${CompilerDir}/g++"
    else
        C_Compiler="gcc"
        Cpp_Compiler="g++"
    fi

    SupportedGenerators=("Ninja" "Unix Makefiles")
    if [[ ! " ${SupportedGenerators[@]} " =~ " ${Generator} " ]]; then
        Generator="Unix Makefiles"
    fi

    CMakeOptions+=("-G" "$Generator")
fi

if [[ "$clang" = true ]]; then
    Compiler="clang"
    if [[ -n "$CompilerDir" ]]; then
        C_Compiler="${CompilerDir}/clang"
        Cpp_Compiler="${CompilerDir}/clang++"
    else
        C_Compiler="clang"
        Cpp_Compiler="clang++"
    fi

    SupportedGenerators=("Ninja" "Unix Makefiles")
    if [[ ! " ${SupportedGenerators[@]} " =~ " ${Generator} " ]]; then
        Generator="Unix Makefiles"
    fi

    CMakeOptions+=("-G" "$Generator")
fi

if [[ " ${Compilers[*]} " =~ " $Compiler " ]]; then
    CMakeOptions+=("-DCMAKE_C_COMPILER=$C_Compiler" "-DCMAKE_CXX_COMPILER=$Cpp_Compiler")
else
    Compiler="default"
fi

if [ -z "$BuildType" ] || { [ "$BuildType" = "Release" ] && [ "$Debug" = true ] && [ "$Release" != true ]; } || { [ "$BuildType" = "Debug" ] && [ "$Release" = true ] && [ "$Debug" != true ]; }; then
    if [ "$Release" = true ]; then
        BuildType="Release"
    else
        BuildType="Debug"
    fi

    CMakeOptions+=("-DCMAKE_BUILD_TYPE=$BuildType")
fi

if [ "$BuildTests" = true ]; then
    CMakeOptions+=("-DHEPH_BUILD_TESTS=ON")
fi

if [ "$BuildDocs" = true ]; then
    CMakeOptions+=("-DHEPH_BUILD_DOCS=ON")
fi

if [ "$Static" = true ]; then
    CMakeOptions+=("-DHEPH_BUILD_STATIC=ON")
fi

if [ "$Shared" = true ]; then
    CMakeOptions+=("-DHEPH_BUILD_SHARED=ON")
fi

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
            echo "BuildDir=$BuildDir"
            echo "InstallDir=$InstallDir"
            echo "CompilerDir=$CompilerDir"
            echo "Compiler=$Compiler"
            echo "Generator=$Generator"
            echo "BuildType=$BuildType"
        } > "$CacheFilePath"
    fi
fi

# ----------------------------------------
# END UPDATE CACHE
# ----------------------------------------

# ----------------------------------------
# BUILD
# ----------------------------------------

echo "Configuring cmake"

echo-verbose "Running command: cmake -S . -B $BuildDir ${CMakeOptions[@]}"
cmake -S . -B "$BuildDir" "${CMakeOptions[@]}"

if [ $? -ne 0 ]; then
    exit $?
fi

echo-verbose "Running command: cmake --build $BuildDir --config $BuildType"
cmake --build "$BuildDir" --config "$BuildType"

if [ $? -ne 0 ]; then
    exit $?
fi

if [ "$install" = true]; then
    
    echo-verbose "Running command: cmake --install $BuildDir --config $BuildType"
    cmake --install "$BuildDir" --config "$BuildType"

    if [ $? -ne 0 ]; then
        exit $?
    fi

fi

echo "Build files have been written to \"$BuildDir\""

# ----------------------------------------
# END BUILD
# ----------------------------------------