import os
import sys
import subprocess

import logging

class CustomFormatter(logging.Formatter):
    grey = "\x1b[38;20m"
    green = "\033[32m"
    yellow = "\x1b[33;20m"
    red = "\x1b[31;20m"
    bold_red = "\x1b[31;1m"

    COLORS = {
        logging.DEBUG: grey,
        logging.INFO: green,
        logging.WARNING: yellow,
        logging.ERROR: red,
        logging.CRITICAL: bold_red
    }

    def format(self, record):
        reset = "\x1b[0m"
        color = self.COLORS[record.levelno]
        log_fmt = f"%(asctime)s - [{color}%(levelname)s{reset}] - %(filename)s:%(lineno)d - %(message)s"
        formatter = logging.Formatter(log_fmt, datefmt="%Y-%m-%d %H:%M:%S")
        return formatter.format(record)

logger = logging.getLogger("My_app")
logger.setLevel(logging.DEBUG)

ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)

ch.setFormatter(CustomFormatter())

logger.addHandler(ch)

def colorize_message(message, color):
    return f"{color}{message}{Color.RESET}"

def build(target=""):
    build_dir = "out"
    abs_build_dir = os.path.join(os.getcwd(), build_dir)

    if not os.path.exists(abs_build_dir):
        logger.info("Creating build directory")
        result = subprocess.run(["unbuffer", "cmake", "-GNinja", "-B", build_dir], capture_output=True)
        for line in result.stdout.decode().strip().split('\n'):
            print(line, file=sys.stderr)
        for line in result.stderr.decode().strip().split('\n'):
            print(line, file=sys.stderr)
        if result.returncode != 0:
            logger.error(f"Failed to create build directory. Error: {result.stderr}")
            os.rmdir(abs_build_dir)
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

    return build_success, build_dir


def run(exe, args=[]):
    build_success, build_dir = build(exe)

    if build_success:
        logger.info(f"{build_dir} {exe} {' '.join(args)}")
        res = subprocess.run(
            [f"{build_dir}/{exe}", *args],
            cwd=os.getcwd(),
            text=True,
            capture_output=False
        )
        logger.info(f"return code: {res.returncode}")
        logger.info("")
    else:
        logger.error("build failed")


def watch(exe, args=[]):
    run(exe, args)
    logger.info("Watching for file changes ...")
    exts = [".cpp", ".h", "CMakeLists.txt"]
    fswatch_cmd = ["fswatch", "-1", os.getcwd(), "-e", "out"]
    while True:
        process = subprocess.Popen(fswatch_cmd, stdout=subprocess.PIPE, text=True)
        stdout, _ = process.communicate()

        if stdout.strip():
            files = stdout.strip().split("\n")
            files = set([file.replace("~","").strip() for file in files if any(ext in file for ext in exts) ])
            
            if files:
                print("="*20,file=sys.stderr)
                logger.info("files changed:\n" + "\n".join(files))
                print("="*20,file=sys.stderr)
                run(exe, args)


