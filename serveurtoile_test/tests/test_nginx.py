import requests
import pytest
from bs4 import BeautifulSoup

"""
    Pre-requisite tests for nginx
"""

@pytest.mark.order(1)
@pytest.mark.prereq
@pytest.mark.quick
def test_nginx_up():
    r = requests.get("http://localhost:8081")
    assert r.status_code == 200
    assert "serveurtoile" in r.text

@pytest.mark.order(2)
@pytest.mark.prereq
@pytest.mark.quick
def test_nginx_title():
    r = requests.get("http://localhost:8081")
    assert r.status_code == 200
    soup = BeautifulSoup(r.text, 'html.parser')
    assert soup.find("title").text.encode("unicode-escape") == b"\\xf0\\x9f\\x95\\xb8\\xef\\xb8\\x8f serveurtoile \\xf0\\x9f\\x95\\xb8\\xef\\xb8\\x8f"

@pytest.mark.order(3)
@pytest.mark.prereq
@pytest.mark.quick
def test_nginx_header():
    r = requests.get("http://localhost:8081")
    assert r.status_code == 200
    assert "nginx" in r.headers['Server']

