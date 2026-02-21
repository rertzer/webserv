"""
Functions for webserv testing
"""

import http.client
import subprocess
import time
from http.client import HTTPResponse

from colors import Color
from webserver import WebServer


def okko(val):
    """
    return OK or KO string according to the boolean value received as argument
    """

    return Color.GREEN + "OK" + Color.ENDC if val else Color.RED + "KO" + Color.ENDC


def tester(fun):
    """
    run the fun test, prints infos and return 1 on success, 0 else.
    """
    passed, total = fun()
    print(fun.__name__, okko(passed == total))
    return (passed, total)


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
        time.sleep(1)
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
        server.proc.wait(timeout=1)
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


def check_res(version, res, status, length):
    """
    Takes a string, an HTTPRespnse, and two int as arguments.
    check that response has the right status and content length
    return True or False
    """
    ok = False
    if (
        isinstance(res, HTTPResponse)
        and res.status == status
        and res.getheader("Content-Length") == str(length)
    ):
        ok = True
    print(f"test_request_{version}", okko(ok))
    return ok
