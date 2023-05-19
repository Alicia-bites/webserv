import socket
import requests
import pytest

"""
    We're not comparing with nginx as the implementation is different
"""

@pytest.mark.quick
def test_raw_request_headerless():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('127.0.0.1', 8080))
    sock.sendall(b'GET / HTTP/1.1\r\n\r\n')
    data = sock.recv(1024)
    sock.close()
    assert b'HTTP/1.1 200' in data

@pytest.mark.quick
def test_raw_request_header():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('127.0.0.1', 8080))
    sock.sendall(b'GET / HTTP/1.1\r\nHost: localhost\r\n\r\n')
    data = sock.recv(1024)
    sock.close()
    assert b'HTTP/1.1 200' in data

@pytest.mark.quick
def test_invalid_request_line():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('127.0.0.1', 8080))
    sock.sendall(b'GET HTTP/1.1\r\nHost: localhost\r\n\r\n')
    data = sock.recv(1024)
    sock.close()
    assert b'HTTP/1.1 400' in data

@pytest.mark.quick
def test_invalid_request_line2():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('127.0.0.1', 8080))
    sock.sendall(b'\r\nHost: localhost\r\n\r\n')
    data = sock.recv(1024)
    sock.close()
    assert b'HTTP/1.1 400' in data

@pytest.mark.quick
def test_invalid_header_format():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('127.0.0.1', 8080))
    sock.sendall(b'GET / HTTP/1.1\r\nHost localhost\r\n\r\n')
    data = sock.recv(1024)
    sock.close()
    assert b'HTTP/1.1 400' in data

@pytest.mark.quick
def test_empty():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('127.0.0.1', 8080))
    sock.sendall(b'\r\n\r\n')
    data = sock.recv(1024)
    sock.close()
    assert b'HTTP/1.1 400' in data

@pytest.mark.quick
def test_duplicate_header():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('127.0.0.1', 8080))
    sock.sendall(b'GET / HTTP/1.1\r\nHost: localhost\r\nHost: localhost\r\n\r\n')
    data = sock.recv(1024)
    sock.close()
    assert b'HTTP/1.1 200' in data

@pytest.mark.quick
def test_lambda_get():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('127.0.0.1', 8080))
    sock.sendall(b'GET / HTTP/1.1\r\nHost: localhost\r\n\r\n')
    data = sock.recv(1024)
    sock.close()
    assert b'HTTP/1.1 200' in data

@pytest.mark.quick
def test_lambda_delete():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('127.0.0.1', 8080))
    sock.sendall(b'DELETE / HTTP/1.1\r\nHost: localhost\r\n\r\n')
    data = sock.recv(1024)
    sock.close()
    assert b'HTTP/1.1 200' in data

@pytest.mark.quick
def test_lambda_post():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('127.0.0.1', 8080))
    sock.sendall(b'POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 5\r\n\r\nhello')
    data = sock.recv(1024)
    sock.close()
    assert b'HTTP/1.1 200' in data

@pytest.mark.quick
def test_unsupported_method():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('127.0.0.1', 8080))
    sock.sendall(b'PUT / HTTP/1.1\r\nHost: localhost\r\n\r\n')
    data = sock.recv(1024)
    sock.close()
    assert b'HTTP/1.1 405' in data or b'HTTP/1.1 400' in data or b'HTTP/1.1 501' in data

@pytest.mark.quick
def test_alive():
    try:
        r = requests.get("http://localhost:8080/")
        assert r.status_code == 200
    except requests.exceptions.ConnectionError:
        assert False

