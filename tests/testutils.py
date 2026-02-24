"""
Functions for webserv testing
"""

import http.client
import io
import socket
import subprocess
import time
from http.client import HTTPResponse
from io import BytesIO

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
    return None


def send_post_request(host, port, path, params, headers):
    """
    send a postrequest to the specified host.
    returns the response.
    """
    try:
        conn = http.client.HTTPConnection(host, port)
        conn.request("POST", path, params, headers)
        return conn.getresponse()
    except ConnectionRefusedError as e:
        print({e})
    return None


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


def test_request(index, host, port, test):
    """
    Tests a properly formed HTTP request
    """

    res = send_request(host, port, test[0], test[1], {"Host": test[4]})
    return check_res(f"1.{index}", res, test[2], test[3])


def test_post_request(index, host, port, path, test):
    """
    Tests a properly formed HTTP  POST request
    """

    res = send_post_request(host, port, path, test[1], test[0])
    return check_res(f"3.{index}", res, test[2], test[3])


def test_raw(index, host, port, t):
    """
    Sends raw request req on socket sock and checks the
    response for status stat and content-length length.
    returns 1 or 0.
    """
    raw_res = send_raw(host, port, t[0])
    # print(raw_res.decode(errors="replace"))
    return check_res(f"{index}", raw_to_http_response(raw_res), t[1], t[2])


def send_raw(host, port, raw):
    """
    Sends raw request via a socket
    """
    with socket.create_connection((host, port)) as sock:
        sock.sendall(raw)
        raw_res = sock.recv(4096)
    return raw_res


class FakeSocket(BytesIO):
    """
    file like object
    """

    def makefile(self, *args, **kwargs):
        """
        Magic
        """
        return self


def raw_to_http_response(raw: bytes) -> HTTPResponse:
    """
    Converts the response from raw socket to an HTTPResponse object
    """
    sock = FakeSocket(raw)
    response = HTTPResponse(sock)
    response.begin()
    return response


def check_server_output(server):
    """
    handle server stdout, stderr, status
    """
    assert isinstance(server, WebServer)
    assert isinstance(server.proc.stdout, io.TextIOWrapper)
    assert isinstance(server.proc.stderr, io.TextIOWrapper)
    output = server.proc.stdout.read()
    outerror = server.proc.stderr.read()
    ret = server.proc.returncode
    print(f"RETURN|{ret}|{output}|{outerror}|\n")
