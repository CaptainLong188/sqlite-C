import pytest
import subprocess

def runScript(commands):
    
    process = subprocess.Popen(
        [r".\db"],
        stdin=subprocess.PIPE,  # Redirects the stdin so we can send data
        stdout=subprocess.PIPE, # Captures the stdout of the process so we can process it
        stderr=subprocess.PIPE, # Captures the stderr output of the subprocess
        text=True,
    )

    data_to_send = "\n".join(commands) + "\n"
    raw_output = ""

    # Write and read data 
    try:
        raw_output, _ = process.communicate(input = data_to_send, timeout=15) 
    except TimeoutError:
        process.kill()
        outs, errs = process.communicate()
        print(outs, errs)

    # Split output into lines and return
    return raw_output.strip().split("\n")

def test1():

    commands = ["insert 1 cesar cgiraldov@uni.pe ", "select", ".exit"]
    to_match = ["db > Statement executed successfully.", "db > (1, cesar, cgiraldov@uni.pe)", "Statement executed successfully.", "db >"]

    assert(runScript(commands) == to_match)

def test2():
    commands = [f"insert {i} for person{i} email{1}@gmail.com" for i in range(1, 1402)]
    commands.append(".exit")
    to_match = "db > Error: Table full."

    assert(runScript(commands)[-2] == to_match)

if __name__ == "__main__":

    test1()
    test2()