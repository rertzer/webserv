"""
Webserv tests: test command line arguments parsing.
"""

from testutils import test_cmd
from webserver import WebServer


def test_cmd_parsing_1():
    """
    Test error message if to many args.
    """
    server = test_cmd("a b c d")
    assert isinstance(server, WebServer)
    output = server.proc.stdout.read()
    ret = server.proc.returncode

    return ret == 1 and output == "The program can have 1 parameter not more\n"


def test_cmd_parsing_2():
    """
    Test error message if arg is not .conf file.
    """
    server = test_cmd("my.bad")
    assert isinstance(server, WebServer)
    output = server.proc.stdout.read()
    ret = server.proc.returncode

    return ret == 1 and output == "The program needs a .conf parameter\n"


def test_cmd_parsing_3():
    """
    Test error message if .conf file do not exists.
    """
    server = test_cmd("youwillneverfindme.conf")
    assert isinstance(server, WebServer)
    output = server.proc.stdout.read()
    ret = server.proc.returncode

    return ret == 1 and output == "The file youwillneverfindme.conf doesn't exist\n"
