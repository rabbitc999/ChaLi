# 第 28 章 HTTP文件服务器

<!-- LANGUAGE NAVIGATION -->
[English](en/28-http-file-server.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

日志工具把数据读完再输出。本章把输入换成网络字节：客户端可能分几次发送，同一时刻还可能有别的客户端等待。我们沿一次请求的路径安排程序，先得到能说明行为的响应，再处理多个连接。

完整工程在 [example/http-files](../../example/http-files/)。它提供首页、文字文件和样式三个明确路由，只绑定 127.0.0.1。它是文本文件服务的教学子集，支持 GET、HEAD 和关闭连接的响应，不实现 TLS、请求体、持久连接、Range 或二进制文件服务。

## 28.1 先运行再观察一个请求

在仓库根目录执行：

```sh
chali run example/http-files/src/main.chl -- 8080 example/http-files/public
```

-- 后的第一个参数是监听端口，第二个是文件目录。启动时加载三个文件，再建立监听器；任一文件读取失败，启动就失败，不带着缺失资源继续运行。Windows 当前文件打开路径应使用系统能识别的字符；文本内容使用 UTF-8。

浏览器打开 http://127.0.0.1:8080/notes.txt。也可以在另一终端使用 curl -i http://127.0.0.1:8080/notes.txt，观察状态行、响应头、空行和正文。访问 /missing 应得到 404。停止服务使用运行终端的中断操作。

## 28.2 给一段响应划出边界

响应形状如下，响应头的换行在网络中使用 CRLF：

```text
HTTP/1.1 200 OK
Content-Type: text/plain; charset=utf-8
Content-Length: 6
Connection: close

hello
```

这里假定正文是 hello 加一个换行，共六个 UTF-8 字节。状态行说明处理结果，Content-Type 说明如何解释内容，Content-Length 说明正文边界。空行结束响应头，随后才是正文。不能把中文字符个数当成字节数。

工程把响应生成集中在一个函数中：

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

第一段选择状态说明并拼接响应头。body.length() 取得 UTF-8 字节长度；长度基于完整表示计算。405 另外生成 Allow，告诉客户端可用方法。

最后的 headOnly 判断决定是否附加正文。HEAD 与 GET 对同一资源给出相同长度信息，但不发送正文；如果为了省事把长度也写成 0，就失去了这个含义。Connection: close 对应每次响应后关闭的实现，没有暗示支持持久连接。

## 28.3 启动时准备固定资源

Asset 用三个 String 保存 route、contentType、body：分别是请求路径、媒体类型和文件内容。把读取放在启动阶段，网络处理中就只需要选择已经准备好的资源。

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

第一条创建空列表，后面三条分别加载首页、文字和样式。每个 loadAsset 返回 Result，? 将失败交回调用者；只有三项全部成功，最后才构造 Ok。

loadAsset 在局部作用域拥有 FileStream，读取上限是 1 MiB，返回后文件资源按作用域清理。请求路径不会加入文件系统路径。/../secret.txt 和编码过的类似路径都不在固定路由表里，因此不会变成读取上级目录的操作。

## 28.4 把请求拆成可检查的部分

Request 保存 errorStatus、method 和 route。parseRequest 先把 errorStatus 设为 400；验证逐项成功后才返回有效请求。较长的解析函数可以按四段阅读：

1. validHeadBytes 拒绝 NUL、DEL 和不允许的控制字节。随后按 CRLF 拆行，把第一行按空格分为方法、目标和版本。
2. 检查 HTTP/1.0 或 HTTP/1.1，目标必须是以 / 开头的形式，不接收片段标识。HTTP/1.1 还要求唯一且非空的 Host。
3. 逐行检查头字段。教学实现只接受保守的字段名，不接收折叠字段、Transfer-Encoding 或非零 Content-Length，因此不进入请求体解析。
4. 方法只接受 GET、HEAD；路由去掉查询部分，但不把路径转换为任意磁盘文件名。其他方法给出 405，未知版本给出 505。

这四段的完整实现见 [http_files.chl](../../example/http-files/src/http_files.chl)。先跟着一个 GET /notes.txt HTTP/1.1 走完，再试着删除 Host，观察在哪一步提前返回。解析器接受的范围是本章明确的子集，不是完整 HTTP 规范的替代实现。协议格式可继续查阅 [RFC 9112](https://www.rfc-editor.org/rfc/rfc9112.html)。

## 28.5 选择路由并生成结果

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

第一段调用解析器。若已经得到错误状态，就生成对应的文字响应，后面不继续查路由。

第二段根据方法确定 headOnly，遍历启动时准备的 Asset。完全相等的 route 才命中，返回 200 及指定类型、内容。没有匹配项就返回 404；不去磁盘上尝试同名文件。这里将“请求是否有效”和“资源是否存在”分成两次判断，便于分别验证。

## 28.6 一个连接有哪些状态

Connection 保存套接字、Phase、输入缓冲、输出缓冲、已发送字节数与截止时间。Phase 只有 Reading 和 Writing；socket.isOpen() 为 false 表示该槽位空闲。

Reading 阶段不断积累收到的字节，直到出现 CRLF CRLF。单次 receiveAsync 只读一部分，请求行和头字段都可能跨越多次读取，不能把第一次拿到的内容直接当作完整请求。

Writing 阶段保留完整输出及 sent 偏移。sendAsync 可能只发送一部分；下一轮必须从 sent 继续，不能重复发送前缀。头部限制为 8192 字节，接收阶段与发送阶段各有两秒总时限，不因收到一小段输入无限续期。

## 28.7 每次只推进一小步

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

先看 Reading 分支。receiveAsync 返回非空片段时追加到 input，再检查大小和头结束标记。超过限制切换到 431 响应；收齐头部则调用 reply。此时只准备输出，不在同一函数里等待全部发送完。

查找头结束标记使用 String.indexOf，从新增片段之前的三个字节开始，既保留跨片段的四字节标记，又不反复转换并扫描整个旧缓冲。

没有片段时还要检查 socket.status()。NET_PENDING 表示暂时没数据；其他状态关闭连接。检查截止时间后，超时连接准备 408。该分支随后返回，允许外层处理下一位客户端。

再看函数下半段。Writing 调用 sendAsync，只有正的返回值才增加 sent。全部完成、发送错误或截止时间已到，都会关闭连接并清理缓冲。保留 sent 是正确处理部分写入的关键；连接关闭是槽位重新可用的条件。

## 28.8 轮流服务八个连接

serve 先声明 Connection connections[8]，每个槽位独立拥有自己的套接字。主循环分三段：

1. acceptAsync 尝试接入一位客户端。找到空槽时用 move peer 交接资源，初始化 Reading、空缓冲、偏移与截止时间。没有空槽则关闭新连接，不继续增加无界队列。
2. 遍历已有槽位，每个打开的连接调用一次 advance。一次没有进展就轮到下一个，慢客户端不会占住整个接收过程。
3. 核对是否达到可选的总接入次数，并统计仍活动的连接；全部处理完即可退出。普通运行的次数上限为 0，持续服务。每轮 sleepFor(1) 让出一小段等待时间。

这里的八个槽位是同时活动的容量；acceptLimit 是整个进程累计接入的停止条件，两者不是同一个数字。套接字始终在同一上下文中，事件循环没有把资源复制给其他线程。它能交错处理等待，不能保证八项 CPU 计算并行。

## 28.9 用可重复请求验证边界

[tests/chali_book_projects.py](../../tests/chali_book_projects.py) 导出一次应用，再通过真实 TCP 检查响应。除了首页，还核对 HEAD 无正文、中文 Content-Length、查询串、不存在的资源、不支持的方法、非法请求与超时。

分段发送的检查先发送半个头，保持连接打开；另一个客户端仍应收到文件。随后补完第一个头，仍能得到正确响应。这个顺序直接检验事件循环是否在某个客户端上阻塞。测试还使用端口 0 取得系统分配端口，并让有限接入的进程自行退出，避免依赖机器上恰好空闲的固定端口。

## 28.10 习题

1. 为什么中文正文长度用 length 而不是视觉字符数量？
2. 为什么 sendAsync 后要保留 sent？
3. /../secret.txt 怎样处理？
4. 为什么套接字不直接作为 createWith 参数？

### 参考解答与验收要点

1. HTTP Content-Length 计算表示字节，UTF-8 汉字可能占多个字节。
2. 一次写入可能只发送部分数据，下一轮需要继续剩余部分。
3. 它不在固定路由表中，返回 404，不拼成磁盘路径。
4. 套接字是有归属的资源；此工程在同一上下文中轮流推进连接。
