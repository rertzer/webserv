import io

import testutils as tu
from confrequest import ConfRequest
from webserver import WebServer


def test_cmdline_and_conf():
    tests = (
        ConfRequest(
            "a b c d",
            1,
            "The program can have 1 parameter not more\n",
            "",
        ),
        ConfRequest("my.bad", 1, "The program needs a .conf parameter\n", ""),
        ConfRequest(
            "youwillneverfindme.conf",
            1,
            "The file youwillneverfindme.conf doesn't exist\n",
            "",
        ),
        ConfRequest(
            "tests/conf_test/test_ok_4.conf",
            -15,
            "-------------TEST SOCKET------------------\n\n"
            "TCP socket 3 on port 8080 created\n"
            "TCP socket 4 on port 8081 created\n"
            "TCP socket 5 on port 8083 created\n"
            "Listening...\n",
            "",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_1.conf",
            1,
            "A close bracket appear that isn't open anywhere\n",
            "",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_2.conf",
            1,
            "Line where ';' is missing: server  line = 1\n"
            "A line in the conf File isn't ending with a semicolon\n",
            "",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_3.conf",
            1,
            "Line where ';' is missing: \tclient_max_body_size 1000000 line = 3\n"
            "A line in the conf File isn't ending with a semicolon\n",
            "",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_4.conf",
            2,
            "-------------TEST SOCKET------------------\n\n",
            "Error: socket failed\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_5.conf",
            1,
            "Error: Server parsing error\n",
            "",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_6.conf",
            1,
            "Error: Server parsing error\n",
            "",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_7.conf",
            1,
            "Error: Server parsing error\n",
            "",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_8.conf",
            1,
            "Error: Server parsing error\n",
            "",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_9.conf",
            1,
            "Error: Server parsing error\n",
            "",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_10.conf",
            1,
            "Error: Server parsing error\n",
            "",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_11.conf",
            1,
            "Error: Server parsing error\n",
            "",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_12.conf",
            1,
            "Error: Server parsing error\n",
            "",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_13.conf",
            1,
            "Error: Server parsing error\n",
            "",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_14.conf",
            1,
            "line not know -> 	this line shouldn't be here; <- End of line not know\n"
            "Error: Server parsing error\n",
            "",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_15.conf",
            1,
            "maxBodySize[maxBodySize.length() - 1] N\n"
            "client_max_body_size doesn't respect subject rules\n"
            "Error: Server parsing error\n",
            "",
        ),
    )

    passed = sum(conf_tester(index + 1, test) for index, test in enumerate(tests))
    return passed, len(tests)


def conf_tester(index, test):
    server = WebServer.run_server(test.conf_file)
    assert isinstance(server, WebServer)
    assert isinstance(server.proc.stdout, io.TextIOWrapper)
    assert isinstance(server.proc.stderr, io.TextIOWrapper)
    ok = (
        server.proc.returncode == test.status
        and server.proc.stdout.read() == test.stdout
        and server.proc.stderr.read() == test.stderr
    )
    tu.print_result("conf_1", index, ok)
    return ok
