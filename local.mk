ifeq ($(origin CC),default)
CC := gcc
endif

# C 侧直通胶水使用 C11；与 C++ 胶水一起并入宿主记录库。
ifeq ($(origin CFLAGS),default)
CFLAGS := -std=c11
endif
CFLAGS += -Isource -O2 -DNDEBUG

# local.mk —— 生成工程自己的挂载点（生成的 Makefile 末尾有 `-include local.mk`）。
#
# chali 自带 ast / vm / bignum 三份源码副本，只有前处理仍用仓库里的库：
#   * source/ast：C 子集语法树 → 四元式 + .data（降级目标）
#   * source/ircode：四元式分析、优化与 AOT 共享运行时（大整数用 source/bignum）
#   * source/preprocess：`%include` / `%if` / `%define` 那一步（源码进解析器之前）
# 三份副本各跑 `make -C source/<名字>`（产物仍落在仓库根的 `build/<名字>/`），
# 所有依赖都使用 CHALI 内部副本。
#
# 注意：`source/{ast,ircode,bignum}` 是从 ParseEngine 的 `library/` 复制进来的**副本**，
# 改了这边不会自动同步到 ParseEngine 那份（反之亦然）——动其中一个时想清楚要不要一起改。
#
# 生成源码与报告位于 source/，对象文件与静态库位于 $(CHALI_BUILD_ROOT)/chali-frontend/。
# 根目录只保留工具链入口与手写构建文件，重跑 gen.sh 不覆盖 local.mk。

AST_DIR := source/ast
VM_DIR  := source/ircode
PP_DIR  := source/preprocess
AST_LIB := $(CHALI_BUILD_ROOT)/chali-runtime/libast.a
VM_BUILD := $(CHALI_BUILD_ROOT)/chali-ircode-aot-core
HOST_BUILD := $(CHALI_BUILD_ROOT)/chali-host-records
HOST_LIB := $(HOST_BUILD)/librecords.a
CAS_BUILD := $(CHALI_BUILD_ROOT)/chali-cas
CAS_SOURCE_NAMES := $(shell $(PYTHON) tools/cas_sources.py)
CAS_OBJS := $(addprefix $(CAS_BUILD)/,$(CAS_SOURCE_NAMES:.cpp=.o))
IPAGE_OBJS := $(HOST_BUILD)/ipage_sql.o $(HOST_BUILD)/ipage_dialect.o
ifneq ($(filter 1,$(NO_CHALI_STD) $(NO_CHALI_STD_CAS) $(NO_CHALI_BIG_INT)),)
CAS_OBJS :=
endif
VM_LIB  := $(VM_BUILD)/libstackvm.a
VM_SOURCE_NAMES := $(shell $(PYTHON) tools/stackvm_sources.py --aot-only)
VM_OBJS := $(addprefix $(VM_BUILD)/,$(VM_SOURCE_NAMES:.cpp=.o))
PP_LIB  := $(CHALI_BUILD_ROOT)/chali-runtime/libpreprocess.a

ifeq ($(OS),Windows_NT)
CHALI_RUNTIME_MKDIR = powershell -NoProfile -Command "New-Item -ItemType Directory -Force -Path '$@' | Out-Null"
else
CHALI_RUNTIME_MKDIR = mkdir -p $@
endif

CHALI_SOURCE_DIR  := source
CHALI_LIBRARY_DIR := library
CHALI_DEPENDENCY_DIR := build/dependencies
CHALI_DEPENDENCY_STAMP := $(CHALI_DEPENDENCY_DIR)/.verified
$(CHALI_DEPENDENCY_STAMP): tools/fetch_dependencies.py tools/dependencies.lock.json
	$(PYTHON) tools/fetch_dependencies.py --dest $(CHALI_DEPENDENCY_DIR)
	$(PYTHON) -c "from pathlib import Path; Path(r'$@').touch()"
$(CHALI_DEPENDENCY_DIR)/sqlite/sqlite3.c $(CHALI_DEPENDENCY_DIR)/sqlite/sqlite3.h $(CHALI_DEPENDENCY_DIR)/miniz/miniz_tinfl.c $(CHALI_DEPENDENCY_DIR)/miniz/miniz_tinfl.h: $(CHALI_DEPENDENCY_STAMP)
	@$(PYTHON) tools/fetch_dependencies.py --dest $(CHALI_DEPENDENCY_DIR)
# 代码里的裸文件名 include 靠这组搜索路径解析：搬进 source/ 的九个库走 CHALI_SOURCE_DIR，
# caspp / ipage 走 CHALI_LIBRARY_DIR；SQLite / miniz 从构建缓存读取。
CHALI_INCLUDES := -I$(CHALI_SOURCE_DIR) -I$(CHALI_LIBRARY_DIR) \
    -I$(CHALI_SOURCE_DIR)/allocator -I$(CHALI_SOURCE_DIR)/lsp \
    -I$(CHALI_SOURCE_DIR)/bignum -I$(CHALI_SOURCE_DIR)/json -I$(CHALI_SOURCE_DIR)/toml \
    -I$(CHALI_DEPENDENCY_DIR)/sqlite -I$(CHALI_DEPENDENCY_DIR)/miniz -I$(CHALI_SOURCE_DIR)/console -I$(CHALI_SOURCE_DIR)/lztext
override CXXFLAGS += -DSTACKVM_AOT_ONLY=1 -DCHALI_CUSTOM_ALLOCATOR=1 -pthread $(CHALI_INCLUDES) -I$(AST_DIR) -I$(VM_DIR) -I$(PP_DIR)

# ---- 统一工具链：chali 同时是前端 / VM / 工程生成器 ----
# `$(FRONTEND_BUILD)/chali_main.o` 只提供 `chali_main_entry`，真正的 main 在 chali_tool.cpp；
# chali-generator 去掉自己的 main，改成进程内 `runCli`。
override CXXFLAGS += -DCHALI_FRONT_END_NO_MAIN=1

CHALI_TOOL_OBJS := $(FRONTEND_BUILD)/chali_tool.o $(FRONTEND_BUILD)/generator_cli.o
DEMO_OBJS += $(CHALI_TOOL_OBJS)
$(TARGET): $(CHALI_TOOL_OBJS)

$(CHALI_TOOL_OBJS): | $(FRONTEND_BUILD)

# 宿主只读取内嵌资产声明，压缩数据与解压缓存在独立编译单元中。
$(FRONTEND_BUILD)/chali_main.o $(FRONTEND_BUILD)/chali_tool.o $(FRONTEND_BUILD)/generator_cli.o: source/chali_module.hpp source/chali_stdlib.hpp

$(FRONTEND_BUILD)/chali_tool.o: source/chali_tool.cpp source/chali_package.hpp tools/abi_import.hpp source/generator/generator_cli.hpp \
        source/chali_runtime_allocator.hpp source/allocator/chali_allocator.hpp
	$(CXX) $(CXXFLAGS) -MMD -MP -I. -c $< -o $@

$(FRONTEND_BUILD)/generator_cli.o: source/generator/generator_cli.cpp source/generator/generator_cli.hpp \
        source/template/engine.hpp source/chali_runtime_allocator.hpp
	$(CXX) $(CXXFLAGS) -MMD -MP -DCHALI_GENERATOR_NO_MAIN=1 -I. -c $< -o $@

# 注意顺序：libparser.a 引用了 ast / stackvm 的符号，必须排在它们前面。
# 直通 ABI 桥接在宿主胶水库里引用 VM 核心的上下文与堆接口，两者互相引用，
# 用 start-group 保证静态库单遍扫描也能解全部符号。
LDLIBS   += $(AST_LIB) -Wl,--start-group $(VM_LIB) $(HOST_LIB) -Wl,--end-group $(PP_LIB)
ifeq ($(shell uname -s),Linux)
LDLIBS   += -ldl
endif

# `%main` / `%code` / `%epilogue` 的 include 会被生成器展开进 .cpp；显式列出原文件，
# 避免只改泛型 / 宏展开头或降级实现时 make 误判为无需重编。
$(FRONTEND_BUILD)/chali_tool.o $(FRONTEND_BUILD)/chali_main.o: source/chali_project.hpp source/chali_cli.hpp

$(FRONTEND_BUILD)/chali_main.o: source/chali_analysis.hpp source/chali_diagnostics.hpp source/chali_stdlib.hpp source/chali_package.hpp source/chali_driver.cpp source/chali_incremental.hpp source/chali_module.hpp source/macro/signature.hpp source/syntax/source_text.hpp source/chali_lambda.hpp source/chali_generic_closure.hpp source/chali_generic.hpp source/chali_glue.hpp $(CHALI_DEPENDENCY_DIR)/sqlite/sqlite3.h $(VM_DIR)/stackvm.hpp $(VM_DIR)/value_text.hpp $(VM_DIR)/reflection.hpp
$(FRONTEND_BUILD)/chali_parser.o: source/chali_ast.hpp source/chali_semantics.cpp source/chali_generic.hpp $(wildcard source/semantics/*.cpp) $(CHALI_DEPENDENCY_DIR)/sqlite/sqlite3.h source/chali_glue.hpp $(VM_DIR)/stackvm.hpp $(VM_DIR)/value_text.hpp $(VM_DIR)/reflection.hpp

# 外部静态库更新后必须重链；只把它们写进 LDLIBS 不会让 make 追踪时间戳。
$(TARGET): $(AST_LIB) $(VM_LIB) $(HOST_LIB) $(PP_LIB)

# ChaLi's VM copy includes the std bindings that are intentionally absent from
# the general-purpose source/ircode build. Keep its archive separate.
$(VM_BUILD) $(HOST_BUILD): | tmp
	$(CHALI_RUNTIME_MKDIR)

$(VM_BUILD)/%.o: $(VM_DIR)/%.cpp | $(VM_BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP -I$(VM_DIR) -c $< -o $@
-include $(wildcard $(VM_BUILD)/*.d)

$(HOST_BUILD)/bigint.o: source/bignum/bigint.cpp source/bignum/bigint.hpp | $(HOST_BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP -I$(CHALI_LIBRARY_DIR)/bignum -c $< -o $@

$(HOST_BUILD)/bigfloat.o: source/bignum/bigfloat.cpp source/bignum/bigfloat.hpp source/bignum/bigint.hpp | $(HOST_BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP -I$(CHALI_LIBRARY_DIR)/bignum -c $< -o $@

BIGINT_MODULE_OBJS := $(HOST_BUILD)/bigint_multiplication.o $(HOST_BUILD)/bigint_division.o $(HOST_BUILD)/bigint_number_theory.o $(HOST_BUILD)/bigint_bits.o $(HOST_BUILD)/bigint_modular.o $(HOST_BUILD)/bigint_conversion.o
$(BIGINT_MODULE_OBJS): $(HOST_BUILD)/%.o: source/bignum/%.cpp $(CHALI_BUILD_ROOT)/chali-runtime/buildConfig.json | $(HOST_BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP -I$(CHALI_LIBRARY_DIR)/bignum -c $< -o $@
-include $(BIGINT_MODULE_OBJS:.o=.d) $(HOST_BUILD)/bigint.d $(HOST_BUILD)/bigfloat.d

BIGFLOAT_MATH_OBJS := $(HOST_BUILD)/bigfloat_roots.o $(HOST_BUILD)/bigfloat_transcendental.o $(HOST_BUILD)/bigfloat_trigonometry.o
$(BIGFLOAT_MATH_OBJS): $(HOST_BUILD)/%.o: source/bignum/%.cpp $(CHALI_BUILD_ROOT)/chali-runtime/buildConfig.json | $(HOST_BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP -I$(CHALI_LIBRARY_DIR)/bignum -c $< -o $@

$(HOST_BUILD)/sqlite3.o: $(CHALI_DEPENDENCY_DIR)/sqlite/sqlite3.c $(CHALI_DEPENDENCY_DIR)/sqlite/sqlite3.h | $(HOST_BUILD)
	$(CC) $(CFLAGS) -std=c11 -O2 -DSQLITE_THREADSAFE=1 -DSQLITE_OMIT_LOAD_EXTENSION=1 -c $< -o $@

$(VM_LIB): $(VM_OBJS)
	$(PYTHON) -c "from pathlib import Path; Path(r'$@').unlink(missing_ok=True)"
	$(AR) $(ARFLAGS) $@ $^

# 每个标准库模块独立编译；实际包含关系由编译器生成 .d 文件追踪。
CHALI_STANDARD_SRCS := $(wildcard source/glue/std_*.cpp)
CHALI_STANDARD_OBJS := $(patsubst source/glue/%.cpp,$(HOST_BUILD)/%.o,$(CHALI_STANDARD_SRCS))
$(HOST_BUILD)/std_%.o: source/glue/std_%.cpp | $(HOST_BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP -I. -c $< -o $@
$(HOST_BUILD)/std_cas.o: private CXXFLAGS += -Ilibrary/caspp/include
$(HOST_BUILD)/json.o: source/json/json.cpp | $(HOST_BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP -I. -c $< -o $@
$(HOST_BUILD)/toml.o: source/toml/toml.cpp | $(HOST_BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP -I. -c $< -o $@
$(HOST_BUILD)/toml_source.o: source/toml/toml_source.cpp | $(HOST_BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP -I. -c $< -o $@
$(CHALI_STANDARD_OBJS) $(HOST_BUILD)/json.o $(HOST_BUILD)/toml.o $(HOST_BUILD)/toml_source.o: $(CHALI_BUILD_ROOT)/chali-runtime/buildConfig.json
-include $(wildcard $(HOST_BUILD)/*.d)

$(CAS_BUILD)/%.o: library/caspp/%.cpp
	$(PYTHON) -c "from pathlib import Path; Path(r'$(@D)').mkdir(parents=True, exist_ok=True)"
	$(CXX) $(CXXFLAGS) -Ilibrary/caspp/include -MMD -MP -c $< -o $@
-include $(shell $(PYTHON) -c "from pathlib import Path; print(' '.join(str(p).replace(chr(92), '/') for p in Path('$(CAS_BUILD)').rglob('*.d')))")
$(CAS_OBJS): $(CHALI_BUILD_ROOT)/chali-runtime/buildConfig.json

$(HOST_BUILD)/ipage_%.o: library/ipage/source/ipage/%.cpp | $(HOST_BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP -I. -c $< -o $@
$(IPAGE_OBJS): $(CHALI_BUILD_ROOT)/chali-runtime/buildConfig.json

# 直通 ABI：C 胶水按模块分文件编译，命名沿用 abi_*.c。
CHALI_ABI_C_SRCS := $(wildcard source/abi/*.c)
CHALI_ABI_CXX_SRCS := $(wildcard source/abi/*.cpp)
CHALI_ABI_OBJS := $(patsubst source/abi/%.c,$(HOST_BUILD)/abi_%.o,$(CHALI_ABI_C_SRCS)) \
                     $(patsubst source/abi/%.cpp,$(HOST_BUILD)/abi_%.o,$(CHALI_ABI_CXX_SRCS))
$(HOST_BUILD)/abi_%.o: source/abi/%.c | $(HOST_BUILD)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@
$(HOST_BUILD)/abi_%.o: source/abi/%.cpp | $(HOST_BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP -I. -c $< -o $@
$(CHALI_ABI_OBJS): $(CHALI_BUILD_ROOT)/chali-runtime/buildConfig.json

$(HOST_LIB): $(BIGINT_MODULE_OBJS) $(HOST_BUILD)/sqlite3.o $(HOST_BUILD)/bigint.o $(HOST_BUILD)/bigfloat.o $(BIGFLOAT_MATH_OBJS) $(CHALI_STANDARD_OBJS) $(HOST_BUILD)/json.o $(HOST_BUILD)/toml.o $(HOST_BUILD)/toml_source.o $(CAS_OBJS) $(IPAGE_OBJS) $(CHALI_ABI_OBJS)
	$(PYTHON) -c "from pathlib import Path; Path(r'$@').unlink(missing_ok=True)"
	$(AR) $(ARFLAGS) $@ $^

.PHONY: check samples

# Windows 上 make 找不到 sh：优先用 Git Bash / MSYS2 的 bash 跑 check.sh（与 project/mini-basic 的写法一致）。
ifeq ($(OS),Windows_NT)
  SH ?= $(firstword $(wildcard C:/PROGRA~1/Git/bin/bash.exe) \
                    $(wildcard C:/PROGRA~2/Git/bin/bash.exe) \
                    $(wildcard C:/msys64/usr/bin/bash.exe) sh)
else
  SH ?= sh
endif

# 自检：跑 example/samples/*.chl，逐个比对 example/expected/*.txt。
check: $(TARGET)
	$(SH) check.sh

# 只跑样例、不重新编译（改样例时用）。
samples: $(TARGET)
	$(SH) check.sh

# Keep allocator-specialized dependencies separate from the generic libraries.
$(CHALI_BUILD_ROOT)/chali-runtime: | tmp
	$(CHALI_RUNTIME_MKDIR)
AST_OBJS := $(CHALI_BUILD_ROOT)/chali-runtime/ast.o $(CHALI_BUILD_ROOT)/chali-runtime/ast_type.o $(CHALI_BUILD_ROOT)/chali-runtime/ast_node.o $(CHALI_BUILD_ROOT)/chali-runtime/ast_data.o
$(AST_OBJS): $(CHALI_BUILD_ROOT)/chali-runtime/%.o: $(AST_DIR)/%.cpp $(CHALI_BUILD_ROOT)/chali-runtime/buildConfig.json | $(CHALI_BUILD_ROOT)/chali-runtime
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@
-include $(AST_OBJS:.o=.d)
$(CHALI_BUILD_ROOT)/chali-runtime/preprocess.o: $(PP_DIR)/preprocess.cpp $(PP_DIR)/preprocess.hpp source/allocator/chali_allocator.hpp source/allocator/chali_pool.c | $(CHALI_BUILD_ROOT)/chali-runtime
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(AST_LIB): $(AST_OBJS)
	$(AR) $(ARFLAGS) $@ $^
$(PP_LIB): $(CHALI_BUILD_ROOT)/chali-runtime/preprocess.o
	$(AR) $(ARFLAGS) $@ $^
$(FRONTEND_BUILD)/chali_main.o $(FRONTEND_BUILD)/chali_parser.o $(VM_OBJS) $(HOST_BUILD)/bigint.o $(HOST_BUILD)/bigfloat.o: source/chali_runtime_allocator.hpp source/allocator/chali_allocator.hpp source/allocator/chali_pool.c source/allocator/chali_pool.h

.PHONY: config-force
$(CHALI_BUILD_ROOT)/chali-runtime/buildConfig.json: config-force
	$(PYTHON) tools/write_build_config.py $@ "$(CXX)" "$(CXXFLAGS)" "$(LDLIBS)" "$(LDFLAGS)"
$(LIB_OBJS) $(DEMO_OBJS) $(VM_OBJS) $(HOST_BUILD)/bigint.o $(HOST_BUILD)/bigfloat.o $(CHALI_BUILD_ROOT)/chali-runtime/ast.o $(CHALI_BUILD_ROOT)/chali-runtime/preprocess.o: $(CHALI_BUILD_ROOT)/chali-runtime/buildConfig.json

# JSON 与 TOML 实现由宿主库单独编译，包含依赖由 .d 文件维护。

# ---- ChaLi 宏/块 LSP（source/lsp）：编进编译器本体，不再另发可执行文件 --------------
# `chali lsp --macros` 在**本进程内**跑它（见 chali_tool.cpp 的分派）；共用件
# （最小 JSON 与头文件级共用类型）本仓库自带：`source/json/` 与 `source/lsp/protocol_types.hpp`，不再引用外面。
LSP_DIR := source/lsp
LSP_LIB_DIR := source/lsp
EMBEDDED_LSP_BUILD := $(CHALI_BUILD_ROOT)/chali-embedded-lsp

$(EMBEDDED_LSP_BUILD):
	$(CHALI_RUNTIME_MKDIR)
$(EMBEDDED_LSP_BUILD)/macro_analyzer.o: $(LSP_DIR)/macro_analyzer.cpp $(LSP_DIR)/macro_analyzer.hpp $(LSP_LIB_DIR)/protocol_types.hpp source/macro/signature.hpp source/syntax/source_text.hpp | $(EMBEDDED_LSP_BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP -I. -c $< -o $@
$(EMBEDDED_LSP_BUILD)/language_server.o: $(LSP_DIR)/language_server.cpp $(LSP_DIR)/language_server.hpp $(LSP_DIR)/macro_analyzer.hpp source/json/json.hpp | $(EMBEDDED_LSP_BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP -I. -c $< -o $@
CHALI_LSP_OBJS := $(EMBEDDED_LSP_BUILD)/macro_analyzer.o $(EMBEDDED_LSP_BUILD)/language_server.o
DEMO_OBJS += $(CHALI_LSP_OBJS)
$(TARGET): $(CHALI_LSP_OBJS)

# 静态消息表变更后所有相关翻译单元必须一致，避免旧消息表混入链接。
$(LIB_OBJS) $(DEMO_OBJS) $(VM_OBJS): source/chali_i18n.hpp source/console/utf8.hpp

# 单态化符号由前端与宿主胶水共享；更新编码时必须重编相关目标。
$(FRONTEND_BUILD)/chali_main.o $(FRONTEND_BUILD)/chali_parser.o: source/chali_generic_name.hpp
$(FRONTEND_BUILD)/chali_main.o $(FRONTEND_BUILD)/chali_parser.o: source/chali_lambda.hpp source/chali_report.hpp

$(FRONTEND_BUILD)/chali_parser.o $(FRONTEND_BUILD)/chali_main.o: source/chali_borrow_checker.hpp

$(FRONTEND_BUILD)/chali_parser.o $(FRONTEND_BUILD)/chali_main.o: source/chali_borrow_flow.hpp

# 原生语言服务和开发命令。
CHALI_NATIVE_DEVELOPMENT := $(CHALI_BUILD_ROOT)/chali-native-development.o
DEMO_OBJS += $(CHALI_NATIVE_DEVELOPMENT)
$(TARGET): $(CHALI_NATIVE_DEVELOPMENT)
$(CHALI_BUILD_ROOT)/chali-native: | tmp
	$(CHALI_RUNTIME_MKDIR)
$(CHALI_NATIVE_DEVELOPMENT): tools/native_development.cpp tools/native_development.hpp source/syntax/source_text.hpp source/chali_module.hpp source/chali_stdlib.hpp | $(CHALI_BUILD_ROOT)/chali-native
	$(CXX) $(CXXFLAGS) -MMD -MP -MF $(CHALI_BUILD_ROOT)/chali-native-development.d -I. -c $< -o $@
$(CHALI_NATIVE_DEVELOPMENT): source/chali_i18n.hpp source/console/utf8.hpp
$(FRONTEND_BUILD)/chali_tool.o: tools/native_development.hpp

# 内置 ZIP/Gzip 的 Deflate 解压器；仅构建解压部分。
CHALI_MINIZ := $(CHALI_BUILD_ROOT)/chali-miniz-tinfl.o
LIB_OBJS += $(CHALI_MINIZ)
$(LIB): $(CHALI_MINIZ)
$(CHALI_MINIZ): $(CHALI_DEPENDENCY_DIR)/miniz/miniz_tinfl.c $(CHALI_DEPENDENCY_DIR)/miniz/miniz_tinfl.h | $(CHALI_BUILD_ROOT)/chali-native
	$(CC) $(CFLAGS) -O2 -DMINIZ_NO_ARCHIVE_APIS -DMINIZ_NO_DEFLATE_APIS -c $< -o $@
ifeq ($(OS),Windows_NT)
LDLIBS += -lwinhttp
endif

-include $(CHALI_BUILD_ROOT)/chali-native-development.d

# 泛型与闭包文本处理实现独立编译，解析器和语言服务只依赖声明。
CHALI_FRONTEND_MODULES := chali_resource_cleanup chali_generic chali_lambda chali_generic_closure chali_package chali_report chali_aot chali_aot_target chali_aot_debug chali_aot_llvm chali_aot_runtime_assets chali_temp
CHALI_FRONTEND_MODULE_OBJS := $(addprefix $(FRONTEND_BUILD)/,$(addsuffix .o,$(CHALI_FRONTEND_MODULES)))
LIB_OBJS += $(CHALI_FRONTEND_MODULE_OBJS)
$(LIB): $(CHALI_FRONTEND_MODULE_OBJS)
$(CHALI_FRONTEND_MODULE_OBJS): $(FRONTEND_BUILD)/%.o: source/%.cpp | $(FRONTEND_BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP -I. -c $< -o $@
$(CHALI_FRONTEND_MODULE_OBJS): $(CHALI_BUILD_ROOT)/chali-runtime/buildConfig.json
-include $(wildcard $(FRONTEND_BUILD)/*.d)

# 静态 LLVM 变体只替换后端隔离单元，使用匹配 GCC 并隔离全部对象文件。
CHALI_STATIC_LLVM ?= 0
CHALI_LLVM_SDK ?= build/dependencies/llvm/static-sdk
$(CHALI_BUILD_ROOT)/chali-native/llvmLinkMode.json: config-force
	$(PYTHON) tools/write_build_config.py $@ "$(CHALI_STATIC_LLVM)" "$(CHALI_LLVM_SDK)" "$(CHALI_AOT_LDLIBS)"
$(TARGET): $(CHALI_BUILD_ROOT)/chali-native/llvmLinkMode.json
ifeq ($(CHALI_STATIC_LLVM),1)
ifneq ($(OS),Windows_NT)
$(error 当前静态 LLVM 分发仅验收 Windows x64)
endif
override CXXFLAGS += --sysroot=$(abspath $(CHALI_LLVM_SDK))
LDFLAGS += --sysroot=$(abspath $(CHALI_LLVM_SDK)) -fuse-ld=lld
CHALI_LLVM_CONFIG := $(CHALI_LLVM_SDK)/bin/llvm-config.exe
ifeq ($(wildcard $(CHALI_LLVM_CONFIG)),)
$(error 请先执行 python library/llvm/setup_static.py 恢复静态 LLVM 开发库)
endif
CHALI_LLVM_STATIC_OBJ := $(CHALI_BUILD_ROOT)/chali-native/chali_aot_llvm_static.o
CHALI_LLVM_COMPONENTS := core irreader passes x86codegen x86asmparser dtlto libdriver lto option windowsdriver windowsmanifest all-targets
CHALI_LLVM_LIBS := $(shell $(CHALI_LLVM_CONFIG) --link-static --libs $(CHALI_LLVM_COMPONENTS))
CHALI_LLVM_SYSTEM_LIBS := $(subst -lzstd.dll,-lzstd,$(shell $(CHALI_LLVM_CONFIG) --link-static --system-libs $(CHALI_LLVM_COMPONENTS))) -liconv -lbcrypt
DEMO_OBJS += $(CHALI_LLVM_STATIC_OBJ)
CHALI_LLVM_IMPORT_OBJ := $(CHALI_BUILD_ROOT)/chali-native/llvm_static_imports.o
DEMO_OBJS += $(CHALI_LLVM_IMPORT_OBJ)
$(TARGET): $(CHALI_LLVM_IMPORT_OBJ)
$(CHALI_LLVM_IMPORT_OBJ): tools/llvm_static_imports.cpp | $(CHALI_BUILD_ROOT)/chali-native
	$(CXX) $(CXXFLAGS) -I$(CHALI_LLVM_SDK)/include/libxml2 -MMD -MP -c $< -o $@
$(TARGET): $(CHALI_LLVM_STATIC_OBJ)
CHALI_AOT_LDLIBS := -L$(CHALI_LLVM_SDK)/lib -Wl,--start-group -llldMinGW -llldCOFF -llldCommon $(CHALI_LLVM_LIBS) $(CHALI_LLVM_SYSTEM_LIBS) -Wl,--end-group
$(CHALI_LLVM_STATIC_OBJ): source/chali_aot_llvm.cpp source/chali_aot_llvm.hpp source/chali_aot.hpp $(CHALI_LLVM_CONFIG) | $(CHALI_BUILD_ROOT)/chali-native
	$(CXX) $(CXXFLAGS) -DCHALI_STATIC_LLVM=1 -DLLVM_BUILD_STATIC -I$(CHALI_LLVM_SDK)/include -Ilibrary -MMD -MP -c $< -o $@
-include $(CHALI_BUILD_ROOT)/chali-native/chali_aot_llvm_static.d
endif

# 编译期 AST 宏的独立实现，避免 regex 等重型头传播到解析器接口。
CHALI_MACRO_MODULES := hygiene regex_matcher source fragments ast_substitution template_binding registry signature
CHALI_MACRO_OBJS := $(addprefix $(FRONTEND_BUILD)/macro_,$(addsuffix .o,$(CHALI_MACRO_MODULES)))
LIB_OBJS += $(CHALI_MACRO_OBJS)
$(LIB): $(CHALI_MACRO_OBJS)
$(CHALI_MACRO_OBJS): $(FRONTEND_BUILD)/macro_%.o: source/macro/%.cpp | $(FRONTEND_BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP -I. -c $< -o $@
$(CHALI_MACRO_OBJS): $(CHALI_BUILD_ROOT)/chali-runtime/buildConfig.json

CHALI_LITERAL_OBJ := $(FRONTEND_BUILD)/string_escape.o
LIB_OBJS += $(CHALI_LITERAL_OBJ)
$(LIB): $(CHALI_LITERAL_OBJ)
$(CHALI_LITERAL_OBJ): source/literals/string_escape.cpp source/literals/string_escape.hpp | $(FRONTEND_BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP -I. -c $< -o $@

CHALI_EDITOR_ANALYSIS_OBJ := $(FRONTEND_BUILD)/compiler_analysis.o
LIB_OBJS += $(CHALI_EDITOR_ANALYSIS_OBJ)
$(LIB): $(CHALI_EDITOR_ANALYSIS_OBJ)
$(CHALI_EDITOR_ANALYSIS_OBJ): source/editor/compiler_analysis.cpp source/editor/compiler_analysis.hpp | $(FRONTEND_BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP -I. -c $< -o $@

CHALI_SYNTAX_MODULES := source_text constant_expression
CHALI_SYNTAX_OBJS := $(addprefix $(FRONTEND_BUILD)/syntax_,$(addsuffix .o,$(CHALI_SYNTAX_MODULES)))
LIB_OBJS += $(CHALI_SYNTAX_OBJS)
$(LIB): $(CHALI_SYNTAX_OBJS)
$(CHALI_SYNTAX_OBJS): $(FRONTEND_BUILD)/syntax_%.o: source/syntax/%.cpp | $(FRONTEND_BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP -I. -c $< -o $@
$(CHALI_LITERAL_OBJ): $(CHALI_BUILD_ROOT)/chali-runtime/buildConfig.json

CHALI_EMBED_SOURCE := $(FRONTEND_BUILD)/chali_stdlib_embedded.cpp
CHALI_STDLIB_OBJ := $(FRONTEND_BUILD)/chali_stdlib_embedded.o
LIB_OBJS += $(CHALI_STDLIB_OBJ)
$(LIB): $(CHALI_STDLIB_OBJ)
$(CHALI_STDLIB_OBJ): $(CHALI_EMBED_SOURCE) source/chali_stdlib.hpp | $(FRONTEND_BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP -I. -c $< -o $@
$(CHALI_STDLIB_OBJ): $(CHALI_BUILD_ROOT)/chali-runtime/buildConfig.json

-include $(wildcard $(EMBEDDED_LSP_BUILD)/*.d)

# AOT 复用共享原生服务；先构建静态库资产，再嵌入编译器，首次克隆同样自动完成。
CHALI_AOT_HOST_BUILD := $(CHALI_BUILD_ROOT)/chali-aot-host
CHALI_AOT_HOST_OBJECT := $(CHALI_AOT_HOST_BUILD)/native_runtime.o
CHALI_AOT_SYSTEM_OBJECT := $(CHALI_AOT_HOST_BUILD)/system_arena.o
CHALI_AOT_PROFILE_OBJECT := $(CHALI_AOT_HOST_BUILD)/backend_profile.o
$(CHALI_AOT_PROFILE_OBJECT): source/aot/backend_profile.cpp $(CHALI_BUILD_ROOT)/chali-runtime/buildConfig.json
	$(PYTHON) -c "from pathlib import Path; Path(r'$(@D)').mkdir(parents=True, exist_ok=True)"
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@
$(CHALI_AOT_HOST_OBJECT): source/aot/native_runtime.cpp $(CHALI_BUILD_ROOT)/chali-runtime/buildConfig.json
	$(PYTHON) -c "from pathlib import Path; Path(r'$(@D)').mkdir(parents=True, exist_ok=True)"
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@
$(CHALI_AOT_SYSTEM_OBJECT): source/allocator/system_arena.cpp $(CHALI_BUILD_ROOT)/chali-runtime/buildConfig.json
	$(PYTHON) -c "from pathlib import Path; Path(r'$(@D)').mkdir(parents=True, exist_ok=True)"
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@
-include $(CHALI_AOT_HOST_BUILD)/native_runtime.d
-include $(CHALI_AOT_HOST_BUILD)/backend_profile.d
-include $(CHALI_AOT_HOST_BUILD)/system_arena.d
$(CHALI_AOT_HOST_BUILD)/assets.txt: $(CHALI_AOT_HOST_OBJECT) $(CHALI_AOT_PROFILE_OBJECT) $(CHALI_AOT_SYSTEM_OBJECT) $(VM_LIB) $(HOST_LIB) $(FRONTEND_BUILD)/lztext.o tools/build_aot_host.py tools/embedded_asset_cache.py
	$(PYTHON) tools/build_aot_host.py --build-root "$(CHALI_BUILD_ROOT)" --cxx "$(CXX)" --ar "$(AR)" --object $(CHALI_AOT_HOST_OBJECT) --object $(CHALI_AOT_PROFILE_OBJECT) --object $(CHALI_AOT_SYSTEM_OBJECT) --output $@

# 更新标准库或胶水实现时只刷新单个内嵌资产对象，不重编宿主入口。
CHALI_EMBED_INPUTS := $(shell $(PYTHON) tools/embed_stdlib.py --build-root "$(CHALI_BUILD_ROOT)" --dependencies)
CHALI_EMBED_STAMP := $(FRONTEND_BUILD)/stdlib_assets.stamp
$(CHALI_EMBED_STAMP): $(CHALI_EMBED_INPUTS) | $(FRONTEND_BUILD)
	$(PYTHON) tools/embed_stdlib.py --build-root "$(CHALI_BUILD_ROOT)" --output "$(CHALI_EMBED_SOURCE)" --stamp $@
$(CHALI_EMBED_SOURCE): $(CHALI_EMBED_STAMP)
	$(PYTHON) tools/embed_stdlib.py --build-root "$(CHALI_BUILD_ROOT)" --output "$(CHALI_EMBED_SOURCE)"
$(CHALI_STDLIB_OBJ): | $(CHALI_EMBED_STAMP)

# 依赖只落入构建缓存；并行编译须等待校验结束。
.PHONY: chali-dependencies
chali-dependencies:
	$(PYTHON) tools/fetch_dependencies.py --dest $(CHALI_DEPENDENCY_DIR)
$(LIB_OBJS) $(DEMO_OBJS) $(CHALI_STANDARD_OBJS) $(HOST_BUILD)/ipage_sql.o $(CHALI_MINIZ): | chali-dependencies

# 标准库核心二进制装载器；构建生成的字节数组从权威生成脚本维护。
CHALI_PRECOMPILED_OBJ := $(FRONTEND_BUILD)/chali_precompiled.o
LIB_OBJS += $(CHALI_PRECOMPILED_OBJ)
$(LIB): $(CHALI_PRECOMPILED_OBJ)
$(CHALI_PRECOMPILED_OBJ): source/chali_precompiled.cpp source/chali_precompiled.hpp source/chali_ast.hpp source/chali_incremental.hpp source/chali_package.hpp source/chali_i18n.hpp $(wildcard source/chali_precompiled_data.inc) | $(FRONTEND_BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP -I. -c $< -o $@
$(FRONTEND_BUILD)/chali_main.o: source/chali_precompiled.hpp
