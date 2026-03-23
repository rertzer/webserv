class RequestCookie:
    def __init__(self, name, value, params):
        self.name = name
        self.value = value
        self.params = params

    def test(self, resp_cookies):
        ok = True
        if (
            self.name not in resp_cookies.keys()
            or resp_cookies[self.name].value != self.value
        ):
            ok = False
        else:
            for k, v in self.params.items():
                if (
                    k.lower() not in resp_cookies[self.name].keys()
                    or v != resp_cookies[self.name][k.lower()]
                ):
                    ok = False
                    break
        return ok
