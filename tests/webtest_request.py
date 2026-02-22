"""
Webserv tests: test command line arguments parsing.
"""

import io
import socket

from testutils import *
from webserver import WebServer


def test_request_1():
    """
    Test GET requests.
    ! server behavior in subtest 7 isn't satisfactory
    """
    subtest_nb = 7
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

    res = send_request(
        host,
        port,
        "GET",
        "/php/cesar_get.php?texte_area=salut+Jules&cipher_key=2&cipher=chiffrer",
        {"Host": host},
    )
    passed += check_res("1.6", res, 200, 2678)

    res = send_request(
        host,
        port,
        "GET",
        "/php/brutus_get.php?texte_area=salut+Jules&cipher_key=2&cipher=chiffrer",
        {"Host": host},
    )
    passed += check_res("1.7", res, 200, 27)
    test_request_end(server)
    assert isinstance(server.proc.stdout, io.TextIOWrapper)
    assert isinstance(server.proc.stderr, io.TextIOWrapper)
    output = server.proc.stdout.read()
    outerror = server.proc.stderr.read()
    ret = server.proc.returncode
    print(f"RETURN|{ret}|{output}|{outerror}|\n")
    return (passed, subtest_nb)


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
    subtest_nb = len(tests)
    passed = 0
    server = test_request_start("")
    assert isinstance(server, WebServer)

    for i, t in enumerate(tests):
        with socket.create_connection(("localhost", 8080)) as sock:
            passed += test_raw(i + 1, sock, t)

    test_request_end(server)
    assert isinstance(server.proc.stdout, io.TextIOWrapper)
    assert isinstance(server.proc.stderr, io.TextIOWrapper)
    output = server.proc.stdout.read()
    outerror = server.proc.stderr.read()
    ret = server.proc.returncode
    print(f"RETURN|{ret}|{output}|{outerror}|\n")
    return (passed, subtest_nb)


def test_raw(index, sock, t):
    """
    Sends raw request req on socket sock and checks the
    response for status stat and content-length length.
    returns 1 or 0.
    """
    sock.sendall(t[0])
    raw_res = sock.recv(4096)
    # print(raw_res.decode(errors="replace"))
    res = raw_to_http_response(raw_res)
    return check_res(f"2.{index}", res, t[1], t[2])
