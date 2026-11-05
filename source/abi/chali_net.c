/**
 * @file chali_net.c
 * @brief 不依赖 C++ 运行库的平台套接字服务。
 */
#ifndef _WIN32
#define _POSIX_C_SOURCE 200112L
#endif
#include "chali_net.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#else
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/select.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>
#endif
/*
 * Windows select 使用定长句柄集合；POSIX 使用 poll，支持大于 FD_SETSIZE 的句柄。
 *
 *   Windows(winsock)：`{ u_int fd_count; SOCKET fd_array[FD_SETSIZE]; }`，
 *                      SOCKET = UINT_PTR 要求 8 字节对齐 → 数组从偏移 8 开始，
 *                      FD_SETSIZE 默认 64，与 kMaxSockets 一致；
 *   POSIX            ：位图，共 FD_SETSIZE(1024) 位。
 */
#if defined(_WIN32)
typedef struct NativeFdSet
{
    unsigned int count;
    uintptr_t items[kMaxSockets];
} NativeFdSet;

#else

#endif
static NetApi net_api;
static bool net_ready;
static char net_error[512];
/**
 * @brief 返回系统调用表。
 * @return 进程级函数表。
 */
const NetApi *chali_net_api(void) { return &net_api; }
/**
 * @brief 返回最近错误文本。
 * @return 固定缓冲区。
 */
char *chali_net_error(void) { return net_error; }
/**
 * @brief 在固定诊断缓冲内保存并终止错误文本。
 * @param message 要保存到错误缓冲的诊断文本。
 */
void chali_net_set_error(const char *message)
{
    snprintf(net_error, 512, "%s", message != NULL ? message : "");
}

/**
 * @brief 把最近一次套接字错误写进 net_error。
 * @param what 操作诊断前缀。
 */
void chali_net_capture_error(const char *what)
{
    const int code = net_api.last_error != NULL ? net_api.last_error() : 0;
#if defined(_WIN32)
    const char *kind = "错误";
    switch (code)
    {
    case 10035: kind = "暂时无数据（超时）"; break;         /* WSAEWOULDBLOCK */
    case 10036: kind = "操作进行中"; break;                 /* WSAEINPROGRESS */
    case 10053: kind = "连接被本机中止"; break;             /* WSAECONNABORTED */
    case 10054: kind = "连接被对端重置"; break;             /* WSAECONNRESET */
    case 10060: kind = "连接超时"; break;                   /* WSAETIMEDOUT */
    case 10061: kind = "连接被拒绝（端口没人监听）"; break; /* WSAECONNREFUSED */
    case 10065: kind = "主机不可达"; break;                 /* WSAEHOSTUNREACH */
    case 11001: kind = "找不到主机"; break;                 /* WSAHOST_NOT_FOUND */
    default: break;
    }
    snprintf(net_error, 512, "%s：%s（WSA 错误码 %d）", what, kind, code);
#else
    snprintf(net_error, 512, "%s：%s（errno %d）", what, strerror(code), code);
#endif
}

/* ── POSIX 侧的薄包装（签名对齐 Windows） ─────────────────────────────── */

#if !defined(_WIN32)

/**
 * @brief 为统一套接字接口提供 POSIX 平台的空初始化适配。
 * @param version 兼容 Windows 初始化接口的版本参数；POSIX 不使用。
 * @param data 兼容 Windows 初始化接口的输出参数；POSIX 不使用。
 * @return 固定返回零，POSIX 套接字不需要启动调用。
 */
static int unixStartup(unsigned short version, void *data)
{
    (void)version;
    (void)data;
    return 0;
}
/**
 * @brief 为统一套接字接口提供 POSIX 平台的空清理适配。
 * @return 固定返回零。
 */
static int unixCleanup(void) { return 0; }
/**
 * @brief 按指定协议创建 POSIX 套接字。
 * @param family 系统套接字地址族。
 * @param type 套接字类型。
 * @param protocol 系统套接字协议编号。
 * @return 新文件描述符；失败返回负值。
 */
static intptr_t unixCreate(int family, int type, int protocol)
{
    return ((intptr_t)(socket(family, type, protocol)));
}
/**
 * @brief 通过 POSIX 接口连接指定套接字地址。
 * @param fd 宿主套接字文件描述符。
 * @param address 系统套接字地址结构，可由接收接口回填。
 * @param length 输入或输出缓冲的字节长度。
 * @return 系统 connect 的状态码。
 */
static int unixConnect(intptr_t fd, const void *address, int length)
{
    return connect(((int)(fd)), ((const struct sockaddr *)(address)), ((socklen_t)(length)));
}
/**
 * @brief 通过 POSIX 接口为套接字绑定本地地址。
 * @param fd 宿主套接字文件描述符。
 * @param address 系统套接字地址结构，可由接收接口回填。
 * @param length 输入或输出缓冲的字节长度。
 * @return 系统 bind 的状态码。
 */
static int unixBind(intptr_t fd, const void *address, int length)
{
    return bind(((int)(fd)), ((const struct sockaddr *)(address)), ((socklen_t)(length)));
}
/**
 * @brief 通过 POSIX 接口将套接字设为监听状态。
 * @param fd 宿主套接字文件描述符。
 * @param backlog 监听队列的容量上限。
 * @return 系统 listen 的状态码。
 */
static int unixListen(intptr_t fd, int backlog) { return listen(((int)(fd)), backlog); }
/**
 * @brief 通过 POSIX 接口接收连接并回填对端地址。
 * @param fd 宿主套接字文件描述符。
 * @param address 系统套接字地址结构，可由接收接口回填。
 * @param length 输入地址或选项缓冲容量，并接收实际字节长度。
 * @return 接收连接的文件描述符；失败返回负值。
 */
static intptr_t unixAccept(intptr_t fd, void *address, int *length)
{
    return ((intptr_t)(accept(((int)(fd)),
                              ((struct sockaddr *)(address)),
                              ((socklen_t *)(length)))));
}
/**
 * @brief 通过 POSIX 接口发送指定字节区间。
 * @param fd 宿主套接字文件描述符。
 * @param data 输入数值或待发送字节的起始地址。
 * @param length 输入或输出缓冲的字节长度。
 * @param flags 传递给系统收发接口的标志位。
 * @return 实际发送字节数；失败返回负值。
 */
static int unixSend(intptr_t fd, const char *data, int length, int flags)
{
    return ((int)(send(((int)(fd)), data, ((size_t)(length)), flags)));
}
/**
 * @brief 通过 POSIX 接口接收字节到调用方缓冲。
 * @param fd 宿主套接字文件描述符。
 * @param buffer 接收字节或诊断文本的输出缓冲。
 * @param length 输入或输出缓冲的字节长度。
 * @param flags 传递给系统收发接口的标志位。
 * @return 实际接收字节数；零表示关闭，负值表示失败。
 */
static int unixRecv(intptr_t fd, char *buffer, int length, int flags)
{
    return ((int)(recv(((int)(fd)), buffer, ((size_t)(length)), flags)));
}
/**
 * @brief 通过 POSIX 接口关闭套接字文件描述符。
 * @param fd 宿主套接字文件描述符。
 * @return 系统 close 的状态码。
 */
static int unixClose(intptr_t fd) { return close(((int)(fd))); }
/**
 * @brief 通过 POSIX 接口关闭套接字的指定通信方向。
 * @param fd 宿主套接字文件描述符。
 * @param how 需要关闭的套接字通信方向。
 * @return 系统 shutdown 的状态码。
 */
static int unixShutdown(intptr_t fd, int how) { return shutdown(((int)(fd)), how); }
/**
 * @brief 等待 POSIX 描述符集合中的可读、可写或异常事件。
 * @param nfds 最大描述符编号加一。
 * @param readSet 输入并回填可读描述符集合，可为空。
 * @param writeSet 输入并回填可写描述符集合，可为空。
 * @param exceptSet 输入并回填异常描述符集合，可为空。
 * @param timeout 系统调用的等待期限，可为空。
 * @return 就绪描述符数；零表示超时，负值表示失败。
 */
static int unixSelect(int nfds, void *readSet, void *writeSet, void *exceptSet, void *timeout)
{
    return select(nfds, ((fd_set *)(readSet)), ((fd_set *)(writeSet)),
                  ((fd_set *)(exceptSet)), ((struct timeval *)(timeout)));
}
/**
 * @brief 通过 POSIX 名称解析接口创建地址链表。
 * @param node 待解析的主机名或地址文本。
 * @param service 服务名称或端口文本，可为空。
 * @param hints 名称解析的地址族和协议约束，可为空。
 * @param result 接收解析结果链表或需要释放的结果对象。
 * @return 名称解析状态码，零表示成功。
 */
static int unixGetAddrInfo(const char *node, const char *service, const void *hints, void **result)
{
    return getaddrinfo(node, service, ((const struct addrinfo *)(hints)),
                       ((struct addrinfo **)(result)));
}
/**
 * @brief 释放 POSIX 名称解析返回的地址链表。
 * @param result 名称解析成功后取得的地址链表。
 */
static void unixFreeAddrInfo(void *result) { freeaddrinfo(((struct addrinfo *)(result))); }
/**
 * @brief 将主机字节序的 16 位整数转换为网络字节序。
 * @param value 需要转换为网络字节序的主机整数。
 * @return 网络字节序的整数位模式。
 */
static unsigned short unixHtons(unsigned short value) { return htons(value); }
/**
 * @brief 将 IPv4 文本地址转换为网络字节序整数。
 * @param text 待解析或转换的完整输入文本。
 * @return 系统 inet_addr 返回的地址位模式。
 */
static unsigned long unixInetAddr(const char *text) { return inet_addr(text); }
/**
 * @brief 读取最近一次 POSIX 网络调用的线程错误码。
 * @return 当前 errno 值。
 */
static int unixLastError() { return errno; }
/**
 * @brief 使用 POSIX fcntl 设置套接字的非阻塞状态。
 * @param command 兼容 ioctl 接口的命令编号；本适配器只实现非阻塞切换。
 * @param fd 宿主套接字文件描述符。
 * @param argument 输入所需的非阻塞状态。
 * @return 修改成功返回零，查询或修改失败返回负值。
 */
static int unixIoctl(intptr_t fd, long command, unsigned long *argument)
{
    (void)command;
    const int flags = fcntl(((int)(fd)), F_GETFL, 0);
    if (flags < 0) return -1;
    const int updated = (argument != NULL && *argument != 0ul) ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
    return fcntl(((int)(fd)), F_SETFL, updated);
}
/**
 * @brief 读取 POSIX 套接字选项并回填实际长度。
 * @param fd 宿主套接字文件描述符。
 * @param level 套接字选项所属的协议层。
 * @param name 需要绑定或解析的入口名称。
 * @param value 读取成功时接收解析值，可为空。
 * @param length 输入地址或选项缓冲容量，并接收实际字节长度。
 * @return 系统 getsockopt 的状态码。
 */
static int unixGetOpt(intptr_t fd, int level, int name, void *value, int *length)
{
    socklen_t native = length != NULL ? ((socklen_t)(*length)) : 0;
    const int result = getsockopt(((int)(fd)), level, name, value, &native);
    if (length != NULL) *length = ((int)(native));
    return result;
}

#endif

/* ── 初始化 ───────────────────────────────────────────────────────────── */

/**
 * @brief 按宿主平台装载套接字函数并初始化一次网络服务。
 * @return 初始化成功为真，失败保存平台诊断并返回假。
 */
bool chali_net_init(void)
{
    if (net_ready) return true;
    NetApi *table = &net_api;
#if defined(_WIN32)
    HMODULE library = LoadLibraryA("ws2_32.dll");
    if (library == NULL)
    {
        chali_net_set_error("找不到 ws2_32.dll（Windows 套接字库）");
        return false;
    }
    table->startup = ((StartupFn)(((void *)(GetProcAddress(library, "WSAStartup")))));
    table->cleanup = ((CleanupFn)(((void *)(GetProcAddress(library, "WSACleanup")))));
    table->create = ((CreateFn)(((void *)(GetProcAddress(library, "socket")))));
    table->connect = ((ConnectFn)(((void *)(GetProcAddress(library, "connect")))));
    table->bind = ((BindFn)(((void *)(GetProcAddress(library, "bind")))));
    table->listen = ((ListenFn)(((void *)(GetProcAddress(library, "listen")))));
    table->accept = ((AcceptFn)(((void *)(GetProcAddress(library, "accept")))));
    table->send = ((SendFn)(((void *)(GetProcAddress(library, "send")))));
    table->recv = ((RecvFn)(((void *)(GetProcAddress(library, "recv")))));
    table->close = ((CloseFn)(((void *)(GetProcAddress(library, "closesocket")))));
    table->shutdown = ((ShutdownFn)(((void *)(GetProcAddress(library, "shutdown")))));
    table->select = ((SelectFn)(((void *)(GetProcAddress(library, "select")))));
    table->getaddrinfo = ((AddrInfoFn)(((void *)(GetProcAddress(library, "getaddrinfo")))));
    table->freeaddrinfo = ((FreeAddrInfoFn)(((void *)(GetProcAddress(library, "freeaddrinfo")))));
    table->hostToNetwork16 = ((HtonsFn)(((void *)(GetProcAddress(library, "htons")))));
    table->inet_addr = ((InetAddrFn)(((void *)(GetProcAddress(library, "inet_addr")))));
    table->last_error = ((LastErrorFn)(((void *)(GetProcAddress(library, "WSAGetLastError")))));
    table->ioctl = ((IoctlFn)(((void *)(GetProcAddress(library, "ioctlsocket")))));
    table->getopt = ((GetOptFn)(((void *)(GetProcAddress(library, "getsockopt")))));
    if (!table->startup || !table->cleanup || !table->create || !table->connect || !table->bind || !table->listen || !table->accept || !table->send || !table->recv || !table->close || !table->shutdown || !table->select || !table->getaddrinfo || !table->freeaddrinfo || !table->hostToNetwork16 || !table->inet_addr || !table->last_error || !table->ioctl || !table->getopt)
    {
        chali_net_set_error("ws2_32.dll 缺少需要的符号");
        FreeLibrary(library);
        return false;
    }
    WSADATA data;
    const int started = table->startup(0x0202, &data);
    if (started != 0)
    {
        chali_net_set_error("WSAStartup 失败");
        FreeLibrary(library);
        return false;
    }
#else
    table->startup = unixStartup;
    table->cleanup = unixCleanup;
    table->create = unixCreate;
    table->connect = unixConnect;
    table->bind = unixBind;
    table->listen = unixListen;
    table->accept = unixAccept;
    table->send = unixSend;
    table->recv = unixRecv;
    table->close = unixClose;
    table->shutdown = unixShutdown;
    table->select = unixSelect;
    table->getaddrinfo = unixGetAddrInfo;
    table->freeaddrinfo = unixFreeAddrInfo;
    table->hostToNetwork16 = unixHtons;
    table->inet_addr = unixInetAddr;
    table->last_error = unixLastError;
    table->ioctl = unixIoctl;
    table->getopt = unixGetOpt;
#endif
    net_ready = true;
    return true;
}

/**
 * @brief 按当前平台检查套接字的无效哨兵值。
 * @param fd 宿主套接字文件描述符。
 * @return 描述符无效时为 true。
 */
bool chali_net_invalid(intptr_t fd)
{
#if defined(_WIN32)
    return fd == ((intptr_t)(~((uintptr_t)(0))));
#else
    return fd < 0;
#endif
}

/**
 * @brief 使用栈上事件集合等待单个套接字，无临时堆分配。
 * @param fd 平台套接字。
 * @param timeout_ms 超时毫秒数；负数无限等待。
 * @param writable 是否等待写入或连接完成。
 * @return 就绪为一，超时为零，错误为负一。
 */
static int wait_socket(intptr_t fd, int timeout_ms, bool writable)
{
    if (!chali_net_init() || chali_net_invalid(fd)) return -1;
#ifdef _WIN32
    NativeFdSet ready_set = { 1, { (uintptr_t)fd } };
    NativeFdSet error_set = { 1, { (uintptr_t)fd } };
    struct timeval timeout = { timeout_ms / 1000, (timeout_ms % 1000) * 1000 };
    int ready = net_api.select(0, writable ? NULL : &ready_set, writable ? &ready_set : NULL,
                               writable ? &error_set : NULL, timeout_ms < 0 ? NULL : &timeout);
#else
    struct pollfd item = { (int)fd, writable ? POLLOUT : POLLIN, 0 };
    int ready = poll(&item, 1, timeout_ms < 0 ? -1 : timeout_ms);
    if (ready > 0 && (item.revents & POLLNVAL))
    {
        errno = EBADF;
        ready = -1;
    }
#endif
    if (ready < 0) chali_net_capture_error(writable ? "等待可写" : "等待可读");
    return ready > 0 ? 1 : ready;
}
/**
 * @brief 等待套接字可读。
 * @param fd 平台套接字。
 * @param timeoutMs 超时毫秒数。
 * @return 一表示就绪，零表示超时，负一表示错误。
 */
int chali_net_wait_readable(intptr_t fd, int timeoutMs) { return wait_socket(fd, timeoutMs, false); }
/**
 * @brief 等待套接字可写。
 * @param fd 平台套接字。
 * @param timeoutMs 超时毫秒数。
 * @return 一表示就绪，零表示超时，负一表示错误。
 */
int chali_net_wait_writable(intptr_t fd, int timeoutMs) { return wait_socket(fd, timeoutMs, true); }

/**
 * @brief 开关非阻塞模式。
 * @param fd 平台套接字。
 * @param enabled 真表示启用非阻塞，假表示恢复阻塞。
 * @return 模式切换成功为真，缺少入口或系统调用失败为假。
 */
bool chali_net_nonblocking(intptr_t fd, bool enabled)
{
    unsigned long mode = enabled ? 1ul : 0ul;
    return net_api.ioctl != NULL && net_api.ioctl(fd, 0x8004667EL /* FIONBIO */, &mode) == 0;
}

/* ── 地址解析 ─────────────────────────────────────────────────────────── */

/**
 * @brief 检查非空地址文本是否只包含数字和点号，不执行名称解析。
 * @param host 主机名或数值地址。
 * @return 文本非空且仅含数字与点号时为真；IPv4 有效性由后续系统解析确认。
 */
bool chali_net_numeric_host(const char *host)
{
    if (!host || !*host) return false;
    for (size_t i = 0; host[i]; ++i)
        if ((host[i] < '0' || host[i] > '9') && host[i] != '.') return false;
    return true;
}

/**
 * @brief 读取当前平台 addrinfo 节点的地址族字段。
 * @param info 名称解析链表中的节点。
 * @return 系统地址族编号。
 */
static int addrInfoFamily(const void *info)
{
    return ((const struct addrinfo *)info)->ai_family;
}

/**
 * @brief 按当前平台布局读取解析结果中的套接字地址。
 * @param info 当前平台名称解析链表中的节点。
 * @return 地址结构的借用指针。
 */
static const void *addrInfoAddr(const void *info)
{
    return ((const struct addrinfo *)info)->ai_addr;
}

/**
 * @brief 按当前平台布局读取名称解析链表的下一节点。
 * @param info 当前平台名称解析链表中的节点。
 * @return 下一节点地址；链尾返回空。
 */
static const void *addrInfoNext(const void *info)
{
    return ((const struct addrinfo *)info)->ai_next;
}

/**
 * @brief 取得指定地址族对应的套接字地址结构长度。
 * @param family 系统套接字地址族。
 * @return 适用于 IPv4 或 IPv6 系统调用的字节数。
 */
static int addressLength(int family)
{
    return family == 2 ? 16 : 28; /* AF_INET / AF_INET6 */
}

/**
 * @brief 把主机名 + 端口解析成 sockaddr 字节。
 * @return 地址长度；失败 0。
 * @param host 主机名或数值地址。
 * @param port 端口。
 * @param buffer 接收 sockaddr 字节的可写缓冲，至少 kAddrBuffer 字节。
 */
int chali_net_resolve(const char *host, int port, unsigned char *buffer)
{
    if (!buffer || !chali_net_init()) return 0;
    const char *target = (!host || strcmp(host, "*") == 0 || strcmp(host, "0.0.0.0") == 0 || strcmp(host, "any") == 0) ? "" : host;
    if ((!*target) || chali_net_numeric_host(target))
    {
        /* sa_family 是 2 字节小端值：AF_INET = 2 → 字节序列 02 00 */
        buffer[0] = 2;
        buffer[1] = 0;
        const unsigned short networkPort = net_api.hostToNetwork16(((unsigned short)(port)));
        memcpy(buffer + 2, &networkPort, 2);
        const unsigned long address = (!*target) ? 0u : net_api.inet_addr(target);
        if ((!*target))
        {
            memset(buffer + 4, 0, 4);
        }
        else
        {
            memcpy(buffer + 4, &address, 4);
            if (address == 0xFFFFFFFFul)
            {
                chali_net_set_error("无效的 IP 地址");
                return 0;
            }
        }
        memset(buffer + 8, 0, 8);
        return 16;
    }
    void *result = NULL;
    if (net_api.getaddrinfo(target, NULL, NULL, &result) != 0)
    {
        char operation[384];
        snprintf(operation, sizeof(operation), "解析主机 %s", target);
        chali_net_capture_error(operation);
        return 0;
    }
    int length = 0;
    for (const void *info = result; info != NULL; info = addrInfoNext(info))
    {
        const int family = addrInfoFamily(info);
        if (family != 2) continue; /* 当前 std::net 创建 AF_INET 套接字，域名也选择 IPv4。 */
        const int size = addressLength(family);
        memcpy(buffer, addrInfoAddr(info), ((size_t)(size)));
        const unsigned short networkPort = net_api.hostToNetwork16(((unsigned short)(port)));
        memcpy(buffer + 2, &networkPort, 2); /* 端口在 sockaddr 的第 2、3 字节 */
        length = size;
        break;
    }
    net_api.freeaddrinfo(result);
    if (length == 0) chali_net_set_error("找不到可用的地址");
    return length;
}

/**
 * @brief 查询平台套接字的本地端口。
 * @param fd 平台套接字。
 * @return 本地端口；查询失败返回零。
 */
int chali_net_local_port(intptr_t fd)
{
#if defined(_WIN32)
    unsigned char address[kAddrBuffer];
    int length = sizeof(address);
    typedef int(WINAPI * GetSockNameFn)(intptr_t, void *, int *);
    GetSockNameFn get_name = (GetSockNameFn)(void *)GetProcAddress(GetModuleHandleA("ws2_32.dll"), "getsockname");
    if (!get_name || get_name(fd, address, &length) != 0) return 0;
    unsigned short port;
    memcpy(&port, address + 2, sizeof(port));
    return net_api.hostToNetwork16(port);
#else
    struct sockaddr_in address;
    socklen_t length = sizeof(address);
    if (getsockname((int)fd, (struct sockaddr *)&address, &length) != 0) return 0;
    return ntohs(address.sin_port);
#endif
}
