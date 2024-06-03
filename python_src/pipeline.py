import subprocess
import os
import tempfile


def run_pipeline(pipeline):
    processes = []
    l = len(pipeline)
    pipes = [os.pipe() for _ in range(l - 1)]

    temp_file = tempfile.NamedTemporaryFile(delete=False, buffering=10*1024)

    for i, (target, args) in enumerate(pipeline):
        cmd = [target, *args]

        write_fd = pipes[i][1] if i < l - 1 else temp_file
        read_fd = pipes[i - 1][0] if i > 0 else None

        proc = subprocess.Popen(
            cmd,
            stdout=write_fd,
            stdin=read_fd
        )
        processes.append(proc)

    for read_fd, write_fd in pipes:
        os.close(read_fd)
        os.close(write_fd)

    rets = []
    for proc in processes:
        proc.wait()
        rets.append(proc.returncode)

    name = temp_file.name
    temp_file.close()

    with open(name, 'r') as f:
        out = f.read()

    return rets, out

