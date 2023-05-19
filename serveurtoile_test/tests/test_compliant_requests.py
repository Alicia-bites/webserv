import requests
import pytest

"""
    We're not comparing with nginx as the implementation is different
"""

@pytest.mark.quick
def test_raw_request_headerless():
    headers = {}
    r = requests.get('http://localhost:8080/', headers=headers)
    assert r.status_code == 200

@pytest.mark.quick
def test_raw_request_header():
    headers = {'Host': 'localhost'}
    r = requests.get('http://localhost:8080/', headers=headers)
    assert r.status_code == 200

@pytest.mark.quick
def test_lambda_get():
    r = requests.get('http://localhost:8080/')
    assert r.status_code == 200

@pytest.mark.quick
def test_lambda_delete():
    r = requests.delete('http://localhost:8080/')
    assert r.status_code == 200

@pytest.mark.quick
def test_lambda_post():
    r = requests.post('http://localhost:8080/', data='hello')
    assert r.status_code == 200

@pytest.mark.quick
def test_unsupported_method():
    r = requests.put('http://localhost:8080/')
    assert r.status_code == 405 or r.status_code == 501 or r.status_code == 400

@pytest.mark.quick
def test_alive():
    try:
        r = requests.get("http://localhost:8080/")
        assert r.status_code == 200
    except requests.exceptions.ConnectionError:
        assert False

