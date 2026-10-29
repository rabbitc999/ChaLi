define internal ptr @chali_primitive_address(i64 %address, i64 %count) alwaysinline {
entry:
  br label %lookup0
lookup0:
  %cached0 = call ptr @chali_cached_view_address(ptr @chali_address_view, i64 %address, i64 %count)
  %found0 = icmp ne ptr %cached0, null
  br i1 %found0, label %hit0, label %lookup1
hit0:
  ret ptr %cached0
lookup1:
  %cached1 = call ptr @chali_cached_view_address(ptr @chali_address_view1, i64 %address, i64 %count)
  %found1 = icmp ne ptr %cached1, null
  br i1 %found1, label %hit1, label %lookup2
hit1:
  ret ptr %cached1
lookup2:
  %cached2 = call ptr @chali_cached_view_address(ptr @chali_address_view2, i64 %address, i64 %count)
  %found2 = icmp ne ptr %cached2, null
  br i1 %found2, label %hit2, label %lookup3
hit2:
  ret ptr %cached2
lookup3:
  %cached3 = call ptr @chali_cached_view_address(ptr @chali_address_view3, i64 %address, i64 %count)
  %found3 = icmp ne ptr %cached3, null
  br i1 %found3, label %hit3, label %slow
hit3:
  ret ptr %cached3
slow:

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
  %resolved = call ptr @chali_host_address_view(i64 %address, i64 %count, ptr %target)

  ret ptr %resolved
}
define void @chali_reset_address_view() {
entry:
  store ptr null, ptr getelementptr ({ i64, i64, ptr, ptr, i64 }, ptr @chali_address_view, i32 0, i32 3)
  store ptr null, ptr getelementptr ({ i64, i64, ptr, ptr, i64 }, ptr @chali_address_view1, i32 0, i32 3)
  store ptr null, ptr getelementptr ({ i64, i64, ptr, ptr, i64 }, ptr @chali_address_view2, i32 0, i32 3)
  store ptr null, ptr getelementptr ({ i64, i64, ptr, ptr, i64 }, ptr @chali_address_view3, i32 0, i32 3)
  store i64 0, ptr @chali_address_next
  store i64 0, ptr @chali_address_secondary_hits
  ret void
}
