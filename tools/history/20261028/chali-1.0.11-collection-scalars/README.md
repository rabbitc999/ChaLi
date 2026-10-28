# 集合标量结果验收

Windows x64 外置 LLVM 编译器 SHA256：`af3bbacec00aa5d2fc9858f517b6e99f3d8c765c226fee09076bf4e9632b6d3c`。List／Map／Set 直接返回无所有权的整数／浮点位模式与种类，LLVM 在当前函数帧接收种类并转入 SSA，避免额外 TLS getter。原 NativeValue 接口和宿主 ABI 保留，设置 CHALI_NO_AOT_COLLECTION_SCALARS=1 可回退。没有增加四元式转换，也没有通过 Python 生成 LLVM IR。

variants.json 包含修改前编译器与本轮八组 malloc／pool、checked／unchecked、标量入口开关对照；浮点正负零、NaN 载荷、正负无穷与次正规数位模式保留，List／Map／Set 副本独立，用户 Hash／Equal 内嵌浮点 List 调用后外层结果正确。生成模块新入口调用点亦按开关核对。

完整 AOT 专项、同步 C 回调、集合传参／返回／复制，以及 malloc／pool 的固定种子哈希碰撞模型通过。日志保留真实输出。尚未据此宣称性能收益；正式只测 ChaLi 的复测随后进行，其他语言复用原基线。静态 LLVM、跨目标与安装包未在本里程碑重新验收。
