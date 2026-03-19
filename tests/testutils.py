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
