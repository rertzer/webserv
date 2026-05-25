import testutils as tu

class VirtualRequest:

    def __init__(self, status, length):
        self.status = status
        self.length = length
        self.pre_test = None
        self.post_test = None

    def same_length(self, resp):
        content = tu.read_content(resp)
        return resp.getheader("Content-Length") == str(
            self.length
        ) and self.length == len(content)

    def add_pre_test(self, func):
        self.pre_test = func
        return self

    def add_post_test(self, func):
        self.post_test = func
        return self
