import testutils as tu

class VirtualRequest:

    def __init__(self):
        self.status = None 
        self.length = None 
        self.pre_test = None
        self.post_test = None

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

