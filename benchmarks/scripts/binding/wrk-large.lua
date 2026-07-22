local file = assert(io.open("../../payloads/binding/binding-large.json", "rb"))
wrk.body = file:read("*all")
file:close()

wrk.method = "POST"
wrk.headers["Content-Type"] = "application/json"
