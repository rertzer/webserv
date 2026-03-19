#!/usr/bin/env python3
"""
Test module for webserv
"""
import webtest_cmdline_parsing as cmd_parsing
import webtest_conf_parsing as conf_parsing
import webtest_request as request
from testutils import tester


def main():
    """
    Tests for Webserv main function.
    """

    test_fun = (
        cmd_parsing.test_1,
        cmd_parsing.test_2,
        cmd_parsing.test_3,
        conf_parsing.test_1,
        conf_parsing.test_2,
        conf_parsing.test_3,
        conf_parsing.test_4,
        conf_parsing.test_5,
        conf_parsing.test_6,
        conf_parsing.test_7,
        conf_parsing.test_8,
        conf_parsing.test_9,
        conf_parsing.test_10,
        conf_parsing.test_11,
        conf_parsing.test_12,
        conf_parsing.test_13,
        conf_parsing.test_14,
        conf_parsing.test_15,
        request.test_1,
        request.test_2,
        request.test_3,
        request.test_4,
        request.test_5,
        request.test_6,
        request.test_7,
        request.test_8,
        request.test_9,
    )

    passed, total = map(sum, zip(*(tester(f) for f in test_fun)))
    print(f"{passed}/{total} tests passed")
    return 0


if __name__ == "__main__":
    main()
