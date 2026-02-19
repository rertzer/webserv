"""
Webserv tests: test command line arguments parsing.
"""

import io

from testutils import check_res, send_request, test_request_end, test_request_start
from webserver import WebServer


def test_request_1():
    """
    Test error message if to many args.
    """
    passed = 0
    server = test_request_start("")
    assert isinstance(server, WebServer)
    host = "localhost"
    port = 8080
    res = send_request(host, port, "GET", "/", {"Host": host})
    passed += check_res("1.1", res, 200, 1146)

    res = send_request(host, port, "GOT", "/", {"Host": host})
    passed += check_res("1.2", res, 400, 847)

    res = send_request(host, port, "GET", "/nowhere", {"Host": host})
    passed += check_res("1.3", res, 404, 851)

    res = send_request(host, port, "GET", "/", {"Host": "hostile"})
    passed += check_res("1.4", res, 200, 1146)

    res = send_request(host, port, "GET", "/html/kitty/kitty.html", {"Host": host})
    passed += check_res("1.5", res, 200, 1469)

    test_request_end(server)
    assert isinstance(server.proc.stdout, io.TextIOWrapper)
    assert isinstance(server.proc.stderr, io.TextIOWrapper)
    output = server.proc.stdout.read()
    outerror = server.proc.stderr.read()
    ret = server.proc.returncode
    print(f"RETURN|{ret}|{output}|{outerror}|\n")
    return passed
