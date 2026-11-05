/**
 * @file abi_toml.c
 * @brief TOML 拥有值的纯 C 帧接口。
 */
#include "chali_standard.h"
#include "../runtime_c/chali_toml.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
static const char type_name[] = "std::toml::TomlValue";
typedef enum TomlOperation
{
    New,
    Copy,
    Drop,
    Parse,
    Valid,
    Error,
    Close,
    Kind,
    Size,
    Int,
    Float,
    Bool,
    TextAvailable,
    String,
    Bytes,
    DateTime,
    Dump,
    Find,
    FindBytes,
    At,
    KeyAvailable,
    Key,
    KeyBytes
} TomlOperation;
/**
 * @brief 通过记录销毁协议释放 TOML C 服务。
 * @param value 拥有值或只读输入节点。
 */
static void CHALI_ABI_CALL destroy_value(void *value) { chali_toml_free(value); }
/**
 * @brief 通过记录复制协议创建独立 TOML。
 * @param value 拥有值或只读输入节点。
 * @return 独立拥有的 C 结果，失败为空；调用方负责归还。
 */
static void *CHALI_ABI_CALL clone_value(const void *value) { return chali_toml_clone(value); }
/**
 * @brief 完整构造后发布独占语言记录。
 * @param frame 当前 C 调用帧。
 * @param value 拥有值或只读输入节点。
 * @return 操作或查询结果，失败使用零状态。
 */
static int64_t make_value(const ChaliAbiFrame *frame, ChaliToml *value)
{
    int64_t handle;
    if (!value)
    {
        chali_abi_fail(frame, "TOML 内核分配失败");
        return 0;
    }
    handle = chali_abi_record_make_clonable(frame, type_name, value, destroy_value, clone_value);
    if (!handle) chali_toml_free(value);
    return handle;
}
/**
 * @brief 复制为语言文本后归还 C 输出缓冲。
 * @param frame 当前 C 调用帧。
 * @param text 输入字节。
 * @param length 输入字节数或接收字节数的指针。
 */
static void return_text(ChaliAbiFrame *frame, char *text, size_t length)
{
    if (!text)
    {
        chali_abi_fail(frame, "TOML 文本分配失败");
        return;
    }
    chali_abi_return_integer(frame, chali_abi_string(frame, text, length));
    free(text);
}
/**
 * @brief 核对 C 帧实参并执行固定 TOML 节点操作。
 * @param frame 当前 C 调用帧。
 * @param operation 固定操作编号。
 */
static void toml_entry(ChaliAbiFrame *frame, TomlOperation operation)
{
    int64_t handle = 0, ignored = 0, index = 0;
    ChaliToml *value;
    const unsigned char *input = NULL;
    size_t length = 0;
    if (operation == New)
    {
        chali_abi_return_integer(frame, make_value(frame, chali_toml_new()));
        return;
    }
    if (!chali_abi_argument_integer(frame, 0, &handle)) return;
    if (operation == Parse)
    {
        if (!chali_abi_bytes(frame, handle, &input, &length)) return;
        chali_abi_return_integer(frame, make_value(frame, chali_toml_parse((const char *)input, length)));
        return;
    }
    if (operation == Copy)
    {
        chali_abi_return_integer(frame, chali_abi_record_clone(frame, handle, type_name, clone_value));
        return;
    }
    if (operation == Drop)
    {
        if (!chali_abi_record_release(frame, handle)) chali_abi_fail(frame, "[CHL1205] drop 的对象已经释放或句柄无效");
        chali_abi_return_integer(frame, 0);
        return;
    }
    if ((operation == Valid || operation == Close) && !handle)
    {
        chali_abi_return_integer(frame, 0);
        return;
    }
    if (operation >= Kind && !chali_abi_argument_integer(frame, 1, &ignored)) return;
    if (operation >= Find && !chali_abi_argument_integer(frame, 2, &index)) return;
    value = chali_abi_record_borrow(frame, handle, type_name);
    if (!value)
    {
        chali_abi_fail(frame, "结构化数据：值已经释放或类型无效");
        return;
    }
    if (operation == Valid)
    {
        chali_abi_return_integer(frame, chali_toml_valid(value));
        return;
    }
    if (operation == Error)
    {
        const char *error = chali_toml_error(value);
        chali_abi_return_integer(frame, chali_abi_string(frame, error, strlen(error)));
        return;
    }
    if (operation == Close)
    {
        int valid = chali_toml_valid(value);
        chali_toml_close(value);
        chali_abi_return_integer(frame, valid);
        return;
    }
    if (!chali_toml_valid(value))
    {
        chali_abi_fail(frame, "结构化数据：值已经关闭或未初始化");
        return;
    }
    switch (operation)
    {
    case Kind: chali_abi_return_integer(frame, chali_toml_kind(value)); break;
    case Size: chali_abi_return_integer(frame, (int64_t)chali_toml_size(value)); break;
    case Int: chali_abi_return_integer(frame, chali_toml_integer(value)); break;
    case Float: chali_abi_return_real(frame, chali_toml_real(value)); break;
    case Bool: chali_abi_return_integer(frame, chali_toml_boolean(value)); break;
    case TextAvailable:
    case String:
    case Bytes:
    {
        const char *text = chali_toml_text(value, &length);
        if (operation == TextAvailable)
            chali_abi_return_integer(frame, text && !memchr(text, 0, length));
        else if (operation == String)
            chali_abi_return_integer(frame, chali_abi_string_bytes(frame, text ? text : "", length));
        else
            chali_abi_return_integer(frame, chali_abi_list_from_bytes(frame, (const unsigned char *)(text ? text : ""), length));
        break;
    }
    case DateTime:
    {
        const char *text = chali_toml_date_time(value, &length);
        chali_abi_return_integer(frame, chali_abi_string(frame, text ? text : "", length));
        break;
    }
    case Dump:
    {
        char *text = chali_toml_dump(value, &length);
        return_text(frame, text, length);
        break;
    }
    case At: chali_abi_return_integer(frame, make_value(frame, chali_toml_at(value, index < 0 ? SIZE_MAX : (size_t)index))); break;
    case Find:
    case FindBytes:
    {
        unsigned char *owned = NULL;
        ChaliToml *child;
        if (operation == Find)
        {
            if (!chali_abi_bytes(frame, index, &input, &length)) return;
        }
        else
        {
            if (!chali_abi_list_bytes(frame, index, NULL, 0, &length)) return;
            owned = malloc(length ? length : 1);
            if (!owned)
            {
                chali_abi_fail(frame, "TOML 键分配失败");
                return;
            }
            if (!chali_abi_list_bytes(frame, index, owned, length, &length))
            {
                free(owned);
                return;
            }
            input = owned;
        }
        child = chali_toml_find(value, (const char *)(input ? input : (const unsigned char *)""), length);
        free(owned);
        chali_abi_return_integer(frame, make_value(frame, child));
        break;
    }
    case Key:
    case KeyBytes:
    case KeyAvailable:
    {
        const char *key = chali_toml_key(value, index < 0 ? SIZE_MAX : (size_t)index, &length);
        if (operation == KeyAvailable)
            chali_abi_return_integer(frame, key && !memchr(key, 0, length));
        else if (operation == Key)
            chali_abi_return_integer(frame, chali_abi_string_bytes(frame, key ? key : "", length));
        else
            chali_abi_return_integer(frame, chali_abi_list_from_bytes(frame, (const unsigned char *)(key ? key : ""), length));
        break;
    }
    default: break;
    }
}

/**
 * @brief 执行对应 TOML 节点操作的 C 帧入口。
 * @param frame 当前 C 调用帧。
 */
static void CHALI_ABI_CALL toml_at_entry(ChaliAbiFrame *frame) { toml_entry(frame, At); }

/**
 * @brief 执行对应 TOML 节点操作的 C 帧入口。
 * @param frame 当前 C 调用帧。
 */
static void CHALI_ABI_CALL toml_bool_entry(ChaliAbiFrame *frame) { toml_entry(frame, Bool); }

/**
 * @brief 执行对应 TOML 节点操作的 C 帧入口。
 * @param frame 当前 C 调用帧。
 */
static void CHALI_ABI_CALL toml_bytes_entry(ChaliAbiFrame *frame) { toml_entry(frame, Bytes); }

/**
 * @brief 执行对应 TOML 节点操作的 C 帧入口。
 * @param frame 当前 C 调用帧。
 */
static void CHALI_ABI_CALL toml_close_entry(ChaliAbiFrame *frame) { toml_entry(frame, Close); }

/**
 * @brief 执行对应 TOML 节点操作的 C 帧入口。
 * @param frame 当前 C 调用帧。
 */
static void CHALI_ABI_CALL toml_date_time_entry(ChaliAbiFrame *frame) { toml_entry(frame, DateTime); }

/**
 * @brief 执行对应 TOML 节点操作的 C 帧入口。
 * @param frame 当前 C 调用帧。
 */
static void CHALI_ABI_CALL toml_dump_entry(ChaliAbiFrame *frame) { toml_entry(frame, Dump); }

/**
 * @brief 执行对应 TOML 节点操作的 C 帧入口。
 * @param frame 当前 C 调用帧。
 */
static void CHALI_ABI_CALL toml_find_entry(ChaliAbiFrame *frame) { toml_entry(frame, Find); }

/**
 * @brief 执行对应 TOML 节点操作的 C 帧入口。
 * @param frame 当前 C 调用帧。
 */
static void CHALI_ABI_CALL toml_find_bytes_entry(ChaliAbiFrame *frame) { toml_entry(frame, FindBytes); }

/**
 * @brief 执行对应 TOML 节点操作的 C 帧入口。
 * @param frame 当前 C 调用帧。
 */
static void CHALI_ABI_CALL toml_float_entry(ChaliAbiFrame *frame) { toml_entry(frame, Float); }

/**
 * @brief 执行对应 TOML 节点操作的 C 帧入口。
 * @param frame 当前 C 调用帧。
 */
static void CHALI_ABI_CALL toml_int_entry(ChaliAbiFrame *frame) { toml_entry(frame, Int); }

/**
 * @brief 执行对应 TOML 节点操作的 C 帧入口。
 * @param frame 当前 C 调用帧。
 */
static void CHALI_ABI_CALL toml_key_entry(ChaliAbiFrame *frame) { toml_entry(frame, Key); }

/**
 * @brief 执行对应 TOML 节点操作的 C 帧入口。
 * @param frame 当前 C 调用帧。
 */
static void CHALI_ABI_CALL toml_key_available_entry(ChaliAbiFrame *frame) { toml_entry(frame, KeyAvailable); }

/**
 * @brief 执行对应 TOML 节点操作的 C 帧入口。
 * @param frame 当前 C 调用帧。
 */
static void CHALI_ABI_CALL toml_key_bytes_entry(ChaliAbiFrame *frame) { toml_entry(frame, KeyBytes); }

/**
 * @brief 执行对应 TOML 节点操作的 C 帧入口。
 * @param frame 当前 C 调用帧。
 */
static void CHALI_ABI_CALL toml_kind_entry(ChaliAbiFrame *frame) { toml_entry(frame, Kind); }

/**
 * @brief 执行对应 TOML 节点操作的 C 帧入口。
 * @param frame 当前 C 调用帧。
 */
static void CHALI_ABI_CALL toml_parse_entry(ChaliAbiFrame *frame) { toml_entry(frame, Parse); }

/**
 * @brief 执行对应 TOML 节点操作的 C 帧入口。
 * @param frame 当前 C 调用帧。
 */
static void CHALI_ABI_CALL toml_size_entry(ChaliAbiFrame *frame) { toml_entry(frame, Size); }

/**
 * @brief 执行对应 TOML 节点操作的 C 帧入口。
 * @param frame 当前 C 调用帧。
 */
static void CHALI_ABI_CALL toml_string_entry(ChaliAbiFrame *frame) { toml_entry(frame, String); }

/**
 * @brief 执行对应 TOML 节点操作的 C 帧入口。
 * @param frame 当前 C 调用帧。
 */
static void CHALI_ABI_CALL toml_take_error_entry(ChaliAbiFrame *frame) { toml_entry(frame, Error); }

/**
 * @brief 执行对应 TOML 节点操作的 C 帧入口。
 * @param frame 当前 C 调用帧。
 */
static void CHALI_ABI_CALL toml_text_available_entry(ChaliAbiFrame *frame) { toml_entry(frame, TextAvailable); }

/**
 * @brief 执行对应 TOML 节点操作的 C 帧入口。
 * @param frame 当前 C 调用帧。
 */
static void CHALI_ABI_CALL toml_valid_entry(ChaliAbiFrame *frame) { toml_entry(frame, Valid); }

/**
 * @brief 执行对应 TOML 节点操作的 C 帧入口。
 * @param frame 当前 C 调用帧。
 */
static void CHALI_ABI_CALL toml_value_copy_entry(ChaliAbiFrame *frame) { toml_entry(frame, Copy); }

/**
 * @brief 执行对应 TOML 节点操作的 C 帧入口。
 * @param frame 当前 C 调用帧。
 */
static void CHALI_ABI_CALL toml_value_drop_entry(ChaliAbiFrame *frame) { toml_entry(frame, Drop); }

/**
 * @brief 执行对应 TOML 节点操作的 C 帧入口。
 * @param frame 当前 C 调用帧。
 */
static void CHALI_ABI_CALL toml_value_new_entry(ChaliAbiFrame *frame) { toml_entry(frame, New); }

/**
 * @brief 取得 TOML C 服务的只读入口表。
 * @param count 接收入口个数，可为空。
 * @return 进程存活期间有效的只读表。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_toml_records(size_t *count)
{
    static const ChaliAbiRecord records[] = {
        { "std::toml::__at", toml_at_entry, 3 },
        { "std::toml::__bool", toml_bool_entry, 2 },
        { "std::toml::__bytes", toml_bytes_entry, 2 },
        { "std::toml::__close", toml_close_entry, 1 },
        { "std::toml::__dateTime", toml_date_time_entry, 2 },
        { "std::toml::__dump", toml_dump_entry, 2 },
        { "std::toml::__find", toml_find_entry, 3 },
        { "std::toml::__findBytes", toml_find_bytes_entry, 3 },
        { "std::toml::__float", toml_float_entry, 2 },
        { "std::toml::__int", toml_int_entry, 2 },
        { "std::toml::__key", toml_key_entry, 3 },
        { "std::toml::__keyAvailable", toml_key_available_entry, 3 },
        { "std::toml::__keyBytes", toml_key_bytes_entry, 3 },
        { "std::toml::__kind", toml_kind_entry, 2 },
        { "std::toml::__parse", toml_parse_entry, 1 },
        { "std::toml::__size", toml_size_entry, 2 },
        { "std::toml::__string", toml_string_entry, 2 },
        { "std::toml::__takeError", toml_take_error_entry, 1 },
        { "std::toml::__textAvailable", toml_text_available_entry, 2 },
        { "std::toml::__valid", toml_valid_entry, 1 },
        { "std::toml::__valueCopy", toml_value_copy_entry, 1 },
        { "std::toml::__valueDrop", toml_value_drop_entry, 1 },
        { "std::toml::__valueNew", toml_value_new_entry, 0 }
    };
    if (count) *count = sizeof(records) / sizeof(records[0]);
    return records;
}
#ifndef GLUE_NO_STD_TOML
#define CHALI_AOT_BIND_MODULE toml
#include "aot_bind_module.h"
#endif
