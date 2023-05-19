import os
import pytest

BINARIES = [
	"tests/http/http_test",
	"tests/config/config_test",
	"tests/socket/socket_test",
]

def pytest_sessionstart(session):
	if os.path.basename(os.getcwd()) != "serveurtoile_test":
		pytest.exit("You must run the tests from the root of the project")
	for binary in BINARIES:
		if os.path.exists(binary):
			os.remove(binary)
