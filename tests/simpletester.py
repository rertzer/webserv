import http.client

from virtualrequesttester import VirtualRequestTester


class SimpleTester(VirtualRequestTester):

    def __init__(self, index, host, port):
        VirtualRequestTester.__init__(self, index, host, port)

    def send_request(self, request):
        resp = None
        try:
            conn = http.client.HTTPConnection(self.host, self.port)
            print("send request A")
            conn.request(request.method, request.path, request.content, request.headers)
            print("send request B")
            resp =  conn.getresponse()
            print("send request C")
            # print(resp.read())
            # conn.close()

        except ConnectionRefusedError as e:
            print({e})
        return resp 
