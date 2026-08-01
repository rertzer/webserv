import io
import os

import testutils as tu
from colors import Color
from confrequest import ConfRequest
from webserver import WebServer


def test_cmdline_and_conf():
    tests = (
        # test 1
        ConfRequest("a b c d")
            .set_status(2)
            .set_stdout("")
            .set_stderr("The program can have 1 parameter not more\n"),

        ConfRequest("my.bad")
            .set_status(2)
            .set_stdout("")
            .set_stderr("The program needs a .conf parameter\n"),

        ConfRequest("youwillneverfindme.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Cannot open file: youwillneverfindme.conf\n"),

        ConfRequest("tests/conf_test/test_ok_4.conf")
            .set_status(-15)
            .set_stdout(
                "Created Server: Port: 8080, Hostname: localhost\n"
                "Created Server: Port: 8081, Hostname: localhost\n"
                "Created Server: Port: 8083, Hostname: \n\n"
                "TCP socket 3 on port 8080 created\n"
                "TCP socket 4 on port 8081 created\n"
                "TCP socket 5 on port 8083 created\n"
                "Listening...\n"
                )
            .set_stderr(""),

        # test 5
        ConfRequest("tests/conf_test/test_ko_server_missing_close_bracket.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Error: Server parsing error.\n"),

        ConfRequest("tests/conf_test/test_ko_server_missing_open_and_close_brackets.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Error: Server parsing error.\n"),

        ConfRequest("tests/conf_test/test_ko_client_max_body_size_semicolon_missing.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Unknown line: client_max_body_size\nError: Server parsing error\n"),

        ConfRequest("tests/conf_test/test_ko_listen_line_missing.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("'listen' missing\nError: Server parsing error\n"),

        ConfRequest("tests/conf_test/test_ko_root_line_missing.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Error: Server parsing error\n"),

        # test 10
        ConfRequest("tests/conf_test/test_ko_index_line_missing.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Error: Server parsing error\n"),

        ConfRequest("tests/conf_test/test_ko_server_line_missing.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Error: Server parsing error\n"),

        ConfRequest("tests/conf_test/test_ko_error_400_line_missing.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Error: Server parsing error\n"),

        ConfRequest("tests/conf_test/test_ko_error_403_line_missing.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Error: Server parsing error\n"),

        ConfRequest("tests/conf_test/test_ko_error_404_line_missing.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Error: Server parsing error\n"),

        # test 15
        ConfRequest("tests/conf_test/test_ko_error_405_line_missing.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Error: Server parsing error\n"),

        ConfRequest("tests/conf_test/test_ko_error_413_line_missing.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Error: Server parsing error\n"),

        ConfRequest("tests/conf_test/test_ko_error_500_line_missing.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Error: Server parsing error\n"),

        ConfRequest("tests/conf_test/test_ko_error_501_line_missing.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Error: Server parsing error\n"),

        ConfRequest("tests/conf_test/test_ko_error_505_line_missing.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Error: Server parsing error\n"),

        # test 20
        ConfRequest("tests/conf_test/test_ko_extra_line.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Unknown line: this\nError: Server parsing error\n"),

        ConfRequest("tests/conf_test/test_ko_max_body_size_as_NAN.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Invalid client_max_body_size\nError: Server parsing error\n"),

        ConfRequest("tests/conf_test/test_ko_bad_port.conf")
            .set_status(6)
            .set_stdout("Created Server: Port: 42, Hostname: localhost\n\n")
            .set_stderr( "Error: socket failed\n"),

        ConfRequest("tests/conf_test/test_ko_two_identical_servers.conf")
            .set_status(3)
            .set_stdout("Created Server: Port: 8080, Hostname: localhost\n"
                        "Created Server: Port: 8080, Hostname: localhost\n")
            .set_stderr("Servers with the same name must have different port numbers.\n"),

        ConfRequest("tests/conf_test/test_ko_server_in_a_server.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Unknown line: server\nError: Server parsing error\n"),

        # test 25
        ConfRequest("tests/conf_test/test_ko_server_in_a_server_bad_end.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Unknown line: server\nError: Server parsing error\n"),

        ConfRequest("tests/conf_test/test_ko_autoindex_as_kapput.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Autoindex needs to be on or off.\nError: Server parsing error\n"),

        ConfRequest("tests/conf_test/test_ko_badservername.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Unknown line: badserver_name\nError: Server parsing error\n"),

        ConfRequest("tests/conf_test/test_ko_line_starting_with_oops.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Unknown line: oops\nError: Server parsing error\n"),

        ConfRequest("tests/conf_test/test_ko_autoindex_on_and_off.conf")
            .set_status(3)
            .set_stdout("Not a valid command\n")
            .set_stderr("Error: Server parsing error\n"),

        # test 30
        ConfRequest("tests/conf_test/test_ko_extra_word_on_server_name_line.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Error: Server parsing error\n"),

        ConfRequest("tests/conf_test/test_ko_extra_line_outside_server.conf")
            .set_status(3)
            .set_stdout("Created Server: Port: 8080, Hostname: localhost\n")
            .set_stderr("Error: Server parsing error.\n"),

        ConfRequest("tests/conf_test/test_ko_invalid_line.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Unknown line: indeedaninvalidline\nError: Server parsing error\n"),

        ConfRequest("tests/conf_test/test_ko_error_page_number_out_of_range.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Error page number out of range\nError: Server parsing error\n"),

        ConfRequest("tests/conf_test/test_ko_empty_file.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Error: Server parsing error.\n"),

        ConfRequest("tests/conf_test/test_ok_4.conf")
            .set_status(3)
            .set_stdout("")
            .set_stderr("Cannot open file: data/index_header.html\n")
            .set_pre_test(lambda : tu.backup_data("index_header.html"))
            .set_post_test(lambda : tu.backup_data("index_header.html", False)),
        ConfRequest("tests/conf_test/test_ko_redirect.conf")
            .set_status(3)
            .set_stdout("Return status not a valid number\n")
            .set_stderr("Error: Server parsing error\n"),
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

    ok_status = server_status == test.status
    ok_out = server_output == test.stdout
    ok_err = server_error == test.stderr
    ok = ok_status and ok_out and ok_err    

    tu.print_result(f"conf_1.{index}", os.path.basename(test.conf_file), ok)
    if not ok_status:
        print(f"{Color.YELLOW}  status: expected: {Color.ENDC}{test.status}, found: {Color.ENDC}{server_status}")
    if not ok_out:
        print(f"{Color.YELLOW}  stdout:\n    expected: {Color.ENDC}{test.stdout}")
        print(f"{Color.YELLOW}    found: {Color.ENDC}{server_output}")
    if not ok_err:
        print(f"{Color.YELLOW}  stderr:\n    expected: {Color.ENDC}{test.stderr}")
        print(f" {Color.YELLOW}   found: {Color.ENDC}{server_error}")

    if test.post_test is not None:
        test.post_test()
    return ok
