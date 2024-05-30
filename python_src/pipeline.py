import subprocess
import build_utils as utils


def run():
    cat_process = subprocess.Popen(["cat", "./OUCHLMM2.incoming.packets"], stdout=subprocess.PIPE)
    list_symbols_process = subprocess.Popen(["out/list_symbols", "-"], stdin=cat_process.stdout, stdout=subprocess.PIPE)
    cat_process.stdout.close()
    output = list_symbols_process.communicate()[0]
    print(output.decode('utf-8'))



if __name__ == "__main__":
    run()
