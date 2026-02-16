"""
Webserv tests: test command line arguments parsing.
"""

from testutils import test_cmd
from webserver import WebServer


def test_conf_parsing_1():
    """
    Test a missing closing } in conf file.
    Notice that the error message isn't the right one.
    """
    server = test_cmd("tests/conf_test/test_ko_1.conf")
    assert isinstance(server, WebServer)
    output = server.proc.stdout.read()
    ret = server.proc.returncode
    return ret == 1 and output == "A close bracket appear that isn't open anywhere\n"


def test_conf_parsing_2():
    """
    Test a missing opening { in conf file.
    Notice that the error message isn't the right one.
    """
    server = test_cmd("tests/conf_test/test_ko_2.conf")
    assert isinstance(server, WebServer)
    output = server.proc.stdout.read()
    ret = server.proc.returncode
    return (
        ret == 1
        and output
        == "Line where ';' is missing: server  line = 1\nA line in the conf File isn't ending with a semicolon\n"
    )


def test_conf_parsing_3():
    """
    Test a missing ending ; in conf file.
    """
    server = test_cmd("tests/conf_test/test_ko_3.conf")
    assert isinstance(server, WebServer)
    output = server.proc.stdout.read()
    ret = server.proc.returncode
    return (
        ret == 1
        and output
        == "Line where ';' is missing: \tclient_max_body_size 1000000 line = 3\nA line in the conf File isn't ending with a semicolon\n"
    )


def test_conf_parsing_4():
    """
    test a missing listen instruction
    """
    server = test_cmd("tests/conf_test/test_ko_4.conf")
    assert isinstance(server, WebServer)
    output = server.proc.stdout.read()
    error = server.proc.stderr.read()
    ret = server.proc.returncode
    return (
        ret == 2
        and output == "-------------TEST SOCKET------------------\n\n"
        and error == "Error: socket failed\n"
    )


def test_conf_parsing_5():
    """
    test  missing root instruction
    """
    server = test_cmd("tests/conf_test/test_ko_5.conf")
    assert isinstance(server, WebServer)
    output = server.proc.stdout.read()
    error = server.proc.stderr.read()
    ret = server.proc.returncode
    return ret == 1 and output == "Error: Server parsing error\n" and error == ""


def test_conf_parsing_6():
    """
    test error_page 400 missing
    """
    server = test_cmd("tests/conf_test/test_ko_6.conf")
    assert isinstance(server, WebServer)
    output = server.proc.stdout.read()
    error = server.proc.stderr.read()
    ret = server.proc.returncode
    return ret == 1 and output == "Error: Server parsing error\n" and error == ""


def test_conf_parsing_7():
    """
    test error_page 403 missing
    """
    server = test_cmd("tests/conf_test/test_ko_7.conf")
    assert isinstance(server, WebServer)
    output = server.proc.stdout.read()
    error = server.proc.stderr.read()
    ret = server.proc.returncode
    return ret == 1 and output == "Error: Server parsing error\n" and error == ""


def test_conf_parsing_8():
    """
    test error_page 404 missing
    """
    server = test_cmd("tests/conf_test/test_ko_8.conf")
    assert isinstance(server, WebServer)
    output = server.proc.stdout.read()
    error = server.proc.stderr.read()
    ret = server.proc.returncode
    return ret == 1 and output == "Error: Server parsing error\n" and error == ""


def test_conf_parsing_9():
    """
    test error_page 405 missing
    """
    server = test_cmd("tests/conf_test/test_ko_9.conf")
    assert isinstance(server, WebServer)
    output = server.proc.stdout.read()
    error = server.proc.stderr.read()
    ret = server.proc.returncode
    return ret == 1 and output == "Error: Server parsing error\n" and error == ""


def test_conf_parsing_10():
    """
    test error_page 413 missing
    """
    server = test_cmd("tests/conf_test/test_ko_10.conf")
    assert isinstance(server, WebServer)
    output = server.proc.stdout.read()
    error = server.proc.stderr.read()
    ret = server.proc.returncode
    return ret == 1 and output == "Error: Server parsing error\n" and error == ""


def test_conf_parsing_11():
    """
    test error_page 500 missing
    """
    server = test_cmd("tests/conf_test/test_ko_11.conf")
    assert isinstance(server, WebServer)
    output = server.proc.stdout.read()
    error = server.proc.stderr.read()
    ret = server.proc.returncode
    return ret == 1 and output == "Error: Server parsing error\n" and error == ""


def test_conf_parsing_12():
    """
    test error_page 501 missing
    """
    server = test_cmd("tests/conf_test/test_ko_12.conf")
    assert isinstance(server, WebServer)
    output = server.proc.stdout.read()
    error = server.proc.stderr.read()
    ret = server.proc.returncode
    return ret == 1 and output == "Error: Server parsing error\n" and error == ""


def test_conf_parsing_13():
    """
    test error_page 505 missing
    """
    server = test_cmd("tests/conf_test/test_ko_13.conf")
    assert isinstance(server, WebServer)
    output = server.proc.stdout.read()
    error = server.proc.stderr.read()
    ret = server.proc.returncode
    return ret == 1 and output == "Error: Server parsing error\n" and error == ""


def test_conf_parsing_14():
    """
    extra line
    """
    server = test_cmd("tests/conf_test/test_ko_14.conf")
    assert isinstance(server, WebServer)
    output = server.proc.stdout.read()
    error = server.proc.stderr.read()
    ret = server.proc.returncode
    return (
        ret == 1
        and output
        == "line not know -> 	this line shouldn't be here; <- End of line not know\nError: Server parsing error\n"
        and error == ""
    )


def test_conf_parsing_15():
    """
    Max body size argument in NAN
    """
    server = test_cmd("tests/conf_test/test_ko_15.conf")
    assert isinstance(server, WebServer)
    output = server.proc.stdout.read()
    error = server.proc.stderr.read()
    ret = server.proc.returncode
    # print(f"|{ret}|{output}|{error}|")
    return (
        ret == 1
        and output
        == "maxBodySize[maxBodySize.length() - 1] N\nclient_max_body_size doesn't respect subject rules\nError: Server parsing error\n"
        and error == ""
    )
