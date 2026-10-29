define internal i64 @f_v253_program__choose(i64 %p0) #23 section "chali_code" {
entry:
  %indirect_arguments = alloca [8 x i64], align 8
  %native_argument_types = alloca [8 x i8], align 1
  %r2.not = icmp eq i64 %p0, 0
  %r62 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 0, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r63 = call i8 @chali_host_result_float()
  br i1 %r2.not, label %ast_block_15, label %ast_block_5

ast_block_5:                                      ; preds = %entry
  %r14.b = load i1, ptr @s_g_1_program__second, align 1
  br i1 %r14.b, label %common.ret, label %0

0:                                                ; preds = %ast_block_5
  call void @chali_panic(ptr noundef nonnull @.str.29)
  unreachable

common.ret:                                       ; preds = %ast_block_5, %ast_block_15
  %1 = phi <2 x i64> [ <i64 120, i64 125>, %ast_block_15 ], [ <i64 136, i64 127>, %ast_block_5 ]
  %2 = phi <2 x i64> [ <i64 152, i64 168>, %ast_block_15 ], [ <i64 200, i64 216>, %ast_block_5 ]
  store i64 %r62, ptr %indirect_arguments, align 8
  store i8 %r63, ptr %native_argument_types, align 1
  %r71 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  %r72 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  store <2 x i64> %1, ptr %r71, align 8
  %r75 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 24
  store i64 0, ptr %r75, align 8
  %r77 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 32
  store i64 0, ptr %r77, align 8
  store <4 x i8> zeroinitializer, ptr %r72, align 1
  %r79 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 40
  %r80 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 5
  store i8 0, ptr %r80, align 1
  store <2 x i64> %2, ptr %r79, align 8
  %r82 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 6
  store i8 0, ptr %r82, align 1
  %r83 = call i64 @chali_host_call(ptr nonnull @native_name2, i64 7, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r84 = call i8 @chali_host_result_float()
  store i8 %r63, ptr @chali_return_float, align 1
  ret i64 %r62

ast_block_15:                                     ; preds = %entry
  %r66.b = load i1, ptr @s_g_0_program__first, align 1
  br i1 %r66.b, label %common.ret, label %3

3:                                                ; preds = %ast_block_15
  call void @chali_panic(ptr noundef nonnull @.str.29)
  unreachable
}
