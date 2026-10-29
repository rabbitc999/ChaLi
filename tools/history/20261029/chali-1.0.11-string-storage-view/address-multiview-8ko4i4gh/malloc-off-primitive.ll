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
  %hits1 = load i64, ptr @chali_address_secondary_hits
  %increment1 = add i64 %hits1, 1
  %period1 = and i64 %increment1, 31
  store i64 %period1, ptr @chali_address_secondary_hits
  %promote1 = icmp eq i64 %period1, 0
  br i1 %promote1, label %swap1, label %return1
swap1:
  %old1 = load { i64, i64, ptr, ptr, i64 }, ptr @chali_address_view, align 8
  %new1 = load { i64, i64, ptr, ptr, i64 }, ptr @chali_address_view1, align 8
  store { i64, i64, ptr, ptr, i64 } %new1, ptr @chali_address_view, align 8
  store { i64, i64, ptr, ptr, i64 } %old1, ptr @chali_address_view1, align 8
  br label %return1
return1:
  ret ptr %cached1
lookup2:
  %cached2 = call ptr @chali_cached_view_address(ptr @chali_address_view2, i64 %address, i64 %count)
  %found2 = icmp ne ptr %cached2, null
  br i1 %found2, label %hit2, label %lookup3
hit2:
  %hits2 = load i64, ptr @chali_address_secondary_hits
  %increment2 = add i64 %hits2, 1
  %period2 = and i64 %increment2, 31
  store i64 %period2, ptr @chali_address_secondary_hits
  %promote2 = icmp eq i64 %period2, 0
  br i1 %promote2, label %swap2, label %return2
swap2:
  %old2 = load { i64, i64, ptr, ptr, i64 }, ptr @chali_address_view, align 8
  %new2 = load { i64, i64, ptr, ptr, i64 }, ptr @chali_address_view2, align 8
  store { i64, i64, ptr, ptr, i64 } %new2, ptr @chali_address_view, align 8
  store { i64, i64, ptr, ptr, i64 } %old2, ptr @chali_address_view2, align 8
  br label %return2
return2:
  ret ptr %cached2
lookup3:
  %cached3 = call ptr @chali_cached_view_address(ptr @chali_address_view3, i64 %address, i64 %count)
  %found3 = icmp ne ptr %cached3, null
  br i1 %found3, label %hit3, label %slow
hit3:
  %hits3 = load i64, ptr @chali_address_secondary_hits
  %increment3 = add i64 %hits3, 1
  %period3 = and i64 %increment3, 31
  store i64 %period3, ptr @chali_address_secondary_hits
  %promote3 = icmp eq i64 %period3, 0
  br i1 %promote3, label %swap3, label %return3
swap3:
  %old3 = load { i64, i64, ptr, ptr, i64 }, ptr @chali_address_view, align 8
  %new3 = load { i64, i64, ptr, ptr, i64 }, ptr @chali_address_view3, align 8
  store { i64, i64, ptr, ptr, i64 } %new3, ptr @chali_address_view, align 8
  store { i64, i64, ptr, ptr, i64 } %old3, ptr @chali_address_view3, align 8
  br label %return3
return3:
  ret ptr %cached3
slow:
  %target = call ptr @chali_address_target()
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
