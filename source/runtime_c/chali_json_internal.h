/**
 * @file chali_json_internal.h
 * @brief JSON/TOML C 内核共享的独占文档，仅供服务内部访问。
 */
#ifndef CHALI_JSON_INTERNAL_H
#define CHALI_JSON_INTERNAL_H
#include "chali_json.h"
#include "yyjson.h"
/** @brief C 文档及其根节点与有界诊断，不向语言层暴露实现。 */
struct ChaliJson
{
    yyjson_mut_doc *document;
    yyjson_mut_val *root;
    char error[256];
};
#endif
