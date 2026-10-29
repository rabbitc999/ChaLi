define internal range(i64 0, 256) i64 @f_v250_program__probe(i64 %p0, i64 %p1) #24 section "chali_code" {
entry:
  %indirect_arguments.i = alloca [2 x i64], align 8
  %native_argument_types.i = alloca [2 x i8], align 1
  %indirect_arguments = alloca [4 x i64], align 8
  %native_argument_types = alloca [4 x i8], align 1
  %raw_memory_output = alloca ptr, align 8
  %raw_memory_input = alloca ptr, align 8
  %0 = tail call i64 @chali_host_allocate(i64 noundef 16) #27
  call void @chali_host_memory_prepare(i64 8, i64 %0, i64 0, i64 16, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r3 = load ptr, ptr %raw_memory_output, align 8
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 1 dereferenceable(16) %r3, i8 0, i64 16, i1 false)
  store i64 %0, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r9 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  store i64 16, ptr %r9, align 8
  %r10 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r10, align 1
  %r11 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r12 = call i8 @chali_host_result_float()
  %1 = icmp eq i64 %p0, 0
  br i1 %1, label %2, label %chali_require_reference.exit

2:                                                ; preds = %entry
  call void @chali_panic(ptr noundef nonnull @.str.29)
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
  %resolved.i = call ptr @chali_host_address_view(i64 %p0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit

chali_primitive_address.exit:                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  %r17 = load i64, ptr %common.ret.op.i, align 1
  store ptr @f_v250_program__probe, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 9, ptr @chali_source_line, align 8
  %r20 = call i64 @f_v103_std__lang__String_byteAt(i64 %r17, i64 %p1)
  %r26 = call i64 @chali_host_copy_words(i64 %0, i64 %r20, i64 2)
  store i64 %0, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 16, ptr %r9, align 8
  store i8 0, ptr %r10, align 1
  %r33 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r34 = call i8 @chali_host_result_float()
  %r39.not = icmp eq i64 %0, %r20
  br i1 %r39.not, label %ast_block_2, label %ast_block_0

ast_block_0:                                      ; preds = %chali_primitive_address.exit
  store i64 %r20, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 16, ptr %r9, align 8
  store i8 0, ptr %r10, align 1
  %r48 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 16
  store i64 285, ptr %r48, align 8
  %r49 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r49, align 1
  %r50 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r51 = call i8 @chali_host_result_float()
  br label %ast_block_2

ast_block_2:                                      ; preds = %chali_primitive_address.exit, %ast_block_0
  store ptr @f_v250_program__probe, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 9, ptr @chali_source_line, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i)
  %revision_pointer.i5.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i6.i = icmp eq ptr %revision_pointer.i5.i, null
  br i1 %has_revision.not.i6.i, label %slow.i21.i, label %check.i7.i

check.i7.i:                                       ; preds = %ast_block_2
  %revision.i8.i = load atomic i64, ptr %revision_pointer.i5.i acquire, align 8
  %stamp.i9.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i10.i = icmp eq i64 %revision.i8.i, %stamp.i9.i
  %enabled.i11.i = icmp ne i64 %revision.i8.i, 0
  %live.i12.i = and i1 %enabled.i11.i, %same_revision.i10.i
  %begin.i13.i = load i64, ptr @chali_address_view, align 16
  %size.i14.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i15.i = sub i64 %0, %begin.i13.i
  %inside.i16.i = icmp ult i64 %offset.i15.i, %size.i14.i
  %remaining.i17.i = sub i64 %size.i14.i, %offset.i15.i
  %fits.i18.i = icmp ugt i64 %remaining.i17.i, 7
  %range.i19.i = and i1 %inside.i16.i, %fits.i18.i
  %valid.i20.i = and i1 %live.i12.i, %range.i19.i
  br i1 %valid.i20.i, label %fast.i24.i, label %slow.i21.i

fast.i24.i:                                       ; preds = %check.i7.i
  %base.i25.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i26.i = getelementptr i8, ptr %base.i25.i, i64 %offset.i15.i
  br label %chali_primitive_address.exit27.i

slow.i21.i:                                       ; preds = %check.i7.i, %ast_block_2
  %resolved.i22.i = call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit27.i

chali_primitive_address.exit27.i:                 ; preds = %slow.i21.i, %fast.i24.i
  %common.ret.op.i23.i = phi ptr [ %physical.i26.i, %fast.i24.i ], [ %resolved.i22.i, %slow.i21.i ]
  %r7.i = load i64, ptr %common.ret.op.i23.i, align 1
  switch i64 %r7.i, label %f_v291_Option__uint8_unwrap.exit [
    i64 0, label %ast_block_0.i
    i64 1, label %ast_block_3.i
  ]

ast_block_0.i:                                    ; preds = %chali_primitive_address.exit27.i
  %r16.i = add i64 %0, 8
  store ptr @f_v291_Option__uint8_unwrap, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 888, ptr @chali_source_line, align 8
  %revision_pointer.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i = icmp eq ptr %revision_pointer.i.i, null
  br i1 %has_revision.not.i.i, label %slow.i.i, label %check.i.i

check.i.i:                                        ; preds = %ast_block_0.i
  %revision.i.i = load atomic i64, ptr %revision_pointer.i.i acquire, align 8
  %stamp.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i = icmp eq i64 %revision.i.i, %stamp.i.i
  %enabled.i.i = icmp ne i64 %revision.i.i, 0
  %live.i.i = and i1 %enabled.i.i, %same_revision.i.i
  %begin.i.i = load i64, ptr @chali_address_view, align 16
  %size.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i = sub i64 %r16.i, %begin.i.i
  %inside.i.i = icmp ult i64 %offset.i.i, %size.i.i
  %valid.i.i = and i1 %live.i.i, %inside.i.i
  br i1 %valid.i.i, label %fast.i.i, label %slow.i.i

fast.i.i:                                         ; preds = %check.i.i
  %base.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i = getelementptr i8, ptr %base.i.i, i64 %offset.i.i
  br label %chali_primitive_address.exit.i

slow.i.i:                                         ; preds = %check.i.i, %ast_block_0.i
  %resolved.i.i = call ptr @chali_host_address_view(i64 %r16.i, i64 1, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit.i

chali_primitive_address.exit.i:                   ; preds = %slow.i.i, %fast.i.i
  %common.ret.op.i.i = phi ptr [ %physical.i.i, %fast.i.i ], [ %resolved.i.i, %slow.i.i ]
  %r20.i = load i8, ptr %common.ret.op.i.i, align 1
  %r21.i = zext i8 %r20.i to i64
  br label %f_v291_Option__uint8_unwrap.exit

ast_block_3.i:                                    ; preds = %chali_primitive_address.exit27.i
  store ptr @f_v291_Option__uint8_unwrap, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 888, ptr @chali_source_line, align 8
  store i64 24, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  %r29.i = call i64 @chali_host_call(ptr nonnull @native_name3, i64 1, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r30.i = call i8 @chali_host_result_float()
  br label %f_v291_Option__uint8_unwrap.exit

f_v291_Option__uint8_unwrap.exit:                 ; preds = %chali_primitive_address.exit27.i, %chali_primitive_address.exit.i, %ast_block_3.i
  %s_v711___ast_291_result_1487.0.i = phi i64 [ %r21.i, %chali_primitive_address.exit.i ], [ 0, %ast_block_3.i ], [ 0, %chali_primitive_address.exit27.i ]
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i)
  %r58.not = icmp eq i64 %0, 0
  br i1 %r58.not, label %ast_block_5, label %ast_block_3

ast_block_3:                                      ; preds = %f_v291_Option__uint8_unwrap.exit
  call void @chali_host_drop(i64 noundef %0) #27
  br label %ast_block_5

ast_block_5:                                      ; preds = %f_v291_Option__uint8_unwrap.exit, %ast_block_3
  ret i64 %s_v711___ast_291_result_1487.0.i
}
