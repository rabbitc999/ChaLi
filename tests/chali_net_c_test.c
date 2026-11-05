/** @file chali_net_c_test.c @brief 纯 C 网络服务的双平台回环与错误检查。 */
#include "../source/abi/chali_net.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#ifndef _WIN32
#include <fcntl.h>
#include <unistd.h>
#endif

/**
 * @brief 验证初始化、解析、连接、等待、二进制和关闭；无需外部网络。
 * @return 全部断言通过返回零。
 */
int main(void)
{
    assert(chali_net_init() && chali_net_init());
    const NetApi *api = chali_net_api();
    assert(chali_net_numeric_host("127.0.0.1"));
    assert(!chali_net_numeric_host("") && !chali_net_numeric_host("localhost"));
    unsigned char address[kAddrBuffer] = { 0 };
    assert(chali_net_resolve("999.0.0.1", 0, address) == 0);
    assert(*chali_net_error());
    assert(chali_net_resolve("localhost", 0, address) == 16);
    assert(address[0] == 2 && address[1] == 0);
    assert(chali_net_resolve("127.0.0.1", 0, address) == 16);
    intptr_t listener = api->create(2, 1, 6);
    assert(!chali_net_invalid(listener));
    assert(api->bind(listener, address, 16) == 0);
    assert(api->listen(listener, 8) == 0);
    int port = chali_net_local_port(listener);
    assert(port > 0 && port <= 65535);
    assert(chali_net_wait_readable(listener, 0) == 0);
    assert(chali_net_wait_readable(listener, 3) == 0);
    assert(chali_net_resolve("127.0.0.1", port, address) == 16);
    intptr_t client = api->create(2, 1, 6);
    assert(!chali_net_invalid(client));
    assert(api->connect(client, address, 16) == 0);
    assert(chali_net_wait_readable(listener, 1000) == 1);
    int address_length = sizeof(address);
    intptr_t server = api->accept(listener, address, &address_length);
    assert(!chali_net_invalid(server));
    assert(chali_net_local_port(server) == port);
    assert(chali_net_nonblocking(server, true));
    char received[4096];
    assert(api->recv(server, received, sizeof(received), 0) < 0);
    assert(chali_net_nonblocking(server, false));
    assert(chali_net_wait_readable(server, 0) == 0);
    assert(chali_net_wait_writable(client, 1000) == 1);
    char payload[4096];
    for (size_t i = 0; i < sizeof(payload); ++i) payload[i] = (char)(i * 37);
    int sent = 0;
    while (sent < (int)sizeof(payload))
    {
        int amount = api->send(client, payload + sent, sizeof(payload) - sent, 0);
        assert(amount > 0);
        sent += amount;
    }
    size_t used = 0;
    while (used < sizeof(received))
    {
        assert(chali_net_wait_readable(server, 1000) == 1);
        int amount = api->recv(server, received + used, sizeof(received) - used, 0);
        assert(amount > 0);
        used += (size_t)amount;
    }
    assert(memcmp(payload, received, sizeof(payload)) == 0);
#ifndef _WIN32
    /* poll 不使用 FD_SET，验证嵌入进程已有大量文件描述符时仍可等待。 */
    int high = fcntl((int)server, F_DUPFD, 2048);
    assert(high >= 2048);
    assert(chali_net_wait_writable(high, 0) == 1);
    close(high);
#endif
    assert(api->shutdown(client, 1) == 0);
    assert(chali_net_wait_readable(server, 1000) == 1);
    assert(api->recv(server, received, sizeof(received), 0) == 0);
    assert(api->close(client) == 0);
    assert(api->close(server) == 0);
    assert(api->close(listener) == 0);
    assert(chali_net_wait_readable(-1, 0) < 0);
    puts("C network: loopback, DNS, timeout, binary bytes, nonblocking, shutdown and descriptors passed");
    return 0;
}
