"""
Simple Tester start a Webserv instance and send and properly formed http test requests on it.
"""

import http.client
import io
import subprocess
import time
from http.client import HTTPResponse

import simplerequest
import testutils as tu
from virtualtester import VirtualTester
from webserver import WebServer


class SimpleTester(VirtualTester):
    """
    Class to run a set of properly formed test requests on a single webserv instance.
    """

    def __init__(self, index, host, port):
        VirtualTester.__init__(self, index, host, port)

    def send_request(self, request):
        """
        Send request to the specified host.
        Returns the response.
        """
        try:
            conn = http.client.HTTPConnection(self.host, self.port)
            conn.request(request.method, request.path, request.content, request.headers)
            return conn.getresponse()
        except ConnectionRefusedError as e:
            print({e})
        return None
