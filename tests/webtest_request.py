"""
Webserv tests: test command line arguments parsing.
"""

from urllib.parse import urlencode

import testutils as tu
from webserver import WebServer


def test_1():
    """
    Test GET requests.
    ! server behavior in subtest 7 isn't satisfactory
    """
    server = tu.test_request_start("")
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
    passed = sum(tu.test_request(i + 1, host, port, t) for i, t in enumerate(tests))

    tu.test_request_end(server)
    tu.check_server_output(server)

    return (passed, len(tests))


def test_2():
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
    server = tu.test_request_start("")
    assert isinstance(server, WebServer)

    passed = sum(
        tu.test_raw(f"2.{i + 1}", "localhost", 8080, t) for i, t in enumerate(tests)
    )

    tu.test_request_end(server)
    tu.check_server_output(server)

    return (passed, len(tests))


def test_3():
    """
    Test POST requests on cesar_post.php.
    """
    server = tu.test_request_start("")
    assert isinstance(server, WebServer)
    host = "localhost"
    port = 8080

    tests = (
        (
            {"Content-Type": "application/x-www-form-urlencoded"},
            urlencode({"cipher_key": "3", "texte_area": "hello world", "cipher": "on"}),
            200,
            2664,
        ),
        (
            {"Content-Type": "application/x-www-form-urlencoded"},
            urlencode(
                {
                    "cipher_key": "23",
                    "texte_area": "Ave Ceasar morituri te salutant",
                    "cipher": "on",
                }
            ),
            200,
            2684,
        ),
        (
            {"Content-Type": "application/x-www-form-urlencoded"},
            urlencode(
                {"cipher_key": "NAN", "texte_area": "hello world", "cipher": "on"}
            ),
            200,
            487,
        ),
        (
            {"Content-Type": "application/x-www-form-urlencoded"},
            urlencode({"texte_area": "hello world"}),
            200,
            2630,
        ),
        (
            {"Content-Type": "application/x-www-form-urlencoded"},
            urlencode({"gloup": "3", "texte_area": "hello world", "cipher": "on"}),
            200,
            2664,
        ),
        (
            {"Content-Type": "text/html"},
            urlencode({"cipher_key": "3", "texte_area": "hello world", "cipher": "on"}),
            200,
            2630,
        ),
    )
    passed = sum(
        tu.test_post_request(i + 1, host, port, "/php/cesar_post.php", t)
        for i, t in enumerate(tests)
    )

    tu.test_request_end(server)
    tu.check_server_output(server)

    return (passed, len(tests))


def test_4():
    """
    Test POST requests on cesar_post.php.
    Testing invalid POST requests.
    """
    server = tu.test_request_start("")
    assert isinstance(server, WebServer)
    host = "localhost"
    port = 8080

    tests = (
        (
            (
                "POST /php/cesar_post.php HTTP/1.1\r\n"
                + "Host: localhost\r\n"
                + "Content-Type: not-appropriate\r\n"
                + "Content-Length: 45\r\n\r\n"
                + urlencode(
                    {"cipher_key": "3", "texte_area": "hello world", "cipher": "on"}
                )
            ).encode(),
            200,
            2630,
        ),
        (
            (
                "POST /php/cesar_post.php HTTP/1.1\r\n"
                + "Host: localhost\r\n"
                + "Content-Type: application/x-www-form-urlencoded\r\n"
                + "Content-Length: 12\r\n\r\n"
                + urlencode(
                    {"cipher_key": "3", "texte_area": "hello world", "cipher": "on"}
                )
            ).encode(),
            200,
            2630,
        ),
    )
    passed = sum(tu.test_raw(f"4.{i + 1}", host, port, t) for i, t in enumerate(tests))

    tu.test_request_end(server)
    tu.check_server_output(server)

    return (passed, len(tests))


def test_5():
    """
    Test GET python CGI requests on quizz.py.
    Testing cookies.
    """
    server = tu.test_request_start("")
    assert isinstance(server, WebServer)
    host = "localhost"
    port = 8080

    tests = (
        (
            ("GET /python/quizz.py HTTP/1.1\r\n" + "Host: localhost\r\n").encode(),
            200,
            1299,
            (
                ["good", "0", {"SameSite": "Strict"}],
                ["total", "0", {"SameSite": "Strict"}],
            ),
        ),
        (
            (
                "GET /python/quizz.py?q_id=0&q_answer=Xavier+Niel HTTP/1.1\r\n"
                + "Host: localhost\r\n"
                + "Cookie: good=0; total=0; name=Droopy\r\n"
            ).encode(),
            200,
            1451,
            (
                ["good", "0", {"SameSite": "Strict"}],
                ["total", "1", {"SameSite": "Strict"}],
                ["name", "Droopy", {"SameSite": "Strict"}],
            ),
        ),
        (
            (
                "GET /python/quizz.py?q_id=4&q_answer=son+chat HTTP/1.1\r\n"
                + "Host: localhost\r\n"
                + "Cookie: good=3; total=4; name=Droopy\r\n"
            ).encode(),
            200,
            1509,
            (
                ["good", "3", {"SameSite": "Strict"}],
                ["total", "5", {"SameSite": "Strict"}],
                ["name", "Droopy", {"SameSite": "Strict"}],
            ),
        ),
        (
            (
                "GET /python/quizz.py?q_id=4&q_answer=notanexpectedvalue HTTP/1.1\r\n"
                + "Host: localhost\r\n"
                + "Cookie: good=3; total=4; name=Droopy\r\n"
            ).encode(),
            200,
            1519,
            (
                ["good", "3", {"SameSite": "Strict"}],
                ["total", "5", {"SameSite": "Strict"}],
                ["name", "Droopy", {"SameSite": "Strict"}],
            ),
        ),
        (
            (
                "GET /python/quizz.py?q_id=4&q_answer=notanexpectedvalue HTTP/1.1\r\n"
                + "Host: localhost\r\n"
                + "Cookie: bad=3; total=4; name=Droopy\r\n"
            ).encode(),
            200,
            1519,
            (
                ["bad", "3", {}],
                ["good", "0", {"SameSite": "Strict"}],
                ["total", "5", {"SameSite": "Strict"}],
                ["name", "Droopy", {"SameSite": "Strict"}],
            ),
        ),
        (
            (
                "GET /python/quizz.py?q_id=4&q_answer=notanexpectedvalue HTTP/1.1\r\n"
                + "Host: localhost\r\n"
                + "Cookie: total=4; name=Droopy\r\n"
            ).encode(),
            200,
            1519,
            (
                ["good", "0", {"SameSite": "Strict"}],
                ["total", "5", {"SameSite": "Strict"}],
                ["name", "Droopy", {"SameSite": "Strict"}],
            ),
        ),
        (
            (
                "GET /python/quizz.py?q_id=4&q_answer=notanexpectedvalue HTTP/1.1\r\n"
                + "Host: localhost\r\n"
                + "Cookie: good=3 total=4;name=Droopy\r\n"
            ).encode(),
            200,
            1519,
            (
                ["good", "3", {"SameSite": "Strict"}],
                ["total", "5", {"SameSite": "Strict"}],
                ["name", "Droopy", {"SameSite": "Strict"}],
            ),
        ),
        (
            (
                "GET /python/quizz.py?q_id=4&q_answer=notanexpectedvalue HTTP/1.1\r\n"
                + "Host: localhost\r\r"
                + "Cookie: good=3; total=4; name=Droopy\r\n"
            ).encode(),
            200,
            1299,
            (
                ["good", "0", {"SameSite": "Strict"}],
                ["total", "0", {"SameSite": "Strict"}],
            ),
        ),
    )
    passed = sum(
        tu.test_raw_with_cookies(f"5.{i + 1}", host, port, t)
        for i, t in enumerate(tests)
    )

    tu.test_request_end(server)
    tu.check_server_output(server)

    return (passed, len(tests))


def test_6():
    """
    Test  GET index.
    """
    server = tu.test_request_start("")
    assert isinstance(server, WebServer)
    host = "localhost"
    port = 8081

    tests = (
        ("GET", "/html/page/", 200, 1874, host),
        ("GET", "/html/", 200, 1457, host),
    )
    passed = sum(tu.test_request(i + 1, host, port, t) for i, t in enumerate(tests))

    tu.test_request_end(server)
    tu.check_server_output(server)

    return (passed, len(tests))
