/** @file chali_native_library.hpp @brief 原生运行时的私有操作系统动态库资源。 */
#pragma once
#include "allocator/chali_allocator.hpp"

namespace chali_host_detail
{
/** @brief 持有一个操作系统库引用，不负责语言类型封送或函数登记。 */
class NativeLibrary
{
public:
    /** @brief 创建尚未打开的库资源。 */
    NativeLibrary() = default;
    /** @brief 关闭本对象持有的操作系统库引用。 */
    ~NativeLibrary();
    NativeLibrary(const NativeLibrary &) = delete;
    NativeLibrary &operator=(const NativeLibrary &) = delete;
    /**
     * @brief 关闭旧引用后按 UTF-8 路径打开动态库。
     * @param path 库文件路径，反斜杠按目录分隔符处理。
     * @param error 接收加载失败诊断，可为空。
     * @return 打开成功时为 true。
     */
    bool open(const Memory::String &path, Memory::String *error);
    /**
     * @brief 按精确 C 符号名查询地址。
     * @param name 区分大小写的符号名。
     * @param error 接收查询失败诊断，可为空。
     * @return 原生地址，失败时为空。
     */
    void *resolve(const Memory::String &name, Memory::String *error) const;

private:
    void *handle = nullptr;  ///< 只由本对象释放的系统句柄。
};
}  // namespace chali_host_detail
