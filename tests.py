import os
import subprocess
import requests
import traceback
import hashlib
import shutil
from threading import Thread
from typing import Callable
from time import time, sleep

INSTANCE_PROCESS = None

def kill_instance():
    global INSTANCE_PROCESS
    if INSTANCE_PROCESS is not None:
        INSTANCE_PROCESS.kill()
        INSTANCE_PROCESS = None

def run_instance(config: str):
    global INSTANCE_PROCESS
    kill_instance()
    INSTANCE_PROCESS = subprocess.Popen(["./webserv", config], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    sleep(0.250) # wait for the server to start

def instance_alive() -> bool:
    global INSTANCE_PROCESS
    if INSTANCE_PROCESS is None:
        return False
    return INSTANCE_PROCESS.poll() is None

def test_root(root_dir: str, base_url: str, cgi_exec: bool, skip_timeout: bool = True) -> int:
    sent_requests = 1
    for root, dirs, files in os.walk(root_dir):
        for file in files:
            path = os.path.join(root, file)
            if skip_timeout and "timeout.php" in path:
                continue
            req_path = path.replace(root_dir, "")
            url = base_url + req_path
            r = requests.get(url)
            sent_requests += 1
            assert r.status_code == 200, f"Failed to get {url}"
            with open(path, "rb") as f:
                checksum = hashlib.sha256(f.read()).hexdigest()
            if path.endswith(".php") and cgi_exec:
                assert checksum != hashlib.sha256(r.content).hexdigest(), f"CGI script {url} was not executed"
            else:
                assert checksum == hashlib.sha256(r.content).hexdigest(), f"Checksum mismatch for {url}"
    return sent_requests

def test_static_base() -> int:
    CONFIG = "./test_configs/basic/static_base"
    run_instance(CONFIG)
    sent_requests = test_root("./front", "http://127.0.0.1:7777", False, False)
    return sent_requests

def test_static_cgi() -> int:
    CONFIG = "./test_configs/basic/static_cgi"
    run_instance(CONFIG)
    sent_requests = test_root("./front", "http://127.0.0.1:7777", True, True)
    return sent_requests

def test_static_root_diff():
    CONFIG = "./test_configs/basic/static_2_srvs_root_diff"
    run_instance(CONFIG)
    sent_requests = test_root("./front", "http://127.0.0.1:7777", False, False)
    sent_requests += test_root("./srcs", "http://localhost:7777", False, False)
    return sent_requests

def test_static_root_identical():
    CONFIG = "./test_configs/basic/static_2_srvs_root_same"
    run_instance(CONFIG)
    sent_requests = test_root("./front", "http://127.0.0.1:7777", False, False)
    sent_requests += test_root("./front", "http://localhost:7777", False, False)
    return sent_requests

def test_methods():
    CONFIG = "./test_configs/basic/methods"
    run_instance(CONFIG)
    tests = {
        "/test_GET": ["GET"],
        "/test_POST": ["POST"],
        "/test_DELETE": ["DELETE"],
        "/test_GET_POST": ["GET", "POST"],
        "/test_GET_DELETE": ["GET", "DELETE"],
    }
    sent_requests = 0
    ALL_METHODS = ["GET", "POST", "DELETE"]
    for url, methods in tests.items():
        for method in methods:
            # test allowed methods and disallowed methods (must return 200 or 405)
            r = requests.request(method, f"http://127.0.0.1:7777{url}")
            sent_requests += 1
            assert r.status_code != 405, f"Method {method} disallowed for {url}"
            # test disallowed methods (must return 405)
            for disallowed_method in [m for m in ALL_METHODS if m not in methods]:
                r = requests.request(disallowed_method, f"http://127.0.0.1:7777{url}")
                sent_requests += 1
                assert r.status_code == 405, f"Method {disallowed_method} allowed for {url}"
    return sent_requests

def test_static_no_read():
    CONFIG = "./test_configs/advanced/no_read_static"
    try:
        os.makedirs("/tmp/test", exist_ok=True)
        os.chmod("/tmp/test", 0o000)
    except:
        return 0
    run_instance(CONFIG)
    try:
        r = requests.get("http://127.0.0.1:7777/")
        assert r.status_code == 403, "Server did not return 403 for /"
    except requests.exceptions.ConnectionError:
        assert False, "Invalid server response"
    try:
        r = requests.get("http://127.0.0.1:7777/")
        assert r.status_code == 403, "Server did not return 403 for /"
    except requests.exceptions.ConnectionError:
        assert False, "Invalid server response"
    return 2

def test_invalid_port():
    run_instance("./test_configs/advanced/invalid_port")
    assert not instance_alive(), "Server did not exit on invalid port"
    run_instance("./test_configs/advanced/invalid_port2")
    assert not instance_alive(), "Server did not exit on invalid port"
    run_instance("./test_configs/advanced/invalid_port3")
    assert not instance_alive(), "Server did not exit on invalid port"
    run_instance("./test_configs/advanced/invalid_port4")
    assert not instance_alive(), "Server did not exit on invalid port"
    run_instance("./test_configs/advanced/invalid_port5")
    assert not instance_alive(), "Server did not exit on invalid port"
    run_instance("./test_configs/advanced/invalid_port6")
    assert not instance_alive(), "Server did not exit on invalid port"
    return 0

def test_forbidden_port():
    run_instance("./test_configs/advanced/disallowed_port")
    assert not instance_alive(), "Server did not exit on forbidden port"
    # run nc to listen on port 65475
    process = subprocess.Popen(["nc", "-l", "65475"])
    try:
        run_instance("./test_configs/advanced/disallowed_port2")
        assert not instance_alive(), "Server did not exit on forbidden port"
    finally:
        process.terminate()
    return 0

def test_cgi_no_exec():
    run_instance("./test_configs/advanced/cgi_no_exec")
    if os.path.exists("/tmp/no_exec_webserv_test"):
        os.remove("/tmp/no_exec_webserv_test")
    with open("/tmp/no_exec_webserv_test", "w+") as f:
        f.write("#!/bin/bash\necho \"Content-type: text/html\"\necho \"\"\necho \"<html><body><h1>TEST</h1></body></html>\"")
    os.chmod("/tmp/no_exec_webserv_test", 0o000)
    try:
        r = requests.get("http://127.0.0.1:7777/test.php")
        assert r.status_code >= 500 and r.status_code <= 599, "Server did not return a server error (5xx)"
    except requests.exceptions.ConnectionError:
        assert False, "Invalid server response"
    return 1

def test_invalid_cgi_behavior():
    run_instance("./test_configs/advanced/cgi_invalid_behavior")
    if os.path.exists("/tmp/no_exec_webserv_test"):
        os.remove("/tmp/no_exec_webserv_test")
    with open("/tmp/no_exec_webserv_test", "w+") as f:
        f.write("#!/bin/bash\necho pomme de terre") # does not return a CGI response
    os.chmod("/tmp/no_exec_webserv_test", 0o744) # make executable
    try:
        r = requests.get("http://127.0.0.1:7777/test.php")
        assert r.status_code >= 500 and r.status_code <= 599, "Server did not return a server error (5xx)"
    except requests.exceptions.ConnectionError:
        assert False, "Invalid server response"
    return 1

def test_delete_scope():
    run_instance("./test_configs/advanced/delete_scope")
    os.makedirs("./test_upload", exist_ok=True)
    with open("./test_upload/test", "w+") as f:
        f.write("test")
    with open("./test", "w+") as f: # this file should not be deleted
        f.write("test")
    r = requests.delete("http://127.0.0.1:7777/test_upload/test") # must delete ./test_upload/test
    assert r.status_code >= 200 and r.status_code <= 299, "Server did not return a success (2xx) (got {})".format(r.status_code)
    assert not os.path.exists("./test_upload/test"), "Server did not delete './test_upload/test'"
    assert os.path.exists("./test"), "Server deleted './test' (file is outside upload dir)"
    os.makedirs("./test_upload/test", exist_ok=True)
    r = requests.delete("http://127.0.0.1:7777/test_upload/test") # must delete ./test_upload/test (dir)
    assert r.status_code >= 200 and r.status_code <= 299, "Server did not return a success (2xx) (got {})".format(r.status_code)
    assert not os.path.exists("./test_upload/test"), "Server did not delete './test_upload/test' (dir)"
    r = requests.delete("http://127.0.0.1:7777/test") # mustn't delete ./test
    assert r.status_code >= 400 and r.status_code <= 499, "Server did not return a client error (4xx) (got {})".format(r.status_code)
    assert os.path.exists("./test"), "Server deleted './test' (file is outside upload dir)"
    os.remove("./test")
    # shutil.rmtree("./test_upload")
    return 3

def test_upload_no_perms():
    run_instance("./test_configs/advanced/upload_no_perm")
    shutil.rmtree("./tmptmptestnoperm", ignore_errors=True)
    os.makedirs("./tmptmptestnoperm", exist_ok=True)
    os.chmod("./tmptmptestnoperm", 0o000)
    with open("tmptmp", "w+") as f:
        f.write("test")
    r = requests.post("http://127.0.0.1:7777/tmptmptestnoperm", files={"file": open("tmptmp", "rb")})
    os.remove("tmptmp")
    shutil.rmtree("./tmptmptestnoperm", ignore_errors=True)
    assert r.status_code >= 400 and r.status_code <= 499, "Server did not return a client error (4xx) (got {})".format(r.status_code)
    assert instance_alive(), "Server exited on upload with no permissions"
    return 1

def test_upload_no_dir():
    run_instance("./test_configs/advanced/upload_no_dir")
    shutil.rmtree("./tmptmptestnothere", ignore_errors=True)
    with open("tmptmp", "w+") as f:
        f.write("test")
    r = requests.post("http://127.0.0.1:7777/tmptmptestnothere", files={"file": open("tmptmp", "rb")})
    os.remove("tmptmp")
    # file should exist in tmptmptestnothere/tmptmp
    assert r.status_code >= 200 and r.status_code <= 299, "Server did not return a success (2xx) (got {})".format(r.status_code)
    assert os.path.exists("./tmptmptestnothere/tmptmp"), "Server did not create file in './tmptmptestnothere/tmptmp'"
    assert open("./tmptmptestnothere/tmptmp", "r").read() == "test", "Server did not create file in './tmptmptestnothere/tmptmp' (file content is invalid)"
    shutil.rmtree("./tmptmptestnothere", ignore_errors=True)
    return 1

def test_server_name_unreachable():
    run_instance("./test_configs/advanced/server_name_unreachable")
    r = requests.get("http://localhost:7777/") # serves front
    r2 = requests.get("http://127.0.0.1:7777/") # serves /tmp

    assert r.status_code == 200
    assert r2.status_code == 200
    assert r.text != r2.text

    assert "Index of" in r2.text # /tmp directory listing should be served
    assert "<html>" in r.text    # index.html should've been served

    return 2

def test_empty_dir_index():
    run_instance("./test_configs/advanced/directory_index_empty")
    shutil.rmtree("/tmp/testtestempty", ignore_errors=True)
    os.makedirs("/tmp/testtestempty", exist_ok=True)
    r = requests.get("http://127.0.0.1:7777")
    assert r.status_code == 200
    assert "Index of" in r.text
    shutil.rmtree("/tmp/testtestempty", ignore_errors=True)
    return 1


def try_test(fn: Callable, name: str):
    try:
        start = time()
        print(f"{name} passed ({fn()} requests sent) in {time() - start} seconds")
    except KeyboardInterrupt:
        exit(1)
    except:
        print(f"{name} failed")
        traceback.print_exc()

if __name__ == "__main__":
    # try_test(test_static_base, "Test static base")
    # try_test(test_static_cgi, "Test static CGI")
    # try_test(test_static_root_diff, "Test double server root difference")
    # try_test(test_static_root_identical, "Test double server root identical")
    # try_test(test_methods, "Test allowed / disallowed methods")
    # try_test(test_static_no_read, "Test static no read")
    # try_test(test_invalid_port, "Test invalid port")
    # try_test(test_forbidden_port, "Test forbidden port")
    # try_test(test_cgi_no_exec, "Test CGI no exec")
    # try_test(test_invalid_cgi_behavior, "Test invalid CGI behavior")
    # try_test(test_delete_scope, "Test delete scope")
    try_test(test_upload_no_perms, "Test upload no perms")
    try_test(test_upload_no_dir, "Test upload no dir")
    # try_test(test_server_name_unreachable, "Test server name unreachable")
    # try_test(test_empty_dir_index, "Test empty dir index")
    kill_instance()