from virtualrequest import VirtualRequest


class RawRequest(VirtualRequest):

    def __init__(self, raw, status, length, cookies=None):
        VirtualRequest.__init__(self, length)
        self.set_status(status)
        self.raw = raw
        self.cookies = cookies
