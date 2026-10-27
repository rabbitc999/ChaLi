/**
 * @file llvm_static_imports.cpp
 * @brief 将锁定 MSYS2 LLVM 预编译对象的 libxml2 导入地址绑定到静态库。
 * @details LLVM 的 WindowsManifest 对象使用 dllimport 声明。本单元提供类型一致的
 * 导入地址槽，指向实际静态 libxml2 符号；不加载 DLL，也不改写第三方二进制。
 */
#define LIBXML_STATIC
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlerror.h>
#include <libxml/xmlmemory.h>

extern "C"
{
/** @brief 静态 XML 子节点追加函数的导入地址。 */
decltype(&xmlAddChild) __imp_xmlAddChild = &xmlAddChild;
/** @brief 静态 XML 命名空间复制函数的导入地址。 */
decltype(&xmlCopyNamespace) __imp_xmlCopyNamespace = &xmlCopyNamespace;
/** @brief 静态 XML 文档编码输出函数的导入地址。 */
decltype(&xmlDocDumpFormatMemoryEnc) __imp_xmlDocDumpFormatMemoryEnc = &xmlDocDumpFormatMemoryEnc;
/** @brief 静态 XML 根节点读取函数的导入地址。 */
decltype(&xmlDocGetRootElement) __imp_xmlDocGetRootElement = &xmlDocGetRootElement;
/** @brief 静态 XML 根节点写入函数的导入地址。 */
decltype(&xmlDocSetRootElement) __imp_xmlDocSetRootElement = &xmlDocSetRootElement;
/** @brief 静态 XML 释放回调变量的地址，保留原变量的间接层数。 */
decltype(&xmlFree) __imp_xmlFree = &xmlFree;
/** @brief 静态 XML 文档释放函数的导入地址。 */
decltype(&xmlFreeDoc) __imp_xmlFreeDoc = &xmlFreeDoc;
/** @brief 静态 XML 节点释放函数的导入地址。 */
decltype(&xmlFreeNode) __imp_xmlFreeNode = &xmlFreeNode;
/** @brief 静态 XML 命名空间释放函数的导入地址。 */
decltype(&xmlFreeNs) __imp_xmlFreeNs = &xmlFreeNs;
/** @brief 静态 XML 文档创建函数的导入地址。 */
decltype(&xmlNewDoc) __imp_xmlNewDoc = &xmlNewDoc;
/** @brief 静态 XML 命名空间创建函数的导入地址。 */
decltype(&xmlNewNs) __imp_xmlNewNs = &xmlNewNs;
/** @brief 静态 XML 属性创建函数的导入地址。 */
decltype(&xmlNewProp) __imp_xmlNewProp = &xmlNewProp;
/** @brief 静态 XML 内存解析函数的导入地址。 */
decltype(&xmlReadMemory) __imp_xmlReadMemory = &xmlReadMemory;
/** @brief 静态 XML 错误回调设置函数的导入地址。 */
decltype(&xmlSetGenericErrorFunc) __imp_xmlSetGenericErrorFunc = &xmlSetGenericErrorFunc;
/** @brief 静态 XML 字符串复制函数的导入地址。 */
decltype(&xmlStrdup) __imp_xmlStrdup = &xmlStrdup;
/** @brief 静态 XML 节点摘除函数的导入地址。 */
decltype(&xmlUnlinkNode) __imp_xmlUnlinkNode = &xmlUnlinkNode;
}
