from virtualrequest import VirtualRequest


class ConfRequest(VirtualRequest):

    def __init__(self, conf_file):
        VirtualRequest.__init__(self, None)
        self.conf_file = conf_file
        self.stdout = None 
        self.stderr = None 

    def set_stdout(self, stdout):
        self.stdout = stdout
        return self

    def set_stderr(self, stderr):
        self.stderr = stderr
        return self
