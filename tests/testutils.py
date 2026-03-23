import os
import subprocess
import sys
from http.client import HTTPResponse
from io import BytesIO

from colors import Color


def print_result(name, index, ok):
    print(f"test_{name}.{index}", okko(ok))


def okko(val):
    return Color.GREEN + "OK" + Color.ENDC if val else Color.RED + "KO" + Color.ENDC


def tester(fun):
    passed, total = fun()
    print(fun.__name__, "series", okko(passed == total))
    return (passed, total)


class FakeSocket(BytesIO):

    def makefile(self, *args, **kwargs):
        return self


def raw_to_http_response(raw: bytes) -> HTTPResponse:
    sock = FakeSocket(raw)
    response = HTTPResponse(sock)
    response.begin()
    return response


def read_content(resp):
    content = b""
    while True:
        chunk = resp.read(4096)
        if not chunk:
            break
        content += chunk

    return content


def get_kitty_content(filename):
    boundary = "----kittyBoundary1234"
    _, extension = filename.rsplit(".", 1)

    with open(filename, "rb") as f:
        file_data = f.read()

        body = (
            (
                f"--{boundary}\r\n"
                f'Content-Disposition: form-data; name="file"; filename="{os.path.basename(filename)}"\r\n'
                f"Content-Type: image/{extension}\r\n"
                "\r\n"
            ).encode()
            + file_data
            + (f"--{boundary}--\r\n").encode()
        )
    return body, boundary, len(body)


def get_wrong_kitty_content(filename):
    boundary_1 = "----kittyBoundary1234"
    boundary_2 = "----kittyBoundary5678"
    _, extension = filename.rsplit(".", 1)

    with open(filename, "rb") as f:
        file_data = f.read()

        body = (
            (
                # f"--{boundary_1}\r\n"
                f'Content-Disposition: form-data; name="file"; filename="{os.path.basename(filename)}"\r\n'
                f"Content-Type: image/{extension}\r\n"
                "\r\n"
            ).encode()
            + file_data
            + (f"--{boundary_2}--\r\n").encode()
        )
    return body, boundary_1, len(body)


def remove_from_upload(file):
    try:
        subprocess.run(
            ["rm", "-f", file],
            cwd="../www/upload/",
            check=True,
        )
    except subprocess.CalledProcessError as e:
        print("Deleting kitty failed:", e.returncode, file=sys.stderr)
        print(e.stderr, file=sys.stderr)


def chmod_in_page(file, mode):
    try:
        subprocess.run(
            ["chmod", mode, file],
            cwd="../www/html/page/",
            check=True,
        )
    except subprocess.CalledProcessError as e:
        print(f"Unable to change {file} rights:", e.returncode, file=sys.stderr)
        print(e.stderr, file=sys.stderr)


def cp_backup_to_delete(file):
    try:
        subprocess.run(
            ["cp", file + ".bak", file],
            cwd="../www/html/page/delete/",
            check=True,
        )
    except subprocess.CalledProcessError as e:
        print("Restoring the toDelelte file failed:", e.returncode, file=sys.stderr)
        print(e.stderr, file=sys.stderr)


def same_length(a, b):
    return len(a) == len(b)
