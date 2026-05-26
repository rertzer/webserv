import os.path
from http import HTTPStatus
from urllib.parse import urlencode

import testutils as tu
from rawrequest import RawRequest
from rawtester import RawTester
from requestcookie import RequestCookie
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
        SimpleRequest("GET")
            .set_path("/")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(1146),
        SimpleRequest("GOT")
            .set_path("/")
            .set_headers(headers)
            .set_status(HTTPStatus.BAD_REQUEST)
            .set_length(847),
        SimpleRequest("GET")
            .set_path("/nowhere")
            .set_headers(headers)
            .set_status(HTTPStatus.NOT_FOUND)
            .set_length(851),
        SimpleRequest("GET")
            .set_path("/html/kitty/kitty.html")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(1469),
        # 5
        SimpleRequest("GET")
            .set_path("/php/cesar_get.php?texte_area=salut+Jules&cipher_key=2&cipher=chiffrer")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(2678),
        SimpleRequest("GET")
            .set_path("/php/brutus_get.php?texte_area=salut+Jules&cipher_key=2&cipher=chiffrer")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(27),
        SimpleRequest("GET")
            .set_path("/")
            .set_headers({"Host": "hostile"})
            .set_status(HTTPStatus.OK)
            .set_length(1146),
        SimpleRequest("POST")
            .set_path("/php/cesar_post.php")
            .set_headers({"Host": host, "Content-Type": "application/x-www-form-urlencoded"})
            .set_content(urlencode({"cipher_key": "3", "texte_area": "hello world", "cipher": "on"}))
            .set_status(HTTPStatus.OK)
            .set_length(2664),
        SimpleRequest("POST")
            .set_path("/php/cesar_post.php")
            .set_headers({"Host": host, "Content-Type": "application/x-www-form-urlencoded"})
            .set_content(urlencode(
                {
                    "cipher_key": "23",
                    "texte_area": "Ave Ceasar morituri te salutant",
                    "cipher": "on",
                }
            ))
            .set_status(HTTPStatus.OK)
            .set_length(2684),
        # 10
        SimpleRequest("POST")
            .set_path("/php/cesar_post.php")
            .set_headers({"Host": host, "Content-Type": "application/x-www-form-urlencoded"})
            .set_content(urlencode({"cipher_key": "NAN", "texte_area": "hello world", "cipher": "on"}))
            .set_status(HTTPStatus.OK)
            .set_length(487),
        SimpleRequest("POST")
            .set_path("/php/cesar_post.php")
            .set_headers({"Host": host, "Content-Type": "application/x-www-form-urlencoded"})
            .set_content(urlencode({"texte_area": "hello world"}))
            .set_status(HTTPStatus.OK)
            .set_length(2630),
        SimpleRequest("POST")
            .set_path("/php/cesar_post.php")
            .set_headers({"Host": host, "Content-Type": "application/x-www-form-urlencoded"})
            .set_content(urlencode({"gloup": "3", "texte_area": "hello world", "cipher": "on"}))
            .set_status(HTTPStatus.OK)
            .set_length(2664),
        SimpleRequest("POST")
            .set_path("/php/cesar_post.php")
            .set_headers({"Host": host, "Content-Type": "text/html"})
            .set_content(urlencode({"cipher_key": "3", "texte_area": "hello world", "cipher": "on"}))
            .set_status(HTTPStatus.OK)
            .set_length(2630),
    )

    passed = SimpleTester(1, host, port).proceed_requests("", requests)
    return (passed, len(requests))


def test_2():
    host = "localhost"
    port = 8080

    requests = (
        RawRequest(b"\r\nHost: localhost\r\n\r\n", )
            .set_status(HTTPStatus.BAD_REQUEST)
            .set_length(847),
        RawRequest(
            b"GET / HTTP/6.1\r\nHost: localhost\r\n\r\n",
        )
            .set_status(HTTPStatus.HTTP_VERSION_NOT_SUPPORTED)
            .set_length(864),
        RawRequest(
            b"PUT / HTTP/1.1\r\nHost: localhost\r\n\r\n",
        )
            .set_status(HTTPStatus.NOT_IMPLEMENTED)
            .set_length(844),
        RawRequest(
            b"HEAD / HTTP/1.1\r\nHost: localhost\r\n\r\n",
        )
            .set_status(HTTPStatus.NOT_IMPLEMENTED)
            .set_length(844),
        # 5
        RawRequest(
            b"Thisisnotavalidrequest HTTP/1.1\r\nHost: localhost\r\n\r\n",
        )
            .set_status(HTTPStatus.BAD_REQUEST)
            .set_length(847),
        RawRequest(
            b"POST /php/norminet.html HTTP/1.1\r\nHost: localhost\r\n\r\n",
        )
            .set_status(HTTPStatus.NOT_FOUND)
            .set_length(851),
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
        )
            .set_status(HTTPStatus.OK)
            .set_length(2630),
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
        )
            .set_status(HTTPStatus.OK)
            .set_length(2630),
        RawRequest(
            ("GET /python/quizz.py HTTP/1.1\r\n" + "Host: localhost\r\n").encode(),
            (
                RequestCookie("good", "0", {"SameSite": "Strict"}),
                RequestCookie("total", "0", {"SameSite": "Strict"}),
            ),
        )
            .set_status(HTTPStatus.OK)
            .set_length(1299),
        # 10
        RawRequest(
            (
                "GET /python/quizz.py?q_id=0&q_answer=Xavier+Niel HTTP/1.1\r\n"
                + "Host: localhost\r\n"
                + "Cookie: good=0; total=0; name=Droopy\r\n"
            ).encode(),
            (
                RequestCookie("good", "0", {"SameSite": "Strict"}),
                RequestCookie("total", "1", {"SameSite": "Strict"}),
                RequestCookie("name", "Droopy", {"SameSite": "Strict"}),
            ),
        )
            .set_status(HTTPStatus.OK)
            .set_length(1451),
        RawRequest(
            (
                "GET /python/quizz.py?q_id=4&q_answer=son+chat HTTP/1.1\r\n"
                + "Host: localhost\r\n"
                + "Cookie: good=3; total=4; name=Droopy\r\n"
            ).encode(),
            (
                RequestCookie("good", "3", {"SameSite": "Strict"}),
                RequestCookie("total", "5", {"SameSite": "Strict"}),
                RequestCookie("name", "Droopy", {"SameSite": "Strict"}),
            ),
        )
            .set_status(HTTPStatus.OK)
            .set_length(1509),
        RawRequest(
            (
                "GET /python/quizz.py?q_id=4&q_answer=notanexpectedvalue HTTP/1.1\r\n"
                + "Host: localhost\r\n"
                + "Cookie: good=3; total=4; name=Droopy\r\n"
            ).encode(),
            (
                RequestCookie("good", "3", {"SameSite": "Strict"}),
                RequestCookie("total", "5", {"SameSite": "Strict"}),
                RequestCookie("name", "Droopy", {"SameSite": "Strict"}),
            ),
        )
            .set_status(HTTPStatus.OK)
            .set_length(1519),
        RawRequest(
            (
                "GET /python/quizz.py?q_id=4&q_answer=notanexpectedvalue HTTP/1.1\r\n"
                + "Host: localhost\r\n"
                + "Cookie: bad=3; total=4; name=Droopy\r\n"
            ).encode(),
            (
                RequestCookie("bad", "3", {}),
                RequestCookie("good", "0", {"SameSite": "Strict"}),
                RequestCookie("total", "5", {"SameSite": "Strict"}),
                RequestCookie("name", "Droopy", {"SameSite": "Strict"}),
            ),
        )
            .set_status(HTTPStatus.OK)
            .set_length(1519),
        RawRequest(
            (
                "GET /python/quizz.py?q_id=4&q_answer=notanexpectedvalue HTTP/1.1\r\n"
                + "Host: localhost\r\n"
                + "Cookie: total=4; name=Droopy\r\n"
            ).encode(),
            (
                RequestCookie("good", "0", {"SameSite": "Strict"}),
                RequestCookie("total", "5", {"SameSite": "Strict"}),
                RequestCookie("name", "Droopy", {"SameSite": "Strict"}),
            ),
        )
            .set_status(HTTPStatus.OK)
            .set_length(1519),
        # 15
        RawRequest(
            (
                "GET /python/quizz.py?q_id=4&q_answer=notanexpectedvalue HTTP/1.1\r\n"
                + "Host: localhost\r\n"
                + "Cookie: good=3 total=4;name=Droopy\r\n"
            ).encode(),
            (
                RequestCookie("good", "3", {"SameSite": "Strict"}),
                RequestCookie("total", "5", {"SameSite": "Strict"}),
                RequestCookie("name", "Droopy", {"SameSite": "Strict"}),
            ),
        )
            .set_status(HTTPStatus.OK)
            .set_length(1519),
        RawRequest(
            (
                "GET /python/quizz.py?q_id=4&q_answer=notanexpectedvalue HTTP/1.1\r\n"
                + "Host: localhost\r\r"
                + "Cookie: good=3; total=4; name=Droopy\r\n"
            ).encode(),
            (
                RequestCookie("good", "0", {"SameSite": "Strict"}),
                RequestCookie("total", "0", {"SameSite": "Strict"}),
            ),
        )
            .set_status(HTTPStatus.OK)
            .set_length(1299),
    )

    passed = RawTester(2, host, port).proceed_requests("", requests)
    return (passed, len(requests))


def test_3():
    host = "localhost"
    port = 8081
    headers = {"Host": host}

    requests = (
        SimpleRequest("GET")
            .set_path("/html/")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(1457),
        SimpleRequest("GET")
            .set_path("/html/page/")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(2310),
        SimpleRequest("GET")
            .set_path("/html/page/delete/")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(1503),
        SimpleRequest("GET")
            .set_path("/html/page/delete/toDelete.html")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(478),
        SimpleRequest("GET")
            .set_path("/redir/anything")
            .set_headers(headers)
            .set_status(HTTPStatus.MOVED_PERMANENTLY)
            .set_length(0),
        SimpleRequest("GET")
            .set_path("/newRoot/newRoot.html")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(379),
        SimpleRequest("GET")
            .set_path("/newIndex/")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(370),
    )

    passed = SimpleTester(3, host, port).proceed_requests("", requests)
    return (passed, len(requests))


def test_not_delete():
    file = "/html/page/notToDelete.html"
    path = "../www" + file

    assert os.path.isfile(path)
    host = "localhost"
    port = 8081
    headers = {"Host": host}
    requests = (
        SimpleRequest("DELETE")
            .set_path(file)
            .set_headers(headers)
            .set_status(HTTPStatus.METHOD_NOT_ALLOWED)
            .set_length(285),
    )
    passed = SimpleTester(3, host, port).proceed_requests(
        "", requests
    ) and os.path.isfile(path)
    return (passed, len(requests))


def test_delete():
    file = "/html/page/delete/toDelete.html"
    path = "../www" + file

    assert os.path.isfile(path)
    host = "localhost"
    port = 8081
    headers = {"Host": host}
    requests = (
        SimpleRequest("DELETE")
            .set_path("/html/page/delete/toDelete.html")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(38)
            .set_server_test(lambda: not os.path.isfile(path)),
        SimpleRequest("DELETE")
            .set_path("/html/page/delete/toDelete.html")
            .set_headers(headers)
            .set_status(HTTPStatus.NOT_FOUND)
            .set_length(281),
        SimpleRequest("GET")
            .set_path("/html/page/delete/")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(1364),
        SimpleRequest("GET")
            .set_path("/html/page/")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(2310),
        SimpleRequest("DELETE")
            .set_path("/html/page/delete/toDelete.html")
            .set_headers(headers)
            .set_status(HTTPStatus.NOT_FOUND)
            .set_length(281),
    )

    passed = SimpleTester(3, host, port).proceed_requests("", requests)

    tu.cp_backup_to_delete("toDelete.html")
    return (passed, len(requests))


def test_4():
    host = "localhost"
    port = 8081
    headers = {"Host": host}

    requests = (
        SimpleRequest("GET")
            .set_path("/html/page/forbidden.html")
            .set_status(HTTPStatus.FORBIDDEN)
            .set_headers(headers)
            .set_length(304),
    )

    tu.chmod_in_page("forbidden.html", "000")
    passed = SimpleTester(4, host, port).proceed_requests("", requests)
    tu.chmod_in_page("forbidden.html", "644")

    return (passed, len(requests))


def test_5():
    host = "localhost"
    port = 8080

    kitty_1 = "kitty1.jpeg"
    kitty_2 = "kitty2.jpg"
    path = "../www/upload/"

    assert not os.path.isfile(path + kitty_1)
    assert not os.path.isfile(path + kitty_2)
    kitty_1_content, boundary_1, length_1 = tu.get_kitty_content(
        "../www/img/kitty1.jpeg"
    )
    kitty_2_content, boundary_2, length_2 = tu.get_kitty_content(
        "../www/img/kitty2.jpg"
    )

    requests_1 = (
        RawRequest(
            (
                "POST /html/kitty/success.html HTTP/1.1\r\n"
                + "Host: localhost\r\n"
                + f"Content-Type: multipart/form-data; boundary={boundary_1}\r\n"
                + f"Content-Length: {length_1}\r\n\r\n"
            ).encode() + kitty_1_content)
            .set_status(HTTPStatus.OK)
            .set_length(1284),
    )
    passed = RawTester(5, host, port).proceed_requests(
        "", requests_1
    ) and os.path.isfile(path + kitty_1)

    requests_2 = (
        RawRequest(
            (
                "POST /html/kitty/success.html HTTP/1.1\r\n"
                + "Host: localhost\r\n"
                + f"Content-Type: multipart/form-data; boundary={boundary_2}\r\n"
                + f"Content-Length: {length_2}\r\n\r\n"
            ).encode() + kitty_2_content)
            .set_status(HTTPStatus.REQUEST_ENTITY_TOO_LARGE)
            .set_length(877),
    )
    passed += RawTester(5, host, port).proceed_requests(
        "", requests_2
    ) and not os.path.isfile(path + kitty_2)
    tu.remove_from_upload("kitty1.jpeg")
    return (passed, len(requests_1) + len(requests_2))


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
            ("GET /upload/ HTTP/1.1\r\nHost: localhost\r\n\r\n").encode())
            .set_status(HTTPStatus.OK)
            .set_length(1444),

        RawRequest(
            (
                "POST /html/kitty/success.html HTTP/1.1\r\n"
                + "Host: localhost\r\n"
                + f"Content-Type: multipart/form-data; boundary={boundary_2} \r\n"
                + f"Content-Length: {length_2}\r\n\r\n"
            ).encode() + kitty_2_content)
            .set_status(HTTPStatus.OK)
            .set_length(1284),
        RawRequest(
            ("GET /upload/ HTTP/1.1\r\nHost: localhost\r\n\r\n").encode())
            .set_status(HTTPStatus.OK)
            .set_length(1444),
        RawRequest(
            (
                "POST /html/kitty/success.html HTTP/1.1\r\n"
                + "Host: localhost\r\n"
                + f"Content-Type: multipart/form-data; boundary={boundary_1}\r\n"
                + f"Content-Length: {length_1}\r\n\r\n"
            ).encode() + kitty_1_content)
            .set_status(HTTPStatus.OK)
            .set_length(1284),
        # 5
        RawRequest(
            ("GET /upload/ HTTP/1.1\r\nHost: localhost\r\n\r\n").encode())
            .set_status(HTTPStatus.OK)
            .set_length(1580),
        RawRequest(
            ("GET /upload/kitty2.jpg HTTP/1.1\r\nHost: localhost\r\n\r\n").encode())
            .set_status(HTTPStatus.OK)
            .set_length(178976),
    )

    passed = RawTester(6, host, port).proceed_requests("", requests)
    tu.remove_from_upload("kitty2.jpg")
    return (passed, len(requests))


def test_7():
    host = "localhost"
    port = 8080
    headers = {"Host": host}

    requests = (
        SimpleRequest("GET")
            .set_path("/")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(1146),
        SimpleRequest("GET")
            .set_path("/css/")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(2280),
        SimpleRequest("GET")
            .set_path("/upload/")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(1444),
        SimpleRequest("GET")
            .set_path("/img/")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(1146),
        SimpleRequest("GET")
            .set_path("/img/toDelete/")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(1350),
    )

    passed = SimpleTester(7, host, port).proceed_requests(
        "tests/conf_test/test_ok_5.conf", requests
    )
    return (passed, len(requests))
