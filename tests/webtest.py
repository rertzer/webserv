#!/usr/bin/env python3
import sys

import webtest_conf_parsing as conf_parsing
import webtest_request as request
from testutils import tester


def main():

    test_fun = (
        conf_parsing.test_cmdline_and_conf,
        request.test_get_post,
        request.test_misc_and_post_cgi,
        request.test_get_misc,
        request.test_get_hostname,
        request.test_delete,
        request.test_multipart_upload_boundary,
        request.test_multipart_upload_size_limit,
        request.test_autoindex,
    )

    passed, total = map(sum, zip(*(tester(f) for f in test_fun)))
    print(f"{passed}/{total} tests passed")
    return  total - passed


if __name__ == "__main__":
    sys.exit(main())
