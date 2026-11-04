#include <curl/curl.h>
#include <climits>
#include <cstdio>
#include <cstdint>
#include <filesystem>
#include <map>
#include <memory>
#include <mutex>
#include <system_error>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

namespace
{
using Cancel = int64_t (*)(int64_t, int64_t);
namespace fs = std::filesystem;

struct Session
{
    CURL *easy = curl_easy_init();
    std::mutex use;

    /** @brief 释放会话及其连接池。 */
    ~Session() { if (easy) curl_easy_cleanup(easy); }
};

std::mutex registry;
std::map<int64_t,std::shared_ptr<Session>> sessions;
int64_t nextSession = 1;

/**
 * @brief 查找尚未关闭的会话，保留其存活直到当前请求退出。
 * @param id 会话标识。
 * @return 会话或空指针。
 */
std::shared_ptr<Session> lookup(int64_t id)
{
    std::lock_guard<std::mutex> lock(registry);
    const auto found = sessions.find(id);
    return found == sessions.end() ? nullptr : found->second;
}

/**
 * @brief 将 libcurl 进度传给当前同步调用的取消回调。
 * @param context 存放回调的请求私有地址。
 * @param downloaded 已下载字节数。
 * @param uploaded 已上传字节数。
 * @return 请求取消时返回一。
 */
int progress(void *context, curl_off_t, curl_off_t downloaded, curl_off_t, curl_off_t uploaded)
{
    const auto cancel = *static_cast<Cancel *>(context);
    return cancel && cancel(downloaded, uploaded) ? 1 : 0;
}

/**
 * @brief 写入响应块；未请求输出文件时消费并丢弃正文。
 * @param bytes 收到的数据。
 * @param size 每项字节数。
 * @param count 项数。
 * @param file 输出 FILE 指针，可空。
 * @return 实际消费的字节数。
 */
size_t response(char *bytes, size_t size, size_t count, void *file)
{
    return file ? std::fwrite(bytes, size, count, static_cast<std::FILE *>(file)) * size : size * count;
}

/**
 * @brief 打开上传输入或独占创建下载目标，接受 UTF-8 路径。
 * @param path 平台文件系统路径。
 * @param output 输出时要求目标不存在。
 * @return 文件指针，失败时为空。
 */
std::FILE *openFile(const fs::path &path, bool output)
{
#ifdef _WIN32
    if (!output) return _wfopen(path.c_str(), L"rb");
    HANDLE handle = CreateFileW(path.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr);
    if (handle == INVALID_HANDLE_VALUE) return nullptr;
    const int fd = _open_osfhandle(reinterpret_cast<intptr_t>(handle),_O_WRONLY | _O_BINARY);
    if (fd < 0) { CloseHandle(handle); DeleteFileW(path.c_str()); return nullptr; }
    auto *file = _fdopen(fd,"wb");
    if (!file) { _close(fd); DeleteFileW(path.c_str()); }
    return file;
#else
    if (!output) return std::fopen(path.c_str(), "rb");
    const int fd = ::open(path.c_str(),O_CREAT | O_EXCL | O_WRONLY | O_CLOEXEC,0600);
    if (fd < 0) return nullptr;
    auto *file = fdopen(fd,"wb");
    if (!file) { ::close(fd); ::unlink(path.c_str()); }
    return file;
#endif
}

struct ResetOptions
{
    CURL *easy;

    /** @brief 撤销本次请求的回调与文件地址，保留缓存连接。 */
    ~ResetOptions() { curl_easy_reset(easy); }
};

struct Output
{
    fs::path path;
    std::FILE *file = nullptr;
    bool committed = false;

    /** @brief 关闭文件并清除本次失败传输留下的文件。 */
    ~Output()
    {
        if (!file) return;
        std::fclose(file);
        std::error_code error;
        if (!committed) fs::remove(path,error);
    }

    /**
     * @brief 关闭输出并在成功时保留文件。
     * @return 文件写入和关闭均成功时为真。
     */
    bool commit()
    {
        if (!file) return true;
        const bool ok = std::fclose(file) == 0;
        file = nullptr;
        if (!ok)
        {
            std::error_code error;
            fs::remove(path,error);
            return false;
        }
        committed = true;
        return true;
    }
};
}

/**
 * @brief 创建一个可重用连接的 HTTP 会话。
 * @return 正数会话标识，失败返回零。
 */
extern "C" int64_t chali_http_open() noexcept
{
    try
    {
        static const CURLcode initialized = curl_global_init(CURL_GLOBAL_DEFAULT);
        if (initialized != CURLE_OK) return 0;
        auto session = std::make_shared<Session>();
        if (!session->easy) return 0;
        std::lock_guard<std::mutex> lock(registry);
        if (nextSession == INT64_MAX) return 0;
        const auto id = nextSession++;
        sessions.emplace(id,std::move(session));
        return id;
    }
    catch (...) { return 0; }
}

/**
 * @brief 关闭会话，正在执行的请求结束后释放连接。
 * @param id 会话标识。
 */
extern "C" void chali_http_close(int64_t id) noexcept
{
    try
    {
        std::lock_guard<std::mutex> lock(registry);
        sessions.erase(id);
    }
    catch (...) {}
}

/**
 * @brief 传输 GET 或 PUT；请求可取消，响应流式写入新文件。
 * @param id 会话标识。
 * @param url HTTP 或 HTTPS 地址。
 * @param upload 上传文件，空串使用 GET。
 * @param download 新下载文件，空串丢弃响应正文。
 * @param timeout_ms 整个请求的正毫秒预算。
 * @param cancel 同步取消回调。
 * @return HTTP 状态或负数传输错误，-1000 表示本地状态无效。
 */
extern "C" int64_t chali_http_transfer(int64_t id, const char *url, const char *upload,
                                       const char *download, int64_t timeout_ms, Cancel cancel) noexcept
{
    try
    {
        if (timeout_ms <= 0 || timeout_ms > LONG_MAX) return -1000;
        auto session = lookup(id);
        if (!session) return -1000;
        std::unique_lock<std::mutex> guard(session->use, std::try_to_lock);
        if (!guard) return -1000;
        std::unique_ptr<std::FILE, decltype(&std::fclose)> input(nullptr,&std::fclose);
        curl_off_t length = 0;
        if (*upload)
        {
            const auto path = fs::u8path(upload);
            std::error_code error;
            if (!fs::is_regular_file(path,error) || error) return -1000;
            const auto size = fs::file_size(path,error);
            if (error || size > uint64_t(INT64_MAX)) return -1000;
            input.reset(openFile(path,false));
            if (!input) return -1000;
            length = static_cast<curl_off_t>(size);
        }
        Output output;
        if (*download)
        {
            output.path = fs::u8path(download);
            output.file = openFile(output.path,true);
            if (!output.file) return -1000;
        }
        CURL *easy = session->easy;
        const ResetOptions reset{easy};
        if (curl_easy_setopt(easy,CURLOPT_URL,url) != CURLE_OK ||
            curl_easy_setopt(easy,CURLOPT_PROTOCOLS_STR,"http,https") != CURLE_OK ||
            curl_easy_setopt(easy,CURLOPT_TIMEOUT_MS,static_cast<long>(timeout_ms)) != CURLE_OK ||
            curl_easy_setopt(easy,CURLOPT_NOSIGNAL,1L) != CURLE_OK ||
            curl_easy_setopt(easy,CURLOPT_NOPROGRESS,0L) != CURLE_OK ||
            curl_easy_setopt(easy,CURLOPT_XFERINFOFUNCTION,progress) != CURLE_OK ||
            curl_easy_setopt(easy,CURLOPT_XFERINFODATA,&cancel) != CURLE_OK ||
            curl_easy_setopt(easy,CURLOPT_WRITEFUNCTION,response) != CURLE_OK ||
            curl_easy_setopt(easy,CURLOPT_WRITEDATA,output.file) != CURLE_OK) return -1000;
        if (input && (curl_easy_setopt(easy,CURLOPT_UPLOAD,1L) != CURLE_OK ||
                      curl_easy_setopt(easy,CURLOPT_READDATA,input.get()) != CURLE_OK ||
                      curl_easy_setopt(easy,CURLOPT_INFILESIZE_LARGE,length) != CURLE_OK)) return -1000;
        const auto transfer = curl_easy_perform(easy);
        if (transfer != CURLE_OK) return -static_cast<int64_t>(transfer);
        long status = 0;
        if (curl_easy_getinfo(easy,CURLINFO_RESPONSE_CODE,&status) != CURLE_OK || status <= 0) return -1000;
        if (status >= 200 && status < 300 && !output.commit()) return -1000;
        return status;
    }
    catch (...) { return -1000; }
}
