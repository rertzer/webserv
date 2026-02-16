"""
Functions for webserv testing
"""

import subprocess

from webserver import WebServer


def test_cmd(params):
    """
    Test cmd line generic function.
    """
    try:
        server = WebServer(params)
    except RuntimeError as e:
        print({e})
        return 1
    try:
        server.proc.wait(timeout=4)
    except subprocess.TimeoutExpired:
        server.finish()
    return server
