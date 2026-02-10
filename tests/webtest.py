#!/usr/bin/env python3
"""
Test module for webserv
"""

import os
import selectors
import subprocess
import time


def start_server(confile):
    """
    start webserv with confile as first argument
    """
    proc = subprocess.Popen(
        ["stdbuf", "-oL", "./webserv", confile],
        cwd="../",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    os.set_blocking(proc.stdout.fileno(), False)
    os.set_blocking(proc.stderr.fileno(), False)
    return proc


def main():
    """
    Tests for Webserv main function.
    """
    proc = start_server("conf/webserv_3rertzer.conf")
    startat = time.monotonic()
    print("polling")
    sel = selectors.DefaultSelector()
    sel.register(proc.stdout, selectors.EVENT_READ)
    sel.register(proc.stderr, selectors.EVENT_READ)
    while proc.poll() is None:
        print("looping")
        events = sel.select(timeout=1)
        for key, _ in events:
            pipe = key.fileobj
            line = pipe.readline()
            if not line:
                sel.unregister(pipe)
                continue

            if pipe is proc.stdout:
                print("stdout: ", line.rstrip())
            elif pipe is proc.stderr:
                print("stderr: ", line.rstrip())
        elapsed = time.monotonic() - startat
        if elapsed > 42.0:
            break
    print("loop ended")
    proc.terminate()
    try:
        proc.wait(timeout=4)
    except subprocess.TimeoutExpired:
        proc.kill()
    print("returned value, ", proc.args, proc.returncode)


if __name__ == "__main__":
    main()
