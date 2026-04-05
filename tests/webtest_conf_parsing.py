import io

import testutils as tu
from confrequest import ConfRequest
from webserver import WebServer


def test_cmdline_and_conf():
    tests = (
        # test 1
        ConfRequest(
            "a b c d",
            2,
            "",
            "The program can have 1 parameter not more\n",
        ),
        ConfRequest("my.bad", 2, "", "The program needs a .conf parameter\n"),
        ConfRequest(
            "youwillneverfindme.conf",
            3,
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
            "tests/conf_test/test_ko_server_missing_close_bracket.conf",
            3,
            "",
            "Error: Server parsing error.\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_server_missing_open_and_close_brackets.conf",
            3,
            "",
            "Error: Server parsing error.\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_client_max_body_size_semicolon_missing.conf",
            3,
            "",
            "Unknown line: client_max_body_size\nError: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_listen_line_missing.conf",
            3,
            "",
            "'listen' missing\nError: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_root_line_missing.conf",
            3,
            "",
            "Error: Server parsing error\n",
        ),
        # test 10
        ConfRequest(
            "tests/conf_test/test_ko_index_line_missing.conf",
            3,
            "",
            "Error: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_server_line_missing.conf",
            3,
            "",
            "Error: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_error_400_line_missing.conf",
            3,
            "",
            "Error: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_error_403_line_missing.conf",
            3,
            "",
            "Error: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_error_404_line_missing.conf",
            3,
            "",
            "Error: Server parsing error\n",
        ),
        # test 15
        ConfRequest(
            "tests/conf_test/test_ko_error_405_line_missing.conf",
            3,
            "",
            "Error: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_error_413_line_missing.conf",
            3,
            "",
            "Error: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_error_500_line_missing.conf",
            3,
            "",
            "Error: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_error_501_line_missing.conf",
            3,
            "",
            "Error: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_error_505_line_missing.conf",
            3,
            "",
            "Error: Server parsing error\n",
        ),
        # test 20
        ConfRequest(
            "tests/conf_test/test_ko_extra_line.conf",
            3,
            "",
            "Unknown line: this\nError: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_max_body_size_as_NAN.conf",
            3,
            "",
            "Invalid client_max_body_size\nError: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_bad_port.conf",
            6,
            "-------------TEST SOCKET------------------\n\n",
            "Error: socket failed\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_two_identical_servers.conf",
            3,
            "",
            "Servers with the same name must have different port numbers.\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_server_in_a_server.conf",
            3,
            "",
            "Error: Server parsing error.\n",
        ),
        # test 25
        ConfRequest(
            "tests/conf_test/test_ko_server_in_a_server_bad_end.conf",
            3,
            "",
            "Error: Server parsing error.\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_autoindex_as_kapput.conf",
            3,
            "",
            "Autoindex needs to be on or off.\nError: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_badservername.conf",
            3,
            "",
            "Unknown line: badserver_name\nError: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_line_starting_with_oops.conf",
            3,
            "",
            "Unknown line: oops\nError: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_autoindex_on_and_off.conf",
            3,
            "Not a valid command\n",
            "Error: Server parsing error\n",
        ),
        # test 30
        ConfRequest(
            "tests/conf_test/test_ko_extra_word_on_server_name_line.conf",
            3,
            "",
            "Error: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_extra_line_outside_server.conf",
            3,
            "",
            "Error: Server parsing error.\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_invalid_line.conf",
            3,
            "",
            "Unknown line: indeedaninvalidline\nError: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_error_page_number_out_of_range.conf",
            3,
            "",
            "Error page number out of range\nError: Server parsing error\n",
        ),
        ConfRequest(
            "tests/conf_test/test_ko_empty_file.conf",
            3,
            "",
            "Error: Server parsing error.\n",
        ),
    )

    passed = sum(conf_tester(index + 1, test) for index, test in enumerate(tests))

    test = ConfRequest(
        "tests/conf_test/test_ok_4.conf",
        3,
        "",
        "Cannot open file: data/index_header.html\n",
    )
    tu.backup_data("index_header.html")
    test_nb = len(tests) + 1
    passed += conf_tester(test_nb, test)
    tu.backup_data("index_header.html", False)
    return passed, test_nb


def conf_tester(index, test):
    server = WebServer.run_server(test.conf_file)
    assert isinstance(server, WebServer)
    assert isinstance(server.proc.stdout, io.TextIOWrapper)
    assert isinstance(server.proc.stderr, io.TextIOWrapper)
    server_status = server.proc.returncode
    server_output = server.proc.stdout.read()
    server_error = server.proc.stderr.read()
    # print(server_status)
    # print("|", server_output, "|")
    # print("!", server_error, "!")

    ok = (
        server_status == test.status
        and server_output == test.stdout
        and server_error == test.stderr
    )
    tu.print_result("conf_1", index, ok)
    return ok
