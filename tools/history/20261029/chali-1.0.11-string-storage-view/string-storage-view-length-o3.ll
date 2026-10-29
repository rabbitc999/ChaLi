define internal i64 @f_v56_std__lang____bootStringLength(i64 %p0) #22 section "chali_code" {
entry:
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 164, ptr @chali_source_line, align 8
  %begin.i53.i = load i64, ptr @chali_address_view, align 16
  %heap.not.i54.i = icmp eq i64 %begin.i53.i, 0
  br i1 %heap.not.i54.i, label %lookup1.i, label %check.i55.i

check.i55.i:                                      ; preds = %entry
  %revision_pointer.i.i56.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i57.i = icmp eq ptr %revision_pointer.i.i56.i, null
  br i1 %has_revision.not.i.i57.i, label %lookup1.i, label %check.i.i58.i

check.i.i58.i:                                    ; preds = %check.i55.i
  %revision.i.i59.i = load atomic i64, ptr %revision_pointer.i.i56.i acquire, align 8
  %stamp.i.i60.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i61.i = icmp ne i64 %revision.i.i59.i, %stamp.i.i60.i
  %enabled.i.i62.i = icmp eq i64 %revision.i.i59.i, 0
  %live.i.i63.i.not13 = or i1 %enabled.i.i62.i, %same_revision.i.i61.i
  %begin.i.i64.i = load i64, ptr @chali_address_view, align 16
  %size.i.i65.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i66.i = sub i64 %p0, %begin.i.i64.i
  %inside.i.i67.i = icmp uge i64 %offset.i.i66.i, %size.i.i65.i
  %remaining.i.i68.i = sub i64 %size.i.i65.i, %offset.i.i66.i
  %fits.i.i69.i = icmp ult i64 %remaining.i.i68.i, 32
  %range.i.i70.i.not14 = or i1 %inside.i.i67.i, %fits.i.i69.i
  %valid.i.i71.i.not12 = or i1 %live.i.i63.i.not13, %range.i.i70.i.not14
  %base.i.i76.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i77.i = getelementptr i8, ptr %base.i.i76.i, i64 %offset.i.i66.i
  %found0.not.i = icmp eq ptr %physical.i.i77.i, null
  %or.cond = select i1 %valid.i.i71.i.not12, i1 true, i1 %found0.not.i
  br i1 %or.cond, label %lookup1.i, label %inspect.i

lookup1.i:                                        ; preds = %check.i55.i, %check.i.i58.i, %entry
  %begin.i27.i = load i64, ptr @chali_address_view1, align 16
  %heap.not.i28.i = icmp eq i64 %begin.i27.i, 0
  br i1 %heap.not.i28.i, label %lookup2.i, label %check.i29.i

check.i29.i:                                      ; preds = %lookup1.i
  %revision_pointer.i.i30.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view1, i64 24), align 8
  %has_revision.not.i.i31.i = icmp eq ptr %revision_pointer.i.i30.i, null
  br i1 %has_revision.not.i.i31.i, label %lookup2.i, label %check.i.i32.i

check.i.i32.i:                                    ; preds = %check.i29.i
  %revision.i.i33.i = load atomic i64, ptr %revision_pointer.i.i30.i acquire, align 8
  %stamp.i.i34.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view1, i64 32), align 16
  %same_revision.i.i35.i = icmp ne i64 %revision.i.i33.i, %stamp.i.i34.i
  %enabled.i.i36.i = icmp eq i64 %revision.i.i33.i, 0
  %live.i.i37.i.not16 = or i1 %enabled.i.i36.i, %same_revision.i.i35.i
  %begin.i.i38.i = load i64, ptr @chali_address_view1, align 16
  %size.i.i39.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view1, i64 8), align 8
  %offset.i.i40.i = sub i64 %p0, %begin.i.i38.i
  %inside.i.i41.i = icmp uge i64 %offset.i.i40.i, %size.i.i39.i
  %remaining.i.i42.i = sub i64 %size.i.i39.i, %offset.i.i40.i
  %fits.i.i43.i = icmp ult i64 %remaining.i.i42.i, 32
  %range.i.i44.i.not17 = or i1 %inside.i.i41.i, %fits.i.i43.i
  %valid.i.i45.i.not15 = or i1 %live.i.i37.i.not16, %range.i.i44.i.not17
  %base.i.i50.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view1, i64 16), align 16
  %physical.i.i51.i = getelementptr i8, ptr %base.i.i50.i, i64 %offset.i.i40.i
  %found1.not.i = icmp eq ptr %physical.i.i51.i, null
  %or.cond9 = select i1 %valid.i.i45.i.not15, i1 true, i1 %found1.not.i
  br i1 %or.cond9, label %lookup2.i, label %inspect.i

lookup2.i:                                        ; preds = %check.i29.i, %check.i.i32.i, %lookup1.i
  %begin.i1.i = load i64, ptr @chali_address_view2, align 16
  %heap.not.i2.i = icmp eq i64 %begin.i1.i, 0
  br i1 %heap.not.i2.i, label %lookup3.i, label %check.i3.i

check.i3.i:                                       ; preds = %lookup2.i
  %revision_pointer.i.i4.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view2, i64 24), align 8
  %has_revision.not.i.i5.i = icmp eq ptr %revision_pointer.i.i4.i, null
  br i1 %has_revision.not.i.i5.i, label %lookup3.i, label %check.i.i6.i

check.i.i6.i:                                     ; preds = %check.i3.i
  %revision.i.i7.i = load atomic i64, ptr %revision_pointer.i.i4.i acquire, align 8
  %stamp.i.i8.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view2, i64 32), align 16
  %same_revision.i.i9.i = icmp ne i64 %revision.i.i7.i, %stamp.i.i8.i
  %enabled.i.i10.i = icmp eq i64 %revision.i.i7.i, 0
  %live.i.i11.i.not19 = or i1 %enabled.i.i10.i, %same_revision.i.i9.i
  %begin.i.i12.i = load i64, ptr @chali_address_view2, align 16
  %size.i.i13.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view2, i64 8), align 8
  %offset.i.i14.i = sub i64 %p0, %begin.i.i12.i
  %inside.i.i15.i = icmp uge i64 %offset.i.i14.i, %size.i.i13.i
  %remaining.i.i16.i = sub i64 %size.i.i13.i, %offset.i.i14.i
  %fits.i.i17.i = icmp ult i64 %remaining.i.i16.i, 32
  %range.i.i18.i.not20 = or i1 %inside.i.i15.i, %fits.i.i17.i
  %valid.i.i19.i.not18 = or i1 %live.i.i11.i.not19, %range.i.i18.i.not20
  %base.i.i24.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view2, i64 16), align 16
  %physical.i.i25.i = getelementptr i8, ptr %base.i.i24.i, i64 %offset.i.i14.i
  %found2.not.i = icmp eq ptr %physical.i.i25.i, null
  %or.cond10 = select i1 %valid.i.i19.i.not18, i1 true, i1 %found2.not.i
  br i1 %or.cond10, label %lookup3.i, label %inspect.i

lookup3.i:                                        ; preds = %check.i3.i, %check.i.i6.i, %lookup2.i
  %begin.i.i = load i64, ptr @chali_address_view3, align 16
  %heap.not.i.i = icmp eq i64 %begin.i.i, 0
  br i1 %heap.not.i.i, label %slow.i, label %check.i.i

check.i.i:                                        ; preds = %lookup3.i
  %revision_pointer.i.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view3, i64 24), align 8
  %has_revision.not.i.i.i = icmp eq ptr %revision_pointer.i.i.i, null
  br i1 %has_revision.not.i.i.i, label %slow.i, label %check.i.i.i

check.i.i.i:                                      ; preds = %check.i.i
  %revision.i.i.i = load atomic i64, ptr %revision_pointer.i.i.i acquire, align 8
  %stamp.i.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view3, i64 32), align 16
  %same_revision.i.i.i = icmp ne i64 %revision.i.i.i, %stamp.i.i.i
  %enabled.i.i.i = icmp eq i64 %revision.i.i.i, 0
  %live.i.i.i.not22 = or i1 %enabled.i.i.i, %same_revision.i.i.i
  %begin.i.i.i = load i64, ptr @chali_address_view3, align 16
  %size.i.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view3, i64 8), align 8
  %offset.i.i.i = sub i64 %p0, %begin.i.i.i
  %inside.i.i.i = icmp uge i64 %offset.i.i.i, %size.i.i.i
  %remaining.i.i.i = sub i64 %size.i.i.i, %offset.i.i.i
  %fits.i.i.i = icmp ult i64 %remaining.i.i.i, 32
  %range.i.i.i.not23 = or i1 %inside.i.i.i, %fits.i.i.i
  %valid.i.i.i.not21 = or i1 %live.i.i.i.not22, %range.i.i.i.not23
  %base.i.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view3, i64 16), align 16
  %physical.i.i.i = getelementptr i8, ptr %base.i.i.i, i64 %offset.i.i.i
  %found3.not.i = icmp eq ptr %physical.i.i.i, null
  %or.cond11 = select i1 %valid.i.i.i.not21, i1 true, i1 %found3.not.i
  br i1 %or.cond11, label %slow.i, label %inspect.i

inspect.i:                                        ; preds = %check.i.i.i, %check.i.i6.i, %check.i.i32.i, %check.i.i58.i
  %header.i = phi ptr [ %physical.i.i25.i, %check.i.i6.i ], [ %physical.i.i77.i, %check.i.i58.i ], [ %physical.i.i51.i, %check.i.i32.i ], [ %physical.i.i.i, %check.i.i.i ]
  %marker.i = load i64, ptr %header.i, align 1
  %buffered.i = icmp eq i64 %marker.i, -1
  br i1 %buffered.i, label %select.i, label %slow.i

select.i:                                         ; preds = %inspect.i
  %length_slot.i = getelementptr i8, ptr %header.i, i64 8
  %value.i = load i64, ptr %length_slot.i, align 1
  %valid.i = icmp sgt i64 %value.i, -1
  br i1 %valid.i, label %chali_primitive_string_storage.exit, label %slow.i

slow.i:                                           ; preds = %check.i.i, %check.i.i.i, %lookup3.i, %select.i, %inspect.i
  %next.i.i = load i64, ptr @chali_address_next, align 8
  %first.i.i = icmp eq i64 %next.i.i, 0
  %second.i.i = icmp eq i64 %next.i.i, 1
  %third.i.i = icmp eq i64 %next.i.i, 2
  %tail.i.i = select i1 %third.i.i, ptr @chali_address_view2, ptr @chali_address_view3
  %rest.i.i = select i1 %second.i.i, ptr @chali_address_view1, ptr %tail.i.i
  %target.i.i = select i1 %first.i.i, ptr @chali_address_view, ptr %rest.i.i
  %following.i.i = add nuw nsw i64 %next.i.i, 1
  %wrapped.i.i = and i64 %following.i.i, 3
  store i64 %wrapped.i.i, ptr @chali_address_next, align 8
  %fallback.i = tail call i64 @chali_host_string_storage_view(i64 0, i64 %p0, ptr nonnull %target.i.i)
  br label %chali_primitive_string_storage.exit

chali_primitive_string_storage.exit:              ; preds = %select.i, %slow.i
  %common.ret.op.i = phi i64 [ %fallback.i, %slow.i ], [ %value.i, %select.i ]
  store i8 0, ptr @chali_return_float, align 1
  ret i64 %common.ret.op.i
}
