/**
 * @file chali_abi_registry.hpp
 * @brief C 句柄的私有实现，复用唯一的运行时函数表。
 */
#pragma once
#include "chali_abi.h"
#include "ircode/native_protocol.hpp"
#include "allocator/c_heap_allocator.hpp"
#include <memory>

/** @brief 拥有或借用同一个运行时函数表，不另建名称索引。 */
struct ChaliAbiRegistry
{
    Memory::CHeapUniquePtr<runtime::NativeRegistry> owned;  ///< create 创建的表；借用视图为空。
    runtime::NativeRegistry *backend = nullptr;      ///< 所有注册与查找共用的表。
};
