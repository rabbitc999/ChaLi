/**
 * @file chali_net.h
 * @brief Windows/POSIX 套接字的独立 C 服务。
 */
#ifndef CHALI_NET_C_H
#define CHALI_NET_C_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#ifdef _WIN32
#define CHALI_NET_CALL __stdcall
#else
#define CHALI_NET_CALL
#endif
typedef int(CHALI_NET_CALL *StartupFn)(unsigned short, void *);
typedef int(CHALI_NET_CALL *CleanupFn)(void);
typedef intptr_t(CHALI_NET_CALL *CreateFn)(int, int, int);
typedef int(CHALI_NET_CALL *ConnectFn)(intptr_t, const void *, int);
typedef int(CHALI_NET_CALL *BindFn)(intptr_t, const void *, int);
typedef int(CHALI_NET_CALL *ListenFn)(intptr_t, int);
typedef intptr_t(CHALI_NET_CALL *AcceptFn)(intptr_t, void *, int *);
typedef int(CHALI_NET_CALL *SendFn)(intptr_t, const char *, int, int);
typedef int(CHALI_NET_CALL *RecvFn)(intptr_t, char *, int, int);
typedef int(CHALI_NET_CALL *CloseFn)(intptr_t);
typedef int(CHALI_NET_CALL *ShutdownFn)(intptr_t, int);
typedef int(CHALI_NET_CALL *SelectFn)(int, void *, void *, void *, void *);
typedef int(CHALI_NET_CALL *AddrInfoFn)(const char *, const char *, const void *, void **);
typedef void(CHALI_NET_CALL *FreeAddrInfoFn)(void *);
typedef unsigned short(CHALI_NET_CALL *HtonsFn)(unsigned short);
typedef unsigned long(CHALI_NET_CALL *InetAddrFn)(const char *);
typedef int(CHALI_NET_CALL *LastErrorFn)(void);
typedef int(CHALI_NET_CALL *IoctlFn)(intptr_t, long, unsigned long *);
typedef int(CHALI_NET_CALL *GetOptFn)(intptr_t, int, int, void *, int *);

typedef struct NetApi
{
    StartupFn startup;
    CleanupFn cleanup;
    CreateFn create;
    ConnectFn connect;
    BindFn bind;
    ListenFn listen;
    AcceptFn accept;
    SendFn send;
    RecvFn recv;
    CloseFn close;
    ShutdownFn shutdown;
    SelectFn select;
    AddrInfoFn getaddrinfo;
    FreeAddrInfoFn freeaddrinfo;
    HtonsFn hostToNetwork16;
    InetAddrFn inet_addr;
    LastErrorFn last_error;
    IoctlFn ioctl;
    GetOptFn getopt;
} NetApi;

enum
{
    kMaxSockets = 64,
    kAddrBuffer = 32,
    kNetOk = 0,
    kNetClosed = 1,
    kNetTimeout = 2,
    kNetError = 3,
    kNetPending = 4
};
#ifdef _WIN32
enum
{
    kSolSocket = 0xffff,
    kSoError = 0x1007
};
#else
enum
{
    kSolSocket = 1,
    kSoError = 4
};
#endif
/**
 * @brief 借用已初始化的系统调用表。
 * @return 进程级函数表。
 */
const NetApi *chali_net_api(void);
/**
 * @brief 借用最近平台错误文本。
 * @return 固定缓冲区。
 */
char *chali_net_error(void);
/**
 * @brief 查询平台套接字的本地端口。
 * @param fd 平台套接字。
 * @return 本地端口；查询失败返回零。
 */
int chali_net_local_port(intptr_t fd);
/** @brief 初始化平台网络入口。 */
bool chali_net_init(void);
/** @brief 保存平台错误文本。 */
void chali_net_set_error(const char *message);
/** @brief 保存最近套接字错误。 */
void chali_net_capture_error(const char *what);
/** @brief 检查平台套接字哨兵。 */
bool chali_net_invalid(intptr_t fd);
/** @brief 等待套接字可读。 */
int chali_net_wait_readable(intptr_t fd, int timeoutMs);
/** @brief 等待套接字可写。 */
int chali_net_wait_writable(intptr_t fd, int timeoutMs);
/** @brief 设置套接字阻塞模式。 */
bool chali_net_nonblocking(intptr_t fd, bool enabled);
/** @brief 检查数值 IPv4 文本。 */
bool chali_net_numeric_host(const char *host);
/** @brief 解析主机和端口为平台地址。 */
int chali_net_resolve(const char *host, int port, unsigned char *buffer);
#ifdef __cplusplus
}
#endif
#endif
