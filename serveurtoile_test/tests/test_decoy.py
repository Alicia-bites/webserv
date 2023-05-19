import requests
import threading
import pytest
from time import sleep

def send_decoy():
    """
    Post 1_000_000 bytes to the server
    """
    requests.post('http://localhost:8080/', data='a' * 1_000_000)

@pytest.mark.quick
def test_resilience_decoy():
    """
    Test resilience against a decoy
    """
    # Start 10 decoy threads
    threads = [threading.Thread(target=send_decoy) for _ in range(10)]
    for thread in threads:
        thread.start()
    # Send a request
    for _ in range(10):
        response = requests.get('http://localhost:8080/')
        # Assert that the response is OK
        assert response.status_code == 200
        # Assert that the response is not empty
        assert response.text
    # Wait for the decoy threads to finish
    for thread in threads:
        thread.join()