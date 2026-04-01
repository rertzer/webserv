import io

import testutils as tu
from confrequest import ConfRequest
from webserver import WebServer


def test_cmdline_and_conf():
    tests = (
        # test 1
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
            "",
            "Error: Server parsing error.\n",
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
        # test 5
        ConfRequest(
            "tests/conf_test/test_ko_1.conf",
            1,
            "",
            "Error: Server parsing error.\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_2.conf",
            1,
            "",
            "Error: Server parsing error.\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_3.conf",
            1,
            "",
            "Error: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_4.conf",
            1,
            "",
            "'listen' missing\nError: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_5.conf",
            1,
            "",
            "Error: Server parsing error\n",
        ),
        # test 10
        ConfRequest(
            "tests/conf_test/test_ko_6.conf",
            1,
            "",
            "Error: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_7.conf",
            1,
            "",
            "Error: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_8.conf",
            1,
            "",
            "Error: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_9.conf",
            1,
            "",
            "Error: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_10.conf",
            1,
            "",
            "Error: Server parsing error\n",
        ),
        # test 15
        ConfRequest(
            "tests/conf_test/test_ko_11.conf",
            1,
            "",
            "Error: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_12.conf",
            1,
            "",
            "Error: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_13.conf",
            1,
            "",
            "Error: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_14.conf",
            1,
            "",
            "Unknown line: this\nError: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_15.conf",
            1,
            "",
            "Invalid client_max_body_size\nError: Server parsing error\n",
        ),
        # test 20
        ConfRequest(
            "tests/conf_test/test_ko_17.conf",
            2,
            "-------------TEST SOCKET------------------\n\n",
            "Error: socket failed\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_19.conf",
            1,
            "",
            "Servers with the same name must have different port numbers.\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_20.conf",
            1,
            "",
            "Error: Server parsing error.\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_21.conf",
            1,
            "",
            "Error: Server parsing error.\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_22.conf",
            1,
            "",
            "Autoindex needs to be on or off.\nError: Server parsing error\n",
        ),
        # test 25
        ConfRequest(
            "tests/conf_test/test_ko_23.conf",
            1,
            "",
            "Unknown line: badserver_name\nError: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_24.conf",
            1,
            "",
            "Unknown line: oops\nError: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_25.conf",
            1,
            "Not a valid command\n",
            "Error: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_26.conf",
            1,
            "",
            "Error: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_27.conf",
            1,
            "",
            "Error: Server parsing error.\n",
        ),
        # test 30
        ConfRequest(
            "tests/conf_test/test_ko_28.conf",
            1,
            "",
            "Unknown line: indeedaninvalidline\nError: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_29.conf",
            1,
            "",
            "Error page number out of range\nError: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_18.conf",
            1,
            "",
            "Error: Server parsing error.\n",
        ),
    )

    passed = sum(conf_tester(index + 1, test) for index, test in enumerate(tests))
    return passed, len(tests)


def conf_tester(index, test):
    server = WebServer.run_server(test.conf_file)
    assert isinstance(server, WebServer)
    assert isinstance(server.proc.stdout, io.TextIOWrapper)
    assert isinstance(server.proc.stderr, io.TextIOWrapper)
    server_status = server.proc.returncode
    server_output = server.proc.stdout.read()
    server_error = server.proc.stderr.read()
    print(server_status)
    print("|", server_output, "|")
    print("!", server_error, "!")

    ok = (
        server_status == test.status
        and server_output == test.stdout
        and server_error == test.stderr
    )
    tu.print_result("conf_1", index, ok)
    return ok
