import socket
import sys

import testutils as tu
from virtualrequesttester import VirtualRequestTester


class RawTester(VirtualRequestTester):
    """
    Tests requests in raw format
    """

    def __init__(self, index, host, port):
        VirtualRequestTester.__init__(self, index, host, port)

    def test_request(self, index, request):
        """
        Sends raw request req on socket sock and checks the
        response for status stat and content-length length.
        returns 1 or 0.
        """
        request.index = index
        raw_res = self.send_request(request)
        return self.check_resp(request, tu.raw_to_http_response(raw_res))

    def send_request(self, request):
        """
        Sends raw request via a socket
        """
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
