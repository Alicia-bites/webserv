import requests
import pytest
import hashlib
import random
from bs4 import BeautifulSoup
from typing import Union, Tuple
from multiprocessing import Pool

SEED = random.randint(0, 99999999)

def _compare_contains(needle: str, haystack: str) -> bool:
    return needle in haystack

def _compare_equal(s1: str, s2: str) -> bool:
    return s1 == s2

def _compare_port(port: Union[int, str]) -> bool:
    if isinstance(port, str):
        port = int(port)
    return 0 < port < 65536

def _compare_nginx(cgi_var: str, serveurtoile: str, serveurtoile_hash: str, advanced: bool = False) -> bool:
    """
    Performs a GET request on nginx's endpoint and compares the CGI variable
    to serveurtoile's
    """
    url = "http://127.0.0.1:8081/cgi_var_test/{}.php?toto=1?to=1&tata=1#bonjour" if advanced else "http://127.0.0.1:8081/cgi_var_test/{}.php?toto=1"
    r = requests.get(
        # Add query string and fragment to check if they are handled correctly
        url.format(
            cgi_var,
        )
    )
    soup = BeautifulSoup(r.text, "html.parser")
    data = soup.find("h1", {"id": cgi_var}).text
    value_identical = data == serveurtoile
    hash_identical = hashlib.sha256(r.text.encode()).hexdigest() == serveurtoile_hash
    assert value_identical, "serveurtoile's metavar '{}' is not the same as nginx's ('{}' != '{}')".format(cgi_var, serveurtoile, data)
    assert hash_identical, "serveurtoile's response is not the same as nginx's"

def _fetch_serveurtoile(cgi_var: str, advanced: bool = False) -> Tuple[str, str]:
    url = "http://127.0.0.1:8080/cgi_var_test/{}.php?toto=1?to=1&tata=1#bonjour" if advanced else "http://127.0.0.1:8080/cgi_var_test/{}.php?toto=1"
    r = requests.get(
        # Add query string and fragment to check if they are handled correctly
        url.format(
            cgi_var,
        )
    )
    try:
        soup = BeautifulSoup(r.text, "html.parser")
        return hashlib.sha256(r.text.encode()).hexdigest(), soup.find("h1", {"id": cgi_var}).text
    except AttributeError:
        raise AssertionError("serveurtoile's CGI response is not valid")

CGI_VARS = {
    "SERVER_SOFTWARE":   ("serveurtoile", _compare_contains), # must contains
    "GATEWAY_INTERFACE": ("CGI/1.1",      _compare_equal),    # must be equal
    "SERVER_PROTOCOL":   ("HTTP/1.1",     _compare_equal),    # must be equal
    "SERVER_PORT":       ("custom_port",  _compare_port),     # custom check
    "REQUEST_METHOD":    ("nginx",        _compare_nginx),    # same as nginx
    "PATH_INFO":         ("nginx",        _compare_nginx),    # same as nginx
    "PATH_TRANSLATED":   ("nginx",        _compare_nginx),    # same as nginx
    "SCRIPT_NAME":       ("nginx",        _compare_nginx),    # same as nginx
    "QUERY_STRING":      ("nginx",        _compare_nginx),    # same as nginx
    "CONTENT_TYPE":      ("nginx",        _compare_nginx),    # same as nginx
    "CONTENT_LENGTH":    ("nginx",        _compare_nginx),    # same as nginx
    # "SERVER_NAME": "nginx",          # untested yet
    # "REMOTE_HOST": "nginx",          # untested yet (docker diff)
    # "REMOTE_ADDR": "nginx",          # untested yet (docker diff)
    # "AUTH_TYPE": "nginx",            # not supported
    # "REMOTE_USER": "nginx",          # not supported
    # "REMOTE_IDENT": "nginx",         # not supported
}

@pytest.mark.parametrize("cgi_var", CGI_VARS)
def test_cgi_variables(cgi_var: str):
    # get serveurtoile's response
    response_hash, serveurtoile = _fetch_serveurtoile(cgi_var)

    # get the function to compare
    fn = CGI_VARS[cgi_var][1]

    # run comparison
    if fn == _compare_nginx:
        fn(cgi_var, serveurtoile, response_hash), "serveurtoile's metavar '{}' is not the same as nginx's".format(cgi_var)
    elif fn == _compare_port:
        assert fn(serveurtoile), "serveurtoile's metavar '{}' is not a valid port".format(cgi_var)
    elif fn == _compare_contains:
        assert fn(CGI_VARS[cgi_var][0], serveurtoile), "serveurtoile's metavar '{}' does not contain '{}'".format(cgi_var, CGI_VARS[cgi_var][0])
    elif fn == _compare_equal:
        assert fn(CGI_VARS[cgi_var][0], serveurtoile), "serveurtoile's metavar '{}' is not equal to '{}'".format(cgi_var, CGI_VARS[cgi_var][0])


@pytest.mark.parametrize("size", range(1, 8192, 8))
def test_cgi_body(size: int):
    serveurtoile = requests.get("http://127.0.0.1:8080/body_generator.php?size={}&seed={}".format(size, SEED)).text
    nginx = requests.get("http://127.0.0.1:8081/body_generator.php?size={}&seed={}".format(size, SEED)).text
    assert serveurtoile == nginx, "serveurtoile's CGI failed to handle a body of size {}".format(size)

def _stress_cgi(size: int) -> bool:
    serveurtoile = requests.get("http://127.0.0.1:8080/body_generator.php?size={}&seed={}".format(size, SEED)).text
    nginx = requests.get("http://127.0.0.1:8081/body_generator.php?size={}&seed={}".format(size, SEED)).text
    return serveurtoile == nginx

@pytest.mark.parametrize("size", [1024, 2048, 4096, 8192, 16384])
def test_cgi_stress(size: int):
    assert _stress_cgi(size) == True, "serveurtoile's CGI failed to handle a stress of size {}".format(size)
    with Pool(4) as pool:
        assert all(pool.map(_stress_cgi, [size] * 150)) == True, "serveurtoile's CGI failed to handle a stress of size {}".format(size)

