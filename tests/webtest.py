#!/usr/bin/env python3
"""
Test module for webserv
"""

from testutils import *
from webtest_cmdline_parsing import *
from webtest_conf_parsing import *
from webtest_request import *


def main():
    """
    Tests for Webserv main function.
    """

    test_fun = (
        test_cmd_parsing_1,
        test_cmd_parsing_2,
        test_cmd_parsing_3,
        test_conf_parsing_1,
        test_conf_parsing_2,
        test_conf_parsing_3,
        test_conf_parsing_4,
        test_conf_parsing_5,
        test_conf_parsing_6,
        test_conf_parsing_7,
        test_conf_parsing_8,
        test_conf_parsing_9,
        test_conf_parsing_10,
        test_conf_parsing_11,
        test_conf_parsing_12,
        test_conf_parsing_13,
        test_conf_parsing_14,
        test_conf_parsing_15,
        test_request_1,
        test_request_2,
        test_request_3,
    )

    passed, total = map(sum, zip(*(tester(f) for f in test_fun)))
    print(f"{passed}/{total} tests passed")
    return 0


if __name__ == "__main__":
    main()
