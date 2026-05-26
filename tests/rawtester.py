import socket
import sys

import testutils as tu
from virtualrequesttester import VirtualRequestTester


class RawTester(VirtualRequestTester):

    def __init__(self, index, host, port):
        VirtualRequestTester.__init__(self, index, host, port)

    def test_request(self, index, request):
        if request.pre_test is not None:
            request.pre_test()

        request.index = index
        raw_res = self.send_request(request)
        ok = self.check_resp(request, tu.raw_to_http_response(raw_res))

        if request.post_test is not None:
            request.post_test()
        return ok

    def send_request(self, request):
        with socket.create_connection((self.host, self.port)) as sock:
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
