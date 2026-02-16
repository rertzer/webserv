"""
Webserv tests: test command line arguments parsing.
"""

from testutils import *
from webserver import WebServer


def test_request_1():
    """
    Test error message if to many args.
    """
    server = test_request_start("")
    assert isinstance(server, WebServer)
    host = "localhost"
    port = 8080
    res = send_request(host, port, "GET", "/", {"Host": host})
    print(
        f"RESPONSE:\n{res.status}\n|{res.getheader('Content-Length')}|\n{res.getheaders()}\n{res.read()}\n"
    )
    test_request_end(server)
    output = server.proc.stdout.read()
    outerror = server.proc.stderr.read()
    ret = server.proc.returncode
    print(f"RETURN|{ret}|{output}|{outerror}|\n")

    return (
        res.status == 200
        and res.getheader("Content-Length") == "1146"
        and ret == -15
        and outerror == ""
    )
