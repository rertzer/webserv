"""
Webserv tests: test command line arguments parsing.
"""

import socket

from testutils import *
from webserver import WebServer


def test_request_1():
    """
    Test GET requests.
    ! server behavior in subtest 7 isn't satisfactory
    """
    server = test_request_start("")
    assert isinstance(server, WebServer)
    host = "localhost"
    port = 8080

    tests = (
        ("GET", "/", 200, 1146, host),
        ("GOT", "/", 400, 847, host),
        ("GET", "/nowhere", 404, 851, host),
        ("GET", "/", 200, 1146, "hostile"),
        ("GET", "/html/kitty/kitty.html", 200, 1469, host),
        (
            "GET",
            "/php/cesar_get.php?texte_area=salut+Jules&cipher_key=2&cipher=chiffrer",
            200,
            2678,
            host,
        ),
        (
            "GET",
            "/php/brutus_get.php?texte_area=salut+Jules&cipher_key=2&cipher=chiffrer",
            200,
            27,
            host,
        ),
    )
    passed = sum(test_request(i + 1, host, port, t) for i, t in enumerate(tests))

    test_request_end(server)
    check_server_output(server)

    return (passed, len(tests))


def test_request_2():
    """
    Test invalid http requests.
    use socket
    """

    tests = (
        (b"\r\nHost: localhost\r\n\r\n", 400, 847),
        (b"GET / HTTP/6.1\r\nHost: localhost\r\n\r\n", 505, 864),
        (b"PUT / HTTP/1.1\r\nHost: localhost\r\n\r\n", 501, 844),
        (b"Thisisnotavalidrequest HTTP/1.1\r\nHost: localhost\r\n\r\n", 400, 847),
        (b"POST /php/norminet.html HTTP/1.1\r\nHost: localhost\r\n\r\n", 404, 851),
    )
    server = test_request_start("")
    assert isinstance(server, WebServer)

    passed = sum(test_raw(i + 1, t) for i, t in enumerate(tests))

    test_request_end(server)
    check_server_output(server)

    return (passed, len(tests))


def test_raw(index, t):
    """
    Sends raw request req on socket sock and checks the
    response for status stat and content-length length.
    returns 1 or 0.
    """
    with socket.create_connection(("localhost", 8080)) as sock:
        sock.sendall(t[0])
        raw_res = sock.recv(4096)
    # print(raw_res.decode(errors="replace"))
    return check_res(f"2.{index}", raw_to_http_response(raw_res), t[1], t[2])
