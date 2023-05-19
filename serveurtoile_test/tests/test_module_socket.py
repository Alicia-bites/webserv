import pytest
import subprocess
import os

BINARY_PATH = "./socket_test"

def _memcheck(path: str, test_index: int) -> int:
    """
    Executes the path, returns :
    - 0 if no errors whatsoever
    - 1 if there are memory issues but the test passed
    - 2 if there are memory issues and the test failed
    - 9 the test doesn't exist (subprocess returned 2)

    Uses popen to execute the command, to get output and return code.
    Uses the flag -fsanitize=address to check for memory issues.
    """
    ret_code = -1
    proc = subprocess.Popen([path, str(test_index)], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    output, err = proc.communicate()
    output += err
    output = output.decode("utf-8")
    ret_code = proc.returncode

    # Check for test existence
    if ret_code == 2:
        return 9
    # Check for memory issues
    if "ERROR:" in output:
        return 1 if ret_code == 0 else 2
    elif ret_code == 0:
        return 0
    elif ret_code == 1:
        return 1
    else:
        return -1

def _test_count(path: str = "./tests/socket/main.cpp") -> int:
    """
    Counts the number of test cases, since main.cpp files should
    have switch cases for each test, this function counts the number of
    cases and returns it.
    """
    with open(path, "r") as f:
        data = f.read()
        return data.count("case ")

@pytest.mark.slow
@pytest.mark.parametrize("test_index", list(range(1, _test_count() + 1)))
def test_socket_module_full(test_index):
    """
    Checks for static build errors, bugs, etc.
    Checks for runtime errors, memory leaks and other bugs.
    Runs all tests in the main routine.
    """
    os.chdir("tests/socket")
    if not os.path.exists(BINARY_PATH):
        try:
            subprocess.check_call(["make", "scan-build"], stderr=subprocess.DEVNULL, stdout=subprocess.DEVNULL)
        except subprocess.CalledProcessError:
            os.chdir("../../")
            raise Exception("Failed to compile socket module")

    # Run all tests (if return code is 9, abort the test)
    try:
        output = _memcheck(BINARY_PATH, test_index)
    except subprocess.CalledProcessError:
        raise Exception("Failed to execute socket module")
    finally:
        os.chdir("../../") # Go back to root directory
    if output == 9:
        pytest.skip("Test doesn't exist")
    elif output == 2:
        raise Exception("Test failed : Memory issues were detected, test failed")
    elif output == 1:
        raise Exception("Test failed : Test failed without memory issues")
    elif output == 0:
        return
    else:
        raise Exception("Test failed : Unknown error")
