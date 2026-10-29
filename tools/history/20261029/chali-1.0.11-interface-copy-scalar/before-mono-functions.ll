define internal i64 @f_v250_program__invoke(i64 %p0, i64 %p1) uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [4 x i64]
  %native_argument_types = alloca [4 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v489___ast_250___chali_ret_v_1330 = alloca i64
  store i64 0, ptr %s_v489___ast_250___chali_ret_v_1330
  %t_v489___ast_250___chali_ret_v_1330 = alloca i8
  store i8 0, ptr %t_v489___ast_250___chali_ret_v_1330
  %s_v490___ast_250_operation_608 = alloca i64
  store i64 0, ptr %s_v490___ast_250_operation_608
  %t_v490___ast_250_operation_608 = alloca i8
  store i8 0, ptr %t_v490___ast_250_operation_608
  %s_v491___ast_250_value_609 = alloca i64
  store i64 0, ptr %s_v491___ast_250_value_609
  %t_v491___ast_250_value_609 = alloca i8
  store i8 0, ptr %t_v491___ast_250_value_609
  store i64 %p0, ptr %s_v490___ast_250_operation_608
  store i64 %p1, ptr %s_v491___ast_250_value_609
  %r0 = load i64, ptr %s_v490___ast_250_operation_608
  %r1 = load i8, ptr %t_v490___ast_250_operation_608
  %r2 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r0, ptr %r2
  %r3 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r1, ptr %r3
  %r4 = call i64 @chali_host_call(ptr @native_name0, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r5 = call i8 @chali_host_result_float()
  store i64 %r4, ptr %s_v490___ast_250_operation_608
  store i8 %r5, ptr %t_v490___ast_250_operation_608
  %r6 = load i64, ptr %s_v490___ast_250_operation_608
  %r7 = load i8, ptr %t_v490___ast_250_operation_608
  %r8 = call i64 @chali_require_reference(i64 %r6)
  %r9 = load i64, ptr %s_v491___ast_250_value_609
  %r10 = load i8, ptr %t_v491___ast_250_value_609
  store ptr @f_v250_program__invoke, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 12, ptr @chali_source_line
  %r11 = call i64 @f_v479___chali_iface_NamedOperation_apply(i64 %r8, i64 1, i64 %r9)
  %r12 = load i8, ptr @chali_return_float
  store i64 %r11, ptr %s_v489___ast_250___chali_ret_v_1330
  store i8 %r12, ptr %t_v489___ast_250___chali_ret_v_1330
  %r13 = load i64, ptr %s_v490___ast_250_operation_608
  %r14 = load i8, ptr %t_v490___ast_250_operation_608
  %r15 = icmp ne i64 %r13, 0
  %r16 = zext i1 %r15 to i64
  %r17 = icmp ne i64 %r16, 0
  br i1 %r17, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r18 = load i64, ptr %s_v490___ast_250_operation_608
  %r19 = load i8, ptr %t_v490___ast_250_operation_608
  call void @chali_owned_drop(i64 %r18)
  br label %ast_block_2
ast_block_1:
  br label %ast_block_2
ast_block_2:
  store i64 0, ptr %s_v490___ast_250_operation_608
  store i8 0, ptr %t_v490___ast_250_operation_608
  %r20 = load i64, ptr %s_v489___ast_250___chali_ret_v_1330
  %r21 = load i8, ptr %t_v489___ast_250___chali_ret_v_1330
  store i8 %r21, ptr @chali_return_float
  ret i64 %r20
ast_block_3:
  %r22 = load i64, ptr %s_v490___ast_250_operation_608
  %r23 = load i8, ptr %t_v490___ast_250_operation_608
  %r24 = icmp ne i64 %r22, 0
  %r25 = zext i1 %r24 to i64
  %r26 = icmp ne i64 %r25, 0
  br i1 %r26, label %ast_block_4, label %ast_block_5
ast_block_4:
  %r27 = load i64, ptr %s_v490___ast_250_operation_608
  %r28 = load i8, ptr %t_v490___ast_250_operation_608
  call void @chali_owned_drop(i64 %r27)
  br label %ast_block_6
ast_block_5:
  br label %ast_block_6
ast_block_6:
  store i64 0, ptr %s_v490___ast_250_operation_608
  store i8 0, ptr %t_v490___ast_250_operation_608
  store i8 0, ptr @chali_return_float
  ret i64 0
ast_label_v492___chali_cleanup_program__invoke:
  %r29 = load i64, ptr %s_v490___ast_250_operation_608
  %r30 = load i8, ptr %t_v490___ast_250_operation_608
  %r31 = icmp ne i64 %r29, 0
  %r32 = zext i1 %r31 to i64
  %r33 = icmp ne i64 %r32, 0
  br i1 %r33, label %ast_block_7, label %ast_block_8
ast_block_7:
  %r34 = load i64, ptr %s_v490___ast_250_operation_608
  %r35 = load i8, ptr %t_v490___ast_250_operation_608
  call void @chali_owned_drop(i64 %r34)
  br label %ast_block_9
ast_block_8:
  br label %ast_block_9
ast_block_9:
  store i64 0, ptr %s_v490___ast_250_operation_608
  store i8 0, ptr %t_v490___ast_250_operation_608
  store i8 0, ptr @chali_return_float
  ret i64 0
}

define i64 @__chali_impl_program__work(i64 %p0) uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [8 x i64]
  %native_argument_types = alloca [8 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v493___ast_251___chali_box_value = alloca i64
  store i64 0, ptr %s_v493___ast_251___chali_box_value
  %t_v493___ast_251___chali_box_value = alloca i8
  store i8 0, ptr %t_v493___ast_251___chali_box_value
  %s_v494___ast_251___chali_ret_v_1333 = alloca i64
  store i64 0, ptr %s_v494___ast_251___chali_ret_v_1333
  %t_v494___ast_251___chali_ret_v_1333 = alloca i8
  store i8 0, ptr %t_v494___ast_251___chali_ret_v_1333
  %s_v495___ast_251___chali_tmp_o_1331 = alloca i64
  store i64 0, ptr %s_v495___ast_251___chali_tmp_o_1331
  %t_v495___ast_251___chali_tmp_o_1331 = alloca i8
  store i8 0, ptr %t_v495___ast_251___chali_tmp_o_1331
  %s_v496___ast_251_acc = alloca i64
  store i64 0, ptr %s_v496___ast_251_acc
  %t_v496___ast_251_acc = alloca i8
  store i8 0, ptr %t_v496___ast_251_acc
  %s_v497___ast_251_derived = alloca i64
  store i64 0, ptr %s_v497___ast_251_derived
  %t_v497___ast_251_derived = alloca i8
  store i8 0, ptr %t_v497___ast_251_derived
  %s_v498___ast_251_i_1332 = alloca i64
  store i64 0, ptr %s_v498___ast_251_i_1332
  %t_v498___ast_251_i_1332 = alloca i8
  store i8 0, ptr %t_v498___ast_251_i_1332
  %s_v499___ast_251_seed = alloca i64
  store i64 0, ptr %s_v499___ast_251_seed
  %t_v499___ast_251_seed = alloca i8
  store i8 0, ptr %t_v499___ast_251_seed
  store i64 %p0, ptr %s_v499___ast_251_seed
  %r0 = load i64, ptr %s_v495___ast_251___chali_tmp_o_1331
  %r1 = load i8, ptr %t_v495___ast_251___chali_tmp_o_1331
  %r2 = icmp ne i64 %r0, 0
  %r3 = zext i1 %r2 to i64
  %r4 = icmp ne i64 %r3, 0
  br i1 %r4, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r5 = load i64, ptr %s_v495___ast_251___chali_tmp_o_1331
  %r6 = load i8, ptr %t_v495___ast_251___chali_tmp_o_1331
  call void @chali_owned_drop(i64 %r5)
  br label %ast_block_2
ast_block_1:
  br label %ast_block_2
ast_block_2:
  store i64 0, ptr %s_v495___ast_251___chali_tmp_o_1331
  store i8 0, ptr %t_v495___ast_251___chali_tmp_o_1331
  %r7 = call i64 @chali_host_call(ptr @native_name1, i64 0, ptr %indirect_arguments, ptr %native_argument_types)
  %r8 = call i8 @chali_host_result_float()
  store i64 %r7, ptr %s_v493___ast_251___chali_box_value
  store i8 %r8, ptr %t_v493___ast_251___chali_box_value
  %r9 = load i64, ptr %s_v493___ast_251___chali_box_value
  %r10 = load i8, ptr %t_v493___ast_251___chali_box_value
  %r11 = load i64, ptr @s_g_0_program__receiver
  %r12 = load i8, ptr @t_g_0_program__receiver
  %r13 = call i64 @chali_require_reference(i64 %r11)
  %r14 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r9, ptr %r14
  %r15 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r10, ptr %r15
  %r16 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 %r13, ptr %r16
  %r17 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r17
  %r18 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 122, ptr %r18
  %r19 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r19
  %r20 = getelementptr i64, ptr %indirect_arguments, i64 3
  store i64 0, ptr %r20
  %r21 = getelementptr i8, ptr %native_argument_types, i64 3
  store i8 0, ptr %r21
  %r22 = getelementptr i64, ptr %indirect_arguments, i64 4
  store i64 0, ptr %r22
  %r23 = getelementptr i8, ptr %native_argument_types, i64 4
  store i8 0, ptr %r23
  %r24 = getelementptr i64, ptr %indirect_arguments, i64 5
  store i64 48, ptr %r24
  %r25 = getelementptr i8, ptr %native_argument_types, i64 5
  store i8 0, ptr %r25
  %r26 = getelementptr i64, ptr %indirect_arguments, i64 6
  store i64 64, ptr %r26
  %r27 = getelementptr i8, ptr %native_argument_types, i64 6
  store i8 0, ptr %r27
  %r28 = call i64 @chali_host_call(ptr @native_name2, i64 7, ptr %indirect_arguments, ptr %native_argument_types)
  %r29 = call i8 @chali_host_result_float()
  %r30 = load i64, ptr %s_v493___ast_251___chali_box_value
  %r31 = load i8, ptr %t_v493___ast_251___chali_box_value
  store i64 %r30, ptr %s_v495___ast_251___chali_tmp_o_1331
  store i8 %r31, ptr %t_v495___ast_251___chali_tmp_o_1331
  %r32 = load i64, ptr %s_v495___ast_251___chali_tmp_o_1331
  %r33 = load i8, ptr %t_v495___ast_251___chali_tmp_o_1331
  %r34 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r32, ptr %r34
  %r35 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r33, ptr %r35
  %r36 = call i64 @chali_host_call(ptr @native_name0, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r37 = call i8 @chali_host_result_float()
  store i64 %r36, ptr %s_v497___ast_251_derived
  store i8 %r37, ptr %t_v497___ast_251_derived
  %r38 = load i64, ptr %s_v499___ast_251_seed
  %r39 = load i8, ptr %t_v499___ast_251_seed
  store i64 %r38, ptr %s_v496___ast_251_acc
  store i8 %r39, ptr %t_v496___ast_251_acc
  store i64 0, ptr %s_v498___ast_251_i_1332
  store i8 0, ptr %t_v498___ast_251_i_1332
  br label %ast_label_v500___chali_loop_135
ast_label_v500___chali_loop_135:
  %r40 = load i64, ptr %s_v498___ast_251_i_1332
  %r41 = load i8, ptr %t_v498___ast_251_i_1332
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 17, ptr @chali_source_line
  %r42 = icmp slt i64 %r40, 262144
  %r43 = zext i1 %r42 to i64
  %r44 = icmp ne i64 %r43, 0
  br i1 %r44, label %ast_block_3, label %ast_block_4
ast_block_3:
  %r45 = load i64, ptr %s_v497___ast_251_derived
  %r46 = load i8, ptr %t_v497___ast_251_derived
  %r47 = call i64 @chali_require_reference(i64 %r45)
  %r48 = call i64 @chali_require_reference(i64 %r47)
  %r49 = load i64, ptr %s_v496___ast_251_acc
  %r50 = load i8, ptr %t_v496___ast_251_acc
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 17, ptr @chali_source_line
  %r51 = call i64 @f_v250_program__invoke(i64 %r48, i64 %r49)
  %r52 = load i8, ptr @chali_return_float
  store i64 %r51, ptr %s_v496___ast_251_acc
  store i8 %r52, ptr %t_v496___ast_251_acc
  %r53 = load i64, ptr %s_v498___ast_251_i_1332
  %r54 = load i8, ptr %t_v498___ast_251_i_1332
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 17, ptr @chali_source_line
  %r55 = add i64 %r53, 1
  store i64 %r55, ptr %s_v498___ast_251_i_1332
  store i8 0, ptr %t_v498___ast_251_i_1332
  br label %ast_block_5
ast_block_4:
  br label %ast_label_v501___chali_brk_136
ast_block_5:
  br label %ast_label_v500___chali_loop_135
ast_label_v501___chali_brk_136:
  %r56 = load i64, ptr %s_v496___ast_251_acc
  %r57 = load i8, ptr %t_v496___ast_251_acc
  store i64 %r56, ptr %s_v494___ast_251___chali_ret_v_1333
  store i8 %r57, ptr %t_v494___ast_251___chali_ret_v_1333
  %r58 = load i64, ptr %s_v497___ast_251_derived
  %r59 = load i8, ptr %t_v497___ast_251_derived
  %r60 = icmp ne i64 %r58, 0
  %r61 = zext i1 %r60 to i64
  %r62 = icmp ne i64 %r61, 0
  br i1 %r62, label %ast_block_6, label %ast_block_7
ast_block_6:
  %r63 = load i64, ptr %s_v497___ast_251_derived
  %r64 = load i8, ptr %t_v497___ast_251_derived
  call void @chali_owned_drop(i64 %r63)
  br label %ast_block_8
ast_block_7:
  br label %ast_block_8
ast_block_8:
  store i64 0, ptr %s_v497___ast_251_derived
  store i8 0, ptr %t_v497___ast_251_derived
  %r65 = load i64, ptr %s_v495___ast_251___chali_tmp_o_1331
  %r66 = load i8, ptr %t_v495___ast_251___chali_tmp_o_1331
  %r67 = icmp ne i64 %r65, 0
  %r68 = zext i1 %r67 to i64
  %r69 = icmp ne i64 %r68, 0
  br i1 %r69, label %ast_block_9, label %ast_block_10
ast_block_9:
  %r70 = load i64, ptr %s_v495___ast_251___chali_tmp_o_1331
  %r71 = load i8, ptr %t_v495___ast_251___chali_tmp_o_1331
  call void @chali_owned_drop(i64 %r70)
  br label %ast_block_11
ast_block_10:
  br label %ast_block_11
ast_block_11:
  store i64 0, ptr %s_v495___ast_251___chali_tmp_o_1331
  store i8 0, ptr %t_v495___ast_251___chali_tmp_o_1331
  %r72 = load i64, ptr %s_v494___ast_251___chali_ret_v_1333
  %r73 = load i8, ptr %t_v494___ast_251___chali_ret_v_1333
  store i8 %r73, ptr @chali_return_float
  ret i64 %r72
ast_block_12:
  %r74 = load i64, ptr %s_v497___ast_251_derived
  %r75 = load i8, ptr %t_v497___ast_251_derived
  %r76 = icmp ne i64 %r74, 0
  %r77 = zext i1 %r76 to i64
  %r78 = icmp ne i64 %r77, 0
  br i1 %r78, label %ast_block_13, label %ast_block_14
ast_block_13:
  %r79 = load i64, ptr %s_v497___ast_251_derived
  %r80 = load i8, ptr %t_v497___ast_251_derived
  call void @chali_owned_drop(i64 %r79)
  br label %ast_block_15
ast_block_14:
  br label %ast_block_15
ast_block_15:
  store i64 0, ptr %s_v497___ast_251_derived
  store i8 0, ptr %t_v497___ast_251_derived
  %r81 = load i64, ptr %s_v495___ast_251___chali_tmp_o_1331
  %r82 = load i8, ptr %t_v495___ast_251___chali_tmp_o_1331
  %r83 = icmp ne i64 %r81, 0
  %r84 = zext i1 %r83 to i64
  %r85 = icmp ne i64 %r84, 0
  br i1 %r85, label %ast_block_16, label %ast_block_17
ast_block_16:
  %r86 = load i64, ptr %s_v495___ast_251___chali_tmp_o_1331
  %r87 = load i8, ptr %t_v495___ast_251___chali_tmp_o_1331
  call void @chali_owned_drop(i64 %r86)
  br label %ast_block_18
ast_block_17:
  br label %ast_block_18
ast_block_18:
  store i64 0, ptr %s_v495___ast_251___chali_tmp_o_1331
  store i8 0, ptr %t_v495___ast_251___chali_tmp_o_1331
  store i8 0, ptr @chali_return_float
  ret i64 0
ast_label_v502___chali_cleanup_program__work:
  %r88 = load i64, ptr %s_v497___ast_251_derived
  %r89 = load i8, ptr %t_v497___ast_251_derived
  %r90 = icmp ne i64 %r88, 0
  %r91 = zext i1 %r90 to i64
  %r92 = icmp ne i64 %r91, 0
  br i1 %r92, label %ast_block_19, label %ast_block_20
ast_block_19:
  %r93 = load i64, ptr %s_v497___ast_251_derived
  %r94 = load i8, ptr %t_v497___ast_251_derived
  call void @chali_owned_drop(i64 %r93)
  br label %ast_block_21
ast_block_20:
  br label %ast_block_21
ast_block_21:
  store i64 0, ptr %s_v497___ast_251_derived
  store i8 0, ptr %t_v497___ast_251_derived
  %r95 = load i64, ptr %s_v495___ast_251___chali_tmp_o_1331
  %r96 = load i8, ptr %t_v495___ast_251___chali_tmp_o_1331
  %r97 = icmp ne i64 %r95, 0
  %r98 = zext i1 %r97 to i64
  %r99 = icmp ne i64 %r98, 0
  br i1 %r99, label %ast_block_22, label %ast_block_23
ast_block_22:
  %r100 = load i64, ptr %s_v495___ast_251___chali_tmp_o_1331
  %r101 = load i8, ptr %t_v495___ast_251___chali_tmp_o_1331
  call void @chali_owned_drop(i64 %r100)
  br label %ast_block_24
ast_block_23:
  br label %ast_block_24
ast_block_24:
  store i64 0, ptr %s_v495___ast_251___chali_tmp_o_1331
  store i8 0, ptr %t_v495___ast_251___chali_tmp_o_1331
  store i8 0, ptr @chali_return_float
  ret i64 0
}
