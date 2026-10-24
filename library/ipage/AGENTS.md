# ipage SQL 核心维护约定

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](AGENTS.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

本目录维护 C++17 SQL 生成后端，仅负责模型元数据、条件树、查询和方言；数据库宿主适配位于 `source/glue/std_sql_ipage.cpp` 与 `std_sql.cpp`。其余约定沿用根目录 `AGENTS.md`。

- 依赖仅限 C++17 标准库。`source/Makefile` 和 `source/CMakeLists.txt` 都必须可用。
- 标识符经过 `Dialect::quote` / `qualify`；字段名通过 `ModelMeta` 白名单校验。
- 参数值只进入占位符和 `SqlStatement::params()`，不得拼接到 SQL 文本。
- 表名来自 `ModelMeta::table()`，别名使用 `t_` 前缀；聚合分组覆盖非聚合选择列。
- `Field<T,M>` 负责声明期成员绑定，`FieldInfo` 与非模板 `Query` 负责运行期生成。
- 核心测试只断言 SQL、参数、白名单、连接与聚合结构，不连接数据库。
  宿主适配按改动核对实际数据库行为；用户要求不运行门禁时遵从。
- 修改核心后同步 ChaLi 构建、内嵌及导出资产；根目录 `TASK.md` 若存在则记录当前任务。
