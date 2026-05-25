from virtualrequest import VirtualRequest


class RawRequest(VirtualRequest):

    def __init__(self, raw, cookies=None):
        VirtualRequest.__init__(self)
        self.raw = raw
        self.cookies = cookies
