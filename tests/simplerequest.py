"""
SimpleRequest handles test requests for properly formatted http requests.
"""

from virtualrequest import VirtualRequest


class SimpleRequest(VirtualRequest):
    """
    Simple Request inherits form VirtualRequest.
    It implements properly formed http requests.
    """

    def __init__(self, method, path, headers, status, length):
        VirtualRequest.__init__(self, status, length)
        self.method = method
        self.path = path
        self.headers = headers
