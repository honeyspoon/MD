import os
import sys
import glob
import subprocess
import shutil
import python_src.log as logger
from python_src.constants import BUILD_DIR

def get_targets():
    return [os.path.splitext(f)[0].split('/')[-1] for f in glob.glob('./src/exe/*.cpp')]

def build(target=""):
    abs_build_dir = os.path.join(os.getcwd(), BUILD_DIR)

    if not os.path.exists(abs_build_dir):
        logger.info("Creating build directory")
        result = subprocess.run(["unbuffer", "cmake", "-GNinja", "-B", BUILD_DIR], capture_output=True)
        for line in result.stdout.decode().strip().split('\n'):
            print(line, file=sys.stderr)
        for line in result.stderr.decode().strip().split('\n'):
            print(line, file=sys.stderr)
        if result.returncode != 0:
            logger.error(f"Failed to create build directory. Error: {result.stderr}")
            shutil.rmtree(abs_build_dir)
            sys.exit(result.returncode)
    else:
        logger.info("Reusing existing build directory")

    cmd = ["unbuffer", "cmake", "--build", abs_build_dir]
    if target:
        cmd += ["--target", target]

    result = subprocess.run(cmd, capture_output=True)
    for line in result.stdout.decode().strip().split('\n'):
        print(line, file=sys.stderr)
    for line in result.stderr.decode().strip().split('\n'):
        print(line, file=sys.stderr)

    build_success = bool(result.returncode == 0)

    return build_success


def run(exe, args=[], stdin=sys.stdin, stdout=sys.stdout, stderr=sys.stderr):
    print("="*20,file=sys.stderr)
    build_success = build(exe)
    print("="*20,file=sys.stderr)
    if build_success:
        logger.info(f"{BUILD_DIR}/{exe} {(' '.join(args))}")
        res = subprocess.run(
            [f"{BUILD_DIR}/{exe}", *args],
            cwd=os.getcwd(),
            text=True,
            stdin=stdin,
            stdout=stdout,
            stderr=stderr,
        )
        if res.returncode == 0:
            logger.info(f"return code: {res.returncode}")
        else:
            logger.warn(f"return code: {res.returncode}")
    else:
        logger.error("build failed")
    print("="*20,file=sys.stderr)


def watch(exts):
    logger.info("Watching for file changes ...")
    fswatch_cmd = ["fswatch", "-1", os.getcwd(), "-e", "out"]
    while True:
        process = subprocess.Popen(fswatch_cmd, stdout=subprocess.PIPE, text=True)
        stdout, _ = process.communicate()

        if stdout.strip():
            files = stdout.strip().split("\n")
            files = set([file.replace("~","").strip() for file in files if any(ext in file for ext in exts) ])
            
            if files:
                yield files

