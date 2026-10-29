define internal range(i64 0, 256) i64 @f_v250_program__probe(i64 %p0, i64 %p1) #24 section "chali_code" {
entry:
  %indirect_arguments.i = alloca [3 x i64], align 8
  %native_argument_types.i = alloca [3 x i8], align 1
  %0 = icmp eq i64 %p0, 0
  br i1 %0, label %1, label %chali_require_reference.exit

1:                                                ; preds = %entry
  tail call void @chali_panic(ptr noundef nonnull @.str.29)
  unreachable

chali_require_reference.exit:                     ; preds = %entry
  %revision_pointer.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i = icmp eq ptr %revision_pointer.i, null
  br i1 %has_revision.not.i, label %slow.i, label %check.i

check.i:                                          ; preds = %chali_require_reference.exit
  %revision.i = load atomic i64, ptr %revision_pointer.i acquire, align 8
  %stamp.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i = icmp eq i64 %revision.i, %stamp.i
  %enabled.i = icmp ne i64 %revision.i, 0
  %live.i = and i1 %enabled.i, %same_revision.i
  %begin.i = load i64, ptr @chali_address_view, align 16
  %size.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i = sub i64 %p0, %begin.i
  %inside.i = icmp ult i64 %offset.i, %size.i
  %remaining.i = sub i64 %size.i, %offset.i
  %fits.i = icmp ugt i64 %remaining.i, 7
  %range.i = and i1 %inside.i, %fits.i
  %valid.i = and i1 %live.i, %range.i
  br i1 %valid.i, label %fast.i, label %slow.i

fast.i:                                           ; preds = %check.i
  %base.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i = getelementptr i8, ptr %base.i, i64 %offset.i
  br label %chali_primitive_address.exit

slow.i:                                           ; preds = %check.i, %chali_require_reference.exit
  %resolved.i = tail call ptr @chali_host_address_view(i64 %p0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit

chali_primitive_address.exit:                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  %r4 = load i64, ptr %common.ret.op.i, align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i)
  %r2.i = icmp slt i64 %p1, 0
  br i1 %r2.i, label %ast_block_4.i, label %ast_block_1.i

ast_block_1.i:                                    ; preds = %chali_primitive_address.exit
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 164, ptr @chali_source_line, align 8
  %r2.i.i = tail call i64 @chali_host_string_storage(i64 0, i64 %r4)
  %r11.i.not = icmp slt i64 %p1, %r2.i.i
  br i1 %r11.i.not, label %ast_block_6.i, label %ast_block_4.i

ast_block_4.i:                                    ; preds = %ast_block_1.i, %chali_primitive_address.exit
  store ptr @f_v57_std__lang____bootStringByteAtUnwrap, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 169, ptr @chali_source_line, align 8
  store i64 24, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  %r19.i = call i64 @chali_host_call(ptr nonnull @native_name3, i64 1, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r20.i = call i8 @chali_host_result_float()
  br label %ast_block_6.i

ast_block_6.i:                                    ; preds = %ast_block_4.i, %ast_block_1.i
  store ptr @f_v57_std__lang____bootStringByteAtUnwrap, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 171, ptr @chali_source_line, align 8
  %r23.i = call i64 @chali_host_string_storage(i64 1, i64 %r4)
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i.i = add i64 %r23.i, %p1
  %2 = icmp eq i64 %r4.i.i, 0
  br i1 %2, label %3, label %chali_require_reference.exit2

3:                                                ; preds = %ast_block_6.i
  call void @chali_panic(ptr noundef nonnull @.str.29)
  unreachable

chali_require_reference.exit2:                    ; preds = %ast_block_6.i
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i.i = icmp eq ptr %revision_pointer.i.i.i, null
  br i1 %has_revision.not.i.i.i, label %slow.i.i.i, label %check.i.i.i

check.i.i.i:                                      ; preds = %chali_require_reference.exit2
  %revision.i.i.i = load atomic i64, ptr %revision_pointer.i.i.i acquire, align 8
  %stamp.i.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i.i = icmp eq i64 %revision.i.i.i, %stamp.i.i.i
  %enabled.i.i.i = icmp ne i64 %revision.i.i.i, 0
  %live.i.i.i = and i1 %enabled.i.i.i, %same_revision.i.i.i
  %begin.i.i.i = load i64, ptr @chali_address_view, align 16
  %size.i.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i.i = sub i64 %r4.i.i, %begin.i.i.i
  %inside.i.i.i = icmp ult i64 %offset.i.i.i, %size.i.i.i
  %valid.i.i.i = and i1 %live.i.i.i, %inside.i.i.i
  br i1 %valid.i.i.i, label %fast.i.i.i, label %slow.i.i.i

fast.i.i.i:                                       ; preds = %check.i.i.i
  %base.i.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i.i = getelementptr i8, ptr %base.i.i.i, i64 %offset.i.i.i
  br label %f_v57_std__lang____bootStringByteAtUnwrap.exit

slow.i.i.i:                                       ; preds = %check.i.i.i, %chali_require_reference.exit2
  %resolved.i.i.i = call ptr @chali_host_address_view(i64 %r4.i.i, i64 1, ptr nonnull @chali_address_view)
  br label %f_v57_std__lang____bootStringByteAtUnwrap.exit

f_v57_std__lang____bootStringByteAtUnwrap.exit:   ; preds = %fast.i.i.i, %slow.i.i.i
  %common.ret.op.i.i.i = phi ptr [ %physical.i.i.i, %fast.i.i.i ], [ %resolved.i.i.i, %slow.i.i.i ]
  %r10.i.i = load i8, ptr %common.ret.op.i.i.i, align 1
  %r11.i.i = zext i8 %r10.i.i to i64
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i)
  ret i64 %r11.i.i
}
