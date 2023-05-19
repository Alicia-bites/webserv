import requests
import pytest
import hashlib
import os
from bs4 import BeautifulSoup
from glob import glob
from typing import List

def _md5(data: bytes) -> str:
    """
    Return the md5 of the data
    """
    return hashlib.md5(data).hexdigest()

def _tree() -> List[str]:
    output = []
    for path in glob("../front/**", recursive=True):
        if path.endswith("/"):
            continue
        # Trim the ../front part
        if os.path.isdir(path):
            continue
        path = path[8:]
        if '.php' in path:
            continue
        output.append(path)
    return output

@pytest.mark.quick
@pytest.mark.parametrize("path", _tree())
def test_static_get_content(caplog, path):
    """
    Dive recursively into the static folder (excludes CGI)
    """
    with caplog.at_level("DEBUG"):
        print("Testing path: " + path)
    nginx = requests.get("http://localhost:8081" + path)
    serveurtoile = requests.get("http://localhost:8080" + path)
    assert nginx.status_code == 200
    assert serveurtoile.status_code == 200
    assert _md5(nginx.content) == _md5(serveurtoile.content)

@pytest.mark.quick
@pytest.mark.parametrize("path", _tree())
def test_static_content_type(caplog, path):
    """
    Dive recursively into the static folder (excludes CGI)
    """
    with caplog.at_level("DEBUG"):
        print("Testing path: " + path)
    nginx = requests.get("http://localhost:8081" + path)
    serveurtoile = requests.get("http://localhost:8080" + path)
    assert nginx.status_code == 200
    assert serveurtoile.status_code == 200
    if path.endswith(".ttf"):
        return
    assert nginx.headers['Content-Type'] == serveurtoile.headers['Content-Type']

@pytest.mark.quick
@pytest.mark.parametrize("path", _tree())
def test_static_content_length(caplog, path):
    """
    Dive recursively into the static folder (excludes CGI)
    """
    with caplog.at_level("DEBUG"):
        print("Testing path: " + path)
    nginx = requests.get("http://localhost:8081" + path)
    serveurtoile = requests.get("http://localhost:8080" + path)
    assert nginx.status_code == 200
    assert serveurtoile.status_code == 200
    assert nginx.headers['Content-Length'] == serveurtoile.headers['Content-Length']
