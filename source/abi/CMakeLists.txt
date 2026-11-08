# 标准库的实现只编入宿主胶水库；运行时 核心继续使用通用原生记录协议。
file(GLOB CHALI_STANDARD_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB CHALI_BIGNUM_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../../source/bignum/bigint*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/../../source/bignum/chali_big*.c" "${CMAKE_CURRENT_SOURCE_DIR}/../../source/runtime_c/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/../../source/bignum/bigfloat*.cpp")
list(APPEND CHALI_STANDARD_SOURCES ${CHALI_BIGNUM_SOURCES}
    ../../source/json/json.cpp ../../source/toml/toml.cpp ../../source/toml/toml_source.cpp)
enable_language(C)
include("${CMAKE_CURRENT_SOURCE_DIR}/../../tools/cmake_dependencies.cmake")
find_package(Threads REQUIRED)
set(CHALI_CAS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../library/caspp")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CHALI_CAS_ROOT}/core_sources.txt")
file(STRINGS "${CHALI_CAS_ROOT}/core_sources.txt" CHALI_CAS_SOURCES)
list(TRANSFORM CHALI_CAS_SOURCES PREPEND "${CHALI_CAS_ROOT}/")
if(NO_CHALI_STD OR NO_CHALI_STD_CAS OR NO_CHALI_BIG_INT)
    set(CHALI_CAS_SOURCES "")
endif()
list(APPEND CHALI_STANDARD_SOURCES ${CHALI_CAS_SOURCES}
    ../../library/ipage/source/ipage/sql.cpp ../../library/ipage/source/ipage/dialect.cpp
    "${CHALI_SQLITE_DIR}/sqlite3.c" "${CHALI_YYJSON_DIR}/src/yyjson.c" "${CHALI_TOMLC17_DIR}/src/chali_tomlc17.c" ../../source/lztext/lztext.cpp)
set_source_files_properties("${CHALI_SQLITE_DIR}/sqlite3.c" PROPERTIES
    COMPILE_DEFINITIONS "SQLITE_THREADSAFE=1;SQLITE_OMIT_LOAD_EXTENSION=1")
set_source_files_properties("${CHALI_YYJSON_DIR}/src/yyjson.c" "${CHALI_TOMLC17_DIR}/src/chali_tomlc17.c" PROPERTIES
    COMPILE_DEFINITIONS "YYJSON_DISABLE_FILE=1;YYJSON_DISABLE_WRITER=1;YYJSON_DISABLE_UTILS=1;YYJSON_DISABLE_INCR_READER=1;YYJSON_READER_DEPTH_LIMIT=256")
add_library(chali_standard_records STATIC ${CHALI_STANDARD_SOURCES})
set_target_properties(chali_standard_records PROPERTIES C_STANDARD 11 C_STANDARD_REQUIRED ON)
add_dependencies(chali_standard_records chali_dependencies_check)
target_link_libraries(chali_standard_records PUBLIC Threads::Threads ${CMAKE_DL_LIBS})
target_include_directories(chali_standard_records PUBLIC
    .. ../../library ../../source/ircode ../../source/bignum ../../source/runtime_c ../../library/caspp/include
    "${CHALI_SQLITE_DIR}" "${CHALI_MINIZ_DIR}" "${CHALI_YYJSON_DIR}/src" "${CHALI_TOMLC17_DIR}/src")
target_compile_definitions(chali_standard_records PUBLIC CHALI_CUSTOM_ALLOCATOR=1
    CHALI_WITH_REFLECTION=$<BOOL:${RUNTIME_WITH_REFLECTION}>
    RUNTIME_WITH_REFLECTION=$<BOOL:${RUNTIME_WITH_REFLECTION}>)

# 与使用这些记录的 运行时 保持相同的预算与功能宏。
target_compile_definitions(chali_standard_records PUBLIC
    $<TARGET_PROPERTY:chali_stackvm,INTERFACE_COMPILE_DEFINITIONS>)
