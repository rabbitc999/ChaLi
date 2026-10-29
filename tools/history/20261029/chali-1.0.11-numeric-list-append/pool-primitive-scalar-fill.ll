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
  %r23 = or i64 0, 0
  %r24 = call i64 @chali_host_list_storage_scalar(i64 18, i64 %r17, i64 %r22, i64 0, i64 0, i64 0, i64 %r23, ptr %collection_result_kind)
  %r25 = load i8, ptr %collection_result_kind
  %r26 = load i64, ptr %s_v771___ast_246_i_1325
  %r27 = load i8, ptr %t_v771___ast_246_i_1325
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 4, ptr @chali_source_line
  %r28 = add i64 %r26, 1
  store i64 %r28, ptr %s_v771___ast_246_i_1325
  store i8 0, ptr %t_v771___ast_246_i_1325
  br label %ast_block_2
ast_block_1:
  br label %ast_label_v777___chali_brk_136
ast_block_2:
  br label %ast_label_v776___chali_loop_135
ast_label_v777___chali_brk_136:
  %r29 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r30 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r31 = call i64 @chali_require_reference(i64 %r29)
  %r32 = call i64 @chali_require_reference(i64 %r31)
  store i64 %r32, ptr %s_v772___ast_246_raw_1326
  store i8 0, ptr %t_v772___ast_246_raw_1326
  %r33 = load i64, ptr %s_v772___ast_246_raw_1326
  %r34 = load i8, ptr %t_v772___ast_246_raw_1326
  %r35 = call i64 @chali_require_reference(i64 %r33)
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 6, ptr @chali_source_line
  %r36 = call ptr @chali_primitive_address(i64 %r35, i64 8)
  %r37 = load i64, ptr %r36, align 1
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 6, ptr @chali_source_line
  %r38 = or i64 0, 0
  %r39 = call i64 @chali_host_list_storage_scalar(i64 1, i64 %r37, i64 0, i64 0, i64 0, i64 0, i64 %r38, ptr %collection_result_kind)
  %r40 = load i8, ptr %collection_result_kind
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 6, ptr @chali_source_line
  %r41 = icmp eq i64 %r39, 512
  %r42 = zext i1 %r41 to i64
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 6, ptr @chali_source_line
  %r43 = call i64 @f_v173_std__lang__assert_bool(i64 %r42)
  %r44 = load i8, ptr @chali_return_float
  store i64 0, ptr %s_v773___ast_246_result_1327
  store i8 0, ptr %t_v773___ast_246_result_1327
  %r45 = call i64 @chali_alloc(i64 8)
  store i64 %r45, ptr %s_v769___ast_246___chali_reference_storage_1328
  store i8 0, ptr %t_v769___ast_246___chali_reference_storage_1328
  %r46 = load i64, ptr %s_v773___ast_246_result_1327
  %r47 = load i8, ptr %t_v773___ast_246_result_1327
  %r48 = load i64, ptr %s_v769___ast_246___chali_reference_storage_1328
  %r49 = load i8, ptr %t_v769___ast_246___chali_reference_storage_1328
  %r50 = call ptr @chali_primitive_address(i64 %r48, i64 8)
  store i64 %r46, ptr %r50, align 1
  store i64 0, ptr %s_v771___ast_246_i_1325
  store i8 0, ptr %t_v771___ast_246_i_1325
  br label %ast_label_v778___chali_loop_137
ast_label_v778___chali_loop_137:
  %r51 = load i64, ptr %s_v771___ast_246_i_1325
  %r52 = load i8, ptr %t_v771___ast_246_i_1325
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 7, ptr @chali_source_line
  %r53 = icmp slt i64 %r51, 257
  %r54 = zext i1 %r53 to i64
  %r55 = icmp ne i64 %r54, 0
  br i1 %r55, label %ast_block_3, label %ast_block_4
ast_block_3:
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 7, ptr @chali_source_line
  %r56 = load i64, ptr %s_v769___ast_246___chali_reference_storage_1328
  %r57 = load i8, ptr %t_v769___ast_246___chali_reference_storage_1328
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 7, ptr @chali_source_line
  %r58 = call ptr @chali_primitive_address(i64 %r56, i64 8)
  %r59 = load i64, ptr %r58, align 1
  %r60 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r61 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 7, ptr @chali_source_line
  %r62 = call ptr @chali_primitive_address(i64 %r60, i64 8)
  %r63 = load i64, ptr %r62, align 1
  %r64 = load i64, ptr %s_v771___ast_246_i_1325
  %r65 = load i8, ptr %t_v771___ast_246_i_1325
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 7, ptr @chali_source_line
  %r66 = zext i8 %r65 to i64
  %r67 = shl i64 %r66, 1
  %r68 = or i64 0, %r67
  %r69 = or i64 %r68, 0
  %r70 = call i64 @chali_host_list_storage_scalar(i64 11, i64 %r63, i64 %r64, i64 0, i64 0, i64 0, i64 %r69, ptr %collection_result_kind)
  %r71 = load i8, ptr %collection_result_kind
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 7, ptr @chali_source_line
  %r72 = add i64 %r59, %r70
  %r73 = load i64, ptr %s_v769___ast_246___chali_reference_storage_1328
  %r74 = load i8, ptr %t_v769___ast_246___chali_reference_storage_1328
  %r75 = call ptr @chali_primitive_address(i64 %r73, i64 8)
  store i64 %r72, ptr %r75, align 1
  %r76 = load i64, ptr %s_v771___ast_246_i_1325
  %r77 = load i8, ptr %t_v771___ast_246_i_1325
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 7, ptr @chali_source_line
  %r78 = add i64 %r76, 1
  store i64 %r78, ptr %s_v771___ast_246_i_1325
  store i8 0, ptr %t_v771___ast_246_i_1325
  br label %ast_block_5
ast_block_4:
  br label %ast_label_v779___chali_brk_138
ast_block_5:
  br label %ast_label_v778___chali_loop_137
ast_label_v779___chali_brk_138:
  %r79 = load i64, ptr %s_v769___ast_246___chali_reference_storage_1328
  %r80 = load i8, ptr %t_v769___ast_246___chali_reference_storage_1328
  store ptr @__chali_impl_program__numericFill, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 7, ptr @chali_source_line
  %r81 = call ptr @chali_primitive_address(i64 %r79, i64 8)
  %r82 = load i64, ptr %r81, align 1
  store i64 %r82, ptr %s_v770___ast_246___chali_ret_v_1329
  store i8 0, ptr %t_v770___ast_246___chali_ret_v_1329
  %r83 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r84 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r85 = call ptr @chali_primitive_address(i64 %r83, i64 8)
  %r86 = load i64, ptr %r85, align 1
  %r87 = icmp ne i64 %r86, 0
  %r88 = zext i1 %r87 to i64
  %r89 = icmp ne i64 %r88, 0
  br i1 %r89, label %ast_block_6, label %ast_block_7
ast_block_6:
  %r90 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r91 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r92 = call ptr @chali_primitive_address(i64 %r90, i64 8)
  %r93 = load i64, ptr %r92, align 1
  call void @chali_owned_drop(i64 %r93)
  br label %ast_block_8
ast_block_7:
  br label %ast_block_8
ast_block_8:
  %r94 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r95 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r96 = call ptr @chali_primitive_address(i64 %r94, i64 8)
  store i64 0, ptr %r96, align 1
  %r97 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r98 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r99 = icmp ne i64 %r97, 0
  %r100 = zext i1 %r99 to i64
  %r101 = icmp ne i64 %r100, 0
  br i1 %r101, label %ast_block_9, label %ast_block_10
ast_block_9:
  %r102 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r103 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  call void @chali_owned_drop(i64 %r102)
  br label %ast_block_11
ast_block_10:
  br label %ast_block_11
ast_block_11:
  store i64 0, ptr %s_v768___ast_246___chali_reference_storage_1324
  store i8 0, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r104 = load i64, ptr %s_v769___ast_246___chali_reference_storage_1328
  %r105 = load i8, ptr %t_v769___ast_246___chali_reference_storage_1328
  %r106 = icmp ne i64 %r104, 0
  %r107 = zext i1 %r106 to i64
  %r108 = icmp ne i64 %r107, 0
  br i1 %r108, label %ast_block_12, label %ast_block_13
ast_block_12:
  %r109 = load i64, ptr %s_v769___ast_246___chali_reference_storage_1328
  %r110 = load i8, ptr %t_v769___ast_246___chali_reference_storage_1328
  call void @chali_owned_drop(i64 %r109)
  br label %ast_block_14
ast_block_13:
  br label %ast_block_14
ast_block_14:
  store i64 0, ptr %s_v769___ast_246___chali_reference_storage_1328
  store i8 0, ptr %t_v769___ast_246___chali_reference_storage_1328
  %r111 = load i64, ptr %s_v770___ast_246___chali_ret_v_1329
  %r112 = load i8, ptr %t_v770___ast_246___chali_ret_v_1329
  store i8 %r112, ptr @chali_return_float
  ret i64 %r111
ast_block_15:
  %r113 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r114 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r115 = call ptr @chali_primitive_address(i64 %r113, i64 8)
  %r116 = load i64, ptr %r115, align 1
  %r117 = icmp ne i64 %r116, 0
  %r118 = zext i1 %r117 to i64
  %r119 = icmp ne i64 %r118, 0
  br i1 %r119, label %ast_block_16, label %ast_block_17
ast_block_16:
  %r120 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r121 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r122 = call ptr @chali_primitive_address(i64 %r120, i64 8)
  %r123 = load i64, ptr %r122, align 1
  call void @chali_owned_drop(i64 %r123)
  br label %ast_block_18
ast_block_17:
  br label %ast_block_18
ast_block_18:
  %r124 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r125 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r126 = call ptr @chali_primitive_address(i64 %r124, i64 8)
  store i64 0, ptr %r126, align 1
  %r127 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r128 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r129 = icmp ne i64 %r127, 0
  %r130 = zext i1 %r129 to i64
  %r131 = icmp ne i64 %r130, 0
  br i1 %r131, label %ast_block_19, label %ast_block_20
ast_block_19:
  %r132 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r133 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  call void @chali_owned_drop(i64 %r132)
  br label %ast_block_21
ast_block_20:
  br label %ast_block_21
ast_block_21:
  store i64 0, ptr %s_v768___ast_246___chali_reference_storage_1324
  store i8 0, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r134 = load i64, ptr %s_v769___ast_246___chali_reference_storage_1328
  %r135 = load i8, ptr %t_v769___ast_246___chali_reference_storage_1328
  %r136 = icmp ne i64 %r134, 0
  %r137 = zext i1 %r136 to i64
  %r138 = icmp ne i64 %r137, 0
  br i1 %r138, label %ast_block_22, label %ast_block_23
ast_block_22:
  %r139 = load i64, ptr %s_v769___ast_246___chali_reference_storage_1328
  %r140 = load i8, ptr %t_v769___ast_246___chali_reference_storage_1328
  call void @chali_owned_drop(i64 %r139)
  br label %ast_block_24
ast_block_23:
  br label %ast_block_24
ast_block_24:
  store i64 0, ptr %s_v769___ast_246___chali_reference_storage_1328
  store i8 0, ptr %t_v769___ast_246___chali_reference_storage_1328
  store i8 0, ptr @chali_return_float
  ret i64 0
ast_label_v780___chali_cleanup_program__numericFill:
  %r141 = load i64, ptr %s_v769___ast_246___chali_reference_storage_1328
  %r142 = load i8, ptr %t_v769___ast_246___chali_reference_storage_1328
  %r143 = icmp ne i64 %r141, 0
  %r144 = zext i1 %r143 to i64
  %r145 = icmp ne i64 %r144, 0
  br i1 %r145, label %ast_block_25, label %ast_block_26
ast_block_25:
  %r146 = load i64, ptr %s_v769___ast_246___chali_reference_storage_1328
  %r147 = load i8, ptr %t_v769___ast_246___chali_reference_storage_1328
  call void @chali_owned_drop(i64 %r146)
  br label %ast_block_27
ast_block_26:
  br label %ast_block_27
ast_block_27:
  store i64 0, ptr %s_v769___ast_246___chali_reference_storage_1328
  store i8 0, ptr %t_v769___ast_246___chali_reference_storage_1328
  %r148 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r149 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r150 = icmp ne i64 %r148, 0
  %r151 = zext i1 %r150 to i64
  %r152 = icmp ne i64 %r151, 0
  br i1 %r152, label %ast_block_28, label %ast_block_29
ast_block_28:
  %r153 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r154 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  call void @chali_owned_drop(i64 %r153)
  br label %ast_block_30
ast_block_29:
  br label %ast_block_30
ast_block_30:
  store i64 0, ptr %s_v768___ast_246___chali_reference_storage_1324
  store i8 0, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r155 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r156 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r157 = call ptr @chali_primitive_address(i64 %r155, i64 8)
  %r158 = load i64, ptr %r157, align 1
  %r159 = icmp ne i64 %r158, 0
  %r160 = zext i1 %r159 to i64
  %r161 = icmp ne i64 %r160, 0
  br i1 %r161, label %ast_block_31, label %ast_block_32
ast_block_31:
  %r162 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r163 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r164 = call ptr @chali_primitive_address(i64 %r162, i64 8)
  %r165 = load i64, ptr %r164, align 1
  call void @chali_owned_drop(i64 %r165)
  br label %ast_block_33
ast_block_32:
  br label %ast_block_33
ast_block_33:
  %r166 = load i64, ptr %s_v768___ast_246___chali_reference_storage_1324
  %r167 = load i8, ptr %t_v768___ast_246___chali_reference_storage_1324
  %r168 = call ptr @chali_primitive_address(i64 %r166, i64 8)
  store i64 0, ptr %r168, align 1
  store i8 0, ptr @chali_return_float
  ret i64 0
}
