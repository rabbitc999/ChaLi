define i64 @__chali_impl_program__numericFill(i64 %p0) uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [5 x i64]
  %native_argument_types = alloca [5 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v768___ast_246___chali_reference_storage_1324 = alloca i64
  store i64 0, ptr %s_v768___ast_246___chali_reference_storage_1324
  %t_v768___ast_246___chali_reference_storage_1324 = alloca i8
  store i8 0, ptr %t_v768___ast_246___chali_reference_storage_1324
  %s_v769___ast_246___chali_reference_storage_1328 = alloca i64
  store i64 0, ptr %s_v769___ast_246___chali_reference_storage_1328
  %t_v769___ast_246___chali_reference_storage_1328 = alloca i8
  store i8 0, ptr %t_v769___ast_246___chali_reference_storage_1328
  %s_v770___ast_246___chali_ret_v_1329 = alloca i64
  store i64 0, ptr %s_v770___ast_246___chali_ret_v_1329
  %t_v770___ast_246___chali_ret_v_1329 = alloca i8
  store i8 0, ptr %t_v770___ast_246___chali_ret_v_1329
  %s_v771___ast_246_i_1325 = alloca i64
  store i64 0, ptr %s_v771___ast_246_i_1325
  %t_v771___ast_246_i_1325 = alloca i8
  store i8 0, ptr %t_v771___ast_246_i_1325
  %s_v772___ast_246_raw_1326 = alloca i64
  store i64 0, ptr %s_v772___ast_246_raw_1326
  %t_v772___ast_246_raw_1326 = alloca i8
  store i8 0, ptr %t_v772___ast_246_raw_1326
  %s_v773___ast_246_result_1327 = alloca i64
  store i64 0, ptr %s_v773___ast_246_result_1327
  %t_v773___ast_246_result_1327 = alloca i8
  store i8 0, ptr %t_v773___ast_246_result_1327
  %s_v774___ast_246_seed = alloca i64
  store i64 0, ptr %s_v774___ast_246_seed
  %t_v774___ast_246_seed = alloca i8
  store i8 0, ptr %t_v774___ast_246_seed
  %s_v775___ast_246_values = alloca i64
  store i64 0, ptr %s_v775___ast_246_values
  %t_v775___ast_246_values = alloca i8
  store i8 0, ptr %t_v775___ast_246_values
  store i64 %p0, ptr %s_v774___ast_246_seed
  %r0 = or i64 0, 0
  %r1 = call i64 @chali_host_list_storage_scalar(i64 9, i64 0, i64 0, i64 0, i64 0, i64 0, i64 %r0, ptr %collection_result_kind)
  %r2 = load i8, ptr %collection_result_kind
  store i64 %r1, ptr %s_v775___ast_246_values
  store i8 %r2, ptr %t_v775___ast_246_values
  %r3 = call i64 @chali_alloc(i64 8)
  store i64 %r3, ptr %s_v768___ast_246___chali_reference_storage_1324
  store i8 0, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r4 = load i64, ptr %s_v775___ast_246_values
  %r5 = load i8, ptr %t_v775___ast_246_values
  %r6 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r7 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r8 = call ptr @chali_primitive_address(i64 %r6, i64 8)
  store i64 %r4, ptr %r8, align 1
  store i64 0, ptr %s_v771___ast_246_i_1325
  store i8 0, ptr %t_v771___ast_246_i_1325
  br label %ast_label_v776___chali_loop_135
ast_label_v776___chali_loop_135:
  %r9 = load i64, ptr %s_v771___ast_246_i_1325
  %r10 = load i8, ptr %t_v771___ast_246_i_1325
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 4, ptr @chali_source_line
  %r11 = icmp slt i64 %r9, 257
  %r12 = zext i1 %r11 to i64
  %r13 = icmp ne i64 %r12, 0
  br i1 %r13, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r14 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r15 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 4, ptr @chali_source_line
  %r16 = call ptr @chali_primitive_address(i64 %r14, i64 8)
  %r17 = load i64, ptr %r16, align 1
  %r18 = load i64, ptr %s_v774___ast_246_seed
  %r19 = load i8, ptr %t_v774___ast_246_seed
  %r20 = load i64, ptr %s_v771___ast_246_i_1325
  %r21 = load i8, ptr %t_v771___ast_246_i_1325
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 4, ptr @chali_source_line
  %r22 = add i64 %r18, %r20
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 4, ptr @chali_source_line
  %r23 = call i64 @f_v185_std__lang____bootListPushI(i64 %r17, i64 %r22, i64 0, i64 0)
  %r24 = load i8, ptr @chali_return_float
  %r25 = load i64, ptr %s_v771___ast_246_i_1325
  %r26 = load i8, ptr %t_v771___ast_246_i_1325
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 4, ptr @chali_source_line
  %r27 = add i64 %r25, 1
  store i64 %r27, ptr %s_v771___ast_246_i_1325
  store i8 0, ptr %t_v771___ast_246_i_1325
  br label %ast_block_2
ast_block_1:
  br label %ast_label_v777___chali_brk_136
ast_block_2:
  br label %ast_label_v776___chali_loop_135
ast_label_v777___chali_brk_136:
  %r28 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r29 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r30 = call i64 @chali_require_reference(i64 %r28)
  %r31 = call i64 @chali_require_reference(i64 %r30)
  store i64 %r31, ptr %s_v772___ast_246_raw_1326
  store i8 0, ptr %t_v772___ast_246_raw_1326
  %r32 = load i64, ptr %s_v772___ast_246_raw_1326
  %r33 = load i8, ptr %t_v772___ast_246_raw_1326
  %r34 = call i64 @chali_require_reference(i64 %r32)
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 6, ptr @chali_source_line
  %r35 = call ptr @chali_primitive_address(i64 %r34, i64 8)
  %r36 = load i64, ptr %r35, align 1
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 6, ptr @chali_source_line
  %r37 = or i64 0, 0
  %r38 = call i64 @chali_host_list_storage_scalar(i64 1, i64 %r36, i64 0, i64 0, i64 0, i64 0, i64 %r37, ptr %collection_result_kind)
  %r39 = load i8, ptr %collection_result_kind
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 6, ptr @chali_source_line
  %r40 = icmp eq i64 %r38, 512
  %r41 = zext i1 %r40 to i64
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 6, ptr @chali_source_line
  %r42 = call i64 @f_v173_std__lang__assert_bool(i64 %r41)
  %r43 = load i8, ptr @chali_return_float
  store i64 0, ptr %s_v773___ast_246_result_1327
  store i8 0, ptr %t_v773___ast_246_result_1327
  %r44 = call i64 @chali_alloc(i64 8)
  store i64 %r44, ptr %s_v769___ast_246___chali_reference_storage_1328
  store i8 0, ptr %t_v769___ast_246___chali_reference_storage_1328
  %r45 = load i64, ptr %s_v773___ast_246_result_1327
  %r46 = load i8, ptr %t_v773___ast_246_result_1327
  %r47 = load i64, ptr %s_v769___ast_246___chali_reference_storage_1328
  %r48 = load i8, ptr %t_v769___ast_246___chali_reference_storage_1328
  %r49 = call ptr @chali_primitive_address(i64 %r47, i64 8)
  store i64 %r45, ptr %r49, align 1
  store i64 0, ptr %s_v771___ast_246_i_1325
  store i8 0, ptr %t_v771___ast_246_i_1325
  br label %ast_label_v778___chali_loop_137
ast_label_v778___chali_loop_137:
  %r50 = load i64, ptr %s_v771___ast_246_i_1325
  %r51 = load i8, ptr %t_v771___ast_246_i_1325
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 7, ptr @chali_source_line
  %r52 = icmp slt i64 %r50, 257
  %r53 = zext i1 %r52 to i64
  %r54 = icmp ne i64 %r53, 0
  br i1 %r54, label %ast_block_3, label %ast_block_4
ast_block_3:
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 7, ptr @chali_source_line
  %r55 = load i64, ptr %s_v769___ast_246___chali_reference_storage_1328
  %r56 = load i8, ptr %t_v769___ast_246___chali_reference_storage_1328
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 7, ptr @chali_source_line
  %r57 = call ptr @chali_primitive_address(i64 %r55, i64 8)
  %r58 = load i64, ptr %r57, align 1
  %r59 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r60 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 7, ptr @chali_source_line
  %r61 = call ptr @chali_primitive_address(i64 %r59, i64 8)
  %r62 = load i64, ptr %r61, align 1
  %r63 = load i64, ptr %s_v771___ast_246_i_1325
  %r64 = load i8, ptr %t_v771___ast_246_i_1325
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 7, ptr @chali_source_line
  %r65 = zext i8 %r64 to i64
  %r66 = shl i64 %r65, 1
  %r67 = or i64 0, %r66
  %r68 = or i64 %r67, 0
  %r69 = call i64 @chali_host_list_storage_scalar(i64 11, i64 %r62, i64 %r63, i64 0, i64 0, i64 0, i64 %r68, ptr %collection_result_kind)
  %r70 = load i8, ptr %collection_result_kind
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 7, ptr @chali_source_line
  %r71 = add i64 %r58, %r69
  %r72 = load i64, ptr %s_v769___ast_246___chali_reference_storage_1328
  %r73 = load i8, ptr %t_v769___ast_246___chali_reference_storage_1328
  %r74 = call ptr @chali_primitive_address(i64 %r72, i64 8)
  store i64 %r71, ptr %r74, align 1
  %r75 = load i64, ptr %s_v771___ast_246_i_1325
  %r76 = load i8, ptr %t_v771___ast_246_i_1325
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 7, ptr @chali_source_line
  %r77 = add i64 %r75, 1
  store i64 %r77, ptr %s_v771___ast_246_i_1325
  store i8 0, ptr %t_v771___ast_246_i_1325
  br label %ast_block_5
ast_block_4:
  br label %ast_label_v779___chali_brk_138
ast_block_5:
  br label %ast_label_v778___chali_loop_137
ast_label_v779___chali_brk_138:
  %r78 = load i64, ptr %s_v769___ast_246___chali_reference_storage_1328
  %r79 = load i8, ptr %t_v769___ast_246___chali_reference_storage_1328
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 7, ptr @chali_source_line
  %r80 = call ptr @chali_primitive_address(i64 %r78, i64 8)
  %r81 = load i64, ptr %r80, align 1
  store i64 %r81, ptr %s_v770___ast_246___chali_ret_v_1329
  store i8 0, ptr %t_v770___ast_246___chali_ret_v_1329
  %r82 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r83 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r84 = call ptr @chali_primitive_address(i64 %r82, i64 8)
  %r85 = load i64, ptr %r84, align 1
  %r86 = icmp ne i64 %r85, 0
  %r87 = zext i1 %r86 to i64
  %r88 = icmp ne i64 %r87, 0
  br i1 %r88, label %ast_block_6, label %ast_block_7
ast_block_6:
  %r89 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r90 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r91 = call ptr @chali_primitive_address(i64 %r89, i64 8)
  %r92 = load i64, ptr %r91, align 1
  call void @chali_owned_drop(i64 %r92)
  br label %ast_block_8
ast_block_7:
  br label %ast_block_8
ast_block_8:
  %r93 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r94 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r95 = call ptr @chali_primitive_address(i64 %r93, i64 8)
  store i64 0, ptr %r95, align 1
  %r96 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r97 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r98 = icmp ne i64 %r96, 0
  %r99 = zext i1 %r98 to i64
  %r100 = icmp ne i64 %r99, 0
  br i1 %r100, label %ast_block_9, label %ast_block_10
ast_block_9:
  %r101 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r102 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  call void @chali_owned_drop(i64 %r101)
  br label %ast_block_11
ast_block_10:
  br label %ast_block_11
ast_block_11:
  store i64 0, ptr %s_v768___ast_246___chali_reference_storage_1324
  store i8 0, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r103 = load i64, ptr %s_v769___ast_246___chali_reference_storage_1328
  %r104 = load i8, ptr %t_v769___ast_246___chali_reference_storage_1328
  %r105 = icmp ne i64 %r103, 0
  %r106 = zext i1 %r105 to i64
  %r107 = icmp ne i64 %r106, 0
  br i1 %r107, label %ast_block_12, label %ast_block_13
ast_block_12:
  %r108 = load i64, ptr %s_v769___ast_246___chali_reference_storage_1328
  %r109 = load i8, ptr %t_v769___ast_246___chali_reference_storage_1328
  call void @chali_owned_drop(i64 %r108)
  br label %ast_block_14
ast_block_13:
  br label %ast_block_14
ast_block_14:
  store i64 0, ptr %s_v769___ast_246___chali_reference_storage_1328
  store i8 0, ptr %t_v769___ast_246___chali_reference_storage_1328
  %r110 = load i64, ptr %s_v770___ast_246___chali_ret_v_1329
  %r111 = load i8, ptr %t_v770___ast_246___chali_ret_v_1329
  store i8 %r111, ptr @chali_return_float
  ret i64 %r110
ast_block_15:
  %r112 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r113 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r114 = call ptr @chali_primitive_address(i64 %r112, i64 8)
  %r115 = load i64, ptr %r114, align 1
  %r116 = icmp ne i64 %r115, 0
  %r117 = zext i1 %r116 to i64
  %r118 = icmp ne i64 %r117, 0
  br i1 %r118, label %ast_block_16, label %ast_block_17
ast_block_16:
  %r119 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r120 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r121 = call ptr @chali_primitive_address(i64 %r119, i64 8)
  %r122 = load i64, ptr %r121, align 1
  call void @chali_owned_drop(i64 %r122)
  br label %ast_block_18
ast_block_17:
  br label %ast_block_18
ast_block_18:
  %r123 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r124 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r125 = call ptr @chali_primitive_address(i64 %r123, i64 8)
  store i64 0, ptr %r125, align 1
  %r126 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r127 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r128 = icmp ne i64 %r126, 0
  %r129 = zext i1 %r128 to i64
  %r130 = icmp ne i64 %r129, 0
  br i1 %r130, label %ast_block_19, label %ast_block_20
ast_block_19:
  %r131 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r132 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  call void @chali_owned_drop(i64 %r131)
  br label %ast_block_21
ast_block_20:
  br label %ast_block_21
ast_block_21:
  store i64 0, ptr %s_v768___ast_246___chali_reference_storage_1324
  store i8 0, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r133 = load i64, ptr %s_v769___ast_246___chali_reference_storage_1328
  %r134 = load i8, ptr %t_v769___ast_246___chali_reference_storage_1328
  %r135 = icmp ne i64 %r133, 0
  %r136 = zext i1 %r135 to i64
  %r137 = icmp ne i64 %r136, 0
  br i1 %r137, label %ast_block_22, label %ast_block_23
ast_block_22:
  %r138 = load i64, ptr %s_v769___ast_246___chali_reference_storage_1328
  %r139 = load i8, ptr %t_v769___ast_246___chali_reference_storage_1328
  call void @chali_owned_drop(i64 %r138)
  br label %ast_block_24
ast_block_23:
  br label %ast_block_24
ast_block_24:
  store i64 0, ptr %s_v769___ast_246___chali_reference_storage_1328
  store i8 0, ptr %t_v769___ast_246___chali_reference_storage_1328
  store i8 0, ptr @chali_return_float
  ret i64 0
ast_label_v780___chali_cleanup_program__numericFill:
  %r140 = load i64, ptr %s_v769___ast_246___chali_reference_storage_1328
  %r141 = load i8, ptr %t_v769___ast_246___chali_reference_storage_1328
  %r142 = icmp ne i64 %r140, 0
  %r143 = zext i1 %r142 to i64
  %r144 = icmp ne i64 %r143, 0
  br i1 %r144, label %ast_block_25, label %ast_block_26
ast_block_25:
  %r145 = load i64, ptr %s_v769___ast_246___chali_reference_storage_1328
  %r146 = load i8, ptr %t_v769___ast_246___chali_reference_storage_1328
  call void @chali_owned_drop(i64 %r145)
  br label %ast_block_27
ast_block_26:
  br label %ast_block_27
ast_block_27:
  store i64 0, ptr %s_v769___ast_246___chali_reference_storage_1328
  store i8 0, ptr %t_v769___ast_246___chali_reference_storage_1328
  %r147 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r148 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r149 = icmp ne i64 %r147, 0
  %r150 = zext i1 %r149 to i64
  %r151 = icmp ne i64 %r150, 0
  br i1 %r151, label %ast_block_28, label %ast_block_29
ast_block_28:
  %r152 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r153 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  call void @chali_owned_drop(i64 %r152)
  br label %ast_block_30
ast_block_29:
  br label %ast_block_30
ast_block_30:
  store i64 0, ptr %s_v768___ast_246___chali_reference_storage_1324
  store i8 0, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r154 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r155 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r156 = call ptr @chali_primitive_address(i64 %r154, i64 8)
  %r157 = load i64, ptr %r156, align 1
  %r158 = icmp ne i64 %r157, 0
  %r159 = zext i1 %r158 to i64
  %r160 = icmp ne i64 %r159, 0
  br i1 %r160, label %ast_block_31, label %ast_block_32
ast_block_31:
  %r161 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r162 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r163 = call ptr @chali_primitive_address(i64 %r161, i64 8)
  %r164 = load i64, ptr %r163, align 1
  call void @chali_owned_drop(i64 %r164)
  br label %ast_block_33
ast_block_32:
  br label %ast_block_33
ast_block_33:
  %r165 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r166 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r167 = call ptr @chali_primitive_address(i64 %r165, i64 8)
  store i64 0, ptr %r167, align 1
  store i8 0, ptr @chali_return_float
  ret i64 0
}
