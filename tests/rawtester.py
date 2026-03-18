"""
RawTester is used to test not properly formated requests.

"""

import http.client
import io
import socket
import subprocess
import sys
import time
from http.client import HTTPResponse

import simplerequest
import testutils as tu
from virtualtester import VirtualTester
from webserver import WebServer


class RawTester(VirtualTester):
    """
    Tests requests in raw format
    """

    def __init__(self, index, host, port):
        VirtualTester.__init__(self, index, host, port)

    def send_request(self, request):
        """
        Sends raw request via a socket
        """
        with socket.create_connection((request.host, request.port)) as sock:
            sock.sendall(request.raw)
            raw_res = b""
            try:
                while True:
                    chunk = sock.recv(4096)
                    if not chunk or chunk == b"":
                        break
                    raw_res += chunk
            except ConnectionResetError as e:
                print("Reset Error", e, file=sys.stderr)
        return raw_res

    def test_request(self, index, request):
        """
        Sends raw request req on socket sock and checks the
        response for status stat and content-length length.
        returns 1 or 0.
        """
        request.index = index
        raw_res = self.send_request(request)
        return self.check_resp(request, tu.raw_to_http_response(raw_res))
