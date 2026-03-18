"""
Functions for webserv testing
"""

import http.client
import io
import os
import socket
import subprocess
import sys
import time
from http.client import HTTPResponse
from http.cookies import SimpleCookie
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
    Test webserv command line generic function.
    """
    try:
        server = WebServer(params)
    except RuntimeError as e:
        print({e})
        return None
    try:
        server.proc.wait(timeout=4)
    except subprocess.TimeoutExpired:
        server.finish()
    return server


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


def read_content(resp):
    """
    Reads and return Response content
    """
    content = b""
    while True:
        chunk = resp.read(4096)
        if not chunk:
            break
        content += chunk

    return content


def check_res_with_cookies(version, res, status, length, cookies):
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
        and test_cookies(res, cookies)
        and len(res.read()) == length
    ):
        ok = True
    print(f"test_request_{version}", okko(ok))
    return ok


def test_cookies(resp, cookies):
    """
    Test the receiced cookies
    """
    ok = True
    resp_cookies = get_cookies(resp)
    if len(resp_cookies) != len(cookies):
        return False
    for cookie in cookies:
        if cookie[0] not in resp_cookies.keys():
            ok = False
            break
        if resp_cookies[cookie[0]].value != cookie[1]:
            ok = False
            break
        for k, v in cookie[2].items():
            if k.lower() not in resp_cookies[cookie[0]].keys():
                ok = False
                break
            if v != resp_cookies[cookie[0]][k.lower()]:
                ok = False
                break
        if ok is False:
            break

    return ok


def get_cookies(resp):
    """
    Retrieve cookies from the server response.
    """
    cookies = SimpleCookie()
    for header, value in resp.getheaders():
        if header.lower() == "set-cookie":
            cookies.load(value)
    return cookies


def test_post_request(index, host, port, path, test):
    """
    Tests a properly formed HTTP  POST request
    """

    res = send_post_request(host, port, path, test[1], test[0])
    return check_res(f"3.{index}", res, test[2], test[3])


def test_raw_with_cookies(index, host, port, t):
    """
    Sends raw request req on socket sock and checks the
    response for status stat and content-length length and cookies.
    returns 1 or 0.
    """
    raw_res = send_raw(host, port, t[0])
    # print(raw_res.decode(errors="replace"))
    return check_res_with_cookies(
        f"{index}", raw_to_http_response(raw_res), t[1], t[2], t[3]
    )


def get_kitty_content(filename):
    """
    format the file `filename` as a POST request body
    """
    boundary = "----kittyBoundary1234"
    _, extension = filename.rsplit(".", 1)

    with open(filename, "rb") as f:
        file_data = f.read()

        body = (
            (
                f"--{boundary}\r\n"
                f'Content-Disposition: form-data; name="file"; filename="{os.path.basename(filename)}"\r\n'
                f"Content-Type: image/{extension}\r\n"
                "\r\n"
            ).encode()
            + file_data
            + (f"--{boundary}--\r\n").encode()
        )
    return body, boundary


def get_wrong_kitty_content(filename):
    """
    format the file `filename` as a POST request body with a boundary mistake
    """
    boundary_1 = "----kittyBoundary1234"
    boundary_2 = "----kittyBoundary5678"
    _, extension = filename.rsplit(".", 1)

    with open(filename, "rb") as f:
        file_data = f.read()

        body = (
            (
                # f"--{boundary_1}\r\n"
                f'Content-Disposition: form-data; name="file"; filename="{os.path.basename(filename)}"\r\n'
                f"Content-Type: image/{extension}\r\n"
                "\r\n"
            ).encode()
            + file_data
            + (f"--{boundary_2}--\r\n").encode()
        )
    return body, boundary_1
