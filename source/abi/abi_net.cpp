/**
 * @file abi_net.cpp
 * @brief std::net 的语言记录适配，平台操作转调 C 服务。
 */
#include "chali_standard.h"
#include "chali_net.h"
#include "allocator/c_heap_allocator.hpp"
#include "../allocator/chali_allocator.hpp"
#include <algorithm>
#include <chrono>
#include <cstring>
#include <memory>
#include <stdexcept>
#ifndef _WIN32
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif
namespace {
using Memory::String;
/**
 * @brief 借用 C 服务的函数表。
 * @return 平台函数表。
 */
inline const NetApi &api() { return *chali_net_api(); }
/**
 * @brief 借用 C 服务的错误文本。
 * @return 固定错误缓冲区。
 */
inline char *errorText() { return chali_net_error(); }
struct SockSlot
{
    bool used = false;
    bool listener = false;
    std::intptr_t fd = -1;
    Memory::CHeapString pending; /* 行缓冲：多收的字节留到下次 */
    char peer[80] = { 0 };
};

/**
 * @brief 取得网络胶水独占的套接字槽表。
 * @return 进程内持久保存的可写槽表。
 */
inline Memory::CHeapVector<SockSlot> &sockets()
{
    static Memory::CHeapVector<SockSlot> instance(kMaxSockets);
    return instance;
}

/**
 * @brief 取得最近一次网络操作的状态槽。
 * @return 可供网络操作更新的状态引用。
 */
inline int &lastStatus()
{
    static int instance = kNetOk;
    return instance;
}

/**
 * @brief 校验网络句柄并读取仍有效的套接字槽。
 * @param handle 当前模块中的资源句柄。
 * @return 有效槽地址；句柄无效或已关闭时返回空。
 */
inline SockSlot *slotAt(std::int32_t handle)
{
    if (handle < 0 || handle >= kMaxSockets) return nullptr;
    SockSlot &slot = sockets()[static_cast<std::size_t>(handle)];
    return slot.used ? &slot : nullptr;
}

/**
 * @brief 复用空闲槽或追加槽以保存新套接字。
 * @param listener 监听句柄或需要保存的监听状态标志。
 * @param fd 宿主套接字文件描述符。
 * @return 用于语言侧访问该套接字的句柄。
 */
inline std::int32_t allocateSlot(bool listener, std::intptr_t fd)
{
    for (int i = 0; i < kMaxSockets; ++i)
    {
        SockSlot &slot = sockets()[static_cast<std::size_t>(i)];
        if (slot.used) continue;
        slot.used = true;
        slot.listener = listener;
        slot.fd = fd;
        slot.pending.clear();
        slot.peer[0] = '\0';
        return i;
    }
    chali_net_set_error("套接字句柄用尽");
    return -1;
}

/* ── 对外函数 ─────────────────────────────────────────────────────────── */

/**
 * @brief 将一次网络初始化结果转换为语言整数状态。
 * @return 初始化成功为零，失败为负一。
 */
inline std::int32_t netInit()
{
    if (!chali_net_init()) return -1;
    lastStatus() = kNetOk;
    return 0;
}

/**
 * @brief 复制最近一次网络胶水诊断。
 * @return 最近一次保存的错误文本。
 */
inline String netError()
{
    return String(errorText());
}

/**
 * @brief 读取最近一次网络操作的可恢复状态。
 * @return 语言层 Net 状态协议使用的状态码。
 */
inline std::int32_t netStatus()
{
    return lastStatus();
}

/**
 * @brief 解析主机并在指定期限内建立 TCP 连接。
 * @param host 主机名或地址文本。
 * @param port 目标或本地 TCP 端口号。
 * @param timeoutMs 允许等待的毫秒数，按模块约定处理无限等待。
 * @return 连接句柄；失败返回无效句柄并保存状态。
 */
inline std::int32_t netConnect(String host, std::int32_t port, std::int32_t timeoutMs)
{
    if (!chali_net_init()) return -1;
    unsigned char address[kAddrBuffer];
    std::memset(address, 0, sizeof(address));
    const int length = chali_net_resolve(host.c_str(), static_cast<int>(port), address);
    if (length == 0) return -1;
    const std::intptr_t fd = api().create(2, 1, 6); /* AF_INET(2) 家族见 resolve；SOCK_STREAM, IPPROTO_TCP */
    if (chali_net_invalid(fd))
    {
        chali_net_capture_error("创建套接字");
        return -1;
    }
    if (timeoutMs > 0)
    {
        /* 带超时的连接：先切非阻塞，连不上就 select 等可写，再查 SO_ERROR。 */
        const bool nonBlocking = chali_net_nonblocking(fd, true);
        if (api().connect(fd, address, length) != 0)
        {
            const int code = api().last_error != nullptr ? api().last_error() : 0;
            bool inProgress = false;
#if defined(_WIN32)
            inProgress = (code == 10035 || code == 10036); /* WSAEWOULDBLOCK / WSAEINPROGRESS */
#else
            inProgress = (code == EINPROGRESS || code == EWOULDBLOCK);
#endif
            if (!inProgress)
            {
                chali_net_capture_error("连接");
                api().close(fd);
                return -1;
            }
            const int ready = chali_net_wait_writable(fd, timeoutMs);
            if (ready < 0)
            {
                api().close(fd);
                return -1;
            }
            int pending = 0;
            int size = static_cast<int>(sizeof(pending));
            if (api().getopt != nullptr && api().getopt(fd, kSolSocket, kSoError, &pending, &size) == 0 && pending != 0)
            {
                char message[128];
                std::snprintf(message, sizeof(message), "连接失败（套接字错误码 %d）", pending);
                chali_net_set_error(message);
                api().close(fd);
                return -1;
            }
            if (ready == 0)
            {
                chali_net_set_error("连接超时");
                api().close(fd);
                return -1;
            }
        }
        if (nonBlocking) chali_net_nonblocking(fd, false);
    }
    else if (api().connect(fd, address, length) != 0)
    {
        chali_net_capture_error("连接");
        api().close(fd);
        return -1;
    }
    const std::int32_t handle = allocateSlot(false, fd);
    if (handle < 0)
    {
        api().close(fd);
        return -1;
    }
    std::snprintf(errorText(), 512, "%s", "");
    return handle;
}

/**
 * @brief 解析本地地址并创建 TCP 监听套接字。
 * @param host 主机名或地址文本。
 * @param port 目标或本地 TCP 端口号。
 * @param backlog 监听队列的容量上限。
 * @return 监听句柄；失败返回无效句柄并保存状态。
 */
inline std::int32_t netListen(String host, std::int32_t port, std::int32_t backlog)
{
    if (!chali_net_init()) return -1;
    unsigned char address[kAddrBuffer];
    std::memset(address, 0, sizeof(address));
    const int length = chali_net_resolve(host.c_str(), static_cast<int>(port), address);
    if (length == 0) return -1;
    const std::intptr_t fd = api().create(2, 1, 6);
    if (chali_net_invalid(fd))
    {
        chali_net_capture_error("创建监听套接字");
        return -1;
    }
    if (api().bind(fd, address, length) != 0)
    {
        chali_net_capture_error("绑定端口");
        api().close(fd);
        return -1;
    }
    if (api().listen(fd, backlog > 0 ? static_cast<int>(backlog) : 8) != 0)
    {
        chali_net_capture_error("开始监听");
        api().close(fd);
        return -1;
    }
    const std::int32_t handle = allocateSlot(true, fd);
    if (handle < 0)
    {
        api().close(fd);
        return -1;
    }
    return handle;
}

/**
 * @brief 在指定期限内从监听器接收一个 TCP 连接。
 * @param listener 监听句柄或需要保存的监听状态标志。
 * @param timeoutMs 允许等待的毫秒数，按模块约定处理无限等待。
 * @return 新连接句柄；超时或失败返回无效句柄。
 */
inline std::int32_t netAccept(std::int32_t listener, std::int32_t timeoutMs)
{
    SockSlot *slot = slotAt(listener);
    if (slot == nullptr || !slot->listener)
    {
        lastStatus() = kNetError;
        chali_net_set_error("不是监听句柄");
        return -1;
    }
    const int ready = chali_net_wait_readable(slot->fd, timeoutMs);
    if (ready <= 0)
    {
        lastStatus() = ready == 0 ? kNetTimeout : kNetError;
        if (ready == 0) chali_net_set_error("accept 超时");
        return -1;
    }
    unsigned char address[kAddrBuffer];
    int length = static_cast<int>(sizeof(address));
    const std::intptr_t fd = api().accept(slot->fd, address, &length);
    if (chali_net_invalid(fd))
    {
        lastStatus() = kNetError;
        chali_net_capture_error("接受连接");
        return -1;
    }
    const std::int32_t handle = allocateSlot(false, fd);
    if (handle < 0)
    {
        api().close(fd);
        lastStatus() = kNetError;
        return -1;
    }
    lastStatus() = kNetOk;
    return handle;
}

/**
 * @brief 发送字符串字节并保留短写及网络错误状态。
 * @param sock 待发送数据的连接句柄。
 * @param data 包含待发送原始字节的字符串。
 * @return 实际发送字节数；失败返回负值。
 */
inline std::int32_t netSend(std::int32_t sock, String data)
{
    lastStatus() = kNetOk;
    SockSlot *slot = slotAt(sock);
    if (slot == nullptr || slot->listener)
    {
        lastStatus() = kNetError;
        chali_net_set_error("不是连接句柄");
        return -1;
    }
    std::size_t sent = 0u;
    const std::size_t total = data.size();
    while (sent < total)
    {
        const int written = api().send(slot->fd, data.data() + sent, static_cast<int>(total - sent), 0);
        if (written <= 0)
        {
            lastStatus() = kNetError;
            chali_net_capture_error("发送");
            return -1;
        }
        sent += static_cast<std::size_t>(written);
    }
    return static_cast<std::int32_t>(sent);
}

/**
 * @brief 收一段文本；结果原因写进 __netStatus。
 * @param sock 已连接的平台槽编号。
 * @param maxBytes 最大接收字节数。
 * @return 接收到的原始字节；空结果需结合 lastStatus 区分关闭与错误。
 */
inline String netReceive(std::int32_t sock, std::int32_t maxBytes)
{
    SockSlot *slot = slotAt(sock);
    if (slot == nullptr || slot->listener)
    {
        lastStatus() = kNetError;
        chali_net_set_error("不是连接句柄");
        return String();
    }
    if (!slot->pending.empty())
    {
        Memory::CHeapString out = slot->pending;
        slot->pending.clear();
        lastStatus() = kNetOk;
        return String(out);
    }
    const int want = maxBytes > 0 ? static_cast<int>(maxBytes) : 4096;
    Memory::CHeapVector<char> buffer(static_cast<std::size_t>(want));
    const int count = api().recv(slot->fd, buffer.data(), want, 0);
    if (count > 0)
    {
        lastStatus() = kNetOk;
        return String(buffer.data(), static_cast<std::size_t>(count));
    }
    if (count == 0)
    {
        lastStatus() = kNetClosed;
        chali_net_set_error("对端已关闭连接");
        return String();
    }
    lastStatus() = kNetError;
    chali_net_capture_error("接收");
    return String();
}

/**
 * @brief 收到行尾（`\n`）为止，返回不含换行的那一行。
 * @param sock 已连接的平台槽编号。
 * @param timeoutMs 超时毫秒数。
 * @return 移除行尾后的字节；空结果需结合 lastStatus 区分空行、关闭、超时和错误。
 */
inline String netReceiveLine(std::int32_t sock, std::int32_t timeoutMs)
{
    SockSlot *slot = slotAt(sock);
    if (slot == nullptr || slot->listener)
    {
        lastStatus() = kNetError;
        chali_net_set_error("不是连接句柄");
        return String();
    }
    while (true)
    {
        const std::size_t newline = slot->pending.find('\n');
        if (newline != Memory::CHeapString::npos)
        {
            Memory::CHeapString line = slot->pending.substr(0u, newline);
            slot->pending.erase(0u, newline + 1u);
            if (!line.empty() && line.back() == '\r') line.pop_back();
            lastStatus() = kNetOk;
            return String(line);
        }
        const int ready = chali_net_wait_readable(slot->fd, timeoutMs);
        if (ready == 0)
        {
            lastStatus() = kNetTimeout;
            chali_net_set_error("读取超时");
            return String();
        }
        if (ready < 0)
        {
            lastStatus() = kNetError;
            return String();
        }
        char buffer[4096];
        const int count = api().recv(slot->fd, buffer, static_cast<int>(sizeof(buffer)), 0);
        if (count > 0)
        {
            slot->pending.append(buffer, static_cast<std::size_t>(count));
            continue;
        }
        if (count == 0)
        {
            lastStatus() = kNetClosed;
            chali_net_set_error("对端已关闭连接");
            /* 缓冲区里还剩最后一行没有换行时也交出去 */
            if (!slot->pending.empty())
            {
                Memory::CHeapString line = slot->pending;
                slot->pending.clear();
                lastStatus() = kNetOk;
                return String(line);
            }
            return String();
        }
        lastStatus() = kNetError;
        chali_net_capture_error("接收");
        return String();
    }
}

/**
 * @brief 释放网络句柄对应的套接字并清空槽。
 * @param handle 当前模块中的资源句柄。
 * @return 关闭操作的状态码。
 */
inline std::int32_t netClose(std::int32_t handle)
{
    SockSlot *slot = slotAt(handle);
    if (slot == nullptr) return -1;
    if (!slot->listener && api().shutdown != nullptr) api().shutdown(slot->fd, 2); /* SD_BOTH / SHUT_RDWR */
    api().close(slot->fd);
    slot->used = false;
    slot->fd = -1;
    slot->listener = false;
    slot->pending.clear();
    slot->peer[0] = '\0';
    return 0;
}

/**
 * @brief 本端端口（监听器用得多：端口传 0 让系统分配）。
 * @param handle 平台槽编号。
 * @return 本地端口，句柄无效或系统查询失败为零。
 */
inline std::int32_t netLocalPort(std::int32_t handle)
{
    SockSlot *slot = slotAt(handle);
    if (slot == nullptr) return 0;
    return chali_net_local_port(slot->fd);
}

/**
 * @brief 持有一项平台连接及该连接最近一次操作的诊断。
 */

/**
 * @brief 持有平台连接及该连接的独立状态、错误和连接超时。
 */
struct SocketResource
{
    std::int32_t slot = -1;
    String error;
    std::int64_t status = kNetOk;
    bool connecting = false;
    bool timed = false;
    std::chrono::steady_clock::time_point deadline;
};

/**
 * @brief 销毁独占套接字资源，尚未关闭时同时归还系统句柄。
 * @param pointer 待销毁的 SocketResource。
 */
inline void CHALI_ABI_CALL destroySocketResource(void *pointer)
{
    auto *resource = static_cast<SocketResource *>(pointer);
    if (resource->slot >= 0) netClose(resource->slot);
    Memory::destroyCHeap(resource);
}

/** @brief 在记录尚未接管平台槽时负责失败清理。 */
struct SocketSlotOwner
{
    int32_t slot;
    /** @brief 归还尚未转移的平台槽。 */
    ~SocketSlotOwner()
    {
        if (slot >= 0) netClose(slot);
    }
    /** @brief 转移平台槽所有权。
     * @return 原槽位置。
     */
    int32_t release()
    {
        const auto value = slot;
        slot = -1;
        return value;
    }
};
/** @brief 语言尚未取得异步结果记录时负责异常清理。 */
struct PendingRecord
{
    ChaliAbiFrame *frame;
    int64_t handle;
    /** @brief 释放尚未返回语言的记录。 */
    ~PendingRecord()
    {
        if (handle) chali_abi_record_release(frame, handle);
    }
    /** @brief 将结果交还语言。
     * @return 原句柄。
     */
    int64_t release()
    {
        const auto value = handle;
        handle = 0;
        return value;
    }
};
/** @brief 解码直接资源句柄或语言引用槽。
 * @param frame 当前 C 帧。
 * @param reference 接收者。
 * @return 记录句柄。
 */
int64_t referenceHandle(ChaliAbiFrame *frame, int64_t reference)
{
    if (chali_abi_record_probe(frame, reference, "std::net::SocketResource")) return reference;
    unsigned char bytes[8];
    if (!chali_abi_read(frame, reference, bytes, 8))
    {
        chali_abi_fail(frame, "opcode 引用槽不可读");
        throw std::runtime_error("opcode 引用槽不可读");
    }
    uint64_t value = 0;
    for (unsigned i = 0; i < 8; ++i) value |= uint64_t(bytes[i]) << (8 * i);
    return int64_t(value);
}
/** @brief 从语言字符串复制独立宿主文本或二进制负载。
 * @param frame 当前 C 帧。
 * @param address 语言字符串地址。
 * @return 按精确字节长度复制的文本。
 */
String readText(ChaliAbiFrame *frame, int64_t address)
{
    const unsigned char *bytes = nullptr;
    size_t count = 0;
    if (!chali_abi_bytes(frame, address, &bytes, &count))
    {
        chali_abi_fail(frame, "胶水层读取 string 失败");
        throw std::runtime_error("胶水层读取 string 失败");
    }
    return String(bytes ? reinterpret_cast<const char *>(bytes) : "", count);
}
/**
 * @brief 将平台连接收进当前运行时的不透明记录。
 * @param frame 当前 C 帧的调用上下文。
 * @param slot 平台连接表位置，负值表示未连接。
 * @return 独占记录句柄。
 */
inline std::int64_t makeSocketResource(ChaliAbiFrame *frame, std::int32_t slot)
{
    SocketSlotOwner pending{ slot };
    std::unique_ptr<SocketResource, decltype(&destroySocketResource)> resource(Memory::makeCHeapUnique<SocketResource>().release(), destroySocketResource);
    resource->slot = pending.release();
    resource->status = lastStatus();
    if (slot < 0) resource->error = netError();
    const auto handle = chali_abi_record_make(frame, "std::net::SocketResource", resource.get(), destroySocketResource);
    if (handle) resource.release();
    return handle;
}

/**
 * @brief 校验并借用当前运行时拥有的套接字资源。
 * @param frame 当前 C 帧的调用上下文。
 * @param handle 不透明记录句柄。
 * @return 有效资源；已经释放或类型不匹配时记录错误并停止当前内核。
 */
inline SocketResource *socketResource(ChaliAbiFrame *frame, std::int64_t handle)
{
    auto *resource = static_cast<SocketResource *>(chali_abi_record_probe(frame, handle, "std::net::SocketResource"));
    if (!resource)
    {
        chali_abi_fail(frame, "网络 opcode 已释放或类型不匹配");
        throw std::runtime_error("网络 opcode 已释放或类型不匹配");
    }
    return resource;
}

/**
 * @brief 保存本次平台操作的结果，避免其他连接覆盖该连接的状态。
 * @param resource 刚完成操作的连接。
 */
inline void saveSocketStatus(SocketResource &resource)
{
    resource.status = lastStatus();
    if (resource.status == kNetError) resource.error = netError();
}

/**
 * @brief 检查平台错误是否表示非阻塞操作尚未就绪。
 * @return 尚未就绪或正在连接时返回 true。
 */
inline bool wouldBlock()
{
    const int code = api().last_error ? api().last_error() : 0;
#if defined(_WIN32)
    return code == 10035 || code == 10036 || code == 10037;
#else
    return code == EWOULDBLOCK || code == EAGAIN || code == EINPROGRESS || code == EALREADY;
#endif
}

/**
 * @brief 保存异步操作错误，不将可恢复网络错误升级为 VM panic。
 * @param resource 操作所属连接。
 * @param message 错误说明。
 * @return 通用失败数值 -1。
 */
inline std::int64_t asyncError(SocketResource &resource, const String &message)
{
    resource.status = kNetError;
    resource.error = message;
    return -1;
}

/**
 * @brief 创建非阻塞 IPv4 连接，地址解析仅接受数值 IP。
 * @param frame 当前 C 帧的上下文。
 * @param host 数值 IPv4 地址。
 * @param port 远端端口。
 * @param timeoutMs 正数表示连接超时毫秒数，非正数表示无内建超时。
 * @return 由当前运行时 拥有的连接记录，status 表示完成、进行中或失败。
 */
inline std::int64_t connectAsync(ChaliAbiFrame *frame, String host, std::int32_t port, std::int32_t timeoutMs)
{
    auto handle = makeSocketResource(frame, -1);
    PendingRecord pendingRecord{ frame, handle };
    auto *resource = socketResource(frame, handle);
    if (!resource) return 0;
    resource->error.clear();
    if (!chali_net_init())
    {
        asyncError(*resource, netError());
        return pendingRecord.release();
    }
    if (!chali_net_numeric_host(host.c_str()) || port <= 0 || port > 65535)
    {
        asyncError(*resource, "connectAsync 需要数值 IPv4 地址及 1..65535 端口；域名请使用 connect");
        return pendingRecord.release();
    }
    unsigned char address[kAddrBuffer]{};
    int length = chali_net_resolve(host.c_str(), port, address);
    if (!length)
    {
        asyncError(*resource, netError());
        return pendingRecord.release();
    }
    auto fd = api().create(2, 1, 6);
    if (chali_net_invalid(fd))
    {
        chali_net_capture_error("创建异步连接");
        asyncError(*resource, netError());
        return pendingRecord.release();
    }
    if (!chali_net_nonblocking(fd, true))
    {
        chali_net_capture_error("设置非阻塞连接");
        api().close(fd);
        asyncError(*resource, netError());
        return pendingRecord.release();
    }
    int result = api().connect(fd, address, length);
    const bool pending = result != 0 && wouldBlock();
    if (result != 0 && !pending)
    {
        chali_net_capture_error("异步连接");
        api().close(fd);
        asyncError(*resource, netError());
        return pendingRecord.release();
    }
    auto slot = allocateSlot(false, fd);
    if (slot < 0)
    {
        api().close(fd);
        asyncError(*resource, netError());
        return pendingRecord.release();
    }
    resource->slot = slot;
    resource->connecting = pending;
    resource->status = pending ? kNetPending : kNetOk;
    resource->timed = timeoutMs > 0;
    resource->deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs > 0 ? timeoutMs : 0);
    if (!pending && !chali_net_nonblocking(fd, false))
    {
        netClose(slot);
        resource->slot = -1;
        asyncError(*resource, "恢复连接模式失败");
    }
    return pendingRecord.release();
}

/**
 * @brief 以零等待轮询异步连接，完成后恢复同步 API 可用的连接模式。
 * @param resource 正在连接或已经完成的连接记录。
 * @return NET_PENDING、NET_OK、NET_TIMEOUT 或 NET_ERROR。
 */
inline std::int64_t pollConnect(SocketResource &resource)
{
    if (!resource.connecting) return resource.status;
    auto *slot = slotAt(resource.slot);
    if (!slot)
    {
        resource.connecting = false;
        return asyncError(resource, "异步连接已经关闭"), resource.status;
    }
    int ready = chali_net_wait_writable(slot->fd, 0);
    if (ready == 0 && (!resource.timed || std::chrono::steady_clock::now() < resource.deadline)) return resource.status = kNetPending;
    int pending = 0, size = sizeof(pending);
    const bool failed = ready < 0 || (ready > 0 && (!api().getopt || api().getopt(slot->fd, kSolSocket, kSoError, &pending, &size) != 0 || pending != 0));
    resource.connecting = false;
    if (ready == 0 || failed || !chali_net_nonblocking(slot->fd, false))
    {
        netClose(resource.slot);
        resource.slot = -1;
        resource.status = ready == 0 ? kNetTimeout : kNetError;
        resource.error = ready == 0 ? "异步连接超时" : "异步连接失败（套接字错误码 " + Memory::toString(pending) + "）";
        return resource.status;
    }
    resource.error.clear();
    return resource.status = kNetOk;
}

/**
 * @brief 以零等待接受一条连接，不就绪时保留监听器并返回 pending 状态记录。
 * @param frame 当前 C 帧的上下文。
 * @param resource 监听器记录。
 * @return 新连接或 pending/错误结果的记录句柄。
 */
inline std::int64_t acceptAsync(ChaliAbiFrame *frame, SocketResource &resource)
{
    auto *slot = slotAt(resource.slot);
    if (!slot || !slot->listener)
    {
        asyncError(resource, "不是有效监听器");
        auto handle = makeSocketResource(frame, -1);
        PendingRecord pendingRecord{ frame, handle };
        auto *result = socketResource(frame, handle);
        if (result)
        {
            result->status = resource.status;
            result->error = resource.error;
        }
        return pendingRecord.release();
    }
    if (!chali_net_nonblocking(slot->fd, true))
    {
        asyncError(resource, "设置非阻塞监听失败");
        auto handle = makeSocketResource(frame, -1);
        PendingRecord pendingRecord{ frame, handle };
        auto *result = socketResource(frame, handle);
        if (result)
        {
            result->status = resource.status;
            result->error = resource.error;
        }
        return pendingRecord.release();
    }
    const auto accepted = netAccept(resource.slot, 0);
    const auto status = accepted >= 0 ? kNetOk : lastStatus() == kNetTimeout || wouldBlock() ? kNetPending :
                                                                                               kNetError;
    chali_net_nonblocking(slot->fd, false);
    const auto handle = makeSocketResource(frame, accepted);
    PendingRecord pendingRecord{ frame, handle };
    auto *result = socketResource(frame, handle);
    resource.status = status;
    if (result)
    {
        result->status = status;
        if (status == kNetPending) result->error.clear();
    }
    if (status == kNetError) resource.error = netError();
    return pendingRecord.release();
}

/**
 * @brief 执行一次有界非阻塞发送，保留部分发送的字节数。
 * @param resource 连接记录。
 * @param data 完整待发送文本。
 * @param offset 本次发送在文本中的字节偏移。
 * @return 本次发送字节数；未就绪为零，失败为 -1。
 */
inline std::int64_t sendAsync(SocketResource &resource, const String &data, std::int64_t offset)
{
    if (resource.connecting && pollConnect(resource) != kNetOk) return resource.status == kNetPending ? 0 : -1;
    auto *slot = slotAt(resource.slot);
    if (!slot || slot->listener) return asyncError(resource, "不是有效连接");
    if (offset < 0 || static_cast<std::uint64_t>(offset) > data.size()) return asyncError(resource, "异步发送偏移越界");
    resource.status = kNetOk;
    if (static_cast<std::size_t>(offset) == data.size()) return 0;
    if (!chali_net_nonblocking(slot->fd, true)) return asyncError(resource, "设置非阻塞发送失败");
    const int count = static_cast<int>((std::min)(data.size() - static_cast<std::size_t>(offset), std::size_t{ 65536 }));
    int flags = 0;
#if defined(MSG_NOSIGNAL)
    flags = MSG_NOSIGNAL;
#endif
    const int sent = api().send(slot->fd, data.data() + offset, count, flags);
    const bool pending = sent < 0 && wouldBlock();
    if (sent < 0 && !pending) chali_net_capture_error("异步发送");
    chali_net_nonblocking(slot->fd, false);
    if (pending)
    {
        resource.status = kNetPending;
        return 0;
    }
    if (sent <= 0) return asyncError(resource, netError());
    return sent;
}

/**
 * @brief 读取一次非阻塞网络数据或交出已有行缓冲中的字节。
 * @param resource 连接记录。
 * @param maxBytes 字节读取上限，每次最多 65536。
 * @param line true 表示等待完整行，false 表示读取任意字节。
 * @return 本次结果；空串通过 status 区分 pending、关闭、空行或错误。
 */
inline String receiveAsync(SocketResource &resource, std::int32_t maxBytes, bool line)
{
    if (resource.connecting && pollConnect(resource) != kNetOk) return {};
    auto *slot = slotAt(resource.slot);
    if (!slot || slot->listener)
    {
        asyncError(resource, "不是有效连接");
        return {};
    }
    if (maxBytes <= 0)
    {
        asyncError(resource, "异步接收长度必须为正");
        return {};
    }
    resource.status = kNetOk;
    bool closed = false;
    const auto readyLine = [&]() { return slot->pending.find('\n') != Memory::CHeapString::npos; };
    if ((line && !readyLine()) || (!line && slot->pending.empty()))
    {
        if (!chali_net_nonblocking(slot->fd, true))
        {
            asyncError(resource, "设置非阻塞接收失败");
            return {};
        }
        char bytes[65536];
        int count = api().recv(slot->fd, bytes, line ? 4096 : (std::min)(maxBytes, std::int32_t{ 65536 }), 0);
        const bool pending = count < 0 && wouldBlock();
        if (count < 0 && !pending) chali_net_capture_error("异步接收");
        chali_net_nonblocking(slot->fd, false);
        if (pending)
        {
            resource.status = kNetPending;
            return {};
        }
        if (count < 0)
        {
            asyncError(resource, netError());
            return {};
        }
        closed = count == 0;
        if (count > 0) slot->pending.append(bytes, static_cast<std::size_t>(count));
    }
    if (line)
    {
        const auto newline = slot->pending.find('\n');
        if (newline == Memory::CHeapString::npos && !closed)
        {
            if (slot->pending.size() > 1048576)
            {
                slot->pending.clear();
                asyncError(resource, "异步行缓冲超过 1 MiB");
                return {};
            }
            resource.status = kNetPending;
            return {};
        }
        if (newline != Memory::CHeapString::npos)
        {
            auto result = slot->pending.substr(0, newline);
            slot->pending.erase(0, newline + 1);
            if (!result.empty() && result.back() == '\r') result.pop_back();
            return String(result);
        }
    }
    const auto count = (std::min)(slot->pending.size(), static_cast<std::size_t>(maxBytes));
    auto result = slot->pending.substr(0, count);
    slot->pending.erase(0, count);
    if (closed && result.empty()) resource.status = kNetClosed;
    return String(result);
}

/** @brief 执行 std::net::__std_netNew 的固定套接字操作。
 * @param frame 当前 C 帧。
 * @return 当前连接、负载或操作状态。
 */
std::int64_t socketNew(ChaliAbiFrame *frame) { return makeSocketResource(frame, -1); }
/** @brief 执行 std::net::__std_netDrop 的固定套接字操作。
 * @param frame 当前 C 帧。
 * @param handle 平台槽编号。
 */
void socketDrop(ChaliAbiFrame *frame, std::int64_t handle)
{
    if (chali_abi_record_probe(frame, handle, "std::net::SocketResource")) chali_abi_record_release(frame, handle);
}
/** @brief 执行 std::net::__std_netConnect 的固定套接字操作。
 * @param frame 当前 C 帧。
 * @param host 主机名或数值地址。
 * @param port 端口。
 * @param timeout 超时毫秒数。
 * @return 当前连接、负载或操作状态。
 */
std::int64_t socketConnect(ChaliAbiFrame *frame, String host, std::int32_t port, std::int32_t timeout) { return makeSocketResource(frame, netConnect(host, port, timeout)); }
/** @brief 执行 std::net::__std_netListen 的固定套接字操作。
 * @param frame 当前 C 帧。
 * @param host 主机名或数值地址。
 * @param port 端口。
 * @param backlog 监听队列长度。
 * @return 当前连接、负载或操作状态。
 */
std::int64_t socketListen(ChaliAbiFrame *frame, String host, std::int32_t port, std::int32_t backlog) { return makeSocketResource(frame, netListen(host, port, backlog)); }
/** @brief 执行 std::net::__std_netAccept 的固定套接字操作。
 * @param frame 当前 C 帧。
 * @param reference 语言声明的实参。
 * @param timeout 超时毫秒数。
 * @return 当前连接、负载或操作状态。
 */
std::int64_t socketAccept(ChaliAbiFrame *frame, std::int64_t reference, std::int32_t timeout)
{
    auto *resource = socketResource(frame, referenceHandle(frame, reference));
    if (!resource) return std::int64_t{};
    SocketSlotOwner accepted{ netAccept(resource->slot, timeout) };
    saveSocketStatus(*resource);
    return makeSocketResource(frame, accepted.release());
}
/** @brief 执行 std::net::__std_netIsOpen 的固定套接字操作。
 * @param frame 当前 C 帧。
 * @param handle 平台槽编号。
 * @return 当前连接、负载或操作状态。
 */
bool socketIsOpen(ChaliAbiFrame *frame, std::int64_t handle)
{
    auto *r = socketResource(frame, handle);
    return r && r->slot >= 0 && !r->connecting;
}
/** @brief 执行 std::net::__std_netError 的固定套接字操作。
 * @param frame 当前 C 帧。
 * @param handle 平台槽编号。
 * @return 当前连接、负载或操作状态。
 */
String socketError(ChaliAbiFrame *frame, std::int64_t handle)
{
    auto *r = socketResource(frame, handle);
    return r ? r->error : String{};
}
/** @brief 执行 std::net::__std_netStatus 的固定套接字操作。
 * @param frame 当前 C 帧。
 * @param handle 平台槽编号。
 * @return 当前连接、负载或操作状态。
 */
std::int64_t socketStatus(ChaliAbiFrame *frame, std::int64_t handle)
{
    auto *r = socketResource(frame, handle);
    return r ? r->status : kNetError;
}
/** @brief 执行 std::net::__std_netCloseSocket 的固定套接字操作。
 * @param frame 当前 C 帧。
 * @param reference 语言声明的实参。
 */
void socketCloseSocket(ChaliAbiFrame *frame, std::int64_t reference)
{
    auto *r = socketResource(frame, referenceHandle(frame, reference));
    if (r && r->slot >= 0)
    {
        netClose(r->slot);
        r->slot = -1;
        r->connecting = false;
        r->status = kNetClosed;
    }
}
/** @brief 执行 std::net::__std_netCloseListener 的固定套接字操作。
 * @param frame 当前 C 帧。
 * @param reference 语言声明的实参。
 */
void socketCloseListener(ChaliAbiFrame *frame, std::int64_t reference)
{
    auto *r = socketResource(frame, referenceHandle(frame, reference));
    if (r && r->slot >= 0)
    {
        netClose(r->slot);
        r->slot = -1;
        r->connecting = false;
        r->status = kNetClosed;
    }
}
/** @brief 执行 std::net::__std_netLocalPort 的固定套接字操作。
 * @param frame 当前 C 帧。
 * @param handle 平台槽编号。
 * @return 当前连接、负载或操作状态。
 */
std::int64_t socketLocalPort(ChaliAbiFrame *frame, std::int64_t handle)
{
    auto *r = socketResource(frame, handle);
    return r ? static_cast<std::int64_t>(netLocalPort(r->slot)) : 0;
}
/** @brief 执行 std::net::__std_netSend 的固定套接字操作。
 * @param frame 当前 C 帧。
 * @param reference 语言声明的实参。
 * @param data 待发送负载。
 * @return 当前连接、负载或操作状态。
 */
std::int64_t socketSend(ChaliAbiFrame *frame, std::int64_t reference, String data)
{
    auto *r = socketResource(frame, referenceHandle(frame, reference));
    if (!r) return std::int64_t{ -1 };
    auto count = netSend(r->slot, data);
    saveSocketStatus(*r);
    return static_cast<std::int64_t>(count);
}
/** @brief 执行 std::net::__std_netReceive 的固定套接字操作。
 * @param frame 当前 C 帧。
 * @param reference 语言声明的实参。
 * @param count 请求字节数。
 * @return 当前连接、负载或操作状态。
 */
String socketReceive(ChaliAbiFrame *frame, std::int64_t reference, std::int32_t count)
{
    auto *r = socketResource(frame, referenceHandle(frame, reference));
    if (!r) return String{};
    auto text = netReceive(r->slot, count);
    saveSocketStatus(*r);
    return text;
}
/** @brief 执行 std::net::__std_netReceiveLine 的固定套接字操作。
 * @param frame 当前 C 帧。
 * @param reference 语言声明的实参。
 * @param timeout 超时毫秒数。
 * @return 当前连接、负载或操作状态。
 */
String socketReceiveLine(ChaliAbiFrame *frame, std::int64_t reference, std::int32_t timeout)
{
    auto *r = socketResource(frame, referenceHandle(frame, reference));
    if (!r) return String{};
    auto text = netReceiveLine(r->slot, timeout);
    saveSocketStatus(*r);
    return text;
}
/** @brief 执行 std::net::__std_netPollConnect 的固定套接字操作。
 * @param frame 当前 C 帧。
 * @param reference 语言声明的实参。
 * @return 当前连接、负载或操作状态。
 */
std::int64_t socketPollConnect(ChaliAbiFrame *frame, std::int64_t reference)
{
    auto *resource = socketResource(frame, referenceHandle(frame, reference));
    return resource ? pollConnect(*resource) : std::int64_t{ kNetError };
}
/** @brief 执行 std::net::__std_netAcceptAsync 的固定套接字操作。
 * @param frame 当前 C 帧。
 * @param reference 语言声明的实参。
 * @return 当前连接、负载或操作状态。
 */
std::int64_t socketAcceptAsync(ChaliAbiFrame *frame, std::int64_t reference)
{
    auto *resource = socketResource(frame, referenceHandle(frame, reference));
    return resource ? acceptAsync(frame, *resource) : 0;
}
/** @brief 执行 std::net::__std_netSendAsync 的固定套接字操作。
 * @param frame 当前 C 帧。
 * @param reference 语言声明的实参。
 * @param data 待发送负载。
 * @param offset 发送起始字节位置。
 * @return 当前连接、负载或操作状态。
 */
std::int64_t socketSendAsync(ChaliAbiFrame *frame, std::int64_t reference, String data, std::int64_t offset)
{
    auto *resource = socketResource(frame, referenceHandle(frame, reference));
    return resource ? sendAsync(*resource, data, offset) : std::int64_t{ -1 };
}
/** @brief 执行 std::net::__std_netReceiveAsync 的固定套接字操作。
 * @param frame 当前 C 帧。
 * @param reference 语言声明的实参。
 * @param count 请求字节数。
 * @return 当前连接、负载或操作状态。
 */
String socketReceiveAsync(ChaliAbiFrame *frame, std::int64_t reference, std::int32_t count)
{
    auto *resource = socketResource(frame, referenceHandle(frame, reference));
    return resource ? receiveAsync(*resource, count, false) : String{};
}
/** @brief 执行 std::net::__std_netReceiveLineAsync 的固定套接字操作。
 * @param frame 当前 C 帧。
 * @param reference 语言声明的实参。
 * @return 当前连接、负载或操作状态。
 */
String socketReceiveLineAsync(ChaliAbiFrame *frame, std::int64_t reference)
{
    auto *resource = socketResource(frame, referenceHandle(frame, reference));
    return resource ? receiveAsync(*resource, 1048576, true) : String{};
}
/** @brief 解码 std::net::__std_netNew 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryNew(ChaliAbiFrame *frame)
try
{
    chali_abi_return_integer(frame, int64_t(socketNew(frame)));
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "网络接口失败");
}
/** @brief 解码 std::net::__std_netDrop 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryDrop(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    socketDrop(frame, static_cast<std::int64_t>(a0));
    chali_abi_return_integer(frame, 0);
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "网络接口失败");
}
/** @brief 解码 std::net::__std_netConnect 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryConnect(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    auto host = readText(frame, a0);
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    int64_t a2 = 0;
    if (!chali_abi_argument_integer(frame, 2, &a2)) return;
    chali_abi_return_integer(frame, int64_t(socketConnect(frame, std::move(host), static_cast<std::int32_t>(a1), static_cast<std::int32_t>(a2))));
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "网络接口失败");
}
/** @brief 解码 std::net::__std_netListen 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryListen(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    auto host = readText(frame, a0);
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    int64_t a2 = 0;
    if (!chali_abi_argument_integer(frame, 2, &a2)) return;
    chali_abi_return_integer(frame, int64_t(socketListen(frame, std::move(host), static_cast<std::int32_t>(a1), static_cast<std::int32_t>(a2))));
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "网络接口失败");
}
/** @brief 解码 std::net::__std_netAccept 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryAccept(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    chali_abi_return_integer(frame, int64_t(socketAccept(frame, static_cast<std::int64_t>(a0), static_cast<std::int32_t>(a1))));
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "网络接口失败");
}
/** @brief 解码 std::net::__std_netIsOpen 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryIsOpen(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    chali_abi_return_integer(frame, int64_t(socketIsOpen(frame, static_cast<std::int64_t>(a0))));
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "网络接口失败");
}
/** @brief 解码 std::net::__std_netError 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryError(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    const auto text = socketError(frame, static_cast<std::int64_t>(a0));
    chali_abi_return_integer(frame, chali_abi_string_bytes(frame, text.data(), text.size()));
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "网络接口失败");
}
/** @brief 解码 std::net::__std_netStatus 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryStatus(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    chali_abi_return_integer(frame, int64_t(socketStatus(frame, static_cast<std::int64_t>(a0))));
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "网络接口失败");
}
/** @brief 解码 std::net::__std_netCloseSocket 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryCloseSocket(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    socketCloseSocket(frame, static_cast<std::int64_t>(a0));
    chali_abi_return_integer(frame, 0);
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "网络接口失败");
}
/** @brief 解码 std::net::__std_netCloseListener 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryCloseListener(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    socketCloseListener(frame, static_cast<std::int64_t>(a0));
    chali_abi_return_integer(frame, 0);
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "网络接口失败");
}
/** @brief 解码 std::net::__std_netLocalPort 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryLocalPort(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    chali_abi_return_integer(frame, int64_t(socketLocalPort(frame, static_cast<std::int64_t>(a0))));
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "网络接口失败");
}
/** @brief 解码 std::net::__std_netSend 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entrySend(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    auto data = readText(frame, a1);
    chali_abi_return_integer(frame, int64_t(socketSend(frame, static_cast<std::int64_t>(a0), std::move(data))));
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "网络接口失败");
}
/** @brief 解码 std::net::__std_netReceive 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryReceive(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    const auto text = socketReceive(frame, static_cast<std::int64_t>(a0), static_cast<std::int32_t>(a1));
    chali_abi_return_integer(frame, chali_abi_string_bytes(frame, text.data(), text.size()));
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "网络接口失败");
}
/** @brief 解码 std::net::__std_netReceiveLine 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryReceiveLine(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    const auto text = socketReceiveLine(frame, static_cast<std::int64_t>(a0), static_cast<std::int32_t>(a1));
    chali_abi_return_integer(frame, chali_abi_string_bytes(frame, text.data(), text.size()));
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "网络接口失败");
}
/** @brief 解码 std::net::__std_netPollConnect 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryPollConnect(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    chali_abi_return_integer(frame, int64_t(socketPollConnect(frame, static_cast<std::int64_t>(a0))));
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "网络接口失败");
}
/** @brief 解码 std::net::__std_netAcceptAsync 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryAcceptAsync(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    chali_abi_return_integer(frame, int64_t(socketAcceptAsync(frame, static_cast<std::int64_t>(a0))));
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "网络接口失败");
}
/** @brief 解码 std::net::__std_netSendAsync 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entrySendAsync(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    auto data = readText(frame, a1);
    int64_t a2 = 0;
    if (!chali_abi_argument_integer(frame, 2, &a2)) return;
    chali_abi_return_integer(frame, int64_t(socketSendAsync(frame, static_cast<std::int64_t>(a0), std::move(data), static_cast<std::int64_t>(a2))));
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "网络接口失败");
}
/** @brief 解码 std::net::__std_netReceiveAsync 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryReceiveAsync(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    const auto text = socketReceiveAsync(frame, static_cast<std::int64_t>(a0), static_cast<std::int32_t>(a1));
    chali_abi_return_integer(frame, chali_abi_string_bytes(frame, text.data(), text.size()));
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "网络接口失败");
}
/** @brief 解码 std::net::__std_netReceiveLineAsync 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryReceiveLineAsync(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    const auto text = socketReceiveLineAsync(frame, static_cast<std::int64_t>(a0));
    chali_abi_return_integer(frame, chali_abi_string_bytes(frame, text.data(), text.size()));
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "网络接口失败");
}
/** @brief 解码 std::net::__std_netConnectAsync 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryConnectAsync(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    auto host = readText(frame, a0);
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    int64_t a2 = 0;
    if (!chali_abi_argument_integer(frame, 2, &a2)) return;
    chali_abi_return_integer(frame, int64_t(connectAsync(frame, std::move(host), static_cast<std::int32_t>(a1), static_cast<std::int32_t>(a2))));
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "网络接口失败");
}
}  // namespace
/** @brief 取得同步及异步 TCP 的 C 入口表。
 * @param count 接收入口数量，可为空。
 * @return 生命周期覆盖进程的表。
 */
extern "C" const ChaliAbiRecord *CHALI_ABI_CALL abi_net_records(size_t *count)
{
    static const ChaliAbiRecord records[] = {
        { "std::net::__std_netAccept", entryAccept, 2 },
        { "std::net::__std_netAcceptAsync", entryAcceptAsync, 1 },
        { "std::net::__std_netCloseListener", entryCloseListener, 1 },
        { "std::net::__std_netCloseSocket", entryCloseSocket, 1 },
        { "std::net::__std_netConnect", entryConnect, 3 },
        { "std::net::__std_netConnectAsync", entryConnectAsync, 3 },
        { "std::net::__std_netDrop", entryDrop, 1 },
        { "std::net::__std_netError", entryError, 1 },
        { "std::net::__std_netIsOpen", entryIsOpen, 1 },
        { "std::net::__std_netListen", entryListen, 3 },
        { "std::net::__std_netLocalPort", entryLocalPort, 1 },
        { "std::net::__std_netNew", entryNew, 0 },
        { "std::net::__std_netPollConnect", entryPollConnect, 1 },
        { "std::net::__std_netReceive", entryReceive, 2 },
        { "std::net::__std_netReceiveAsync", entryReceiveAsync, 2 },
        { "std::net::__std_netReceiveLine", entryReceiveLine, 2 },
        { "std::net::__std_netReceiveLineAsync", entryReceiveLineAsync, 1 },
        { "std::net::__std_netSend", entrySend, 2 },
        { "std::net::__std_netSendAsync", entrySendAsync, 3 },
        { "std::net::__std_netStatus", entryStatus, 1 },
    };
    if (count) *count = sizeof(records) / sizeof(records[0]);
    return records;
}

#ifndef GLUE_NO_STD_NET
#define CHALI_AOT_BIND_MODULE net
#include "aot_bind_module.h"
#endif
