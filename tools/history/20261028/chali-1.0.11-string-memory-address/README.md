ChaLi 字符串头写入和字节写入使用直接地址引用；复制使用已有裸地址 memcpy 内建，AST 直接到 LLVM MemoryCopy，保留原范围验证、求值顺序和原语回退。旧版 6e85699 对照、四组分配器／检查模式、关闭 raw 原语和关闭 inline hint 配置、完整 String 自举及四组内存 API／LLVM 检查通过。

初始 inline-off 行误用了不存在的 CHALI_NO_AOT_AST_INLINE，实际为重复默认配置，不能算关闭测试。随后以真实 CHALI_NO_AOT_INLINE_HINT 补测通过，见 actual-inline-off-results.json。初始统计误匹配了 chali_host_alloc，正确符号为 chali_host_allocate；以 *-stats-corrected.json 为准。

O3 独立函数体静态分配位置：Buffer 3→2，CopyBytes 4→0，Set 6→2；不能将这些计数当作整个程序动态分配次数。实际性能需提交后仅测 ChaLi。
