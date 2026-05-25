from virtualrequest import VirtualRequest

class SimpleRequest(VirtualRequest):

    def __init__(self, method, path, headers, content="", cookies=""):
        VirtualRequest.__init__(self)
        self.method = method
        self.path = path
        self.headers = headers
        self.content = content
        self.cookies = cookies
