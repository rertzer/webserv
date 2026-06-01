import http.client

from virtualtester import VirtualTester

class SimpleTester(VirtualTester):

    def __init__(self, name, host, port):
        VirtualTester.__init__(self, name, host, port)

    def send_request(self, request):
        resp = None
        try:
            current_connection = http.client.HTTPConnection(self.host, self.port)
            current_connection.request(request.method, request.path, request.content, request.headers)
            resp =  current_connection.getresponse()

        except ConnectionRefusedError as e:
            print({e})
        return resp 
