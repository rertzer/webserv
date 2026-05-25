from virtualrequest import VirtualRequest


class ConfRequest(VirtualRequest):

    def __init__(self, conf_file, stdout, stderr):
        VirtualRequest.__init__(self, None)
        self.conf_file = conf_file
        self.stdout = stdout
        self.stderr = stderr
