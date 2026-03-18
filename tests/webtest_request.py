"""
Webserv tests: test command line arguments parsing.
"""

import subprocess
import sys
from urllib.parse import urlencode

import testutils as tu
from simplerequest import SimpleRequest
from simpletester import SimpleTester
from webserver import WebServer


def test_1():
    """
    Test GET requests.
    ! server behavior in subtest 6 (brutus_get) isn't satisfactory
    """
    host = "localhost"
    headers = {"Host": host}

    tester = SimpleTester(1, host, 8080)
    tester.start_server("")

    requests = (
        SimpleRequest("GET", "/", headers, 200, 1146),
        SimpleRequest("GOT", "/", headers, 400, 847),
        SimpleRequest("GET", "/nowhere", headers, 404, 851),
        SimpleRequest("GET", "/html/kitty/kitty.html", headers, 200, 1469),
        SimpleRequest(
            "GET",
            "/php/cesar_get.php?texte_area=salut+Jules&cipher_key=2&cipher=chiffrer",
            headers,
            200,
            2678,
        ),
        SimpleRequest(
            "GET",
            "/php/brutus_get.php?texte_area=salut+Jules&cipher_key=2&cipher=chiffrer",
            headers,
            200,
            27,
        ),
        SimpleRequest("GET", "/", {"Host": "hostile"}, 200, 1146),
    )
    passed = tester.run_requests(requests)
    tester.stop_server()
    tester.check_server_output()
    return (passed, len(requests))


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
    Test  GET index and DELETE.
    """
    server = tu.test_request_start("")
    assert isinstance(server, WebServer)
    host = "localhost"
    port = 8081

    tests = (
        ("GET", "/html/", 200, 1457, host),
        ("GET", "/html/page/", 200, 1988, host),
        ("DELETE", "/html/page/notToDelete.html", 405, 285, host),
        ("GET", "/html/page/delete/", 200, 1257, host),
        ("GET", "/html/page/delete/toDelete.html", 200, 478, host),
        ("DELETE", "/html/page/delete/toDelete.html", 200, 38, host),
        ("DELETE", "/html/page/delete/toDelete.html", 404, 281, host),
        ("GET", "/html/page/delete/", 200, 1131, host),
        ("GET", "/html/page/", 200, 1988, host),
        ("DELETE", "/html/page/delete/toDelete.html", 404, 281, host),
    )
    passed = sum(tu.test_request(i + 1, host, port, t) for i, t in enumerate(tests))

    tu.test_request_end(server)
    tu.check_server_output(server)
    try:
        subprocess.run(
            ["cp", "toDelete.html.bak", "toDelete.html"],
            cwd="../www/html/page/delete/",
            check=True,
        )
    except subprocess.CalledProcessError as e:
        print("Restoring the toDelelte file failed:", e.returncode, file=sys.stderr)
        print(e.stderr, file=sys.stderr)

    return (passed, len(tests))


def test_7():
    """
    Test GET redirection, new root, new index, ERROR 403.
    """
    server = tu.test_request_start("")
    assert isinstance(server, WebServer)
    host = "localhost"
    port = 8081

    tests = (
        ("GET", "/redir/anything", 301, 0, host),
        ("GET", "/newRoot/newRoot.html", 200, 379, host),
        ("GET", "/newIndex/", 200, 370, host),
        ("GET", "/html/page/forbidden.html", 403, 304, host),
    )
    try:
        subprocess.run(
            ["chmod", "000", "forbidden.html"],
            cwd="../www/html/page/",
            check=True,
        )
    except subprocess.CalledProcessError as e:
        print("Unable to change forbidden.html rights:", e.returncode, file=sys.stderr)
        print(e.stderr, file=sys.stderr)
    passed = sum(tu.test_request(i + 1, host, port, t) for i, t in enumerate(tests))

    tu.test_request_end(server)
    # tu.check_server_output(server)
    try:
        subprocess.run(
            ["chmod", "644", "forbidden.html"],
            cwd="../www/html/page/",
            check=True,
        )
    except subprocess.CalledProcessError as e:
        print("Unable to change forbidden.html rights:", e.returncode, file=sys.stderr)
        print(e.stderr, file=sys.stderr)

    return (passed, len(tests))


def test_8():
    """
    Test POST file upload and error 413 (Request Entity To Large).
    """
    server = tu.test_request_start("")
    assert isinstance(server, WebServer)
    host = "localhost"
    port = 8080

    kitty_1_content, boundary_1 = tu.get_kitty_content("../www/img/kitty1.jpeg")
    length_1 = len(kitty_1_content)

    kitty_2_content, boundary_2 = tu.get_kitty_content("../www/img/kitty2.jpg")
    length_2 = len(kitty_2_content)

    tests = (
        (
            (
                "POST /html/kitty/success.html HTTP/1.1\r\n"
                + "Host: localhost\r\n"
                + f"Content-Type: multipart/form-data; boundary={boundary_1}\r\n"
                + f"Content-Length: {length_1}\r\n\r\n"
            ).encode()
            + kitty_1_content,
            200,
            1284,
        ),
        (
            (
                "POST /html/kitty/success.html HTTP/1.1\r\n"
                + "Host: localhost\r\n"
                + f"Content-Type: multipart/form-data; boundary={boundary_2}\r\n"
                + f"Content-Length: {length_2}\r\n\r\n"
            ).encode()
            + kitty_2_content,
            413,
            877,
        ),
    )
    passed = sum(tu.test_raw(f"8.{i + 1}", host, port, t) for i, t in enumerate(tests))

    tu.test_request_end(server)
    tu.check_server_output(server)
    try:
        subprocess.run(
            ["rm", "-f", "kitty1.jpeg"],
            cwd="../www/upload/",
            check=True,
        )
    except subprocess.CalledProcessError as e:
        print("Deleting kitty failed:", e.returncode, file=sys.stderr)
        print(e.stderr, file=sys.stderr)

    return (passed, len(tests))


def test_9():
    """
    Test file large upload and download.
    ! ResetError needs to be investigated
    ! Get no error when doing a boundary mistake during upload
    """
    server = tu.test_request_start("")
    assert isinstance(server, WebServer)
    host = "localhost"
    port = 8081

    kitty_1_content, boundary_1 = tu.get_kitty_content("../www/img/kitty2.jpg")
    length_1 = len(kitty_1_content)

    kitty_2_content, boundary_2 = tu.get_wrong_kitty_content("../www/img/kitty2.jpg")
    length_2 = len(kitty_2_content)

    tests = (
        (
            (
                "POST /html/kitty/success.html HTTP/1.1\r\n"
                + "Host: localhost\r\n"
                + f"Content-Type: multipart/form-data; boundary={boundary_2} \r\n"
                + f"Content-Length: {length_2}\r\n\r\n"
            ).encode()
            + kitty_2_content,
            200,
            1284,
        ),
        (("GET /upload/ HTTP/1.1\r\nHost: localhost\r\n\r\n").encode(), 200, 1198),
        (
            (
                "POST /html/kitty/success.html HTTP/1.1\r\n"
                + "Host: localhost\r\n"
                + f"Content-Type: multipart/form-data; boundary={boundary_1}\r\n"
                + f"Content-Length: {length_1}\r\n\r\n"
            ).encode()
            + kitty_1_content,
            200,
            1284,
        ),
        (("GET /upload/ HTTP/1.1\r\nHost: localhost\r\n\r\n").encode(), 200, 1321),
        (
            ("GET /upload/kitty2.jpg HTTP/1.1\r\nHost: localhost\r\n\r\n").encode(),
            200,
            178976,
        ),
    )
    passed = sum(tu.test_raw(f"9.{i + 1}", host, port, t) for i, t in enumerate(tests))

    tu.test_request_end(server)
    tu.check_server_output(server)
    try:
        subprocess.run(
            ["rm", "-f", "kitty2.jpg"],
            cwd="../www/upload/",
            check=True,
        )
    except subprocess.CalledProcessError as e:
        print("Deleting kitty failed:", e.returncode, file=sys.stderr)
        print(e.stderr, file=sys.stderr)

    return (passed, len(tests))
