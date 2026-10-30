# Chapter 28 An HTTP File Server

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../28-HTTP文件服务器.md)
<!-- END LANGUAGE NAVIGATION -->

The log tool reads complete input before producing output. Network input may arrive in fragments while other clients are waiting. Follow one request first, then interleave several connections.

The real project is [example/http-files](../../../example/http-files/). It binds 127.0.0.1 and serves three explicit text routes. This teaching subset supports GET, HEAD and connection-close responses. It does not implement TLS, request bodies, persistence, ranges or binary file service.

## 28.1 Run and Observe

From the repository root:

```sh
chali run example/http-files/src/main.chl -- 8080 example/http-files/public
```

Arguments after -- specify port and resource directory. All three files load before listening. Windows paths must use characters supported by the current file-opening path; text content is UTF-8. Browse http://127.0.0.1:8080/notes.txt or use curl -i, then request /missing and inspect 404. Interrupt the server from its terminal.

## 28.2 Delimit a Response

The wire uses CRLF at header line boundaries:

```text
HTTP/1.1 200 OK
Content-Type: text/plain; charset=utf-8
Content-Length: 6
Connection: close

hello
```

The body here is hello followed by a newline: six UTF-8 bytes. The status line identifies the result, headers describe the representation and length, and a blank line starts the body.

```chali
String response(int64 status, String contentType, String body, bool headOnly)
{
    String result = "HTTP/1.1 " + format("{}", status) + " " + reason(status) + "\r\n";
    result += "Content-Type: " + contentType + "\r\n";
    result += "Content-Length: " + format("{}", body.length()) + "\r\n";
    result += "Connection: close\r\nX-Content-Type-Options: nosniff\r\n";
    if (status == 405)
    {
        result += "Allow: GET, HEAD\r\n";
    }
    result += "\r\n";
    if (!headOnly)
    {
        result += body;
    }
    return result;
}
```

The first section builds status and headers. body.length() counts UTF-8 bytes, including for Chinese text. A 405 response adds Allow. The final headOnly branch omits the body for HEAD while retaining the complete representation length. Connection: close matches the implementation's lifecycle.

## 28.3 Prepare Fixed Resources at Startup

Asset stores route, contentType and body as strings. Loading before listening removes file access from request handling.

```chali
Result<List<Asset>, Error> loadAssets(String root)
{
    List<Asset> assets;
    assets.push(loadAsset(root, "/", "index.html", "text/html; charset=utf-8")?);
    assets.push(loadAsset(root, "/notes.txt", "notes.txt", "text/plain; charset=utf-8")?);
    assets.push(loadAsset(root, "/style.css", "style.css", "text/css; charset=utf-8")?);
    return Result<List<Asset>, Error>::Ok(assets);
}
```

The list receives three assets. Each ? propagates loading failure, and only complete success reaches Ok. loadAsset owns a local FileStream, enforces a 1 MiB limit and releases the file at scope exit. Network targets are never concatenated into disk paths; traversal-shaped targets do not match the fixed table.

## 28.4 Read the Parser in Four Sections

Request stores errorStatus, method and route. parseRequest starts with 400 and validates before reporting success:

1. Reject invalid control bytes and split CRLF lines. The request line must have three components.
2. Accept HTTP/1.0 or HTTP/1.1, require an origin-form target and reject fragments. HTTP/1.1 requires one nonempty Host.
3. Check conservative field names. Reject folded fields, Transfer-Encoding and nonzero Content-Length; this server does not parse bodies.
4. Accept GET or HEAD, strip the query for routing, use 405 for other methods and 505 for unsupported versions.

Read the full implementation in [http_files.chl](../../../example/http-files/src/http_files.chl). Trace a valid request, then remove Host and find the early return. This is the stated subset, not a complete HTTP implementation. Consult [RFC 9112](https://www.rfc-editor.org/rfc/rfc9112.html) for protocol syntax.

## 28.5 Select a Route

```chali
String reply(String head, List<Asset> const& assets)
{
    Request request = parseRequest(head);
    bool headOnly = request.method == "HEAD";
    if (request.errorStatus != 0)
    {
        return response(request.errorStatus, "text/plain; charset=utf-8", reason(request.errorStatus) + "\n", headOnly);
    }
    for (Asset asset in *assets)
    {
        if (asset.route == request.route)
        {
            return response(200, asset.contentType, asset.body, headOnly);
        }
    }
    return response(404, "text/plain; charset=utf-8", "Not Found\n", headOnly);
}
```

The first section returns parse errors before route lookup. The second determines HEAD behavior and compares routes against preloaded assets. A match produces 200; otherwise 404. It never attempts an arbitrary filename on disk. Request validity and resource existence are separate checks.

## 28.6 Connection State

Connection owns a socket, Reading or Writing phase, input and output buffers, a sent offset and a deadline. A closed socket marks an available slot.

Reading accumulates fragments until CRLF CRLF. Writing retains unsent bytes across partial writes. Headers are capped at 8192 bytes. Each phase has a total two-second deadline, without indefinite extension for trickled input.

## 28.7 Advance One Step

```chali
void advance(Connection mut& connection, List<Asset> const& assets)
{
    if (connection.phase == Phase::Reading)
    {
        String part = connection.socket.receiveAsync((int32)1024);
        if (!part.isEmpty())
        {
            int64 searchFrom = connection.input.length() - 3;
            if (searchFrom < 0) { searchFrom = 0; }
            connection.input += part;
            if (connection.input.length() > 8192)
            {
                startReply(connection, response(431, "text/plain", "Header too large\n", false));
                return;
            }
            int64 end = connection.input.indexOf("\r\n\r\n", searchFrom);
            if (end >= 0)
            {
                startReply(connection, reply(connection.input.substring(0, end + 4), assets));
                return;
            }
        }
        else if (connection.socket.status() != std::net::NET_PENDING)
        {
            connection.socket.close();
            return;
        }
        if (std::clock::nowMs() >= connection.deadline)
        {
            startReply(connection, response(408, "text/plain", "Request timed out\n", false));
        }
        return;
    }
    int64 count = connection.socket.sendAsync(connection.output, connection.sent);
    if (count > 0)
    {
        connection.sent += count;
    }
    if (connection.sent >= connection.output.length() || count < 0 || std::clock::nowMs() >= connection.deadline)
    {
        connection.socket.close();
        connection.input = "";
        connection.output = "";
    }
}
```

Reading appends a nonempty fragment, checks the cap and looks for a complete header. It prepares either 431 or a parsed response, without waiting for transmission to finish. An empty receive requires a status check: NET_PENDING means no data yet, while other states close the connection. A deadline prepares 408.

The header search uses String.indexOf beginning three bytes before the new fragment. This detects a delimiter split across reads without repeatedly converting and scanning the complete prefix.

Writing calls sendAsync and increments sent only for a positive count. Completion, error or deadline closes the socket and clears buffers. Retaining the offset is essential to avoiding repeated prefixes.

## 28.8 Interleave Eight Connections

Read serve in three sections:

1. Accept one peer without blocking. Move it into an empty slot and initialize state. Close excess peers rather than building an unbounded queue.
2. Visit all live slots and advance each once. A slow client's incomplete input does not block other slots.
3. Check the optional total accepted-connection limit, count active sockets and sleep briefly between rounds.

The eight-slot capacity differs from acceptLimit, which is a process-wide stopping condition. Zero means ongoing service. Socket ownership remains in one context. Interleaved waits do not parallelize CPU work across cores.

## 28.9 Verify Through Real Requests

[tests/chali_book_projects.py](../../../tests/chali_book_projects.py) exports once and tests real TCP responses, HEAD, UTF-8 length, queries, missing routes, unsupported methods, malformed requests and timeouts.

A fragmented-request case leaves one header incomplete while another client retrieves a file, then completes the first header. This directly checks nonblocking progression. Tests use port 0 and a bounded server that exits after its configured requests, avoiding fixed-port assumptions.

## 28.10 Exercises

1. Why use bytes for Content-Length?
2. Why retain sent after sendAsync?
3. What happens to /../secret.txt?
4. Why not pass sockets through createWith?

### Answers and checks

1. HTTP lengths count representation bytes, not visual characters.
2. Writes may be partial and must resume at the remaining suffix.
3. It misses the fixed table and produces 404 without disk path construction.
4. Sockets have ownership; this event loop advances them within their creating context.
