define internal i64 @f_v252_program__callOperation(i64 %p0, i64 %p1) #23 section "chali_code" {
entry:
  %indirect_arguments = alloca [4 x i64], align 8
  %native_argument_types = alloca [4 x i8], align 1
  store i64 %p0, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r4 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 1, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r5 = call i8 @chali_host_result_float()
  %0 = icmp eq i64 %r4, 0
  br i1 %0, label %1, label %ast_block_2

1:                                                ; preds = %entry
  call void @chali_panic(ptr noundef nonnull @.str.29)
  unreachable

ast_block_2:                                      ; preds = %entry
  store ptr @f_v252_program__callOperation, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 15, ptr @chali_source_line, align 8
  %r11 = call i64 @f_v488___chali_iface_NamedOperation_apply(i64 %r4, i64 poison, i64 %p1)
  %r12 = load i8, ptr @chali_return_float, align 1
  call void @chali_host_drop(i64 noundef %r4) #24
  store i8 %r12, ptr @chali_return_float, align 1
  ret i64 %r11
}
