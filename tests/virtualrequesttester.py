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
        self.check_server_output()
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
        ok = False
        if isinstance(resp, HTTPResponse) and request.status == resp.status:
            ok = self.check_content(request, resp)

        tu.print_result(f"request_{self.index}", request.index, ok)
        return ok

    def check_content(self, request, resp):
        ok = (self.check_content_len(request, resp), self.check_location(resp))[
            request.length == 0
        ]

        if request.cookies is not None:
            ok = ok and self.test_cookies(request, resp)
        return ok

    def check_content_len(self, request, resp):
        content = tu.read_content(resp)
        return (
            resp.getheader("Content-Length") == str(request.length)
            and len(content) == request.length
        )

    def check_location(self, resp):
        return (
            resp.getheader("Content-Length") is None
            and resp.getheader("Location") is not None
        )

    def test_cookies(self, request, resp):
        ok = True
        resp_cookies = self.get_cookies(resp)
        if len(resp_cookies) != len(request.cookies):
            return False
        for cookie in request.cookies:
            ok = self.test_cookie(cookie, resp_cookies)
            if ok is False:
                break

        return ok

    def test_cookie(self, cookie, resp_cookies):
        ok = True
        if (
            cookie[0] not in resp_cookies.keys()
            or resp_cookies[cookie[0]].value != cookie[1]
        ):
            ok = False
        else:
            for k, v in cookie[2].items():
                if (
                    k.lower() not in resp_cookies[cookie[0]].keys()
                    or v != resp_cookies[cookie[0]][k.lower()]
                ):
                    ok = False
                    break
        return ok

    def get_cookies(self, resp):
        cookies = SimpleCookie()
        for header, value in resp.getheaders():
            if header.lower() == "set-cookie":
                cookies.load(value)
        return cookies
