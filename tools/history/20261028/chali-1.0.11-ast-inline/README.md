# AST 短函数内联验证

Windows x64 外置 LLVM 开发构建 SHA256：`03e95ebd2e387b892987285170d0193f5da41d461575dcf9e0212e3231d3e7a1`。

直接扫描已检查 AST，建立可达函数的调用依赖。自叶子向上累计展开成本，重复调用按实际次数计入；直接递归、相互递归、间接调用及依赖它们的调用者不进入强制展开集合。短函数预算 48，已识别的标准指针与集合操作包装预算 256；超过预算保留 LLVM 常规优化决策。导出函数、取地址函数、调试和看门狗模式保留相应边界。无四元式转换，没有手工重写用户函数的参数和局部身份。

`tests/fixtures/aot_inline_functions.chl` 核对参数副作用只执行一次、独立局部、直接递归、相互递归、具名及捕获闭包、字符串拥有值和具名 C ABI 导出。五组分别覆盖 malloc／pool、checked／unchecked 以及显式关闭内联提示，均退出零且输出与修改前一致。另验证 debug 模式不强制内联这些函数。

definitions.json 文件记录各组生成的函数属性：shortAdd、localValue 被标记展开，factorial、even／odd、间接调用 apply、取地址 doubleValue 及导出实现 inlineProbe 未被强制展开。将编译器生成的 LLVM 模块直接交给 clang -O3 -S -emit-llvm 后，动态参数路径中的 shortAdd 调用消失，证据见 optimized-inline-probe.txt。Python 只调度编译器并检查生成结果，不生成 LLVM IR；临时验证脚本未保留。

完整 AOT 专项、C 同步回调重入及 panic、集合传参／返回／深复制、malloc／pool 下自定义 Hash／Equal 碰撞模型均通过；数学特殊值输出继续与内联前一致。原始日志随目录保存。

本轮证据仅证明外置 LLVM 的代码生成和行为，不声称静态 LLVM、交叉 SDK 或性能成绩已完成；后续七项基准独立保存。
