from virtualrequest import VirtualRequest

class SimpleRequest(VirtualRequest):

    def __init__(self, method, content="", cookies=""):
        VirtualRequest.__init__(self)
        self.method = method
        self.content = content
        self.cookies = cookies
        self.cookies = None

    def set_headers(self, headers):
        self.headers = headers
        return self
    
    def set_path(self, path):
        self.path = path
        return self
