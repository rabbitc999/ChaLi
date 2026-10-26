# 只在构建目录缓存经校验的依赖；源码与导出工程均使用同一锁定清单。
find_package(Python3 COMPONENTS Interpreter REQUIRED)
if(NOT DEFINED CHALI_DEPENDENCY_DIR)
    set(CHALI_DEPENDENCY_DIR "${CMAKE_BINARY_DIR}/dependencies")
endif()
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/fetch_dependencies.py"
    --dest "${CHALI_DEPENDENCY_DIR}"
    RESULT_VARIABLE CHALI_DEPENDENCY_STATUS)
if(NOT CHALI_DEPENDENCY_STATUS STREQUAL "0")
    message(FATAL_ERROR "SQLite/miniz dependency download or checksum verification failed")
endif()
set(CHALI_SQLITE_DIR "${CHALI_DEPENDENCY_DIR}/sqlite")
set(CHALI_MINIZ_DIR "${CHALI_DEPENDENCY_DIR}/miniz")
if(NOT TARGET chali_dependencies_check)
    add_custom_target(chali_dependencies_check
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/fetch_dependencies.py"
            --dest "${CHALI_DEPENDENCY_DIR}"
        COMMENT "Verify locked SQLite/miniz build dependencies")
endif()
