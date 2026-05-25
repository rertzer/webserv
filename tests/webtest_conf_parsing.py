import io
import os

import testutils as tu
from confrequest import ConfRequest
from webserver import WebServer


def test_cmdline_and_conf():
    tests = (
        # test 1
        ConfRequest(
            "a b c d",
            "",
            "The program can have 1 parameter not more\n",
        )
        .set_status(2),
        ConfRequest("my.bad", "", "The program needs a .conf parameter\n").set_status(2),
        ConfRequest(
            "youwillneverfindme.conf",
            "",
            "Cannot open file: youwillneverfindme.conf\n",
        )
        .set_status(3),
        ConfRequest(
            "tests/conf_test/test_ok_4.conf",
            "-------------TEST SOCKET------------------\n\n"
            "TCP socket 3 on port 8080 created\n"
            "TCP socket 4 on port 8081 created\n"
            "TCP socket 5 on port 8083 created\n"
            "Listening...\n",
            "",
        )
        .set_status(-15),
        # test 5
        ConfRequest(
            "tests/conf_test/test_ko_server_missing_close_bracket.conf",
            "",
            "Error: Server parsing error.\n",
        )
        .set_status(3),
        ConfRequest(
            "tests/conf_test/test_ko_server_missing_open_and_close_brackets.conf",
            "",
            "Error: Server parsing error.\n",
        )
        .set_status(3),
        ConfRequest(
            "tests/conf_test/test_ko_client_max_body_size_semicolon_missing.conf",
            "",
            "Unknown line: client_max_body_size\nError: Server parsing error\n",
        )
        .set_status(3),
        ConfRequest(
            "tests/conf_test/test_ko_listen_line_missing.conf",
            "",
            "'listen' missing\nError: Server parsing error\n",
        )
        .set_status(3),
        ConfRequest(
            "tests/conf_test/test_ko_root_line_missing.conf",
            "",
            "Error: Server parsing error\n",
        )
        .set_status(3),
        # test 10
        ConfRequest(
            "tests/conf_test/test_ko_index_line_missing.conf",
            "",
            "Error: Server parsing error\n",
        )
        .set_status(3),
        ConfRequest(
            "tests/conf_test/test_ko_server_line_missing.conf",
            "",
            "Error: Server parsing error\n",
        )
        .set_status(3),
        ConfRequest(
            "tests/conf_test/test_ko_error_400_line_missing.conf",
            "",
            "Error: Server parsing error\n",
        )
        .set_status(3),
        ConfRequest(
            "tests/conf_test/test_ko_error_403_line_missing.conf",
            "",
            "Error: Server parsing error\n",
        )
        .set_status(3),
        ConfRequest(
            "tests/conf_test/test_ko_error_404_line_missing.conf",
            "",
            "Error: Server parsing error\n",
        )
        .set_status(3),
        # test 15
        ConfRequest(
            "tests/conf_test/test_ko_error_405_line_missing.conf",
            "",
            "Error: Server parsing error\n",
        )
        .set_status(3),
        ConfRequest(
            "tests/conf_test/test_ko_error_413_line_missing.conf",
            "",
            "Error: Server parsing error\n",
        )
        .set_status(3),
        ConfRequest(
            "tests/conf_test/test_ko_error_500_line_missing.conf",
            "",
            "Error: Server parsing error\n",
        )
        .set_status(3),
        ConfRequest(
            "tests/conf_test/test_ko_error_501_line_missing.conf",
            "",
            "Error: Server parsing error\n",
        )
        .set_status(3),
        ConfRequest(
            "tests/conf_test/test_ko_error_505_line_missing.conf",
            "",
            "Error: Server parsing error\n",
        )
        .set_status(3),
        # test 20
        ConfRequest(
            "tests/conf_test/test_ko_extra_line.conf",
            "",
            "Unknown line: this\nError: Server parsing error\n",
        )
        .set_status(3),
        ConfRequest(
            "tests/conf_test/test_ko_max_body_size_as_NAN.conf",
            "",
            "Invalid client_max_body_size\nError: Server parsing error\n",
        )
        .set_status(3),
        ConfRequest(
            "tests/conf_test/test_ko_bad_port.conf",
            "-------------TEST SOCKET------------------\n\n",
            "Error: socket failed\n",
        )
        .set_status(6),
        ConfRequest(
            "tests/conf_test/test_ko_two_identical_servers.conf",
            "",
            "Servers with the same name must have different port numbers.\n",
        )
        .set_status(3),
        ConfRequest(
            "tests/conf_test/test_ko_server_in_a_server.conf",
            "",
            "Unknown line: server\nError: Server parsing error\n",
        )
        .set_status(3),
        # test 25
        ConfRequest(
            "tests/conf_test/test_ko_server_in_a_server_bad_end.conf",
            "",
            "Unknown line: server\nError: Server parsing error\n",
        )
        .set_status(3),
        ConfRequest(
            "tests/conf_test/test_ko_autoindex_as_kapput.conf",
            "",
            "Autoindex needs to be on or off.\nError: Server parsing error\n",
        )
        .set_status(3),
        ConfRequest(
            "tests/conf_test/test_ko_badservername.conf",
            "",
            "Unknown line: badserver_name\nError: Server parsing error\n",
        )
        .set_status(3),
        ConfRequest(
            "tests/conf_test/test_ko_line_starting_with_oops.conf",
            "",
            "Unknown line: oops\nError: Server parsing error\n",
        )
        .set_status(3),
        ConfRequest(
            "tests/conf_test/test_ko_autoindex_on_and_off.conf",
            "Not a valid command\n",
            "Error: Server parsing error\n",
        )
        .set_status(3),
        # test 30
        ConfRequest(
            "tests/conf_test/test_ko_extra_word_on_server_name_line.conf",
            "",
            "Error: Server parsing error\n",
        )
        .set_status(3),
        ConfRequest(
            "tests/conf_test/test_ko_extra_line_outside_server.conf",
            "",
            "Error: Server parsing error.\n",
        )
        .set_status(3),
        ConfRequest(
            "tests/conf_test/test_ko_invalid_line.conf",
            "",
            "Unknown line: indeedaninvalidline\nError: Server parsing error\n",
        )
        .set_status(3),
        ConfRequest(
            "tests/conf_test/test_ko_error_page_number_out_of_range.conf",
            "",
            "Error page number out of range\nError: Server parsing error\n",
        )
        .set_status(3),
        ConfRequest(
            "tests/conf_test/test_ko_empty_file.conf",
            "",
            "Error: Server parsing error.\n",
        )
        .set_status(3),
        ConfRequest(
        "tests/conf_test/test_ok_4.conf",
        "",
        "Cannot open file: data/index_header.html\n",
        )
        .set_status(3)
        .set_pre_test(lambda : tu.backup_data("index_header.html"))
        .set_post_test(lambda : tu.backup_data("index_header.html", False)),
    )

    passed = sum(conf_tester(index + 1, test) for index, test in enumerate(tests))

    return passed, len(tests) 


def conf_tester(index, test):
    if test.pre_test is not None:
        test.pre_test()
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
    tu.print_result(f"conf_1.{index}", os.path.basename(test.conf_file), ok)
    if test.post_test is not None:
        test.post_test()
    return ok
