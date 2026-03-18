"""
RawRequest handles test requests for unproper http requests.
"""

from virtualrequest import VirtualRequest


class RawRequest(VirtualRequest):
    """
    Raw requests inherit from VirtualRequest.
    It implements raw http requests.
    """

    def __init__(self, raw, status, length):
        VirtualRequest.__init__(self, status, length)
        self.raw = raw
