define internal i64 @f_v256_program__choose(i64 %p0) uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [8 x i64]
  %native_argument_types = alloca [8 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v502___ast_256___chali_box_value = alloca i64
  store i64 0, ptr %s_v502___ast_256___chali_box_value
  %t_v502___ast_256___chali_box_value = alloca i8
  store i8 0, ptr %t_v502___ast_256___chali_box_value
  %s_v503___ast_256___chali_box_value_1343 = alloca i64
  store i64 0, ptr %s_v503___ast_256___chali_box_value_1343
  %t_v503___ast_256___chali_box_value_1343 = alloca i8
  store i8 0, ptr %t_v503___ast_256___chali_box_value_1343
  %s_v504___ast_256___chali_box_value_1346 = alloca i64
  store i64 0, ptr %s_v504___ast_256___chali_box_value_1346
  %t_v504___ast_256___chali_box_value_1346 = alloca i8
  store i8 0, ptr %t_v504___ast_256___chali_box_value_1346
  %s_v505___ast_256___chali_ret_v_1342 = alloca i64
  store i64 0, ptr %s_v505___ast_256___chali_ret_v_1342
  %t_v505___ast_256___chali_ret_v_1342 = alloca i8
  store i8 0, ptr %t_v505___ast_256___chali_ret_v_1342
  %s_v506___ast_256___chali_ret_v_1345 = alloca i64
  store i64 0, ptr %s_v506___ast_256___chali_ret_v_1345
  %t_v506___ast_256___chali_ret_v_1345 = alloca i8
  store i8 0, ptr %t_v506___ast_256___chali_ret_v_1345
  %s_v507___ast_256___chali_ret_v_1348 = alloca i64
  store i64 0, ptr %s_v507___ast_256___chali_ret_v_1348
  %t_v507___ast_256___chali_ret_v_1348 = alloca i8
  store i8 0, ptr %t_v507___ast_256___chali_ret_v_1348
  %s_v508___ast_256___chali_tmp_o_1341 = alloca i64
  store i64 0, ptr %s_v508___ast_256___chali_tmp_o_1341
  %t_v508___ast_256___chali_tmp_o_1341 = alloca i8
  store i8 0, ptr %t_v508___ast_256___chali_tmp_o_1341
  %s_v509___ast_256___chali_tmp_o_1344 = alloca i64
  store i64 0, ptr %s_v509___ast_256___chali_tmp_o_1344
  %t_v509___ast_256___chali_tmp_o_1344 = alloca i8
  store i8 0, ptr %t_v509___ast_256___chali_tmp_o_1344
  %s_v510___ast_256___chali_tmp_o_1347 = alloca i64
  store i64 0, ptr %s_v510___ast_256___chali_tmp_o_1347
  %t_v510___ast_256___chali_tmp_o_1347 = alloca i8
  store i8 0, ptr %t_v510___ast_256___chali_tmp_o_1347
  %s_v511___ast_256_index_610 = alloca i64
  store i64 0, ptr %s_v511___ast_256_index_610
  %t_v511___ast_256_index_610 = alloca i8
  store i8 0, ptr %t_v511___ast_256_index_610
  store i64 %p0, ptr %s_v511___ast_256_index_610
  %r0 = load i64, ptr %s_v511___ast_256_index_610
  %r1 = load i8, ptr %t_v511___ast_256_index_610
  store ptr @f_v256_program__choose, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 27, ptr @chali_source_line
  %r2 = icmp eq i64 %r0, 1
  %r3 = zext i1 %r2 to i64
  %r4 = icmp ne i64 %r3, 0
  br i1 %r4, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r5 = load i64, ptr %s_v508___ast_256___chali_tmp_o_1341
  %r6 = load i8, ptr %t_v508___ast_256___chali_tmp_o_1341
  %r7 = icmp ne i64 %r5, 0
  %r8 = zext i1 %r7 to i64
  %r9 = icmp ne i64 %r8, 0
  br i1 %r9, label %ast_block_3, label %ast_block_4
ast_block_3:
  %r10 = load i64, ptr %s_v508___ast_256___chali_tmp_o_1341
  %r11 = load i8, ptr %t_v508___ast_256___chali_tmp_o_1341
  call void @chali_owned_drop(i64 %r10)
  br label %ast_block_5
ast_block_4:
  br label %ast_block_5
ast_block_5:
  store i64 0, ptr %s_v508___ast_256___chali_tmp_o_1341
  store i8 0, ptr %t_v508___ast_256___chali_tmp_o_1341
  %r12 = call i64 @chali_host_call(ptr @native_name0, i64 0, ptr %indirect_arguments, ptr %native_argument_types)
  %r13 = call i8 @chali_host_result_float()
  store i64 %r12, ptr %s_v502___ast_256___chali_box_value
  store i8 %r13, ptr %t_v502___ast_256___chali_box_value
  %r14 = load i64, ptr %s_v502___ast_256___chali_box_value
  %r15 = load i8, ptr %t_v502___ast_256___chali_box_value
  %r16 = load i64, ptr @s_g_1_program__second
  %r17 = load i8, ptr @t_g_1_program__second
  %r18 = call i64 @chali_require_reference(i64 %r16)
  %r19 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r14, ptr %r19
  %r20 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r15, ptr %r20
  %r21 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 %r18, ptr %r21
  %r22 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r22
  %r23 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 126, ptr %r23
  %r24 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r24
  %r25 = getelementptr i64, ptr %indirect_arguments, i64 3
  store i64 0, ptr %r25
  %r26 = getelementptr i8, ptr %native_argument_types, i64 3
  store i8 0, ptr %r26
  %r27 = getelementptr i64, ptr %indirect_arguments, i64 4
  store i64 0, ptr %r27
  %r28 = getelementptr i8, ptr %native_argument_types, i64 4
  store i8 0, ptr %r28
  %r29 = getelementptr i64, ptr %indirect_arguments, i64 5
  store i64 80, ptr %r29
  %r30 = getelementptr i8, ptr %native_argument_types, i64 5
  store i8 0, ptr %r30
  %r31 = getelementptr i64, ptr %indirect_arguments, i64 6
  store i64 96, ptr %r31
  %r32 = getelementptr i8, ptr %native_argument_types, i64 6
  store i8 0, ptr %r32
  %r33 = call i64 @chali_host_call(ptr @native_name1, i64 7, ptr %indirect_arguments, ptr %native_argument_types)
  %r34 = call i8 @chali_host_result_float()
  %r35 = load i64, ptr %s_v502___ast_256___chali_box_value
  %r36 = load i8, ptr %t_v502___ast_256___chali_box_value
  store i64 %r35, ptr %s_v508___ast_256___chali_tmp_o_1341
  store i8 %r36, ptr %t_v508___ast_256___chali_tmp_o_1341
  %r37 = load i64, ptr %s_v508___ast_256___chali_tmp_o_1341
  %r38 = load i8, ptr %t_v508___ast_256___chali_tmp_o_1341
  %r39 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r37, ptr %r39
  %r40 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r38, ptr %r40
  %r41 = call i64 @chali_host_call(ptr @native_name2, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r42 = call i8 @chali_host_result_float()
  store i64 %r41, ptr %s_v505___ast_256___chali_ret_v_1342
  store i8 %r42, ptr %t_v505___ast_256___chali_ret_v_1342
  %r43 = load i64, ptr %s_v508___ast_256___chali_tmp_o_1341
  %r44 = load i8, ptr %t_v508___ast_256___chali_tmp_o_1341
  %r45 = icmp ne i64 %r43, 0
  %r46 = zext i1 %r45 to i64
  %r47 = icmp ne i64 %r46, 0
  br i1 %r47, label %ast_block_6, label %ast_block_7
ast_block_6:
  %r48 = load i64, ptr %s_v508___ast_256___chali_tmp_o_1341
  %r49 = load i8, ptr %t_v508___ast_256___chali_tmp_o_1341
  call void @chali_owned_drop(i64 %r48)
  br label %ast_block_8
ast_block_7:
  br label %ast_block_8
ast_block_8:
  store i64 0, ptr %s_v508___ast_256___chali_tmp_o_1341
  store i8 0, ptr %t_v508___ast_256___chali_tmp_o_1341
  %r50 = load i64, ptr %s_v505___ast_256___chali_ret_v_1342
  %r51 = load i8, ptr %t_v505___ast_256___chali_ret_v_1342
  store i8 %r51, ptr @chali_return_float
  ret i64 %r50
ast_block_9:
  %r52 = load i64, ptr %s_v508___ast_256___chali_tmp_o_1341
  %r53 = load i8, ptr %t_v508___ast_256___chali_tmp_o_1341
  %r54 = icmp ne i64 %r52, 0
  %r55 = zext i1 %r54 to i64
  %r56 = icmp ne i64 %r55, 0
  br i1 %r56, label %ast_block_10, label %ast_block_11
ast_block_10:
  %r57 = load i64, ptr %s_v508___ast_256___chali_tmp_o_1341
  %r58 = load i8, ptr %t_v508___ast_256___chali_tmp_o_1341
  call void @chali_owned_drop(i64 %r57)
  br label %ast_block_12
ast_block_11:
  br label %ast_block_12
ast_block_12:
  store i64 0, ptr %s_v508___ast_256___chali_tmp_o_1341
  store i8 0, ptr %t_v508___ast_256___chali_tmp_o_1341
  br label %ast_block_2
ast_block_1:
  br label %ast_block_2
ast_block_2:
  %r59 = load i64, ptr %s_v511___ast_256_index_610
  %r60 = load i8, ptr %t_v511___ast_256_index_610
  store ptr @f_v256_program__choose, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 28, ptr @chali_source_line
  %r61 = icmp eq i64 %r59, 2
  %r62 = zext i1 %r61 to i64
  %r63 = icmp ne i64 %r62, 0
  br i1 %r63, label %ast_block_13, label %ast_block_14
ast_block_13:
  %r64 = load i64, ptr %s_v509___ast_256___chali_tmp_o_1344
  %r65 = load i8, ptr %t_v509___ast_256___chali_tmp_o_1344
  %r66 = icmp ne i64 %r64, 0
  %r67 = zext i1 %r66 to i64
  %r68 = icmp ne i64 %r67, 0
  br i1 %r68, label %ast_block_16, label %ast_block_17
ast_block_16:
  %r69 = load i64, ptr %s_v509___ast_256___chali_tmp_o_1344
  %r70 = load i8, ptr %t_v509___ast_256___chali_tmp_o_1344
  call void @chali_owned_drop(i64 %r69)
  br label %ast_block_18
ast_block_17:
  br label %ast_block_18
ast_block_18:
  store i64 0, ptr %s_v509___ast_256___chali_tmp_o_1344
  store i8 0, ptr %t_v509___ast_256___chali_tmp_o_1344
  %r71 = call i64 @chali_host_call(ptr @native_name0, i64 0, ptr %indirect_arguments, ptr %native_argument_types)
  %r72 = call i8 @chali_host_result_float()
  store i64 %r71, ptr %s_v503___ast_256___chali_box_value_1343
  store i8 %r72, ptr %t_v503___ast_256___chali_box_value_1343
  %r73 = load i64, ptr %s_v503___ast_256___chali_box_value_1343
  %r74 = load i8, ptr %t_v503___ast_256___chali_box_value_1343
  %r75 = load i64, ptr @s_g_2_program__third
  %r76 = load i8, ptr @t_g_2_program__third
  %r77 = call i64 @chali_require_reference(i64 %r75)
  %r78 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r73, ptr %r78
  %r79 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r74, ptr %r79
  %r80 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 %r77, ptr %r80
  %r81 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r81
  %r82 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 128, ptr %r82
  %r83 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r83
  %r84 = getelementptr i64, ptr %indirect_arguments, i64 3
  store i64 0, ptr %r84
  %r85 = getelementptr i8, ptr %native_argument_types, i64 3
  store i8 0, ptr %r85
  %r86 = getelementptr i64, ptr %indirect_arguments, i64 4
  store i64 0, ptr %r86
  %r87 = getelementptr i8, ptr %native_argument_types, i64 4
  store i8 0, ptr %r87
  %r88 = getelementptr i64, ptr %indirect_arguments, i64 5
  store i64 128, ptr %r88
  %r89 = getelementptr i8, ptr %native_argument_types, i64 5
  store i8 0, ptr %r89
  %r90 = getelementptr i64, ptr %indirect_arguments, i64 6
  store i64 152, ptr %r90
  %r91 = getelementptr i8, ptr %native_argument_types, i64 6
  store i8 0, ptr %r91
  %r92 = call i64 @chali_host_call(ptr @native_name1, i64 7, ptr %indirect_arguments, ptr %native_argument_types)
  %r93 = call i8 @chali_host_result_float()
  %r94 = load i64, ptr %s_v503___ast_256___chali_box_value_1343
  %r95 = load i8, ptr %t_v503___ast_256___chali_box_value_1343
  store i64 %r94, ptr %s_v509___ast_256___chali_tmp_o_1344
  store i8 %r95, ptr %t_v509___ast_256___chali_tmp_o_1344
  %r96 = load i64, ptr %s_v509___ast_256___chali_tmp_o_1344
  %r97 = load i8, ptr %t_v509___ast_256___chali_tmp_o_1344
  %r98 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r96, ptr %r98
  %r99 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r97, ptr %r99
  %r100 = call i64 @chali_host_call(ptr @native_name2, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r101 = call i8 @chali_host_result_float()
  store i64 %r100, ptr %s_v506___ast_256___chali_ret_v_1345
  store i8 %r101, ptr %t_v506___ast_256___chali_ret_v_1345
  %r102 = load i64, ptr %s_v509___ast_256___chali_tmp_o_1344
  %r103 = load i8, ptr %t_v509___ast_256___chali_tmp_o_1344
  %r104 = icmp ne i64 %r102, 0
  %r105 = zext i1 %r104 to i64
  %r106 = icmp ne i64 %r105, 0
  br i1 %r106, label %ast_block_19, label %ast_block_20
ast_block_19:
  %r107 = load i64, ptr %s_v509___ast_256___chali_tmp_o_1344
  %r108 = load i8, ptr %t_v509___ast_256___chali_tmp_o_1344
  call void @chali_owned_drop(i64 %r107)
  br label %ast_block_21
ast_block_20:
  br label %ast_block_21
ast_block_21:
  store i64 0, ptr %s_v509___ast_256___chali_tmp_o_1344
  store i8 0, ptr %t_v509___ast_256___chali_tmp_o_1344
  %r109 = load i64, ptr %s_v506___ast_256___chali_ret_v_1345
  %r110 = load i8, ptr %t_v506___ast_256___chali_ret_v_1345
  store i8 %r110, ptr @chali_return_float
  ret i64 %r109
ast_block_22:
  %r111 = load i64, ptr %s_v509___ast_256___chali_tmp_o_1344
  %r112 = load i8, ptr %t_v509___ast_256___chali_tmp_o_1344
  %r113 = icmp ne i64 %r111, 0
  %r114 = zext i1 %r113 to i64
  %r115 = icmp ne i64 %r114, 0
  br i1 %r115, label %ast_block_23, label %ast_block_24
ast_block_23:
  %r116 = load i64, ptr %s_v509___ast_256___chali_tmp_o_1344
  %r117 = load i8, ptr %t_v509___ast_256___chali_tmp_o_1344
  call void @chali_owned_drop(i64 %r116)
  br label %ast_block_25
ast_block_24:
  br label %ast_block_25
ast_block_25:
  store i64 0, ptr %s_v509___ast_256___chali_tmp_o_1344
  store i8 0, ptr %t_v509___ast_256___chali_tmp_o_1344
  br label %ast_block_15
ast_block_14:
  br label %ast_block_15
ast_block_15:
  %r118 = load i64, ptr %s_v510___ast_256___chali_tmp_o_1347
  %r119 = load i8, ptr %t_v510___ast_256___chali_tmp_o_1347
  %r120 = icmp ne i64 %r118, 0
  %r121 = zext i1 %r120 to i64
  %r122 = icmp ne i64 %r121, 0
  br i1 %r122, label %ast_block_26, label %ast_block_27
ast_block_26:
  %r123 = load i64, ptr %s_v510___ast_256___chali_tmp_o_1347
  %r124 = load i8, ptr %t_v510___ast_256___chali_tmp_o_1347
  call void @chali_owned_drop(i64 %r123)
  br label %ast_block_28
ast_block_27:
  br label %ast_block_28
ast_block_28:
  store i64 0, ptr %s_v510___ast_256___chali_tmp_o_1347
  store i8 0, ptr %t_v510___ast_256___chali_tmp_o_1347
  %r125 = call i64 @chali_host_call(ptr @native_name0, i64 0, ptr %indirect_arguments, ptr %native_argument_types)
  %r126 = call i8 @chali_host_result_float()
  store i64 %r125, ptr %s_v504___ast_256___chali_box_value_1346
  store i8 %r126, ptr %t_v504___ast_256___chali_box_value_1346
  %r127 = load i64, ptr %s_v504___ast_256___chali_box_value_1346
  %r128 = load i8, ptr %t_v504___ast_256___chali_box_value_1346
  %r129 = load i64, ptr @s_g_0_program__first
  %r130 = load i8, ptr @t_g_0_program__first
  %r131 = call i64 @chali_require_reference(i64 %r129)
  %r132 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r127, ptr %r132
  %r133 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r128, ptr %r133
  %r134 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 %r131, ptr %r134
  %r135 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r135
  %r136 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 124, ptr %r136
  %r137 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r137
  %r138 = getelementptr i64, ptr %indirect_arguments, i64 3
  store i64 0, ptr %r138
  %r139 = getelementptr i8, ptr %native_argument_types, i64 3
  store i8 0, ptr %r139
  %r140 = getelementptr i64, ptr %indirect_arguments, i64 4
  store i64 0, ptr %r140
  %r141 = getelementptr i8, ptr %native_argument_types, i64 4
  store i8 0, ptr %r141
  %r142 = getelementptr i64, ptr %indirect_arguments, i64 5
  store i64 184, ptr %r142
  %r143 = getelementptr i8, ptr %native_argument_types, i64 5
  store i8 0, ptr %r143
  %r144 = getelementptr i64, ptr %indirect_arguments, i64 6
  store i64 200, ptr %r144
  %r145 = getelementptr i8, ptr %native_argument_types, i64 6
  store i8 0, ptr %r145
  %r146 = call i64 @chali_host_call(ptr @native_name1, i64 7, ptr %indirect_arguments, ptr %native_argument_types)
  %r147 = call i8 @chali_host_result_float()
  %r148 = load i64, ptr %s_v504___ast_256___chali_box_value_1346
  %r149 = load i8, ptr %t_v504___ast_256___chali_box_value_1346
  store i64 %r148, ptr %s_v510___ast_256___chali_tmp_o_1347
  store i8 %r149, ptr %t_v510___ast_256___chali_tmp_o_1347
  %r150 = load i64, ptr %s_v510___ast_256___chali_tmp_o_1347
  %r151 = load i8, ptr %t_v510___ast_256___chali_tmp_o_1347
  %r152 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r150, ptr %r152
  %r153 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r151, ptr %r153
  %r154 = call i64 @chali_host_call(ptr @native_name2, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r155 = call i8 @chali_host_result_float()
  store i64 %r154, ptr %s_v507___ast_256___chali_ret_v_1348
  store i8 %r155, ptr %t_v507___ast_256___chali_ret_v_1348
  %r156 = load i64, ptr %s_v510___ast_256___chali_tmp_o_1347
  %r157 = load i8, ptr %t_v510___ast_256___chali_tmp_o_1347
  %r158 = icmp ne i64 %r156, 0
  %r159 = zext i1 %r158 to i64
  %r160 = icmp ne i64 %r159, 0
  br i1 %r160, label %ast_block_29, label %ast_block_30
ast_block_29:
  %r161 = load i64, ptr %s_v510___ast_256___chali_tmp_o_1347
  %r162 = load i8, ptr %t_v510___ast_256___chali_tmp_o_1347
  call void @chali_owned_drop(i64 %r161)
  br label %ast_block_31
ast_block_30:
  br label %ast_block_31
ast_block_31:
  store i64 0, ptr %s_v510___ast_256___chali_tmp_o_1347
  store i8 0, ptr %t_v510___ast_256___chali_tmp_o_1347
  %r163 = load i64, ptr %s_v507___ast_256___chali_ret_v_1348
  %r164 = load i8, ptr %t_v507___ast_256___chali_ret_v_1348
  store i8 %r164, ptr @chali_return_float
  ret i64 %r163
ast_block_32:
  %r165 = load i64, ptr %s_v510___ast_256___chali_tmp_o_1347
  %r166 = load i8, ptr %t_v510___ast_256___chali_tmp_o_1347
  %r167 = icmp ne i64 %r165, 0
  %r168 = zext i1 %r167 to i64
  %r169 = icmp ne i64 %r168, 0
  br i1 %r169, label %ast_block_33, label %ast_block_34
ast_block_33:
  %r170 = load i64, ptr %s_v510___ast_256___chali_tmp_o_1347
  %r171 = load i8, ptr %t_v510___ast_256___chali_tmp_o_1347
  call void @chali_owned_drop(i64 %r170)
  br label %ast_block_35
ast_block_34:
  br label %ast_block_35
ast_block_35:
  store i64 0, ptr %s_v510___ast_256___chali_tmp_o_1347
  store i8 0, ptr %t_v510___ast_256___chali_tmp_o_1347
  store i8 0, ptr @chali_return_float
  ret i64 0
}

define internal i64 @f_v257_program__invoke(i64 %p0, i64 %p1) uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [4 x i64]
  %native_argument_types = alloca [4 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v512___ast_257___chali_ret_v_1349 = alloca i64
  store i64 0, ptr %s_v512___ast_257___chali_ret_v_1349
  %t_v512___ast_257___chali_ret_v_1349 = alloca i8
  store i8 0, ptr %t_v512___ast_257___chali_ret_v_1349
  %s_v513___ast_257_operation_611 = alloca i64
  store i64 0, ptr %s_v513___ast_257_operation_611
  %t_v513___ast_257_operation_611 = alloca i8
  store i8 0, ptr %t_v513___ast_257_operation_611
  %s_v514___ast_257_value_612 = alloca i64
  store i64 0, ptr %s_v514___ast_257_value_612
  %t_v514___ast_257_value_612 = alloca i8
  store i8 0, ptr %t_v514___ast_257_value_612
  store i64 %p0, ptr %s_v513___ast_257_operation_611
  store i64 %p1, ptr %s_v514___ast_257_value_612
  %r0 = load i64, ptr %s_v513___ast_257_operation_611
  %r1 = load i8, ptr %t_v513___ast_257_operation_611
  %r2 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r0, ptr %r2
  %r3 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r1, ptr %r3
  %r4 = call i64 @chali_host_call(ptr @native_name2, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r5 = call i8 @chali_host_result_float()
  store i64 %r4, ptr %s_v513___ast_257_operation_611
  store i8 %r5, ptr %t_v513___ast_257_operation_611
  %r6 = load i64, ptr %s_v513___ast_257_operation_611
  %r7 = load i8, ptr %t_v513___ast_257_operation_611
  %r8 = call i64 @chali_require_reference(i64 %r6)
  %r9 = load i64, ptr %s_v514___ast_257_value_612
  %r10 = load i8, ptr %t_v514___ast_257_value_612
  store ptr @f_v257_program__invoke, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 31, ptr @chali_source_line
  %r11 = call i64 @f_v486___chali_iface_NamedOperation_apply(i64 %r8, i64 1, i64 %r9)
  %r12 = load i8, ptr @chali_return_float
  store i64 %r11, ptr %s_v512___ast_257___chali_ret_v_1349
  store i8 %r12, ptr %t_v512___ast_257___chali_ret_v_1349
  %r13 = load i64, ptr %s_v513___ast_257_operation_611
  %r14 = load i8, ptr %t_v513___ast_257_operation_611
  %r15 = icmp ne i64 %r13, 0
  %r16 = zext i1 %r15 to i64
  %r17 = icmp ne i64 %r16, 0
  br i1 %r17, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r18 = load i64, ptr %s_v513___ast_257_operation_611
  %r19 = load i8, ptr %t_v513___ast_257_operation_611
  call void @chali_owned_drop(i64 %r18)
  br label %ast_block_2
ast_block_1:
  br label %ast_block_2
ast_block_2:
  store i64 0, ptr %s_v513___ast_257_operation_611
  store i8 0, ptr %t_v513___ast_257_operation_611
  %r20 = load i64, ptr %s_v512___ast_257___chali_ret_v_1349
  %r21 = load i8, ptr %t_v512___ast_257___chali_ret_v_1349
  store i8 %r21, ptr @chali_return_float
  ret i64 %r20
ast_block_3:
  %r22 = load i64, ptr %s_v513___ast_257_operation_611
  %r23 = load i8, ptr %t_v513___ast_257_operation_611
  %r24 = icmp ne i64 %r22, 0
  %r25 = zext i1 %r24 to i64
  %r26 = icmp ne i64 %r25, 0
  br i1 %r26, label %ast_block_4, label %ast_block_5
ast_block_4:
  %r27 = load i64, ptr %s_v513___ast_257_operation_611
  %r28 = load i8, ptr %t_v513___ast_257_operation_611
  call void @chali_owned_drop(i64 %r27)
  br label %ast_block_6
ast_block_5:
  br label %ast_block_6
ast_block_6:
  store i64 0, ptr %s_v513___ast_257_operation_611
  store i8 0, ptr %t_v513___ast_257_operation_611
  store i8 0, ptr @chali_return_float
  ret i64 0
ast_label_v515___chali_cleanup_program__invoke:
  %r29 = load i64, ptr %s_v513___ast_257_operation_611
  %r30 = load i8, ptr %t_v513___ast_257_operation_611
  %r31 = icmp ne i64 %r29, 0
  %r32 = zext i1 %r31 to i64
  %r33 = icmp ne i64 %r32, 0
  br i1 %r33, label %ast_block_7, label %ast_block_8
ast_block_7:
  %r34 = load i64, ptr %s_v513___ast_257_operation_611
  %r35 = load i8, ptr %t_v513___ast_257_operation_611
  call void @chali_owned_drop(i64 %r34)
  br label %ast_block_9
ast_block_8:
  br label %ast_block_9
ast_block_9:
  store i64 0, ptr %s_v513___ast_257_operation_611
  store i8 0, ptr %t_v513___ast_257_operation_611
  store i8 0, ptr @chali_return_float
  ret i64 0
}

define i64 @__chali_impl_program__work(i64 %p0) uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [3 x i64]
  %native_argument_types = alloca [3 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v516___ast_258___chali_ret_v_1351 = alloca i64
  store i64 0, ptr %s_v516___ast_258___chali_ret_v_1351
  %t_v516___ast_258___chali_ret_v_1351 = alloca i8
  store i8 0, ptr %t_v516___ast_258___chali_ret_v_1351
  %s_v517___ast_258_acc = alloca i64
  store i64 0, ptr %s_v517___ast_258_acc
  %t_v517___ast_258_acc = alloca i8
  store i8 0, ptr %t_v517___ast_258_acc
  %s_v518___ast_258_derived = alloca i64
  store i64 0, ptr %s_v518___ast_258_derived
  %t_v518___ast_258_derived = alloca i8
  store i8 0, ptr %t_v518___ast_258_derived
  %s_v519___ast_258_i_1350 = alloca i64
  store i64 0, ptr %s_v519___ast_258_i_1350
  %t_v519___ast_258_i_1350 = alloca i8
  store i8 0, ptr %t_v519___ast_258_i_1350
  %s_v520___ast_258_seed = alloca i64
  store i64 0, ptr %s_v520___ast_258_seed
  %t_v520___ast_258_seed = alloca i8
  store i8 0, ptr %t_v520___ast_258_seed
  store i64 %p0, ptr %s_v520___ast_258_seed
  %r0 = load i64, ptr %s_v520___ast_258_seed
  %r1 = load i8, ptr %t_v520___ast_258_seed
  store i64 %r0, ptr %s_v517___ast_258_acc
  store i8 %r1, ptr %t_v517___ast_258_acc
  store i64 0, ptr %s_v519___ast_258_i_1350
  store i8 0, ptr %t_v519___ast_258_i_1350
  br label %ast_label_v521___chali_loop_135
ast_label_v521___chali_loop_135:
  %r2 = load i64, ptr %s_v519___ast_258_i_1350
  %r3 = load i8, ptr %t_v519___ast_258_i_1350
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 35, ptr @chali_source_line
  %r4 = icmp slt i64 %r2, 262144
  %r5 = zext i1 %r4 to i64
  %r6 = icmp ne i64 %r5, 0
  br i1 %r6, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r7 = load i64, ptr %s_v519___ast_258_i_1350
  %r8 = load i8, ptr %t_v519___ast_258_i_1350
  %r9 = load i64, ptr %s_v520___ast_258_seed
  %r10 = load i8, ptr %t_v520___ast_258_seed
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 36, ptr @chali_source_line
  %r11 = add i64 %r7, %r9
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 36, ptr @chali_source_line
  %r12 = and i64 %r11, 3
  %r13 = call i64 @f_v256_program__choose(i64 %r12)
  %r14 = load i8, ptr @chali_return_float
  %r15 = call i64 @chali_require_reference(i64 %r13)
  store i64 %r15, ptr %s_v518___ast_258_derived
  store i8 0, ptr %t_v518___ast_258_derived
  %r16 = load i64, ptr %s_v518___ast_258_derived
  %r17 = load i8, ptr %t_v518___ast_258_derived
  %r18 = call i64 @chali_require_reference(i64 %r16)
  %r19 = call i64 @chali_require_reference(i64 %r18)
  %r20 = load i64, ptr %s_v517___ast_258_acc
  %r21 = load i8, ptr %t_v517___ast_258_acc
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 37, ptr @chali_source_line
  %r22 = call i64 @f_v257_program__invoke(i64 %r19, i64 %r20)
  %r23 = load i8, ptr @chali_return_float
  store i64 %r22, ptr %s_v517___ast_258_acc
  store i8 %r23, ptr %t_v517___ast_258_acc
  %r24 = load i64, ptr %s_v519___ast_258_i_1350
  %r25 = load i8, ptr %t_v519___ast_258_i_1350
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 38, ptr @chali_source_line
  %r26 = add i64 %r24, 1
  store i64 %r26, ptr %s_v519___ast_258_i_1350
  store i8 0, ptr %t_v519___ast_258_i_1350
  %r27 = load i64, ptr %s_v518___ast_258_derived
  %r28 = load i8, ptr %t_v518___ast_258_derived
  %r29 = icmp ne i64 %r27, 0
  %r30 = zext i1 %r29 to i64
  %r31 = icmp ne i64 %r30, 0
  br i1 %r31, label %ast_block_3, label %ast_block_4
ast_block_3:
  %r32 = load i64, ptr %s_v518___ast_258_derived
  %r33 = load i8, ptr %t_v518___ast_258_derived
  call void @chali_owned_drop(i64 %r32)
  br label %ast_block_5
ast_block_4:
  br label %ast_block_5
ast_block_5:
  store i64 0, ptr %s_v518___ast_258_derived
  store i8 0, ptr %t_v518___ast_258_derived
  br label %ast_block_2
ast_block_1:
  br label %ast_label_v522___chali_brk_136
ast_block_2:
  br label %ast_label_v521___chali_loop_135
ast_label_v522___chali_brk_136:
  %r34 = load i64, ptr %s_v517___ast_258_acc
  %r35 = load i8, ptr %t_v517___ast_258_acc
  store i64 %r34, ptr %s_v516___ast_258___chali_ret_v_1351
  store i8 %r35, ptr %t_v516___ast_258___chali_ret_v_1351
  %r36 = load i64, ptr %s_v516___ast_258___chali_ret_v_1351
  %r37 = load i8, ptr %t_v516___ast_258___chali_ret_v_1351
  store i8 %r37, ptr @chali_return_float
  ret i64 %r36
}
