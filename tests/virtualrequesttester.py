from functools import reduce
from http.client import HTTPResponse
from http.cookies import SimpleCookie

import testutils as tu
from webserver import WebServer


class VirtualRequestTester:

    def __init__(self, index, host, port):
        self.index = index
        self.host = host
        self.port = port
        self.server = None

    def proceed_requests(self, conf_file, requests):
        self.server = WebServer.start(conf_file)
        if not isinstance(self.server, WebServer):
            return None
        passed = self.run_requests(requests)
        self.server.stop()
        # self.check_server_output()
        return passed

    def check_server_output(self):
        assert isinstance(self.server, WebServer)
        self.server.check_output()

    def run_requests(self, requests):
        return sum(self.test_request(i + 1, t) for i, t in enumerate(requests))

    def test_request(self, index, request):
        request.index = index
        resp = self.send_request(request)
        return self.check_resp(request, resp)

    def send_request(self, request):
        raise NotImplementedError()
        return None

    def check_resp(self, request, resp):
        ok = (
            isinstance(resp, HTTPResponse)
            and request.status == resp.status
            and self.check_content(request, resp)
        )

        tu.print_result(f"request_{self.index}", request.index, ok)
        return ok

    def check_content(self, request, resp):
        return (
            (
                request.same_length(resp)
                if request.length != 0
                else self.check_location(resp)
            )
            and self.test_cookies(request, resp)
            if request.cookies is not None
            else True
        )

    def check_location(self, resp):
        return (
            resp.getheader("Content-Length") is None
            and resp.getheader("Location") is not None
        )

    def test_cookies(self, request, resp):
        resp_cookies = self.get_cookies(resp)
        return tu.same_length(request.cookies, resp_cookies) and self.cookies_identical(
            request.cookies, resp_cookies
        )

    def get_cookies(self, resp):
        cookies = SimpleCookie()
        for header, value in resp.getheaders():
            if header.lower() == "set-cookie":
                cookies.load(value)
        return cookies

    def cookies_identical(self, request_cookies, resp_cookies):
        return reduce(lambda a, b: a and b.test(resp_cookies), request_cookies, True)
