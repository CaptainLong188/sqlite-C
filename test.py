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

# Test program behavior
def test1():

    commands = ["insert 1 user user@gmail.com ", "select", ".exit"]
    to_match = ["db > Statement executed successfully.", "db > (1, user, user@gmail.com)", "Statement executed successfully.", "db >"]
    assert(runScript(commands) == to_match)

# Test that it's not possible to insert more than TABLE_MAX_ROWS
def test2():
    commands = [f"insert {i} for person{i} email{1}@gmail.com" for i in range(1, 1402)]
    commands.append(".exit")
    to_match = "db > Error: Table full."
    assert(runScript(commands)[-2] == to_match)

# Test that it's possible to insert strings of the maximum length
def test3():
    long_username = "a"*32
    long_email = "a"*255
    commands = [f"insert 1 {long_username} {long_email}", "select", ".exit"]
    to_match = ["db > Statement executed successfully.", f"db > (1, {long_username}, {long_email})", "Statement executed successfully.", "db >"]
    assert(runScript(commands) == to_match)

# Test that it's not possible to insert strings that are too long
def test4():
    long_username = "a"*33
    long_email = "a"*256
    commands = [f"insert 1 {long_username} {long_email}", ".exit"]
    to_match = "db > Error : String is too long."
    assert(runScript(commands)[-2] == to_match)

# Test that it's not possible to insert a negative id
    commands = [f"insert -1 user user@gmail.com", ".exit"]
    to_match = "db > Error: ID must be positive."
    assert(runScript(commands)[-2] == to_match)

if __name__ == "__main__":
    pass