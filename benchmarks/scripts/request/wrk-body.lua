wrk.method = "POST"
wrk.headers["Content-Type"] = "text/plain"
wrk.body = string.rep("a", 1024)