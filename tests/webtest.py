#!/usr/bin/env python3
"""
Test module for webserv
"""

from colors import Color
from webserver import WebServer
from webtest_cmdline_parsing import *
from webtest_conf_parsing import *


def okko(val):
    """
    return OK or KO string according to the boolean value received as argument
    """

    return Color.GREEN + "OK" + Color.ENDC if val else Color.RED + "KO" + Color.ENDC


def tester(fun):
    """
    run the fun test, prints infos and return 1 on success, 0 else.
    """
    ret = fun()
    print(fun.__name__, okko(ret))
    return int(ret)


def main():
    """
    Tests for Webserv main function.
    """
    # try:
    #     server = WebServer("conf/webserv_3rertzer.conf")
    # except RuntimeError as e:
    #     print({e})
    #     return 1
    total = 18
    total_ok = 0
    total_ok += tester(test_cmd_parsing_1)
    total_ok += tester(test_cmd_parsing_2)
    total_ok += tester(test_cmd_parsing_3)
    total_ok += tester(test_conf_parsing_1)
    total_ok += tester(test_conf_parsing_2)
    total_ok += tester(test_conf_parsing_3)
    total_ok += tester(test_conf_parsing_4)
    total_ok += tester(test_conf_parsing_5)
    total_ok += tester(test_conf_parsing_6)
    total_ok += tester(test_conf_parsing_7)
    total_ok += tester(test_conf_parsing_8)
    total_ok += tester(test_conf_parsing_9)
    total_ok += tester(test_conf_parsing_10)
    total_ok += tester(test_conf_parsing_11)
    total_ok += tester(test_conf_parsing_12)
    total_ok += tester(test_conf_parsing_13)
    total_ok += tester(test_conf_parsing_14)
    total_ok += tester(test_conf_parsing_15)
    print(f"{total_ok}/{total} tests passed")

    # server.run_for(8.0)
    # server.finish()
    # print("returned value, ", server.proc.args, server.proc.returncode)
    return 0


if __name__ == "__main__":
    main()
