define internal ptr @chali_address_target() alwaysinline {
entry:

  %next = load i64, ptr @chali_address_next
  %first = icmp eq i64 %next, 0
  %second = icmp eq i64 %next, 1
  %third = icmp eq i64 %next, 2
  %tail = select i1 %third, ptr @chali_address_view2, ptr @chali_address_view3
  %rest = select i1 %second, ptr @chali_address_view1, ptr %tail
  %target = select i1 %first, ptr @chali_address_view, ptr %rest
  %following = add i64 %next, 1
  %wrapped = and i64 %following, 3
  store i64 %wrapped, ptr @chali_address_next
  ret ptr %target

}

define internal ptr @chali_cached_string_header(ptr %view, i64 %address) alwaysinline {
entry:
  %begin = load i64, ptr %view
  %heap = icmp ne i64 %begin, 0
  br i1 %heap, label %check, label %miss
check:
  %header = call ptr @chali_cached_view_address(ptr %view, i64 %address, i64 32)
  ret ptr %header
miss:
  ret ptr null
}

define internal i64 @chali_primitive_string_storage(i64 %operation, i64 %address) alwaysinline {
entry:
  br label %lookup0
lookup0:
  %header0 = call ptr @chali_cached_string_header(ptr @chali_address_view, i64 %address)
  %found0 = icmp ne ptr %header0, null
  br i1 %found0, label %hit0, label %lookup1
hit0:
  br label %inspect
lookup1:
  %header1 = call ptr @chali_cached_string_header(ptr @chali_address_view1, i64 %address)
  %found1 = icmp ne ptr %header1, null
  br i1 %found1, label %hit1, label %lookup2
hit1:
  br label %inspect
lookup2:
  %header2 = call ptr @chali_cached_string_header(ptr @chali_address_view2, i64 %address)
  %found2 = icmp ne ptr %header2, null
  br i1 %found2, label %hit2, label %lookup3
hit2:
  br label %inspect
lookup3:
  %header3 = call ptr @chali_cached_string_header(ptr @chali_address_view3, i64 %address)
  %found3 = icmp ne ptr %header3, null
  br i1 %found3, label %hit3, label %slow
hit3:
  br label %inspect
inspect:
  %header = phi ptr [ %header0, %hit0 ], [ %header1, %hit1 ], [ %header2, %hit2 ], [ %header3, %hit3 ]
  %marker = load i64, ptr %header, align 1
  %buffered = icmp eq i64 %marker, -1
  br i1 %buffered, label %select, label %slow
select:
  switch i64 %operation, label %slow [ i64 0, label %length
                                     i64 1, label %data
                                     i64 2, label %capacity
                                     i64 3, label %kind ]
length:
  %length_slot = getelementptr i8, ptr %header, i64 8
  br label %read
data:
  %data_slot = getelementptr i8, ptr %header, i64 24
  br label %read
capacity:
  %capacity_slot = getelementptr i8, ptr %header, i64 16
  br label %read
read:
  %slot = phi ptr [ %length_slot, %length ], [ %data_slot, %data ], [ %capacity_slot, %capacity ]
  %value = load i64, ptr %slot, align 1
  %valid = icmp sge i64 %value, 0
  br i1 %valid, label %success, label %slow
success:
  ret i64 %value
kind:
  ret i64 1
slow:
  %target = call ptr @chali_address_target()
  %fallback = call i64 @chali_host_string_storage_view(i64 %operation, i64 %address, ptr %target)
  ret i64 %fallback
}
