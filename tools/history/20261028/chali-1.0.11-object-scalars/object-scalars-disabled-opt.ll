; ModuleID = 'tmp/object-scalars-disabled.ll'
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
@chali_source_function = internal thread_local unnamed_addr global ptr null
@chali_source_file = internal thread_local unnamed_addr global ptr null
@chali_source_line = internal thread_local unnamed_addr global i64 0
@chali_threads_enabled = local_unnamed_addr constant i32 1
@chali_step_limit = local_unnamed_addr constant i64 0
@chali_function_count = local_unnamed_addr constant i64 0
@chali_function_addresses = local_unnamed_addr constant [0 x i64] poison
@chali_data_layout_size = local_unnamed_addr constant i64 66
@chali_data_layout = local_unnamed_addr constant [67 x i8] c"CHLY\01\04\0Aast_data_0\08\03\15\08\0Aast_data_1 \03\10\08\0Aast_data_20\03\0E\08\0Aast_data_3@\03\1C\08\00"
@chali_data_image = local_unnamed_addr constant [104 x i8] c"\00\00\00\00\00\00\00\00NamedOperation.apply\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00Multiply mut&\00\00\00R13:Multiply mut&0:0:8:0:0:\00\00\00\00\00\00\00\00\00\00\00\00\00"
@chali_data_segment_count = local_unnamed_addr constant i64 1
@chali_data_segments = local_unnamed_addr constant [1 x { i64, i64, i64 }] [{ i64, i64, i64 } { i64 0, i64 0, i64 104 }]
@chali_data_size = local_unnamed_addr constant i64 104
@chali_heap_size = local_unnamed_addr constant i64 536870912
@chali_allocator_kind = local_unnamed_addr constant i32 1
@source_file0 = private constant [39 x i8] c"benchmarks/features/interface_mono.chl\00"
@trace_anchor_name = private constant [1 x i8] zeroinitializer
@trace_name0 = private constant [15 x i8] c"Multiply.apply\00"
@trace_name1 = private constant [15 x i8] c"program::setup\00"
@trace_name2 = private constant [16 x i8] c"program::invoke\00"
@trace_name3 = private constant [14 x i8] c"program::work\00"
@trace_name4 = private constant [21 x i8] c"__chali_library_init\00"
@trace_name5 = private constant [23 x i8] c"__chali_library_finish\00"
@trace_name6 = private constant [35 x i8] c"__chali_iface.NamedOperation.apply\00"
@chali_trace_count = local_unnamed_addr constant i64 8
@chali_trace_functions = local_unnamed_addr constant [8 x ptr] [ptr @chali_code_anchor, ptr @f_v234_Multiply_apply, ptr @__chali_impl_program__setup, ptr @f_v238_program__invoke, ptr @__chali_impl_program__work, ptr @f___chali_library_init, ptr @f___chali_library_finish, ptr @f_v470___chali_iface_NamedOperation_apply]
@chali_trace_names = local_unnamed_addr constant [8 x ptr] [ptr @trace_anchor_name, ptr @trace_name0, ptr @trace_name1, ptr @trace_name2, ptr @trace_name3, ptr @trace_name4, ptr @trace_name5, ptr @trace_name6]
@native_name6 = private constant [17 x i8] c"__std_iface_fail\00"
@native_name3 = private constant [17 x i8] c"__std_move_begin\00"
@native_name5 = private constant [15 x i8] c"__std_obj_addr\00"
@native_name4 = private constant [15 x i8] c"__std_obj_name\00"
@native_name1 = private constant [14 x i8] c"__std_obj_new\00"
@native_name2 = private constant [14 x i8] c"__std_obj_set\00"
@native_name0 = private constant [18 x i8] c"__std_object_copy\00"
@s_g_0_program__receiver = internal thread_local unnamed_addr global i1 false

; Function Attrs: nounwind uwtable
define dso_local ptr @chali_standard_stream(i32 noundef %0) local_unnamed_addr #0 {
  %2 = icmp eq i32 %0, 1
  %3 = select i1 %2, i32 1, i32 2
  %4 = icmp eq i32 %0, 0
  %5 = select i1 %4, i32 0, i32 %3
  %6 = tail call ptr @__acrt_iob_func(i32 noundef %5) #25
  ret ptr %6
}

declare dllimport ptr @__acrt_iob_func(i32 noundef) local_unnamed_addr #1

; Function Attrs: cold noreturn nounwind uwtable
define dso_local void @chali_panic(ptr noundef %0) local_unnamed_addr #2 {
  %2 = tail call ptr @__acrt_iob_func(i32 noundef 1) #25
  %3 = tail call i32 @fflush(ptr noundef %2)
  %4 = tail call ptr @__acrt_iob_func(i32 noundef 2) #25
  %5 = tail call i32 (ptr, ptr, ...) @fprintf(ptr noundef %4, ptr noundef nonnull @.str, ptr noundef %0) #25
  tail call void @chali_host_trace() #25
  tail call void @exit(i32 noundef 1) #26
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
define dso_local void @chali_runtime_init() local_unnamed_addr #0 {
  tail call void @chali_host_init() #25
  ret void
}

declare dso_local void @chali_host_init() local_unnamed_addr #1

; Function Attrs: nounwind uwtable
define dso_local void @chali_runtime_finish() local_unnamed_addr #0 {
  %1 = tail call ptr @__acrt_iob_func(i32 noundef 1) #25
  %2 = tail call i32 @fflush(ptr noundef %1)
  tail call void @chali_host_finish() #25
  ret void
}

declare dso_local void @chali_host_finish() local_unnamed_addr #1

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_alloc(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call i64 @chali_host_allocate(i64 noundef %0) #25
  ret i64 %2
}

declare dso_local i64 @chali_host_allocate(i64 noundef) local_unnamed_addr #1

; Function Attrs: nounwind uwtable
define dso_local void @chali_drop(i64 noundef %0) local_unnamed_addr #0 {
  tail call void @chali_host_drop(i64 noundef %0) #25
  ret void
}

declare dso_local void @chali_host_drop(i64 noundef) local_unnamed_addr #1

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_load_i64(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #25
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
  %3 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #25
  store i64 %1, ptr %3, align 1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local double @chali_load_f64(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #25
  %3 = load double, ptr %2, align 1
  ret double %3
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_store_f64(i64 noundef %0, double noundef %1) local_unnamed_addr #0 {
  %3 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #25
  store double %1, ptr %3, align 1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 0, 256) i64 @chali_load_u8(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #25
  %3 = load i8, ptr %2, align 1, !tbaa !12
  %4 = zext i8 %3 to i64
  ret i64 %4
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_store_u8(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = trunc i64 %1 to i8
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #25
  store i8 %3, ptr %4, align 1, !tbaa !12
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 -2147483648, 2147483648) i64 @chali_load_i32(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 4) #25
  %3 = load i32, ptr %2, align 1
  %4 = sext i32 %3 to i64
  ret i64 %4
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_store_i32(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = trunc i64 %1 to i32
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 4) #25
  store i32 %3, ptr %4, align 1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 -32768, 32768) i64 @chali_load_i16(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 2) #25
  %3 = load i16, ptr %2, align 1
  %4 = sext i16 %3 to i64
  ret i64 %4
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_store_i16(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = trunc i64 %1 to i16
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 2) #25
  store i16 %3, ptr %4, align 1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_new(ptr nofree noundef readonly captures(none) %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = icmp eq i64 %1, 0
  %4 = tail call i64 @llvm.umax.i64(i64 %1, i64 1)
  %5 = tail call ptr @malloc(i64 noundef %4) #27
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
  %12 = tail call i64 @chali_host_allocate(i64 noundef %11) #25
  %13 = tail call ptr @chali_host_address(i64 noundef %12, i64 noundef 8) #25
  store i64 %1, ptr %13, align 1
  br i1 %3, label %17, label %14

14:                                               ; preds = %10
  %15 = add nsw i64 %12, 8
  %16 = tail call ptr @chali_host_address(i64 noundef %15, i64 noundef %1) #25
  tail call void @llvm.memcpy.p0.p0.i64(ptr align 1 %16, ptr nonnull align 1 %5, i64 %1, i1 false)
  br label %17

17:                                               ; preds = %14, %10
  %18 = add i64 %1, 8
  %19 = add i64 %18, %12
  %20 = tail call ptr @chali_host_address(i64 noundef %19, i64 noundef 1) #25
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
  %2 = icmp sgt i64 %0, 103
  %3 = zext i1 %2 to i32
  ret i32 %3
}

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_string_length(i64 noundef %0) local_unnamed_addr #0 {
  %2 = icmp eq i64 %0, 0
  br i1 %2, label %11, label %3

3:                                                ; preds = %1
  %4 = icmp slt i64 %0, 104
  br i1 %4, label %8, label %5

5:                                                ; preds = %3
  %6 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #25
  %7 = load i64, ptr %6, align 1
  br label %11

8:                                                ; preds = %3
  %9 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #25
  %10 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %9) #25
  br label %11

11:                                               ; preds = %8, %5, %1
  %12 = phi i64 [ 0, %1 ], [ %7, %5 ], [ %10, %8 ]
  ret i64 %12
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare dso_local i64 @strlen(ptr noundef captures(none)) local_unnamed_addr #9

; Function Attrs: nounwind uwtable
define dso_local ptr @chali_string_bytes(i64 noundef %0) local_unnamed_addr #0 {
  %2 = icmp slt i64 %0, 104
  %3 = select i1 %2, i64 0, i64 8
  %4 = add nsw i64 %3, %0
  %5 = tail call ptr @chali_host_address(i64 noundef %4, i64 noundef 0) #25
  ret ptr %5
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_literal(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #25
  %3 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %2) #25
  %4 = tail call i64 @chali_string_new(ptr noundef nonnull %2, i64 noundef %3)
  ret i64 %4
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_copy(i64 noundef %0) local_unnamed_addr #0 {
  %2 = icmp eq i64 %0, 0
  br i1 %2, label %17, label %3

3:                                                ; preds = %1
  %4 = icmp slt i64 %0, 104
  %5 = select i1 %4, i64 0, i64 8
  %6 = add nsw i64 %5, %0
  %7 = tail call ptr @chali_host_address(i64 noundef %6, i64 noundef 0) #25
  br i1 %4, label %11, label %8

8:                                                ; preds = %3
  %9 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #25
  %10 = load i64, ptr %9, align 1
  br label %14

11:                                               ; preds = %3
  %12 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #25
  %13 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %12) #25
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
  tail call void @chali_host_drop(i64 noundef %0) #25
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 -9223372036854775800, -9223372036854775808) i64 @chali_new_array(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = tail call i64 @llvm.smax.i64(i64 %0, i64 0)
  %4 = mul i64 %3, %1
  %5 = add i64 %4, 8
  %6 = tail call i64 @chali_host_allocate(i64 noundef %5) #25
  %7 = tail call ptr @chali_host_address(i64 noundef %6, i64 noundef 8) #25
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
  call void @llvm.lifetime.start.p0(ptr nonnull %4) #25
  %9 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %4, i64 noundef 128, ptr noundef nonnull @.str.6, ptr noundef %2, i64 noundef %0, i64 noundef %1) #25
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
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #25
  %6 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i64 noundef 128, ptr noundef nonnull @.str.7, ptr noundef %1) #25
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
  %5 = icmp slt i64 %4, 104
  %6 = select i1 %5, i64 0, i64 8
  %7 = add nsw i64 %6, %4
  %8 = tail call ptr @chali_host_address(i64 noundef %7, i64 noundef 0) #25
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
  %26 = tail call ptr @__acrt_iob_func(i32 noundef 1) #25
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
  %102 = tail call ptr @__acrt_iob_func(i32 noundef 1) #25
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
  call void @llvm.lifetime.start.p0(ptr nonnull %5) #25
  call void @llvm.lifetime.start.p0(ptr nonnull %6) #25
  switch i32 %0, label %33 [
    i32 115, label %7
    i32 99, label %25
    i32 63, label %30
  ]

7:                                                ; preds = %4
  %8 = icmp eq i64 %2, 0
  br i1 %8, label %58, label %9

9:                                                ; preds = %7
  %10 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef 64, ptr noundef nonnull @.str.10, ptr noundef %1) #25
  %11 = icmp slt i64 %2, 104
  br i1 %11, label %15, label %12

12:                                               ; preds = %9
  %13 = tail call ptr @chali_host_address(i64 noundef %2, i64 noundef 8) #25
  %14 = load i64, ptr %13, align 1
  br label %18

15:                                               ; preds = %9
  %16 = tail call ptr @chali_host_address(i64 noundef %2, i64 noundef 1) #25
  %17 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %16) #25
  br label %18

18:                                               ; preds = %15, %12
  %19 = phi i64 [ 0, %15 ], [ 8, %12 ]
  %20 = phi i64 [ %17, %15 ], [ %14, %12 ]
  %21 = trunc i64 %20 to i32
  %22 = add nsw i64 %19, %2
  %23 = tail call ptr @chali_host_address(i64 noundef %22, i64 noundef 0) #25
  %24 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %5, i32 noundef %21, ptr noundef %23)
  br label %58

25:                                               ; preds = %4
  %26 = trunc i64 %2 to i32
  %27 = and i32 %26, 255
  %28 = tail call ptr @__acrt_iob_func(i32 noundef 1) #25
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
  %39 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %6, i64 noundef 40, ptr noundef nonnull @.str.12, ptr noundef %1) #25
  %40 = call ptr @strchr(ptr noundef nonnull dereferenceable(1) %6, i32 noundef 46) #25
  %41 = icmp eq ptr %40, null
  br i1 %41, label %42, label %46

42:                                               ; preds = %37
  %43 = call i64 @strlen(ptr noundef nonnull dereferenceable(1) %6) #25
  %44 = sub i64 39, %43
  %45 = call ptr @strncat(ptr noundef nonnull dereferenceable(1) %6, ptr noundef nonnull @.str.13, i64 noundef %44) #25
  br label %46

46:                                               ; preds = %42, %37
  %47 = shl i32 %34, 24
  %48 = ashr exact i32 %47, 24
  %49 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef 64, ptr noundef nonnull @.str.14, ptr noundef nonnull %6, i32 noundef %48) #25
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
  %53 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef 64, ptr noundef nonnull @.str.15, ptr noundef %1, i32 noundef %34) #25
  %54 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef %2)
  br label %58

55:                                               ; preds = %51
  %56 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef 64, ptr noundef nonnull @.str.16, ptr noundef %1) #25
  %57 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef %2)
  br label %58

58:                                               ; preds = %55, %52, %46, %25, %18, %7
  call void @llvm.lifetime.end.p0(ptr nonnull %6) #25
  call void @llvm.lifetime.end.p0(ptr nonnull %5) #25
  ret void
}

; Function Attrs: nofree nounwind
declare dso_local noundef i32 @printf(ptr noundef readonly captures(none), ...) local_unnamed_addr #3

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare dso_local ptr @strncat(ptr noalias noundef returned, ptr noalias noundef readonly captures(none), i64 noundef) local_unnamed_addr #11

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_int(i64 noundef %0) local_unnamed_addr #0 {
  %2 = alloca [32 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #25
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
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #25
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_uint(i64 noundef %0) local_unnamed_addr #0 {
  %2 = alloca [32 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #25
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
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #25
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_char(i64 noundef %0) local_unnamed_addr #0 {
  %2 = alloca [32 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #25
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
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #25
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_double(double noundef %0) local_unnamed_addr #0 {
  %2 = alloca [32 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #25
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
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #25
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_string(i64 noundef %0) local_unnamed_addr #0 {
  %2 = alloca [32 x i8], align 16
  %3 = alloca [48 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #25
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #25
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
  %9 = icmp slt i64 %0, 104
  %10 = select i1 %9, i64 0, i64 8
  %11 = add nsw i64 %10, %0
  %12 = tail call ptr @chali_host_address(i64 noundef %11, i64 noundef 0) #25
  br i1 %9, label %16, label %13

13:                                               ; preds = %8
  %14 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #25
  %15 = load i64, ptr %14, align 1
  br label %19

16:                                               ; preds = %8
  %17 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #25
  %18 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %17) #25
  br label %19

19:                                               ; preds = %16, %13
  %20 = phi i64 [ %18, %16 ], [ %15, %13 ]
  %21 = trunc i64 %20 to i32
  %22 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i64 noundef 48, ptr noundef nonnull @.str.10, ptr noundef nonnull %2) #25
  %23 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %3, i32 noundef %21, ptr noundef %12)
  br label %24

24:                                               ; preds = %19, %6, %6, %1
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #25
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #25
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_finish() local_unnamed_addr #0 {
  %1 = load i64, ptr @chali_fmt_at, align 8, !tbaa !13
  %2 = icmp slt i64 %1, 104
  %3 = select i1 %2, i64 0, i64 8
  %4 = add nsw i64 %3, %1
  %5 = tail call ptr @chali_host_address(i64 noundef %4, i64 noundef 0) #25
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
  %18 = tail call ptr @__acrt_iob_func(i32 noundef 1) #25
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
  %5 = icmp slt i64 %0, 104
  br i1 %5, label %9, label %6

6:                                                ; preds = %4
  %7 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #25
  %8 = load i64, ptr %7, align 1
  br label %12

9:                                                ; preds = %4
  %10 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #25
  %11 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %10) #25
  br label %12

12:                                               ; preds = %9, %6, %2
  %13 = phi i64 [ 0, %2 ], [ %11, %9 ], [ %8, %6 ]
  %14 = icmp eq i64 %1, 0
  br i1 %14, label %23, label %15

15:                                               ; preds = %12
  %16 = icmp slt i64 %1, 104
  br i1 %16, label %20, label %17

17:                                               ; preds = %15
  %18 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 8) #25
  %19 = load i64, ptr %18, align 1
  br label %23

20:                                               ; preds = %15
  %21 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 1) #25
  %22 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %21) #25
  br label %23

23:                                               ; preds = %20, %17, %12
  %24 = phi i64 [ 0, %12 ], [ %22, %20 ], [ %19, %17 ]
  %25 = tail call i64 @llvm.umin.i64(i64 %13, i64 %24)
  %26 = icmp eq i64 %25, 0
  br i1 %26, label %.thread, label %27

27:                                               ; preds = %23
  %28 = icmp slt i64 %0, 104
  %29 = select i1 %28, i64 0, i64 8
  %30 = add nsw i64 %29, %0
  %31 = tail call ptr @chali_host_address(i64 noundef %30, i64 noundef 0) #25
  %32 = icmp slt i64 %1, 104
  %33 = select i1 %32, i64 0, i64 8
  %34 = add nsw i64 %33, %1
  %35 = tail call ptr @chali_host_address(i64 noundef %34, i64 noundef 0) #25
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
  %6 = tail call i64 @chali_host_allocate(i64 noundef 9) #25
  %7 = tail call ptr @chali_host_address(i64 noundef %6, i64 noundef 8) #25
  store i64 0, ptr %7, align 1
  %8 = add i64 %6, 8
  %9 = tail call ptr @chali_host_address(i64 noundef %8, i64 noundef 1) #25
  store i8 0, ptr %9, align 1, !tbaa !12
  br label %30

10:                                               ; preds = %3
  %11 = icmp slt i64 %0, 104
  br i1 %11, label %15, label %12

12:                                               ; preds = %10
  %13 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #25
  %14 = load i64, ptr %13, align 1
  br label %18

15:                                               ; preds = %10
  %16 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #25
  %17 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %16) #25
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
  %27 = tail call ptr @chali_host_address(i64 noundef %26, i64 noundef 0) #25
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
  %7 = icmp slt i64 %0, 104
  br i1 %7, label %11, label %8

8:                                                ; preds = %6
  %9 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #25
  %10 = load i64, ptr %9, align 1
  br label %14

11:                                               ; preds = %6
  %12 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #25
  %13 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %12) #25
  br label %14

14:                                               ; preds = %11, %8
  %15 = phi i64 [ %13, %11 ], [ %10, %8 ]
  %16 = icmp slt i64 %1, 104
  br i1 %16, label %20, label %17

17:                                               ; preds = %14
  %18 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 8) #25
  %19 = load i64, ptr %18, align 1
  br label %23

20:                                               ; preds = %14
  %21 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 1) #25
  %22 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %21) #25
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
  %35 = tail call ptr @chali_host_address(i64 noundef %30, i64 noundef 0) #25
  %36 = getelementptr inbounds nuw i8, ptr %35, i64 %34
  %37 = tail call ptr @chali_host_address(i64 noundef %32, i64 noundef 0) #25
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
  %1 = tail call dereferenceable_or_null(256) ptr @malloc(i64 noundef 256) #27
  %2 = icmp eq ptr %1, null
  br i1 %2, label %3, label %.preheader

3:                                                ; preds = %0
  tail call void @chali_panic(ptr noundef nonnull @.str.18)
  unreachable

.preheader:                                       ; preds = %0, %20
  %4 = phi ptr [ %21, %20 ], [ %1, %0 ]
  %5 = phi i64 [ %24, %20 ], [ 0, %0 ]
  %6 = phi i64 [ %22, %20 ], [ 256, %0 ]
  %7 = tail call ptr @__acrt_iob_func(i32 noundef 0) #25
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
  %17 = tail call ptr @realloc(ptr noundef %4, i64 noundef %16) #28
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
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #25
  %4 = icmp slt i64 %1, 0
  %5 = trunc i64 %1 to i32
  %6 = select i1 %4, i32 3, i32 %5
  %7 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i64 noundef 256, ptr noundef nonnull @.str.21, i32 noundef %6, double noundef %0) #25
  %8 = icmp slt i32 %7, 0
  br i1 %8, label %9, label %10

9:                                                ; preds = %2
  tail call void @chali_panic(ptr noundef nonnull @.str.22)
  unreachable

10:                                               ; preds = %2
  %11 = tail call i32 @llvm.umin.i32(i32 %7, i32 255)
  %12 = zext nneg i32 %11 to i64
  %13 = call i64 @chali_string_new(ptr noundef nonnull %3, i64 noundef %12)
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #25
  ret i64 %13
}

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_read_number(i32 noundef %0) local_unnamed_addr #0 {
  %2 = alloca [256 x i8], align 16
  %3 = alloca double, align 8
  %4 = alloca i64, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #25
  %5 = tail call ptr @__acrt_iob_func(i32 noundef 0) #25
  %6 = call ptr @fgets(ptr noundef nonnull %2, i32 noundef 256, ptr noundef %5)
  %7 = icmp eq ptr %6, null
  br i1 %7, label %18, label %8

8:                                                ; preds = %1
  %9 = icmp eq i32 %0, 0
  br i1 %9, label %14, label %10

10:                                               ; preds = %8
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #25
  store double 0.000000e+00, ptr %3, align 8, !tbaa !30
  %11 = call i32 (ptr, ptr, ...) @sscanf(ptr noundef nonnull %2, ptr noundef nonnull @.str.23, ptr noundef nonnull %3)
  %12 = icmp eq i32 %11, 1
  %.pre = load i64, ptr %3, align 8
  %13 = select i1 %12, i64 %.pre, i64 0
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #25
  br label %18

14:                                               ; preds = %8
  call void @llvm.lifetime.start.p0(ptr nonnull %4) #25
  store i64 0, ptr %4, align 8, !tbaa !13
  %15 = call i32 (ptr, ptr, ...) @sscanf(ptr noundef nonnull %2, ptr noundef nonnull @.str.24, ptr noundef nonnull %4)
  %16 = icmp eq i32 %15, 1
  %.pre2 = load i64, ptr %4, align 8
  %17 = select i1 %16, i64 %.pre2, i64 0
  call void @llvm.lifetime.end.p0(ptr nonnull %4) #25
  br label %18

18:                                               ; preds = %14, %10, %1
  %19 = phi i64 [ %17, %14 ], [ %13, %10 ], [ 0, %1 ]
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #25
  ret i64 %19
}

; Function Attrs: nofree nounwind
declare dso_local noundef ptr @fgets(ptr noundef writeonly, i32 noundef, ptr noundef captures(none)) local_unnamed_addr #3

; Function Attrs: nofree nounwind
declare dso_local noundef i32 @sscanf(ptr noundef readonly captures(none), ptr noundef readonly captures(none), ...) local_unnamed_addr #3

; Function Attrs: nounwind uwtable
define dso_local range(i64 0, 2) i64 @chali_stdin_eof() local_unnamed_addr #0 {
  %1 = tail call ptr @__acrt_iob_func(i32 noundef 0) #25
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
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #25
  %3 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %2, i64 noundef 32, ptr noundef nonnull @.str.24, i64 noundef %0) #25
  %4 = call i64 @strlen(ptr noundef nonnull dereferenceable(1) %2) #25
  %5 = call i64 @chali_string_new(ptr noundef nonnull %2, i64 noundef %4)
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #25
  ret i64 %5
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_of_double(double noundef %0) local_unnamed_addr #0 {
  %2 = alloca [64 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #25
  %3 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %2, i64 noundef 64, ptr noundef nonnull @.str.25, double noundef %0) #25
  %4 = call i64 @strlen(ptr noundef nonnull dereferenceable(1) %2) #25
  %5 = call i64 @chali_string_new(ptr noundef nonnull %2, i64 noundef %4)
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #25
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
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #25
  %8 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i64 noundef 128, ptr noundef nonnull @.str.6, ptr noundef nonnull @.str.26, i64 noundef %0, i64 noundef %1) #25
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
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #25
  %5 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %2, i64 noundef 128, ptr noundef nonnull @.str.7, ptr noundef nonnull @.str.27) #25
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
  %4 = tail call ptr @__acrt_iob_func(i32 noundef 1) #25
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
  %4 = icmp slt i64 %0, 104
  %5 = select i1 %4, i64 0, i64 8
  %6 = add nsw i64 %5, %0
  %7 = tail call ptr @chali_host_address(i64 noundef %6, i64 noundef 0) #25
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
  %20 = tail call ptr @__acrt_iob_func(i32 noundef 1) #25
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
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef %2) #25
  %5 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef %2) #25
  tail call void @llvm.memmove.p0.p0.i64(ptr align 1 %4, ptr align 1 %5, i64 %2, i1 false)
  ret i64 %0
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memmove.p0.p0.i64(ptr writeonly captures(none), ptr readonly captures(none), i64, i1 immarg) #5

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_set_bytes(i64 noundef returned %0, i64 noundef %1, i64 noundef %2) local_unnamed_addr #0 {
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef %2) #25
  %5 = trunc i64 %1 to i8
  tail call void @llvm.memset.p0.i64(ptr align 1 %4, i8 %5, i64 %2, i1 false)
  ret i64 %0
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #14

; Function Attrs: nounwind uwtable
define dso_local range(i64 0, 256) i64 @chali_string_at(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = icmp slt i64 %0, 104
  %4 = select i1 %3, i64 0, i64 8
  %5 = add nsw i64 %4, %0
  %6 = tail call ptr @chali_host_address(i64 noundef %5, i64 noundef 0) #25
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
  %7 = tail call ptr @chali_host_address(i64 noundef %5, i64 noundef 1) #25
  store i8 %6, ptr %7, align 1, !tbaa !12
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_concat(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = icmp eq i64 %0, 0
  br i1 %3, label %12, label %4

4:                                                ; preds = %2
  %5 = icmp slt i64 %0, 104
  br i1 %5, label %9, label %6

6:                                                ; preds = %4
  %7 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #25
  %8 = load i64, ptr %7, align 1
  br label %12

9:                                                ; preds = %4
  %10 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #25
  %11 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %10) #25
  br label %12

12:                                               ; preds = %9, %6, %2
  %13 = phi i64 [ 0, %2 ], [ %8, %6 ], [ %11, %9 ]
  %14 = icmp eq i64 %1, 0
  br i1 %14, label %23, label %15

15:                                               ; preds = %12
  %16 = icmp slt i64 %1, 104
  br i1 %16, label %20, label %17

17:                                               ; preds = %15
  %18 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 8) #25
  %19 = load i64, ptr %18, align 1
  br label %23

20:                                               ; preds = %15
  %21 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 1) #25
  %22 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %21) #25
  br label %23

23:                                               ; preds = %20, %17, %12
  %24 = phi i64 [ 0, %12 ], [ %19, %17 ], [ %22, %20 ]
  %25 = add i64 %24, %13
  %26 = add i64 %25, 8
  %27 = tail call i64 @chali_host_allocate(i64 noundef %26) #25
  %28 = tail call ptr @chali_host_address(i64 noundef %27, i64 noundef 8) #25
  store i64 %25, ptr %28, align 1
  %29 = add nsw i64 %27, 8
  %30 = tail call ptr @chali_host_address(i64 noundef %29, i64 noundef %13) #25
  %31 = icmp slt i64 %0, 104
  %32 = select i1 %31, i64 0, i64 8
  %33 = add nsw i64 %32, %0
  %34 = tail call ptr @chali_host_address(i64 noundef %33, i64 noundef 0) #25
  tail call void @llvm.memcpy.p0.p0.i64(ptr align 1 %30, ptr align 1 %34, i64 %13, i1 false)
  %35 = add i64 %29, %13
  %36 = tail call ptr @chali_host_address(i64 noundef %35, i64 noundef %24) #25
  %37 = icmp slt i64 %1, 104
  %38 = select i1 %37, i64 0, i64 8
  %39 = add nsw i64 %38, %1
  %40 = tail call ptr @chali_host_address(i64 noundef %39, i64 noundef 0) #25
  tail call void @llvm.memcpy.p0.p0.i64(ptr align 1 %36, ptr align 1 %40, i64 %24, i1 false)
  ret i64 %27
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_owned_drop(i64 noundef %0) local_unnamed_addr #0 {
  tail call void @chali_host_drop(i64 noundef %0) #25
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_function_target(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call i64 @chali_host_function(i64 noundef %0, i32 noundef 0) #25
  ret i64 %2
}

declare dso_local i64 @chali_host_function(i64 noundef, i32 noundef) local_unnamed_addr #1

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_function_environment(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call i64 @chali_host_function(i64 noundef %0, i32 noundef 1) #25
  ret i64 %2
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 0, 2) i64 @chali_function_match(i64 noundef %0, ptr nofree noundef readonly captures(none) %1) local_unnamed_addr #0 {
  %3 = add i64 %0, -1
  %or.cond = icmp ult i64 %3, 103
  br i1 %or.cond, label %4, label %14

4:                                                ; preds = %2
  %5 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %1) #25
  %6 = sub nuw nsw i64 104, %0
  %7 = icmp ult i64 %5, %6
  br i1 %7, label %8, label %14

8:                                                ; preds = %4
  %9 = add nuw nsw i64 %5, 1
  %10 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef %9) #25
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

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define noundef i8 @chali_result_float() local_unnamed_addr #19 {
entry:
  ret i8 0
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable
define void @chali_code_anchor() #20 section "chali_code" {
entry:
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none, target_mem: none)
define void @chali_reset_slots() local_unnamed_addr #17 {
entry:
  store ptr null, ptr @chali_source_function, align 8
  store ptr null, ptr @chali_source_file, align 8
  store i64 0, ptr @chali_source_line, align 8
  store i1 false, ptr @s_g_0_program__receiver, align 1
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define noundef i1 @chali_program_std_bind(ptr nofree readnone captures(none) %registry, ptr nofree readnone captures(none) %error) local_unnamed_addr #19 {
entry:
  ret i1 true
}

; Function Attrs: alwaysinline uwtable
define internal range(i64 0, 2147483648) i64 @f_v234_Multiply_apply(i64 %p0, i64 %p1) #21 section "chali_code" {
entry:
  store ptr @f_v234_Multiply_apply, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 6, ptr @chali_source_line, align 8
  %revision_pointer.i3 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i4 = icmp eq ptr %revision_pointer.i3, null
  br i1 %has_revision.not.i4, label %slow.i19, label %check.i5

check.i5:                                         ; preds = %entry
  %revision.i6 = load atomic i64, ptr %revision_pointer.i3 acquire, align 8
  %stamp.i7 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i8 = icmp eq i64 %revision.i6, %stamp.i7
  %enabled.i9 = icmp ne i64 %revision.i6, 0
  %live.i10 = and i1 %enabled.i9, %same_revision.i8
  %begin.i11 = load i64, ptr @chali_address_view, align 16
  %size.i12 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i13 = sub i64 %p0, %begin.i11
  %inside.i14 = icmp ult i64 %offset.i13, %size.i12
  %remaining.i15 = sub i64 %size.i12, %offset.i13
  %fits.i16 = icmp ugt i64 %remaining.i15, 7
  %range.i17 = and i1 %inside.i14, %fits.i16
  %valid.i18 = and i1 %live.i10, %range.i17
  br i1 %valid.i18, label %fast.i22, label %slow.i19

fast.i22:                                         ; preds = %check.i5
  %base.i23 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i24 = getelementptr i8, ptr %base.i23, i64 %offset.i13
  br label %chali_primitive_address.exit25

slow.i19:                                         ; preds = %check.i5, %entry
  %resolved.i20 = tail call ptr @chali_host_address_view(i64 %p0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit25

chali_primitive_address.exit25:                   ; preds = %fast.i22, %slow.i19
  %common.ret.op.i21 = phi ptr [ %physical.i24, %fast.i22 ], [ %resolved.i20, %slow.i19 ]
  %r7 = load i64, ptr %common.ret.op.i21, align 1
  %r12 = add i64 %p0, 8
  store ptr @f_v234_Multiply_apply, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 6, ptr @chali_source_line, align 8
  %revision_pointer.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i = icmp eq ptr %revision_pointer.i, null
  br i1 %has_revision.not.i, label %slow.i, label %check.i

check.i:                                          ; preds = %chali_primitive_address.exit25
  %revision.i = load atomic i64, ptr %revision_pointer.i acquire, align 8
  %stamp.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i = icmp eq i64 %revision.i, %stamp.i
  %enabled.i = icmp ne i64 %revision.i, 0
  %live.i = and i1 %enabled.i, %same_revision.i
  %begin.i = load i64, ptr @chali_address_view, align 16
  %size.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i = sub i64 %r12, %begin.i
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

slow.i:                                           ; preds = %check.i, %chali_primitive_address.exit25
  %resolved.i = tail call ptr @chali_host_address_view(i64 %r12, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit

chali_primitive_address.exit:                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  %r8 = mul i64 %r7, %p1
  %r14 = load i64, ptr %common.ret.op.i, align 1
  %r15 = add i64 %r14, %r8
  store ptr @f_v234_Multiply_apply, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 6, ptr @chali_source_line, align 8
  %r16 = and i64 %r15, 2147483647
  ret i64 %r16
}

; Function Attrs: uwtable
define noundef i64 @__chali_impl_program__setup() #22 section "chali_code" {
entry:
  %r0.b = load i1, ptr @s_g_0_program__receiver, align 1
  %r0 = select i1 %r0.b, i64 32, i64 0
  %revision_pointer.i2 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i3 = icmp eq ptr %revision_pointer.i2, null
  br i1 %has_revision.not.i3, label %slow.i18, label %check.i4

check.i4:                                         ; preds = %entry
  %revision.i5 = load atomic i64, ptr %revision_pointer.i2 acquire, align 8
  %stamp.i6 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i7 = icmp eq i64 %revision.i5, %stamp.i6
  %enabled.i8 = icmp ne i64 %revision.i5, 0
  %live.i9 = and i1 %enabled.i8, %same_revision.i7
  %begin.i10 = load i64, ptr @chali_address_view, align 16
  %size.i11 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i12 = sub i64 %r0, %begin.i10
  %inside.i13 = icmp ult i64 %offset.i12, %size.i11
  %remaining.i14 = sub i64 %size.i11, %offset.i12
  %fits.i15 = icmp ugt i64 %remaining.i14, 7
  %range.i16 = and i1 %inside.i13, %fits.i15
  %valid.i17 = and i1 %live.i9, %range.i16
  br i1 %valid.i17, label %fast.i21, label %slow.i18

fast.i21:                                         ; preds = %check.i4
  %base.i22 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i23 = getelementptr i8, ptr %base.i22, i64 %offset.i12
  br label %chali_primitive_address.exit24

slow.i18:                                         ; preds = %check.i4, %entry
  %resolved.i19 = tail call ptr @chali_host_address_view(i64 %r0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit24

chali_primitive_address.exit24:                   ; preds = %fast.i21, %slow.i18
  %common.ret.op.i20 = phi ptr [ %physical.i23, %fast.i21 ], [ %resolved.i19, %slow.i18 ]
  store i64 3, ptr %common.ret.op.i20, align 1
  %r5.b = load i1, ptr @s_g_0_program__receiver, align 1
  %r8 = select i1 %r5.b, i64 40, i64 8
  %revision_pointer.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i = icmp eq ptr %revision_pointer.i, null
  br i1 %has_revision.not.i, label %slow.i, label %check.i

check.i:                                          ; preds = %chali_primitive_address.exit24
  %revision.i = load atomic i64, ptr %revision_pointer.i acquire, align 8
  %stamp.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i = icmp eq i64 %revision.i, %stamp.i
  %enabled.i = icmp ne i64 %revision.i, 0
  %live.i = and i1 %enabled.i, %same_revision.i
  %begin.i = load i64, ptr @chali_address_view, align 16
  %size.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i = sub i64 %r8, %begin.i
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

slow.i:                                           ; preds = %check.i, %chali_primitive_address.exit24
  %resolved.i = tail call ptr @chali_host_address_view(i64 %r8, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit

chali_primitive_address.exit:                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  store i64 7, ptr %common.ret.op.i, align 1
  ret i64 0
}

; Function Attrs: uwtable
define internal range(i64 0, 2147483648) i64 @f_v238_program__invoke(i64 %p0, i64 %p1) #22 section "chali_code" {
entry:
  %indirect_arguments = alloca [4 x i64], align 8
  %native_argument_types = alloca [4 x i8], align 1
  store i64 %p0, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r4 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 1, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r5 = call i8 @chali_host_result_float()
  %0 = icmp eq i64 %r4, 0
  br i1 %0, label %1, label %ast_block_2

1:                                                ; preds = %entry
  call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

ast_block_2:                                      ; preds = %entry
  store ptr @f_v238_program__invoke, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 12, ptr @chali_source_line, align 8
  %r11 = call i64 @f_v470___chali_iface_NamedOperation_apply(i64 %r4, i64 poison, i64 %p1)
  call void @chali_host_drop(i64 noundef %r4) #25
  ret i64 %r11
}

; Function Attrs: uwtable
define range(i64 0, 2147483648) i64 @__chali_impl_program__work(i64 %p0) #22 section "chali_code" {
entry:
  %indirect_arguments.i = alloca [4 x i64], align 8
  %native_argument_types.i = alloca [4 x i8], align 1
  %indirect_arguments = alloca [8 x i64], align 8
  %native_argument_types = alloca [8 x i8], align 1
  %r7 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 0, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r8 = call i8 @chali_host_result_float()
  %r11.b = load i1, ptr @s_g_0_program__receiver, align 1
  br i1 %r11.b, label %chali_require_reference.exit, label %0

0:                                                ; preds = %entry
  call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit:                     ; preds = %entry
  store i64 %r7, ptr %indirect_arguments, align 8
  store i8 %r8, ptr %native_argument_types, align 1
  %r16 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  store i64 32, ptr %r16, align 8
  %r17 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  %r18 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 16
  store i64 122, ptr %r18, align 8
  %r20 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 24
  store i64 0, ptr %r20, align 8
  %r22 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 32
  store i64 0, ptr %r22, align 8
  store <4 x i8> zeroinitializer, ptr %r17, align 1
  %r24 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 40
  store i64 48, ptr %r24, align 8
  %r25 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 5
  store i8 0, ptr %r25, align 1
  %r26 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 48
  store i64 64, ptr %r26, align 8
  %r27 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 6
  store i8 0, ptr %r27, align 1
  %r28 = call i64 @chali_host_call(ptr nonnull @native_name2, i64 7, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r29 = call i8 @chali_host_result_float()
  store i64 %r7, ptr %indirect_arguments, align 8
  store i8 %r8, ptr %native_argument_types, align 1
  %r36 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 1, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r37 = call i8 @chali_host_result_float()
  %1 = icmp eq i64 %r36, 0
  store ptr @__chali_impl_program__work, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 17, ptr @chali_source_line, align 8
  br i1 %1, label %3, label %ast_block_3

ast_block_3:                                      ; preds = %chali_require_reference.exit, %f_v238_program__invoke.exit
  %s_v489___ast_239_i_1314.024 = phi i64 [ %r55, %f_v238_program__invoke.exit ], [ 0, %chali_require_reference.exit ]
  %s_v487___ast_239_acc.023 = phi i64 [ %r11.i, %f_v238_program__invoke.exit ], [ %p0, %chali_require_reference.exit ]
  store ptr @__chali_impl_program__work, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 17, ptr @chali_source_line, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i)
  store i64 %r36, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  %r4.i = call i64 @chali_host_call(ptr nonnull @native_name0, i64 1, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i), !inline_history !32
  %r5.i = call i8 @chali_host_result_float(), !inline_history !32
  %2 = icmp eq i64 %r4.i, 0
  br i1 %2, label %4, label %f_v238_program__invoke.exit

3:                                                ; preds = %chali_require_reference.exit
  call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

4:                                                ; preds = %ast_block_3
  call void @chali_panic(ptr noundef nonnull @.str.28), !inline_history !32
  unreachable

f_v238_program__invoke.exit:                      ; preds = %ast_block_3
  store ptr @f_v238_program__invoke, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 12, ptr @chali_source_line, align 8
  %r11.i = call i64 @f_v470___chali_iface_NamedOperation_apply(i64 %r4.i, i64 poison, i64 %s_v487___ast_239_acc.023), !inline_history !32
  call void @chali_host_drop(i64 noundef %r4.i) #25, !inline_history !32
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i)
  %r55 = add nuw nsw i64 %s_v489___ast_239_i_1314.024, 1
  store ptr @__chali_impl_program__work, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 17, ptr @chali_source_line, align 8
  %exitcond.not = icmp eq i64 %r55, 262144
  br i1 %exitcond.not, label %ast_block_8, label %ast_block_3

ast_block_8:                                      ; preds = %f_v238_program__invoke.exit
  call void @chali_host_drop(i64 noundef %r36) #25
  %r67.not = icmp eq i64 %r7, 0
  br i1 %r67.not, label %ast_block_11, label %ast_block_9

ast_block_9:                                      ; preds = %ast_block_8
  call void @chali_host_drop(i64 noundef %r7) #25
  br label %ast_block_11

ast_block_11:                                     ; preds = %ast_block_8, %ast_block_9
  ret i64 %r11.i
}

; Function Attrs: alwaysinline uwtable
define internal noundef i64 @f___chali_library_init() #21 section "chali_code" {
entry:
  %indirect_arguments = alloca [3 x i64], align 8
  %native_argument_types = alloca [3 x i8], align 1
  store i1 true, ptr @s_g_0_program__receiver, align 1
  store i64 32, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r4 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  store i64 16, ptr %r4, align 8
  %r5 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r5, align 1
  %r6 = call i64 @chali_host_call(ptr nonnull @native_name3, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r7 = call i8 @chali_host_result_float()
  %r8.b = load i1, ptr @s_g_0_program__receiver, align 1
  %r8 = select i1 %r8.b, i64 32, i64 0
  %revision_pointer.i2 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i3 = icmp eq ptr %revision_pointer.i2, null
  br i1 %has_revision.not.i3, label %slow.i18, label %check.i4

check.i4:                                         ; preds = %entry
  %revision.i5 = load atomic i64, ptr %revision_pointer.i2 acquire, align 8
  %stamp.i6 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i7 = icmp eq i64 %revision.i5, %stamp.i6
  %enabled.i8 = icmp ne i64 %revision.i5, 0
  %live.i9 = and i1 %enabled.i8, %same_revision.i7
  %begin.i10 = load i64, ptr @chali_address_view, align 16
  %size.i11 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i12 = sub i64 %r8, %begin.i10
  %inside.i13 = icmp ult i64 %offset.i12, %size.i11
  %remaining.i14 = sub i64 %size.i11, %offset.i12
  %fits.i15 = icmp ugt i64 %remaining.i14, 7
  %range.i16 = and i1 %inside.i13, %fits.i15
  %valid.i17 = and i1 %live.i9, %range.i16
  br i1 %valid.i17, label %fast.i21, label %slow.i18

fast.i21:                                         ; preds = %check.i4
  %base.i22 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i23 = getelementptr i8, ptr %base.i22, i64 %offset.i12
  br label %chali_primitive_address.exit24

slow.i18:                                         ; preds = %check.i4, %entry
  %resolved.i19 = call ptr @chali_host_address_view(i64 %r8, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit24

chali_primitive_address.exit24:                   ; preds = %fast.i21, %slow.i18
  %common.ret.op.i20 = phi ptr [ %physical.i23, %fast.i21 ], [ %resolved.i19, %slow.i18 ]
  store i64 0, ptr %common.ret.op.i20, align 1
  %r13.b = load i1, ptr @s_g_0_program__receiver, align 1
  %r16 = select i1 %r13.b, i64 40, i64 8
  %revision_pointer.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i = icmp eq ptr %revision_pointer.i, null
  br i1 %has_revision.not.i, label %slow.i, label %check.i

check.i:                                          ; preds = %chali_primitive_address.exit24
  %revision.i = load atomic i64, ptr %revision_pointer.i acquire, align 8
  %stamp.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i = icmp eq i64 %revision.i, %stamp.i
  %enabled.i = icmp ne i64 %revision.i, 0
  %live.i = and i1 %enabled.i, %same_revision.i
  %begin.i = load i64, ptr @chali_address_view, align 16
  %size.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i = sub i64 %r16, %begin.i
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

slow.i:                                           ; preds = %check.i, %chali_primitive_address.exit24
  %resolved.i = call ptr @chali_host_address_view(i64 %r16, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit

chali_primitive_address.exit:                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  store i64 0, ptr %common.ret.op.i, align 1
  ret i64 0
}

; Function Attrs: alwaysinline mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable
define internal noundef i64 @f___chali_library_finish() #23 section "chali_code" {
entry:
  ret i64 0
}

; Function Attrs: uwtable
define internal range(i64 0, 2147483648) i64 @f_v470___chali_iface_NamedOperation_apply(i64 %p0, i64 %p1, i64 %p2) #22 section "chali_code" {
entry:
  %indirect_arguments = alloca [3 x i64], align 8
  %native_argument_types = alloca [3 x i8], align 1
  store i64 %p0, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r4 = call i64 @chali_host_call(ptr nonnull @native_name4, i64 1, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r5 = call i8 @chali_host_result_float()
  %0 = add i64 %r4, -121
  %1 = icmp ult i64 %0, 2
  br i1 %1, label %ast_block_4, label %ast_block_6

common.ret:                                       ; preds = %ast_block_6, %f_v234_Multiply_apply.exit
  %common.ret.op = phi i64 [ %r16.i, %f_v234_Multiply_apply.exit ], [ 0, %ast_block_6 ]
  ret i64 %common.ret.op

ast_block_4:                                      ; preds = %entry
  store i64 %p0, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r23 = call i64 @chali_host_call(ptr nonnull @native_name5, i64 1, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r24 = call i8 @chali_host_result_float()
  store ptr @f_v234_Multiply_apply, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 6, ptr @chali_source_line, align 8
  %revision_pointer.i3.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i4.i = icmp eq ptr %revision_pointer.i3.i, null
  br i1 %has_revision.not.i4.i, label %slow.i19.i, label %check.i5.i

check.i5.i:                                       ; preds = %ast_block_4
  %revision.i6.i = load atomic i64, ptr %revision_pointer.i3.i acquire, align 8
  %stamp.i7.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i8.i = icmp eq i64 %revision.i6.i, %stamp.i7.i
  %enabled.i9.i = icmp ne i64 %revision.i6.i, 0
  %live.i10.i = and i1 %enabled.i9.i, %same_revision.i8.i
  %begin.i11.i = load i64, ptr @chali_address_view, align 16
  %size.i12.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i13.i = sub i64 %r23, %begin.i11.i
  %inside.i14.i = icmp ult i64 %offset.i13.i, %size.i12.i
  %remaining.i15.i = sub i64 %size.i12.i, %offset.i13.i
  %fits.i16.i = icmp ugt i64 %remaining.i15.i, 7
  %range.i17.i = and i1 %inside.i14.i, %fits.i16.i
  %valid.i18.i = and i1 %live.i10.i, %range.i17.i
  br i1 %valid.i18.i, label %fast.i22.i, label %slow.i19.i

fast.i22.i:                                       ; preds = %check.i5.i
  %base.i23.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i24.i = getelementptr i8, ptr %base.i23.i, i64 %offset.i13.i
  br label %chali_primitive_address.exit25.i

slow.i19.i:                                       ; preds = %check.i5.i, %ast_block_4
  %resolved.i20.i = call ptr @chali_host_address_view(i64 %r23, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit25.i

chali_primitive_address.exit25.i:                 ; preds = %slow.i19.i, %fast.i22.i
  %common.ret.op.i21.i = phi ptr [ %physical.i24.i, %fast.i22.i ], [ %resolved.i20.i, %slow.i19.i ]
  %r7.i = load i64, ptr %common.ret.op.i21.i, align 1
  %r8.i = mul i64 %r7.i, %p2
  %r12.i = add i64 %r23, 8
  store ptr @f_v234_Multiply_apply, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 6, ptr @chali_source_line, align 8
  %revision_pointer.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i = icmp eq ptr %revision_pointer.i.i, null
  br i1 %has_revision.not.i.i, label %slow.i.i, label %check.i.i

check.i.i:                                        ; preds = %chali_primitive_address.exit25.i
  %revision.i.i = load atomic i64, ptr %revision_pointer.i.i acquire, align 8
  %stamp.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i = icmp eq i64 %revision.i.i, %stamp.i.i
  %enabled.i.i = icmp ne i64 %revision.i.i, 0
  %live.i.i = and i1 %enabled.i.i, %same_revision.i.i
  %begin.i.i = load i64, ptr @chali_address_view, align 16
  %size.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i = sub i64 %r12.i, %begin.i.i
  %inside.i.i = icmp ult i64 %offset.i.i, %size.i.i
  %remaining.i.i = sub i64 %size.i.i, %offset.i.i
  %fits.i.i = icmp ugt i64 %remaining.i.i, 7
  %range.i.i = and i1 %inside.i.i, %fits.i.i
  %valid.i.i = and i1 %live.i.i, %range.i.i
  br i1 %valid.i.i, label %fast.i.i, label %slow.i.i

fast.i.i:                                         ; preds = %check.i.i
  %base.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i = getelementptr i8, ptr %base.i.i, i64 %offset.i.i
  br label %f_v234_Multiply_apply.exit

slow.i.i:                                         ; preds = %check.i.i, %chali_primitive_address.exit25.i
  %resolved.i.i = call ptr @chali_host_address_view(i64 %r12.i, i64 8, ptr nonnull @chali_address_view)
  br label %f_v234_Multiply_apply.exit

f_v234_Multiply_apply.exit:                       ; preds = %fast.i.i, %slow.i.i
  %common.ret.op.i.i = phi ptr [ %physical.i.i, %fast.i.i ], [ %resolved.i.i, %slow.i.i ]
  %r14.i = load i64, ptr %common.ret.op.i.i, align 1
  %r15.i = add i64 %r14.i, %r8.i
  store ptr @f_v234_Multiply_apply, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 6, ptr @chali_source_line, align 8
  %r16.i = and i64 %r15.i, 2147483647
  br label %common.ret

ast_block_6:                                      ; preds = %entry
  store i64 %r4, ptr %indirect_arguments, align 8
  store i8 %r5, ptr %native_argument_types, align 1
  %r33 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  store i64 8, ptr %r33, align 8
  %r34 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r34, align 1
  %r35 = call i64 @chali_host_call(ptr nonnull @native_name6, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r36 = call i8 @chali_host_result_float()
  br label %common.ret
}

; Function Attrs: noreturn nounwind
define noundef i64 @chali_invoke(i64 %function, i64 %count, ptr nofree readnone captures(none) %arguments) local_unnamed_addr #24 {
entry:
  %target = tail call i64 @chali_function_target(i64 %function)
  %environment = tail call i64 @chali_function_environment(i64 %function)
  tail call void @chali_abort(i64 0)
  unreachable
}

; Function Attrs: noreturn nounwind
define noundef i64 @chali_function_returns_real(i64 %function) local_unnamed_addr #24 {
entry:
  %target = tail call i64 @chali_function_target(i64 %function)
  tail call void @chali_abort(i64 0)
  unreachable
}

define range(i64 0, 2147483648) i64 @chali_export_dispatch(i64 %index, ptr nofree readonly captures(none) %arguments) local_unnamed_addr {
entry:
  switch i64 %index, label %bad [
    i64 0, label %case0
    i64 1, label %case1
  ]

common.ret:                                       ; preds = %case1, %case0
  %common.ret.op = phi i64 [ 0, %case0 ], [ %result1, %case1 ]
  ret i64 %common.ret.op

case0:                                            ; preds = %entry
  %result0 = tail call i64 @__chali_impl_program__setup()
  br label %common.ret

case1:                                            ; preds = %entry
  %v1_0 = load i64, ptr %arguments, align 8
  %result1 = tail call i64 @__chali_impl_program__work(i64 %v1_0)
  br label %common.ret

bad:                                              ; preds = %entry
  tail call void @chali_abort(i64 0)
  unreachable
}

define dllexport void @chali_init() local_unnamed_addr {
entry:
  %indirect_arguments.i = alloca [3 x i64], align 8
  %native_argument_types.i = alloca [3 x i8], align 1
  tail call void @chali_host_init() #25
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i)
  store i1 true, ptr @s_g_0_program__receiver, align 1
  store i64 32, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  %r4.i = getelementptr inbounds nuw i8, ptr %indirect_arguments.i, i64 8
  store i64 16, ptr %r4.i, align 8
  %r5.i = getelementptr inbounds nuw i8, ptr %native_argument_types.i, i64 1
  store i8 0, ptr %r5.i, align 1
  %r6.i = call i64 @chali_host_call(ptr nonnull @native_name3, i64 2, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r7.i = call i8 @chali_host_result_float()
  %r8.b.i = load i1, ptr @s_g_0_program__receiver, align 1
  %r8.i = select i1 %r8.b.i, i64 32, i64 0
  %revision_pointer.i2.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i3.i = icmp eq ptr %revision_pointer.i2.i, null
  br i1 %has_revision.not.i3.i, label %slow.i18.i, label %check.i4.i

check.i4.i:                                       ; preds = %entry
  %revision.i5.i = load atomic i64, ptr %revision_pointer.i2.i acquire, align 8
  %stamp.i6.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i7.i = icmp eq i64 %revision.i5.i, %stamp.i6.i
  %enabled.i8.i = icmp ne i64 %revision.i5.i, 0
  %live.i9.i = and i1 %enabled.i8.i, %same_revision.i7.i
  %begin.i10.i = load i64, ptr @chali_address_view, align 16
  %size.i11.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i12.i = sub i64 %r8.i, %begin.i10.i
  %inside.i13.i = icmp ult i64 %offset.i12.i, %size.i11.i
  %remaining.i14.i = sub i64 %size.i11.i, %offset.i12.i
  %fits.i15.i = icmp ugt i64 %remaining.i14.i, 7
  %range.i16.i = and i1 %inside.i13.i, %fits.i15.i
  %valid.i17.i = and i1 %live.i9.i, %range.i16.i
  br i1 %valid.i17.i, label %fast.i21.i, label %slow.i18.i

fast.i21.i:                                       ; preds = %check.i4.i
  %base.i22.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i23.i = getelementptr i8, ptr %base.i22.i, i64 %offset.i12.i
  br label %chali_primitive_address.exit24.i

slow.i18.i:                                       ; preds = %check.i4.i, %entry
  %resolved.i19.i = call ptr @chali_host_address_view(i64 %r8.i, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit24.i

chali_primitive_address.exit24.i:                 ; preds = %slow.i18.i, %fast.i21.i
  %common.ret.op.i20.i = phi ptr [ %physical.i23.i, %fast.i21.i ], [ %resolved.i19.i, %slow.i18.i ]
  store i64 0, ptr %common.ret.op.i20.i, align 1
  %r13.b.i = load i1, ptr @s_g_0_program__receiver, align 1
  %r16.i = select i1 %r13.b.i, i64 40, i64 8
  %revision_pointer.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i = icmp eq ptr %revision_pointer.i.i, null
  br i1 %has_revision.not.i.i, label %slow.i.i, label %check.i.i

check.i.i:                                        ; preds = %chali_primitive_address.exit24.i
  %revision.i.i = load atomic i64, ptr %revision_pointer.i.i acquire, align 8
  %stamp.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i = icmp eq i64 %revision.i.i, %stamp.i.i
  %enabled.i.i = icmp ne i64 %revision.i.i, 0
  %live.i.i = and i1 %enabled.i.i, %same_revision.i.i
  %begin.i.i = load i64, ptr @chali_address_view, align 16
  %size.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i = sub i64 %r16.i, %begin.i.i
  %inside.i.i = icmp ult i64 %offset.i.i, %size.i.i
  %remaining.i.i = sub i64 %size.i.i, %offset.i.i
  %fits.i.i = icmp ugt i64 %remaining.i.i, 7
  %range.i.i = and i1 %inside.i.i, %fits.i.i
  %valid.i.i = and i1 %live.i.i, %range.i.i
  br i1 %valid.i.i, label %fast.i.i, label %slow.i.i

fast.i.i:                                         ; preds = %check.i.i
  %base.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i = getelementptr i8, ptr %base.i.i, i64 %offset.i.i
  br label %f___chali_library_init.exit

slow.i.i:                                         ; preds = %check.i.i, %chali_primitive_address.exit24.i
  %resolved.i.i = call ptr @chali_host_address_view(i64 %r16.i, i64 8, ptr nonnull @chali_address_view)
  br label %f___chali_library_init.exit

f___chali_library_init.exit:                      ; preds = %fast.i.i, %slow.i.i
  %common.ret.op.i.i = phi ptr [ %physical.i.i, %fast.i.i ], [ %resolved.i.i, %slow.i.i ]
  store i64 0, ptr %common.ret.op.i.i, align 1
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i)
  ret void
}

; Function Attrs: nounwind
define dllexport void @chali_finish() local_unnamed_addr #25 {
entry:
  %0 = tail call ptr @__acrt_iob_func(i32 noundef 1) #25
  %1 = tail call i32 @fflush(ptr noundef %0)
  tail call void @chali_host_finish() #25
  ret void
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
attributes #19 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #20 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable }
attributes #21 = { alwaysinline uwtable }
attributes #22 = { uwtable }
attributes #23 = { alwaysinline mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable }
attributes #24 = { noreturn nounwind }
attributes #25 = { nounwind }
attributes #26 = { cold noreturn nounwind }
attributes #27 = { allocsize(0) }
attributes #28 = { allocsize(1) }

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
!32 = !{ptr @f_v238_program__invoke}
