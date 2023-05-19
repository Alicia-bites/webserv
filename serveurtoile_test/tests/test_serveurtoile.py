import requests
import pytest
from bs4 import BeautifulSoup

"""
    Pre-requisite tests for serveurtoile
"""

@pytest.mark.order(4)
@pytest.mark.prereq
def test_serveurtoile_up():
    r = requests.get("http://localhost:8080")
    assert r.status_code == 200
    assert "serveurtoile" in r.text

@pytest.mark.order(5)
@pytest.mark.prereq
def test_serveurtoile_title():
    r = requests.get("http://localhost:8080")
    assert r.status_code == 200
    soup = BeautifulSoup(r.text, 'html.parser')
    assert soup.find("title").text.encode("unicode-escape") == b"\\xf0\\x9f\\x95\\xb8\\xef\\xb8\\x8f serveurtoile \\xf0\\x9f\\x95\\xb8\\xef\\xb8\\x8f"

@pytest.mark.order(6)
@pytest.mark.prereq
def test_serveurtoile_header():
    r = requests.get("http://localhost:8080")
    assert r.status_code == 200
    assert "serveurtoile" in r.headers['Server']
