import sys
import os
import stat
import subprocess
import platform

color_default = "\033[0m"
color_warning = "\033[93m"
color_error = "\033[91m"
color_success = "\033[92m"
color_info = "\033[96m"

cache_file_path = ".bootstrap_cache"

options = {
    "--build-dir": "build",
    "--install-dir": "",
    "--compiler-dir": "",
    "--generator": "default",
    "--install": False,

    "--msvc": False,
    "--gcc": False,
    "--clang": False,

    "--debug": False,
    "--release": False,
    "--rebuild": False,
    "--clean": False,

    "--clean-cache": False,
    "--no-cache": False,

    "--build-tests": False,
    "--build-benchmarks": False,
    "--build-docs": False,
    "--static": False,
    "--shared": False,

    "--verbose": False,
    "--help": False
}

def parse_args(args):
    skip_next = False
    for i in range(1, len(args)):
        if skip_next:
            skip_next = False
            continue

        if args[i] in options:
            
            if type(options[args[i]]) == bool:
                options[args[i]] = True

            else:
                if (i + 1) >= len(args):
                    print(f"{color_warning}[Heph] Invalid usage: {args[i]} requires a value. Use --help for more information.{color_default}")
                    continue
                
                options[args[i]] = args[i + 1]
                skip_next = True

        else:
            print(f"{color_warning}[Heph] Invalid argument: {args[i]}{color_default}.")

def get_compiler_arg_count():
    return (options["--msvc"], options["--gcc"], options["--clang"]).count(True)

def restore_cache(args):
    with open(cache_file_path, "r", encoding="utf-8-sig") as f:
        for line in f.readlines():
            line = line.strip()

            if len(line) > 0:
                key, value = line.split("=")
                if key == "BuildDir":
                    if "--build-dir" not in args:
                        options["--build-dir"] = value
                elif key == "InstallDir":
                    if "--install-dir" not in args:
                        options["--install-dir"] = value
                elif key == "CompilerDir":
                    if "--compiler-dir" not in args:
                        options["--compiler-dir"] = value
                elif key == "Compiler":
                    if get_compiler_arg_count() == 0 and value != "default":
                        options[f"--{value}"] = True
                    elif get_compiler_arg_count() == 1 and (value == "default" or not options[f"--{value}"]):
                        options["--rebuild"] = True
                elif key == "Generator":
                    if "--generator" not in args:
                        options["--generator"] = value
                elif key == "BuildType":
                    if "--debug" not in args and "--release" not in args:
                        options[f"--{value.lower()}"] = True
                    elif ("--debug" in args and value.lower() == "release") or ("--release" in args and value.lower() == "debug"):
                        options["--rebuild"] = True
                else:
                    print(f"{color_warning}[Heph] Invalid cache key: {key}{color_default}")
                

def print_verbose(msg):
    if options["--verbose"]:
        print(msg)

def print_help():
        print(f"""{color_info}Usage:{color_default}
  ./bootstrap.sh [OPTIONS]

{color_info}Options:{color_default}
  --build-dir <path>      Specify the build directory (default: 'build')
  --install-dir <path>    Specify the install directory
  --compiler-dir <path>   Path to the custom compiler toolchain

  --install               Run the installation step after building the project

  --gcc                   Use GCC as the compiler
  --clang                 Use Clang as the compiler
  --msvc                  Use MSVC as the compiler
  --generator <string>    Specify the CMake generator (e.g., 'Ninja', 'Unix Makefiles')

  --debug                 Configure the project in Debug mode
  --release               Configure the project in Release mode

  --rebuild               Delete the build directory and perform a clean rebuild
  --clean                 Delete the build directory and exit

  --clean-cache           Delete the cache file
  --no-cache              Ignore cache file for this run

  --build-tests           Build the unit tests
  --build-benchmarks      Build the benchmarks
  --build-docs            Build the documentation

  --static                Build static libraries
  --shared                Build shared libraries

  --verbose               Show detailed output

{color_info}Examples:{color_default}
  python bootstrap.py --build-dir "path/to/build" --build-tests --generator Ninja
  python bootstrap.py --build-docs
  python bootstrap.py --rebuild --build-tests --verbose
  python bootstrap.py --msvc --release --build-tests
""")

if __name__ == "__main__":
    args = sys.argv
    parse_args(args)

    if options["--debug"] and options["--release"]:
        print(f"{color_warning}[Heph] Cannot specify more than one build type, using debug{color_default}")
        options["--release"] = False

    if get_compiler_arg_count() > 1:
        print(f"{color_warning}[Heph] Cannot specify more than one compiler, using default{color_default}")
        options["--msvc"] = False
        options["--gcc"] = False
        options["--clang"] = False

    if options["--help"]:
        print_help()
        sys.exit(0)

    if options["--clean-cache"] and os.path.isfile(cache_file_path):
        print("[Heph] Removing the cache file")
        os.remove(cache_file_path)
        if os.path.isfile(cache_file_path):
            print(f"{color_warning}[Heph] Failed to remove the cache file{color_default}")

    if not options["--no-cache"] and not options["--clean-cache"] and os.path.isfile(cache_file_path):
        print("[Heph] Restoring options from the cache file")
        restore_cache(args)

    if options["--clean"] or options["--rebuild"]:
        print("[Heph] Cleaning")

        def remove_dir(dir):
            if os.path.isdir(dir):
                for name in os.listdir(dir):
                    path = os.path.join(dir, name)
                    if os.path.isfile(path):
                        os.chmod(path, stat.S_IWRITE)
                        os.remove(path)
                    elif os.path.isdir(path):
                        remove_dir(path)
                os.rmdir(dir)
        
        remove_dir(options["--build-dir"])

        if os.path.isdir(options["--build-dir"]):
            print(f"{color_warning}[Heph] Failed to remove the build directory{color_default}")

        if options["--clean"]:
            sys.exit(0)

    print("[Heph] Building")
    print("[Heph] Checking for the build directory")
    
    if not os.path.isdir(options["--build-dir"]):
        print("[Heph] Creating the build directory")
        os.makedirs(options["--build-dir"])
        if not os.path.isdir(options["--build-dir"]):
            print(f"{color_error}[Heph] Failed to create the build directory{color_default}")
            sys.exit(1)
    else:
        print("[Heph] Build directory found")

    cmake_options = []
    compiler = "default"
    build_type = "Debug"

    if options["--install-dir"]:
        cmake_options.append(options["--install-dir"])

    if options["--gcc"]:
        compiler = "gcc"
        c_compiler = os.path.join(options["--compiler-dir"], "gcc")
        cpp_compiler = os.path.join(options["--compiler-dir"], "g++")

        supported_generators = ["Ninja", "Unix Makefiles"]
        if options["--generator"] not in supported_generators:
            options["--generator"] = "Ninja" if platform.system() == "Windows" else "Unix Makefiles"

        cmake_options.extend([f"-DCMAKE_C_COMPILER={c_compiler}", f"-DCMAKE_CXX_COMPILER={cpp_compiler}"])
        cmake_options.extend(["-G", options["--generator"]])

    if options["--clang"]:
        compiler = "clang"
        c_compiler = os.path.join(options["--compiler-dir"], "clang")
        cpp_compiler = os.path.join(options["--compiler-dir"], "clang++")

        supported_generators = ["Ninja", "Unix Makefiles"]
        if options["--generator"] not in supported_generators:
            options["--generator"] = "Ninja" if platform.system() == "Windows" else "Unix Makefiles"

        cmake_options.extend([f"-DCMAKE_C_COMPILER={c_compiler}", f"-DCMAKE_CXX_COMPILER={cpp_compiler}"])
        cmake_options.extend(["-G", options["--generator"]])

    if options["--msvc"]:
        compiler = "msvc"
        c_compiler = os.path.join(options["--compiler-dir"], "cl")
        cpp_compiler = os.path.join(options["--compiler-dir"], "cl")

        supported_generators = ["Visual Studio 17 2022"]
        if options["--generator"] not in supported_generators:
            options["--generator"] = "Visual Studio 17 2022"

        cmake_options.extend([f"-DCMAKE_C_COMPILER={c_compiler}", f"-DCMAKE_CXX_COMPILER={cpp_compiler}"])
        cmake_options.extend(["-G", options["--generator"]])

    if options["--debug"]:
        build_type = "Debug"
    elif options["--release"]:
        build_type = "Release"

    cmake_options.append(f"-DCMAKE_BUILD_TYPE={build_type}")

    if options["--build-tests"]:
        cmake_options.append("-DHEPH_BUILD_TESTS=ON")
    if options["--build-benchmarks"]:
        cmake_options.append("-DHEPH_BUILD_BENCHMARKS=ON")
    if options["--build-docs"]:
        cmake_options.append("-DHEPH_BUILD_DOCS=ON")
    if options["--static"]:
        cmake_options.append("-DHEPH_BUILD_STATIC=ON")
    if options["--shared"]:
        cmake_options.append("-DHEPH_BUILD_SHARED=ON")

    if not options["--no-cache"]:
        print("[Heph] Updating the cache")

        try:
            with open(cache_file_path, 'w') as f:
                print(f"BuildDir={options["--build-dir"]}", file=f)
                print(f"InstallDir={options["--install-dir"]}", file=f)
                print(f"CompilerDir={options["--compiler-dir"]}", file=f)
                print(f"Compiler={compiler}", file=f)
                print(f"Generator={options["--generator"]}", file=f)
                print(f"BuildType={build_type}", file=f)

        except:
            print(f"{color_warning}[Heph] Failed to create the cache file{color_default}")

    print("[Heph] Configuring CMake")
        
    try:
        cmd = ["cmake", "-S", ".", "-B", options["--build-dir"]] + cmake_options
        cmd_str = " ".join(cmd)
        print_verbose(f"[Heph] Running command: {cmd_str}")
        subprocess.run(cmd, check=True)
    except Exception as ex:
        print(f"{color_error}[Heph] Failed to run command: {cmd_str}{color_default}")
        print(f"{color_error}Error {ex}{color_default}")
        sys.exit(1)

    try:
        cmd = ["cmake", "--build", options["--build-dir"], "--config", build_type]
        cmd_str = " ".join(cmd)
        print_verbose(f"[Heph] Running command: {cmd_str}")
        subprocess.run(cmd, check=True)
    except Exception as ex:
        print(f"{color_error}[Heph] Failed to run command: {cmd_str}{color_default}")
        print(f"{color_error}Error {ex}{color_default}")
        sys.exit(1)

    if options["--install"]:
        try:
            cmd = ["cmake", "--install", options["--build-dir"], "--config", build_type]
            cmd_str = " ".join(cmd)
            print_verbose(f"[Heph] Running command: {cmd_str}")
            subprocess.run(cmd, check=True)
        except Exception as ex:
            print(f"{color_error}[Heph] Failed to run command: {cmd_str}{color_default}")
            print(f"{color_error}Error {ex}{color_default}")
            sys.exit(1)

    print(f"{color_success}[Heph] Build files have been written to {options["--build-dir"]}{color_default}")