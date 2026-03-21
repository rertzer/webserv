#!/usr/bin/env python3
import webtest_conf_parsing as conf_parsing
import webtest_request as request
from testutils import tester


def main():

    test_fun = (
        conf_parsing.test_cmdline_and_conf,
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
