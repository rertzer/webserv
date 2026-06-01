import testutils as tu

class VirtualRequest:

    def __init__(self):
        self.description = ""
        self.status = None 
        self.length = None 
        self.pre_test = None
        self.post_test = None
        self.server_test = None
        self.index = 0
   
    def set_description(self, description):
        self.description = description
        return self

    def set_server_test(self, server_test):
        self.server_test = server_test
        return self

    def same_length(self, resp):
        content = tu.read_content(resp)
        return resp.getheader("Content-Length") == str(
            self.length
        ) and self.length == len(content)

    def set_status(self, status):
        self.status = status
        return self

    def set_length(self, length):
        self.length = length
        return self

    def set_pre_test(self, func):
        self.pre_test = func
        return self

    def set_post_test(self, func):
        self.post_test = func
        return self

