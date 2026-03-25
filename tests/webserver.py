import io
import os
import selectors
import subprocess
import time


class WebServer:

    def __init__(self, confile):
        self.proc = subprocess.Popen(
            ["stdbuf", "-oL", "./webserv", *confile.split()],
            cwd="../",
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        if self.proc.stdout is None or self.proc.stderr is None:
            self.finish()
            raise RuntimeError("pipe error")
        os.set_blocking(self.proc.stdout.fileno(), False)
        os.set_blocking(self.proc.stderr.fileno(), False)

    def finish(self):
        self.proc.terminate()
        try:
            self.proc.wait(timeout=4)
        except subprocess.TimeoutExpired:
            self.proc.kill()

    def stop(self):
        try:
            self.proc.wait(timeout=1)
        except subprocess.TimeoutExpired:
            self.finish()

    def check_output(self):
        assert isinstance(self.proc.stdout, io.TextIOWrapper)
        assert isinstance(self.proc.stderr, io.TextIOWrapper)
        output = self.proc.stdout.read()
        outerror = self.proc.stderr.read()
        ret = self.proc.returncode
        print(f"RETURN status: {ret}\nSTDOUT:\n{output}\nSTDERR:\n{outerror}\n")

    def run_for(self, delay):
        startat = time.monotonic()
        assert self.proc.stdout is not None
        assert self.proc.stderr is not None
        sel = selectors.DefaultSelector()
        sel.register(self.proc.stdout, selectors.EVENT_READ)
        sel.register(self.proc.stderr, selectors.EVENT_READ)

        while self.proc.poll() is None:
            self.print_msg(sel)
            elapsed = time.monotonic() - startat
            if elapsed > delay:
                break

    def print_msg(self, sel):
        events = sel.select(timeout=1)
        for key, _ in events:
            pipe = key.fileobj
            line = pipe.readline()
            if not line:
                sel.unregister(pipe)
                continue

            if pipe is self.proc.stdout:
                print("stdout: ", line.rstrip())
            elif pipe is self.proc.stderr:
                print("stderr: ", line.rstrip())

    @staticmethod
    def run_server(params):
        try:
            server = WebServer(params)
        except RuntimeError as e:
            print({e})
            return None
        try:
            server.proc.wait(timeout=2)
        except subprocess.TimeoutExpired:
            server.finish()
        return server

    @staticmethod
    def start(conf_file):
        server = None
        try:
            server = WebServer(conf_file)
            time.sleep(1)
        except RuntimeError as e:
            print({e})
        return server
