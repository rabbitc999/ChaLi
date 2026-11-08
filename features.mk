# NO_CHALI_* = 1 时加入对应宏；省略或 0 保持启用。直接 -D 定义按存在性裁剪。
CHALI_TRIM_OPTIONS := NO_CHALI_BIG_INT NO_CHALI_BIG_FLOAT NO_CHALI_DEFAULT_POOL NO_CHALI_STD NO_CHALI_STD_CAS NO_CHALI_STD_MATH NO_CHALI_STD_RANDOM NO_CHALI_STD_REGEX NO_CHALI_STD_SIGNAL NO_CHALI_STD_JSON NO_CHALI_STD_TOML NO_CHALI_STD_BINARY NO_CHALI_STD_LINALG NO_CHALI_STD_PARSE NO_CHALI_STD_PROCESS

override CHALI_WITH_REFLECTION := 1
override RUNTIME_WITH_REFLECTION := 1
CHALI_FEATURE_FLAGS = -DCHALI_WITH_REFLECTION=$(CHALI_WITH_REFLECTION) -DRUNTIME_WITH_REFLECTION=$(RUNTIME_WITH_REFLECTION)
CHALI_TRIM_FLAGS = $(CHALI_FEATURE_FLAGS) $(foreach option,$(CHALI_TRIM_OPTIONS),$(if $(filter 1,$($(option))),-D$(option)))

PYTHON_NULL := $(if $(filter sh bash sh.exe bash.exe,$(notdir $(SHELL))),/dev/null,NUL)
PYTHON ?= $(or $(shell python3 -c "import sys; print('python3' if sys.version_info.major == 3 else '')" 2>$(PYTHON_NULL)), $(shell python -c "import sys; print('python' if sys.version_info.major == 3 else '')" 2>$(PYTHON_NULL)))
PYTHON := $(PYTHON)
