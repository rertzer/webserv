"""
Functions for webserv testing
"""

import http.client
import subprocess
import time

from webserver import WebServer


def test_cmd(params):
    """
    Test cmd line generic function.
    """
    try:
        server = WebServer(params)
    except RuntimeError as e:
        print({e})
        return 1
    try:
        server.proc.wait(timeout=4)
    except subprocess.TimeoutExpired:
        server.finish()
    return server


def test_request_start(serv):
    """
    Start webserv to test requests.
    """
    try:
        server = WebServer(serv)
        time.sleep(2)
    except RuntimeError as e:
        print({e})
        return None
    return server


def test_request_end(server):
    """
    Close the server connection.
    Returns the server object.
    """
    try:
        server.proc.wait(timeout=4)
    except subprocess.TimeoutExpired:
        server.finish()
    return server


def send_request(host, port, method, path, head):
    """
    send request to the specified host.
    returns the response.
    """
    try:
        conn = http.client.HTTPConnection(host, port)
        conn.request(method, path, headers=head)
        return conn.getresponse()
    except ConnectionRefusedError as e:
        print({e})
    return None
