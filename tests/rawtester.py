import socket
import sys

import testutils as tu
from virtualtester import VirtualTester


class RawTester(VirtualTester):

    def __init__(self, name, host, port):
        VirtualTester.__init__(self, name, host, port)


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
        return tu.raw_to_http_response(raw_res)
