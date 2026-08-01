import os.path
from http import HTTPStatus
from urllib.parse import urlencode

import testutils as tu
from rawrequest import RawRequest
from rawtester import RawTester
from requestcookie import RequestCookie
from simplerequest import SimpleRequest
from simpletester import SimpleTester


def test_get_post():
    """
    ! server behavior in subtest 6 (brutus_get) isn't satisfactory
    """
    host = "localhost"
    port = 8080
    headers = {"Host": host}

    requests = (
        SimpleRequest("GET")
            .set_description("GET request on /")
            .set_path("/")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(1146),
        SimpleRequest("GOT")
            .set_description("bad (GOT) method on /")
            .set_path("/")
            .set_headers(headers)
            .set_status(HTTPStatus.BAD_REQUEST)
            .set_length(847),
        SimpleRequest("GET")
            .set_description("GET request on non existing file")
            .set_path("/nowhere")
            .set_headers(headers)
            .set_status(HTTPStatus.NOT_FOUND)
            .set_length(851),
        SimpleRequest("GET")
            .set_description("GET request on existing html file")
            .set_path("/html/kitty/kitty.html")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(1469),
        # 5
        SimpleRequest("GET")
            .set_description("GET request on php cgi with query")
            .set_path("/php/cesar_get.php?texte_area=salut+Jules&cipher_key=2&cipher=chiffrer")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(2678),
        SimpleRequest("GET")
            .set_description("GET request on php cgi with query on non existing file")
            .set_path("/php/brutus_get.php?texte_area=salut+Jules&cipher_key=2&cipher=chiffrer")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(27),
        SimpleRequest("GET")
            .set_description("get request on / with wrong Host name")
            .set_path("/")
            .set_headers({"Host": "hostile"})
            .set_status(HTTPStatus.OK)
            .set_length(1146),
        SimpleRequest("POST")
            .set_description("POST request on php cgi with content")
            .set_path("/php/cesar_post.php")
            .set_headers({"Host": host, "Content-Type": "application/x-www-form-urlencoded"})
            .set_content(urlencode({"cipher_key": "3", "texte_area": "hello world", "cipher": "on"}))
            .set_status(HTTPStatus.OK)
            .set_length(2664),
        SimpleRequest("POST")
            .set_description("POST request on php cgi with content")
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
            .set_description("POST request on php cgi with bad value in content")
            .set_path("/php/cesar_post.php")
            .set_headers({"Host": host, "Content-Type": "application/x-www-form-urlencoded"})
            .set_content(urlencode({"cipher_key": "NAN", "texte_area": "hello world", "cipher": "on"}))
            .set_status(HTTPStatus.OK)
            .set_length(487),
        SimpleRequest("POST")
            .set_description("POST request on php cgi with incomplete content")
            .set_path("/php/cesar_post.php")
            .set_headers({"Host": host, "Content-Type": "application/x-www-form-urlencoded"})
            .set_content(urlencode({"texte_area": "hello world"}))
            .set_status(HTTPStatus.OK)
            .set_length(2630),
        SimpleRequest("POST")
            .set_description("POST request on php cgi with bad content")
            .set_path("/php/cesar_post.php")
            .set_headers({"Host": host, "Content-Type": "application/x-www-form-urlencoded"})
            .set_content(urlencode({"gloup": "3", "texte_area": "hello world", "cipher": "on"}))
            .set_status(HTTPStatus.OK)
            .set_length(2664),
        SimpleRequest("POST")
            .set_description("POST request on php content with text/html as Content-Type header")
            .set_path("/php/cesar_post.php")
            .set_headers({"Host": host, "Content-Type": "text/html"})
            .set_content(urlencode({"cipher_key": "3", "texte_area": "hello world", "cipher": "on"}))
            .set_status(HTTPStatus.OK)
            .set_length(2630),
    )

    passed = SimpleTester("get_post", host, port).proceed_requests("", requests)
    return (passed, len(requests))


def test_misc_and_post_cgi():
    host = "localhost"
    port = 8080

    requests = (
        RawRequest(b"\r\nHost: localhost\r\n\r\n", )
            .set_description("start line missing")
            .set_status(HTTPStatus.BAD_REQUEST)
            .set_length(847),
        RawRequest(
            b"GET / HTTP/6.1\r\nHost: localhost\r\n\r\n",
        )
            .set_description("Non existant HTTP version number in start line")
            .set_status(HTTPStatus.HTTP_VERSION_NOT_SUPPORTED)
            .set_length(864),
        RawRequest(
            b"PUT / HTTP/1.1\r\nHost: localhost\r\n\r\n",
        )
            .set_description("Not implemented PUT request")
            .set_status(HTTPStatus.NOT_IMPLEMENTED)
            .set_length(844),
        RawRequest(
            b"HEAD / HTTP/1.1\r\nHost: localhost\r\n\r\n",
        )
            .set_description("Not implemented HEAD request")
            .set_status(HTTPStatus.NOT_IMPLEMENTED)
            .set_length(844),
        # 5
        RawRequest(
            b"Thisisnotavalidrequest HTTP/1.1\r\nHost: localhost\r\n\r\n",
        )
            .set_description("Invalid start line")
            .set_status(HTTPStatus.BAD_REQUEST)
            .set_length(847),
        RawRequest(
            b"POST /php/norminet.html HTTP/1.1\r\nHost: localhost\r\n\r\n",
        )
            .set_description("POST request on non existing path")
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
            .set_description("POST cgi php request")
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
            .set_description("POST request on php cgi; Content-Length to short")
            .set_status(HTTPStatus.OK)
            .set_length(2630),
        RawRequest(
            ("GET /python/quizz.py HTTP/1.1\r\n" + "Host: localhost\r\n").encode(),
            (
                RequestCookie("good", "0", {"SameSite": "Strict"}),
                RequestCookie("total", "0", {"SameSite": "Strict"}),
            ),
        )
            .set_description("GET python cgi request with cookies")
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
            .set_description("GET python cgi with cookies and query")
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
            .set_description("GET python cgi request with query and cookies")
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
            .set_description("GET python cgi request with query and cookies")
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
            .set_description("GET python CGI request with query and cookies")
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
            .set_description("GET python cgi request with query and cookies")
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
            .set_description("GET python cgi request with query and cookies")
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
            .set_description("GET python cgi request with query and cookies")
            .set_status(HTTPStatus.OK)
            .set_length(1299),
    )

    passed = RawTester("misc_and_post_cgi", host, port).proceed_requests("", requests)
    return (passed, len(requests))


def test_get_misc():
    host = "localhost"
    port = 8081
    headers = {"Host": host}

    requests = (
        SimpleRequest("GET")
            .set_description("GET request on /html/ path (autoindex off)")
            .set_path("/html/")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(1457),
        SimpleRequest("GET")
            .set_description("GET request on /html/page/ path autoindex on")
            .set_path("/html/page/")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(2310),
        SimpleRequest("GET")
            .set_description("GET request on /html/page/delete/ path autoindex on")
            .set_path("/html/page/delete/")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(1503),
        SimpleRequest("GET")
            .set_description("GET request on /html/page/delete/toDelete.html path")
            .set_path("/html/page/delete/toDelete.html")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(478),
        SimpleRequest("GET")
            .set_description("GET request on /redir/anything moved permanently")
            .set_path("/redir/anything")
            .set_headers(headers)
            .set_status(HTTPStatus.MOVED_PERMANENTLY)
            .set_length(0),
        SimpleRequest("GET")
            .set_description("GET request on /newRoot/newRoot.html with another root directory")
            .set_path("/newRoot/newRoot.html")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(379),
        SimpleRequest("GET")
            .set_description("GET request on /newIndex/ with another index file")
            .set_path("/newIndex/")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(370),
    )

    passed = SimpleTester("GET_misc", host, port).proceed_requests("", requests)
    return (passed, len(requests))

def test_get_hostname():
    host = "localhost"
    port = 8080
    headers_localhost = {"Host": host}
    headers_default = {"Host": "default"}
    headers_another = {"Host": "anotherserver"}
    headers_badhost = {"Host": "badhostname"}

    requests = (
        SimpleRequest("GET")
            .set_description("GET request for default host")
            .set_path("/")
            .set_headers(headers_default)
            .set_status(HTTPStatus.OK)
            .set_length(1457),
        SimpleRequest("GET")
            .set_description("GET request for non default localhost")
            .set_path("/")
            .set_headers(headers_localhost)
            .set_status(HTTPStatus.OK)
            .set_length(1146),
        SimpleRequest("GET")
            .set_description("GET request for anotherserver host")
            .set_path("/")
            .set_headers(headers_another)
            .set_status(HTTPStatus.OK)
            .set_length(370),
        SimpleRequest("GET")
            .set_description("GET request for bad hostname, default expected")
            .set_path("/")
            .set_headers(headers_badhost)
            .set_status(HTTPStatus.OK)
            .set_length(1457),
    )

    passed = SimpleTester("GET_hostname", host, port).proceed_requests("tests/conf_test/test_ok_default_server.conf", requests)
    return (passed, len(requests))
 
def test_delete():
    file_not_to_delete = "/html/page/notToDelete.html"
    path_not_to_delete = "../www" + file_not_to_delete
    file_to_delete = "/html/page/delete/toDelete.html"
    path_to_delete = "../www" + file_to_delete

    assert os.path.isfile(path_not_to_delete)
    assert os.path.isfile(path_to_delete)
    host = "localhost"
    port = 8081
    headers = {"Host": host}
    requests = (
        SimpleRequest("DELETE")
            .set_description("DELETE request on /html/page/notToDelete.html, method not allowed")
            .set_path(file_not_to_delete)
            .set_headers(headers)
            .set_status(HTTPStatus.METHOD_NOT_ALLOWED)
            .set_length(285)
            .set_server_test(lambda: os.path.isfile(path_not_to_delete)),
        SimpleRequest("DELETE")
            .set_description("DELETE request on /html/page/delete/toDelelte.html, file deletion expected")
            .set_path(file_to_delete)
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(38)
            .set_server_test(lambda: not os.path.isfile(path_to_delete)),
        SimpleRequest("DELETE")
            .set_description("DELETE request on /html/page/delet/toDelete.html, not found expected")
            .set_path("/html/page/delete/toDelete.html")
            .set_headers(headers)
            .set_status(HTTPStatus.NOT_FOUND)
            .set_length(281),
        SimpleRequest("GET")
            .set_description("GET request on /html/page/delete/ autoindex on toDelete.html should not appear")
            .set_path("/html/page/delete/")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(1364),
        SimpleRequest("GET")
            .set_description("GET request on /html/page/ autoindex on")
            .set_path("/html/page/")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(2310)
            .set_post_test(lambda: tu.cp_backup_to_delete("toDelete.html")),
        SimpleRequest("GET")
            .set_description("GET request on /html/page/forbidden.html FORBIDDEN status expected")
            .set_path("/html/page/forbidden.html")
            .set_status(HTTPStatus.FORBIDDEN)
            .set_headers(headers)
            .set_length(304)
            .set_pre_test(lambda: tu.chmod_in_page("forbidden.html", "000"))
            .set_post_test(lambda: tu.chmod_in_page("forbidden.html", "644")),
    )

    passed = SimpleTester("delete", host, port).proceed_requests("", requests)
    
    return (passed, len(requests))


def test_multipart_upload_size_limit():
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

    requests = (
        RawRequest(
            (
                "POST /html/kitty/success.html HTTP/1.1\r\n"
                + "Host: localhost\r\n"
                + f"Content-Type: multipart/form-data; boundary={boundary_1}\r\n"
                + f"Content-Length: {length_1}\r\n\r\n"
            ).encode() + kitty_1_content)
            .set_description("POST request sending jpeg file to upload")
            .set_status(HTTPStatus.OK)
            .set_length(1284)
            .set_server_test(lambda: os.path.isfile(path + kitty_1))
            .set_post_test(lambda: tu.remove_from_upload("kitty1.jpeg")),
        RawRequest(
            (
                "POST /html/kitty/success.html HTTP/1.1\r\n"
                + "Host: localhost\r\n"
                + f"Content-Type: multipart/form-data; boundary={boundary_2}\r\n"
                + f"Content-Length: {length_2}\r\n\r\n"
            ).encode() + kitty_2_content)
            .set_description("POST request sending oversized jpeg file to upload, REQUEST_ENTITY_TOO_LARGE expected")
            .set_status(HTTPStatus.REQUEST_ENTITY_TOO_LARGE)
            .set_length(877)
            .set_server_test(lambda: not os.path.isfile(path + kitty_2)),
    )
    passed = RawTester("multipart_upload_size_limit", host, port).proceed_requests("", requests) 
    
    return (passed, len(requests))


def test_multipart_upload_boundary():
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
            .set_description("GET request on /upload/ autoindex on kitty2.jpg should not appear")
            .set_status(HTTPStatus.OK)
            .set_length(1444),
        RawRequest(
            (
                "POST /html/kitty/success.html HTTP/1.1\r\n"
                + "Host: localhost\r\n"
                + f"Content-Type: multipart/form-data; boundary={boundary_2} \r\n"
                + f"Content-Length: {length_2}\r\n\r\n"
            ).encode() + kitty_2_content)
            .set_description("POST request upload multipart/form-data, bad boundaries")
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
            .set_description("POST request upload multipart/form-data")
            .set_status(HTTPStatus.OK)
            .set_length(1284),
        # 5
        RawRequest(
            ("GET /upload/ HTTP/1.1\r\nHost: localhost\r\n\r\n").encode())
            .set_description("GET request on /upload/ autoindex on kitty2.jpg should appear")
            .set_status(HTTPStatus.OK)
            .set_length(1580),
        RawRequest(
            ("GET /upload/kitty2.jpg HTTP/1.1\r\nHost: localhost\r\n\r\n").encode())
            .set_description("GET request on /upload/kitty2.jpg, the previously uploaded file")
            .set_status(HTTPStatus.OK)
            .set_length(178976)
            .set_post_test(lambda: tu.remove_from_upload("kitty2.jpg")),
    )

    passed = RawTester("multipart_upload_boundary", host, port).proceed_requests("", requests)
    return (passed, len(requests))


def test_autoindex():
    host = "localhost"
    port = 8080
    headers = {"Host": host}

    requests = (
        SimpleRequest("GET")
            .set_description("GET request on / ; index file expected")
            .set_path("/")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(1146),
        SimpleRequest("GET")
            .set_description("GET request on directory with no autoindex indication autoindex file expected")
            .set_path("/css/")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(2280),
        SimpleRequest("GET")
            .set_description("GET request on directory with autoindex on")
            .set_path("/upload/")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(1444),
        SimpleRequest("GET")
            .set_description("GET request on directory with autoindex off")
            .set_path("/img/")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(1146),
        SimpleRequest("GET")
            .set_description("GET request on sub-directory with autoindex on in a directory with autoindex off")
            .set_path("/img/toDelete/")
            .set_headers(headers)
            .set_status(HTTPStatus.OK)
            .set_length(1350),
    )

    passed = SimpleTester("autoindex", host, port).proceed_requests(
        "tests/conf_test/test_ok_5.conf", requests
    )
    return (passed, len(requests))
