/**
 * @file chali_aot_numeric_list.hpp
 * @brief 生成以线程代次保护的数值 List LLVM 操作原语。
 */

/**
 * @brief 直接读写已验证的数值缓冲区；扩容、未知布局和失效视图回退宿主。
 * @return LLVM 数值视图缓存及内联操作函数。
 */
Text astNumericListPrimitive()
{
    return R"LLVM(
@chali_list_view = internal thread_local global { i64, ptr, ptr, i64 } zeroinitializer
declare void @chali_host_list_scalar_view(i64, ptr nocapture writeonly)

define internal i64 @chali_list_storage_fast(i64 %op, i64 %handle, i64 %second, i64 %third, i64 %fourth, i64 %fifth, i64 %floating, ptr %kind) alwaysinline {
entry:
  %append = icmp eq i64 %op, 18
  %read = icmp eq i64 %op, 11
  %supported_op = or i1 %append, %read
  %allowed = select i1 %append, i64 2, i64 0
  %disallowed = xor i64 %allowed, -1
  %other_floats = and i64 %floating, %disallowed
  %valid_kinds = icmp eq i64 %other_floats, 0
  %eligible = and i1 %supported_op, %valid_kinds
  %cached_handle = load i64, ptr @chali_list_view
  %same_handle = icmp eq i64 %cached_handle, %handle
  %revision_pointer = load ptr, ptr getelementptr ({ i64, ptr, ptr, i64 }, ptr @chali_list_view, i32 0, i32 2)
  %has_revision = icmp ne ptr %revision_pointer, null
  %matched = and i1 %eligible, %same_handle
  %candidate = and i1 %matched, %has_revision
  br i1 %candidate, label %check, label %slow
check:
  %revision = load i64, ptr %revision_pointer
  %stamp = load i64, ptr getelementptr ({ i64, ptr, ptr, i64 }, ptr @chali_list_view, i32 0, i32 3)
  %same_revision = icmp eq i64 %revision, %stamp
  %enabled = icmp ne i64 %revision, 0
  %live = and i1 %same_revision, %enabled
  br i1 %live, label %buffer, label %slow
buffer:
  %header = load ptr, ptr getelementptr ({ i64, ptr, ptr, i64 }, ptr @chali_list_view, i32 0, i32 1)
  %data = load ptr, ptr %header
  %length_slot = getelementptr { ptr, i64, i64 }, ptr %header, i32 0, i32 1
  %length = load i64, ptr %length_slot
  br i1 %append, label %push, label %at
push:
  %capacity_slot = getelementptr { ptr, i64, i64 }, ptr %header, i32 0, i32 2
  %capacity = load i64, ptr %capacity_slot
  %room = icmp ult i64 %length, %capacity
  br i1 %room, label %write, label %slow
write:
  %destination = getelementptr { i64, i8 }, ptr %data, i64 %length
  store i64 %second, ptr %destination
  %floating_bit = and i64 %floating, 2
  %is_float = icmp ne i64 %floating_bit, 0
  %float_kind = zext i1 %is_float to i8
  %destination_kind = getelementptr { i64, i8 }, ptr %destination, i32 0, i32 1
  store i8 %float_kind, ptr %destination_kind
  %next = add i64 %length, 1
  store i64 %next, ptr %length_slot
  store i8 0, ptr %kind
  ret i64 0
at:
  %inside = icmp ult i64 %second, %length
  br i1 %inside, label %load, label %slow
load:
  %source = getelementptr { i64, i8 }, ptr %data, i64 %second
  %bits = load i64, ptr %source
  %source_kind = getelementptr { i64, i8 }, ptr %source, i32 0, i32 1
  %stored_kind = load i8, ptr %source_kind
  store i8 %stored_kind, ptr %kind
  ret i64 %bits
slow:
  %result = call i64 @chali_host_list_storage_scalar(i64 %op, i64 %handle, i64 %second, i64 %third, i64 %fourth, i64 %fifth, i64 %floating, ptr %kind)
  call void @chali_host_list_scalar_view(i64 %handle, ptr @chali_list_view)
  ret i64 %result
}
)LLVM";
}
