; ModuleID = 'tmp/buffer-ir.ll'
source_filename = "chali-aot"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-w64-windows-gnu"

@.str = private unnamed_addr constant [15 x i8] c"chali aot: %s\0A\00", align 1
@.str.1 = private unnamed_addr constant [55 x i8] c"\E7\A7\BB\E4\BD\8D\E4\BD\8D\E6\95\B0\E8\B6\85\E5\87\BA\E6\95\B4\E6\95\B0\E4\BD\8D\E5\AE\BD\EF\BC\88\E6\9C\89\E6\95\88\E8\8C\83\E5\9B\B4 0..63\EF\BC\89\00", align 1
@.str.2 = private unnamed_addr constant [13 x i8] c"\E6\B5\AE\E7\82\B9\E9\99\A4\E9\9B\B6\00", align 1
@.str.3 = private unnamed_addr constant [13 x i8] c"\E6\95\B4\E6\95\B0\E9\99\A4\E9\9B\B6\00", align 1
@.str.4 = private unnamed_addr constant [72 x i8] c"\E6\95\B4\E6\95\B0\E9\99\A4\E6\B3\95\E6\BA\A2\E5\87\BA\EF\BC\9A\E6\9C\80\E5\B0\8F\E8\B4\9F\E6\95\B0\E9\99\A4\E4\BB\A5 -1 \E7\9A\84\E7\BB\93\E6\9E\9C\E8\B6\85\E5\87\BA int64 \E8\8C\83\E5\9B\B4\00", align 1
@.str.5 = private unnamed_addr constant [31 x i8] c"\E5\A4\8D\E5\88\B6\E5\AD\97\E7\AC\A6\E4\B8\B2\E6\97\B6\E5\86\85\E5\AD\98\E4\B8\8D\E8\B6\B3\00", align 1
@.str.6 = private unnamed_addr constant [37 x i8] c"%s\E8\B6\8A\E7\95\8C\EF\BC\9A\E4\B8\8B\E6\A0\87 %lld\EF\BC\8C\E9\95\BF\E5\BA\A6 %lld\00", align 1
@.str.7 = private unnamed_addr constant [15 x i8] c"%s\EF\BC\9A\E7\A9\BA\E6\8C\87\E9\92\88\00", align 1
@chali_fmt_at = internal unnamed_addr global i64 0, align 8
@chali_fmt_pos = internal unnamed_addr global i64 0, align 8
@.str.9 = private unnamed_addr constant [8 x i8] c"hlLqjzt\00", align 1
@.str.10 = private unnamed_addr constant [6 x i8] c"%s.*s\00", align 1
@.str.11 = private unnamed_addr constant [9 x i8] c"gGeEfFaA\00", align 1
@.str.12 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str.13 = private unnamed_addr constant [3 x i8] c".3\00", align 1
@.str.14 = private unnamed_addr constant [5 x i8] c"%s%c\00", align 1
@.str.15 = private unnamed_addr constant [7 x i8] c"%sll%c\00", align 1
@.str.16 = private unnamed_addr constant [6 x i8] c"%slld\00", align 1
@.str.18 = private unnamed_addr constant [34 x i8] c"\E8\AF\BB\E5\8F\96\E6\A0\87\E5\87\86\E8\BE\93\E5\85\A5\E6\97\B6\E5\86\85\E5\AD\98\E4\B8\8D\E8\B6\B3\00", align 1
@.str.19 = private unnamed_addr constant [65 x i8] c"readLine \E4\B8\8D\E6\8E\A5\E5\8F\97\E9\9B\B6\E5\AD\97\E8\8A\82\EF\BC\8CString \E4\B8\8D\E8\83\BD\E5\8C\85\E5\90\AB\E5\86\85\E5\B5\8C\E9\9B\B6\E5\AD\97\E8\8A\82\00", align 1
@.str.20 = private unnamed_addr constant [22 x i8] c"\E6\A0\87\E5\87\86\E8\BE\93\E5\85\A5\E8\A1\8C\E8\BF\87\E9\95\BF\00", align 1
@.str.21 = private unnamed_addr constant [5 x i8] c"%.*f\00", align 1
@.str.22 = private unnamed_addr constant [25 x i8] c"\E6\B5\AE\E7\82\B9\E6\95\B0\E6\A0\BC\E5\BC\8F\E5\8C\96\E5\A4\B1\E8\B4\A5\00", align 1
@.str.23 = private unnamed_addr constant [4 x i8] c"%lf\00", align 1
@.str.24 = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@.str.25 = private unnamed_addr constant [3 x i8] c"%g\00", align 1
@.str.26 = private unnamed_addr constant [7 x i8] c"\E4\B8\8B\E6\A0\87\00", align 1
@.str.27 = private unnamed_addr constant [7 x i8] c"\E8\AE\BF\E9\97\AE\00", align 1
@.str.28 = private unnamed_addr constant [59 x i8] c"\E5\BC\95\E7\94\A8\E5\B0\9A\E6\9C\AA\E7\BB\91\E5\AE\9A\E6\9C\89\E6\95\88\E5\85\83\E7\B4\A0\EF\BC\9B\E7\BC\BA\E7\9C\81\E5\80\BC\E8\AF\B7\E4\BD\BF\E7\94\A8 Option\00", align 1
@.str.29 = private unnamed_addr constant [6 x i8] c"panic\00", align 1
@chali_address_view = internal thread_local global { i64, i64, ptr, ptr, i64 } zeroinitializer
@chali_return_float = internal thread_local unnamed_addr global i8 0
@chali_source_function = internal thread_local unnamed_addr global ptr null
@chali_source_file = internal thread_local unnamed_addr global ptr null
@chali_source_line = internal thread_local unnamed_addr global i64 0
@chali_threads_enabled = local_unnamed_addr constant i32 1
@chali_step_limit = local_unnamed_addr constant i64 0
@chali_function_count = local_unnamed_addr constant i64 0
@chali_function_addresses = local_unnamed_addr constant [0 x i64] poison
@chali_data_layout_size = local_unnamed_addr constant i64 160
@chali_data_layout = local_unnamed_addr constant [161 x i8] c"CHLY\01\0A\0Aast_data_0\08\03\06\08\0Aast_data_1\10\03\07\08\0Aast_data_2\18\03\06\08\0Aast_data_3 \035\08\0Aast_data_4X\03\1C\08\0Aast_data_5x\03\1C\08\0Aast_data_6\98\01\03\16\08\0Aast_data_7\B0\01\03\17\08\0Aast_data_8\C8\01\03\08\08\0Aast_data_9\D0\01\03\08\08\00"
@chali_data_image = local_unnamed_addr constant [224 x i8] c"\00\00\00\00\00\00\00\00first\00\00\00second\00\00third\00\00\00bounded buffer growth and retained List capacity OK\0A\00\00\00\00\E7\BC\93\E5\86\B2\E5\8C\BA\E5\AE\B9\E9\87\8F\E5\8F\82\E6\95\B0\E6\97\A0\E6\95\88\00\00\00\00\00\E7\BC\93\E5\86\B2\E5\8C\BA\E5\AE\B9\E9\87\8F\E8\B6\85\E5\87\BA\E4\B8\8A\E9\99\90\00\00\00\00\00String\EF\BC\9A\E7\BB\93\E6\9E\9C\E8\BF\87\E9\95\BF\00\00\00assert(x) \E7\9A\84 x \E4\B8\BA\E5\81\87\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00"
@chali_data_segment_count = local_unnamed_addr constant i64 1
@chali_data_segments = local_unnamed_addr constant [1 x { i64, i64, i64 }] [{ i64, i64, i64 } { i64 0, i64 0, i64 224 }]
@chali_data_size = local_unnamed_addr constant i64 224
@chali_heap_size = local_unnamed_addr constant i64 536870912
@chali_allocator_kind = local_unnamed_addr constant i32 1
@source_file0 = private constant [19 x i8] c"<\E5\86\85\E5\BB\BA std::lang>\00"
@source_file1 = private constant [37 x i8] c"tests/fixtures/aot_buffer_growth.chl\00"
@trace_anchor_name = private constant [1 x i8] zeroinitializer
@trace_name0 = private constant [28 x i8] c"std::lang::__bootStringByte\00"
@trace_name1 = private constant [30 x i8] c"std::lang::__bootStringLength\00"
@trace_name2 = private constant [30 x i8] c"std::lang::__bootStringBuffer\00"
@trace_name3 = private constant [33 x i8] c"std::lang::__bootStringCopyBytes\00"
@trace_name4 = private constant [28 x i8] c"std::lang::__bootStringCopy\00"
@trace_name5 = private constant [28 x i8] c"std::lang::__bootStringDrop\00"
@trace_name6 = private constant [31 x i8] c"std::lang::__bootStringCompare\00"
@trace_name7 = private constant [23 x i8] c"std::lang::assert$bool\00"
@trace_name8 = private constant [41 x i8] c"std::lang::assert$bool$std::lang::String\00"
@trace_name9 = private constant [32 x i8] c"std::lang::__bootBufferCapacity\00"
@trace_name10 = private constant [29 x i8] c"std::lang::__bootListReserve\00"
@trace_name11 = private constant [28 x i8] c"std::lang::__bootListRemove\00"
@trace_name12 = private constant [27 x i8] c"std::lang::__bootListClear\00"
@trace_name13 = private constant [26 x i8] c"std::lang::__bootListCopy\00"
@trace_name14 = private constant [27 x i8] c"std::lang::__bootListPushI\00"
@trace_name15 = private constant [26 x i8] c"std::lang::__bootListPopI\00"
@trace_name16 = private constant [26 x i8] c"program::referenceAddress\00"
@trace_name17 = private constant [5 x i8] c"main\00"
@trace_name18 = private constant [37 x i8] c"std::lang::pointerFromAddress__int64\00"
@trace_name19 = private constant [37 x i8] c"std::lang::pointerFromAddress__uint8\00"
@trace_name20 = private constant [20 x i8] c"__chali_copy_struct\00"
@chali_trace_count = local_unnamed_addr constant i64 22
@chali_trace_functions = local_unnamed_addr constant [22 x ptr] [ptr @chali_code_anchor, ptr @f_v49_std__lang____bootStringByte, ptr @f_v56_std__lang____bootStringLength, ptr @f_v57_std__lang____bootStringBuffer, ptr @f_v58_std__lang____bootStringCopyBytes, ptr @f_v60_std__lang____bootStringCopy, ptr @f_v61_std__lang____bootStringDrop, ptr @f_v64_std__lang____bootStringCompare, ptr @f_v162_std__lang__assert_bool, ptr @f_v163_std__lang__assert_bool_std__lang__String, ptr @f_v169_std__lang____bootBufferCapacity, ptr @f_v170_std__lang____bootListReserve, ptr @f_v171_std__lang____bootListRemove, ptr @f_v172_std__lang____bootListClear, ptr @f_v173_std__lang____bootListCopy, ptr @f_v174_std__lang____bootListPushI, ptr @f_v176_std__lang____bootListPopI, ptr @f_v235_program__referenceAddress, ptr @f_main, ptr @f_v271_std__lang__pointerFromAddress__int64, ptr @f_v273_std__lang__pointerFromAddress__uint8, ptr @f_v468___chali_copy_struct]
@chali_trace_names = local_unnamed_addr constant [22 x ptr] [ptr @trace_anchor_name, ptr @trace_name0, ptr @trace_name1, ptr @trace_name2, ptr @trace_name3, ptr @trace_name4, ptr @trace_name5, ptr @trace_name6, ptr @trace_name7, ptr @trace_name8, ptr @trace_name9, ptr @trace_name10, ptr @trace_name11, ptr @trace_name12, ptr @trace_name13, ptr @trace_name14, ptr @trace_name15, ptr @trace_name16, ptr @trace_name17, ptr @trace_name18, ptr @trace_name19, ptr @trace_name20]
@native_name1 = private constant [17 x i8] c"__std_move_begin\00"
@native_name2 = private constant [15 x i8] c"__std_move_end\00"
@native_name0 = private constant [12 x i8] c"__std_panic\00"
@chali_abi_lit0 = private constant [52 x i8] c"bounded buffer growth and retained List capacity OK\0A"

; Function Attrs: nounwind uwtable
define dso_local ptr @chali_standard_stream(i32 noundef %0) local_unnamed_addr #0 {
  %2 = icmp eq i32 %0, 1
  %3 = select i1 %2, i32 1, i32 2
  %4 = icmp eq i32 %0, 0
  %5 = select i1 %4, i32 0, i32 %3
  %6 = tail call ptr @__acrt_iob_func(i32 noundef %5) #26
  ret ptr %6
}

declare dllimport ptr @__acrt_iob_func(i32 noundef) local_unnamed_addr #1

; Function Attrs: cold noreturn nounwind uwtable
define dso_local void @chali_panic(ptr noundef %0) local_unnamed_addr #2 {
  %2 = tail call ptr @__acrt_iob_func(i32 noundef 1) #26
  %3 = tail call i32 @fflush(ptr noundef %2)
  %4 = tail call ptr @__acrt_iob_func(i32 noundef 2) #26
  %5 = tail call i32 (ptr, ptr, ...) @fprintf(ptr noundef %4, ptr noundef nonnull @.str, ptr noundef %0) #26
  tail call void @chali_host_trace() #26
  tail call void @exit(i32 noundef 1) #27
  unreachable
}

; Function Attrs: nofree nounwind
declare dso_local noundef i32 @fflush(ptr noundef captures(none)) local_unnamed_addr #3

; Function Attrs: nofree nounwind
declare dso_local noundef i32 @fprintf(ptr noundef captures(none), ptr noundef readonly captures(none), ...) local_unnamed_addr #3

declare dso_local void @chali_host_trace() local_unnamed_addr #1

; Function Attrs: nofree noreturn nounwind
declare dso_local void @exit(i32 noundef) local_unnamed_addr #4

; Function Attrs: nounwind uwtable
define dso_local void @chali_check_shift(i64 noundef %0) local_unnamed_addr #0 {
  %2 = icmp ugt i64 %0, 63
  br i1 %2, label %3, label %4

3:                                                ; preds = %1
  tail call void @chali_panic(ptr noundef nonnull @.str.1)
  unreachable

4:                                                ; preds = %1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_check_real_divisor(double noundef %0) local_unnamed_addr #0 {
  %2 = fcmp oeq double %0, 0.000000e+00
  br i1 %2, label %3, label %4

3:                                                ; preds = %1
  tail call void @chali_panic(ptr noundef nonnull @.str.2)
  unreachable

4:                                                ; preds = %1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_integer_divide(i64 noundef %0, i64 noundef %1, i32 noundef %2, i32 noundef %3) local_unnamed_addr #0 {
  %5 = icmp eq i64 %1, 0
  br i1 %5, label %6, label %7

6:                                                ; preds = %4
  tail call void @chali_panic(ptr noundef nonnull @.str.3)
  unreachable

7:                                                ; preds = %4
  %8 = icmp eq i32 %2, 0
  br i1 %8, label %14, label %9

9:                                                ; preds = %7
  %10 = icmp eq i32 %3, 0
  %11 = udiv i64 %0, %1
  %12 = urem i64 %0, %1
  %13 = select i1 %10, i64 %11, i64 %12
  br label %25

14:                                               ; preds = %7
  %15 = icmp eq i64 %0, -9223372036854775808
  %16 = icmp eq i64 %1, -1
  %17 = and i1 %15, %16
  %18 = icmp eq i32 %3, 0
  br i1 %17, label %19, label %21

19:                                               ; preds = %14
  br i1 %18, label %20, label %25

20:                                               ; preds = %19
  tail call void @chali_panic(ptr noundef nonnull @.str.4)
  unreachable

21:                                               ; preds = %14
  %22 = sdiv i64 %0, %1
  %23 = srem i64 %0, %1
  %24 = select i1 %18, i64 %22, i64 %23
  br label %25

25:                                               ; preds = %21, %19, %9
  %26 = phi i64 [ 0, %19 ], [ %24, %21 ], [ %13, %9 ]
  ret i64 %26
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_init() local_unnamed_addr #0 {
  tail call void @chali_host_init() #26
  ret void
}

declare dso_local void @chali_host_init() local_unnamed_addr #1

; Function Attrs: nounwind uwtable
define dso_local void @chali_finish() local_unnamed_addr #0 {
  %1 = tail call ptr @__acrt_iob_func(i32 noundef 1) #26
  %2 = tail call i32 @fflush(ptr noundef %1)
  tail call void @chali_host_finish() #26
  ret void
}

declare dso_local void @chali_host_finish() local_unnamed_addr #1

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_alloc(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call i64 @chali_host_allocate(i64 noundef %0) #26
  ret i64 %2
}

declare dso_local i64 @chali_host_allocate(i64 noundef) local_unnamed_addr #1

; Function Attrs: nounwind uwtable
define dso_local void @chali_drop(i64 noundef %0) local_unnamed_addr #0 {
  tail call void @chali_host_drop(i64 noundef %0) #26
  ret void
}

declare dso_local void @chali_host_drop(i64 noundef) local_unnamed_addr #1

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_load_i64(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #26
  %3 = load i64, ptr %2, align 1
  ret i64 %3
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(ptr captures(none)) #5

declare dso_local ptr @chali_host_address(i64 noundef, i64 noundef) local_unnamed_addr #1

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias writeonly captures(none), ptr noalias readonly captures(none), i64, i1 immarg) #5

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(ptr captures(none)) #5

; Function Attrs: nounwind uwtable
define dso_local void @chali_store_i64(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #26
  store i64 %1, ptr %3, align 1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local double @chali_load_f64(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #26
  %3 = load double, ptr %2, align 1
  ret double %3
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_store_f64(i64 noundef %0, double noundef %1) local_unnamed_addr #0 {
  %3 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #26
  store double %1, ptr %3, align 1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 0, 256) i64 @chali_load_u8(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #26
  %3 = load i8, ptr %2, align 1, !tbaa !12
  %4 = zext i8 %3 to i64
  ret i64 %4
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_store_u8(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = trunc i64 %1 to i8
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #26
  store i8 %3, ptr %4, align 1, !tbaa !12
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 -2147483648, 2147483648) i64 @chali_load_i32(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 4) #26
  %3 = load i32, ptr %2, align 1
  %4 = sext i32 %3 to i64
  ret i64 %4
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_store_i32(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = trunc i64 %1 to i32
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 4) #26
  store i32 %3, ptr %4, align 1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 -32768, 32768) i64 @chali_load_i16(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 2) #26
  %3 = load i16, ptr %2, align 1
  %4 = sext i16 %3 to i64
  ret i64 %4
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_store_i16(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = trunc i64 %1 to i16
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 2) #26
  store i16 %3, ptr %4, align 1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_new(ptr nofree noundef readonly captures(none) %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = icmp eq i64 %1, 0
  %4 = tail call i64 @llvm.umax.i64(i64 %1, i64 1)
  %5 = tail call ptr @malloc(i64 noundef %4) #28
  %6 = icmp eq ptr %5, null
  br i1 %6, label %7, label %8

7:                                                ; preds = %2
  tail call void @chali_panic(ptr noundef nonnull @.str.5)
  unreachable

8:                                                ; preds = %2
  br i1 %3, label %10, label %9

9:                                                ; preds = %8
  tail call void @llvm.memcpy.p0.p0.i64(ptr nonnull align 1 %5, ptr align 1 %0, i64 %1, i1 false)
  br label %10

10:                                               ; preds = %9, %8
  %11 = add i64 %1, 9
  %12 = tail call i64 @chali_host_allocate(i64 noundef %11) #26
  %13 = tail call ptr @chali_host_address(i64 noundef %12, i64 noundef 8) #26
  store i64 %1, ptr %13, align 1
  br i1 %3, label %17, label %14

14:                                               ; preds = %10
  %15 = add nsw i64 %12, 8
  %16 = tail call ptr @chali_host_address(i64 noundef %15, i64 noundef %1) #26
  tail call void @llvm.memcpy.p0.p0.i64(ptr align 1 %16, ptr nonnull align 1 %5, i64 %1, i1 false)
  br label %17

17:                                               ; preds = %14, %10
  %18 = add i64 %1, 8
  %19 = add i64 %18, %12
  %20 = tail call ptr @chali_host_address(i64 noundef %19, i64 noundef 1) #26
  store i8 0, ptr %20, align 1, !tbaa !12
  tail call void @free(ptr noundef nonnull %5)
  ret i64 %12
}

; Function Attrs: mustprogress nofree nounwind willreturn allockind("alloc,uninitialized") allocsize(0) memory(inaccessiblemem: readwrite, errnomem: write)
declare dso_local noalias noundef ptr @malloc(i64 noundef) local_unnamed_addr #6

; Function Attrs: mustprogress nounwind willreturn allockind("free") memory(argmem: readwrite, inaccessiblemem: readwrite)
declare dso_local void @free(ptr allocptr noundef captures(none)) local_unnamed_addr #7

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable
define dso_local range(i32 0, 2) i32 @chali_on_heap(i64 noundef %0) local_unnamed_addr #8 {
  %2 = icmp sgt i64 %0, 223
  %3 = zext i1 %2 to i32
  ret i32 %3
}

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_string_length(i64 noundef %0) local_unnamed_addr #0 {
  %2 = icmp eq i64 %0, 0
  br i1 %2, label %11, label %3

3:                                                ; preds = %1
  %4 = icmp slt i64 %0, 224
  br i1 %4, label %8, label %5

5:                                                ; preds = %3
  %6 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #26
  %7 = load i64, ptr %6, align 1
  br label %11

8:                                                ; preds = %3
  %9 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #26
  %10 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %9) #26
  br label %11

11:                                               ; preds = %8, %5, %1
  %12 = phi i64 [ 0, %1 ], [ %7, %5 ], [ %10, %8 ]
  ret i64 %12
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare dso_local i64 @strlen(ptr noundef captures(none)) local_unnamed_addr #9

; Function Attrs: nounwind uwtable
define dso_local ptr @chali_string_bytes(i64 noundef %0) local_unnamed_addr #0 {
  %2 = icmp slt i64 %0, 224
  %3 = select i1 %2, i64 0, i64 8
  %4 = add nsw i64 %3, %0
  %5 = tail call ptr @chali_host_address(i64 noundef %4, i64 noundef 0) #26
  ret ptr %5
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_literal(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #26
  %3 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %2) #26
  %4 = tail call i64 @chali_string_new(ptr noundef nonnull %2, i64 noundef %3)
  ret i64 %4
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_copy(i64 noundef %0) local_unnamed_addr #0 {
  %2 = icmp eq i64 %0, 0
  br i1 %2, label %17, label %3

3:                                                ; preds = %1
  %4 = icmp slt i64 %0, 224
  %5 = select i1 %4, i64 0, i64 8
  %6 = add nsw i64 %5, %0
  %7 = tail call ptr @chali_host_address(i64 noundef %6, i64 noundef 0) #26
  br i1 %4, label %11, label %8

8:                                                ; preds = %3
  %9 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #26
  %10 = load i64, ptr %9, align 1
  br label %14

11:                                               ; preds = %3
  %12 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #26
  %13 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %12) #26
  br label %14

14:                                               ; preds = %11, %8
  %15 = phi i64 [ %13, %11 ], [ %10, %8 ]
  %16 = tail call i64 @chali_string_new(ptr noundef %7, i64 noundef %15)
  br label %17

17:                                               ; preds = %14, %1
  %18 = phi i64 [ %16, %14 ], [ 0, %1 ]
  ret i64 %18
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_string_drop(i64 noundef %0) local_unnamed_addr #0 {
  tail call void @chali_host_drop(i64 noundef %0) #26
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 -9223372036854775800, -9223372036854775808) i64 @chali_new_array(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = tail call i64 @llvm.smax.i64(i64 %0, i64 0)
  %4 = mul i64 %3, %1
  %5 = add i64 %4, 8
  %6 = tail call i64 @chali_host_allocate(i64 noundef %5) #26
  %7 = tail call ptr @chali_host_address(i64 noundef %6, i64 noundef 8) #26
  store i64 %3, ptr %7, align 1
  %8 = add nsw i64 %6, 8
  ret i64 %8
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_bounds(i64 noundef %0, i64 noundef %1, ptr noundef %2) local_unnamed_addr #0 {
  %4 = alloca [128 x i8], align 16
  %5 = icmp sgt i64 %0, -1
  %6 = icmp slt i64 %0, %1
  %7 = and i1 %5, %6
  br i1 %7, label %10, label %8

8:                                                ; preds = %3
  call void @llvm.lifetime.start.p0(ptr nonnull %4) #26
  %9 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %4, i64 noundef 128, ptr noundef nonnull @.str.6, ptr noundef %2, i64 noundef %0, i64 noundef %1) #26
  call void @chali_panic(ptr noundef nonnull %4)
  unreachable

10:                                               ; preds = %3
  ret void
}

; Function Attrs: nofree nounwind
declare dso_local noundef i32 @snprintf(ptr noalias noundef writeonly captures(none), i64 noundef, ptr noundef readonly captures(none), ...) local_unnamed_addr #3

; Function Attrs: nounwind uwtable
define dso_local void @chali_null(i64 noundef %0, ptr noundef %1) local_unnamed_addr #0 {
  %3 = alloca [128 x i8], align 16
  %4 = icmp eq i64 %0, 0
  br i1 %4, label %5, label %7

5:                                                ; preds = %2
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #26
  %6 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i64 noundef 128, ptr noundef nonnull @.str.7, ptr noundef %1) #26
  call void @chali_panic(ptr noundef nonnull %3)
  unreachable

7:                                                ; preds = %2
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none, target_mem: none) uwtable
define dso_local void @chali_fmt_begin(i64 noundef %0) local_unnamed_addr #10 {
  store i64 %0, ptr @chali_fmt_at, align 8, !tbaa !13
  store i64 0, ptr @chali_fmt_pos, align 8, !tbaa !13
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i32 0, 256) i32 @chali_fmt_scan(ptr nofree noundef writeonly captures(none) %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = ptrtoaddr ptr %0 to i64
  %4 = load i64, ptr @chali_fmt_at, align 8, !tbaa !13
  %5 = icmp slt i64 %4, 224
  %6 = select i1 %5, i64 0, i64 8
  %7 = add nsw i64 %6, %4
  %8 = tail call ptr @chali_host_address(i64 noundef %7, i64 noundef 0) #26
  %9 = ptrtoaddr ptr %8 to i64
  %10 = load i64, ptr @chali_fmt_pos, align 8, !tbaa !13
  br label %11

11:                                               ; preds = %19, %2
  %indvar = phi i64 [ %indvar.next, %19 ], [ 0, %2 ]
  %12 = phi i64 [ %20, %19 ], [ %10, %2 ]
  %13 = getelementptr inbounds nuw i8, ptr %8, i64 %12
  %14 = load i8, ptr %13, align 1, !tbaa !12
  switch i8 %14, label %19 [
    i8 0, label %21
    i8 37, label %21
    i8 123, label %15
  ]

15:                                               ; preds = %11
  %16 = getelementptr i8, ptr %13, i64 1
  %17 = load i8, ptr %16, align 1, !tbaa !12
  %18 = icmp eq i8 %17, 125
  br i1 %18, label %21, label %19

19:                                               ; preds = %15, %11
  %20 = add i64 %12, 1
  %indvar.next = add i64 %indvar, 1
  br label %11, !llvm.loop !15

21:                                               ; preds = %15, %11, %11
  %22 = icmp ugt i64 %12, %10
  br i1 %22, label %23, label %28

23:                                               ; preds = %21
  %24 = getelementptr inbounds nuw i8, ptr %8, i64 %10
  %25 = sub nuw i64 %12, %10
  %26 = tail call ptr @__acrt_iob_func(i32 noundef 1) #26
  %27 = tail call i64 @fwrite(ptr noundef %24, i64 noundef 1, i64 noundef %25, ptr noundef %26)
  store i64 %12, ptr @chali_fmt_pos, align 8, !tbaa !13
  %.pre = load i8, ptr %13, align 1, !tbaa !12
  br label %28

28:                                               ; preds = %23, %21
  %29 = phi i8 [ %.pre, %23 ], [ %14, %21 ]
  switch i8 %29, label %96 [
    i8 0, label %30
    i8 123, label %31
  ]

30:                                               ; preds = %28
  store i8 0, ptr %0, align 1, !tbaa !12
  br label %145

31:                                               ; preds = %28
  %32 = add i64 %12, 1
  br label %33

33:                                               ; preds = %37, %31
  %indvar72 = phi i64 [ %indvar.next73, %37 ], [ 0, %31 ]
  %34 = phi i64 [ %38, %37 ], [ %32, %31 ]
  %35 = getelementptr inbounds nuw i8, ptr %8, i64 %34
  %36 = load i8, ptr %35, align 1, !tbaa !12
  switch i8 %36, label %37 [
    i8 0, label %39
    i8 125, label %39
  ]

37:                                               ; preds = %33
  %38 = add i64 %34, 1
  %indvar.next73 = add i64 %indvar72, 1
  br label %33, !llvm.loop !18

39:                                               ; preds = %33, %33
  %40 = getelementptr inbounds nuw i8, ptr %8, i64 %34
  %41 = icmp eq i8 %36, 125
  %42 = zext i1 %41 to i64
  %43 = add i64 %34, %42
  store i64 %43, ptr @chali_fmt_pos, align 8, !tbaa !13
  store i8 37, ptr %0, align 1, !tbaa !12
  %44 = getelementptr inbounds nuw i8, ptr %0, i64 1
  store i8 0, ptr %44, align 1, !tbaa !12
  %45 = getelementptr inbounds nuw i8, ptr %8, i64 %32
  %46 = load i8, ptr %45, align 1, !tbaa !12
  %47 = icmp eq i8 %46, 58
  %48 = add i64 %12, 2
  %49 = select i1 %47, i64 %48, i64 %32
  %50 = icmp ugt i64 %34, %49
  br i1 %50, label %51, label %145

51:                                               ; preds = %39
  %52 = add nuw i64 %49, 1
  %53 = icmp ult i64 %52, %34
  %54 = icmp ugt i64 %1, 3
  %55 = and i1 %54, %53
  br i1 %55, label %iter.check, label %.loopexit16

iter.check:                                       ; preds = %51
  %56 = add i64 %10, -1
  %57 = add i64 %56, %indvar
  %58 = add i64 %57, %indvar72
  %59 = sub i64 %58, %49
  %60 = add i64 %1, -4
  %umin = tail call i64 @llvm.umin.i64(i64 %59, i64 %60)
  %61 = add i64 %umin, 1
  %min.iters.check = icmp ult i64 %61, 4
  br i1 %min.iters.check, label %.preheader15.preheader, label %vector.memcheck

vector.memcheck:                                  ; preds = %iter.check
  %62 = add i64 %49, %9
  %63 = sub i64 %3, %62
  %diff.check = icmp ult i64 %63, 31
  br i1 %diff.check, label %.preheader15.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %vector.memcheck
  %min.iters.check74 = icmp ult i64 %61, 16
  br i1 %min.iters.check74, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.mod.vf = and i64 %61, 12
  %n.vec = and i64 %61, -16
  %64 = add i64 %52, %n.vec
  %65 = add i64 %49, %n.vec
  %66 = or disjoint i64 %n.vec, 1
  %67 = getelementptr i8, ptr %8, i64 %49
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %68 = getelementptr i8, ptr %67, i64 %index
  %wide.load = load <16 x i8>, ptr %68, align 1, !tbaa !12
  %69 = getelementptr inbounds nuw i8, ptr %0, i64 %index
  %70 = getelementptr inbounds nuw i8, ptr %69, i64 1
  store <16 x i8> %wide.load, ptr %70, align 1, !tbaa !12
  %index.next = add nuw i64 %index, 16
  %71 = icmp eq i64 %index.next, %n.vec
  br i1 %71, label %middle.block, label %vector.body, !llvm.loop !19

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i64 %61, %n.vec
  br i1 %cmp.n, label %.loopexit16, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %min.epilog.iters.check = icmp eq i64 %n.mod.vf, 0
  br i1 %min.epilog.iters.check, label %.preheader15.preheader, label %vec.epilog.ph, !prof !22

vec.epilog.ph:                                    ; preds = %vector.main.loop.iter.check, %vec.epilog.iter.check
  %vec.epilog.resume.val = phi i64 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec78 = and i64 %61, -4
  %72 = add i64 %52, %n.vec78
  %73 = add i64 %49, %n.vec78
  %74 = or disjoint i64 %n.vec78, 1
  %75 = getelementptr i8, ptr %8, i64 %49
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index79 = phi i64 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next81, %vec.epilog.vector.body ]
  %76 = getelementptr i8, ptr %75, i64 %index79
  %wide.load80 = load <4 x i8>, ptr %76, align 1, !tbaa !12
  %77 = getelementptr inbounds nuw i8, ptr %0, i64 %index79
  %78 = getelementptr inbounds nuw i8, ptr %77, i64 1
  store <4 x i8> %wide.load80, ptr %78, align 1, !tbaa !12
  %index.next81 = add nuw i64 %index79, 4
  %79 = icmp eq i64 %index.next81, %n.vec78
  br i1 %79, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !23

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %cmp.n82 = icmp eq i64 %61, %n.vec78
  br i1 %cmp.n82, label %.loopexit16, label %.preheader15.preheader

.preheader15.preheader:                           ; preds = %vector.memcheck, %iter.check, %vec.epilog.iter.check, %vec.epilog.middle.block
  %.ph93 = phi i64 [ %52, %iter.check ], [ %52, %vector.memcheck ], [ %64, %vec.epilog.iter.check ], [ %72, %vec.epilog.middle.block ]
  %.ph94 = phi i64 [ %49, %iter.check ], [ %49, %vector.memcheck ], [ %65, %vec.epilog.iter.check ], [ %73, %vec.epilog.middle.block ]
  %.ph95 = phi i64 [ 1, %iter.check ], [ 1, %vector.memcheck ], [ %66, %vec.epilog.iter.check ], [ %74, %vec.epilog.middle.block ]
  br label %.preheader15

.loopexit16:                                      ; preds = %.preheader15, %middle.block, %vec.epilog.middle.block, %51
  %80 = phi i64 [ 1, %51 ], [ %74, %vec.epilog.middle.block ], [ %66, %middle.block ], [ %89, %.preheader15 ]
  %81 = getelementptr inbounds nuw i8, ptr %0, i64 %80
  store i8 0, ptr %81, align 1, !tbaa !12
  %82 = getelementptr i8, ptr %40, i64 -1
  %83 = load i8, ptr %82, align 1, !tbaa !12
  br label %145

.preheader15:                                     ; preds = %.preheader15.preheader, %.preheader15
  %84 = phi i64 [ %91, %.preheader15 ], [ %.ph93, %.preheader15.preheader ]
  %85 = phi i64 [ %84, %.preheader15 ], [ %.ph94, %.preheader15.preheader ]
  %86 = phi i64 [ %89, %.preheader15 ], [ %.ph95, %.preheader15.preheader ]
  %87 = getelementptr inbounds nuw i8, ptr %8, i64 %85
  %88 = load i8, ptr %87, align 1, !tbaa !12
  %89 = add nuw i64 %86, 1
  %90 = getelementptr inbounds nuw i8, ptr %0, i64 %86
  store i8 %88, ptr %90, align 1, !tbaa !12
  %91 = add nuw i64 %84, 1
  %92 = icmp ult i64 %91, %34
  %93 = add nuw i64 %86, 3
  %94 = icmp ult i64 %93, %1
  %95 = and i1 %92, %94
  br i1 %95, label %.preheader15, label %.loopexit16, !llvm.loop !24

96:                                               ; preds = %28
  %97 = add i64 %12, 1
  %98 = getelementptr inbounds nuw i8, ptr %8, i64 %97
  %99 = load i8, ptr %98, align 1, !tbaa !12
  %100 = icmp eq i8 %99, 37
  br i1 %100, label %101, label %105

101:                                              ; preds = %96
  %102 = tail call ptr @__acrt_iob_func(i32 noundef 1) #26
  %103 = tail call i32 @fputc(i32 noundef 37, ptr noundef %102)
  %104 = add i64 %12, 2
  store i64 %104, ptr @chali_fmt_pos, align 8, !tbaa !13
  store i8 0, ptr %0, align 1, !tbaa !12
  br label %145

105:                                              ; preds = %96
  store i8 37, ptr %0, align 1, !tbaa !12
  %106 = load i8, ptr %98, align 1, !tbaa !12
  %107 = icmp eq i8 %106, 0
  br i1 %107, label %.loopexit, label %.preheader11

.preheader11:                                     ; preds = %105, %119
  %.pre30.pre = phi i8 [ %124, %119 ], [ %106, %105 ]
  %108 = phi i64 [ %121, %119 ], [ 1, %105 ]
  %109 = phi i64 [ %120, %119 ], [ %97, %105 ]
  %110 = zext nneg i8 %.pre30.pre to i64
  %111 = icmp ult i8 %.pre30.pre, 64
  %112 = shl nuw i64 1, %110
  %113 = and i64 %112, 288063289038995457
  %114 = icmp ne i64 %113, 0
  %115 = select i1 %111, i1 %114, i1 false
  %116 = add nuw i64 %108, 2
  %117 = icmp ult i64 %116, %1
  %118 = select i1 %115, i1 %117, i1 false
  br i1 %118, label %119, label %.preheader

119:                                              ; preds = %.preheader11
  %120 = add i64 %109, 1
  %121 = add nuw i64 %108, 1
  %122 = getelementptr inbounds nuw i8, ptr %0, i64 %108
  store i8 %.pre30.pre, ptr %122, align 1, !tbaa !12
  %123 = getelementptr inbounds nuw i8, ptr %8, i64 %120
  %124 = load i8, ptr %123, align 1, !tbaa !12
  %125 = icmp eq i8 %124, 0
  br i1 %125, label %.loopexit, label %.preheader11, !llvm.loop !25

.preheader:                                       ; preds = %.preheader11, %131
  %126 = phi i8 [ %134, %131 ], [ %.pre30.pre, %.preheader11 ]
  %127 = phi i64 [ %132, %131 ], [ %109, %.preheader11 ]
  %128 = sext i8 %126 to i32
  %129 = tail call ptr @memchr(ptr noundef nonnull dereferenceable(1) @.str.9, i32 %128, i64 8)
  %130 = icmp eq ptr %129, null
  br i1 %130, label %.loopexit, label %131

131:                                              ; preds = %.preheader
  %132 = add i64 %127, 1
  %133 = getelementptr inbounds nuw i8, ptr %8, i64 %132
  %134 = load i8, ptr %133, align 1, !tbaa !12
  %135 = icmp eq i8 %134, 0
  br i1 %135, label %.loopexit, label %.preheader, !llvm.loop !26

.loopexit:                                        ; preds = %119, %131, %.preheader, %105
  %136 = phi i64 [ %108, %131 ], [ 1, %105 ], [ %108, %.preheader ], [ %121, %119 ]
  %137 = phi i64 [ %132, %131 ], [ %97, %105 ], [ %127, %.preheader ], [ %120, %119 ]
  %138 = getelementptr inbounds nuw i8, ptr %8, i64 %137
  %139 = getelementptr inbounds nuw i8, ptr %0, i64 %136
  store i8 0, ptr %139, align 1, !tbaa !12
  %140 = load i8, ptr %138, align 1, !tbaa !12
  %141 = icmp eq i8 %140, 0
  br i1 %141, label %142, label %143

142:                                              ; preds = %.loopexit
  store i64 %137, ptr @chali_fmt_pos, align 8, !tbaa !13
  br label %145

143:                                              ; preds = %.loopexit
  %144 = add i64 %137, 1
  store i64 %144, ptr @chali_fmt_pos, align 8, !tbaa !13
  br label %145

145:                                              ; preds = %143, %142, %101, %.loopexit16, %39, %30
  %146 = phi i8 [ 0, %30 ], [ 63, %39 ], [ %83, %.loopexit16 ], [ 37, %101 ], [ 0, %142 ], [ %140, %143 ]
  %147 = zext i8 %146 to i32
  ret i32 %147
}

; Function Attrs: nofree nounwind
declare dso_local noundef i64 @fwrite(ptr noundef readonly captures(none), i64 noundef, i64 noundef, ptr noundef captures(none)) local_unnamed_addr #3

; Function Attrs: nofree nounwind
declare dso_local noundef i32 @fputc(i32 noundef, ptr noundef captures(none)) local_unnamed_addr #3

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare dso_local ptr @strchr(ptr noundef, i32 noundef) local_unnamed_addr #9

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_emit(i32 noundef %0, ptr noundef %1, i64 noundef %2, i32 noundef %3) local_unnamed_addr #0 {
  %5 = alloca [64 x i8], align 16
  %6 = alloca [40 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %5) #26
  call void @llvm.lifetime.start.p0(ptr nonnull %6) #26
  switch i32 %0, label %33 [
    i32 115, label %7
    i32 99, label %25
    i32 63, label %30
  ]

7:                                                ; preds = %4
  %8 = icmp eq i64 %2, 0
  br i1 %8, label %58, label %9

9:                                                ; preds = %7
  %10 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef 64, ptr noundef nonnull @.str.10, ptr noundef %1) #26
  %11 = icmp slt i64 %2, 224
  br i1 %11, label %15, label %12

12:                                               ; preds = %9
  %13 = tail call ptr @chali_host_address(i64 noundef %2, i64 noundef 8) #26
  %14 = load i64, ptr %13, align 1
  br label %18

15:                                               ; preds = %9
  %16 = tail call ptr @chali_host_address(i64 noundef %2, i64 noundef 1) #26
  %17 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %16) #26
  br label %18

18:                                               ; preds = %15, %12
  %19 = phi i64 [ 0, %15 ], [ 8, %12 ]
  %20 = phi i64 [ %17, %15 ], [ %14, %12 ]
  %21 = trunc i64 %20 to i32
  %22 = add nsw i64 %19, %2
  %23 = tail call ptr @chali_host_address(i64 noundef %22, i64 noundef 0) #26
  %24 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %5, i32 noundef %21, ptr noundef %23)
  br label %58

25:                                               ; preds = %4
  %26 = trunc i64 %2 to i32
  %27 = and i32 %26, 255
  %28 = tail call ptr @__acrt_iob_func(i32 noundef 1) #26
  %29 = tail call i32 @fputc(i32 noundef %27, ptr noundef %28)
  br label %58

30:                                               ; preds = %4
  %31 = icmp eq i32 %3, 0
  %32 = select i1 %31, i32 100, i32 103
  br label %33

33:                                               ; preds = %30, %4
  %34 = phi i32 [ %32, %30 ], [ %0, %4 ]
  %35 = tail call ptr @memchr(ptr noundef nonnull dereferenceable(1) @.str.11, i32 %34, i64 9)
  %36 = icmp eq ptr %35, null
  br i1 %36, label %51, label %37

37:                                               ; preds = %33
  %38 = bitcast i64 %2 to double
  %39 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %6, i64 noundef 40, ptr noundef nonnull @.str.12, ptr noundef %1) #26
  %40 = call ptr @strchr(ptr noundef nonnull dereferenceable(1) %6, i32 noundef 46) #26
  %41 = icmp eq ptr %40, null
  br i1 %41, label %42, label %46

42:                                               ; preds = %37
  %43 = call i64 @strlen(ptr noundef nonnull dereferenceable(1) %6) #26
  %44 = sub i64 39, %43
  %45 = call ptr @strncat(ptr noundef nonnull dereferenceable(1) %6, ptr noundef nonnull @.str.13, i64 noundef %44) #26
  br label %46

46:                                               ; preds = %42, %37
  %47 = shl i32 %34, 24
  %48 = ashr exact i32 %47, 24
  %49 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef 64, ptr noundef nonnull @.str.14, ptr noundef nonnull %6, i32 noundef %48) #26
  %50 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %5, double noundef %38)
  br label %58

51:                                               ; preds = %33
  switch i32 %34, label %55 [
    i32 120, label %52
    i32 117, label %52
    i32 111, label %52
    i32 88, label %52
  ]

52:                                               ; preds = %51, %51, %51, %51
  %53 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef 64, ptr noundef nonnull @.str.15, ptr noundef %1, i32 noundef %34) #26
  %54 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef %2)
  br label %58

55:                                               ; preds = %51
  %56 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef 64, ptr noundef nonnull @.str.16, ptr noundef %1) #26
  %57 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef %2)
  br label %58

58:                                               ; preds = %55, %52, %46, %25, %18, %7
  call void @llvm.lifetime.end.p0(ptr nonnull %6) #26
  call void @llvm.lifetime.end.p0(ptr nonnull %5) #26
  ret void
}

; Function Attrs: nofree nounwind
declare dso_local noundef i32 @printf(ptr noundef readonly captures(none), ...) local_unnamed_addr #3

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare dso_local ptr @strncat(ptr noalias noundef returned, ptr noalias noundef readonly captures(none), i64 noundef) local_unnamed_addr #11

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_int(i64 noundef %0) local_unnamed_addr #0 {
  %2 = alloca [32 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #26
  %3 = call i32 @chali_fmt_scan(ptr noundef nonnull %2, i64 noundef 32)
  %4 = trunc nuw i32 %3 to i8
  switch i8 %4, label %5 [
    i8 37, label %6
    i8 0, label %6
  ]

5:                                                ; preds = %1
  call void @chali_fmt_emit(i32 noundef %3, ptr noundef nonnull %2, i64 noundef %0, i32 noundef 0)
  br label %6

6:                                                ; preds = %5, %1, %1
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #26
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_uint(i64 noundef %0) local_unnamed_addr #0 {
  %2 = alloca [32 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #26
  %3 = call i32 @chali_fmt_scan(ptr noundef nonnull %2, i64 noundef 32)
  %4 = trunc nuw i32 %3 to i8
  switch i8 %4, label %5 [
    i8 37, label %6
    i8 0, label %6
  ]

5:                                                ; preds = %1
  call void @chali_fmt_emit(i32 noundef %3, ptr noundef nonnull %2, i64 noundef %0, i32 noundef 0)
  br label %6

6:                                                ; preds = %5, %1, %1
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #26
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_char(i64 noundef %0) local_unnamed_addr #0 {
  %2 = alloca [32 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #26
  %3 = call i32 @chali_fmt_scan(ptr noundef nonnull %2, i64 noundef 32)
  %4 = trunc nuw i32 %3 to i8
  switch i8 %4, label %5 [
    i8 37, label %6
    i8 0, label %6
  ]

5:                                                ; preds = %1
  call void @chali_fmt_emit(i32 noundef %3, ptr noundef nonnull %2, i64 noundef %0, i32 noundef 0)
  br label %6

6:                                                ; preds = %5, %1, %1
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #26
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_double(double noundef %0) local_unnamed_addr #0 {
  %2 = alloca [32 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #26
  %3 = call i32 @chali_fmt_scan(ptr noundef nonnull %2, i64 noundef 32)
  %4 = trunc nuw i32 %3 to i8
  switch i8 %4, label %5 [
    i8 37, label %7
    i8 0, label %7
  ]

5:                                                ; preds = %1
  %6 = bitcast double %0 to i64
  call void @chali_fmt_emit(i32 noundef %3, ptr noundef nonnull %2, i64 noundef %6, i32 noundef 1)
  br label %7

7:                                                ; preds = %5, %1, %1
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #26
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_string(i64 noundef %0) local_unnamed_addr #0 {
  %2 = alloca [32 x i8], align 16
  %3 = alloca [48 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #26
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #26
  %4 = call i32 @chali_fmt_scan(ptr noundef nonnull %2, i64 noundef 32)
  %5 = icmp eq i64 %0, 0
  br i1 %5, label %24, label %6

6:                                                ; preds = %1
  %7 = trunc nuw i32 %4 to i8
  switch i8 %7, label %8 [
    i8 37, label %24
    i8 0, label %24
  ]

8:                                                ; preds = %6
  %9 = icmp slt i64 %0, 224
  %10 = select i1 %9, i64 0, i64 8
  %11 = add nsw i64 %10, %0
  %12 = tail call ptr @chali_host_address(i64 noundef %11, i64 noundef 0) #26
  br i1 %9, label %16, label %13

13:                                               ; preds = %8
  %14 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #26
  %15 = load i64, ptr %14, align 1
  br label %19

16:                                               ; preds = %8
  %17 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #26
  %18 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %17) #26
  br label %19

19:                                               ; preds = %16, %13
  %20 = phi i64 [ %18, %16 ], [ %15, %13 ]
  %21 = trunc i64 %20 to i32
  %22 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i64 noundef 48, ptr noundef nonnull @.str.10, ptr noundef nonnull %2) #26
  %23 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %3, i32 noundef %21, ptr noundef %12)
  br label %24

24:                                               ; preds = %19, %6, %6, %1
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #26
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #26
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_finish() local_unnamed_addr #0 {
  %1 = load i64, ptr @chali_fmt_at, align 8, !tbaa !13
  %2 = icmp slt i64 %1, 224
  %3 = select i1 %2, i64 0, i64 8
  %4 = add nsw i64 %3, %1
  %5 = tail call ptr @chali_host_address(i64 noundef %4, i64 noundef 0) #26
  %6 = load i64, ptr @chali_fmt_pos, align 8, !tbaa !13
  br label %7

7:                                                ; preds = %15, %0
  %8 = phi i64 [ %6, %0 ], [ %20, %15 ]
  %9 = getelementptr inbounds nuw i8, ptr %5, i64 %8
  %10 = load i8, ptr %9, align 1, !tbaa !12
  switch i8 %10, label %21 [
    i8 0, label %23
    i8 37, label %11
  ]

11:                                               ; preds = %7
  %12 = getelementptr i8, ptr %9, i64 1
  %13 = load i8, ptr %12, align 1, !tbaa !12
  %14 = icmp eq i8 %13, 37
  br i1 %14, label %15, label %21

15:                                               ; preds = %21, %11
  %16 = phi i32 [ %22, %21 ], [ 37, %11 ]
  %17 = phi i64 [ 1, %21 ], [ 2, %11 ]
  %18 = tail call ptr @__acrt_iob_func(i32 noundef 1) #26
  %19 = tail call i32 @fputc(i32 noundef %16, ptr noundef %18)
  %20 = add i64 %17, %8
  br label %7, !llvm.loop !27

21:                                               ; preds = %11, %7
  %22 = sext i8 %10 to i32
  br label %15

23:                                               ; preds = %7
  store i64 %8, ptr @chali_fmt_pos, align 8, !tbaa !13
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 -1, 2) i64 @chali_string_cmp(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = icmp eq i64 %0, 0
  br i1 %3, label %12, label %4

4:                                                ; preds = %2
  %5 = icmp slt i64 %0, 224
  br i1 %5, label %9, label %6

6:                                                ; preds = %4
  %7 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #26
  %8 = load i64, ptr %7, align 1
  br label %12

9:                                                ; preds = %4
  %10 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #26
  %11 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %10) #26
  br label %12

12:                                               ; preds = %9, %6, %2
  %13 = phi i64 [ 0, %2 ], [ %11, %9 ], [ %8, %6 ]
  %14 = icmp eq i64 %1, 0
  br i1 %14, label %23, label %15

15:                                               ; preds = %12
  %16 = icmp slt i64 %1, 224
  br i1 %16, label %20, label %17

17:                                               ; preds = %15
  %18 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 8) #26
  %19 = load i64, ptr %18, align 1
  br label %23

20:                                               ; preds = %15
  %21 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 1) #26
  %22 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %21) #26
  br label %23

23:                                               ; preds = %20, %17, %12
  %24 = phi i64 [ 0, %12 ], [ %22, %20 ], [ %19, %17 ]
  %25 = tail call i64 @llvm.umin.i64(i64 %13, i64 %24)
  %26 = icmp eq i64 %25, 0
  br i1 %26, label %.thread, label %27

27:                                               ; preds = %23
  %28 = icmp slt i64 %0, 224
  %29 = select i1 %28, i64 0, i64 8
  %30 = add nsw i64 %29, %0
  %31 = tail call ptr @chali_host_address(i64 noundef %30, i64 noundef 0) #26
  %32 = icmp slt i64 %1, 224
  %33 = select i1 %32, i64 0, i64 8
  %34 = add nsw i64 %33, %1
  %35 = tail call ptr @chali_host_address(i64 noundef %34, i64 noundef 0) #26
  %36 = tail call i32 @memcmp(ptr noundef %31, ptr noundef %35, i64 noundef %25)
  %37 = icmp eq i32 %36, 0
  br i1 %37, label %.thread, label %38

38:                                               ; preds = %27
  %39 = icmp sgt i32 %36, -1
  %40 = select i1 %39, i64 1, i64 -1
  br label %42

.thread:                                          ; preds = %23, %27
  %41 = tail call i64 @llvm.ucmp.i64.i64(i64 %13, i64 %24)
  br label %42

42:                                               ; preds = %.thread, %38
  %43 = phi i64 [ %40, %38 ], [ %41, %.thread ]
  ret i64 %43
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare dso_local i32 @memcmp(ptr noundef captures(none), ptr noundef captures(none), i64 noundef) local_unnamed_addr #9

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_sub(i64 noundef %0, i64 noundef %1, i64 noundef %2) local_unnamed_addr #0 {
  %4 = icmp eq i64 %0, 0
  br i1 %4, label %5, label %10

5:                                                ; preds = %3
  %6 = tail call i64 @chali_host_allocate(i64 noundef 9) #26
  %7 = tail call ptr @chali_host_address(i64 noundef %6, i64 noundef 8) #26
  store i64 0, ptr %7, align 1
  %8 = add i64 %6, 8
  %9 = tail call ptr @chali_host_address(i64 noundef %8, i64 noundef 1) #26
  store i8 0, ptr %9, align 1, !tbaa !12
  br label %30

10:                                               ; preds = %3
  %11 = icmp slt i64 %0, 224
  br i1 %11, label %15, label %12

12:                                               ; preds = %10
  %13 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #26
  %14 = load i64, ptr %13, align 1
  br label %18

15:                                               ; preds = %10
  %16 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #26
  %17 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %16) #26
  br label %18

18:                                               ; preds = %15, %12
  %19 = phi i64 [ 0, %15 ], [ 8, %12 ]
  %20 = phi i64 [ %17, %15 ], [ %14, %12 ]
  %21 = tail call i64 @llvm.smax.i64(i64 %1, i64 0)
  %22 = tail call i64 @llvm.smax.i64(i64 %2, i64 0)
  %23 = tail call i64 @llvm.smin.i64(i64 %21, i64 %20)
  %24 = sub nsw i64 %20, %23
  %25 = tail call i64 @llvm.smin.i64(i64 %22, i64 %24)
  %26 = add nsw i64 %19, %0
  %27 = tail call ptr @chali_host_address(i64 noundef %26, i64 noundef 0) #26
  %28 = getelementptr inbounds i8, ptr %27, i64 %23
  %29 = tail call i64 @chali_string_new(ptr noundef %28, i64 noundef %25)
  br label %30

30:                                               ; preds = %18, %5
  %31 = phi i64 [ %6, %5 ], [ %29, %18 ]
  ret i64 %31
}

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_string_pos(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = icmp eq i64 %0, 0
  %4 = icmp eq i64 %1, 0
  %5 = or i1 %3, %4
  br i1 %5, label %.loopexit, label %6

6:                                                ; preds = %2
  %7 = icmp slt i64 %0, 224
  br i1 %7, label %11, label %8

8:                                                ; preds = %6
  %9 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #26
  %10 = load i64, ptr %9, align 1
  br label %14

11:                                               ; preds = %6
  %12 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #26
  %13 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %12) #26
  br label %14

14:                                               ; preds = %11, %8
  %15 = phi i64 [ %13, %11 ], [ %10, %8 ]
  %16 = icmp slt i64 %1, 224
  br i1 %16, label %20, label %17

17:                                               ; preds = %14
  %18 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 8) #26
  %19 = load i64, ptr %18, align 1
  br label %23

20:                                               ; preds = %14
  %21 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 1) #26
  %22 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %21) #26
  br label %23

23:                                               ; preds = %20, %17
  %24 = phi i64 [ %22, %20 ], [ %19, %17 ]
  %25 = icmp eq i64 %24, 0
  br i1 %25, label %.loopexit, label %26

26:                                               ; preds = %23
  %27 = icmp sgt i64 %24, %15
  br i1 %27, label %.loopexit, label %28

28:                                               ; preds = %26
  %29 = select i1 %7, i64 0, i64 8
  %30 = add nsw i64 %29, %0
  %31 = select i1 %16, i64 0, i64 8
  %32 = add nsw i64 %31, %1
  br label %33

33:                                               ; preds = %40, %28
  %34 = phi i64 [ 0, %28 ], [ %41, %40 ]
  %35 = tail call ptr @chali_host_address(i64 noundef %30, i64 noundef 0) #26
  %36 = getelementptr inbounds nuw i8, ptr %35, i64 %34
  %37 = tail call ptr @chali_host_address(i64 noundef %32, i64 noundef 0) #26
  %38 = tail call i32 @memcmp(ptr noundef %36, ptr noundef %37, i64 noundef %24)
  %39 = icmp eq i32 %38, 0
  br i1 %39, label %.loopexit, label %40

40:                                               ; preds = %33
  %41 = add nuw nsw i64 %34, 1
  %42 = add nsw i64 %41, %24
  %43 = icmp sgt i64 %42, %15
  br i1 %43, label %.loopexit, label %33, !llvm.loop !28

.loopexit:                                        ; preds = %40, %33, %26, %23, %2
  %44 = phi i64 [ -1, %2 ], [ 0, %23 ], [ -1, %26 ], [ %34, %33 ], [ -1, %40 ]
  ret i64 %44
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_read_line() local_unnamed_addr #0 {
  %1 = tail call dereferenceable_or_null(256) ptr @malloc(i64 noundef 256) #28
  %2 = icmp eq ptr %1, null
  br i1 %2, label %3, label %.preheader

3:                                                ; preds = %0
  tail call void @chali_panic(ptr noundef nonnull @.str.18)
  unreachable

.preheader:                                       ; preds = %0, %20
  %4 = phi ptr [ %21, %20 ], [ %1, %0 ]
  %5 = phi i64 [ %24, %20 ], [ 0, %0 ]
  %6 = phi i64 [ %22, %20 ], [ 256, %0 ]
  %7 = tail call ptr @__acrt_iob_func(i32 noundef 0) #26
  %8 = tail call i32 @fgetc(ptr noundef %7)
  switch i32 %8, label %10 [
    i32 -1, label %26
    i32 10, label %26
    i32 0, label %9
  ]

9:                                                ; preds = %.preheader
  tail call void @free(ptr noundef %4)
  tail call void @chali_panic(ptr noundef nonnull @.str.19)
  unreachable

10:                                               ; preds = %.preheader
  %11 = icmp eq i64 %5, %6
  br i1 %11, label %12, label %20

12:                                               ; preds = %10
  %13 = icmp slt i64 %5, 0
  br i1 %13, label %14, label %15

14:                                               ; preds = %12
  tail call void @chali_panic(ptr noundef nonnull @.str.20)
  unreachable

15:                                               ; preds = %12
  %16 = shl nuw i64 %5, 1
  %17 = tail call ptr @realloc(ptr noundef %4, i64 noundef %16) #29
  %18 = icmp eq ptr %17, null
  br i1 %18, label %19, label %20

19:                                               ; preds = %15
  tail call void @free(ptr noundef %4)
  tail call void @chali_panic(ptr noundef nonnull @.str.18)
  unreachable

20:                                               ; preds = %15, %10
  %21 = phi ptr [ %4, %10 ], [ %17, %15 ]
  %22 = phi i64 [ %6, %10 ], [ %16, %15 ]
  %23 = trunc i32 %8 to i8
  %24 = add i64 %5, 1
  %25 = getelementptr inbounds nuw i8, ptr %21, i64 %5
  store i8 %23, ptr %25, align 1, !tbaa !12
  br label %.preheader, !llvm.loop !29

26:                                               ; preds = %.preheader, %.preheader
  %27 = icmp eq i32 %8, 10
  %28 = icmp ne i64 %5, 0
  %29 = select i1 %27, i1 %28, i1 false
  br i1 %29, label %30, label %37

30:                                               ; preds = %26
  %31 = getelementptr i8, ptr %4, i64 %5
  %32 = getelementptr i8, ptr %31, i64 -1
  %33 = load i8, ptr %32, align 1, !tbaa !12
  %34 = icmp eq i8 %33, 13
  %35 = sext i1 %34 to i64
  %36 = add i64 %5, %35
  br label %37

37:                                               ; preds = %30, %26
  %38 = phi i64 [ %5, %26 ], [ %36, %30 ]
  %39 = tail call i64 @chali_string_new(ptr noundef %4, i64 noundef %38)
  tail call void @free(ptr noundef %4)
  ret i64 %39
}

; Function Attrs: nofree nounwind
declare dso_local noundef i32 @fgetc(ptr noundef captures(none)) local_unnamed_addr #3

; Function Attrs: mustprogress nounwind willreturn allockind("realloc") allocsize(1) memory(argmem: readwrite, inaccessiblemem: readwrite, errnomem: write)
declare dso_local noalias noundef ptr @realloc(ptr allocptr noundef captures(none), i64 noundef) local_unnamed_addr #12

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_fixed(double noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = alloca [256 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #26
  %4 = icmp slt i64 %1, 0
  %5 = trunc i64 %1 to i32
  %6 = select i1 %4, i32 3, i32 %5
  %7 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i64 noundef 256, ptr noundef nonnull @.str.21, i32 noundef %6, double noundef %0) #26
  %8 = icmp slt i32 %7, 0
  br i1 %8, label %9, label %10

9:                                                ; preds = %2
  tail call void @chali_panic(ptr noundef nonnull @.str.22)
  unreachable

10:                                               ; preds = %2
  %11 = tail call i32 @llvm.umin.i32(i32 %7, i32 255)
  %12 = zext nneg i32 %11 to i64
  %13 = call i64 @chali_string_new(ptr noundef nonnull %3, i64 noundef %12)
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #26
  ret i64 %13
}

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_read_number(i32 noundef %0) local_unnamed_addr #0 {
  %2 = alloca [256 x i8], align 16
  %3 = alloca double, align 8
  %4 = alloca i64, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #26
  %5 = tail call ptr @__acrt_iob_func(i32 noundef 0) #26
  %6 = call ptr @fgets(ptr noundef nonnull %2, i32 noundef 256, ptr noundef %5)
  %7 = icmp eq ptr %6, null
  br i1 %7, label %18, label %8

8:                                                ; preds = %1
  %9 = icmp eq i32 %0, 0
  br i1 %9, label %14, label %10

10:                                               ; preds = %8
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #26
  store double 0.000000e+00, ptr %3, align 8, !tbaa !30
  %11 = call i32 (ptr, ptr, ...) @sscanf(ptr noundef nonnull %2, ptr noundef nonnull @.str.23, ptr noundef nonnull %3)
  %12 = icmp eq i32 %11, 1
  %.pre = load i64, ptr %3, align 8
  %13 = select i1 %12, i64 %.pre, i64 0
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #26
  br label %18

14:                                               ; preds = %8
  call void @llvm.lifetime.start.p0(ptr nonnull %4) #26
  store i64 0, ptr %4, align 8, !tbaa !13
  %15 = call i32 (ptr, ptr, ...) @sscanf(ptr noundef nonnull %2, ptr noundef nonnull @.str.24, ptr noundef nonnull %4)
  %16 = icmp eq i32 %15, 1
  %.pre2 = load i64, ptr %4, align 8
  %17 = select i1 %16, i64 %.pre2, i64 0
  call void @llvm.lifetime.end.p0(ptr nonnull %4) #26
  br label %18

18:                                               ; preds = %14, %10, %1
  %19 = phi i64 [ %17, %14 ], [ %13, %10 ], [ 0, %1 ]
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #26
  ret i64 %19
}

; Function Attrs: nofree nounwind
declare dso_local noundef ptr @fgets(ptr noundef writeonly, i32 noundef, ptr noundef captures(none)) local_unnamed_addr #3

; Function Attrs: nofree nounwind
declare dso_local noundef i32 @sscanf(ptr noundef readonly captures(none), ptr noundef readonly captures(none), ...) local_unnamed_addr #3

; Function Attrs: nounwind uwtable
define dso_local range(i64 0, 2) i64 @chali_stdin_eof() local_unnamed_addr #0 {
  %1 = tail call ptr @__acrt_iob_func(i32 noundef 0) #26
  %2 = tail call i32 @feof(ptr noundef %1)
  %3 = icmp ne i32 %2, 0
  %4 = zext i1 %3 to i64
  ret i64 %4
}

; Function Attrs: nofree nounwind
declare dso_local noundef i32 @feof(ptr noundef captures(none)) local_unnamed_addr #3

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_of_int(i64 noundef %0) local_unnamed_addr #0 {
  %2 = alloca [32 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #26
  %3 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %2, i64 noundef 32, ptr noundef nonnull @.str.24, i64 noundef %0) #26
  %4 = call i64 @strlen(ptr noundef nonnull dereferenceable(1) %2) #26
  %5 = call i64 @chali_string_new(ptr noundef nonnull %2, i64 noundef %4)
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #26
  ret i64 %5
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_of_double(double noundef %0) local_unnamed_addr #0 {
  %2 = alloca [64 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #26
  %3 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %2, i64 noundef 64, ptr noundef nonnull @.str.25, double noundef %0) #26
  %4 = call i64 @strlen(ptr noundef nonnull dereferenceable(1) %2) #26
  %5 = call i64 @chali_string_new(ptr noundef nonnull %2, i64 noundef %4)
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #26
  ret i64 %5
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_check(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = alloca [128 x i8], align 16
  %4 = icmp sgt i64 %0, -1
  %5 = icmp slt i64 %0, %1
  %6 = and i1 %4, %5
  br i1 %6, label %9, label %7

7:                                                ; preds = %2
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #26
  %8 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i64 noundef 128, ptr noundef nonnull @.str.6, ptr noundef nonnull @.str.26, i64 noundef %0, i64 noundef %1) #26
  call void @chali_panic(ptr noundef nonnull %3)
  unreachable

9:                                                ; preds = %2
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_check_pointer(i64 noundef %0) local_unnamed_addr #0 {
  %2 = alloca [128 x i8], align 16
  %3 = icmp eq i64 %0, 0
  br i1 %3, label %4, label %6

4:                                                ; preds = %1
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #26
  %5 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %2, i64 noundef 128, ptr noundef nonnull @.str.7, ptr noundef nonnull @.str.27) #26
  call void @chali_panic(ptr noundef nonnull %2)
  unreachable

6:                                                ; preds = %1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 1, 0) i64 @chali_require_reference(i64 noundef returned %0) local_unnamed_addr #0 {
  %2 = icmp eq i64 %0, 0
  br i1 %2, label %3, label %4

3:                                                ; preds = %1
  tail call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

4:                                                ; preds = %1
  ret i64 %0
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_put_char(i64 noundef %0) local_unnamed_addr #0 {
  %2 = trunc i64 %0 to i32
  %3 = and i32 %2, 255
  %4 = tail call ptr @__acrt_iob_func(i32 noundef 1) #26
  %5 = tail call i32 @fputc(i32 noundef %3, ptr noundef %4)
  ret void
}

; Function Attrs: noreturn nounwind uwtable
define dso_local void @chali_abort(i64 noundef %0) local_unnamed_addr #13 {
  %2 = icmp eq i64 %0, 0
  br i1 %2, label %25, label %3

3:                                                ; preds = %1
  store i64 %0, ptr @chali_fmt_at, align 8, !tbaa !13
  store i64 0, ptr @chali_fmt_pos, align 8, !tbaa !13
  %4 = icmp slt i64 %0, 224
  %5 = select i1 %4, i64 0, i64 8
  %6 = add nsw i64 %5, %0
  %7 = tail call ptr @chali_host_address(i64 noundef %6, i64 noundef 0) #26
  %8 = load i64, ptr @chali_fmt_pos, align 8, !tbaa !13
  br label %9

9:                                                ; preds = %17, %3
  %10 = phi i64 [ %8, %3 ], [ %22, %17 ]
  %11 = getelementptr inbounds nuw i8, ptr %7, i64 %10
  %12 = load i8, ptr %11, align 1, !tbaa !12
  switch i8 %12, label %23 [
    i8 0, label %chali_fmt_finish.exit
    i8 37, label %13
  ]

13:                                               ; preds = %9
  %14 = getelementptr i8, ptr %11, i64 1
  %15 = load i8, ptr %14, align 1, !tbaa !12
  %16 = icmp eq i8 %15, 37
  br i1 %16, label %17, label %23

17:                                               ; preds = %23, %13
  %18 = phi i32 [ %24, %23 ], [ 37, %13 ]
  %19 = phi i64 [ 1, %23 ], [ 2, %13 ]
  %20 = tail call ptr @__acrt_iob_func(i32 noundef 1) #26
  %21 = tail call i32 @fputc(i32 noundef %18, ptr noundef %20)
  %22 = add i64 %19, %10
  br label %9, !llvm.loop !27

23:                                               ; preds = %13, %9
  %24 = sext i8 %12 to i32
  br label %17

chali_fmt_finish.exit:                            ; preds = %9
  store i64 %10, ptr @chali_fmt_pos, align 8, !tbaa !13
  br label %25

25:                                               ; preds = %chali_fmt_finish.exit, %1
  tail call void @chali_panic(ptr noundef nonnull @.str.29)
  unreachable
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_assert(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = icmp eq i64 %0, 0
  br i1 %3, label %4, label %8

4:                                                ; preds = %2
  %5 = icmp eq i64 %1, 0
  br i1 %5, label %7, label %6

6:                                                ; preds = %4
  store i64 %1, ptr @chali_fmt_at, align 8, !tbaa !13
  store i64 0, ptr @chali_fmt_pos, align 8, !tbaa !13
  tail call void @chali_fmt_finish()
  br label %7

7:                                                ; preds = %6, %4
  tail call void @chali_panic(ptr noundef nonnull @.str.29)
  unreachable

8:                                                ; preds = %2
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_copy_bytes(i64 noundef returned %0, i64 noundef %1, i64 noundef %2) local_unnamed_addr #0 {
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef %2) #26
  %5 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef %2) #26
  tail call void @llvm.memmove.p0.p0.i64(ptr align 1 %4, ptr align 1 %5, i64 %2, i1 false)
  ret i64 %0
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memmove.p0.p0.i64(ptr writeonly captures(none), ptr readonly captures(none), i64, i1 immarg) #5

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_set_bytes(i64 noundef returned %0, i64 noundef %1, i64 noundef %2) local_unnamed_addr #0 {
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef %2) #26
  %5 = trunc i64 %1 to i8
  tail call void @llvm.memset.p0.i64(ptr align 1 %4, i8 %5, i64 %2, i1 false)
  ret i64 %0
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #14

; Function Attrs: nounwind uwtable
define dso_local range(i64 0, 256) i64 @chali_string_at(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = icmp slt i64 %0, 224
  %4 = select i1 %3, i64 0, i64 8
  %5 = add nsw i64 %4, %0
  %6 = tail call ptr @chali_host_address(i64 noundef %5, i64 noundef 0) #26
  %7 = getelementptr inbounds i8, ptr %6, i64 %1
  %8 = load i8, ptr %7, align 1, !tbaa !12
  %9 = zext i8 %8 to i64
  ret i64 %9
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_string_set(i64 noundef %0, i64 noundef %1, i64 noundef %2) local_unnamed_addr #0 {
  %4 = add nsw i64 %0, 8
  %5 = add nsw i64 %4, %1
  %6 = trunc i64 %2 to i8
  %7 = tail call ptr @chali_host_address(i64 noundef %5, i64 noundef 1) #26
  store i8 %6, ptr %7, align 1, !tbaa !12
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_concat(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = icmp eq i64 %0, 0
  br i1 %3, label %12, label %4

4:                                                ; preds = %2
  %5 = icmp slt i64 %0, 224
  br i1 %5, label %9, label %6

6:                                                ; preds = %4
  %7 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #26
  %8 = load i64, ptr %7, align 1
  br label %12

9:                                                ; preds = %4
  %10 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #26
  %11 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %10) #26
  br label %12

12:                                               ; preds = %9, %6, %2
  %13 = phi i64 [ 0, %2 ], [ %8, %6 ], [ %11, %9 ]
  %14 = icmp eq i64 %1, 0
  br i1 %14, label %23, label %15

15:                                               ; preds = %12
  %16 = icmp slt i64 %1, 224
  br i1 %16, label %20, label %17

17:                                               ; preds = %15
  %18 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 8) #26
  %19 = load i64, ptr %18, align 1
  br label %23

20:                                               ; preds = %15
  %21 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 1) #26
  %22 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %21) #26
  br label %23

23:                                               ; preds = %20, %17, %12
  %24 = phi i64 [ 0, %12 ], [ %19, %17 ], [ %22, %20 ]
  %25 = add i64 %24, %13
  %26 = add i64 %25, 8
  %27 = tail call i64 @chali_host_allocate(i64 noundef %26) #26
  %28 = tail call ptr @chali_host_address(i64 noundef %27, i64 noundef 8) #26
  store i64 %25, ptr %28, align 1
  %29 = add nsw i64 %27, 8
  %30 = tail call ptr @chali_host_address(i64 noundef %29, i64 noundef %13) #26
  %31 = icmp slt i64 %0, 224
  %32 = select i1 %31, i64 0, i64 8
  %33 = add nsw i64 %32, %0
  %34 = tail call ptr @chali_host_address(i64 noundef %33, i64 noundef 0) #26
  tail call void @llvm.memcpy.p0.p0.i64(ptr align 1 %30, ptr align 1 %34, i64 %13, i1 false)
  %35 = add i64 %29, %13
  %36 = tail call ptr @chali_host_address(i64 noundef %35, i64 noundef %24) #26
  %37 = icmp slt i64 %1, 224
  %38 = select i1 %37, i64 0, i64 8
  %39 = add nsw i64 %38, %1
  %40 = tail call ptr @chali_host_address(i64 noundef %39, i64 noundef 0) #26
  tail call void @llvm.memcpy.p0.p0.i64(ptr align 1 %36, ptr align 1 %40, i64 %24, i1 false)
  ret i64 %27
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_owned_drop(i64 noundef %0) local_unnamed_addr #0 {
  tail call void @chali_host_drop(i64 noundef %0) #26
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_function_target(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call i64 @chali_host_function(i64 noundef %0, i32 noundef 0) #26
  ret i64 %2
}

declare dso_local i64 @chali_host_function(i64 noundef, i32 noundef) local_unnamed_addr #1

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_function_environment(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call i64 @chali_host_function(i64 noundef %0, i32 noundef 1) #26
  ret i64 %2
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 0, 2) i64 @chali_function_match(i64 noundef %0, ptr nofree noundef readonly captures(none) %1) local_unnamed_addr #0 {
  %3 = add i64 %0, -1
  %or.cond = icmp ult i64 %3, 223
  br i1 %or.cond, label %4, label %14

4:                                                ; preds = %2
  %5 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %1) #26
  %6 = sub nuw nsw i64 224, %0
  %7 = icmp ult i64 %5, %6
  br i1 %7, label %8, label %14

8:                                                ; preds = %4
  %9 = add nuw nsw i64 %5, 1
  %10 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef %9) #26
  %11 = tail call i32 @memcmp(ptr noundef nonnull dereferenceable(1) %10, ptr noundef nonnull dereferenceable(1) %1, i64 noundef %9)
  %12 = icmp eq i32 %11, 0
  %13 = zext i1 %12 to i64
  br label %14

14:                                               ; preds = %8, %4, %2
  %15 = phi i64 [ 0, %2 ], [ 0, %4 ], [ %13, %8 ]
  ret i64 %15
}

; Function Attrs: mustprogress nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.umax.i64(i64, i64) #15

; Function Attrs: mustprogress nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.smax.i64(i64, i64) #15

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare ptr @memchr(ptr, i32, i64) local_unnamed_addr #16

; Function Attrs: mustprogress nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.umin.i64(i64, i64) #15

; Function Attrs: mustprogress nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.umin.i32(i32, i32) #15

; Function Attrs: mustprogress nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare range(i64 -1, 2) i64 @llvm.ucmp.i64.i64(i64, i64) #15

; Function Attrs: mustprogress nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.smin.i64(i64, i64) #15

declare i64 @chali_host_call(ptr, i64, ptr, ptr) local_unnamed_addr

declare i64 @chali_host_string_storage(i64, i64) local_unnamed_addr

declare void @chali_host_memory_prepare(i64, i64, i64, i64, ptr, ptr) local_unnamed_addr

declare i64 @chali_host_list_storage_scalar(i64, i64, i64, i64, i64, i64, i64, ptr writeonly captures(none)) local_unnamed_addr

declare i8 @chali_host_result_float() local_unnamed_addr

declare ptr @chali_host_address_view(i64, i64, ptr captures(none)) local_unnamed_addr

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none, target_mem: none)
define void @chali_reset_address_view() local_unnamed_addr #17 {
entry:
  store ptr null, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(read, argmem: write, inaccessiblemem: none, target_mem: none)
define void @chali_location(ptr nofree writeonly captures(none) initializes((0, 8)) %file, ptr nofree writeonly captures(none) initializes((0, 8)) %line, ptr nofree writeonly captures(none) initializes((0, 8)) %function) local_unnamed_addr #18 {
entry:
  %f = load ptr, ptr @chali_source_file, align 8
  %l = load i64, ptr @chali_source_line, align 8
  %n = load ptr, ptr @chali_source_function, align 8
  store ptr %n, ptr %function, align 8
  store ptr %f, ptr %file, align 8
  store i64 %l, ptr %line, align 8
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(read, argmem: none, inaccessiblemem: none, target_mem: none)
define i8 @chali_result_float() local_unnamed_addr #19 {
entry:
  %flag = load i8, ptr @chali_return_float, align 1
  ret i8 %flag
}

declare void @chali_host_process_arguments(i32, ptr) local_unnamed_addr

declare i64 @chali_host_copy_words(i64, i64, i64) local_unnamed_addr

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable
define void @chali_code_anchor() #20 section "chali_code" {
entry:
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none, target_mem: none)
define void @chali_reset_slots() local_unnamed_addr #17 {
entry:
  store i8 0, ptr @chali_return_float, align 1
  store ptr null, ptr @chali_source_function, align 8
  store ptr null, ptr @chali_source_file, align 8
  store i64 0, ptr @chali_source_line, align 8
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define noundef i1 @chali_program_abi_bind(ptr nofree readnone captures(none) %registry, ptr nofree readnone captures(none) %error) local_unnamed_addr #21 {
entry:
  ret i1 true
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define noundef i1 @chali_program_std_bind(ptr nofree readnone captures(none) %registry, ptr nofree readnone captures(none) %error) local_unnamed_addr #21 {
entry:
  ret i1 true
}

; Function Attrs: alwaysinline uwtable
define internal range(i64 0, 256) i64 @f_v49_std__lang____bootStringByte(i64 %p0, i64 %p1) #22 section "chali_code" {
entry:
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4 = add i64 %p1, %p0
  %0 = icmp eq i64 %r4, 0
  br i1 %0, label %1, label %chali_require_reference.exit1

1:                                                ; preds = %entry
  tail call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit1:                    ; preds = %entry
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i = icmp eq ptr %revision_pointer.i, null
  br i1 %has_revision.not.i, label %slow.i, label %check.i

check.i:                                          ; preds = %chali_require_reference.exit1
  %revision.i = load atomic i64, ptr %revision_pointer.i acquire, align 8
  %stamp.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i = icmp eq i64 %revision.i, %stamp.i
  %enabled.i = icmp ne i64 %revision.i, 0
  %live.i = and i1 %enabled.i, %same_revision.i
  %begin.i = load i64, ptr @chali_address_view, align 16
  %size.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i = sub i64 %r4, %begin.i
  %inside.i = icmp ult i64 %offset.i, %size.i
  %valid.i = and i1 %live.i, %inside.i
  br i1 %valid.i, label %fast.i, label %slow.i

fast.i:                                           ; preds = %check.i
  %base.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i = getelementptr i8, ptr %base.i, i64 %offset.i
  br label %chali_primitive_address.exit

slow.i:                                           ; preds = %check.i, %chali_require_reference.exit1
  %resolved.i = tail call ptr @chali_host_address_view(i64 %r4, i64 1, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit

chali_primitive_address.exit:                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  %r10 = load i8, ptr %common.ret.op.i, align 1
  %r11 = zext i8 %r10 to i64
  store i8 0, ptr @chali_return_float, align 1
  ret i64 %r11
}

; Function Attrs: alwaysinline uwtable
define internal i64 @f_v56_std__lang____bootStringLength(i64 %p0) #22 section "chali_code" {
entry:
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2 = tail call i64 @chali_host_string_storage(i64 0, i64 %p0)
  store i8 0, ptr @chali_return_float, align 1
  ret i64 %r2
}

; Function Attrs: uwtable
define internal i64 @f_v57_std__lang____bootStringBuffer(i64 %p0) #23 section "chali_code" {
entry:
  %indirect_arguments.i = alloca [4 x i64], align 8
  %native_argument_types.i = alloca [4 x i8], align 1
  %raw_memory_output.i = alloca ptr, align 8
  %raw_memory_input.i = alloca ptr, align 8
  %indirect_arguments = alloca [4 x i64], align 8
  %native_argument_types = alloca [4 x i8], align 1
  %raw_memory_output = alloca ptr, align 8
  %raw_memory_input = alloca ptr, align 8
  %or.cond = icmp ugt i64 %p0, 9223372036854775799
  br i1 %or.cond, label %ast_block_4, label %ast_block_6

ast_block_4:                                      ; preds = %entry
  store ptr @f_v57_std__lang____bootStringBuffer, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 165, ptr @chali_source_line, align 8
  store i64 152, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r18 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 1, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r19 = call i8 @chali_host_result_float()
  br label %ast_block_6

ast_block_6:                                      ; preds = %entry, %ast_block_4
  store ptr @f_v57_std__lang____bootStringBuffer, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 166, ptr @chali_source_line, align 8
  %r22 = add i64 %p0, 8
  %0 = call i64 @chali_host_allocate(i64 noundef %r22) #26
  %1 = call i64 @chali_host_allocate(i64 noundef 8) #26
  call void @chali_host_memory_prepare(i64 8, i64 %1, i64 0, i64 8, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r27 = load ptr, ptr %raw_memory_output, align 8
  store i64 0, ptr %r27, align 1
  store i64 %1, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r33 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  store i64 8, ptr %r33, align 8
  %r34 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r34, align 1
  %r35 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r36 = call i8 @chali_host_result_float()
  store ptr @f_v57_std__lang____bootStringBuffer, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 167, ptr @chali_source_line, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %raw_memory_output.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %raw_memory_input.i)
  %2 = call i64 @chali_host_allocate(i64 noundef 8) #26
  call void @chali_host_memory_prepare(i64 8, i64 %2, i64 0, i64 8, ptr nonnull %raw_memory_output.i, ptr nonnull %raw_memory_input.i)
  %r3.i = load ptr, ptr %raw_memory_output.i, align 8
  store i64 0, ptr %r3.i, align 1
  store i64 %2, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  %r9.i = getelementptr inbounds nuw i8, ptr %indirect_arguments.i, i64 8
  store i64 8, ptr %r9.i, align 8
  %r10.i = getelementptr inbounds nuw i8, ptr %native_argument_types.i, i64 1
  store i8 0, ptr %r10.i, align 1
  %r11.i = call i64 @chali_host_call(ptr nonnull @native_name1, i64 2, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r12.i = call i8 @chali_host_result_float()
  %revision_pointer.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i = icmp eq ptr %revision_pointer.i.i, null
  br i1 %has_revision.not.i.i, label %slow.i.i, label %check.i.i

check.i.i:                                        ; preds = %ast_block_6
  %revision.i.i = load atomic i64, ptr %revision_pointer.i.i acquire, align 8
  %stamp.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i = icmp eq i64 %revision.i.i, %stamp.i.i
  %enabled.i.i = icmp ne i64 %revision.i.i, 0
  %live.i.i = and i1 %enabled.i.i, %same_revision.i.i
  %begin.i.i = load i64, ptr @chali_address_view, align 16
  %size.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i = sub i64 %2, %begin.i.i
  %inside.i.i = icmp ult i64 %offset.i.i, %size.i.i
  %remaining.i.i = sub i64 %size.i.i, %offset.i.i
  %fits.i.i = icmp ugt i64 %remaining.i.i, 7
  %range.i.i = and i1 %inside.i.i, %fits.i.i
  %valid.i.i = and i1 %live.i.i, %range.i.i
  br i1 %valid.i.i, label %fast.i.i, label %slow.i.i

fast.i.i:                                         ; preds = %check.i.i
  %base.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i = getelementptr i8, ptr %base.i.i, i64 %offset.i.i
  br label %chali_primitive_address.exit.i

slow.i.i:                                         ; preds = %check.i.i, %ast_block_6
  %resolved.i.i = call ptr @chali_host_address_view(i64 %2, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit.i

chali_primitive_address.exit.i:                   ; preds = %slow.i.i, %fast.i.i
  %common.ret.op.i.i = phi ptr [ %physical.i.i, %fast.i.i ], [ %resolved.i.i, %slow.i.i ]
  store i64 %0, ptr %common.ret.op.i.i, align 1
  store i64 200, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  store i64 8, ptr %r9.i, align 8
  store i8 0, ptr %r10.i, align 1
  %r26.i = call i64 @chali_host_call(ptr nonnull @native_name1, i64 2, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r27.i = call i8 @chali_host_result_float()
  %r32.i = call i64 @chali_host_copy_words(i64 200, i64 %2, i64 1)
  store i64 200, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  store i64 8, ptr %r9.i, align 8
  store i8 0, ptr %r10.i, align 1
  %r39.i = call i64 @chali_host_call(ptr nonnull @native_name1, i64 2, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r40.i = call i8 @chali_host_result_float()
  %r43.not.i = icmp eq i64 %2, 0
  br i1 %r43.not.i, label %f_v271_std__lang__pointerFromAddress__int64.exit, label %ast_block_0.i

ast_block_0.i:                                    ; preds = %chali_primitive_address.exit.i
  call void @chali_host_drop(i64 noundef %2) #26
  br label %f_v271_std__lang__pointerFromAddress__int64.exit

f_v271_std__lang__pointerFromAddress__int64.exit: ; preds = %chali_primitive_address.exit.i, %ast_block_0.i
  store i8 0, ptr @chali_return_float, align 1
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %raw_memory_output.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %raw_memory_input.i)
  %r45 = call i64 @chali_host_copy_words(i64 %1, i64 200, i64 1)
  store i64 %1, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r33, align 8
  store i8 0, ptr %r34, align 1
  %r52 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r53 = call i8 @chali_host_result_float()
  %r58.not = icmp eq i64 %1, 200
  br i1 %r58.not, label %ast_block_9, label %ast_block_7

ast_block_7:                                      ; preds = %f_v271_std__lang__pointerFromAddress__int64.exit
  store i64 200, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r33, align 8
  store i8 0, ptr %r34, align 1
  %r67 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 16
  store i64 49, ptr %r67, align 8
  %r68 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r68, align 1
  %r69 = call i64 @chali_host_call(ptr nonnull @native_name2, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r70 = call i8 @chali_host_result_float()
  br label %ast_block_9

ast_block_9:                                      ; preds = %f_v271_std__lang__pointerFromAddress__int64.exit, %ast_block_7
  %revision_pointer.i27 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i28 = icmp eq ptr %revision_pointer.i27, null
  br i1 %has_revision.not.i28, label %slow.i43, label %check.i29

check.i29:                                        ; preds = %ast_block_9
  %revision.i30 = load atomic i64, ptr %revision_pointer.i27 acquire, align 8
  %stamp.i31 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i32 = icmp eq i64 %revision.i30, %stamp.i31
  %enabled.i33 = icmp ne i64 %revision.i30, 0
  %live.i34 = and i1 %enabled.i33, %same_revision.i32
  %begin.i35 = load i64, ptr @chali_address_view, align 16
  %size.i36 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i37 = sub i64 %1, %begin.i35
  %inside.i38 = icmp ult i64 %offset.i37, %size.i36
  %remaining.i39 = sub i64 %size.i36, %offset.i37
  %fits.i40 = icmp ugt i64 %remaining.i39, 7
  %range.i41 = and i1 %inside.i38, %fits.i40
  %valid.i42 = and i1 %live.i34, %range.i41
  br i1 %valid.i42, label %fast.i46, label %slow.i43

fast.i46:                                         ; preds = %check.i29
  %base.i47 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i48 = getelementptr i8, ptr %base.i47, i64 %offset.i37
  br label %chali_primitive_address.exit49

slow.i43:                                         ; preds = %check.i29, %ast_block_9
  %resolved.i44 = call ptr @chali_host_address_view(i64 %1, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit49

chali_primitive_address.exit49:                   ; preds = %fast.i46, %slow.i43
  %common.ret.op.i45 = phi ptr [ %physical.i48, %fast.i46 ], [ %resolved.i44, %slow.i43 ]
  %r74 = load i64, ptr %common.ret.op.i45, align 1
  %3 = icmp eq i64 %r74, 0
  br i1 %3, label %4, label %chali_require_reference.exit50

4:                                                ; preds = %chali_primitive_address.exit49
  call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit50:                   ; preds = %chali_primitive_address.exit49
  %revision_pointer.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i = icmp eq ptr %revision_pointer.i, null
  br i1 %has_revision.not.i, label %slow.i, label %check.i

check.i:                                          ; preds = %chali_require_reference.exit50
  %revision.i = load atomic i64, ptr %revision_pointer.i acquire, align 8
  %stamp.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i = icmp eq i64 %revision.i, %stamp.i
  %enabled.i = icmp ne i64 %revision.i, 0
  %live.i = and i1 %enabled.i, %same_revision.i
  %begin.i = load i64, ptr @chali_address_view, align 16
  %size.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i = sub i64 %r74, %begin.i
  %inside.i = icmp ult i64 %offset.i, %size.i
  %remaining.i = sub i64 %size.i, %offset.i
  %fits.i = icmp ugt i64 %remaining.i, 7
  %range.i = and i1 %inside.i, %fits.i
  %valid.i = and i1 %live.i, %range.i
  br i1 %valid.i, label %fast.i, label %slow.i

fast.i:                                           ; preds = %check.i
  %base.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i = getelementptr i8, ptr %base.i, i64 %offset.i
  br label %chali_primitive_address.exit

slow.i:                                           ; preds = %check.i, %chali_require_reference.exit50
  %resolved.i = call ptr @chali_host_address_view(i64 %r74, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit

chali_primitive_address.exit:                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  store i64 %p0, ptr %common.ret.op.i, align 1
  %r86.not = icmp eq i64 %1, 0
  br i1 %r86.not, label %ast_block_12, label %ast_block_10

ast_block_10:                                     ; preds = %chali_primitive_address.exit
  call void @chali_host_drop(i64 noundef %1) #26
  br label %ast_block_12

ast_block_12:                                     ; preds = %chali_primitive_address.exit, %ast_block_10
  store i8 0, ptr @chali_return_float, align 1
  ret i64 %0
}

; Function Attrs: uwtable
define internal noundef i64 @f_v58_std__lang____bootStringCopyBytes(i64 %p0, i64 %p1, i64 %p2) #23 section "chali_code" {
entry:
  %indirect_arguments.i62 = alloca [4 x i64], align 8
  %native_argument_types.i63 = alloca [4 x i8], align 1
  %raw_memory_output.i64 = alloca ptr, align 8
  %raw_memory_input.i65 = alloca ptr, align 8
  %indirect_arguments.i = alloca [4 x i64], align 8
  %native_argument_types.i = alloca [4 x i8], align 1
  %raw_memory_output.i = alloca ptr, align 8
  %raw_memory_input.i = alloca ptr, align 8
  %indirect_arguments = alloca [4 x i64], align 8
  %native_argument_types = alloca [4 x i8], align 1
  %raw_memory_output = alloca ptr, align 8
  %raw_memory_input = alloca ptr, align 8
  store ptr @f_v58_std__lang____bootStringCopyBytes, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 172, ptr @chali_source_line, align 8
  %r2 = icmp sgt i64 %p2, 0
  br i1 %r2, label %ast_block_0, label %ast_block_2

ast_block_0:                                      ; preds = %entry
  %0 = tail call i64 @chali_host_allocate(i64 noundef 8) #26
  call void @chali_host_memory_prepare(i64 8, i64 %0, i64 0, i64 8, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r8 = load ptr, ptr %raw_memory_output, align 8
  store i64 0, ptr %r8, align 1
  store i64 %0, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r14 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  store i64 8, ptr %r14, align 8
  %r15 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r15, align 1
  %r16 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r17 = call i8 @chali_host_result_float()
  %1 = call i64 @chali_host_allocate(i64 noundef 8) #26
  call void @chali_host_memory_prepare(i64 8, i64 %1, i64 0, i64 8, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r21 = load ptr, ptr %raw_memory_output, align 8
  store i64 0, ptr %r21, align 1
  store i64 %1, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r14, align 8
  store i8 0, ptr %r15, align 1
  %r29 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r30 = call i8 @chali_host_result_float()
  store ptr @f_v58_std__lang____bootStringCopyBytes, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 172, ptr @chali_source_line, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %raw_memory_output.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %raw_memory_input.i)
  %2 = call i64 @chali_host_allocate(i64 noundef 8) #26
  call void @chali_host_memory_prepare(i64 8, i64 %2, i64 0, i64 8, ptr nonnull %raw_memory_output.i, ptr nonnull %raw_memory_input.i)
  %r3.i = load ptr, ptr %raw_memory_output.i, align 8
  store i64 0, ptr %r3.i, align 1
  store i64 %2, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  %r9.i = getelementptr inbounds nuw i8, ptr %indirect_arguments.i, i64 8
  store i64 8, ptr %r9.i, align 8
  %r10.i = getelementptr inbounds nuw i8, ptr %native_argument_types.i, i64 1
  store i8 0, ptr %r10.i, align 1
  %r11.i = call i64 @chali_host_call(ptr nonnull @native_name1, i64 2, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r12.i = call i8 @chali_host_result_float()
  %revision_pointer.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i = icmp eq ptr %revision_pointer.i.i, null
  br i1 %has_revision.not.i.i, label %slow.i.i, label %check.i.i

check.i.i:                                        ; preds = %ast_block_0
  %revision.i.i = load atomic i64, ptr %revision_pointer.i.i acquire, align 8
  %stamp.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i = icmp eq i64 %revision.i.i, %stamp.i.i
  %enabled.i.i = icmp ne i64 %revision.i.i, 0
  %live.i.i = and i1 %enabled.i.i, %same_revision.i.i
  %begin.i.i = load i64, ptr @chali_address_view, align 16
  %size.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i = sub i64 %2, %begin.i.i
  %inside.i.i = icmp ult i64 %offset.i.i, %size.i.i
  %remaining.i.i = sub i64 %size.i.i, %offset.i.i
  %fits.i.i = icmp ugt i64 %remaining.i.i, 7
  %range.i.i = and i1 %inside.i.i, %fits.i.i
  %valid.i.i = and i1 %live.i.i, %range.i.i
  br i1 %valid.i.i, label %fast.i.i, label %slow.i.i

fast.i.i:                                         ; preds = %check.i.i
  %base.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i = getelementptr i8, ptr %base.i.i, i64 %offset.i.i
  br label %chali_primitive_address.exit.i

slow.i.i:                                         ; preds = %check.i.i, %ast_block_0
  %resolved.i.i = call ptr @chali_host_address_view(i64 %2, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit.i

chali_primitive_address.exit.i:                   ; preds = %slow.i.i, %fast.i.i
  %common.ret.op.i.i = phi ptr [ %physical.i.i, %fast.i.i ], [ %resolved.i.i, %slow.i.i ]
  store i64 %p0, ptr %common.ret.op.i.i, align 1
  store i64 208, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  store i64 8, ptr %r9.i, align 8
  store i8 0, ptr %r10.i, align 1
  %r26.i = call i64 @chali_host_call(ptr nonnull @native_name1, i64 2, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r27.i = call i8 @chali_host_result_float()
  %r32.i = call i64 @chali_host_copy_words(i64 208, i64 %2, i64 1)
  store i64 208, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  store i64 8, ptr %r9.i, align 8
  store i8 0, ptr %r10.i, align 1
  %r39.i = call i64 @chali_host_call(ptr nonnull @native_name1, i64 2, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r40.i = call i8 @chali_host_result_float()
  %r43.not.i = icmp eq i64 %2, 0
  br i1 %r43.not.i, label %f_v273_std__lang__pointerFromAddress__uint8.exit, label %ast_block_0.i

ast_block_0.i:                                    ; preds = %chali_primitive_address.exit.i
  call void @chali_host_drop(i64 noundef %2) #26
  br label %f_v273_std__lang__pointerFromAddress__uint8.exit

f_v273_std__lang__pointerFromAddress__uint8.exit: ; preds = %chali_primitive_address.exit.i, %ast_block_0.i
  store i8 0, ptr @chali_return_float, align 1
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %raw_memory_output.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %raw_memory_input.i)
  %r39 = call i64 @chali_host_copy_words(i64 %0, i64 208, i64 1)
  store i64 %0, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r14, align 8
  store i8 0, ptr %r15, align 1
  %r46 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r47 = call i8 @chali_host_result_float()
  %r52.not = icmp eq i64 %0, 208
  br i1 %r52.not, label %ast_block_5, label %ast_block_3

ast_block_3:                                      ; preds = %f_v273_std__lang__pointerFromAddress__uint8.exit
  store i64 208, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r14, align 8
  store i8 0, ptr %r15, align 1
  %r61 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 16
  store i64 47, ptr %r61, align 8
  %r62 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r62, align 1
  %r63 = call i64 @chali_host_call(ptr nonnull @native_name2, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r64 = call i8 @chali_host_result_float()
  br label %ast_block_5

ast_block_5:                                      ; preds = %f_v273_std__lang__pointerFromAddress__uint8.exit, %ast_block_3
  store ptr @f_v58_std__lang____bootStringCopyBytes, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 172, ptr @chali_source_line, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i62)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i63)
  call void @llvm.lifetime.start.p0(ptr nonnull %raw_memory_output.i64)
  call void @llvm.lifetime.start.p0(ptr nonnull %raw_memory_input.i65)
  %3 = call i64 @chali_host_allocate(i64 noundef 8) #26
  call void @chali_host_memory_prepare(i64 8, i64 %3, i64 0, i64 8, ptr nonnull %raw_memory_output.i64, ptr nonnull %raw_memory_input.i65)
  %r3.i67 = load ptr, ptr %raw_memory_output.i64, align 8
  store i64 0, ptr %r3.i67, align 1
  store i64 %3, ptr %indirect_arguments.i62, align 8
  store i8 0, ptr %native_argument_types.i63, align 1
  %r9.i68 = getelementptr inbounds nuw i8, ptr %indirect_arguments.i62, i64 8
  store i64 8, ptr %r9.i68, align 8
  %r10.i69 = getelementptr inbounds nuw i8, ptr %native_argument_types.i63, i64 1
  store i8 0, ptr %r10.i69, align 1
  %r11.i70 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 2, ptr nonnull %indirect_arguments.i62, ptr nonnull %native_argument_types.i63)
  %r12.i71 = call i8 @chali_host_result_float()
  %revision_pointer.i.i72 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i73 = icmp eq ptr %revision_pointer.i.i72, null
  br i1 %has_revision.not.i.i73, label %slow.i.i88, label %check.i.i74

check.i.i74:                                      ; preds = %ast_block_5
  %revision.i.i75 = load atomic i64, ptr %revision_pointer.i.i72 acquire, align 8
  %stamp.i.i76 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i77 = icmp eq i64 %revision.i.i75, %stamp.i.i76
  %enabled.i.i78 = icmp ne i64 %revision.i.i75, 0
  %live.i.i79 = and i1 %enabled.i.i78, %same_revision.i.i77
  %begin.i.i80 = load i64, ptr @chali_address_view, align 16
  %size.i.i81 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i82 = sub i64 %3, %begin.i.i80
  %inside.i.i83 = icmp ult i64 %offset.i.i82, %size.i.i81
  %remaining.i.i84 = sub i64 %size.i.i81, %offset.i.i82
  %fits.i.i85 = icmp ugt i64 %remaining.i.i84, 7
  %range.i.i86 = and i1 %inside.i.i83, %fits.i.i85
  %valid.i.i87 = and i1 %live.i.i79, %range.i.i86
  br i1 %valid.i.i87, label %fast.i.i99, label %slow.i.i88

fast.i.i99:                                       ; preds = %check.i.i74
  %base.i.i100 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i101 = getelementptr i8, ptr %base.i.i100, i64 %offset.i.i82
  br label %chali_primitive_address.exit.i90

slow.i.i88:                                       ; preds = %check.i.i74, %ast_block_5
  %resolved.i.i89 = call ptr @chali_host_address_view(i64 %3, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit.i90

chali_primitive_address.exit.i90:                 ; preds = %slow.i.i88, %fast.i.i99
  %common.ret.op.i.i91 = phi ptr [ %physical.i.i101, %fast.i.i99 ], [ %resolved.i.i89, %slow.i.i88 ]
  store i64 %p1, ptr %common.ret.op.i.i91, align 1
  store i64 208, ptr %indirect_arguments.i62, align 8
  store i8 0, ptr %native_argument_types.i63, align 1
  store i64 8, ptr %r9.i68, align 8
  store i8 0, ptr %r10.i69, align 1
  %r26.i92 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 2, ptr nonnull %indirect_arguments.i62, ptr nonnull %native_argument_types.i63)
  %r27.i93 = call i8 @chali_host_result_float()
  %r32.i94 = call i64 @chali_host_copy_words(i64 208, i64 %3, i64 1)
  store i64 208, ptr %indirect_arguments.i62, align 8
  store i8 0, ptr %native_argument_types.i63, align 1
  store i64 8, ptr %r9.i68, align 8
  store i8 0, ptr %r10.i69, align 1
  %r39.i95 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 2, ptr nonnull %indirect_arguments.i62, ptr nonnull %native_argument_types.i63)
  %r40.i96 = call i8 @chali_host_result_float()
  %r43.not.i97 = icmp eq i64 %3, 0
  br i1 %r43.not.i97, label %f_v273_std__lang__pointerFromAddress__uint8.exit102, label %ast_block_0.i98

ast_block_0.i98:                                  ; preds = %chali_primitive_address.exit.i90
  call void @chali_host_drop(i64 noundef %3) #26
  br label %f_v273_std__lang__pointerFromAddress__uint8.exit102

f_v273_std__lang__pointerFromAddress__uint8.exit102: ; preds = %chali_primitive_address.exit.i90, %ast_block_0.i98
  store i8 0, ptr @chali_return_float, align 1
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i62)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i63)
  call void @llvm.lifetime.end.p0(ptr nonnull %raw_memory_output.i64)
  call void @llvm.lifetime.end.p0(ptr nonnull %raw_memory_input.i65)
  %r75 = call i64 @chali_host_copy_words(i64 %1, i64 208, i64 1)
  store i64 %1, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r14, align 8
  store i8 0, ptr %r15, align 1
  %r82 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r83 = call i8 @chali_host_result_float()
  %r88.not = icmp eq i64 %1, 208
  br i1 %r88.not, label %ast_block_8, label %ast_block_6

ast_block_6:                                      ; preds = %f_v273_std__lang__pointerFromAddress__uint8.exit102
  store i64 208, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r14, align 8
  store i8 0, ptr %r15, align 1
  %r97 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 16
  store i64 47, ptr %r97, align 8
  %r98 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r98, align 1
  %r99 = call i64 @chali_host_call(ptr nonnull @native_name2, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r100 = call i8 @chali_host_result_float()
  br label %ast_block_8

ast_block_8:                                      ; preds = %f_v273_std__lang__pointerFromAddress__uint8.exit102, %ast_block_6
  %revision_pointer.i39 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i40 = icmp eq ptr %revision_pointer.i39, null
  br i1 %has_revision.not.i40, label %slow.i55, label %check.i41

check.i41:                                        ; preds = %ast_block_8
  %revision.i42 = load atomic i64, ptr %revision_pointer.i39 acquire, align 8
  %stamp.i43 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i44 = icmp eq i64 %revision.i42, %stamp.i43
  %enabled.i45 = icmp ne i64 %revision.i42, 0
  %live.i46 = and i1 %enabled.i45, %same_revision.i44
  %begin.i47 = load i64, ptr @chali_address_view, align 16
  %size.i48 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i49 = sub i64 %0, %begin.i47
  %inside.i50 = icmp ult i64 %offset.i49, %size.i48
  %remaining.i51 = sub i64 %size.i48, %offset.i49
  %fits.i52 = icmp ugt i64 %remaining.i51, 7
  %range.i53 = and i1 %inside.i50, %fits.i52
  %valid.i54 = and i1 %live.i46, %range.i53
  br i1 %valid.i54, label %fast.i58, label %slow.i55

fast.i58:                                         ; preds = %check.i41
  %base.i59 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i60 = getelementptr i8, ptr %base.i59, i64 %offset.i49
  br label %chali_primitive_address.exit61

slow.i55:                                         ; preds = %check.i41, %ast_block_8
  %resolved.i56 = call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit61

chali_primitive_address.exit61:                   ; preds = %fast.i58, %slow.i55
  %common.ret.op.i57 = phi ptr [ %physical.i60, %fast.i58 ], [ %resolved.i56, %slow.i55 ]
  %r108 = load i64, ptr %common.ret.op.i57, align 1
  %revision_pointer.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i = icmp eq ptr %revision_pointer.i, null
  br i1 %has_revision.not.i, label %slow.i, label %check.i

check.i:                                          ; preds = %chali_primitive_address.exit61
  %revision.i = load atomic i64, ptr %revision_pointer.i acquire, align 8
  %stamp.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i = icmp eq i64 %revision.i, %stamp.i
  %enabled.i = icmp ne i64 %revision.i, 0
  %live.i = and i1 %enabled.i, %same_revision.i
  %begin.i = load i64, ptr @chali_address_view, align 16
  %size.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i = sub i64 %1, %begin.i
  %inside.i = icmp ult i64 %offset.i, %size.i
  %remaining.i = sub i64 %size.i, %offset.i
  %fits.i = icmp ugt i64 %remaining.i, 7
  %range.i = and i1 %inside.i, %fits.i
  %valid.i = and i1 %live.i, %range.i
  br i1 %valid.i, label %fast.i, label %slow.i

fast.i:                                           ; preds = %check.i
  %base.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i = getelementptr i8, ptr %base.i, i64 %offset.i
  br label %chali_primitive_address.exit

slow.i:                                           ; preds = %check.i, %chali_primitive_address.exit61
  %resolved.i = call ptr @chali_host_address_view(i64 %1, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit

chali_primitive_address.exit:                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  %r112 = load i64, ptr %common.ret.op.i, align 1
  call void @chali_host_memory_prepare(i64 7, i64 %r108, i64 %r112, i64 %p2, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r115 = load ptr, ptr %raw_memory_output, align 8
  %r116 = load ptr, ptr %raw_memory_input, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %r115, ptr align 1 %r116, i64 %p2, i1 false)
  %r119.not = icmp eq i64 %1, 0
  br i1 %r119.not, label %ast_block_11, label %ast_block_9

ast_block_9:                                      ; preds = %chali_primitive_address.exit
  call void @chali_host_drop(i64 noundef %1) #26
  br label %ast_block_11

ast_block_11:                                     ; preds = %chali_primitive_address.exit, %ast_block_9
  %r126.not = icmp eq i64 %0, 0
  br i1 %r126.not, label %ast_block_2, label %ast_block_12

ast_block_12:                                     ; preds = %ast_block_11
  call void @chali_host_drop(i64 noundef %0) #26
  br label %ast_block_2

ast_block_2:                                      ; preds = %ast_block_12, %ast_block_11, %entry
  store i8 0, ptr @chali_return_float, align 1
  ret i64 0
}

; Function Attrs: uwtable
define internal i64 @f_v60_std__lang____bootStringCopy(i64 %p0) #23 section "chali_code" {
entry:
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i = tail call i64 @chali_host_string_storage(i64 0, i64 %p0)
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 178, ptr @chali_source_line, align 8
  %r6 = tail call i64 @f_v57_std__lang____bootStringBuffer(i64 %r2.i)
  %r7 = load i8, ptr @chali_return_float, align 1
  %r10 = add i64 %r6, 8
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 179, ptr @chali_source_line, align 8
  %r13 = tail call i64 @chali_host_string_storage(i64 1, i64 %p0)
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 179, ptr @chali_source_line, align 8
  %r16 = tail call i64 @f_v58_std__lang____bootStringCopyBytes(i64 %r10, i64 %r13, i64 %r2.i)
  store i8 %r7, ptr @chali_return_float, align 1
  ret i64 %r6
}

; Function Attrs: alwaysinline nounwind uwtable
define internal noundef i64 @f_v61_std__lang____bootStringDrop(i64 %p0) #24 section "chali_code" {
entry:
  tail call void @chali_host_drop(i64 noundef %p0) #26
  store i8 0, ptr @chali_return_float, align 1
  ret i64 0
}

; Function Attrs: alwaysinline uwtable
define internal range(i64 -1, 2) i64 @f_v64_std__lang____bootStringCompare(i64 %p0, i64 %p1) #22 section "chali_code" {
entry:
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i57 = tail call i64 @chali_host_string_storage(i64 0, i64 %p0)
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i = tail call i64 @chali_host_string_storage(i64 0, i64 %p1)
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 199, ptr @chali_source_line, align 8
  %r10 = tail call i64 @chali_host_string_storage(i64 1, i64 %p0)
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 199, ptr @chali_source_line, align 8
  %r13 = tail call i64 @chali_host_string_storage(i64 1, i64 %p1)
  %r1861 = icmp sgt i64 %r2.i57, 0
  br i1 %r1861, label %ast_block_1.preheader, label %ast_label_v513___chali_brk_24

ast_block_1.preheader:                            ; preds = %entry
  %smax = tail call i64 @llvm.smax.i64(i64 %r2.i, i64 0)
  br label %ast_block_1

ast_block_1:                                      ; preds = %ast_block_1.preheader, %ast_block_12
  %s_v507___ast_64_i_736.062 = phi i64 [ %r64, %ast_block_12 ], [ 0, %ast_block_1.preheader ]
  %exitcond.not = icmp eq i64 %s_v507___ast_64_i_736.062, %smax
  br i1 %exitcond.not, label %ast_label_v513___chali_brk_24, label %ast_block_4

ast_block_4:                                      ; preds = %ast_block_1
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i29 = add i64 %s_v507___ast_64_i_736.062, %r10
  %0 = icmp eq i64 %r4.i29, 0
  br i1 %0, label %1, label %chali_require_reference.exit58

1:                                                ; preds = %ast_block_4
  tail call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit58:                   ; preds = %ast_block_4
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i32 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i33 = icmp eq ptr %revision_pointer.i.i32, null
  br i1 %has_revision.not.i.i33, label %slow.i.i48, label %check.i.i34

check.i.i34:                                      ; preds = %chali_require_reference.exit58
  %revision.i.i35 = load atomic i64, ptr %revision_pointer.i.i32 acquire, align 8
  %stamp.i.i36 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i37 = icmp eq i64 %revision.i.i35, %stamp.i.i36
  %enabled.i.i38 = icmp ne i64 %revision.i.i35, 0
  %live.i.i39 = and i1 %enabled.i.i38, %same_revision.i.i37
  %begin.i.i40 = load i64, ptr @chali_address_view, align 16
  %size.i.i41 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i42 = sub i64 %r4.i29, %begin.i.i40
  %inside.i.i43 = icmp ult i64 %offset.i.i42, %size.i.i41
  %valid.i.i47 = and i1 %live.i.i39, %inside.i.i43
  br i1 %valid.i.i47, label %fast.i.i53, label %slow.i.i48

fast.i.i53:                                       ; preds = %check.i.i34
  %base.i.i54 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i55 = getelementptr i8, ptr %base.i.i54, i64 %offset.i.i42
  br label %f_v49_std__lang____bootStringByte.exit56

slow.i.i48:                                       ; preds = %check.i.i34, %chali_require_reference.exit58
  %resolved.i.i49 = tail call ptr @chali_host_address_view(i64 %r4.i29, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit56

f_v49_std__lang____bootStringByte.exit56:         ; preds = %fast.i.i53, %slow.i.i48
  %common.ret.op.i.i50 = phi ptr [ %physical.i.i55, %fast.i.i53 ], [ %resolved.i.i49, %slow.i.i48 ]
  %r10.i51 = load i8, ptr %common.ret.op.i.i50, align 1
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i = add i64 %s_v507___ast_64_i_736.062, %r13
  %2 = icmp eq i64 %r4.i, 0
  br i1 %2, label %3, label %chali_require_reference.exit60

3:                                                ; preds = %f_v49_std__lang____bootStringByte.exit56
  tail call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit60:                   ; preds = %f_v49_std__lang____bootStringByte.exit56
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i = icmp eq ptr %revision_pointer.i.i, null
  br i1 %has_revision.not.i.i, label %slow.i.i, label %check.i.i

check.i.i:                                        ; preds = %chali_require_reference.exit60
  %revision.i.i = load atomic i64, ptr %revision_pointer.i.i acquire, align 8
  %stamp.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i = icmp eq i64 %revision.i.i, %stamp.i.i
  %enabled.i.i = icmp ne i64 %revision.i.i, 0
  %live.i.i = and i1 %enabled.i.i, %same_revision.i.i
  %begin.i.i = load i64, ptr @chali_address_view, align 16
  %size.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i = sub i64 %r4.i, %begin.i.i
  %inside.i.i = icmp ult i64 %offset.i.i, %size.i.i
  %valid.i.i = and i1 %live.i.i, %inside.i.i
  br i1 %valid.i.i, label %fast.i.i, label %slow.i.i

fast.i.i:                                         ; preds = %check.i.i
  %base.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i = getelementptr i8, ptr %base.i.i, i64 %offset.i.i
  br label %f_v49_std__lang____bootStringByte.exit

slow.i.i:                                         ; preds = %check.i.i, %chali_require_reference.exit60
  %resolved.i.i = tail call ptr @chali_host_address_view(i64 %r4.i, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit

f_v49_std__lang____bootStringByte.exit:           ; preds = %fast.i.i, %slow.i.i
  %common.ret.op.i.i = phi ptr [ %physical.i.i, %fast.i.i ], [ %resolved.i.i, %slow.i.i ]
  %r10.i = load i8, ptr %common.ret.op.i.i, align 1
  store i8 0, ptr @chali_return_float, align 1
  %r47 = icmp ult i8 %r10.i51, %r10.i
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 202, ptr @chali_source_line, align 8
  br i1 %r47, label %common.ret, label %ast_block_9

common.ret:                                       ; preds = %ast_block_9, %f_v49_std__lang____bootStringByte.exit, %ast_block_15, %ast_block_13
  %common.ret.op = phi i64 [ %., %ast_block_15 ], [ -1, %ast_block_13 ], [ -1, %f_v49_std__lang____bootStringByte.exit ], [ 1, %ast_block_9 ]
  ret i64 %common.ret.op

ast_block_9:                                      ; preds = %f_v49_std__lang____bootStringByte.exit
  %r57 = icmp ugt i8 %r10.i51, %r10.i
  br i1 %r57, label %common.ret, label %ast_block_12

ast_block_12:                                     ; preds = %ast_block_9
  %r64 = add nuw nsw i64 %s_v507___ast_64_i_736.062, 1
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 200, ptr @chali_source_line, align 8
  %exitcond63.not = icmp eq i64 %r64, %r2.i57
  br i1 %exitcond63.not, label %ast_label_v513___chali_brk_24, label %ast_block_1

ast_label_v513___chali_brk_24:                    ; preds = %ast_block_1, %ast_block_12, %entry
  %r69 = icmp slt i64 %r2.i57, %r2.i
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 204, ptr @chali_source_line, align 8
  br i1 %r69, label %ast_block_13, label %ast_block_15

ast_block_13:                                     ; preds = %ast_label_v513___chali_brk_24
  store i8 0, ptr @chali_return_float, align 1
  br label %common.ret

ast_block_15:                                     ; preds = %ast_label_v513___chali_brk_24
  %r79 = icmp sgt i64 %r2.i57, %r2.i
  store i8 0, ptr @chali_return_float, align 1
  %. = zext i1 %r79 to i64
  br label %common.ret
}

; Function Attrs: alwaysinline uwtable
define internal noundef i64 @f_v162_std__lang__assert_bool(i64 %p0) #22 section "chali_code" {
entry:
  %indirect_arguments = alloca [2 x i64], align 8
  %native_argument_types = alloca [2 x i8], align 1
  %r2.not = icmp eq i64 %p0, 0
  br i1 %r2.not, label %ast_block_0, label %ast_block_2

ast_block_0:                                      ; preds = %entry
  store ptr @f_v162_std__lang__assert_bool, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 782, ptr @chali_source_line, align 8
  store i64 176, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r8 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 1, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r9 = call i8 @chali_host_result_float()
  br label %ast_block_2

ast_block_2:                                      ; preds = %entry, %ast_block_0
  store i8 0, ptr @chali_return_float, align 1
  ret i64 0
}

; Function Attrs: uwtable
define internal noundef i64 @f_v163_std__lang__assert_bool_std__lang__String(i64 %p0, i64 %p1) #23 section "chali_code" {
entry:
  %indirect_arguments = alloca [2 x i64], align 8
  %native_argument_types = alloca [2 x i8], align 1
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i.i = tail call i64 @chali_host_string_storage(i64 0, i64 %p1), !inline_history !32
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 178, ptr @chali_source_line, align 8
  %r6.i = tail call i64 @f_v57_std__lang____bootStringBuffer(i64 %r2.i.i), !inline_history !32
  %r7.i = load i8, ptr @chali_return_float, align 1
  %r10.i = add i64 %r6.i, 8
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 179, ptr @chali_source_line, align 8
  %r13.i = tail call i64 @chali_host_string_storage(i64 1, i64 %p1), !inline_history !32
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 179, ptr @chali_source_line, align 8
  %r16.i = tail call i64 @f_v58_std__lang____bootStringCopyBytes(i64 %r10.i, i64 %r13.i, i64 %r2.i.i), !inline_history !32
  store i8 %r7.i, ptr @chali_return_float, align 1
  %r6.not = icmp eq i64 %p0, 0
  br i1 %r6.not, label %ast_block_0, label %ast_block_2

ast_block_0:                                      ; preds = %entry
  store ptr @f_v163_std__lang__assert_bool_std__lang__String, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 786, ptr @chali_source_line, align 8
  store i64 %r6.i, ptr %indirect_arguments, align 8
  store i8 %r7.i, ptr %native_argument_types, align 1
  %r14 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 1, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r15 = call i8 @chali_host_result_float()
  br label %ast_block_2

ast_block_2:                                      ; preds = %entry, %ast_block_0
  %r18.not = icmp eq i64 %r6.i, 0
  br i1 %r18.not, label %ast_block_5, label %ast_block_3

ast_block_3:                                      ; preds = %ast_block_2
  call void @chali_host_drop(i64 noundef %r6.i) #26
  br label %ast_block_5

ast_block_5:                                      ; preds = %ast_block_2, %ast_block_3
  store i8 0, ptr @chali_return_float, align 1
  ret i64 0
}

; Function Attrs: uwtable
define internal i64 @f_v169_std__lang____bootBufferCapacity(i64 %p0, i64 %p1, i64 %p2, i64 %p3, i64 %p4) #23 section "chali_code" {
entry:
  %r2 = icmp slt i64 %p0, 0
  %r7 = icmp slt i64 %p1, 0
  %or.cond.not47.not49.not52 = select i1 %r2, i1 true, i1 %r7
  %r17.not = icmp slt i64 %p2, %p0
  %or.cond43.not48.not51 = select i1 %or.cond.not47.not49.not52, i1 true, i1 %r17.not
  %r25 = icmp slt i64 %p3, 0
  %or.cond44.not50 = select i1 %or.cond43.not48.not51, i1 true, i1 %r25
  %r35.not = icmp slt i64 %p4, %p3
  %or.cond45 = select i1 %or.cond44.not50, i1 true, i1 %r35.not
  br i1 %or.cond45, label %ast_block_18, label %ast_block_17

ast_block_17:                                     ; preds = %entry
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function, align 8
  %r45 = icmp samesign ule i64 %p2, %p4
  %0 = zext i1 %r45 to i64
  br label %ast_block_18

ast_block_18:                                     ; preds = %entry, %ast_block_17
  %r48 = phi i64 [ %0, %ast_block_17 ], [ 0, %entry ]
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1390, ptr @chali_source_line, align 8
  %r50 = tail call i64 @f_v163_std__lang__assert_bool_std__lang__String(i64 %r48, i64 88)
  %r58 = sub i64 %p4, %p0
  %r59 = icmp sle i64 %p1, %r58
  %r60 = zext i1 %r59 to i64
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1391, ptr @chali_source_line, align 8
  %r61 = tail call i64 @f_v163_std__lang__assert_bool_std__lang__String(i64 %r60, i64 120)
  %r67 = add i64 %p1, %p0
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1393, ptr @chali_source_line, align 8
  %r72.not = icmp sgt i64 %r67, %p2
  br i1 %r72.not, label %ast_block_22, label %common.ret

common.ret:                                       ; preds = %ast_block_18, %ast_block_22
  %common.ret.op = phi i64 [ %s_v521___ast_169_grown.2, %ast_block_22 ], [ %p2, %ast_block_18 ]
  store i8 0, ptr @chali_return_float, align 1
  ret i64 %common.ret.op

ast_block_22:                                     ; preds = %ast_block_18
  %1 = sdiv i64 %p4, 2
  %r86.not = icmp sgt i64 %p2, %1
  %r91 = shl i64 %p2, 1
  %spec.select46 = select i1 %r86.not, i64 %p4, i64 %r91
  %spec.select = tail call i64 @llvm.smax.i64(i64 %spec.select46, i64 %r67)
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1397, ptr @chali_source_line, align 8
  %s_v521___ast_169_grown.2 = tail call i64 @llvm.smax.i64(i64 %spec.select, i64 %p3)
  br label %common.ret
}

; Function Attrs: uwtable
define internal noundef i64 @f_v170_std__lang____bootListReserve(i64 %p0, i64 %p1) #23 section "chali_code" {
entry:
  %collection_result_kind = alloca i8, align 1
  store ptr @f_v170_std__lang____bootListReserve, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1403, ptr @chali_source_line, align 8
  %r6 = call i64 @chali_host_list_storage_scalar(i64 1, i64 %p0, i64 0, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind)
  store ptr @f_v170_std__lang____bootListReserve, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1404, ptr @chali_source_line, align 8
  %r12.not = icmp sgt i64 %p1, %r6
  br i1 %r12.not, label %ast_block_2, label %common.ret

common.ret:                                       ; preds = %entry, %ast_block_2
  store i8 0, ptr @chali_return_float, align 1
  ret i64 0

ast_block_2:                                      ; preds = %entry
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1405, ptr @chali_source_line, align 8
  %r23 = call i64 @chali_host_list_storage_scalar(i64 17, i64 %p0, i64 0, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind)
  %spec.select = tail call i64 @llvm.smin.i64(i64 %r23, i64 8)
  %0 = or i64 %r6, %p1
  %or.cond43.not48.not51.i = icmp sgt i64 %0, -1
  %r25.i = icmp sgt i64 %r23, -1
  %or.cond44.not50.i.not = select i1 %or.cond43.not48.not51.i, i1 %r25.i, i1 false
  %r45.i = icmp samesign ule i64 %r6, %r23
  %narrow = select i1 %or.cond44.not50.i.not, i1 %r45.i, i1 false
  %r48.i = zext i1 %narrow to i64
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1390, ptr @chali_source_line, align 8
  %r50.i = tail call i64 @f_v163_std__lang__assert_bool_std__lang__String(i64 %r48.i, i64 88), !inline_history !33
  %r59.i = icmp sle i64 %p1, %r23
  %r60.i = zext i1 %r59.i to i64
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1391, ptr @chali_source_line, align 8
  %r61.i = tail call i64 @f_v163_std__lang__assert_bool_std__lang__String(i64 %r60.i, i64 120), !inline_history !33
  %1 = sdiv i64 %r23, 2
  %r86.not.i = icmp sgt i64 %r6, %1
  %r91.i = shl i64 %r6, 1
  %spec.select46.i = select i1 %r86.not.i, i64 %r23, i64 %r91.i
  %spec.select.i = tail call i64 @llvm.smax.i64(i64 %spec.select46.i, i64 %p1)
  %s_v521___ast_169_grown.2.i = tail call i64 @llvm.smax.i64(i64 %spec.select.i, i64 %spec.select)
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_v170_std__lang____bootListReserve, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1407, ptr @chali_source_line, align 8
  %r53 = call i64 @chali_host_list_storage_scalar(i64 2, i64 %p0, i64 %s_v521___ast_169_grown.2.i, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind)
  br label %common.ret
}

; Function Attrs: alwaysinline uwtable
define internal noundef i64 @f_v171_std__lang____bootListRemove(i64 %p0, i64 %p1) #22 section "chali_code" {
entry:
  %collection_result_kind = alloca i8, align 1
  store ptr @f_v171_std__lang____bootListRemove, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1411, ptr @chali_source_line, align 8
  %r11 = call i64 @chali_host_list_storage_scalar(i64 0, i64 %p0, i64 2, i64 %p1, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind)
  %r1713 = add i64 %p1, 1
  %r2014 = icmp slt i64 %r1713, %r11
  br i1 %r2014, label %ast_block_0, label %ast_label_v539___chali_brk_70

ast_block_0:                                      ; preds = %entry, %ast_block_0
  %r1716 = phi i64 [ %r17, %ast_block_0 ], [ %r1713, %entry ]
  %s_v535___ast_171_i_1015.015 = phi i64 [ %r1716, %ast_block_0 ], [ %p1, %entry ]
  store ptr @f_v171_std__lang____bootListRemove, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1412, ptr @chali_source_line, align 8
  %r37 = call i64 @chali_host_list_storage_scalar(i64 3, i64 %p0, i64 %s_v535___ast_171_i_1015.015, i64 %r1716, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind)
  %r17 = add nsw i64 %r1716, 1
  store ptr @f_v171_std__lang____bootListRemove, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1412, ptr @chali_source_line, align 8
  %exitcond.not = icmp eq i64 %r17, %r11
  br i1 %exitcond.not, label %ast_label_v539___chali_brk_70, label %ast_block_0

ast_label_v539___chali_brk_70:                    ; preds = %ast_block_0, %entry
  store ptr @f_v171_std__lang____bootListRemove, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1413, ptr @chali_source_line, align 8
  %r48 = call i64 @chali_host_list_storage_scalar(i64 4, i64 %p0, i64 0, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind)
  store i8 0, ptr @chali_return_float, align 1
  ret i64 0
}

; Function Attrs: alwaysinline uwtable
define internal noundef i64 @f_v172_std__lang____bootListClear(i64 %p0) #22 section "chali_code" {
entry:
  %collection_result_kind = alloca i8, align 1
  store ptr @f_v172_std__lang____bootListClear, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1417, ptr @chali_source_line, align 8
  %r6 = call i64 @chali_host_list_storage_scalar(i64 0, i64 %p0, i64 4, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind)
  store ptr @f_v172_std__lang____bootListClear, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1418, ptr @chali_source_line, align 8
  %r105 = icmp sgt i64 %r6, 0
  br i1 %r105, label %ast_block_0, label %ast_label_v544___chali_brk_72

ast_block_0:                                      ; preds = %entry, %ast_block_0
  %s_v542___ast_172_size_1017.06 = phi i64 [ %r23, %ast_block_0 ], [ %r6, %entry ]
  store ptr @f_v172_std__lang____bootListClear, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1418, ptr @chali_source_line, align 8
  %r19 = call i64 @chali_host_list_storage_scalar(i64 4, i64 %p0, i64 0, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind)
  %r23 = add nsw i64 %s_v542___ast_172_size_1017.06, -1
  store ptr @f_v172_std__lang____bootListClear, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1418, ptr @chali_source_line, align 8
  %r10 = icmp samesign ugt i64 %s_v542___ast_172_size_1017.06, 1
  br i1 %r10, label %ast_block_0, label %ast_label_v544___chali_brk_72

ast_label_v544___chali_brk_72:                    ; preds = %ast_block_0, %entry
  store i8 0, ptr @chali_return_float, align 1
  ret i64 0
}

; Function Attrs: uwtable
define internal i64 @f_v173_std__lang____bootListCopy(i64 %p0) #23 section "chali_code" {
entry:
  %collection_result_kind = alloca i8, align 1
  store ptr @f_v173_std__lang____bootListCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1422, ptr @chali_source_line, align 8
  %r1 = call i64 @chali_host_list_storage_scalar(i64 9, i64 0, i64 0, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind)
  %r2 = load i8, ptr %collection_result_kind, align 1
  store ptr @f_v173_std__lang____bootListCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1422, ptr @chali_source_line, align 8
  %r9 = call i64 @chali_host_list_storage_scalar(i64 10, i64 %p0, i64 0, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind)
  store ptr @f_v173_std__lang____bootListCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1422, ptr @chali_source_line, align 8
  %r15 = tail call i64 @f_v170_std__lang____bootListReserve(i64 %r1, i64 %r9)
  store ptr @f_v173_std__lang____bootListCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1423, ptr @chali_source_line, align 8
  %r2113 = icmp sgt i64 %r9, 0
  br i1 %r2113, label %ast_block_0.lr.ph, label %ast_label_v551___chali_brk_74

ast_block_0.lr.ph:                                ; preds = %entry
  %r30 = zext i8 %r2 to i64
  br label %ast_block_0

ast_block_0:                                      ; preds = %ast_block_0.lr.ph, %ast_block_0
  %s_v546___ast_173_i_1021.014 = phi i64 [ 0, %ast_block_0.lr.ph ], [ %r44, %ast_block_0 ]
  store ptr @f_v173_std__lang____bootListCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1423, ptr @chali_source_line, align 8
  %r40 = call i64 @chali_host_list_storage_scalar(i64 6, i64 %r1, i64 %p0, i64 %s_v546___ast_173_i_1021.014, i64 0, i64 0, i64 %r30, ptr nonnull %collection_result_kind)
  %r44 = add nuw nsw i64 %s_v546___ast_173_i_1021.014, 1
  store ptr @f_v173_std__lang____bootListCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1423, ptr @chali_source_line, align 8
  %exitcond.not = icmp eq i64 %r44, %r9
  br i1 %exitcond.not, label %ast_label_v551___chali_brk_74, label %ast_block_0

ast_label_v551___chali_brk_74:                    ; preds = %ast_block_0, %entry
  store i8 %r2, ptr @chali_return_float, align 1
  ret i64 %r1
}

; Function Attrs: uwtable
define internal i64 @f_v174_std__lang____bootListPushI(i64 %p0, i64 %p1, i64 %p2, i64 %p3) #23 section "chali_code" {
entry:
  %collection_result_kind = alloca i8, align 1
  store ptr @f_v174_std__lang____bootListPushI, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1431, ptr @chali_source_line, align 8
  %r6 = call i64 @chali_host_list_storage_scalar(i64 0, i64 %p0, i64 0, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind)
  %r12 = add i64 %r6, 1
  store ptr @f_v174_std__lang____bootListPushI, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1431, ptr @chali_source_line, align 8
  %r13 = tail call i64 @f_v170_std__lang____bootListReserve(i64 %p0, i64 %r12)
  store ptr @f_v174_std__lang____bootListPushI, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1432, ptr @chali_source_line, align 8
  %r36 = call i64 @chali_host_list_storage_scalar(i64 7, i64 %p0, i64 %p1, i64 %p2, i64 %p3, i64 0, i64 0, ptr nonnull %collection_result_kind)
  %r37 = load i8, ptr %collection_result_kind, align 1
  store i8 %r37, ptr @chali_return_float, align 1
  ret i64 %r36
}

; Function Attrs: alwaysinline uwtable
define internal i64 @f_v176_std__lang____bootListPopI(i64 %p0, i64 %p1, i64 %p2) #22 section "chali_code" {
entry:
  %collection_result_kind = alloca i8, align 1
  store ptr @f_v176_std__lang____bootListPopI, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1442, ptr @chali_source_line, align 8
  %r6 = call i64 @chali_host_list_storage_scalar(i64 0, i64 %p0, i64 3, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind)
  store ptr @f_v176_std__lang____bootListPopI, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1442, ptr @chali_source_line, align 8
  %r24 = call i64 @chali_host_list_storage_scalar(i64 8, i64 %p0, i64 %p1, i64 %p2, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind)
  %r25 = load i8, ptr %collection_result_kind, align 1
  store i8 %r25, ptr @chali_return_float, align 1
  ret i64 %r24
}

; Function Attrs: alwaysinline nounwind uwtable
define internal range(i64 1, 0) i64 @f_v235_program__referenceAddress(i64 returned %p0) #24 section "chali_code" {
entry:
  %0 = icmp eq i64 %p0, 0
  br i1 %0, label %1, label %chali_require_reference.exit

1:                                                ; preds = %entry
  tail call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit:                     ; preds = %entry
  store i8 0, ptr @chali_return_float, align 1
  ret i64 %p0
}

; Function Attrs: uwtable
define internal noundef i64 @f_main() #23 section "chali_code" {
entry:
  %collection_result_kind.i711 = alloca i8, align 1
  %collection_result_kind.i705 = alloca i8, align 1
  %collection_result_kind.i699 = alloca i8, align 1
  %collection_result_kind.i693 = alloca i8, align 1
  %collection_result_kind.i689 = alloca i8, align 1
  %collection_result_kind.i651 = alloca i8, align 1
  %collection_result_kind.i642 = alloca i8, align 1
  %collection_result_kind.i639 = alloca i8, align 1
  %collection_result_kind.i = alloca i8, align 1
  %indirect_arguments.i568 = alloca [2 x i64], align 8
  %native_argument_types.i569 = alloca [2 x i8], align 1
  %indirect_arguments.i561 = alloca [2 x i64], align 8
  %native_argument_types.i562 = alloca [2 x i8], align 1
  %indirect_arguments.i554 = alloca [2 x i64], align 8
  %native_argument_types.i555 = alloca [2 x i8], align 1
  %indirect_arguments.i547 = alloca [2 x i64], align 8
  %native_argument_types.i548 = alloca [2 x i8], align 1
  %indirect_arguments.i540 = alloca [2 x i64], align 8
  %native_argument_types.i541 = alloca [2 x i8], align 1
  %indirect_arguments.i = alloca [2 x i64], align 8
  %native_argument_types.i = alloca [2 x i8], align 1
  %collection_result_kind = alloca i8, align 1
  %0 = tail call i64 @chali_host_allocate(i64 noundef 8) #26
  %revision_pointer.i354 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i355 = icmp eq ptr %revision_pointer.i354, null
  br i1 %has_revision.not.i355, label %slow.i370, label %check.i356

check.i356:                                       ; preds = %entry
  %revision.i357 = load atomic i64, ptr %revision_pointer.i354 acquire, align 8
  %stamp.i358 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i359 = icmp eq i64 %revision.i357, %stamp.i358
  %enabled.i360 = icmp ne i64 %revision.i357, 0
  %live.i361 = and i1 %enabled.i360, %same_revision.i359
  %begin.i362 = load i64, ptr @chali_address_view, align 16
  %size.i363 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i364 = sub i64 %0, %begin.i362
  %inside.i365 = icmp ult i64 %offset.i364, %size.i363
  %remaining.i366 = sub i64 %size.i363, %offset.i364
  %fits.i367 = icmp ugt i64 %remaining.i366, 7
  %range.i368 = and i1 %inside.i365, %fits.i367
  %valid.i369 = and i1 %live.i361, %range.i368
  br i1 %valid.i369, label %fast.i373, label %slow.i370

fast.i373:                                        ; preds = %check.i356
  %base.i374 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i375 = getelementptr i8, ptr %base.i374, i64 %offset.i364
  br label %chali_primitive_address.exit376

slow.i370:                                        ; preds = %check.i356, %entry
  %resolved.i371 = tail call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit376

chali_primitive_address.exit376:                  ; preds = %fast.i373, %slow.i370
  %common.ret.op.i372 = phi ptr [ %physical.i375, %fast.i373 ], [ %resolved.i371, %slow.i370 ]
  store i64 17, ptr %common.ret.op.i372, align 1
  %1 = icmp eq i64 %0, 0
  br i1 %1, label %2, label %f_v162_std__lang__assert_bool.exit581

2:                                                ; preds = %chali_primitive_address.exit376
  tail call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

f_v162_std__lang__assert_bool.exit581:            ; preds = %chali_primitive_address.exit376
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1390, ptr @chali_source_line, align 8
  %r50.i = tail call i64 @f_v163_std__lang__assert_bool_std__lang__String(i64 1, i64 88), !inline_history !33
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1391, ptr @chali_source_line, align 8
  %r61.i = tail call i64 @f_v163_std__lang__assert_bool_std__lang__String(i64 1, i64 120), !inline_history !33
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1390, ptr @chali_source_line, align 8
  %r50.i659 = tail call i64 @f_v163_std__lang__assert_bool_std__lang__String(i64 1, i64 88), !inline_history !33
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1391, ptr @chali_source_line, align 8
  %r61.i660 = tail call i64 @f_v163_std__lang__assert_bool_std__lang__String(i64 1, i64 120), !inline_history !33
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1390, ptr @chali_source_line, align 8
  %r50.i663 = tail call i64 @f_v163_std__lang__assert_bool_std__lang__String(i64 1, i64 88), !inline_history !33
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1391, ptr @chali_source_line, align 8
  %r61.i664 = tail call i64 @f_v163_std__lang__assert_bool_std__lang__String(i64 1, i64 120), !inline_history !33
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1390, ptr @chali_source_line, align 8
  %r50.i667 = tail call i64 @f_v163_std__lang__assert_bool_std__lang__String(i64 1, i64 88), !inline_history !33
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1391, ptr @chali_source_line, align 8
  %r61.i668 = tail call i64 @f_v163_std__lang__assert_bool_std__lang__String(i64 1, i64 120), !inline_history !33
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1390, ptr @chali_source_line, align 8
  %r50.i671 = tail call i64 @f_v163_std__lang__assert_bool_std__lang__String(i64 1, i64 88), !inline_history !33
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1391, ptr @chali_source_line, align 8
  %r61.i672 = tail call i64 @f_v163_std__lang__assert_bool_std__lang__String(i64 1, i64 120), !inline_history !33
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1390, ptr @chali_source_line, align 8
  %r50.i675 = tail call i64 @f_v163_std__lang__assert_bool_std__lang__String(i64 1, i64 88), !inline_history !33
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1391, ptr @chali_source_line, align 8
  %r61.i676 = tail call i64 @f_v163_std__lang__assert_bool_std__lang__String(i64 1, i64 120), !inline_history !33
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1390, ptr @chali_source_line, align 8
  %r50.i679 = tail call i64 @f_v163_std__lang__assert_bool_std__lang__String(i64 1, i64 88), !inline_history !33
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1391, ptr @chali_source_line, align 8
  %r61.i680 = tail call i64 @f_v163_std__lang__assert_bool_std__lang__String(i64 1, i64 120), !inline_history !33
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1390, ptr @chali_source_line, align 8
  %r50.i683 = tail call i64 @f_v163_std__lang__assert_bool_std__lang__String(i64 1, i64 88), !inline_history !33
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1391, ptr @chali_source_line, align 8
  %r61.i684 = tail call i64 @f_v163_std__lang__assert_bool_std__lang__String(i64 1, i64 120), !inline_history !33
  store ptr @f_main, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 16, ptr @chali_source_line, align 8
  store i8 0, ptr @chali_return_float, align 1
  %r67 = call i64 @chali_host_list_storage_scalar(i64 9, i64 0, i64 0, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind)
  %3 = tail call i64 @chali_host_allocate(i64 noundef 8) #26
  %revision_pointer.i331 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i332 = icmp eq ptr %revision_pointer.i331, null
  br i1 %has_revision.not.i332, label %slow.i347, label %check.i333

check.i333:                                       ; preds = %f_v162_std__lang__assert_bool.exit581
  %revision.i334 = load atomic i64, ptr %revision_pointer.i331 acquire, align 8
  %stamp.i335 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i336 = icmp eq i64 %revision.i334, %stamp.i335
  %enabled.i337 = icmp ne i64 %revision.i334, 0
  %live.i338 = and i1 %enabled.i337, %same_revision.i336
  %begin.i339 = load i64, ptr @chali_address_view, align 16
  %size.i340 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i341 = sub i64 %3, %begin.i339
  %inside.i342 = icmp ult i64 %offset.i341, %size.i340
  %remaining.i343 = sub i64 %size.i340, %offset.i341
  %fits.i344 = icmp ugt i64 %remaining.i343, 7
  %range.i345 = and i1 %inside.i342, %fits.i344
  %valid.i346 = and i1 %live.i338, %range.i345
  br i1 %valid.i346, label %fast.i350, label %slow.i347

fast.i350:                                        ; preds = %check.i333
  %base.i351 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i352 = getelementptr i8, ptr %base.i351, i64 %offset.i341
  br label %chali_primitive_address.exit353

slow.i347:                                        ; preds = %check.i333, %f_v162_std__lang__assert_bool.exit581
  %resolved.i348 = tail call ptr @chali_host_address_view(i64 %3, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit353

chali_primitive_address.exit353:                  ; preds = %fast.i350, %slow.i347
  %common.ret.op.i349 = phi ptr [ %physical.i352, %fast.i350 ], [ %resolved.i348, %slow.i347 ]
  store i64 %r67, ptr %common.ret.op.i349, align 1
  %4 = icmp eq i64 %3, 0
  br i1 %4, label %5, label %chali_require_reference.exit688

5:                                                ; preds = %chali_primitive_address.exit353
  tail call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit688:                  ; preds = %chali_primitive_address.exit353
  store ptr @f_main, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 20, ptr @chali_source_line, align 8
  %revision_pointer.i308 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i309 = icmp eq ptr %revision_pointer.i308, null
  br i1 %has_revision.not.i309, label %slow.i324, label %check.i310

check.i310:                                       ; preds = %chali_require_reference.exit688
  %revision.i311 = load atomic i64, ptr %revision_pointer.i308 acquire, align 8
  %stamp.i312 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i313 = icmp eq i64 %revision.i311, %stamp.i312
  %enabled.i314 = icmp ne i64 %revision.i311, 0
  %live.i315 = and i1 %enabled.i314, %same_revision.i313
  %begin.i316 = load i64, ptr @chali_address_view, align 16
  %size.i317 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i318 = sub i64 %3, %begin.i316
  %inside.i319 = icmp ult i64 %offset.i318, %size.i317
  %remaining.i320 = sub i64 %size.i317, %offset.i318
  %fits.i321 = icmp ugt i64 %remaining.i320, 7
  %range.i322 = and i1 %inside.i319, %fits.i321
  %valid.i323 = and i1 %live.i315, %range.i322
  br i1 %valid.i323, label %fast.i327, label %slow.i324

fast.i327:                                        ; preds = %check.i310
  %base.i328 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i329 = getelementptr i8, ptr %base.i328, i64 %offset.i318
  br label %chali_primitive_address.exit330

slow.i324:                                        ; preds = %check.i310, %chali_require_reference.exit688
  %resolved.i325 = tail call ptr @chali_host_address_view(i64 %3, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit330

chali_primitive_address.exit330:                  ; preds = %fast.i327, %slow.i324
  %common.ret.op.i326 = phi ptr [ %physical.i329, %fast.i327 ], [ %resolved.i325, %slow.i324 ]
  %r83 = load i64, ptr %common.ret.op.i326, align 1
  store ptr @f_main, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 21, ptr @chali_source_line, align 8
  %r90 = call i64 @chali_host_list_storage_scalar(i64 17, i64 %r83, i64 0, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind)
  %r94 = icmp sgt i64 %r90, 8
  br i1 %r94, label %ast_block_2, label %ast_block_2.thread

ast_block_2.thread:                               ; preds = %chali_primitive_address.exit330
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i568)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i569)
  br label %ast_block_0.i571

ast_block_2:                                      ; preds = %chali_primitive_address.exit330
  %r99.not = icmp eq i64 %r90, 9223372036854775807
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i568)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i569)
  br i1 %r99.not, label %ast_block_0.i571, label %f_v162_std__lang__assert_bool.exit574

ast_block_0.i571:                                 ; preds = %ast_block_2.thread, %ast_block_2
  store ptr @f_v162_std__lang__assert_bool, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 782, ptr @chali_source_line, align 8
  store i64 176, ptr %indirect_arguments.i568, align 8
  store i8 0, ptr %native_argument_types.i569, align 1
  %r8.i572 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 1, ptr nonnull %indirect_arguments.i568, ptr nonnull %native_argument_types.i569)
  %r9.i573 = call i8 @chali_host_result_float()
  br label %f_v162_std__lang__assert_bool.exit574

f_v162_std__lang__assert_bool.exit574:            ; preds = %ast_block_2, %ast_block_0.i571
  store i8 0, ptr @chali_return_float, align 1
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i568)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i569)
  store ptr @f_main, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 23, ptr @chali_source_line, align 8
  %r108 = call i64 @f_v170_std__lang____bootListReserve(i64 %r83, i64 33)
  store ptr @f_main, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 24, ptr @chali_source_line, align 8
  %r116 = call i64 @chali_host_list_storage_scalar(i64 1, i64 %r83, i64 0, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind)
  %r118.not = icmp eq i64 %r116, 33
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i561)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i562)
  br i1 %r118.not, label %f_v162_std__lang__assert_bool.exit567, label %ast_block_0.i564

ast_block_0.i564:                                 ; preds = %f_v162_std__lang__assert_bool.exit574
  store ptr @f_v162_std__lang__assert_bool, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 782, ptr @chali_source_line, align 8
  store i64 176, ptr %indirect_arguments.i561, align 8
  store i8 0, ptr %native_argument_types.i562, align 1
  %r8.i565 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 1, ptr nonnull %indirect_arguments.i561, ptr nonnull %native_argument_types.i562)
  %r9.i566 = call i8 @chali_host_result_float()
  br label %f_v162_std__lang__assert_bool.exit567

f_v162_std__lang__assert_bool.exit567:            ; preds = %f_v162_std__lang__assert_bool.exit574, %ast_block_0.i564
  store i8 0, ptr @chali_return_float, align 1
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i561)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i562)
  br label %ast_block_4

ast_block_4:                                      ; preds = %f_v162_std__lang__assert_bool.exit567, %chali_primitive_address.exit307
  %s_v571___ast_236_i_1308.0750 = phi i64 [ 0, %f_v162_std__lang__assert_bool.exit567 ], [ %r137, %chali_primitive_address.exit307 ]
  store ptr @f_main, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 27, ptr @chali_source_line, align 8
  %revision_pointer.i285 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i286 = icmp eq ptr %revision_pointer.i285, null
  br i1 %has_revision.not.i286, label %slow.i301, label %check.i287

check.i287:                                       ; preds = %ast_block_4
  %revision.i288 = load atomic i64, ptr %revision_pointer.i285 acquire, align 8
  %stamp.i289 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i290 = icmp eq i64 %revision.i288, %stamp.i289
  %enabled.i291 = icmp ne i64 %revision.i288, 0
  %live.i292 = and i1 %enabled.i291, %same_revision.i290
  %begin.i293 = load i64, ptr @chali_address_view, align 16
  %size.i294 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i295 = sub i64 %3, %begin.i293
  %inside.i296 = icmp ult i64 %offset.i295, %size.i294
  %remaining.i297 = sub i64 %size.i294, %offset.i295
  %fits.i298 = icmp ugt i64 %remaining.i297, 7
  %range.i299 = and i1 %inside.i296, %fits.i298
  %valid.i300 = and i1 %live.i292, %range.i299
  br i1 %valid.i300, label %fast.i304, label %slow.i301

fast.i304:                                        ; preds = %check.i287
  %base.i305 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i306 = getelementptr i8, ptr %base.i305, i64 %offset.i295
  br label %chali_primitive_address.exit307

slow.i301:                                        ; preds = %check.i287, %ast_block_4
  %resolved.i302 = call ptr @chali_host_address_view(i64 %3, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit307

chali_primitive_address.exit307:                  ; preds = %fast.i304, %slow.i301
  %common.ret.op.i303 = phi ptr [ %physical.i306, %fast.i304 ], [ %resolved.i302, %slow.i301 ]
  %r130 = load i64, ptr %common.ret.op.i303, align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %collection_result_kind.i689)
  store ptr @f_v174_std__lang____bootListPushI, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1431, ptr @chali_source_line, align 8
  %r6.i690 = call i64 @chali_host_list_storage_scalar(i64 0, i64 %r130, i64 0, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind.i689), !inline_history !34
  %r12.i = add i64 %r6.i690, 1
  store ptr @f_v174_std__lang____bootListPushI, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1431, ptr @chali_source_line, align 8
  %r13.i691 = call i64 @f_v170_std__lang____bootListReserve(i64 %r130, i64 %r12.i), !inline_history !34
  store ptr @f_v174_std__lang____bootListPushI, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1432, ptr @chali_source_line, align 8
  %r36.i = call i64 @chali_host_list_storage_scalar(i64 7, i64 %r130, i64 %s_v571___ast_236_i_1308.0750, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind.i689), !inline_history !34
  %r37.i692 = load i8, ptr %collection_result_kind.i689, align 1
  store i8 %r37.i692, ptr @chali_return_float, align 1
  call void @llvm.lifetime.end.p0(ptr nonnull %collection_result_kind.i689)
  %r137 = add nuw nsw i64 %s_v571___ast_236_i_1308.0750, 1
  store ptr @f_main, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 27, ptr @chali_source_line, align 8
  %exitcond.not = icmp eq i64 %r137, 33
  br i1 %exitcond.not, label %ast_label_v579___chali_brk_138, label %ast_block_4

ast_label_v579___chali_brk_138:                   ; preds = %chali_primitive_address.exit307
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 28, ptr @chali_source_line, align 8
  %revision_pointer.i262 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i263 = icmp eq ptr %revision_pointer.i262, null
  br i1 %has_revision.not.i263, label %slow.i278, label %check.i264

check.i264:                                       ; preds = %ast_label_v579___chali_brk_138
  %revision.i265 = load atomic i64, ptr %revision_pointer.i262 acquire, align 8
  %stamp.i266 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i267 = icmp eq i64 %revision.i265, %stamp.i266
  %enabled.i268 = icmp ne i64 %revision.i265, 0
  %live.i269 = and i1 %enabled.i268, %same_revision.i267
  %begin.i270 = load i64, ptr @chali_address_view, align 16
  %size.i271 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i272 = sub i64 %3, %begin.i270
  %inside.i273 = icmp ult i64 %offset.i272, %size.i271
  %remaining.i274 = sub i64 %size.i271, %offset.i272
  %fits.i275 = icmp ugt i64 %remaining.i274, 7
  %range.i276 = and i1 %inside.i273, %fits.i275
  %valid.i277 = and i1 %live.i269, %range.i276
  br i1 %valid.i277, label %fast.i281, label %slow.i278

fast.i281:                                        ; preds = %check.i264
  %base.i282 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i283 = getelementptr i8, ptr %base.i282, i64 %offset.i272
  br label %chali_primitive_address.exit284

slow.i278:                                        ; preds = %check.i264, %ast_label_v579___chali_brk_138
  %resolved.i279 = call ptr @chali_host_address_view(i64 %3, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit284

chali_primitive_address.exit284:                  ; preds = %fast.i281, %slow.i278
  %common.ret.op.i280 = phi ptr [ %physical.i283, %fast.i281 ], [ %resolved.i279, %slow.i278 ]
  %r141 = load i64, ptr %common.ret.op.i280, align 1
  %r142 = call i64 @f_v173_std__lang____bootListCopy(i64 %r141)
  %r143 = load i8, ptr @chali_return_float, align 1
  store ptr @f_main, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 29, ptr @chali_source_line, align 8
  %revision_pointer.i239 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i240 = icmp eq ptr %revision_pointer.i239, null
  br i1 %has_revision.not.i240, label %slow.i255, label %check.i241

check.i241:                                       ; preds = %chali_primitive_address.exit284
  %revision.i242 = load atomic i64, ptr %revision_pointer.i239 acquire, align 8
  %stamp.i243 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i244 = icmp eq i64 %revision.i242, %stamp.i243
  %enabled.i245 = icmp ne i64 %revision.i242, 0
  %live.i246 = and i1 %enabled.i245, %same_revision.i244
  %begin.i247 = load i64, ptr @chali_address_view, align 16
  %size.i248 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i249 = sub i64 %3, %begin.i247
  %inside.i250 = icmp ult i64 %offset.i249, %size.i248
  %remaining.i251 = sub i64 %size.i248, %offset.i249
  %fits.i252 = icmp ugt i64 %remaining.i251, 7
  %range.i253 = and i1 %inside.i250, %fits.i252
  %valid.i254 = and i1 %live.i246, %range.i253
  br i1 %valid.i254, label %fast.i258, label %slow.i255

fast.i258:                                        ; preds = %check.i241
  %base.i259 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i260 = getelementptr i8, ptr %base.i259, i64 %offset.i249
  br label %chali_primitive_address.exit261

slow.i255:                                        ; preds = %check.i241, %chali_primitive_address.exit284
  %resolved.i256 = call ptr @chali_host_address_view(i64 %3, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit261

chali_primitive_address.exit261:                  ; preds = %fast.i258, %slow.i255
  %common.ret.op.i257 = phi ptr [ %physical.i260, %fast.i258 ], [ %resolved.i256, %slow.i255 ]
  %r147 = load i64, ptr %common.ret.op.i257, align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %collection_result_kind.i)
  store ptr @f_v171_std__lang____bootListRemove, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1411, ptr @chali_source_line, align 8
  %r11.i = call i64 @chali_host_list_storage_scalar(i64 0, i64 %r147, i64 2, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind.i)
  %r20.i751 = icmp sgt i64 %r11.i, 1
  br i1 %r20.i751, label %ast_block_0.i638, label %f_v171_std__lang____bootListRemove.exit

ast_block_0.i638:                                 ; preds = %chali_primitive_address.exit261, %ast_block_0.i638
  %r17.i753 = phi i64 [ %r17.i, %ast_block_0.i638 ], [ 1, %chali_primitive_address.exit261 ]
  %s_v535___ast_171_i_1015.0.i752 = phi i64 [ %r17.i753, %ast_block_0.i638 ], [ 0, %chali_primitive_address.exit261 ]
  store ptr @f_v171_std__lang____bootListRemove, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1412, ptr @chali_source_line, align 8
  %r37.i = call i64 @chali_host_list_storage_scalar(i64 3, i64 %r147, i64 %s_v535___ast_171_i_1015.0.i752, i64 %r17.i753, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind.i)
  %r17.i = add nuw nsw i64 %r17.i753, 1
  store ptr @f_v171_std__lang____bootListRemove, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1412, ptr @chali_source_line, align 8
  %exitcond764.not = icmp eq i64 %r17.i, %r11.i
  br i1 %exitcond764.not, label %f_v171_std__lang____bootListRemove.exit, label %ast_block_0.i638

f_v171_std__lang____bootListRemove.exit:          ; preds = %ast_block_0.i638, %chali_primitive_address.exit261
  store ptr @f_v171_std__lang____bootListRemove, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1413, ptr @chali_source_line, align 8
  %r48.i = call i64 @chali_host_list_storage_scalar(i64 4, i64 %r147, i64 0, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind.i)
  store i8 0, ptr @chali_return_float, align 1
  call void @llvm.lifetime.end.p0(ptr nonnull %collection_result_kind.i)
  store ptr @f_main, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 29, ptr @chali_source_line, align 8
  %revision_pointer.i216 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i217 = icmp eq ptr %revision_pointer.i216, null
  br i1 %has_revision.not.i217, label %slow.i232, label %check.i218

check.i218:                                       ; preds = %f_v171_std__lang____bootListRemove.exit
  %revision.i219 = load atomic i64, ptr %revision_pointer.i216 acquire, align 8
  %stamp.i220 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i221 = icmp eq i64 %revision.i219, %stamp.i220
  %enabled.i222 = icmp ne i64 %revision.i219, 0
  %live.i223 = and i1 %enabled.i222, %same_revision.i221
  %begin.i224 = load i64, ptr @chali_address_view, align 16
  %size.i225 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i226 = sub i64 %3, %begin.i224
  %inside.i227 = icmp ult i64 %offset.i226, %size.i225
  %remaining.i228 = sub i64 %size.i225, %offset.i226
  %fits.i229 = icmp ugt i64 %remaining.i228, 7
  %range.i230 = and i1 %inside.i227, %fits.i229
  %valid.i231 = and i1 %live.i223, %range.i230
  br i1 %valid.i231, label %fast.i235, label %slow.i232

fast.i235:                                        ; preds = %check.i218
  %base.i236 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i237 = getelementptr i8, ptr %base.i236, i64 %offset.i226
  br label %chali_primitive_address.exit238

slow.i232:                                        ; preds = %check.i218, %f_v171_std__lang____bootListRemove.exit
  %resolved.i233 = call ptr @chali_host_address_view(i64 %3, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit238

chali_primitive_address.exit238:                  ; preds = %fast.i235, %slow.i232
  %common.ret.op.i234 = phi ptr [ %physical.i237, %fast.i235 ], [ %resolved.i233, %slow.i232 ]
  %r153 = load i64, ptr %common.ret.op.i234, align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %collection_result_kind.i651)
  store ptr @f_v176_std__lang____bootListPopI, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1442, ptr @chali_source_line, align 8
  %r6.i652 = call i64 @chali_host_list_storage_scalar(i64 0, i64 %r153, i64 3, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind.i651)
  store ptr @f_v176_std__lang____bootListPopI, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1442, ptr @chali_source_line, align 8
  %r24.i = call i64 @chali_host_list_storage_scalar(i64 8, i64 %r153, i64 0, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind.i651)
  %r25.i653 = load i8, ptr %collection_result_kind.i651, align 1
  store i8 %r25.i653, ptr @chali_return_float, align 1
  call void @llvm.lifetime.end.p0(ptr nonnull %collection_result_kind.i651)
  store ptr @f_main, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 29, ptr @chali_source_line, align 8
  %revision_pointer.i193 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i194 = icmp eq ptr %revision_pointer.i193, null
  br i1 %has_revision.not.i194, label %slow.i209, label %check.i195

check.i195:                                       ; preds = %chali_primitive_address.exit238
  %revision.i196 = load atomic i64, ptr %revision_pointer.i193 acquire, align 8
  %stamp.i197 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i198 = icmp eq i64 %revision.i196, %stamp.i197
  %enabled.i199 = icmp ne i64 %revision.i196, 0
  %live.i200 = and i1 %enabled.i199, %same_revision.i198
  %begin.i201 = load i64, ptr @chali_address_view, align 16
  %size.i202 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i203 = sub i64 %3, %begin.i201
  %inside.i204 = icmp ult i64 %offset.i203, %size.i202
  %remaining.i205 = sub i64 %size.i202, %offset.i203
  %fits.i206 = icmp ugt i64 %remaining.i205, 7
  %range.i207 = and i1 %inside.i204, %fits.i206
  %valid.i208 = and i1 %live.i200, %range.i207
  br i1 %valid.i208, label %fast.i212, label %slow.i209

fast.i212:                                        ; preds = %check.i195
  %base.i213 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i214 = getelementptr i8, ptr %base.i213, i64 %offset.i203
  br label %chali_primitive_address.exit215

slow.i209:                                        ; preds = %check.i195, %chali_primitive_address.exit238
  %resolved.i210 = call ptr @chali_host_address_view(i64 %3, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit215

chali_primitive_address.exit215:                  ; preds = %fast.i212, %slow.i209
  %common.ret.op.i211 = phi ptr [ %physical.i214, %fast.i212 ], [ %resolved.i210, %slow.i209 ]
  %r159 = load i64, ptr %common.ret.op.i211, align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %collection_result_kind.i642)
  store ptr @f_v172_std__lang____bootListClear, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1417, ptr @chali_source_line, align 8
  %r6.i643 = call i64 @chali_host_list_storage_scalar(i64 0, i64 %r159, i64 4, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind.i642)
  %r10.i646754 = icmp sgt i64 %r6.i643, 0
  br i1 %r10.i646754, label %ast_block_0.i647, label %f_v172_std__lang____bootListClear.exit650

ast_block_0.i647:                                 ; preds = %chali_primitive_address.exit215, %ast_block_0.i647
  %s_v542___ast_172_size_1017.0.i645755 = phi i64 [ %r23.i649, %ast_block_0.i647 ], [ %r6.i643, %chali_primitive_address.exit215 ]
  store ptr @f_v172_std__lang____bootListClear, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1418, ptr @chali_source_line, align 8
  %r19.i648 = call i64 @chali_host_list_storage_scalar(i64 4, i64 %r159, i64 0, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind.i642)
  %r23.i649 = add nsw i64 %s_v542___ast_172_size_1017.0.i645755, -1
  store ptr @f_v172_std__lang____bootListClear, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1418, ptr @chali_source_line, align 8
  %r10.i646 = icmp samesign ugt i64 %s_v542___ast_172_size_1017.0.i645755, 1
  br i1 %r10.i646, label %ast_block_0.i647, label %f_v172_std__lang____bootListClear.exit650

f_v172_std__lang____bootListClear.exit650:        ; preds = %ast_block_0.i647, %chali_primitive_address.exit215
  store i8 0, ptr @chali_return_float, align 1
  call void @llvm.lifetime.end.p0(ptr nonnull %collection_result_kind.i642)
  store ptr @f_main, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 30, ptr @chali_source_line, align 8
  %r168 = call i64 @chali_host_list_storage_scalar(i64 1, i64 %r83, i64 0, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind)
  %r170.not = icmp eq i64 %r168, 33
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i554)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i555)
  br i1 %r170.not, label %f_v162_std__lang__assert_bool.exit560, label %ast_block_0.i557

ast_block_0.i557:                                 ; preds = %f_v172_std__lang____bootListClear.exit650
  store ptr @f_v162_std__lang__assert_bool, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 782, ptr @chali_source_line, align 8
  store i64 176, ptr %indirect_arguments.i554, align 8
  store i8 0, ptr %native_argument_types.i555, align 1
  %r8.i558 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 1, ptr nonnull %indirect_arguments.i554, ptr nonnull %native_argument_types.i555)
  %r9.i559 = call i8 @chali_host_result_float()
  br label %f_v162_std__lang__assert_bool.exit560

f_v162_std__lang__assert_bool.exit560:            ; preds = %f_v172_std__lang____bootListClear.exit650, %ast_block_0.i557
  store i8 0, ptr @chali_return_float, align 1
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i554)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i555)
  store ptr @f_main, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 31, ptr @chali_source_line, align 8
  %revision_pointer.i170 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i171 = icmp eq ptr %revision_pointer.i170, null
  br i1 %has_revision.not.i171, label %slow.i186, label %check.i172

check.i172:                                       ; preds = %f_v162_std__lang__assert_bool.exit560
  %revision.i173 = load atomic i64, ptr %revision_pointer.i170 acquire, align 8
  %stamp.i174 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i175 = icmp eq i64 %revision.i173, %stamp.i174
  %enabled.i176 = icmp ne i64 %revision.i173, 0
  %live.i177 = and i1 %enabled.i176, %same_revision.i175
  %begin.i178 = load i64, ptr @chali_address_view, align 16
  %size.i179 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i180 = sub i64 %3, %begin.i178
  %inside.i181 = icmp ult i64 %offset.i180, %size.i179
  %remaining.i182 = sub i64 %size.i179, %offset.i180
  %fits.i183 = icmp ugt i64 %remaining.i182, 7
  %range.i184 = and i1 %inside.i181, %fits.i183
  %valid.i185 = and i1 %live.i177, %range.i184
  br i1 %valid.i185, label %fast.i189, label %slow.i186

fast.i189:                                        ; preds = %check.i172
  %base.i190 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i191 = getelementptr i8, ptr %base.i190, i64 %offset.i180
  br label %chali_primitive_address.exit192

slow.i186:                                        ; preds = %check.i172, %f_v162_std__lang__assert_bool.exit560
  %resolved.i187 = call ptr @chali_host_address_view(i64 %3, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit192

chali_primitive_address.exit192:                  ; preds = %fast.i189, %slow.i186
  %common.ret.op.i188 = phi ptr [ %physical.i191, %fast.i189 ], [ %resolved.i187, %slow.i186 ]
  %r177 = load i64, ptr %common.ret.op.i188, align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %collection_result_kind.i693)
  store ptr @f_v174_std__lang____bootListPushI, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1431, ptr @chali_source_line, align 8
  %r6.i694 = call i64 @chali_host_list_storage_scalar(i64 0, i64 %r177, i64 0, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind.i693), !inline_history !34
  %r12.i695 = add i64 %r6.i694, 1
  store ptr @f_v174_std__lang____bootListPushI, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1431, ptr @chali_source_line, align 8
  %r13.i696 = call i64 @f_v170_std__lang____bootListReserve(i64 %r177, i64 %r12.i695), !inline_history !34
  store ptr @f_v174_std__lang____bootListPushI, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1432, ptr @chali_source_line, align 8
  %r36.i697 = call i64 @chali_host_list_storage_scalar(i64 7, i64 %r177, i64 99, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind.i693), !inline_history !34
  %r37.i698 = load i8, ptr %collection_result_kind.i693, align 1
  store i8 %r37.i698, ptr @chali_return_float, align 1
  call void @llvm.lifetime.end.p0(ptr nonnull %collection_result_kind.i693)
  store ptr @f_main, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 32, ptr @chali_source_line, align 8
  %revision_pointer.i147 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i148 = icmp eq ptr %revision_pointer.i147, null
  br i1 %has_revision.not.i148, label %slow.i163, label %check.i149

check.i149:                                       ; preds = %chali_primitive_address.exit192
  %revision.i150 = load atomic i64, ptr %revision_pointer.i147 acquire, align 8
  %stamp.i151 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i152 = icmp eq i64 %revision.i150, %stamp.i151
  %enabled.i153 = icmp ne i64 %revision.i150, 0
  %live.i154 = and i1 %enabled.i153, %same_revision.i152
  %begin.i155 = load i64, ptr @chali_address_view, align 16
  %size.i156 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i157 = sub i64 %3, %begin.i155
  %inside.i158 = icmp ult i64 %offset.i157, %size.i156
  %remaining.i159 = sub i64 %size.i156, %offset.i157
  %fits.i160 = icmp ugt i64 %remaining.i159, 7
  %range.i161 = and i1 %inside.i158, %fits.i160
  %valid.i162 = and i1 %live.i154, %range.i161
  br i1 %valid.i162, label %fast.i166, label %slow.i163

fast.i166:                                        ; preds = %check.i149
  %base.i167 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i168 = getelementptr i8, ptr %base.i167, i64 %offset.i157
  br label %chali_primitive_address.exit169

slow.i163:                                        ; preds = %check.i149, %chali_primitive_address.exit192
  %resolved.i164 = call ptr @chali_host_address_view(i64 %3, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit169

chali_primitive_address.exit169:                  ; preds = %fast.i166, %slow.i163
  %common.ret.op.i165 = phi ptr [ %physical.i168, %fast.i166 ], [ %resolved.i164, %slow.i163 ]
  %r183 = load i64, ptr %common.ret.op.i165, align 1
  store ptr @f_main, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 32, ptr @chali_source_line, align 8
  %r185 = call i64 @chali_host_list_storage_scalar(i64 11, i64 %r183, i64 0, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind)
  %r187 = icmp eq i64 %r185, 99
  br i1 %r187, label %ast_block_8, label %ast_block_17.thread

ast_block_8:                                      ; preds = %chali_primitive_address.exit169
  store ptr @f_main, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 32, ptr @chali_source_line, align 8
  %r192 = zext i8 %r143 to i64
  %r196 = call i64 @chali_host_list_storage_scalar(i64 10, i64 %r142, i64 0, i64 0, i64 0, i64 0, i64 %r192, ptr nonnull %collection_result_kind)
  %r198 = icmp eq i64 %r196, 33
  br i1 %r198, label %ast_block_12, label %ast_block_17.thread

ast_block_12:                                     ; preds = %ast_block_8
  store ptr @f_main, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 32, ptr @chali_source_line, align 8
  %r210 = call i64 @chali_host_list_storage_scalar(i64 11, i64 %r142, i64 0, i64 0, i64 0, i64 0, i64 %r192, ptr nonnull %collection_result_kind)
  %r212 = icmp eq i64 %r210, 0
  br i1 %r212, label %ast_block_17, label %ast_block_17.thread

ast_block_17.thread:                              ; preds = %ast_block_12, %chali_primitive_address.exit169, %ast_block_8
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i547)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i548)
  br label %ast_block_0.i550

ast_block_17:                                     ; preds = %ast_block_12
  store ptr @f_main, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 32, ptr @chali_source_line, align 8
  %r224 = call i64 @chali_host_list_storage_scalar(i64 11, i64 %r142, i64 32, i64 0, i64 0, i64 0, i64 %r192, ptr nonnull %collection_result_kind)
  %r226.not = icmp eq i64 %r224, 32
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i547)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i548)
  br i1 %r226.not, label %f_v162_std__lang__assert_bool.exit553, label %ast_block_0.i550

ast_block_0.i550:                                 ; preds = %ast_block_17.thread, %ast_block_17
  store ptr @f_v162_std__lang__assert_bool, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 782, ptr @chali_source_line, align 8
  store i64 176, ptr %indirect_arguments.i547, align 8
  store i8 0, ptr %native_argument_types.i548, align 1
  %r8.i551 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 1, ptr nonnull %indirect_arguments.i547, ptr nonnull %native_argument_types.i548)
  %r9.i552 = call i8 @chali_host_result_float()
  br label %f_v162_std__lang__assert_bool.exit553

f_v162_std__lang__assert_bool.exit553:            ; preds = %ast_block_17, %ast_block_0.i550
  store i8 0, ptr @chali_return_float, align 1
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i547)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i548)
  store ptr @f_main, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 33, ptr @chali_source_line, align 8
  %r239 = call i64 @chali_host_list_storage_scalar(i64 1, i64 %r83, i64 0, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind)
  %r241.not = icmp eq i64 %r239, 33
  store ptr @f_main, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 33, ptr @chali_source_line, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i540)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i541)
  br i1 %r241.not, label %f_v162_std__lang__assert_bool.exit546, label %ast_block_0.i543

ast_block_0.i543:                                 ; preds = %f_v162_std__lang__assert_bool.exit553
  store ptr @f_v162_std__lang__assert_bool, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 782, ptr @chali_source_line, align 8
  store i64 176, ptr %indirect_arguments.i540, align 8
  store i8 0, ptr %native_argument_types.i541, align 1
  %r8.i544 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 1, ptr nonnull %indirect_arguments.i540, ptr nonnull %native_argument_types.i541)
  %r9.i545 = call i8 @chali_host_result_float()
  br label %f_v162_std__lang__assert_bool.exit546

f_v162_std__lang__assert_bool.exit546:            ; preds = %f_v162_std__lang__assert_bool.exit553, %ast_block_0.i543
  store i8 0, ptr @chali_return_float, align 1
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i540)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i541)
  %r246 = call i64 @chali_host_list_storage_scalar(i64 9, i64 0, i64 0, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind)
  %r247 = load i8, ptr %collection_result_kind, align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %collection_result_kind.i699)
  store ptr @f_v174_std__lang____bootListPushI, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1431, ptr @chali_source_line, align 8
  %r6.i700 = call i64 @chali_host_list_storage_scalar(i64 0, i64 %r246, i64 0, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind.i699), !inline_history !34
  %r12.i701 = add i64 %r6.i700, 1
  store ptr @f_v174_std__lang____bootListPushI, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1431, ptr @chali_source_line, align 8
  %r13.i702 = call i64 @f_v170_std__lang____bootListReserve(i64 %r246, i64 %r12.i701), !inline_history !34
  store ptr @f_v174_std__lang____bootListPushI, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1432, ptr @chali_source_line, align 8
  %r36.i703 = call i64 @chali_host_list_storage_scalar(i64 7, i64 %r246, i64 8, i64 1, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind.i699), !inline_history !34
  %r37.i704 = load i8, ptr %collection_result_kind.i699, align 1
  store i8 %r37.i704, ptr @chali_return_float, align 1
  call void @llvm.lifetime.end.p0(ptr nonnull %collection_result_kind.i699)
  call void @llvm.lifetime.start.p0(ptr nonnull %collection_result_kind.i705)
  store ptr @f_v174_std__lang____bootListPushI, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1431, ptr @chali_source_line, align 8
  %r6.i706 = call i64 @chali_host_list_storage_scalar(i64 0, i64 %r246, i64 0, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind.i705), !inline_history !34
  %r12.i707 = add i64 %r6.i706, 1
  store ptr @f_v174_std__lang____bootListPushI, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1431, ptr @chali_source_line, align 8
  %r13.i708 = call i64 @f_v170_std__lang____bootListReserve(i64 %r246, i64 %r12.i707), !inline_history !34
  store ptr @f_v174_std__lang____bootListPushI, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1432, ptr @chali_source_line, align 8
  %r36.i709 = call i64 @chali_host_list_storage_scalar(i64 7, i64 %r246, i64 16, i64 1, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind.i705), !inline_history !34
  %r37.i710 = load i8, ptr %collection_result_kind.i705, align 1
  store i8 %r37.i710, ptr @chali_return_float, align 1
  call void @llvm.lifetime.end.p0(ptr nonnull %collection_result_kind.i705)
  %r258 = call i64 @f_v173_std__lang____bootListCopy(i64 %r246)
  %r259 = load i8, ptr @chali_return_float, align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %collection_result_kind.i639)
  store ptr @f_v172_std__lang____bootListClear, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1417, ptr @chali_source_line, align 8
  %r6.i = call i64 @chali_host_list_storage_scalar(i64 0, i64 %r246, i64 4, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind.i639)
  %r10.i640756 = icmp sgt i64 %r6.i, 0
  br i1 %r10.i640756, label %ast_block_0.i641, label %f_v172_std__lang____bootListClear.exit

ast_block_0.i641:                                 ; preds = %f_v162_std__lang__assert_bool.exit546, %ast_block_0.i641
  %s_v542___ast_172_size_1017.0.i757 = phi i64 [ %r23.i, %ast_block_0.i641 ], [ %r6.i, %f_v162_std__lang__assert_bool.exit546 ]
  store ptr @f_v172_std__lang____bootListClear, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1418, ptr @chali_source_line, align 8
  %r19.i = call i64 @chali_host_list_storage_scalar(i64 4, i64 %r246, i64 0, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind.i639)
  %r23.i = add nsw i64 %s_v542___ast_172_size_1017.0.i757, -1
  store ptr @f_v172_std__lang____bootListClear, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1418, ptr @chali_source_line, align 8
  %r10.i640 = icmp samesign ugt i64 %s_v542___ast_172_size_1017.0.i757, 1
  br i1 %r10.i640, label %ast_block_0.i641, label %f_v172_std__lang____bootListClear.exit

f_v172_std__lang____bootListClear.exit:           ; preds = %ast_block_0.i641, %f_v162_std__lang__assert_bool.exit546
  store i8 0, ptr @chali_return_float, align 1
  call void @llvm.lifetime.end.p0(ptr nonnull %collection_result_kind.i639)
  call void @llvm.lifetime.start.p0(ptr nonnull %collection_result_kind.i711)
  store ptr @f_v174_std__lang____bootListPushI, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1431, ptr @chali_source_line, align 8
  %r6.i712 = call i64 @chali_host_list_storage_scalar(i64 0, i64 %r246, i64 0, i64 0, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind.i711), !inline_history !34
  %r12.i713 = add i64 %r6.i712, 1
  store ptr @f_v174_std__lang____bootListPushI, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1431, ptr @chali_source_line, align 8
  %r13.i714 = call i64 @f_v170_std__lang____bootListReserve(i64 %r246, i64 %r12.i713), !inline_history !34
  store ptr @f_v174_std__lang____bootListPushI, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1432, ptr @chali_source_line, align 8
  %r36.i715 = call i64 @chali_host_list_storage_scalar(i64 7, i64 %r246, i64 24, i64 1, i64 0, i64 0, i64 0, ptr nonnull %collection_result_kind.i711), !inline_history !34
  %r37.i716 = load i8, ptr %collection_result_kind.i711, align 1
  store i8 %r37.i716, ptr @chali_return_float, align 1
  call void @llvm.lifetime.end.p0(ptr nonnull %collection_result_kind.i711)
  store ptr @f_main, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 36, ptr @chali_source_line, align 8
  %r279 = zext i8 %r259 to i64
  %r283 = call i64 @chali_host_list_storage_scalar(i64 11, i64 %r258, i64 0, i64 1, i64 0, i64 0, i64 %r279, ptr nonnull %collection_result_kind)
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i57.i459 = call i64 @chali_host_string_storage(i64 0, i64 %r283)
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i.i460 = call i64 @chali_host_string_storage(i64 0, i64 8)
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 199, ptr @chali_source_line, align 8
  %r10.i461 = call i64 @chali_host_string_storage(i64 1, i64 %r283)
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 199, ptr @chali_source_line, align 8
  %r13.i462 = call i64 @chali_host_string_storage(i64 1, i64 8)
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 200, ptr @chali_source_line, align 8
  %r18.i465758 = icmp sgt i64 %r2.i57.i459, 0
  br i1 %r18.i465758, label %ast_block_1.i473.preheader, label %ast_label_v513___chali_brk_24.i466

ast_block_1.i473.preheader:                       ; preds = %f_v172_std__lang____bootListClear.exit
  %smax = call i64 @llvm.smax.i64(i64 %r2.i.i460, i64 0)
  br label %ast_block_1.i473

ast_block_1.i473:                                 ; preds = %ast_block_1.i473.preheader, %ast_block_12.i529
  %s_v507___ast_64_i_736.0.i464759 = phi i64 [ %r64.i530, %ast_block_12.i529 ], [ 0, %ast_block_1.i473.preheader ]
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 200, ptr @chali_source_line, align 8
  %exitcond765.not = icmp eq i64 %s_v507___ast_64_i_736.0.i464759, %smax
  br i1 %exitcond765.not, label %ast_label_v513___chali_brk_24.i466, label %ast_block_4.i475

ast_block_4.i475:                                 ; preds = %ast_block_1.i473
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i29.i476 = add i64 %s_v507___ast_64_i_736.0.i464759, %r10.i461
  %6 = icmp eq i64 %r4.i29.i476, 0
  br i1 %6, label %7, label %chali_require_reference.exit718

7:                                                ; preds = %ast_block_4.i475
  call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit718:                  ; preds = %ast_block_4.i475
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i32.i479 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i33.i480 = icmp eq ptr %revision_pointer.i.i32.i479, null
  br i1 %has_revision.not.i.i33.i480, label %slow.i.i48.i495, label %check.i.i34.i481

check.i.i34.i481:                                 ; preds = %chali_require_reference.exit718
  %revision.i.i35.i482 = load atomic i64, ptr %revision_pointer.i.i32.i479 acquire, align 8
  %stamp.i.i36.i483 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i37.i484 = icmp eq i64 %revision.i.i35.i482, %stamp.i.i36.i483
  %enabled.i.i38.i485 = icmp ne i64 %revision.i.i35.i482, 0
  %live.i.i39.i486 = and i1 %enabled.i.i38.i485, %same_revision.i.i37.i484
  %begin.i.i40.i487 = load i64, ptr @chali_address_view, align 16
  %size.i.i41.i488 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i42.i489 = sub i64 %r4.i29.i476, %begin.i.i40.i487
  %inside.i.i43.i490 = icmp ult i64 %offset.i.i42.i489, %size.i.i41.i488
  %valid.i.i47.i494 = and i1 %live.i.i39.i486, %inside.i.i43.i490
  br i1 %valid.i.i47.i494, label %fast.i.i53.i536, label %slow.i.i48.i495

fast.i.i53.i536:                                  ; preds = %check.i.i34.i481
  %base.i.i54.i537 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i55.i538 = getelementptr i8, ptr %base.i.i54.i537, i64 %offset.i.i42.i489
  br label %f_v49_std__lang____bootStringByte.exit56.i497

slow.i.i48.i495:                                  ; preds = %check.i.i34.i481, %chali_require_reference.exit718
  %resolved.i.i49.i496 = call ptr @chali_host_address_view(i64 %r4.i29.i476, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit56.i497

f_v49_std__lang____bootStringByte.exit56.i497:    ; preds = %slow.i.i48.i495, %fast.i.i53.i536
  %common.ret.op.i.i50.i498 = phi ptr [ %physical.i.i55.i538, %fast.i.i53.i536 ], [ %resolved.i.i49.i496, %slow.i.i48.i495 ]
  %r10.i51.i499 = load i8, ptr %common.ret.op.i.i50.i498, align 1
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i.i501 = add i64 %s_v507___ast_64_i_736.0.i464759, %r13.i462
  %8 = icmp eq i64 %r4.i.i501, 0
  br i1 %8, label %9, label %chali_require_reference.exit720

9:                                                ; preds = %f_v49_std__lang____bootStringByte.exit56.i497
  call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit720:                  ; preds = %f_v49_std__lang____bootStringByte.exit56.i497
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i.i504 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i.i505 = icmp eq ptr %revision_pointer.i.i.i504, null
  br i1 %has_revision.not.i.i.i505, label %slow.i.i.i520, label %check.i.i.i506

check.i.i.i506:                                   ; preds = %chali_require_reference.exit720
  %revision.i.i.i507 = load atomic i64, ptr %revision_pointer.i.i.i504 acquire, align 8
  %stamp.i.i.i508 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i.i509 = icmp eq i64 %revision.i.i.i507, %stamp.i.i.i508
  %enabled.i.i.i510 = icmp ne i64 %revision.i.i.i507, 0
  %live.i.i.i511 = and i1 %enabled.i.i.i510, %same_revision.i.i.i509
  %begin.i.i.i512 = load i64, ptr @chali_address_view, align 16
  %size.i.i.i513 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i.i514 = sub i64 %r4.i.i501, %begin.i.i.i512
  %inside.i.i.i515 = icmp ult i64 %offset.i.i.i514, %size.i.i.i513
  %valid.i.i.i519 = and i1 %live.i.i.i511, %inside.i.i.i515
  br i1 %valid.i.i.i519, label %fast.i.i.i533, label %slow.i.i.i520

fast.i.i.i533:                                    ; preds = %check.i.i.i506
  %base.i.i.i534 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i.i535 = getelementptr i8, ptr %base.i.i.i534, i64 %offset.i.i.i514
  br label %f_v49_std__lang____bootStringByte.exit.i522

slow.i.i.i520:                                    ; preds = %check.i.i.i506, %chali_require_reference.exit720
  %resolved.i.i.i521 = call ptr @chali_host_address_view(i64 %r4.i.i501, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit.i522

f_v49_std__lang____bootStringByte.exit.i522:      ; preds = %slow.i.i.i520, %fast.i.i.i533
  %common.ret.op.i.i.i523 = phi ptr [ %physical.i.i.i535, %fast.i.i.i533 ], [ %resolved.i.i.i521, %slow.i.i.i520 ]
  %r10.i.i524 = load i8, ptr %common.ret.op.i.i.i523, align 1
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 202, ptr @chali_source_line, align 8
  %r47.i526 = icmp ult i8 %r10.i51.i499, %r10.i.i524
  br i1 %r47.i526, label %ast_block_0.i.sink.split.sink.split, label %ast_block_9.i527

ast_block_9.i527:                                 ; preds = %f_v49_std__lang____bootStringByte.exit.i522
  store ptr @source_file0, ptr @chali_source_file, align 8
  %r57.i528 = icmp ugt i8 %r10.i51.i499, %r10.i.i524
  br i1 %r57.i528, label %ast_block_0.i.sink.split, label %ast_block_12.i529

ast_block_12.i529:                                ; preds = %ast_block_9.i527
  %r64.i530 = add nuw nsw i64 %s_v507___ast_64_i_736.0.i464759, 1
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 200, ptr @chali_source_line, align 8
  %exitcond766.not = icmp eq i64 %r64.i530, %r2.i57.i459
  br i1 %exitcond766.not, label %ast_label_v513___chali_brk_24.i466, label %ast_block_1.i473

ast_label_v513___chali_brk_24.i466:               ; preds = %ast_block_12.i529, %ast_block_1.i473, %f_v172_std__lang____bootListClear.exit
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 204, ptr @chali_source_line, align 8
  %r69.i467 = icmp slt i64 %r2.i57.i459, %r2.i.i460
  br i1 %r69.i467, label %ast_block_0.i.sink.split.sink.split, label %ast_block_15.i468

ast_block_15.i468:                                ; preds = %ast_label_v513___chali_brk_24.i466
  store ptr @source_file0, ptr @chali_source_file, align 8
  %r79.i469 = icmp sgt i64 %r2.i57.i459, %r2.i.i460
  store i8 0, ptr @chali_return_float, align 1
  br i1 %r79.i469, label %ast_block_0.i.sink.split, label %ast_block_27

ast_block_27:                                     ; preds = %ast_block_15.i468
  store ptr @f_main, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 36, ptr @chali_source_line, align 8
  %r307 = call i64 @chali_host_list_storage_scalar(i64 11, i64 %r258, i64 1, i64 1, i64 0, i64 0, i64 %r279, ptr nonnull %collection_result_kind)
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i57.i378 = call i64 @chali_host_string_storage(i64 0, i64 %r307)
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i.i379 = call i64 @chali_host_string_storage(i64 0, i64 16)
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 199, ptr @chali_source_line, align 8
  %r10.i380 = call i64 @chali_host_string_storage(i64 1, i64 %r307)
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 199, ptr @chali_source_line, align 8
  %r13.i381 = call i64 @chali_host_string_storage(i64 1, i64 16)
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  %r18.i384760 = icmp sgt i64 %r2.i57.i378, 0
  br i1 %r18.i384760, label %ast_block_1.i392.preheader, label %ast_label_v513___chali_brk_24.i385

ast_block_1.i392.preheader:                       ; preds = %ast_block_27
  %smax767 = call i64 @llvm.smax.i64(i64 %r2.i.i379, i64 0)
  br label %ast_block_1.i392

ast_block_1.i392:                                 ; preds = %ast_block_1.i392.preheader, %ast_block_12.i448
  %s_v507___ast_64_i_736.0.i383761 = phi i64 [ %r64.i449, %ast_block_12.i448 ], [ 0, %ast_block_1.i392.preheader ]
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  %exitcond768.not = icmp eq i64 %s_v507___ast_64_i_736.0.i383761, %smax767
  br i1 %exitcond768.not, label %ast_label_v513___chali_brk_24.i385, label %ast_block_4.i394

ast_block_4.i394:                                 ; preds = %ast_block_1.i392
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i29.i395 = add i64 %s_v507___ast_64_i_736.0.i383761, %r10.i380
  %10 = icmp eq i64 %r4.i29.i395, 0
  br i1 %10, label %11, label %chali_require_reference.exit722

11:                                               ; preds = %ast_block_4.i394
  call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit722:                  ; preds = %ast_block_4.i394
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i32.i398 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i33.i399 = icmp eq ptr %revision_pointer.i.i32.i398, null
  br i1 %has_revision.not.i.i33.i399, label %slow.i.i48.i414, label %check.i.i34.i400

check.i.i34.i400:                                 ; preds = %chali_require_reference.exit722
  %revision.i.i35.i401 = load atomic i64, ptr %revision_pointer.i.i32.i398 acquire, align 8
  %stamp.i.i36.i402 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i37.i403 = icmp eq i64 %revision.i.i35.i401, %stamp.i.i36.i402
  %enabled.i.i38.i404 = icmp ne i64 %revision.i.i35.i401, 0
  %live.i.i39.i405 = and i1 %enabled.i.i38.i404, %same_revision.i.i37.i403
  %begin.i.i40.i406 = load i64, ptr @chali_address_view, align 16
  %size.i.i41.i407 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i42.i408 = sub i64 %r4.i29.i395, %begin.i.i40.i406
  %inside.i.i43.i409 = icmp ult i64 %offset.i.i42.i408, %size.i.i41.i407
  %valid.i.i47.i413 = and i1 %live.i.i39.i405, %inside.i.i43.i409
  br i1 %valid.i.i47.i413, label %fast.i.i53.i455, label %slow.i.i48.i414

fast.i.i53.i455:                                  ; preds = %check.i.i34.i400
  %base.i.i54.i456 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i55.i457 = getelementptr i8, ptr %base.i.i54.i456, i64 %offset.i.i42.i408
  br label %f_v49_std__lang____bootStringByte.exit56.i416

slow.i.i48.i414:                                  ; preds = %check.i.i34.i400, %chali_require_reference.exit722
  %resolved.i.i49.i415 = call ptr @chali_host_address_view(i64 %r4.i29.i395, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit56.i416

f_v49_std__lang____bootStringByte.exit56.i416:    ; preds = %slow.i.i48.i414, %fast.i.i53.i455
  %common.ret.op.i.i50.i417 = phi ptr [ %physical.i.i55.i457, %fast.i.i53.i455 ], [ %resolved.i.i49.i415, %slow.i.i48.i414 ]
  %r10.i51.i418 = load i8, ptr %common.ret.op.i.i50.i417, align 1
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i.i420 = add i64 %s_v507___ast_64_i_736.0.i383761, %r13.i381
  %12 = icmp eq i64 %r4.i.i420, 0
  br i1 %12, label %13, label %chali_require_reference.exit724

13:                                               ; preds = %f_v49_std__lang____bootStringByte.exit56.i416
  call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit724:                  ; preds = %f_v49_std__lang____bootStringByte.exit56.i416
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i.i423 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i.i424 = icmp eq ptr %revision_pointer.i.i.i423, null
  br i1 %has_revision.not.i.i.i424, label %slow.i.i.i439, label %check.i.i.i425

check.i.i.i425:                                   ; preds = %chali_require_reference.exit724
  %revision.i.i.i426 = load atomic i64, ptr %revision_pointer.i.i.i423 acquire, align 8
  %stamp.i.i.i427 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i.i428 = icmp eq i64 %revision.i.i.i426, %stamp.i.i.i427
  %enabled.i.i.i429 = icmp ne i64 %revision.i.i.i426, 0
  %live.i.i.i430 = and i1 %enabled.i.i.i429, %same_revision.i.i.i428
  %begin.i.i.i431 = load i64, ptr @chali_address_view, align 16
  %size.i.i.i432 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i.i433 = sub i64 %r4.i.i420, %begin.i.i.i431
  %inside.i.i.i434 = icmp ult i64 %offset.i.i.i433, %size.i.i.i432
  %valid.i.i.i438 = and i1 %live.i.i.i430, %inside.i.i.i434
  br i1 %valid.i.i.i438, label %fast.i.i.i452, label %slow.i.i.i439

fast.i.i.i452:                                    ; preds = %check.i.i.i425
  %base.i.i.i453 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i.i454 = getelementptr i8, ptr %base.i.i.i453, i64 %offset.i.i.i433
  br label %f_v49_std__lang____bootStringByte.exit.i441

slow.i.i.i439:                                    ; preds = %check.i.i.i425, %chali_require_reference.exit724
  %resolved.i.i.i440 = call ptr @chali_host_address_view(i64 %r4.i.i420, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit.i441

f_v49_std__lang____bootStringByte.exit.i441:      ; preds = %slow.i.i.i439, %fast.i.i.i452
  %common.ret.op.i.i.i442 = phi ptr [ %physical.i.i.i454, %fast.i.i.i452 ], [ %resolved.i.i.i440, %slow.i.i.i439 ]
  %r10.i.i443 = load i8, ptr %common.ret.op.i.i.i442, align 1
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 202, ptr @chali_source_line, align 8
  %r47.i445 = icmp ult i8 %r10.i51.i418, %r10.i.i443
  store ptr @source_file0, ptr @chali_source_file, align 8
  br i1 %r47.i445, label %ast_block_0.i.sink.split.sink.split, label %ast_block_9.i446

ast_block_9.i446:                                 ; preds = %f_v49_std__lang____bootStringByte.exit.i441
  %r57.i447 = icmp ugt i8 %r10.i51.i418, %r10.i.i443
  br i1 %r57.i447, label %ast_block_0.i.sink.split, label %ast_block_12.i448

ast_block_12.i448:                                ; preds = %ast_block_9.i446
  %r64.i449 = add nuw nsw i64 %s_v507___ast_64_i_736.0.i383761, 1
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 200, ptr @chali_source_line, align 8
  %exitcond769.not = icmp eq i64 %r64.i449, %r2.i57.i378
  br i1 %exitcond769.not, label %ast_label_v513___chali_brk_24.i385, label %ast_block_1.i392

ast_label_v513___chali_brk_24.i385:               ; preds = %ast_block_12.i448, %ast_block_1.i392, %ast_block_27
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  %r69.i386 = icmp slt i64 %r2.i57.i378, %r2.i.i379
  br i1 %r69.i386, label %ast_block_13.i391, label %ast_block_15.i387

ast_block_13.i391:                                ; preds = %ast_label_v513___chali_brk_24.i385
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 204, ptr @chali_source_line, align 8
  br label %ast_block_0.i.sink.split.sink.split

ast_block_15.i387:                                ; preds = %ast_label_v513___chali_brk_24.i385
  %r79.i388 = icmp sgt i64 %r2.i57.i378, %r2.i.i379
  store i8 0, ptr @chali_return_float, align 1
  br i1 %r79.i388, label %ast_block_0.i.sink.split, label %ast_block_34

ast_block_34:                                     ; preds = %ast_block_15.i387
  store ptr @f_main, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 36, ptr @chali_source_line, align 8
  %r330 = zext i8 %r247 to i64
  %r334 = call i64 @chali_host_list_storage_scalar(i64 11, i64 %r246, i64 0, i64 1, i64 0, i64 0, i64 %r330, ptr nonnull %collection_result_kind)
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i57.i = call i64 @chali_host_string_storage(i64 0, i64 %r334)
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i.i = call i64 @chali_host_string_storage(i64 0, i64 24)
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 199, ptr @chali_source_line, align 8
  %r10.i = call i64 @chali_host_string_storage(i64 1, i64 %r334)
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 199, ptr @chali_source_line, align 8
  %r13.i = call i64 @chali_host_string_storage(i64 1, i64 24)
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 200, ptr @chali_source_line, align 8
  %r18.i762 = icmp sgt i64 %r2.i57.i, 0
  br i1 %r18.i762, label %ast_block_1.i.preheader, label %ast_label_v513___chali_brk_24.i

ast_block_1.i.preheader:                          ; preds = %ast_block_34
  %smax770 = call i64 @llvm.smax.i64(i64 %r2.i.i, i64 0)
  br label %ast_block_1.i

ast_block_1.i:                                    ; preds = %ast_block_1.i.preheader, %ast_block_12.i
  %s_v507___ast_64_i_736.0.i763 = phi i64 [ %r64.i, %ast_block_12.i ], [ 0, %ast_block_1.i.preheader ]
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 200, ptr @chali_source_line, align 8
  %exitcond771.not = icmp eq i64 %s_v507___ast_64_i_736.0.i763, %smax770
  br i1 %exitcond771.not, label %ast_label_v513___chali_brk_24.i, label %ast_block_4.i

ast_block_4.i:                                    ; preds = %ast_block_1.i
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i29.i = add i64 %s_v507___ast_64_i_736.0.i763, %r10.i
  %14 = icmp eq i64 %r4.i29.i, 0
  br i1 %14, label %15, label %chali_require_reference.exit726

15:                                               ; preds = %ast_block_4.i
  call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit726:                  ; preds = %ast_block_4.i
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i32.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i33.i = icmp eq ptr %revision_pointer.i.i32.i, null
  br i1 %has_revision.not.i.i33.i, label %slow.i.i48.i, label %check.i.i34.i

check.i.i34.i:                                    ; preds = %chali_require_reference.exit726
  %revision.i.i35.i = load atomic i64, ptr %revision_pointer.i.i32.i acquire, align 8
  %stamp.i.i36.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i37.i = icmp eq i64 %revision.i.i35.i, %stamp.i.i36.i
  %enabled.i.i38.i = icmp ne i64 %revision.i.i35.i, 0
  %live.i.i39.i = and i1 %enabled.i.i38.i, %same_revision.i.i37.i
  %begin.i.i40.i = load i64, ptr @chali_address_view, align 16
  %size.i.i41.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i42.i = sub i64 %r4.i29.i, %begin.i.i40.i
  %inside.i.i43.i = icmp ult i64 %offset.i.i42.i, %size.i.i41.i
  %valid.i.i47.i = and i1 %live.i.i39.i, %inside.i.i43.i
  br i1 %valid.i.i47.i, label %fast.i.i53.i, label %slow.i.i48.i

fast.i.i53.i:                                     ; preds = %check.i.i34.i
  %base.i.i54.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i55.i = getelementptr i8, ptr %base.i.i54.i, i64 %offset.i.i42.i
  br label %f_v49_std__lang____bootStringByte.exit56.i

slow.i.i48.i:                                     ; preds = %check.i.i34.i, %chali_require_reference.exit726
  %resolved.i.i49.i = call ptr @chali_host_address_view(i64 %r4.i29.i, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit56.i

f_v49_std__lang____bootStringByte.exit56.i:       ; preds = %slow.i.i48.i, %fast.i.i53.i
  %common.ret.op.i.i50.i = phi ptr [ %physical.i.i55.i, %fast.i.i53.i ], [ %resolved.i.i49.i, %slow.i.i48.i ]
  %r10.i51.i = load i8, ptr %common.ret.op.i.i50.i, align 1
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i.i = add i64 %s_v507___ast_64_i_736.0.i763, %r13.i
  %16 = icmp eq i64 %r4.i.i, 0
  br i1 %16, label %17, label %chali_require_reference.exit728

17:                                               ; preds = %f_v49_std__lang____bootStringByte.exit56.i
  call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit728:                  ; preds = %f_v49_std__lang____bootStringByte.exit56.i
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i.i = icmp eq ptr %revision_pointer.i.i.i, null
  br i1 %has_revision.not.i.i.i, label %slow.i.i.i, label %check.i.i.i

check.i.i.i:                                      ; preds = %chali_require_reference.exit728
  %revision.i.i.i = load atomic i64, ptr %revision_pointer.i.i.i acquire, align 8
  %stamp.i.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i.i = icmp eq i64 %revision.i.i.i, %stamp.i.i.i
  %enabled.i.i.i = icmp ne i64 %revision.i.i.i, 0
  %live.i.i.i = and i1 %enabled.i.i.i, %same_revision.i.i.i
  %begin.i.i.i = load i64, ptr @chali_address_view, align 16
  %size.i.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i.i = sub i64 %r4.i.i, %begin.i.i.i
  %inside.i.i.i = icmp ult i64 %offset.i.i.i, %size.i.i.i
  %valid.i.i.i = and i1 %live.i.i.i, %inside.i.i.i
  br i1 %valid.i.i.i, label %fast.i.i.i, label %slow.i.i.i

fast.i.i.i:                                       ; preds = %check.i.i.i
  %base.i.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i.i = getelementptr i8, ptr %base.i.i.i, i64 %offset.i.i.i
  br label %f_v49_std__lang____bootStringByte.exit.i

slow.i.i.i:                                       ; preds = %check.i.i.i, %chali_require_reference.exit728
  %resolved.i.i.i = call ptr @chali_host_address_view(i64 %r4.i.i, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit.i

f_v49_std__lang____bootStringByte.exit.i:         ; preds = %slow.i.i.i, %fast.i.i.i
  %common.ret.op.i.i.i = phi ptr [ %physical.i.i.i, %fast.i.i.i ], [ %resolved.i.i.i, %slow.i.i.i ]
  %r10.i.i = load i8, ptr %common.ret.op.i.i.i, align 1
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 202, ptr @chali_source_line, align 8
  %r47.i = icmp ult i8 %r10.i51.i, %r10.i.i
  br i1 %r47.i, label %ast_block_0.i.sink.split.sink.split, label %ast_block_9.i

ast_block_9.i:                                    ; preds = %f_v49_std__lang____bootStringByte.exit.i
  store ptr @source_file0, ptr @chali_source_file, align 8
  %r57.i = icmp ugt i8 %r10.i51.i, %r10.i.i
  br i1 %r57.i, label %ast_block_0.i.sink.split, label %ast_block_12.i

ast_block_12.i:                                   ; preds = %ast_block_9.i
  %r64.i = add nuw nsw i64 %s_v507___ast_64_i_736.0.i763, 1
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 200, ptr @chali_source_line, align 8
  %exitcond772.not = icmp eq i64 %r64.i, %r2.i57.i
  br i1 %exitcond772.not, label %ast_label_v513___chali_brk_24.i, label %ast_block_1.i

ast_label_v513___chali_brk_24.i:                  ; preds = %ast_block_12.i, %ast_block_1.i, %ast_block_34
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 204, ptr @chali_source_line, align 8
  %r69.i = icmp slt i64 %r2.i57.i, %r2.i.i
  br i1 %r69.i, label %ast_block_0.i.sink.split.sink.split, label %ast_block_15.i

ast_block_15.i:                                   ; preds = %ast_label_v513___chali_brk_24.i
  %r79.i.not = icmp sgt i64 %r2.i57.i, %r2.i.i
  store i8 0, ptr @chali_return_float, align 1
  store ptr @f_main, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 36, ptr @chali_source_line, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i)
  br i1 %r79.i.not, label %ast_block_0.i, label %f_v162_std__lang__assert_bool.exit

ast_block_0.i.sink.split.sink.split:              ; preds = %f_v49_std__lang____bootStringByte.exit.i522, %f_v49_std__lang____bootStringByte.exit.i441, %f_v49_std__lang____bootStringByte.exit.i, %ast_label_v513___chali_brk_24.i, %ast_block_13.i391, %ast_label_v513___chali_brk_24.i466
  %s_v568___ast_236___chali_tmp_s_1311.0749.ph.ph = phi i64 [ 0, %ast_block_13.i391 ], [ 0, %f_v49_std__lang____bootStringByte.exit.i441 ], [ 0, %ast_label_v513___chali_brk_24.i466 ], [ %r334, %f_v49_std__lang____bootStringByte.exit.i ], [ %r334, %ast_label_v513___chali_brk_24.i ], [ 0, %f_v49_std__lang____bootStringByte.exit.i522 ]
  %s_v567___ast_236___chali_tmp_s_1310.0737747.ph.ph = phi i64 [ %r307, %ast_block_13.i391 ], [ %r307, %f_v49_std__lang____bootStringByte.exit.i441 ], [ 0, %ast_label_v513___chali_brk_24.i466 ], [ %r307, %f_v49_std__lang____bootStringByte.exit.i ], [ %r307, %ast_label_v513___chali_brk_24.i ], [ 0, %f_v49_std__lang____bootStringByte.exit.i522 ]
  store i8 0, ptr @chali_return_float, align 1
  br label %ast_block_0.i.sink.split

ast_block_0.i.sink.split:                         ; preds = %ast_block_9.i527, %ast_block_9.i446, %ast_block_9.i, %ast_block_0.i.sink.split.sink.split, %ast_block_15.i468, %ast_block_15.i387
  %s_v568___ast_236___chali_tmp_s_1311.0749.ph = phi i64 [ 0, %ast_block_15.i468 ], [ %s_v568___ast_236___chali_tmp_s_1311.0749.ph.ph, %ast_block_0.i.sink.split.sink.split ], [ 0, %ast_block_9.i446 ], [ %r334, %ast_block_9.i ], [ 0, %ast_block_15.i387 ], [ 0, %ast_block_9.i527 ]
  %s_v567___ast_236___chali_tmp_s_1310.0737747.ph = phi i64 [ 0, %ast_block_15.i468 ], [ %s_v567___ast_236___chali_tmp_s_1310.0737747.ph.ph, %ast_block_0.i.sink.split.sink.split ], [ %r307, %ast_block_9.i446 ], [ %r307, %ast_block_9.i ], [ %r307, %ast_block_15.i387 ], [ 0, %ast_block_9.i527 ]
  store ptr @f_main, ptr @chali_source_function, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i)
  br label %ast_block_0.i

ast_block_0.i:                                    ; preds = %ast_block_0.i.sink.split, %ast_block_15.i
  %s_v568___ast_236___chali_tmp_s_1311.0749 = phi i64 [ %r334, %ast_block_15.i ], [ %s_v568___ast_236___chali_tmp_s_1311.0749.ph, %ast_block_0.i.sink.split ]
  %s_v567___ast_236___chali_tmp_s_1310.0737747 = phi i64 [ %r307, %ast_block_15.i ], [ %s_v567___ast_236___chali_tmp_s_1310.0737747.ph, %ast_block_0.i.sink.split ]
  store ptr @f_v162_std__lang__assert_bool, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 782, ptr @chali_source_line, align 8
  store i64 176, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  %r8.i = call i64 @chali_host_call(ptr nonnull @native_name0, i64 1, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r9.i = call i8 @chali_host_result_float()
  br label %f_v162_std__lang__assert_bool.exit

f_v162_std__lang__assert_bool.exit:               ; preds = %ast_block_15.i, %ast_block_0.i
  %s_v568___ast_236___chali_tmp_s_1311.0748 = phi i64 [ %r334, %ast_block_15.i ], [ %s_v568___ast_236___chali_tmp_s_1311.0749, %ast_block_0.i ]
  %s_v567___ast_236___chali_tmp_s_1310.0737746 = phi i64 [ %r307, %ast_block_15.i ], [ %s_v567___ast_236___chali_tmp_s_1310.0737747, %ast_block_0.i ]
  store i8 0, ptr @chali_return_float, align 1
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i)
  %r349.not = icmp eq i64 %s_v568___ast_236___chali_tmp_s_1311.0748, 0
  br i1 %r349.not, label %ast_block_38, label %ast_block_36

ast_block_36:                                     ; preds = %f_v162_std__lang__assert_bool.exit
  call void @chali_host_drop(i64 noundef %s_v568___ast_236___chali_tmp_s_1311.0748) #26
  store i8 0, ptr @chali_return_float, align 1
  br label %ast_block_38

ast_block_38:                                     ; preds = %f_v162_std__lang__assert_bool.exit, %ast_block_36
  %r358.not = icmp eq i64 %s_v567___ast_236___chali_tmp_s_1310.0737746, 0
  br i1 %r358.not, label %ast_block_41, label %ast_block_39

ast_block_39:                                     ; preds = %ast_block_38
  call void @chali_host_drop(i64 noundef %s_v567___ast_236___chali_tmp_s_1310.0737746) #26
  store i8 0, ptr @chali_return_float, align 1
  br label %ast_block_41

ast_block_41:                                     ; preds = %ast_block_38, %ast_block_39
  %r367.not = icmp eq i64 %r283, 0
  br i1 %r367.not, label %ast_block_44, label %ast_block_42

ast_block_42:                                     ; preds = %ast_block_41
  call void @chali_host_drop(i64 noundef %r283) #26
  store i8 0, ptr @chali_return_float, align 1
  br label %ast_block_44

ast_block_44:                                     ; preds = %ast_block_41, %ast_block_42
  store ptr @f_main, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 37, ptr @chali_source_line, align 8
  %18 = call ptr @__acrt_iob_func(i32 noundef 1) #26
  %r376 = call i64 @fwrite(ptr nonnull @chali_abi_lit0, i64 1, i64 52, ptr %18)
  %r379.not = icmp eq i64 %r258, 0
  br i1 %r379.not, label %ast_block_47, label %ast_block_45

ast_block_45:                                     ; preds = %ast_block_44
  call void @chali_host_drop(i64 noundef %r258) #26
  br label %ast_block_47

ast_block_47:                                     ; preds = %ast_block_44, %ast_block_45
  %r386.not = icmp eq i64 %r246, 0
  br i1 %r386.not, label %ast_block_50, label %ast_block_48

ast_block_48:                                     ; preds = %ast_block_47
  call void @chali_host_drop(i64 noundef %r246) #26
  br label %ast_block_50

ast_block_50:                                     ; preds = %ast_block_47, %ast_block_48
  %r393.not = icmp eq i64 %r142, 0
  br i1 %r393.not, label %ast_block_53, label %ast_block_51

ast_block_51:                                     ; preds = %ast_block_50
  call void @chali_host_drop(i64 noundef %r142) #26
  br label %ast_block_53

ast_block_53:                                     ; preds = %ast_block_50, %ast_block_51
  %revision_pointer.i124 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i125 = icmp eq ptr %revision_pointer.i124, null
  br i1 %has_revision.not.i125, label %slow.i140, label %check.i126

check.i126:                                       ; preds = %ast_block_53
  %revision.i127 = load atomic i64, ptr %revision_pointer.i124 acquire, align 8
  %stamp.i128 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i129 = icmp eq i64 %revision.i127, %stamp.i128
  %enabled.i130 = icmp ne i64 %revision.i127, 0
  %live.i131 = and i1 %enabled.i130, %same_revision.i129
  %begin.i132 = load i64, ptr @chali_address_view, align 16
  %size.i133 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i134 = sub i64 %3, %begin.i132
  %inside.i135 = icmp ult i64 %offset.i134, %size.i133
  %remaining.i136 = sub i64 %size.i133, %offset.i134
  %fits.i137 = icmp ugt i64 %remaining.i136, 7
  %range.i138 = and i1 %inside.i135, %fits.i137
  %valid.i139 = and i1 %live.i131, %range.i138
  br i1 %valid.i139, label %fast.i143, label %slow.i140

fast.i143:                                        ; preds = %check.i126
  %base.i144 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i145 = getelementptr i8, ptr %base.i144, i64 %offset.i134
  br label %chali_primitive_address.exit146

slow.i140:                                        ; preds = %check.i126, %ast_block_53
  %resolved.i141 = call ptr @chali_host_address_view(i64 %3, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit146

chali_primitive_address.exit146:                  ; preds = %fast.i143, %slow.i140
  %common.ret.op.i142 = phi ptr [ %physical.i145, %fast.i143 ], [ %resolved.i141, %slow.i140 ]
  %r401 = load i64, ptr %common.ret.op.i142, align 1
  %r402.not = icmp eq i64 %r401, 0
  br i1 %r402.not, label %ast_block_56, label %ast_block_54

ast_block_54:                                     ; preds = %chali_primitive_address.exit146
  %revision_pointer.i101 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i102 = icmp eq ptr %revision_pointer.i101, null
  br i1 %has_revision.not.i102, label %slow.i117, label %check.i103

check.i103:                                       ; preds = %ast_block_54
  %revision.i104 = load atomic i64, ptr %revision_pointer.i101 acquire, align 8
  %stamp.i105 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i106 = icmp eq i64 %revision.i104, %stamp.i105
  %enabled.i107 = icmp ne i64 %revision.i104, 0
  %live.i108 = and i1 %enabled.i107, %same_revision.i106
  %begin.i109 = load i64, ptr @chali_address_view, align 16
  %size.i110 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i111 = sub i64 %3, %begin.i109
  %inside.i112 = icmp ult i64 %offset.i111, %size.i110
  %remaining.i113 = sub i64 %size.i110, %offset.i111
  %fits.i114 = icmp ugt i64 %remaining.i113, 7
  %range.i115 = and i1 %inside.i112, %fits.i114
  %valid.i116 = and i1 %live.i108, %range.i115
  br i1 %valid.i116, label %fast.i120, label %slow.i117

fast.i120:                                        ; preds = %check.i103
  %base.i121 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i122 = getelementptr i8, ptr %base.i121, i64 %offset.i111
  br label %chali_primitive_address.exit123

slow.i117:                                        ; preds = %check.i103, %ast_block_54
  %resolved.i118 = call ptr @chali_host_address_view(i64 %3, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit123

chali_primitive_address.exit123:                  ; preds = %fast.i120, %slow.i117
  %common.ret.op.i119 = phi ptr [ %physical.i122, %fast.i120 ], [ %resolved.i118, %slow.i117 ]
  %r408 = load i64, ptr %common.ret.op.i119, align 1
  call void @chali_host_drop(i64 noundef %r408) #26
  br label %ast_block_56

ast_block_56:                                     ; preds = %chali_primitive_address.exit146, %chali_primitive_address.exit123
  %revision_pointer.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i = icmp eq ptr %revision_pointer.i, null
  br i1 %has_revision.not.i, label %slow.i, label %check.i

check.i:                                          ; preds = %ast_block_56
  %revision.i = load atomic i64, ptr %revision_pointer.i acquire, align 8
  %stamp.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i = icmp eq i64 %revision.i, %stamp.i
  %enabled.i = icmp ne i64 %revision.i, 0
  %live.i = and i1 %enabled.i, %same_revision.i
  %begin.i = load i64, ptr @chali_address_view, align 16
  %size.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i = sub i64 %3, %begin.i
  %inside.i = icmp ult i64 %offset.i, %size.i
  %remaining.i = sub i64 %size.i, %offset.i
  %fits.i = icmp ugt i64 %remaining.i, 7
  %range.i = and i1 %inside.i, %fits.i
  %valid.i = and i1 %live.i, %range.i
  br i1 %valid.i, label %fast.i, label %slow.i

fast.i:                                           ; preds = %check.i
  %base.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i = getelementptr i8, ptr %base.i, i64 %offset.i
  br label %ast_block_62

slow.i:                                           ; preds = %check.i, %ast_block_56
  %resolved.i = call ptr @chali_host_address_view(i64 %3, i64 8, ptr nonnull @chali_address_view)
  br label %ast_block_62

ast_block_62:                                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  store i64 0, ptr %common.ret.op.i, align 1
  call void @chali_host_drop(i64 noundef %0) #26
  call void @chali_host_drop(i64 noundef %3) #26
  store i8 0, ptr @chali_return_float, align 1
  ret i64 0
}

; Function Attrs: alwaysinline uwtable
define internal noundef i64 @f_v271_std__lang__pointerFromAddress__int64(i64 %p0) #22 section "chali_code" {
entry:
  %indirect_arguments = alloca [4 x i64], align 8
  %native_argument_types = alloca [4 x i8], align 1
  %raw_memory_output = alloca ptr, align 8
  %raw_memory_input = alloca ptr, align 8
  %0 = tail call i64 @chali_host_allocate(i64 noundef 8) #26
  call void @chali_host_memory_prepare(i64 8, i64 %0, i64 0, i64 8, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r3 = load ptr, ptr %raw_memory_output, align 8
  store i64 0, ptr %r3, align 1
  store i64 %0, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r9 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  store i64 8, ptr %r9, align 8
  %r10 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r10, align 1
  %r11 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r12 = call i8 @chali_host_result_float()
  %revision_pointer.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i = icmp eq ptr %revision_pointer.i, null
  br i1 %has_revision.not.i, label %slow.i, label %check.i

check.i:                                          ; preds = %entry
  %revision.i = load atomic i64, ptr %revision_pointer.i acquire, align 8
  %stamp.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i = icmp eq i64 %revision.i, %stamp.i
  %enabled.i = icmp ne i64 %revision.i, 0
  %live.i = and i1 %enabled.i, %same_revision.i
  %begin.i = load i64, ptr @chali_address_view, align 16
  %size.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i = sub i64 %0, %begin.i
  %inside.i = icmp ult i64 %offset.i, %size.i
  %remaining.i = sub i64 %size.i, %offset.i
  %fits.i = icmp ugt i64 %remaining.i, 7
  %range.i = and i1 %inside.i, %fits.i
  %valid.i = and i1 %live.i, %range.i
  br i1 %valid.i, label %fast.i, label %slow.i

fast.i:                                           ; preds = %check.i
  %base.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i = getelementptr i8, ptr %base.i, i64 %offset.i
  br label %chali_primitive_address.exit

slow.i:                                           ; preds = %check.i, %entry
  %resolved.i = call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit

chali_primitive_address.exit:                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  store i64 %p0, ptr %common.ret.op.i, align 1
  store i64 200, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r9, align 8
  store i8 0, ptr %r10, align 1
  %r26 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r27 = call i8 @chali_host_result_float()
  %r32 = call i64 @chali_host_copy_words(i64 200, i64 %0, i64 1)
  store i64 200, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r9, align 8
  store i8 0, ptr %r10, align 1
  %r39 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r40 = call i8 @chali_host_result_float()
  %r43.not = icmp eq i64 %0, 0
  br i1 %r43.not, label %ast_block_2, label %ast_block_0

ast_block_0:                                      ; preds = %chali_primitive_address.exit
  call void @chali_host_drop(i64 noundef %0) #26
  br label %ast_block_2

ast_block_2:                                      ; preds = %chali_primitive_address.exit, %ast_block_0
  store i8 0, ptr @chali_return_float, align 1
  ret i64 200
}

; Function Attrs: alwaysinline uwtable
define internal noundef i64 @f_v273_std__lang__pointerFromAddress__uint8(i64 %p0) #22 section "chali_code" {
entry:
  %indirect_arguments = alloca [4 x i64], align 8
  %native_argument_types = alloca [4 x i8], align 1
  %raw_memory_output = alloca ptr, align 8
  %raw_memory_input = alloca ptr, align 8
  %0 = tail call i64 @chali_host_allocate(i64 noundef 8) #26
  call void @chali_host_memory_prepare(i64 8, i64 %0, i64 0, i64 8, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r3 = load ptr, ptr %raw_memory_output, align 8
  store i64 0, ptr %r3, align 1
  store i64 %0, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r9 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  store i64 8, ptr %r9, align 8
  %r10 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r10, align 1
  %r11 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r12 = call i8 @chali_host_result_float()
  %revision_pointer.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i = icmp eq ptr %revision_pointer.i, null
  br i1 %has_revision.not.i, label %slow.i, label %check.i

check.i:                                          ; preds = %entry
  %revision.i = load atomic i64, ptr %revision_pointer.i acquire, align 8
  %stamp.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i = icmp eq i64 %revision.i, %stamp.i
  %enabled.i = icmp ne i64 %revision.i, 0
  %live.i = and i1 %enabled.i, %same_revision.i
  %begin.i = load i64, ptr @chali_address_view, align 16
  %size.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i = sub i64 %0, %begin.i
  %inside.i = icmp ult i64 %offset.i, %size.i
  %remaining.i = sub i64 %size.i, %offset.i
  %fits.i = icmp ugt i64 %remaining.i, 7
  %range.i = and i1 %inside.i, %fits.i
  %valid.i = and i1 %live.i, %range.i
  br i1 %valid.i, label %fast.i, label %slow.i

fast.i:                                           ; preds = %check.i
  %base.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i = getelementptr i8, ptr %base.i, i64 %offset.i
  br label %chali_primitive_address.exit

slow.i:                                           ; preds = %check.i, %entry
  %resolved.i = call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit

chali_primitive_address.exit:                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  store i64 %p0, ptr %common.ret.op.i, align 1
  store i64 208, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r9, align 8
  store i8 0, ptr %r10, align 1
  %r26 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r27 = call i8 @chali_host_result_float()
  %r32 = call i64 @chali_host_copy_words(i64 208, i64 %0, i64 1)
  store i64 208, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r9, align 8
  store i8 0, ptr %r10, align 1
  %r39 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r40 = call i8 @chali_host_result_float()
  %r43.not = icmp eq i64 %0, 0
  br i1 %r43.not, label %ast_block_2, label %ast_block_0

ast_block_0:                                      ; preds = %chali_primitive_address.exit
  call void @chali_host_drop(i64 noundef %0) #26
  br label %ast_block_2

ast_block_2:                                      ; preds = %chali_primitive_address.exit, %ast_block_0
  store i8 0, ptr @chali_return_float, align 1
  ret i64 208
}

; Function Attrs: alwaysinline uwtable
define internal noundef i64 @f_v468___chali_copy_struct(i64 %p0, i64 %p1, i64 %p2) #22 section "chali_code" {
entry:
  %r430 = icmp sgt i64 %p2, 0
  br i1 %r430, label %ast_block_1, label %ast_block_2

ast_block_1:                                      ; preds = %entry, %chali_primitive_address.exit
  %s_v590___ast_468___chali_k.031 = phi i64 [ %r26, %chali_primitive_address.exit ], [ 0, %entry ]
  %r11 = shl nsw i64 %s_v590___ast_468___chali_k.031, 3
  %r12 = add i64 %r11, %p1
  %revision_pointer.i7 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i8 = icmp eq ptr %revision_pointer.i7, null
  br i1 %has_revision.not.i8, label %slow.i23, label %check.i9

check.i9:                                         ; preds = %ast_block_1
  %revision.i10 = load atomic i64, ptr %revision_pointer.i7 acquire, align 8
  %stamp.i11 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i12 = icmp eq i64 %revision.i10, %stamp.i11
  %enabled.i13 = icmp ne i64 %revision.i10, 0
  %live.i14 = and i1 %enabled.i13, %same_revision.i12
  %begin.i15 = load i64, ptr @chali_address_view, align 16
  %size.i16 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i17 = sub i64 %r12, %begin.i15
  %inside.i18 = icmp ult i64 %offset.i17, %size.i16
  %remaining.i19 = sub i64 %size.i16, %offset.i17
  %fits.i20 = icmp ugt i64 %remaining.i19, 7
  %range.i21 = and i1 %inside.i18, %fits.i20
  %valid.i22 = and i1 %live.i14, %range.i21
  br i1 %valid.i22, label %fast.i26, label %slow.i23

fast.i26:                                         ; preds = %check.i9
  %base.i27 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i28 = getelementptr i8, ptr %base.i27, i64 %offset.i17
  br label %chali_primitive_address.exit29

slow.i23:                                         ; preds = %check.i9, %ast_block_1
  %resolved.i24 = tail call ptr @chali_host_address_view(i64 %r12, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit29

chali_primitive_address.exit29:                   ; preds = %fast.i26, %slow.i23
  %common.ret.op.i25 = phi ptr [ %physical.i28, %fast.i26 ], [ %resolved.i24, %slow.i23 ]
  %r14 = load i64, ptr %common.ret.op.i25, align 1
  %r20 = add i64 %r11, %p0
  %revision_pointer.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i = icmp eq ptr %revision_pointer.i, null
  br i1 %has_revision.not.i, label %slow.i, label %check.i

check.i:                                          ; preds = %chali_primitive_address.exit29
  %revision.i = load atomic i64, ptr %revision_pointer.i acquire, align 8
  %stamp.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i = icmp eq i64 %revision.i, %stamp.i
  %enabled.i = icmp ne i64 %revision.i, 0
  %live.i = and i1 %enabled.i, %same_revision.i
  %begin.i = load i64, ptr @chali_address_view, align 16
  %size.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i = sub i64 %r20, %begin.i
  %inside.i = icmp ult i64 %offset.i, %size.i
  %remaining.i = sub i64 %size.i, %offset.i
  %fits.i = icmp ugt i64 %remaining.i, 7
  %range.i = and i1 %inside.i, %fits.i
  %valid.i = and i1 %live.i, %range.i
  br i1 %valid.i, label %fast.i, label %slow.i

fast.i:                                           ; preds = %check.i
  %base.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i = getelementptr i8, ptr %base.i, i64 %offset.i
  br label %chali_primitive_address.exit

slow.i:                                           ; preds = %check.i, %chali_primitive_address.exit29
  %resolved.i = tail call ptr @chali_host_address_view(i64 %r20, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit

chali_primitive_address.exit:                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  store i64 %r14, ptr %common.ret.op.i, align 1
  %r25 = shl nsw i64 %s_v590___ast_468___chali_k.031, 32
  %sext = add i64 %r25, 4294967296
  %r26 = ashr exact i64 %sext, 32
  %r4 = icmp slt i64 %r26, %p2
  br i1 %r4, label %ast_block_1, label %ast_block_2

ast_block_2:                                      ; preds = %chali_primitive_address.exit, %entry
  store i8 0, ptr @chali_return_float, align 1
  ret i64 0
}

; Function Attrs: noreturn nounwind
define noundef i64 @chali_invoke(i64 %function, i64 %count, ptr nofree readnone captures(none) %arguments) local_unnamed_addr #25 {
entry:
  %target = tail call i64 @chali_function_target(i64 %function)
  %environment = tail call i64 @chali_function_environment(i64 %function)
  tail call void @chali_abort(i64 0)
  unreachable
}

; Function Attrs: noreturn nounwind
define noundef i64 @chali_function_returns_real(i64 %function) local_unnamed_addr #25 {
entry:
  %target = tail call i64 @chali_function_target(i64 %function)
  tail call void @chali_abort(i64 0)
  unreachable
}

define noundef i32 @main(i32 %argc, ptr %argv) local_unnamed_addr {
entry:
  tail call void @chali_host_init() #26
  tail call void @chali_host_process_arguments(i32 %argc, ptr %argv)
  %result = tail call i64 @f_main()
  %0 = tail call ptr @__acrt_iob_func(i32 noundef 1) #26
  %1 = tail call i32 @fflush(ptr noundef %0)
  tail call void @chali_host_finish() #26
  ret i32 0
}

attributes #0 = { nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { cold noreturn nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nofree nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { nofree noreturn nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #6 = { mustprogress nofree nounwind willreturn allockind("alloc,uninitialized") allocsize(0) memory(inaccessiblemem: readwrite, errnomem: write) "alloc-family"="malloc" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #7 = { mustprogress nounwind willreturn allockind("free") memory(argmem: readwrite, inaccessiblemem: readwrite) "alloc-family"="malloc" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #8 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #9 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: read) "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #10 = { mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none, target_mem: none) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #11 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #12 = { mustprogress nounwind willreturn allockind("realloc") allocsize(1) memory(argmem: readwrite, inaccessiblemem: readwrite, errnomem: write) "alloc-family"="malloc" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #13 = { noreturn nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #14 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: write) }
attributes #15 = { mustprogress nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none) }
attributes #16 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: read) }
attributes #17 = { mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none, target_mem: none) }
attributes #18 = { mustprogress nofree norecurse nosync nounwind willreturn memory(read, argmem: write, inaccessiblemem: none, target_mem: none) }
attributes #19 = { mustprogress nofree norecurse nosync nounwind willreturn memory(read, argmem: none, inaccessiblemem: none, target_mem: none) }
attributes #20 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable }
attributes #21 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #22 = { alwaysinline uwtable }
attributes #23 = { uwtable }
attributes #24 = { alwaysinline nounwind uwtable }
attributes #25 = { noreturn nounwind }
attributes #26 = { nounwind }
attributes #27 = { cold noreturn nounwind }
attributes #28 = { allocsize(0) }
attributes #29 = { allocsize(1) }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5}
!llvm.ident = !{!6}
!llvm.errno.tbaa = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "clang version 23.1.4 (https://github.com/llvm/llvm-project.git 7a941dfb2877a0b8cf30323fefa85ad019e0b0f5)", isOptimized: true, runtimeVersion: 0, emissionKind: NoDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "source/aot/runtime.c", directory: "G:/Desktop/ChaLi")
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = !{i32 8, !"PIC Level", i32 2}
!4 = !{i32 7, !"uwtable", i32 2}
!5 = !{i32 1, !"MaxTLSAlign", i32 65536}
!6 = !{!"clang version 23.1.4 (https://github.com/llvm/llvm-project.git 7a941dfb2877a0b8cf30323fefa85ad019e0b0f5)"}
!7 = !{!8, !9, i64 0}
!8 = !{!"__libc_errno", !9, i64 0}
!9 = !{!"int", !10, i64 0}
!10 = !{!"omnipotent char", !11, i64 0}
!11 = !{!"Simple C/C++ TBAA"}
!12 = !{!10, !10, i64 0}
!13 = !{!14, !14, i64 0}
!14 = !{!"long long", !10, i64 0}
!15 = distinct !{!15, !16, !17}
!16 = !{!"llvm.loop.mustprogress"}
!17 = !{!"llvm.loop.unroll.disable"}
!18 = distinct !{!18, !16, !17}
!19 = distinct !{!19, !16, !17, !20, !21}
!20 = !{!"llvm.loop.isvectorized", i32 1}
!21 = !{!"llvm.loop.unroll.runtime.disable"}
!22 = !{!"branch_weights", i32 4, i32 12}
!23 = distinct !{!23, !16, !17, !20, !21}
!24 = distinct !{!24, !16, !17, !20}
!25 = distinct !{!25, !16, !17}
!26 = distinct !{!26, !16, !17}
!27 = distinct !{!27, !16, !17}
!28 = distinct !{!28, !16, !17}
!29 = distinct !{!29, !16, !17}
!30 = !{!31, !31, i64 0}
!31 = !{!"double", !10, i64 0}
!32 = !{ptr @f_v60_std__lang____bootStringCopy}
!33 = !{ptr @f_v169_std__lang____bootBufferCapacity}
!34 = !{ptr @f_v174_std__lang____bootListPushI}
