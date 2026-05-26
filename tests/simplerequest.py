from virtualrequest import VirtualRequest

class SimpleRequest(VirtualRequest):

    def __init__(self, method):
        VirtualRequest.__init__(self)
        self.method = method
        self.path = None
        self.headers = None
        self.content = None
        self.cookies = None 
 
    def set_path(self, path):
        self.path = path
        return self

    def set_headers(self, headers):
        self.headers = headers
        return self
    
    def set_content(self, content):
        self.content = content
        return self

