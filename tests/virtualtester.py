"""
Virtual Tester start a Webserv instance and send and test requests on it.
"""

import http.client
import io
import subprocess
import time
from http.client import HTTPResponse
from http.cookies import SimpleCookie

import simplerequest
import testutils as tu
from webserver import WebServer


class VirtualTester:
    """
    Class to run a set of tests on a single webserv instance.
    """

    def __init__(self, index, host, port):
        self.index = index
        self.host = host
        self.port = port
        self.server = None

    def start_server(self, conf_file):
        """
        Start webserv with conf_file as argument.
        """
        try:
            self.server = WebServer(conf_file)
            time.sleep(1)
        except RuntimeError as e:
            print({e})

    def stop_server(self):
        """
        Close the server connection.
        """
        assert self.server is not None
        try:
            self.server.proc.wait(timeout=1)
        except subprocess.TimeoutExpired:
            self.server.finish()

    def check_server_output(self):
        """
        Print server stdout, stderr, status
        """
        assert isinstance(self.server, WebServer)
        assert isinstance(self.server.proc.stdout, io.TextIOWrapper)
        assert isinstance(self.server.proc.stderr, io.TextIOWrapper)
        output = self.server.proc.stdout.read()
        outerror = self.server.proc.stderr.read()
        ret = self.server.proc.returncode
        print(f"RETURN|{ret}|{output}|{outerror}|\n")

    def run_requests(self, requests):
        """
        run requests given as argument and returns number of success.
        """
        return sum(self.test_request(i + 1, t) for i, t in enumerate(requests))

    def test_request(self, index, request):
        """
        Tests a properly formed HTTP request
        """
        request.index = index
        resp = self.send_request(request)
        return self.check_resp(request, resp)

    def send_request(self, request):
        raise NotImplementedError()
        return None

    def check_resp(self, request, resp):
        """
        Takes a string, an HTTPRespnse, and two int as arguments.
        Check that response has the right status and content length
        Returns True or False
        """
        ok = False
        if isinstance(resp, HTTPResponse) and request.status == resp.status:
            if request.length != 0:
                content = tu.read_content(resp)
                if (
                    resp.getheader("Content-Length") == str(request.length)
                    and len(content) == request.length
                ):
                    ok = True
            else:
                if (
                    resp.getheader("Content-Length") is None
                    and resp.getheader("Location") is not None
                ):
                    ok = True
            if request.cookies is not None:
                ok = ok and self.test_cookies(request, resp)

        print(f"test_request_{self.index}.{request.index}", tu.okko(ok))
        return ok

    def test_cookies(self, request, resp):
        """
        Test the receiced cookies
        """
        ok = True
        resp_cookies = self.get_cookies(resp)
        if len(resp_cookies) != len(request.cookies):
            return False
        for cookie in request.cookies:
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

    def get_cookies(self, resp):
        """
        Retrieve cookies from the server response.
        """
        cookies = SimpleCookie()
        for header, value in resp.getheaders():
            if header.lower() == "set-cookie":
                cookies.load(value)
        return cookies
