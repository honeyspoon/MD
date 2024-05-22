import os
import sys
import subprocess


def build(target=""):
    build_dir = "out"
    abs_build_dir = os.path.join(os.getcwd(), build_dir)

    if not os.path.exists(abs_build_dir):
        print("Creating build directory")
        result = subprocess.run(["cmake", "-GNinja", "-B", build_dir], capture_output=False)
        if result.returncode != 0:
            print(f"Failed to create build directory. Error: {result.stderr}")
            sys.exit(result.returncode)
    else:
        print("Reusing existing build directory")

    cmd = ["cmake", "--build", abs_build_dir]
    if target:
        cmd += ["--target", target]

    result = subprocess.run(cmd, cwd=abs_build_dir, capture_output=False)

    build_success = bool(result.returncode == 0)

    return build_success, build_dir


def run(exe, args=[]):
    print()
    print("=============")
    build_success, build_dir = build(exe)
    print("=============")

    print()
    if build_success:
        print("=============")
        print(build_dir, exe, *args)
        subprocess.run(
            [f"{build_dir}/{exe}", *args],
            cwd=os.getcwd(),
            text=True,
            capture_output=False
        )
        print("=============")
        print()
    else:
        print("build failed")


def watch(exe, args=[]):
    run(exe, args)
    print("Watching for file changes...")
    exts = [".cpp", ".h", ".cmake", "CMakeLists.txt"]
    fswatch_cmd = ["fswatch", "-1", os.getcwd(), "-e", "out"]
    while True:
        process = subprocess.Popen(fswatch_cmd, stdout=subprocess.PIPE, text=True)
        stdout, _ = process.communicate()

        if stdout.strip():
            files = stdout.strip().split("\n")
            files = [file for file in files if any(ext in file for ext in exts) ]
            if files:
                print("files changed:", *files, sep="\n")
                run(exe, args)

