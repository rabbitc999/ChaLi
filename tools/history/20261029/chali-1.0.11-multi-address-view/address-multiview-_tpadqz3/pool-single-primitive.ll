define internal ptr @chali_primitive_address(i64 %address, i64 %count) alwaysinline {
entry:
  br label %lookup0
lookup0:
  %cached0 = call ptr @chali_cached_view_address(ptr @chali_address_view, i64 %address, i64 %count)
  %found0 = icmp ne ptr %cached0, null
  br i1 %found0, label %hit0, label %slow
hit0:
  ret ptr %cached0
slow:
  %resolved = call ptr @chali_host_address_view(i64 %address, i64 %count, ptr @chali_address_view)

  ret ptr %resolved
}
define void @chali_reset_address_view() {
entry:
  store ptr null, ptr getelementptr ({ i64, i64, ptr, ptr, i64 }, ptr @chali_address_view, i32 0, i32 3)
  store ptr null, ptr getelementptr ({ i64, i64, ptr, ptr, i64 }, ptr @chali_address_view1, i32 0, i32 3)
  store ptr null, ptr getelementptr ({ i64, i64, ptr, ptr, i64 }, ptr @chali_address_view2, i32 0, i32 3)
  store ptr null, ptr getelementptr ({ i64, i64, ptr, ptr, i64 }, ptr @chali_address_view3, i32 0, i32 3)
  store i64 0, ptr @chali_address_next
  ret void
}
