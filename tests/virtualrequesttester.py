from functools import reduce
from http.client import HTTPResponse
from http.cookies import SimpleCookie

import testutils as tu
from colors import Color
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
        # Uncomment for verbose mode
        # self.server.check_output()

    def run_requests(self, requests):
        return sum(self.test_request(i + 1, t) for i, t in enumerate(requests))

    def test_request(self, index, request):
        request.index = index
        ok = False
        if request.pre_test is not None:
            request.pre_test()
        try:
            resp = self.send_request(request)
            ok =self.check_resp(request, resp)
        except:
            tu.print_result(f"request_{self.index}", request.index, ok)
            print(f"  {Color.RED}Send Request: Error{Color.ENDC}")
        finally:
            if request.post_test is not None:
                request.post_test()
        return ok

    def send_request(self, request):
        raise NotImplementedError()
        return None

    def check_resp(self, request, resp):
        ok = (
            isinstance(resp, HTTPResponse)
            and request.status == resp.status
            and self.check_content(request, resp)
            and self.check_server(request)
        )
        tu.print_result(f"request_{self.index}", request.index, ok)
        return ok

    def check_content(self, request, resp):
        ok = False
        ok_cookie = True

        if request.length != 0:
            ok = request.same_length(resp)
        else:
            ok = self.check_location(resp)
        
        if request.cookies is not None:
            ok_cookie = self.test_cookies(request, resp)
        return ok and ok_cookie 
        

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

    def check_server(self, request):
        ok = True
        if request.server_test is not None:
            ok = request.server_test()
        return ok

