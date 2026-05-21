import testutils as tu

class VirtualRequest:

    def __init__(self, status, length):
        self.status = status
        self.length = length

    def same_length(self, resp):
        content = tu.read_content(resp)
        return resp.getheader("Content-Length") == str(
            self.length
        ) and self.length == len(content)
