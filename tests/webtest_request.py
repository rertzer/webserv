from urllib.parse import urlencode

import testutils as tu
from rawrequest import RawRequest
from rawtester import RawTester
from simplerequest import SimpleRequest
from simpletester import SimpleTester


def test_1():
    """
    ! server behavior in subtest 6 (brutus_get) isn't satisfactory
    """
    host = "localhost"
    port = 8080
    headers = {"Host": host}

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
        SimpleRequest(
            "POST",
            "/php/cesar_post.php",
            {"Host": host, "Content-Type": "application/x-www-form-urlencoded"},
            200,
            2664,
            urlencode({"cipher_key": "3", "texte_area": "hello world", "cipher": "on"}),
        ),
        SimpleRequest(
            "POST",
            "/php/cesar_post.php",
            {"Host": host, "Content-Type": "application/x-www-form-urlencoded"},
            200,
            2684,
            urlencode(
                {
                    "cipher_key": "23",
                    "texte_area": "Ave Ceasar morituri te salutant",
                    "cipher": "on",
                }
            ),
        ),
        SimpleRequest(
            "POST",
            "/php/cesar_post.php",
            {"Host": host, "Content-Type": "application/x-www-form-urlencoded"},
            200,
            487,
            urlencode(
                {"cipher_key": "NAN", "texte_area": "hello world", "cipher": "on"}
            ),
        ),
        SimpleRequest(
            "POST",
            "/php/cesar_post.php",
            {"Host": host, "Content-Type": "application/x-www-form-urlencoded"},
            200,
            2630,
            urlencode({"texte_area": "hello world"}),
        ),
        SimpleRequest(
            "POST",
            "/php/cesar_post.php",
            {"Host": host, "Content-Type": "application/x-www-form-urlencoded"},
            200,
            2664,
            urlencode({"gloup": "3", "texte_area": "hello world", "cipher": "on"}),
        ),
        SimpleRequest(
            "POST",
            "/php/cesar_post.php",
            {"Host": host, "Content-Type": "text/html"},
            200,
            2630,
            urlencode({"cipher_key": "3", "texte_area": "hello world", "cipher": "on"}),
        ),
    )

    passed = SimpleTester(1, host, port).proceed_requests("", requests)
    return (passed, len(requests))


def test_2():
    host = "localhost"
    port = 8080

    requests = (
        RawRequest(b"\r\nHost: localhost\r\n\r\n", 400, 847),
        RawRequest(b"GET / HTTP/6.1\r\nHost: localhost\r\n\r\n", 505, 864),
        RawRequest(b"PUT / HTTP/1.1\r\nHost: localhost\r\n\r\n", 501, 844),
        RawRequest(
            b"Thisisnotavalidrequest HTTP/1.1\r\nHost: localhost\r\n\r\n", 400, 847
        ),
        RawRequest(
            b"POST /php/norminet.html HTTP/1.1\r\nHost: localhost\r\n\r\n", 404, 851
        ),
        RawRequest(
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
        RawRequest(
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
        RawRequest(
            ("GET /python/quizz.py HTTP/1.1\r\n" + "Host: localhost\r\n").encode(),
            200,
            1299,
            (
                ["good", "0", {"SameSite": "Strict"}],
                ["total", "0", {"SameSite": "Strict"}],
            ),
        ),
        RawRequest(
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
        RawRequest(
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
        RawRequest(
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
        RawRequest(
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
        RawRequest(
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
        RawRequest(
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
        RawRequest(
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

    passed = RawTester(2, host, port).proceed_requests("", requests)
    return (passed, len(requests))


def test_3():
    host = "localhost"
    port = 8081
    headers = {"Host": host}

    requests = (
        SimpleRequest("GET", "/html/", headers, 200, 1457),
        SimpleRequest("GET", "/html/page/", headers, 200, 1988),
        SimpleRequest("DELETE", "/html/page/notToDelete.html", headers, 405, 285),
        SimpleRequest("GET", "/html/page/delete/", headers, 200, 1257),
        SimpleRequest("GET", "/html/page/delete/toDelete.html", headers, 200, 478),
        SimpleRequest("DELETE", "/html/page/delete/toDelete.html", headers, 200, 38),
        SimpleRequest("DELETE", "/html/page/delete/toDelete.html", headers, 404, 281),
        SimpleRequest("GET", "/html/page/delete/", headers, 200, 1131),
        SimpleRequest("GET", "/html/page/", headers, 200, 1988),
        SimpleRequest("DELETE", "/html/page/delete/toDelete.html", headers, 404, 281),
        SimpleRequest("GET", "/redir/anything", headers, 301, 0),
        SimpleRequest("GET", "/newRoot/newRoot.html", headers, 200, 379),
        SimpleRequest("GET", "/newIndex/", headers, 200, 370),
    )

    passed = SimpleTester(3, host, port).proceed_requests("", requests)
    tu.cp_backup_to_delete("toDelete.html")
    return (passed, len(requests))


def test_4():
    host = "localhost"
    port = 8081
    headers = {"Host": host}

    requests = (SimpleRequest("GET", "/html/page/forbidden.html", headers, 403, 304),)

    tu.chmod_in_page("forbidden.html", "000")
    passed = SimpleTester(4, host, port).proceed_requests("", requests)
    tu.chmod_in_page("forbidden.html", "644")

    return (passed, len(requests))


def test_5():
    host = "localhost"
    port = 8080

    kitty_1_content, boundary_1, length_1 = tu.get_kitty_content(
        "../www/img/kitty1.jpeg"
    )
    kitty_2_content, boundary_2, length_2 = tu.get_kitty_content(
        "../www/img/kitty2.jpg"
    )

    requests = (
        RawRequest(
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
        RawRequest(
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

    passed = RawTester(5, host, port).proceed_requests("", requests)
    tu.remove_from_upload("kitty1.jpeg")

    return (passed, len(requests))


def test_6():
    """
    ! ResetError needs to be investigated
    ! Get no error when doing a boundary mistake during upload
    """
    host = "localhost"
    port = 8081

    kitty_1_content, boundary_1, length_1 = tu.get_kitty_content(
        "../www/img/kitty2.jpg"
    )
    kitty_2_content, boundary_2, length_2 = tu.get_wrong_kitty_content(
        "../www/img/kitty2.jpg"
    )

    requests = (
        RawRequest(
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
        RawRequest(
            ("GET /upload/ HTTP/1.1\r\nHost: localhost\r\n\r\n").encode(), 200, 1198
        ),
        RawRequest(
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
        RawRequest(
            ("GET /upload/ HTTP/1.1\r\nHost: localhost\r\n\r\n").encode(), 200, 1321
        ),
        RawRequest(
            ("GET /upload/kitty2.jpg HTTP/1.1\r\nHost: localhost\r\n\r\n").encode(),
            200,
            178976,
        ),
    )

    passed = RawTester(6, host, port).proceed_requests("", requests)
    tu.remove_from_upload("kitty2.jpg")
    return (passed, len(requests))
