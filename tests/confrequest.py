from virtualrequest import VirtualRequest


class ConfRequest(VirtualRequest):

    def __init__(self, conf_file, status, stdout, stderr):
        VirtualRequest.__init__(self, status, None)
        self.conf_file = conf_file
        self.stdout = stdout
        self.stderr = stderr
