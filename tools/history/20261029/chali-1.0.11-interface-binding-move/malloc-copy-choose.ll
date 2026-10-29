define internal i64 @f_v253_program__choose(i64 %p0) #23 section "chali_code" {
entry:
  %indirect_arguments = alloca [8 x i64], align 8
  %native_argument_types = alloca [8 x i8], align 1
  %r2.not = icmp eq i64 %p0, 0
  %r63 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 0, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r64 = call i8 @chali_host_result_float()
  br i1 %r2.not, label %ast_block_15, label %ast_block_5

ast_block_5:                                      ; preds = %entry
  %r14.b = load i1, ptr @s_g_1_program__second, align 1
  br i1 %r14.b, label %chali_require_reference.exit, label %0

0:                                                ; preds = %ast_block_5
  call void @chali_panic(ptr noundef nonnull @.str.29)
  unreachable

chali_require_reference.exit:                     ; preds = %ast_block_5
  store i64 %r63, ptr %indirect_arguments, align 8
  store i8 %r64, ptr %native_argument_types, align 1
  %r19 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  store i64 136, ptr %r19, align 8
  %r20 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  %r21 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 16
  store i64 127, ptr %r21, align 8
  %r23 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 24
  store i64 0, ptr %r23, align 8
  %r25 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 32
  store i64 0, ptr %r25, align 8
  store <4 x i8> zeroinitializer, ptr %r20, align 1
  %r27 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 40
  store i64 200, ptr %r27, align 8
  %r28 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 5
  store i8 0, ptr %r28, align 1
  %r29 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 48
  store i64 216, ptr %r29, align 8
  %r30 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 6
  store i8 0, ptr %r30, align 1
  %r31 = call i64 @chali_host_call(ptr nonnull @native_name2, i64 7, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r32 = call i8 @chali_host_result_float()
  %r37.not = icmp eq i8 %r64, 0
  %r38 = select i1 %r37.not, i64 2, i64 3
  %r39 = call i64 @chali_host_object_scalar(i64 %r38, i64 %r63)
  %r42.not = icmp eq i64 %r63, 0
  br i1 %r42.not, label %common.ret, label %common.ret.sink.split

common.ret.sink.split:                            ; preds = %chali_require_reference.exit, %chali_require_reference.exit21
  %common.ret.op.ph = phi i64 [ %r92, %chali_require_reference.exit21 ], [ %r39, %chali_require_reference.exit ]
  call void @chali_host_drop(i64 noundef %r63) #24
  br label %common.ret

common.ret:                                       ; preds = %common.ret.sink.split, %chali_require_reference.exit21, %chali_require_reference.exit
  %common.ret.op = phi i64 [ %r92, %chali_require_reference.exit21 ], [ %r39, %chali_require_reference.exit ], [ %common.ret.op.ph, %common.ret.sink.split ]
  store i8 0, ptr @chali_return_float, align 1
  ret i64 %common.ret.op

ast_block_15:                                     ; preds = %entry
  %r67.b = load i1, ptr @s_g_0_program__first, align 1
  br i1 %r67.b, label %chali_require_reference.exit21, label %1

1:                                                ; preds = %ast_block_15
  call void @chali_panic(ptr noundef nonnull @.str.29)
  unreachable

chali_require_reference.exit21:                   ; preds = %ast_block_15
  store i64 %r63, ptr %indirect_arguments, align 8
  store i8 %r64, ptr %native_argument_types, align 1
  %r72 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  store i64 120, ptr %r72, align 8
  %r73 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  %r74 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 16
  store i64 125, ptr %r74, align 8
  %r76 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 24
  store i64 0, ptr %r76, align 8
  %r78 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 32
  store i64 0, ptr %r78, align 8
  store <4 x i8> zeroinitializer, ptr %r73, align 1
  %r80 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 40
  store i64 152, ptr %r80, align 8
  %r81 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 5
  store i8 0, ptr %r81, align 1
  %r82 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 48
  store i64 168, ptr %r82, align 8
  %r83 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 6
  store i8 0, ptr %r83, align 1
  %r84 = call i64 @chali_host_call(ptr nonnull @native_name2, i64 7, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r85 = call i8 @chali_host_result_float()
  %r90.not = icmp eq i8 %r64, 0
  %r91 = select i1 %r90.not, i64 2, i64 3
  %r92 = call i64 @chali_host_object_scalar(i64 %r91, i64 %r63)
  %r95.not = icmp eq i64 %r63, 0
  br i1 %r95.not, label %common.ret, label %common.ret.sink.split
}
