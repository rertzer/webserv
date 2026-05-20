import http.client

from virtualrequesttester import VirtualRequestTester


class SimpleTester(VirtualRequestTester):

    def __init__(self, index, host, port):
        VirtualRequestTester.__init__(self, index, host, port)

    def send_request(self, request):
        resp = None
        try:
            conn = http.client.HTTPConnection(self.host, self.port)
            conn.request(request.method, request.path, request.content, request.headers)
            resp =  conn.getresponse()
            # print(resp.read())
            # conn.close()

        except ConnectionRefusedError as e:
            print({e})
        return resp 
