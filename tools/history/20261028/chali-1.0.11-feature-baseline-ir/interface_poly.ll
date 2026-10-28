; ChaLi LLVM AOT
; checks: off
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
@.str.28 = private unnamed_addr constant [6 x i8] c"panic\00", align 1

; Function Attrs: nounwind uwtable
define dso_local ptr @chali_standard_stream(i32 noundef %0) local_unnamed_addr #0 {
  %2 = icmp eq i32 %0, 1
  %3 = select i1 %2, i32 1, i32 2
  %4 = icmp eq i32 %0, 0
  %5 = select i1 %4, i32 0, i32 %3
  %6 = tail call ptr @__acrt_iob_func(i32 noundef %5) #17
  ret ptr %6
}

declare dllimport ptr @__acrt_iob_func(i32 noundef) local_unnamed_addr #1

; Function Attrs: cold noreturn nounwind uwtable
define dso_local void @chali_panic(ptr noundef %0) local_unnamed_addr #2 {
  %2 = tail call ptr @__acrt_iob_func(i32 noundef 1) #17
  %3 = tail call i32 @fflush(ptr noundef %2)
  %4 = tail call ptr @__acrt_iob_func(i32 noundef 2) #17
  %5 = tail call i32 (ptr, ptr, ...) @fprintf(ptr noundef %4, ptr noundef nonnull @.str, ptr noundef %0) #17
  tail call void @chali_host_trace() #17
  tail call void @exit(i32 noundef 1) #18
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

25:                                               ; preds = %21, %9, %19
  %26 = phi i64 [ 0, %19 ], [ %24, %21 ], [ %13, %9 ]
  ret i64 %26
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_runtime_init() local_unnamed_addr #0 {
  tail call void @chali_host_init() #17
  ret void
}

declare dso_local void @chali_host_init() local_unnamed_addr #1

; Function Attrs: nounwind uwtable
define dso_local void @chali_runtime_finish() local_unnamed_addr #0 {
  %1 = tail call ptr @__acrt_iob_func(i32 noundef 1) #17
  %2 = tail call i32 @fflush(ptr noundef %1)
  tail call void @chali_host_finish() #17
  ret void
}

declare dso_local void @chali_host_finish() local_unnamed_addr #1

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_alloc(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call i64 @chali_host_allocate(i64 noundef %0) #17
  ret i64 %2
}

declare dso_local i64 @chali_host_allocate(i64 noundef) local_unnamed_addr #1

; Function Attrs: nounwind uwtable
define dso_local void @chali_drop(i64 noundef %0) local_unnamed_addr #0 {
  tail call void @chali_host_drop(i64 noundef %0) #17
  ret void
}

declare dso_local void @chali_host_drop(i64 noundef) local_unnamed_addr #1

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_load_i64(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
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
  %3 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
  store i64 %1, ptr %3, align 1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local double @chali_load_f64(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
  %3 = load double, ptr %2, align 1
  ret double %3
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_store_f64(i64 noundef %0, double noundef %1) local_unnamed_addr #0 {
  %3 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
  store double %1, ptr %3, align 1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 0, 256) i64 @chali_load_u8(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #17
  %3 = load i8, ptr %2, align 1, !tbaa !12
  %4 = zext i8 %3 to i64
  ret i64 %4
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_store_u8(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = trunc i64 %1 to i8
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #17
  store i8 %3, ptr %4, align 1, !tbaa !12
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 -2147483648, 2147483648) i64 @chali_load_i32(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 4) #17
  %3 = load i32, ptr %2, align 1
  %4 = sext i32 %3 to i64
  ret i64 %4
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_store_i32(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = trunc i64 %1 to i32
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 4) #17
  store i32 %3, ptr %4, align 1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 -32768, 32768) i64 @chali_load_i16(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 2) #17
  %3 = load i16, ptr %2, align 1
  %4 = sext i16 %3 to i64
  ret i64 %4
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_store_i16(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = trunc i64 %1 to i16
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 2) #17
  store i16 %3, ptr %4, align 1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_new(ptr nofree noundef readonly captures(none) %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = icmp eq i64 %1, 0
  %4 = tail call i64 @llvm.umax.i64(i64 %1, i64 1)
  %5 = tail call ptr @malloc(i64 noundef %4) #19
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
  %12 = tail call i64 @chali_host_allocate(i64 noundef %11) #17
  %13 = tail call ptr @chali_host_address(i64 noundef %12, i64 noundef 8) #17
  store i64 %1, ptr %13, align 1
  br i1 %3, label %17, label %14

14:                                               ; preds = %10
  %15 = add nsw i64 %12, 8
  %16 = tail call ptr @chali_host_address(i64 noundef %15, i64 noundef %1) #17
  tail call void @llvm.memcpy.p0.p0.i64(ptr align 1 %16, ptr nonnull align 1 %5, i64 %1, i1 false)
  br label %17

17:                                               ; preds = %14, %10
  %18 = add i64 %1, 8
  %19 = add i64 %18, %12
  %20 = tail call ptr @chali_host_address(i64 noundef %19, i64 noundef 1) #17
  store i8 0, ptr %20, align 1, !tbaa !12
  tail call void @free(ptr noundef %5)
  ret i64 %12
}

; Function Attrs: mustprogress nofree nounwind willreturn allockind("alloc,uninitialized") allocsize(0) memory(inaccessiblemem: readwrite, errnomem: write)
declare dso_local noalias noundef ptr @malloc(i64 noundef) local_unnamed_addr #6

; Function Attrs: mustprogress nounwind willreturn allockind("free") memory(argmem: readwrite, inaccessiblemem: readwrite)
declare dso_local void @free(ptr allocptr noundef captures(none)) local_unnamed_addr #7

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable
define dso_local range(i32 0, 2) i32 @chali_on_heap(i64 noundef %0) local_unnamed_addr #8 {
  %2 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %3 = icmp sge i64 %0, %2
  %4 = zext i1 %3 to i32
  ret i32 %4
}

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_string_length(i64 noundef %0) local_unnamed_addr #0 {
  %2 = icmp eq i64 %0, 0
  br i1 %2, label %12, label %3

3:                                                ; preds = %1
  %4 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %5 = icmp slt i64 %0, %4
  br i1 %5, label %9, label %6

6:                                                ; preds = %3
  %7 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
  %8 = load i64, ptr %7, align 1
  br label %12

9:                                                ; preds = %3
  %10 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #17
  %11 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %10) #17
  br label %12

12:                                               ; preds = %6, %9, %1
  %13 = phi i64 [ 0, %1 ], [ %8, %6 ], [ %11, %9 ]
  ret i64 %13
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare dso_local i64 @strlen(ptr noundef captures(none)) local_unnamed_addr #9

; Function Attrs: nounwind uwtable
define dso_local ptr @chali_string_bytes(i64 noundef %0) local_unnamed_addr #0 {
  %2 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %3 = icmp slt i64 %0, %2
  %4 = select i1 %3, i64 0, i64 8
  %5 = add nsw i64 %4, %0
  %6 = tail call ptr @chali_host_address(i64 noundef %5, i64 noundef 0) #17
  ret ptr %6
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_literal(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #17
  %3 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %2) #17
  %4 = tail call i64 @chali_string_new(ptr noundef nonnull %2, i64 noundef %3)
  ret i64 %4
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_copy(i64 noundef %0) local_unnamed_addr #0 {
  %2 = icmp eq i64 %0, 0
  br i1 %2, label %18, label %3

3:                                                ; preds = %1
  %4 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %5 = icmp slt i64 %0, %4
  %6 = select i1 %5, i64 0, i64 8
  %7 = add nsw i64 %6, %0
  %8 = tail call ptr @chali_host_address(i64 noundef %7, i64 noundef 0) #17
  br i1 %5, label %12, label %9

9:                                                ; preds = %3
  %10 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
  %11 = load i64, ptr %10, align 1
  br label %15

12:                                               ; preds = %3
  %13 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #17
  %14 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %13) #17
  br label %15

15:                                               ; preds = %9, %12
  %16 = phi i64 [ %14, %12 ], [ %11, %9 ]
  %17 = tail call i64 @chali_string_new(ptr noundef %8, i64 noundef %16)
  br label %18

18:                                               ; preds = %1, %15
  %19 = phi i64 [ %17, %15 ], [ 0, %1 ]
  ret i64 %19
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_string_drop(i64 noundef %0) local_unnamed_addr #0 {
  tail call void @chali_host_drop(i64 noundef %0) #17
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 -9223372036854775800, -9223372036854775808) i64 @chali_new_array(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = tail call i64 @llvm.smax.i64(i64 %0, i64 0)
  %4 = mul i64 %3, %1
  %5 = add i64 %4, 8
  %6 = tail call i64 @chali_host_allocate(i64 noundef %5) #17
  %7 = tail call ptr @chali_host_address(i64 noundef %6, i64 noundef 8) #17
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
  call void @llvm.lifetime.start.p0(ptr nonnull %4) #17
  %9 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %4, i64 noundef 128, ptr noundef nonnull @.str.6, ptr noundef %2, i64 noundef %0, i64 noundef %1) #17
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
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #17
  %6 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i64 noundef 128, ptr noundef nonnull @.str.7, ptr noundef %1) #17
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
  %3 = load i64, ptr @chali_fmt_at, align 8, !tbaa !13
  %4 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %5 = icmp slt i64 %3, %4
  %6 = select i1 %5, i64 0, i64 8
  %7 = add nsw i64 %6, %3
  %8 = tail call ptr @chali_host_address(i64 noundef %7, i64 noundef 0) #17
  %9 = load i64, ptr @chali_fmt_pos, align 8, !tbaa !13
  br label %10

10:                                               ; preds = %18, %2
  %11 = phi i64 [ %9, %2 ], [ %19, %18 ]
  %12 = getelementptr inbounds nuw i8, ptr %8, i64 %11
  %13 = load i8, ptr %12, align 1, !tbaa !12
  switch i8 %13, label %18 [
    i8 0, label %20
    i8 37, label %20
    i8 123, label %14
  ]

14:                                               ; preds = %10
  %15 = getelementptr i8, ptr %12, i64 1
  %16 = load i8, ptr %15, align 1, !tbaa !12
  %17 = icmp eq i8 %16, 125
  br i1 %17, label %20, label %18

18:                                               ; preds = %10, %14
  %19 = add i64 %11, 1
  br label %10, !llvm.loop !15

20:                                               ; preds = %10, %10, %14
  %21 = icmp ugt i64 %11, %9
  br i1 %21, label %22, label %27

22:                                               ; preds = %20
  %23 = getelementptr inbounds nuw i8, ptr %8, i64 %9
  %24 = sub nuw i64 %11, %9
  %25 = tail call ptr @__acrt_iob_func(i32 noundef 1) #17
  %26 = tail call i64 @fwrite(ptr noundef %23, i64 noundef 1, i64 noundef %24, ptr noundef %25)
  store i64 %11, ptr @chali_fmt_pos, align 8, !tbaa !13
  br label %27

27:                                               ; preds = %22, %20
  %28 = load i8, ptr %12, align 1, !tbaa !12
  switch i8 %28, label %73 [
    i8 0, label %29
    i8 123, label %30
  ]

29:                                               ; preds = %27
  store i8 0, ptr %0, align 1, !tbaa !12
  br label %132

30:                                               ; preds = %27
  %31 = add i64 %11, 1
  br label %32

32:                                               ; preds = %36, %30
  %33 = phi i64 [ %31, %30 ], [ %37, %36 ]
  %34 = getelementptr inbounds nuw i8, ptr %8, i64 %33
  %35 = load i8, ptr %34, align 1, !tbaa !12
  switch i8 %35, label %36 [
    i8 0, label %38
    i8 125, label %38
  ]

36:                                               ; preds = %32
  %37 = add i64 %33, 1
  br label %32, !llvm.loop !18

38:                                               ; preds = %32, %32
  %39 = getelementptr inbounds nuw i8, ptr %8, i64 %33
  %40 = icmp eq i8 %35, 125
  %41 = zext i1 %40 to i64
  %42 = add i64 %33, %41
  store i64 %42, ptr @chali_fmt_pos, align 8, !tbaa !13
  store i8 37, ptr %0, align 1, !tbaa !12
  %43 = getelementptr inbounds nuw i8, ptr %0, i64 1
  store i8 0, ptr %43, align 1, !tbaa !12
  %44 = getelementptr inbounds nuw i8, ptr %8, i64 %31
  %45 = load i8, ptr %44, align 1, !tbaa !12
  %46 = icmp eq i8 %45, 58
  %47 = add i64 %11, 2
  %48 = select i1 %46, i64 %47, i64 %31
  %49 = icmp ugt i64 %33, %48
  br i1 %49, label %50, label %132

50:                                               ; preds = %38
  %51 = add i64 %48, 1
  %52 = icmp ult i64 %51, %33
  %53 = icmp ugt i64 %1, 3
  %54 = and i1 %52, %53
  br i1 %54, label %60, label %55

55:                                               ; preds = %60, %50
  %56 = phi i64 [ 1, %50 ], [ %66, %60 ]
  %57 = getelementptr inbounds nuw i8, ptr %0, i64 %56
  store i8 0, ptr %57, align 1, !tbaa !12
  %58 = getelementptr i8, ptr %39, i64 -1
  %59 = load i8, ptr %58, align 1, !tbaa !12
  br label %132

60:                                               ; preds = %50, %60
  %61 = phi i64 [ %68, %60 ], [ %51, %50 ]
  %62 = phi i64 [ %61, %60 ], [ %48, %50 ]
  %63 = phi i64 [ %66, %60 ], [ 1, %50 ]
  %64 = getelementptr inbounds nuw i8, ptr %8, i64 %62
  %65 = load i8, ptr %64, align 1, !tbaa !12
  %66 = add nuw i64 %63, 1
  %67 = getelementptr inbounds nuw i8, ptr %0, i64 %63
  store i8 %65, ptr %67, align 1, !tbaa !12
  %68 = add nuw i64 %61, 1
  %69 = icmp ult i64 %68, %33
  %70 = add nuw i64 %63, 3
  %71 = icmp ult i64 %70, %1
  %72 = and i1 %69, %71
  br i1 %72, label %60, label %55, !llvm.loop !19

73:                                               ; preds = %27
  %74 = add i64 %11, 1
  %75 = getelementptr inbounds nuw i8, ptr %8, i64 %74
  %76 = load i8, ptr %75, align 1, !tbaa !12
  %77 = icmp eq i8 %76, 37
  br i1 %77, label %78, label %82

78:                                               ; preds = %73
  %79 = tail call ptr @__acrt_iob_func(i32 noundef 1) #17
  %80 = tail call i32 @fputc(i32 noundef 37, ptr noundef %79)
  %81 = add i64 %11, 2
  store i64 %81, ptr @chali_fmt_pos, align 8, !tbaa !13
  store i8 0, ptr %0, align 1, !tbaa !12
  br label %132

82:                                               ; preds = %73
  store i8 37, ptr %0, align 1, !tbaa !12
  %83 = getelementptr inbounds nuw i8, ptr %8, i64 %74
  %84 = load i8, ptr %83, align 1, !tbaa !12
  %85 = icmp eq i8 %84, 0
  br i1 %85, label %106, label %86

86:                                               ; preds = %82, %99
  %87 = phi i8 [ %104, %99 ], [ %84, %82 ]
  %88 = phi i64 [ %101, %99 ], [ 1, %82 ]
  %89 = phi i64 [ %100, %99 ], [ %74, %82 ]
  %90 = zext nneg i8 %87 to i64
  %91 = icmp ult i8 %87, 64
  %92 = shl nuw i64 1, %90
  %93 = and i64 %92, 288063289038995457
  %94 = icmp ne i64 %93, 0
  %95 = select i1 %91, i1 %94, i1 false
  %96 = add nuw i64 %88, 2
  %97 = icmp ult i64 %96, %1
  %98 = select i1 %95, i1 %97, i1 false
  br i1 %98, label %99, label %106

99:                                               ; preds = %86
  %100 = add i64 %89, 1
  %101 = add nuw i64 %88, 1
  %102 = getelementptr inbounds nuw i8, ptr %0, i64 %88
  store i8 %87, ptr %102, align 1, !tbaa !12
  %103 = getelementptr inbounds nuw i8, ptr %8, i64 %100
  %104 = load i8, ptr %103, align 1, !tbaa !12
  %105 = icmp eq i8 %104, 0
  br i1 %105, label %106, label %86, !llvm.loop !20

106:                                              ; preds = %99, %86, %82
  %107 = phi i64 [ %74, %82 ], [ %100, %99 ], [ %89, %86 ]
  %108 = phi i64 [ 1, %82 ], [ %101, %99 ], [ %88, %86 ]
  %109 = getelementptr inbounds nuw i8, ptr %8, i64 %107
  %110 = load i8, ptr %109, align 1, !tbaa !12
  %111 = icmp eq i8 %110, 0
  br i1 %111, label %123, label %112

112:                                              ; preds = %106, %118
  %113 = phi i8 [ %121, %118 ], [ %110, %106 ]
  %114 = phi i64 [ %119, %118 ], [ %107, %106 ]
  %115 = sext i8 %113 to i32
  %116 = tail call ptr @memchr(ptr noundef nonnull dereferenceable(1) @.str.9, i32 %115, i64 8)
  %117 = icmp eq ptr %116, null
  br i1 %117, label %123, label %118

118:                                              ; preds = %112
  %119 = add i64 %114, 1
  %120 = getelementptr inbounds nuw i8, ptr %8, i64 %119
  %121 = load i8, ptr %120, align 1, !tbaa !12
  %122 = icmp eq i8 %121, 0
  br i1 %122, label %123, label %112, !llvm.loop !21

123:                                              ; preds = %112, %118, %106
  %124 = phi i64 [ %107, %106 ], [ %114, %112 ], [ %119, %118 ]
  %125 = getelementptr inbounds nuw i8, ptr %8, i64 %124
  %126 = getelementptr inbounds nuw i8, ptr %0, i64 %108
  store i8 0, ptr %126, align 1, !tbaa !12
  %127 = load i8, ptr %125, align 1, !tbaa !12
  %128 = icmp eq i8 %127, 0
  br i1 %128, label %129, label %130

129:                                              ; preds = %123
  store i64 %124, ptr @chali_fmt_pos, align 8, !tbaa !13
  br label %132

130:                                              ; preds = %123
  %131 = add i64 %124, 1
  store i64 %131, ptr @chali_fmt_pos, align 8, !tbaa !13
  br label %132

132:                                              ; preds = %78, %130, %129, %55, %38, %29
  %133 = phi i8 [ 0, %29 ], [ 63, %38 ], [ %59, %55 ], [ 37, %78 ], [ 0, %129 ], [ %127, %130 ]
  %134 = zext i8 %133 to i32
  ret i32 %134
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
  call void @llvm.lifetime.start.p0(ptr nonnull %5) #17
  call void @llvm.lifetime.start.p0(ptr nonnull %6) #17
  switch i32 %0, label %36 [
    i32 115, label %7
    i32 99, label %28
    i32 63, label %33
  ]

7:                                                ; preds = %4
  %8 = icmp eq i64 %2, 0
  br i1 %8, label %61, label %9

9:                                                ; preds = %7
  %10 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef 64, ptr noundef nonnull @.str.10, ptr noundef %1) #17
  %11 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %12 = icmp slt i64 %2, %11
  br i1 %12, label %16, label %13

13:                                               ; preds = %9
  %14 = tail call ptr @chali_host_address(i64 noundef %2, i64 noundef 8) #17
  %15 = load i64, ptr %14, align 1
  br label %19

16:                                               ; preds = %9
  %17 = tail call ptr @chali_host_address(i64 noundef %2, i64 noundef 1) #17
  %18 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %17) #17
  br label %19

19:                                               ; preds = %13, %16
  %20 = phi i64 [ %18, %16 ], [ %15, %13 ]
  %21 = trunc i64 %20 to i32
  %22 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %23 = icmp slt i64 %2, %22
  %24 = select i1 %23, i64 0, i64 8
  %25 = add nsw i64 %24, %2
  %26 = tail call ptr @chali_host_address(i64 noundef %25, i64 noundef 0) #17
  %27 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %5, i32 noundef %21, ptr noundef %26)
  br label %61

28:                                               ; preds = %4
  %29 = trunc i64 %2 to i32
  %30 = and i32 %29, 255
  %31 = tail call ptr @__acrt_iob_func(i32 noundef 1) #17
  %32 = tail call i32 @fputc(i32 noundef %30, ptr noundef %31)
  br label %61

33:                                               ; preds = %4
  %34 = icmp eq i32 %3, 0
  %35 = select i1 %34, i32 100, i32 103
  br label %36

36:                                               ; preds = %4, %33
  %37 = phi i32 [ %35, %33 ], [ %0, %4 ]
  %38 = tail call ptr @memchr(ptr noundef nonnull dereferenceable(1) @.str.11, i32 %37, i64 9)
  %39 = icmp eq ptr %38, null
  br i1 %39, label %54, label %40

40:                                               ; preds = %36
  %41 = bitcast i64 %2 to double
  %42 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %6, i64 noundef 40, ptr noundef nonnull @.str.12, ptr noundef %1) #17
  %43 = call ptr @strchr(ptr noundef nonnull dereferenceable(1) %6, i32 noundef 46) #17
  %44 = icmp eq ptr %43, null
  br i1 %44, label %45, label %49

45:                                               ; preds = %40
  %46 = call i64 @strlen(ptr noundef nonnull dereferenceable(1) %6) #17
  %47 = sub i64 39, %46
  %48 = call ptr @strncat(ptr noundef nonnull dereferenceable(1) %6, ptr noundef nonnull @.str.13, i64 noundef %47) #17
  br label %49

49:                                               ; preds = %45, %40
  %50 = shl i32 %37, 24
  %51 = ashr exact i32 %50, 24
  %52 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef 64, ptr noundef nonnull @.str.14, ptr noundef nonnull %6, i32 noundef %51) #17
  %53 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %5, double noundef %41)
  br label %61

54:                                               ; preds = %36
  switch i32 %37, label %58 [
    i32 120, label %55
    i32 117, label %55
    i32 111, label %55
    i32 88, label %55
  ]

55:                                               ; preds = %54, %54, %54, %54
  %56 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef 64, ptr noundef nonnull @.str.15, ptr noundef %1, i32 noundef %37) #17
  %57 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef %2)
  br label %61

58:                                               ; preds = %54
  %59 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef 64, ptr noundef nonnull @.str.16, ptr noundef %1) #17
  %60 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef %2)
  br label %61

61:                                               ; preds = %7, %58, %55, %49, %28, %19
  call void @llvm.lifetime.end.p0(ptr nonnull %6) #17
  call void @llvm.lifetime.end.p0(ptr nonnull %5) #17
  ret void
}

; Function Attrs: nofree nounwind
declare dso_local noundef i32 @printf(ptr noundef readonly captures(none), ...) local_unnamed_addr #3

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare dso_local ptr @strncat(ptr noalias noundef returned, ptr noalias noundef readonly captures(none), i64 noundef) local_unnamed_addr #11

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_int(i64 noundef %0) local_unnamed_addr #0 {
  %2 = alloca [32 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #17
  %3 = call i32 @chali_fmt_scan(ptr noundef nonnull %2, i64 noundef 32)
  %4 = trunc nuw i32 %3 to i8
  switch i8 %4, label %5 [
    i8 37, label %6
    i8 0, label %6
  ]

5:                                                ; preds = %1
  call void @chali_fmt_emit(i32 noundef %3, ptr noundef nonnull %2, i64 noundef %0, i32 noundef 0)
  br label %6

6:                                                ; preds = %1, %1, %5
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #17
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_uint(i64 noundef %0) local_unnamed_addr #0 {
  %2 = alloca [32 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #17
  %3 = call i32 @chali_fmt_scan(ptr noundef nonnull %2, i64 noundef 32)
  %4 = trunc nuw i32 %3 to i8
  switch i8 %4, label %5 [
    i8 37, label %6
    i8 0, label %6
  ]

5:                                                ; preds = %1
  call void @chali_fmt_emit(i32 noundef %3, ptr noundef nonnull %2, i64 noundef %0, i32 noundef 0)
  br label %6

6:                                                ; preds = %1, %1, %5
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #17
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_char(i64 noundef %0) local_unnamed_addr #0 {
  %2 = alloca [32 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #17
  %3 = call i32 @chali_fmt_scan(ptr noundef nonnull %2, i64 noundef 32)
  %4 = trunc nuw i32 %3 to i8
  switch i8 %4, label %5 [
    i8 37, label %6
    i8 0, label %6
  ]

5:                                                ; preds = %1
  call void @chali_fmt_emit(i32 noundef %3, ptr noundef nonnull %2, i64 noundef %0, i32 noundef 0)
  br label %6

6:                                                ; preds = %1, %1, %5
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #17
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_double(double noundef %0) local_unnamed_addr #0 {
  %2 = alloca [32 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #17
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

7:                                                ; preds = %1, %1, %5
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #17
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_string(i64 noundef %0) local_unnamed_addr #0 {
  %2 = alloca [32 x i8], align 16
  %3 = alloca [48 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #17
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #17
  %4 = call i32 @chali_fmt_scan(ptr noundef nonnull %2, i64 noundef 32)
  %5 = icmp eq i64 %0, 0
  br i1 %5, label %25, label %6

6:                                                ; preds = %1
  %7 = trunc nuw i32 %4 to i8
  switch i8 %7, label %8 [
    i8 37, label %25
    i8 0, label %25
  ]

8:                                                ; preds = %6
  %9 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %10 = icmp slt i64 %0, %9
  %11 = select i1 %10, i64 0, i64 8
  %12 = add nsw i64 %11, %0
  %13 = tail call ptr @chali_host_address(i64 noundef %12, i64 noundef 0) #17
  br i1 %10, label %17, label %14

14:                                               ; preds = %8
  %15 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
  %16 = load i64, ptr %15, align 1
  br label %20

17:                                               ; preds = %8
  %18 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #17
  %19 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %18) #17
  br label %20

20:                                               ; preds = %14, %17
  %21 = phi i64 [ %19, %17 ], [ %16, %14 ]
  %22 = trunc i64 %21 to i32
  %23 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i64 noundef 48, ptr noundef nonnull @.str.10, ptr noundef nonnull %2) #17
  %24 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %3, i32 noundef %22, ptr noundef %13)
  br label %25

25:                                               ; preds = %1, %6, %6, %20
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #17
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #17
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_finish() local_unnamed_addr #0 {
  %1 = load i64, ptr @chali_fmt_at, align 8, !tbaa !13
  %2 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %3 = icmp slt i64 %1, %2
  %4 = select i1 %3, i64 0, i64 8
  %5 = add nsw i64 %4, %1
  %6 = tail call ptr @chali_host_address(i64 noundef %5, i64 noundef 0) #17
  %7 = load i64, ptr @chali_fmt_pos, align 8, !tbaa !13
  br label %8

8:                                                ; preds = %16, %0
  %9 = phi i64 [ %7, %0 ], [ %21, %16 ]
  %10 = getelementptr inbounds nuw i8, ptr %6, i64 %9
  %11 = load i8, ptr %10, align 1, !tbaa !12
  switch i8 %11, label %22 [
    i8 0, label %24
    i8 37, label %12
  ]

12:                                               ; preds = %8
  %13 = getelementptr i8, ptr %10, i64 1
  %14 = load i8, ptr %13, align 1, !tbaa !12
  %15 = icmp eq i8 %14, 37
  br i1 %15, label %16, label %22

16:                                               ; preds = %12, %22
  %17 = phi i32 [ %23, %22 ], [ 37, %12 ]
  %18 = phi i64 [ 1, %22 ], [ 2, %12 ]
  %19 = tail call ptr @__acrt_iob_func(i32 noundef 1) #17
  %20 = tail call i32 @fputc(i32 noundef %17, ptr noundef %19)
  %21 = add i64 %9, %18
  br label %8, !llvm.loop !22

22:                                               ; preds = %8, %12
  %23 = sext i8 %11 to i32
  br label %16

24:                                               ; preds = %8
  store i64 %9, ptr @chali_fmt_pos, align 8, !tbaa !13
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 -1, 2) i64 @chali_string_cmp(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = icmp eq i64 %0, 0
  br i1 %3, label %13, label %4

4:                                                ; preds = %2
  %5 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %6 = icmp slt i64 %0, %5
  br i1 %6, label %10, label %7

7:                                                ; preds = %4
  %8 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
  %9 = load i64, ptr %8, align 1
  br label %13

10:                                               ; preds = %4
  %11 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #17
  %12 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %11) #17
  br label %13

13:                                               ; preds = %10, %7, %2
  %14 = phi i64 [ 0, %2 ], [ %12, %10 ], [ %9, %7 ]
  %15 = icmp eq i64 %1, 0
  br i1 %15, label %25, label %16

16:                                               ; preds = %13
  %17 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %18 = icmp slt i64 %1, %17
  br i1 %18, label %22, label %19

19:                                               ; preds = %16
  %20 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 8) #17
  %21 = load i64, ptr %20, align 1
  br label %25

22:                                               ; preds = %16
  %23 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 1) #17
  %24 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %23) #17
  br label %25

25:                                               ; preds = %22, %19, %13
  %26 = phi i64 [ 0, %13 ], [ %24, %22 ], [ %21, %19 ]
  %27 = tail call i64 @llvm.umin.i64(i64 %14, i64 %26)
  %28 = icmp eq i64 %27, 0
  br i1 %28, label %40, label %29

29:                                               ; preds = %25
  %30 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %31 = icmp slt i64 %0, %30
  %32 = select i1 %31, i64 0, i64 8
  %33 = add nsw i64 %32, %0
  %34 = tail call ptr @chali_host_address(i64 noundef %33, i64 noundef 0) #17
  %35 = icmp slt i64 %1, %30
  %36 = select i1 %35, i64 0, i64 8
  %37 = add nsw i64 %36, %1
  %38 = tail call ptr @chali_host_address(i64 noundef %37, i64 noundef 0) #17
  %39 = tail call i32 @memcmp(ptr noundef %34, ptr noundef %38, i64 noundef %27)
  br label %40

40:                                               ; preds = %25, %29
  %41 = phi i32 [ %39, %29 ], [ 0, %25 ]
  %42 = icmp eq i32 %41, 0
  br i1 %42, label %46, label %43

43:                                               ; preds = %40
  %44 = icmp sgt i32 %41, -1
  %45 = select i1 %44, i64 1, i64 -1
  br label %48

46:                                               ; preds = %40
  %47 = tail call i64 @llvm.ucmp.i64.i64(i64 %14, i64 %26)
  br label %48

48:                                               ; preds = %46, %43
  %49 = phi i64 [ %45, %43 ], [ %47, %46 ]
  ret i64 %49
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare dso_local i32 @memcmp(ptr noundef captures(none), ptr noundef captures(none), i64 noundef) local_unnamed_addr #9

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_sub(i64 noundef %0, i64 noundef %1, i64 noundef %2) local_unnamed_addr #0 {
  %4 = icmp eq i64 %0, 0
  br i1 %4, label %5, label %10

5:                                                ; preds = %3
  %6 = tail call i64 @chali_host_allocate(i64 noundef 9) #17
  %7 = tail call ptr @chali_host_address(i64 noundef %6, i64 noundef 8) #17
  store i64 0, ptr %7, align 1
  %8 = add i64 %6, 8
  %9 = tail call ptr @chali_host_address(i64 noundef %8, i64 noundef 1) #17
  store i8 0, ptr %9, align 1, !tbaa !12
  br label %33

10:                                               ; preds = %3
  %11 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %12 = icmp slt i64 %0, %11
  br i1 %12, label %16, label %13

13:                                               ; preds = %10
  %14 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
  %15 = load i64, ptr %14, align 1
  br label %19

16:                                               ; preds = %10
  %17 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #17
  %18 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %17) #17
  br label %19

19:                                               ; preds = %13, %16
  %20 = phi i64 [ %18, %16 ], [ %15, %13 ]
  %21 = tail call i64 @llvm.smax.i64(i64 %1, i64 0)
  %22 = tail call i64 @llvm.smax.i64(i64 %2, i64 0)
  %23 = tail call i64 @llvm.smin.i64(i64 %21, i64 %20)
  %24 = sub nsw i64 %20, %23
  %25 = tail call i64 @llvm.smin.i64(i64 %22, i64 %24)
  %26 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %27 = icmp slt i64 %0, %26
  %28 = select i1 %27, i64 0, i64 8
  %29 = add nsw i64 %28, %0
  %30 = tail call ptr @chali_host_address(i64 noundef %29, i64 noundef 0) #17
  %31 = getelementptr inbounds i8, ptr %30, i64 %23
  %32 = tail call i64 @chali_string_new(ptr noundef %31, i64 noundef %25)
  br label %33

33:                                               ; preds = %19, %5
  %34 = phi i64 [ %6, %5 ], [ %32, %19 ]
  ret i64 %34
}

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_string_pos(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = icmp eq i64 %0, 0
  %4 = icmp eq i64 %1, 0
  %5 = or i1 %3, %4
  br i1 %5, label %49, label %6

6:                                                ; preds = %2
  %7 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %8 = icmp slt i64 %0, %7
  br i1 %8, label %12, label %9

9:                                                ; preds = %6
  %10 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
  %11 = load i64, ptr %10, align 1
  br label %15

12:                                               ; preds = %6
  %13 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #17
  %14 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %13) #17
  br label %15

15:                                               ; preds = %12, %9
  %16 = phi i64 [ %14, %12 ], [ %11, %9 ]
  %17 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %18 = icmp slt i64 %1, %17
  br i1 %18, label %22, label %19

19:                                               ; preds = %15
  %20 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 8) #17
  %21 = load i64, ptr %20, align 1
  br label %25

22:                                               ; preds = %15
  %23 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 1) #17
  %24 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %23) #17
  br label %25

25:                                               ; preds = %19, %22
  %26 = phi i64 [ %24, %22 ], [ %21, %19 ]
  %27 = icmp eq i64 %26, 0
  br i1 %27, label %49, label %28

28:                                               ; preds = %25
  %29 = icmp sgt i64 %26, %16
  br i1 %29, label %49, label %30

30:                                               ; preds = %28
  %31 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %32 = icmp slt i64 %0, %31
  %33 = select i1 %32, i64 0, i64 8
  %34 = add nsw i64 %33, %0
  %35 = icmp slt i64 %1, %31
  %36 = select i1 %35, i64 0, i64 8
  %37 = add nsw i64 %36, %1
  br label %38

38:                                               ; preds = %30, %45
  %39 = phi i64 [ 0, %30 ], [ %46, %45 ]
  %40 = tail call ptr @chali_host_address(i64 noundef %34, i64 noundef 0) #17
  %41 = getelementptr inbounds nuw i8, ptr %40, i64 %39
  %42 = tail call ptr @chali_host_address(i64 noundef %37, i64 noundef 0) #17
  %43 = tail call i32 @memcmp(ptr noundef %41, ptr noundef %42, i64 noundef %26)
  %44 = icmp eq i32 %43, 0
  br i1 %44, label %49, label %45

45:                                               ; preds = %38
  %46 = add nuw nsw i64 %39, 1
  %47 = add nsw i64 %46, %26
  %48 = icmp sgt i64 %47, %16
  br i1 %48, label %49, label %38, !llvm.loop !23

49:                                               ; preds = %38, %45, %28, %25, %2
  %50 = phi i64 [ -1, %2 ], [ 0, %25 ], [ -1, %28 ], [ -1, %45 ], [ %39, %38 ]
  ret i64 %50
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_read_line() local_unnamed_addr #0 {
  %1 = tail call dereferenceable_or_null(256) ptr @malloc(i64 noundef 256) #19
  %2 = icmp eq ptr %1, null
  br i1 %2, label %3, label %4

3:                                                ; preds = %0
  tail call void @chali_panic(ptr noundef nonnull @.str.18)
  unreachable

4:                                                ; preds = %0, %21
  %5 = phi ptr [ %22, %21 ], [ %1, %0 ]
  %6 = phi i64 [ %25, %21 ], [ 0, %0 ]
  %7 = phi i64 [ %23, %21 ], [ 256, %0 ]
  %8 = tail call ptr @__acrt_iob_func(i32 noundef 0) #17
  %9 = tail call i32 @fgetc(ptr noundef %8)
  switch i32 %9, label %11 [
    i32 -1, label %27
    i32 10, label %27
    i32 0, label %10
  ]

10:                                               ; preds = %4
  tail call void @free(ptr noundef %5)
  tail call void @chali_panic(ptr noundef nonnull @.str.19)
  unreachable

11:                                               ; preds = %4
  %12 = icmp eq i64 %6, %7
  br i1 %12, label %13, label %21

13:                                               ; preds = %11
  %14 = icmp slt i64 %7, 0
  br i1 %14, label %15, label %16

15:                                               ; preds = %13
  tail call void @chali_panic(ptr noundef nonnull @.str.20)
  unreachable

16:                                               ; preds = %13
  %17 = shl nuw i64 %7, 1
  %18 = tail call ptr @realloc(ptr noundef %5, i64 noundef %17) #20
  %19 = icmp eq ptr %18, null
  br i1 %19, label %20, label %21

20:                                               ; preds = %16
  tail call void @free(ptr noundef %5)
  tail call void @chali_panic(ptr noundef nonnull @.str.18)
  unreachable

21:                                               ; preds = %16, %11
  %22 = phi ptr [ %5, %11 ], [ %18, %16 ]
  %23 = phi i64 [ %7, %11 ], [ %17, %16 ]
  %24 = trunc i32 %9 to i8
  %25 = add i64 %6, 1
  %26 = getelementptr inbounds nuw i8, ptr %22, i64 %6
  store i8 %24, ptr %26, align 1, !tbaa !12
  br label %4, !llvm.loop !24

27:                                               ; preds = %4, %4
  %28 = icmp eq i32 %9, 10
  %29 = icmp ne i64 %6, 0
  %30 = select i1 %28, i1 %29, i1 false
  br i1 %30, label %31, label %38

31:                                               ; preds = %27
  %32 = getelementptr i8, ptr %5, i64 %6
  %33 = getelementptr i8, ptr %32, i64 -1
  %34 = load i8, ptr %33, align 1, !tbaa !12
  %35 = icmp eq i8 %34, 13
  %36 = sext i1 %35 to i64
  %37 = add i64 %6, %36
  br label %38

38:                                               ; preds = %31, %27
  %39 = phi i64 [ %6, %27 ], [ %37, %31 ]
  %40 = tail call i64 @chali_string_new(ptr noundef %5, i64 noundef %39)
  tail call void @free(ptr noundef %5)
  ret i64 %40
}

; Function Attrs: nofree nounwind
declare dso_local noundef i32 @fgetc(ptr noundef captures(none)) local_unnamed_addr #3

; Function Attrs: mustprogress nounwind willreturn allockind("realloc") allocsize(1) memory(argmem: readwrite, inaccessiblemem: readwrite, errnomem: write)
declare dso_local noalias noundef ptr @realloc(ptr allocptr noundef captures(none), i64 noundef) local_unnamed_addr #12

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_fixed(double noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = alloca [256 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #17
  %4 = icmp slt i64 %1, 0
  %5 = trunc i64 %1 to i32
  %6 = select i1 %4, i32 3, i32 %5
  %7 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i64 noundef 256, ptr noundef nonnull @.str.21, i32 noundef %6, double noundef %0) #17
  %8 = icmp slt i32 %7, 0
  br i1 %8, label %9, label %10

9:                                                ; preds = %2
  tail call void @chali_panic(ptr noundef nonnull @.str.22)
  unreachable

10:                                               ; preds = %2
  %11 = tail call i32 @llvm.umin.i32(i32 %7, i32 255)
  %12 = zext nneg i32 %11 to i64
  %13 = call i64 @chali_string_new(ptr noundef nonnull %3, i64 noundef %12)
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #17
  ret i64 %13
}

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_read_number(i32 noundef %0) local_unnamed_addr #0 {
  %2 = alloca [256 x i8], align 16
  %3 = alloca double, align 8
  %4 = alloca i64, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #17
  %5 = tail call ptr @__acrt_iob_func(i32 noundef 0) #17
  %6 = call ptr @fgets(ptr noundef nonnull %2, i32 noundef 256, ptr noundef %5)
  %7 = icmp eq ptr %6, null
  br i1 %7, label %22, label %8

8:                                                ; preds = %1
  %9 = icmp eq i32 %0, 0
  br i1 %9, label %16, label %10

10:                                               ; preds = %8
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #17
  store double 0.000000e+00, ptr %3, align 8, !tbaa !25
  %11 = call i32 (ptr, ptr, ...) @sscanf(ptr noundef nonnull %2, ptr noundef nonnull @.str.23, ptr noundef nonnull %3)
  %12 = icmp eq i32 %11, 1
  br i1 %12, label %14, label %13

13:                                               ; preds = %10
  store double 0.000000e+00, ptr %3, align 8, !tbaa !25
  br label %14

14:                                               ; preds = %13, %10
  %15 = load i64, ptr %3, align 8, !tbaa !25
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #17
  br label %22

16:                                               ; preds = %8
  call void @llvm.lifetime.start.p0(ptr nonnull %4) #17
  store i64 0, ptr %4, align 8, !tbaa !13
  %17 = call i32 (ptr, ptr, ...) @sscanf(ptr noundef nonnull %2, ptr noundef nonnull @.str.24, ptr noundef nonnull %4)
  %18 = icmp eq i32 %17, 1
  br i1 %18, label %20, label %19

19:                                               ; preds = %16
  store i64 0, ptr %4, align 8, !tbaa !13
  br label %20

20:                                               ; preds = %19, %16
  %21 = load i64, ptr %4, align 8, !tbaa !13
  call void @llvm.lifetime.end.p0(ptr nonnull %4) #17
  br label %22

22:                                               ; preds = %1, %20, %14
  %23 = phi i64 [ %21, %20 ], [ %15, %14 ], [ 0, %1 ]
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #17
  ret i64 %23
}

; Function Attrs: nofree nounwind
declare dso_local noundef ptr @fgets(ptr noundef writeonly, i32 noundef, ptr noundef captures(none)) local_unnamed_addr #3

; Function Attrs: nofree nounwind
declare dso_local noundef i32 @sscanf(ptr noundef readonly captures(none), ptr noundef readonly captures(none), ...) local_unnamed_addr #3

; Function Attrs: nounwind uwtable
define dso_local range(i64 0, 2) i64 @chali_stdin_eof() local_unnamed_addr #0 {
  %1 = tail call ptr @__acrt_iob_func(i32 noundef 0) #17
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
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #17
  %3 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %2, i64 noundef 32, ptr noundef nonnull @.str.24, i64 noundef %0) #17
  %4 = call i64 @strlen(ptr noundef nonnull dereferenceable(1) %2) #17
  %5 = call i64 @chali_string_new(ptr noundef nonnull %2, i64 noundef %4)
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #17
  ret i64 %5
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_of_double(double noundef %0) local_unnamed_addr #0 {
  %2 = alloca [64 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #17
  %3 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %2, i64 noundef 64, ptr noundef nonnull @.str.25, double noundef %0) #17
  %4 = call i64 @strlen(ptr noundef nonnull dereferenceable(1) %2) #17
  %5 = call i64 @chali_string_new(ptr noundef nonnull %2, i64 noundef %4)
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #17
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
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #17
  %8 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i64 noundef 128, ptr noundef nonnull @.str.6, ptr noundef nonnull @.str.26, i64 noundef %0, i64 noundef %1) #17
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
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #17
  %5 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %2, i64 noundef 128, ptr noundef nonnull @.str.7, ptr noundef nonnull @.str.27) #17
  call void @chali_panic(ptr noundef nonnull %2)
  unreachable

6:                                                ; preds = %1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_put_char(i64 noundef %0) local_unnamed_addr #0 {
  %2 = trunc i64 %0 to i32
  %3 = and i32 %2, 255
  %4 = tail call ptr @__acrt_iob_func(i32 noundef 1) #17
  %5 = tail call i32 @fputc(i32 noundef %3, ptr noundef %4)
  ret void
}

; Function Attrs: noreturn nounwind uwtable
define dso_local void @chali_abort(i64 noundef %0) local_unnamed_addr #13 {
  %2 = icmp eq i64 %0, 0
  br i1 %2, label %4, label %3

3:                                                ; preds = %1
  store i64 %0, ptr @chali_fmt_at, align 8, !tbaa !13
  store i64 0, ptr @chali_fmt_pos, align 8, !tbaa !13
  tail call void @chali_fmt_finish()
  br label %4

4:                                                ; preds = %3, %1
  tail call void @chali_panic(ptr noundef nonnull @.str.28)
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
  tail call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

8:                                                ; preds = %2
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_copy_bytes(i64 noundef returned %0, i64 noundef %1, i64 noundef %2) local_unnamed_addr #0 {
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef %2) #17
  %5 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef %2) #17
  tail call void @llvm.memmove.p0.p0.i64(ptr align 1 %4, ptr align 1 %5, i64 %2, i1 false)
  ret i64 %0
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memmove.p0.p0.i64(ptr writeonly captures(none), ptr readonly captures(none), i64, i1 immarg) #5

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_set_bytes(i64 noundef returned %0, i64 noundef %1, i64 noundef %2) local_unnamed_addr #0 {
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef %2) #17
  %5 = trunc i64 %1 to i8
  tail call void @llvm.memset.p0.i64(ptr align 1 %4, i8 %5, i64 %2, i1 false)
  ret i64 %0
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #14

; Function Attrs: nounwind uwtable
define dso_local range(i64 0, 256) i64 @chali_string_at(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %4 = icmp slt i64 %0, %3
  %5 = select i1 %4, i64 0, i64 8
  %6 = add nsw i64 %5, %0
  %7 = tail call ptr @chali_host_address(i64 noundef %6, i64 noundef 0) #17
  %8 = getelementptr inbounds i8, ptr %7, i64 %1
  %9 = load i8, ptr %8, align 1, !tbaa !12
  %10 = zext i8 %9 to i64
  ret i64 %10
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_string_set(i64 noundef %0, i64 noundef %1, i64 noundef %2) local_unnamed_addr #0 {
  %4 = add nsw i64 %0, 8
  %5 = add nsw i64 %4, %1
  %6 = trunc i64 %2 to i8
  %7 = tail call ptr @chali_host_address(i64 noundef %5, i64 noundef 1) #17
  store i8 %6, ptr %7, align 1, !tbaa !12
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_concat(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = icmp eq i64 %0, 0
  br i1 %3, label %13, label %4

4:                                                ; preds = %2
  %5 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %6 = icmp slt i64 %0, %5
  br i1 %6, label %10, label %7

7:                                                ; preds = %4
  %8 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
  %9 = load i64, ptr %8, align 1
  br label %13

10:                                               ; preds = %4
  %11 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #17
  %12 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %11) #17
  br label %13

13:                                               ; preds = %2, %7, %10
  %14 = phi i64 [ 0, %2 ], [ %9, %7 ], [ %12, %10 ]
  %15 = icmp eq i64 %1, 0
  br i1 %15, label %25, label %16

16:                                               ; preds = %13
  %17 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %18 = icmp slt i64 %1, %17
  br i1 %18, label %22, label %19

19:                                               ; preds = %16
  %20 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 8) #17
  %21 = load i64, ptr %20, align 1
  br label %25

22:                                               ; preds = %16
  %23 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 1) #17
  %24 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %23) #17
  br label %25

25:                                               ; preds = %13, %19, %22
  %26 = phi i64 [ 0, %13 ], [ %21, %19 ], [ %24, %22 ]
  %27 = add i64 %26, %14
  %28 = add i64 %27, 8
  %29 = tail call i64 @chali_host_allocate(i64 noundef %28) #17
  %30 = tail call ptr @chali_host_address(i64 noundef %29, i64 noundef 8) #17
  store i64 %27, ptr %30, align 1
  %31 = add nsw i64 %29, 8
  %32 = tail call ptr @chali_host_address(i64 noundef %31, i64 noundef %14) #17
  %33 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %34 = icmp slt i64 %0, %33
  %35 = select i1 %34, i64 0, i64 8
  %36 = add nsw i64 %35, %0
  %37 = tail call ptr @chali_host_address(i64 noundef %36, i64 noundef 0) #17
  tail call void @llvm.memcpy.p0.p0.i64(ptr align 1 %32, ptr align 1 %37, i64 %14, i1 false)
  %38 = add i64 %31, %14
  %39 = tail call ptr @chali_host_address(i64 noundef %38, i64 noundef %26) #17
  %40 = icmp slt i64 %1, %33
  %41 = select i1 %40, i64 0, i64 8
  %42 = add nsw i64 %41, %1
  %43 = tail call ptr @chali_host_address(i64 noundef %42, i64 noundef 0) #17
  tail call void @llvm.memcpy.p0.p0.i64(ptr align 1 %39, ptr align 1 %43, i64 %26, i1 false)
  ret i64 %29
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_owned_drop(i64 noundef %0) local_unnamed_addr #0 {
  tail call void @chali_host_drop(i64 noundef %0) #17
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_function_target(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call i64 @chali_host_function(i64 noundef %0, i32 noundef 0) #17
  ret i64 %2
}

declare dso_local i64 @chali_host_function(i64 noundef, i32 noundef) local_unnamed_addr #1

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_function_environment(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call i64 @chali_host_function(i64 noundef %0, i32 noundef 1) #17
  ret i64 %2
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 0, 2) i64 @chali_function_match(i64 noundef %0, ptr nofree noundef readonly captures(none) %1) local_unnamed_addr #0 {
  %3 = icmp slt i64 %0, 1
  br i1 %3, label %17, label %4

4:                                                ; preds = %2
  %5 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %6 = icmp ult i64 %0, %5
  br i1 %6, label %7, label %17

7:                                                ; preds = %4
  %8 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %1) #17
  %9 = sub nuw i64 %5, %0
  %10 = icmp ult i64 %8, %9
  br i1 %10, label %11, label %17

11:                                               ; preds = %7
  %12 = add nuw i64 %8, 1
  %13 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef %12) #17
  %14 = tail call i32 @memcmp(ptr noundef nonnull dereferenceable(1) %13, ptr noundef nonnull dereferenceable(1) %1, i64 noundef %12)
  %15 = icmp eq i32 %14, 0
  %16 = zext i1 %15 to i64
  br label %17

17:                                               ; preds = %11, %7, %2, %4
  %18 = phi i64 [ 0, %2 ], [ 0, %4 ], [ %16, %11 ], [ 0, %7 ]
  ret i64 %18
}

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.umax.i64(i64, i64) #15

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.smax.i64(i64, i64) #15

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare ptr @memchr(ptr, i32, i64) local_unnamed_addr #16

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.umin.i64(i64, i64) #15

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.umin.i32(i32, i32) #15

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare range(i64 -1, 2) i64 @llvm.ucmp.i64.i64(i64, i64) #15

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.smin.i64(i64, i64) #15

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
attributes #15 = { nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none) }
attributes #16 = { nocallback nofree nosync nounwind willreturn memory(argmem: read) }
attributes #17 = { nounwind }
attributes #18 = { cold noreturn nounwind }
attributes #19 = { allocsize(0) }
attributes #20 = { allocsize(1) }

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
!19 = distinct !{!19, !16, !17}
!20 = distinct !{!20, !16, !17}
!21 = distinct !{!21, !16, !17}
!22 = distinct !{!22, !16, !17}
!23 = distinct !{!23, !16, !17}
!24 = distinct !{!24, !16, !17}
!25 = !{!26, !26, i64 0}
!26 = !{!"double", !10, i64 0}
declare i64 @chali_host_call(ptr, i64, ptr, ptr)
declare i64 @chali_host_raw_storage(i64, i64, i64, i64, i64, i64)
declare i64 @chali_host_list_storage(i64, i64, i64, i64, i64, i64, i64)
declare i64 @chali_host_map_storage(i64, i64, i64, i64, i64, i64, i64, i64, i64)
declare i64 @chali_host_string_storage(i64, i64)
declare void @chali_host_memory_prepare(i64, i64, i64, i64, ptr, ptr)
declare i64 @chali_host_list_storage_scalar(i64, i64, i64, i64, i64, i64, i64, ptr nocapture writeonly)
declare i64 @chali_host_map_storage_scalar(i64, i64, i64, i64, i64, i64, i64, i64, i64, ptr nocapture writeonly)
declare i8 @chali_host_result_float()

@chali_address_view = internal thread_local global { i64, i64, ptr, ptr, i64 } zeroinitializer
declare ptr @chali_host_address_view(i64, i64, ptr nocapture)
define void @chali_reset_address_view() {
entry:
  store ptr null, ptr getelementptr ({ i64, i64, ptr, ptr, i64 }, ptr @chali_address_view, i32 0, i32 3)
  ret void
}
define internal ptr @chali_primitive_address(i64 %address, i64 %count) alwaysinline {
entry:
  %revision_pointer = load ptr, ptr getelementptr ({ i64, i64, ptr, ptr, i64 }, ptr @chali_address_view, i32 0, i32 3)
  %has_revision = icmp ne ptr %revision_pointer, null
  br i1 %has_revision, label %check, label %slow
check:
  %revision = load atomic i64, ptr %revision_pointer acquire, align 8
  %stamp = load i64, ptr getelementptr ({ i64, i64, ptr, ptr, i64 }, ptr @chali_address_view, i32 0, i32 4)
  %same_revision = icmp eq i64 %revision, %stamp
  %enabled = icmp ne i64 %revision, 0
  %live = and i1 %same_revision, %enabled
  %begin = load i64, ptr getelementptr ({ i64, i64, ptr, ptr, i64 }, ptr @chali_address_view, i32 0, i32 0)
  %size = load i64, ptr getelementptr ({ i64, i64, ptr, ptr, i64 }, ptr @chali_address_view, i32 0, i32 1)
  %offset = sub i64 %address, %begin
  %inside = icmp ult i64 %offset, %size
  %remaining = sub i64 %size, %offset
  %fits = icmp ule i64 %count, %remaining
  %range = and i1 %inside, %fits
  %valid = and i1 %live, %range
  br i1 %valid, label %fast, label %slow
fast:
  %base = load ptr, ptr getelementptr ({ i64, i64, ptr, ptr, i64 }, ptr @chali_address_view, i32 0, i32 2)
  %physical = getelementptr i8, ptr %base, i64 %offset
  ret ptr %physical
slow:
  %resolved = call ptr @chali_host_address_view(i64 %address, i64 %count, ptr @chali_address_view)
  ret ptr %resolved
}
declare double @llvm.sqrt.f64(double)
declare double @llvm.floor.f64(double)
declare double @llvm.ceil.f64(double)
declare double @llvm.round.f64(double)
declare double @llvm.trunc.f64(double)
@chali_return_float = internal thread_local global i8 0
@chali_source_function = internal thread_local global ptr null
@chali_source_file = internal thread_local global ptr null
@chali_source_line = internal thread_local global i64 0
define void @chali_location(ptr %file, ptr %line, ptr %function) {
entry:
  %f = load ptr, ptr @chali_source_file
  %l = load i64, ptr @chali_source_line
  %n = load ptr, ptr @chali_source_function
  store ptr %n, ptr %function
  store ptr %f, ptr %file
  store i64 %l, ptr %line
  ret void
}
define i8 @chali_result_float() {
entry:
  %flag = load i8, ptr @chali_return_float
  ret i8 %flag
}
declare void @chali_host_check_pointer(i64, i64, i64)
declare void @chali_host_process_arguments(i32, ptr)
declare void @chali_host_poll()
declare void @chali_host_tick(i64)
declare i64 @chali_host_copy_words(i64, i64, i64)
@chali_threads_enabled = constant i32 1
@chali_step_limit = constant i64 0
@chali_function_count = constant i64 0
@chali_function_addresses = constant [0 x i64] []
@chali_data_layout_size = constant i64 160
; AOT data image: original=240 stored=240 rows=10/10 segments=1
@chali_data_layout = constant [161 x i8] c"\43\48\4C\59\01\0A\0A\61\73\74\5F\64\61\74\61\5F\30\08\03\15\08\0A\61\73\74\5F\64\61\74\61\5F\31\20\03\10\08\0A\61\73\74\5F\64\61\74\61\5F\32\30\03\10\08\0A\61\73\74\5F\64\61\74\61\5F\33\40\03\10\08\0A\61\73\74\5F\64\61\74\61\5F\34\50\03\0E\08\0A\61\73\74\5F\64\61\74\61\5F\35\60\03\1C\08\0A\61\73\74\5F\64\61\74\61\5F\36\80\01\03\11\08\0A\61\73\74\5F\64\61\74\61\5F\37\98\01\03\1F\08\0A\61\73\74\5F\64\61\74\61\5F\38\B8\01\03\0E\08\0A\61\73\74\5F\64\61\74\61\5F\39\C8\01\03\1C\08\00"
@chali_data_image = constant [240 x i8] c"\00\00\00\00\00\00\00\00\4E\61\6D\65\64\4F\70\65\72\61\74\69\6F\6E\2E\61\70\70\6C\79\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\53\75\62\74\72\61\63\74\20\6D\75\74\26\00\00\00\52\31\33\3A\53\75\62\74\72\61\63\74\20\6D\75\74\26\30\3A\30\3A\38\3A\30\3A\30\3A\00\00\00\00\00\45\78\63\6C\75\73\69\76\65\4F\72\20\6D\75\74\26\00\00\00\00\00\00\00\00\52\31\36\3A\45\78\63\6C\75\73\69\76\65\4F\72\20\6D\75\74\26\30\3A\30\3A\38\3A\30\3A\30\3A\00\00\4D\75\6C\74\69\70\6C\79\20\6D\75\74\26\00\00\00\52\31\33\3A\4D\75\6C\74\69\70\6C\79\20\6D\75\74\26\30\3A\30\3A\38\3A\30\3A\30\3A\00\00\00\00\00\00\00\00\00\00\00\00\00"
@chali_data_segment_count = constant i64 1
@chali_data_segments = constant [1 x { i64, i64, i64 }] [{ i64, i64, i64 } { i64 0, i64 0, i64 240 }]
@chali_data_size = constant i64 240
@chali_heap_size = constant i64 536870912
@chali_allocator_kind = constant i32 1
@source_file0 = private constant [56 x i8] c"\47\3A\2F\44\65\73\6B\74\6F\70\2F\43\68\61\4C\69\2F\62\65\6E\63\68\6D\61\72\6B\73\2F\66\65\61\74\75\72\65\73\2F\69\6E\74\65\72\66\61\63\65\5F\70\6F\6C\79\2E\63\68\6C\00"
define void @chali_code_anchor() uwtable section "chali_code" {
entry:
  ret void
}
@trace_anchor_name = private constant [1 x i8] zeroinitializer
@trace_name0 = private constant [15 x i8] c"\4D\75\6C\74\69\70\6C\79\2E\61\70\70\6C\79\00"
@trace_name1 = private constant [15 x i8] c"\53\75\62\74\72\61\63\74\2E\61\70\70\6C\79\00"
@trace_name2 = private constant [18 x i8] c"\45\78\63\6C\75\73\69\76\65\4F\72\2E\61\70\70\6C\79\00"
@trace_name3 = private constant [15 x i8] c"\70\72\6F\67\72\61\6D\3A\3A\73\65\74\75\70\00"
@trace_name4 = private constant [16 x i8] c"\70\72\6F\67\72\61\6D\3A\3A\63\68\6F\6F\73\65\00"
@trace_name5 = private constant [16 x i8] c"\70\72\6F\67\72\61\6D\3A\3A\69\6E\76\6F\6B\65\00"
@trace_name6 = private constant [14 x i8] c"\70\72\6F\67\72\61\6D\3A\3A\77\6F\72\6B\00"
@trace_name7 = private constant [21 x i8] c"\5F\5F\63\68\61\6C\69\5F\6C\69\62\72\61\72\79\5F\69\6E\69\74\00"
@trace_name8 = private constant [23 x i8] c"\5F\5F\63\68\61\6C\69\5F\6C\69\62\72\61\72\79\5F\66\69\6E\69\73\68\00"
@trace_name9 = private constant [35 x i8] c"\5F\5F\63\68\61\6C\69\5F\69\66\61\63\65\2E\4E\61\6D\65\64\4F\70\65\72\61\74\69\6F\6E\2E\61\70\70\6C\79\00"
@chali_trace_count = constant i64 11
@chali_trace_functions = constant [11 x ptr] [ptr @chali_code_anchor, ptr @f_v234_Multiply_apply, ptr @f_v237_Subtract_apply, ptr @f_v240_ExclusiveOr_apply, ptr @__chali_impl_program__setup, ptr @f_v244_program__choose, ptr @f_v245_program__invoke, ptr @__chali_impl_program__work, ptr @f___chali_library_init, ptr @f___chali_library_finish, ptr @f_v477___chali_iface_NamedOperation_apply]
@chali_trace_names = constant [11 x ptr] [ptr @trace_anchor_name, ptr @trace_name0, ptr @trace_name1, ptr @trace_name2, ptr @trace_name3, ptr @trace_name4, ptr @trace_name5, ptr @trace_name6, ptr @trace_name7, ptr @trace_name8, ptr @trace_name9]
@native_name7 = private constant [17 x i8] c"\5F\5F\73\74\64\5F\69\66\61\63\65\5F\66\61\69\6C\00"
@native_name4 = private constant [17 x i8] c"\5F\5F\73\74\64\5F\6D\6F\76\65\5F\62\65\67\69\6E\00"
@native_name6 = private constant [15 x i8] c"\5F\5F\73\74\64\5F\6F\62\6A\5F\61\64\64\72\00"
@native_name5 = private constant [15 x i8] c"\5F\5F\73\74\64\5F\6F\62\6A\5F\6E\61\6D\65\00"
@native_name0 = private constant [14 x i8] c"\5F\5F\73\74\64\5F\6F\62\6A\5F\6E\65\77\00"
@native_name2 = private constant [14 x i8] c"\5F\5F\73\74\64\5F\6F\62\6A\5F\73\65\74\00"
@native_name3 = private constant [18 x i8] c"\5F\5F\73\74\64\5F\6F\62\6A\65\63\74\5F\63\6F\70\79\00"
@native_name1 = private constant [16 x i8] c"\5F\5F\73\74\64\5F\72\65\66\65\72\65\6E\63\65\00"
@s_g_0_program__first = internal thread_local global i64 0
@t_g_0_program__first = internal thread_local global i8 0
@s_g_1_program__second = internal thread_local global i64 0
@t_g_1_program__second = internal thread_local global i8 0
@s_g_2_program__third = internal thread_local global i64 0
@t_g_2_program__third = internal thread_local global i8 0
define void @chali_reset_slots() {
entry:
  store i8 0, ptr @chali_return_float
  store ptr null, ptr @chali_source_function
  store ptr null, ptr @chali_source_file
  store i64 0, ptr @chali_source_line
  store i64 0, ptr @s_g_0_program__first
  store i8 0, ptr @t_g_0_program__first
  store i64 0, ptr @s_g_1_program__second
  store i8 0, ptr @t_g_1_program__second
  store i64 0, ptr @s_g_2_program__third
  store i8 0, ptr @t_g_2_program__third
  ret void
}
; AOT standard modules:
define i1 @chali_program_std_bind(ptr %registry, ptr %error) {
entry:
  ret i1 true
}
define internal i64 @f_v234_Multiply_apply(i64 %p0, i64 %p1) uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [1 x i64]
  %native_argument_types = alloca [1 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v484___ast_234___chali_parameter_reference_1383 = alloca i64
  store i64 0, ptr %s_v484___ast_234___chali_parameter_reference_1383
  %t_v484___ast_234___chali_parameter_reference_1383 = alloca i8
  store i8 0, ptr %t_v484___ast_234___chali_parameter_reference_1383
  %s_v485___ast_234___chali_ret_v_1384 = alloca i64
  store i64 0, ptr %s_v485___ast_234___chali_ret_v_1384
  %t_v485___ast_234___chali_ret_v_1384 = alloca i8
  store i8 0, ptr %t_v485___ast_234___chali_ret_v_1384
  %s_v486___ast_234_self = alloca i64
  store i64 0, ptr %s_v486___ast_234_self
  %t_v486___ast_234_self = alloca i8
  store i8 0, ptr %t_v486___ast_234_self
  %s_v487___ast_234_value_15 = alloca i64
  store i64 0, ptr %s_v487___ast_234_value_15
  %t_v487___ast_234_value_15 = alloca i8
  store i8 0, ptr %t_v487___ast_234_value_15
  store i64 %p0, ptr %s_v486___ast_234_self
  store i64 %p1, ptr %s_v487___ast_234_value_15
  %r0 = call i64 @chali_alloc(i64 8)
  store i64 %r0, ptr %s_v484___ast_234___chali_parameter_reference_1383
  store i8 0, ptr %t_v484___ast_234___chali_parameter_reference_1383
  %r1 = load i64, ptr %s_v487___ast_234_value_15
  %r2 = load i8, ptr %t_v487___ast_234_value_15
  %r3 = load i64, ptr %s_v484___ast_234___chali_parameter_reference_1383
  %r4 = load i8, ptr %t_v484___ast_234___chali_parameter_reference_1383
  %r5 = call ptr @chali_primitive_address(i64 %r3, i64 8)
  store i64 %r1, ptr %r5, align 1
  %r6 = load i64, ptr %s_v484___ast_234___chali_parameter_reference_1383
  %r7 = load i8, ptr %t_v484___ast_234___chali_parameter_reference_1383
  store ptr @f_v234_Multiply_apply, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 6, ptr @chali_source_line
  %r8 = call ptr @chali_primitive_address(i64 %r6, i64 8)
  %r9 = load i64, ptr %r8, align 1
  %r10 = load i64, ptr %s_v486___ast_234_self
  %r11 = load i8, ptr %t_v486___ast_234_self
  %r12 = mul i64 0, 8
  %r13 = add i64 %r10, %r12
  store ptr @f_v234_Multiply_apply, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 6, ptr @chali_source_line
  %r14 = call ptr @chali_primitive_address(i64 %r13, i64 8)
  %r15 = load i64, ptr %r14, align 1
  store ptr @f_v234_Multiply_apply, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 6, ptr @chali_source_line
  %r16 = mul i64 %r9, %r15
  %r17 = load i64, ptr %s_v486___ast_234_self
  %r18 = load i8, ptr %t_v486___ast_234_self
  %r19 = mul i64 1, 8
  %r20 = add i64 %r17, %r19
  store ptr @f_v234_Multiply_apply, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 6, ptr @chali_source_line
  %r21 = call ptr @chali_primitive_address(i64 %r20, i64 8)
  %r22 = load i64, ptr %r21, align 1
  store ptr @f_v234_Multiply_apply, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 6, ptr @chali_source_line
  %r23 = add i64 %r16, %r22
  store ptr @f_v234_Multiply_apply, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 6, ptr @chali_source_line
  %r24 = and i64 %r23, 2147483647
  store i64 %r24, ptr %s_v485___ast_234___chali_ret_v_1384
  store i8 0, ptr %t_v485___ast_234___chali_ret_v_1384
  %r25 = load i64, ptr %s_v484___ast_234___chali_parameter_reference_1383
  %r26 = load i8, ptr %t_v484___ast_234___chali_parameter_reference_1383
  %r27 = icmp ne i64 %r25, 0
  %r28 = zext i1 %r27 to i64
  %r29 = icmp ne i64 %r28, 0
  br i1 %r29, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r30 = load i64, ptr %s_v484___ast_234___chali_parameter_reference_1383
  %r31 = load i8, ptr %t_v484___ast_234___chali_parameter_reference_1383
  call void @chali_owned_drop(i64 %r30)
  br label %ast_block_2
ast_block_1:
  br label %ast_block_2
ast_block_2:
  store i64 0, ptr %s_v484___ast_234___chali_parameter_reference_1383
  store i8 0, ptr %t_v484___ast_234___chali_parameter_reference_1383
  %r32 = load i64, ptr %s_v485___ast_234___chali_ret_v_1384
  %r33 = load i8, ptr %t_v485___ast_234___chali_ret_v_1384
  store i8 %r33, ptr @chali_return_float
  ret i64 %r32
ast_block_3:
  %r34 = load i64, ptr %s_v484___ast_234___chali_parameter_reference_1383
  %r35 = load i8, ptr %t_v484___ast_234___chali_parameter_reference_1383
  %r36 = icmp ne i64 %r34, 0
  %r37 = zext i1 %r36 to i64
  %r38 = icmp ne i64 %r37, 0
  br i1 %r38, label %ast_block_4, label %ast_block_5
ast_block_4:
  %r39 = load i64, ptr %s_v484___ast_234___chali_parameter_reference_1383
  %r40 = load i8, ptr %t_v484___ast_234___chali_parameter_reference_1383
  call void @chali_owned_drop(i64 %r39)
  br label %ast_block_6
ast_block_5:
  br label %ast_block_6
ast_block_6:
  store i64 0, ptr %s_v484___ast_234___chali_parameter_reference_1383
  store i8 0, ptr %t_v484___ast_234___chali_parameter_reference_1383
  store i8 0, ptr @chali_return_float
  ret i64 0
ast_label_v488___chali_cleanup_Multiply_apply:
  %r41 = load i64, ptr %s_v484___ast_234___chali_parameter_reference_1383
  %r42 = load i8, ptr %t_v484___ast_234___chali_parameter_reference_1383
  %r43 = icmp ne i64 %r41, 0
  %r44 = zext i1 %r43 to i64
  %r45 = icmp ne i64 %r44, 0
  br i1 %r45, label %ast_block_7, label %ast_block_8
ast_block_7:
  %r46 = load i64, ptr %s_v484___ast_234___chali_parameter_reference_1383
  %r47 = load i8, ptr %t_v484___ast_234___chali_parameter_reference_1383
  call void @chali_owned_drop(i64 %r46)
  br label %ast_block_9
ast_block_8:
  br label %ast_block_9
ast_block_9:
  store i64 0, ptr %s_v484___ast_234___chali_parameter_reference_1383
  store i8 0, ptr %t_v484___ast_234___chali_parameter_reference_1383
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define internal i64 @f_v237_Subtract_apply(i64 %p0, i64 %p1) uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [1 x i64]
  %native_argument_types = alloca [1 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v489___ast_237___chali_parameter_reference_1387 = alloca i64
  store i64 0, ptr %s_v489___ast_237___chali_parameter_reference_1387
  %t_v489___ast_237___chali_parameter_reference_1387 = alloca i8
  store i8 0, ptr %t_v489___ast_237___chali_parameter_reference_1387
  %s_v490___ast_237___chali_ret_v_1388 = alloca i64
  store i64 0, ptr %s_v490___ast_237___chali_ret_v_1388
  %t_v490___ast_237___chali_ret_v_1388 = alloca i8
  store i8 0, ptr %t_v490___ast_237___chali_ret_v_1388
  %s_v491___ast_237_self = alloca i64
  store i64 0, ptr %s_v491___ast_237_self
  %t_v491___ast_237_self = alloca i8
  store i8 0, ptr %t_v491___ast_237_self
  %s_v492___ast_237_value_73 = alloca i64
  store i64 0, ptr %s_v492___ast_237_value_73
  %t_v492___ast_237_value_73 = alloca i8
  store i8 0, ptr %t_v492___ast_237_value_73
  store i64 %p0, ptr %s_v491___ast_237_self
  store i64 %p1, ptr %s_v492___ast_237_value_73
  %r0 = call i64 @chali_alloc(i64 8)
  store i64 %r0, ptr %s_v489___ast_237___chali_parameter_reference_1387
  store i8 0, ptr %t_v489___ast_237___chali_parameter_reference_1387
  %r1 = load i64, ptr %s_v492___ast_237_value_73
  %r2 = load i8, ptr %t_v492___ast_237_value_73
  %r3 = load i64, ptr %s_v489___ast_237___chali_parameter_reference_1387
  %r4 = load i8, ptr %t_v489___ast_237___chali_parameter_reference_1387
  %r5 = call ptr @chali_primitive_address(i64 %r3, i64 8)
  store i64 %r1, ptr %r5, align 1
  %r6 = load i64, ptr %s_v489___ast_237___chali_parameter_reference_1387
  %r7 = load i8, ptr %t_v489___ast_237___chali_parameter_reference_1387
  store ptr @f_v237_Subtract_apply, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 11, ptr @chali_source_line
  %r8 = call ptr @chali_primitive_address(i64 %r6, i64 8)
  %r9 = load i64, ptr %r8, align 1
  %r10 = load i64, ptr %s_v491___ast_237_self
  %r11 = load i8, ptr %t_v491___ast_237_self
  %r12 = mul i64 0, 8
  %r13 = add i64 %r10, %r12
  store ptr @f_v237_Subtract_apply, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 11, ptr @chali_source_line
  %r14 = call ptr @chali_primitive_address(i64 %r13, i64 8)
  %r15 = load i64, ptr %r14, align 1
  store ptr @f_v237_Subtract_apply, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 11, ptr @chali_source_line
  %r16 = mul i64 %r9, %r15
  %r17 = load i64, ptr %s_v491___ast_237_self
  %r18 = load i8, ptr %t_v491___ast_237_self
  %r19 = mul i64 1, 8
  %r20 = add i64 %r17, %r19
  store ptr @f_v237_Subtract_apply, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 11, ptr @chali_source_line
  %r21 = call ptr @chali_primitive_address(i64 %r20, i64 8)
  %r22 = load i64, ptr %r21, align 1
  store ptr @f_v237_Subtract_apply, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 11, ptr @chali_source_line
  %r23 = sub i64 %r16, %r22
  store ptr @f_v237_Subtract_apply, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 11, ptr @chali_source_line
  %r24 = and i64 %r23, 2147483647
  store i64 %r24, ptr %s_v490___ast_237___chali_ret_v_1388
  store i8 0, ptr %t_v490___ast_237___chali_ret_v_1388
  %r25 = load i64, ptr %s_v489___ast_237___chali_parameter_reference_1387
  %r26 = load i8, ptr %t_v489___ast_237___chali_parameter_reference_1387
  %r27 = icmp ne i64 %r25, 0
  %r28 = zext i1 %r27 to i64
  %r29 = icmp ne i64 %r28, 0
  br i1 %r29, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r30 = load i64, ptr %s_v489___ast_237___chali_parameter_reference_1387
  %r31 = load i8, ptr %t_v489___ast_237___chali_parameter_reference_1387
  call void @chali_owned_drop(i64 %r30)
  br label %ast_block_2
ast_block_1:
  br label %ast_block_2
ast_block_2:
  store i64 0, ptr %s_v489___ast_237___chali_parameter_reference_1387
  store i8 0, ptr %t_v489___ast_237___chali_parameter_reference_1387
  %r32 = load i64, ptr %s_v490___ast_237___chali_ret_v_1388
  %r33 = load i8, ptr %t_v490___ast_237___chali_ret_v_1388
  store i8 %r33, ptr @chali_return_float
  ret i64 %r32
ast_block_3:
  %r34 = load i64, ptr %s_v489___ast_237___chali_parameter_reference_1387
  %r35 = load i8, ptr %t_v489___ast_237___chali_parameter_reference_1387
  %r36 = icmp ne i64 %r34, 0
  %r37 = zext i1 %r36 to i64
  %r38 = icmp ne i64 %r37, 0
  br i1 %r38, label %ast_block_4, label %ast_block_5
ast_block_4:
  %r39 = load i64, ptr %s_v489___ast_237___chali_parameter_reference_1387
  %r40 = load i8, ptr %t_v489___ast_237___chali_parameter_reference_1387
  call void @chali_owned_drop(i64 %r39)
  br label %ast_block_6
ast_block_5:
  br label %ast_block_6
ast_block_6:
  store i64 0, ptr %s_v489___ast_237___chali_parameter_reference_1387
  store i8 0, ptr %t_v489___ast_237___chali_parameter_reference_1387
  store i8 0, ptr @chali_return_float
  ret i64 0
ast_label_v493___chali_cleanup_Subtract_apply:
  %r41 = load i64, ptr %s_v489___ast_237___chali_parameter_reference_1387
  %r42 = load i8, ptr %t_v489___ast_237___chali_parameter_reference_1387
  %r43 = icmp ne i64 %r41, 0
  %r44 = zext i1 %r43 to i64
  %r45 = icmp ne i64 %r44, 0
  br i1 %r45, label %ast_block_7, label %ast_block_8
ast_block_7:
  %r46 = load i64, ptr %s_v489___ast_237___chali_parameter_reference_1387
  %r47 = load i8, ptr %t_v489___ast_237___chali_parameter_reference_1387
  call void @chali_owned_drop(i64 %r46)
  br label %ast_block_9
ast_block_8:
  br label %ast_block_9
ast_block_9:
  store i64 0, ptr %s_v489___ast_237___chali_parameter_reference_1387
  store i8 0, ptr %t_v489___ast_237___chali_parameter_reference_1387
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define internal i64 @f_v240_ExclusiveOr_apply(i64 %p0, i64 %p1) uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [1 x i64]
  %native_argument_types = alloca [1 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v494___ast_240___chali_parameter_reference_1391 = alloca i64
  store i64 0, ptr %s_v494___ast_240___chali_parameter_reference_1391
  %t_v494___ast_240___chali_parameter_reference_1391 = alloca i8
  store i8 0, ptr %t_v494___ast_240___chali_parameter_reference_1391
  %s_v495___ast_240___chali_ret_v_1392 = alloca i64
  store i64 0, ptr %s_v495___ast_240___chali_ret_v_1392
  %t_v495___ast_240___chali_ret_v_1392 = alloca i8
  store i8 0, ptr %t_v495___ast_240___chali_ret_v_1392
  %s_v496___ast_240_self = alloca i64
  store i64 0, ptr %s_v496___ast_240_self
  %t_v496___ast_240_self = alloca i8
  store i8 0, ptr %t_v496___ast_240_self
  %s_v497___ast_240_value = alloca i64
  store i64 0, ptr %s_v497___ast_240_value
  %t_v497___ast_240_value = alloca i8
  store i8 0, ptr %t_v497___ast_240_value
  store i64 %p0, ptr %s_v496___ast_240_self
  store i64 %p1, ptr %s_v497___ast_240_value
  %r0 = call i64 @chali_alloc(i64 8)
  store i64 %r0, ptr %s_v494___ast_240___chali_parameter_reference_1391
  store i8 0, ptr %t_v494___ast_240___chali_parameter_reference_1391
  %r1 = load i64, ptr %s_v497___ast_240_value
  %r2 = load i8, ptr %t_v497___ast_240_value
  %r3 = load i64, ptr %s_v494___ast_240___chali_parameter_reference_1391
  %r4 = load i8, ptr %t_v494___ast_240___chali_parameter_reference_1391
  %r5 = call ptr @chali_primitive_address(i64 %r3, i64 8)
  store i64 %r1, ptr %r5, align 1
  %r6 = load i64, ptr %s_v494___ast_240___chali_parameter_reference_1391
  %r7 = load i8, ptr %t_v494___ast_240___chali_parameter_reference_1391
  store ptr @f_v240_ExclusiveOr_apply, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 16, ptr @chali_source_line
  %r8 = call ptr @chali_primitive_address(i64 %r6, i64 8)
  %r9 = load i64, ptr %r8, align 1
  %r10 = load i64, ptr %s_v496___ast_240_self
  %r11 = load i8, ptr %t_v496___ast_240_self
  %r12 = mul i64 0, 8
  %r13 = add i64 %r10, %r12
  store ptr @f_v240_ExclusiveOr_apply, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 16, ptr @chali_source_line
  %r14 = call ptr @chali_primitive_address(i64 %r13, i64 8)
  %r15 = load i64, ptr %r14, align 1
  store ptr @f_v240_ExclusiveOr_apply, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 16, ptr @chali_source_line
  %r16 = mul i64 %r9, %r15
  %r17 = load i64, ptr %s_v496___ast_240_self
  %r18 = load i8, ptr %t_v496___ast_240_self
  %r19 = mul i64 1, 8
  %r20 = add i64 %r17, %r19
  store ptr @f_v240_ExclusiveOr_apply, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 16, ptr @chali_source_line
  %r21 = call ptr @chali_primitive_address(i64 %r20, i64 8)
  %r22 = load i64, ptr %r21, align 1
  store ptr @f_v240_ExclusiveOr_apply, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 16, ptr @chali_source_line
  %r23 = xor i64 %r16, %r22
  store ptr @f_v240_ExclusiveOr_apply, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 16, ptr @chali_source_line
  %r24 = and i64 %r23, 2147483647
  store i64 %r24, ptr %s_v495___ast_240___chali_ret_v_1392
  store i8 0, ptr %t_v495___ast_240___chali_ret_v_1392
  %r25 = load i64, ptr %s_v494___ast_240___chali_parameter_reference_1391
  %r26 = load i8, ptr %t_v494___ast_240___chali_parameter_reference_1391
  %r27 = icmp ne i64 %r25, 0
  %r28 = zext i1 %r27 to i64
  %r29 = icmp ne i64 %r28, 0
  br i1 %r29, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r30 = load i64, ptr %s_v494___ast_240___chali_parameter_reference_1391
  %r31 = load i8, ptr %t_v494___ast_240___chali_parameter_reference_1391
  call void @chali_owned_drop(i64 %r30)
  br label %ast_block_2
ast_block_1:
  br label %ast_block_2
ast_block_2:
  store i64 0, ptr %s_v494___ast_240___chali_parameter_reference_1391
  store i8 0, ptr %t_v494___ast_240___chali_parameter_reference_1391
  %r32 = load i64, ptr %s_v495___ast_240___chali_ret_v_1392
  %r33 = load i8, ptr %t_v495___ast_240___chali_ret_v_1392
  store i8 %r33, ptr @chali_return_float
  ret i64 %r32
ast_block_3:
  %r34 = load i64, ptr %s_v494___ast_240___chali_parameter_reference_1391
  %r35 = load i8, ptr %t_v494___ast_240___chali_parameter_reference_1391
  %r36 = icmp ne i64 %r34, 0
  %r37 = zext i1 %r36 to i64
  %r38 = icmp ne i64 %r37, 0
  br i1 %r38, label %ast_block_4, label %ast_block_5
ast_block_4:
  %r39 = load i64, ptr %s_v494___ast_240___chali_parameter_reference_1391
  %r40 = load i8, ptr %t_v494___ast_240___chali_parameter_reference_1391
  call void @chali_owned_drop(i64 %r39)
  br label %ast_block_6
ast_block_5:
  br label %ast_block_6
ast_block_6:
  store i64 0, ptr %s_v494___ast_240___chali_parameter_reference_1391
  store i8 0, ptr %t_v494___ast_240___chali_parameter_reference_1391
  store i8 0, ptr @chali_return_float
  ret i64 0
ast_label_v498___chali_cleanup_ExclusiveOr_apply:
  %r41 = load i64, ptr %s_v494___ast_240___chali_parameter_reference_1391
  %r42 = load i8, ptr %t_v494___ast_240___chali_parameter_reference_1391
  %r43 = icmp ne i64 %r41, 0
  %r44 = zext i1 %r43 to i64
  %r45 = icmp ne i64 %r44, 0
  br i1 %r45, label %ast_block_7, label %ast_block_8
ast_block_7:
  %r46 = load i64, ptr %s_v494___ast_240___chali_parameter_reference_1391
  %r47 = load i8, ptr %t_v494___ast_240___chali_parameter_reference_1391
  call void @chali_owned_drop(i64 %r46)
  br label %ast_block_9
ast_block_8:
  br label %ast_block_9
ast_block_9:
  store i64 0, ptr %s_v494___ast_240___chali_parameter_reference_1391
  store i8 0, ptr %t_v494___ast_240___chali_parameter_reference_1391
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define i64 @__chali_impl_program__setup() uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [1 x i64]
  %native_argument_types = alloca [1 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %r0 = load i64, ptr @s_g_0_program__first
  %r1 = load i8, ptr @t_g_0_program__first
  %r2 = mul i64 0, 8
  %r3 = add i64 %r0, %r2
  %r4 = call ptr @chali_primitive_address(i64 %r3, i64 8)
  store i64 3, ptr %r4, align 1
  %r5 = load i64, ptr @s_g_0_program__first
  %r6 = load i8, ptr @t_g_0_program__first
  %r7 = mul i64 1, 8
  %r8 = add i64 %r5, %r7
  %r9 = call ptr @chali_primitive_address(i64 %r8, i64 8)
  store i64 7, ptr %r9, align 1
  %r10 = load i64, ptr @s_g_1_program__second
  %r11 = load i8, ptr @t_g_1_program__second
  %r12 = mul i64 0, 8
  %r13 = add i64 %r10, %r12
  %r14 = call ptr @chali_primitive_address(i64 %r13, i64 8)
  store i64 5, ptr %r14, align 1
  %r15 = load i64, ptr @s_g_1_program__second
  %r16 = load i8, ptr @t_g_1_program__second
  %r17 = mul i64 1, 8
  %r18 = add i64 %r15, %r17
  %r19 = call ptr @chali_primitive_address(i64 %r18, i64 8)
  store i64 11, ptr %r19, align 1
  %r20 = load i64, ptr @s_g_2_program__third
  %r21 = load i8, ptr @t_g_2_program__third
  %r22 = mul i64 0, 8
  %r23 = add i64 %r20, %r22
  %r24 = call ptr @chali_primitive_address(i64 %r23, i64 8)
  store i64 7, ptr %r24, align 1
  %r25 = load i64, ptr @s_g_2_program__third
  %r26 = load i8, ptr @t_g_2_program__third
  %r27 = mul i64 1, 8
  %r28 = add i64 %r25, %r27
  %r29 = call ptr @chali_primitive_address(i64 %r28, i64 8)
  store i64 13, ptr %r29, align 1
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define internal i64 @f_v244_program__choose(i64 %p0) uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [8 x i64]
  %native_argument_types = alloca [8 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v499___ast_244___chali_box_value = alloca i64
  store i64 0, ptr %s_v499___ast_244___chali_box_value
  %t_v499___ast_244___chali_box_value = alloca i8
  store i8 0, ptr %t_v499___ast_244___chali_box_value
  %s_v500___ast_244___chali_box_value_1397 = alloca i64
  store i64 0, ptr %s_v500___ast_244___chali_box_value_1397
  %t_v500___ast_244___chali_box_value_1397 = alloca i8
  store i8 0, ptr %t_v500___ast_244___chali_box_value_1397
  %s_v501___ast_244___chali_box_value_1400 = alloca i64
  store i64 0, ptr %s_v501___ast_244___chali_box_value_1400
  %t_v501___ast_244___chali_box_value_1400 = alloca i8
  store i8 0, ptr %t_v501___ast_244___chali_box_value_1400
  %s_v502___ast_244___chali_ret_v_1396 = alloca i64
  store i64 0, ptr %s_v502___ast_244___chali_ret_v_1396
  %t_v502___ast_244___chali_ret_v_1396 = alloca i8
  store i8 0, ptr %t_v502___ast_244___chali_ret_v_1396
  %s_v503___ast_244___chali_ret_v_1399 = alloca i64
  store i64 0, ptr %s_v503___ast_244___chali_ret_v_1399
  %t_v503___ast_244___chali_ret_v_1399 = alloca i8
  store i8 0, ptr %t_v503___ast_244___chali_ret_v_1399
  %s_v504___ast_244___chali_ret_v_1402 = alloca i64
  store i64 0, ptr %s_v504___ast_244___chali_ret_v_1402
  %t_v504___ast_244___chali_ret_v_1402 = alloca i8
  store i8 0, ptr %t_v504___ast_244___chali_ret_v_1402
  %s_v505___ast_244___chali_tmp_o_1395 = alloca i64
  store i64 0, ptr %s_v505___ast_244___chali_tmp_o_1395
  %t_v505___ast_244___chali_tmp_o_1395 = alloca i8
  store i8 0, ptr %t_v505___ast_244___chali_tmp_o_1395
  %s_v506___ast_244___chali_tmp_o_1398 = alloca i64
  store i64 0, ptr %s_v506___ast_244___chali_tmp_o_1398
  %t_v506___ast_244___chali_tmp_o_1398 = alloca i8
  store i8 0, ptr %t_v506___ast_244___chali_tmp_o_1398
  %s_v507___ast_244___chali_tmp_o_1401 = alloca i64
  store i64 0, ptr %s_v507___ast_244___chali_tmp_o_1401
  %t_v507___ast_244___chali_tmp_o_1401 = alloca i8
  store i8 0, ptr %t_v507___ast_244___chali_tmp_o_1401
  %s_v508___ast_244_index_593 = alloca i64
  store i64 0, ptr %s_v508___ast_244_index_593
  %t_v508___ast_244_index_593 = alloca i8
  store i8 0, ptr %t_v508___ast_244_index_593
  store i64 %p0, ptr %s_v508___ast_244_index_593
  %r0 = load i64, ptr %s_v508___ast_244_index_593
  %r1 = load i8, ptr %t_v508___ast_244_index_593
  store ptr @f_v244_program__choose, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 27, ptr @chali_source_line
  %r2 = icmp eq i64 %r0, 1
  %r3 = zext i1 %r2 to i64
  %r4 = icmp ne i64 %r3, 0
  br i1 %r4, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r5 = load i64, ptr %s_v505___ast_244___chali_tmp_o_1395
  %r6 = load i8, ptr %t_v505___ast_244___chali_tmp_o_1395
  %r7 = icmp ne i64 %r5, 0
  %r8 = zext i1 %r7 to i64
  %r9 = icmp ne i64 %r8, 0
  br i1 %r9, label %ast_block_3, label %ast_block_4
ast_block_3:
  %r10 = load i64, ptr %s_v505___ast_244___chali_tmp_o_1395
  %r11 = load i8, ptr %t_v505___ast_244___chali_tmp_o_1395
  call void @chali_owned_drop(i64 %r10)
  br label %ast_block_5
ast_block_4:
  br label %ast_block_5
ast_block_5:
  store i64 0, ptr %s_v505___ast_244___chali_tmp_o_1395
  store i8 0, ptr %t_v505___ast_244___chali_tmp_o_1395
  %r12 = call i64 @chali_host_call(ptr @native_name0, i64 0, ptr %indirect_arguments, ptr %native_argument_types)
  %r13 = call i8 @chali_host_result_float()
  store i64 %r12, ptr %s_v499___ast_244___chali_box_value
  store i8 %r13, ptr %t_v499___ast_244___chali_box_value
  %r14 = load i64, ptr %s_v499___ast_244___chali_box_value
  %r15 = load i8, ptr %t_v499___ast_244___chali_box_value
  %r16 = load i64, ptr @s_g_1_program__second
  %r17 = load i8, ptr @t_g_1_program__second
  %r18 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r16, ptr %r18
  %r19 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r17, ptr %r19
  %r20 = call i64 @chali_host_call(ptr @native_name1, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r21 = call i8 @chali_host_result_float()
  %r22 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r14, ptr %r22
  %r23 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r15, ptr %r23
  %r24 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 %r20, ptr %r24
  %r25 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 %r21, ptr %r25
  %r26 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 126, ptr %r26
  %r27 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r27
  %r28 = getelementptr i64, ptr %indirect_arguments, i64 3
  store i64 0, ptr %r28
  %r29 = getelementptr i8, ptr %native_argument_types, i64 3
  store i8 0, ptr %r29
  %r30 = getelementptr i64, ptr %indirect_arguments, i64 4
  store i64 0, ptr %r30
  %r31 = getelementptr i8, ptr %native_argument_types, i64 4
  store i8 0, ptr %r31
  %r32 = getelementptr i64, ptr %indirect_arguments, i64 5
  store i64 80, ptr %r32
  %r33 = getelementptr i8, ptr %native_argument_types, i64 5
  store i8 0, ptr %r33
  %r34 = getelementptr i64, ptr %indirect_arguments, i64 6
  store i64 96, ptr %r34
  %r35 = getelementptr i8, ptr %native_argument_types, i64 6
  store i8 0, ptr %r35
  %r36 = call i64 @chali_host_call(ptr @native_name2, i64 7, ptr %indirect_arguments, ptr %native_argument_types)
  %r37 = call i8 @chali_host_result_float()
  %r38 = load i64, ptr %s_v499___ast_244___chali_box_value
  %r39 = load i8, ptr %t_v499___ast_244___chali_box_value
  store i64 %r38, ptr %s_v505___ast_244___chali_tmp_o_1395
  store i8 %r39, ptr %t_v505___ast_244___chali_tmp_o_1395
  %r40 = load i64, ptr %s_v505___ast_244___chali_tmp_o_1395
  %r41 = load i8, ptr %t_v505___ast_244___chali_tmp_o_1395
  %r42 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r40, ptr %r42
  %r43 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r41, ptr %r43
  %r44 = call i64 @chali_host_call(ptr @native_name3, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r45 = call i8 @chali_host_result_float()
  store i64 %r44, ptr %s_v502___ast_244___chali_ret_v_1396
  store i8 %r45, ptr %t_v502___ast_244___chali_ret_v_1396
  %r46 = load i64, ptr %s_v505___ast_244___chali_tmp_o_1395
  %r47 = load i8, ptr %t_v505___ast_244___chali_tmp_o_1395
  %r48 = icmp ne i64 %r46, 0
  %r49 = zext i1 %r48 to i64
  %r50 = icmp ne i64 %r49, 0
  br i1 %r50, label %ast_block_6, label %ast_block_7
ast_block_6:
  %r51 = load i64, ptr %s_v505___ast_244___chali_tmp_o_1395
  %r52 = load i8, ptr %t_v505___ast_244___chali_tmp_o_1395
  call void @chali_owned_drop(i64 %r51)
  br label %ast_block_8
ast_block_7:
  br label %ast_block_8
ast_block_8:
  store i64 0, ptr %s_v505___ast_244___chali_tmp_o_1395
  store i8 0, ptr %t_v505___ast_244___chali_tmp_o_1395
  %r53 = load i64, ptr %s_v502___ast_244___chali_ret_v_1396
  %r54 = load i8, ptr %t_v502___ast_244___chali_ret_v_1396
  store i8 %r54, ptr @chali_return_float
  ret i64 %r53
ast_block_9:
  %r55 = load i64, ptr %s_v505___ast_244___chali_tmp_o_1395
  %r56 = load i8, ptr %t_v505___ast_244___chali_tmp_o_1395
  %r57 = icmp ne i64 %r55, 0
  %r58 = zext i1 %r57 to i64
  %r59 = icmp ne i64 %r58, 0
  br i1 %r59, label %ast_block_10, label %ast_block_11
ast_block_10:
  %r60 = load i64, ptr %s_v505___ast_244___chali_tmp_o_1395
  %r61 = load i8, ptr %t_v505___ast_244___chali_tmp_o_1395
  call void @chali_owned_drop(i64 %r60)
  br label %ast_block_12
ast_block_11:
  br label %ast_block_12
ast_block_12:
  store i64 0, ptr %s_v505___ast_244___chali_tmp_o_1395
  store i8 0, ptr %t_v505___ast_244___chali_tmp_o_1395
  br label %ast_block_2
ast_block_1:
  br label %ast_block_2
ast_block_2:
  %r62 = load i64, ptr %s_v508___ast_244_index_593
  %r63 = load i8, ptr %t_v508___ast_244_index_593
  store ptr @f_v244_program__choose, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 28, ptr @chali_source_line
  %r64 = icmp eq i64 %r62, 2
  %r65 = zext i1 %r64 to i64
  %r66 = icmp ne i64 %r65, 0
  br i1 %r66, label %ast_block_13, label %ast_block_14
ast_block_13:
  %r67 = load i64, ptr %s_v506___ast_244___chali_tmp_o_1398
  %r68 = load i8, ptr %t_v506___ast_244___chali_tmp_o_1398
  %r69 = icmp ne i64 %r67, 0
  %r70 = zext i1 %r69 to i64
  %r71 = icmp ne i64 %r70, 0
  br i1 %r71, label %ast_block_16, label %ast_block_17
ast_block_16:
  %r72 = load i64, ptr %s_v506___ast_244___chali_tmp_o_1398
  %r73 = load i8, ptr %t_v506___ast_244___chali_tmp_o_1398
  call void @chali_owned_drop(i64 %r72)
  br label %ast_block_18
ast_block_17:
  br label %ast_block_18
ast_block_18:
  store i64 0, ptr %s_v506___ast_244___chali_tmp_o_1398
  store i8 0, ptr %t_v506___ast_244___chali_tmp_o_1398
  %r74 = call i64 @chali_host_call(ptr @native_name0, i64 0, ptr %indirect_arguments, ptr %native_argument_types)
  %r75 = call i8 @chali_host_result_float()
  store i64 %r74, ptr %s_v500___ast_244___chali_box_value_1397
  store i8 %r75, ptr %t_v500___ast_244___chali_box_value_1397
  %r76 = load i64, ptr %s_v500___ast_244___chali_box_value_1397
  %r77 = load i8, ptr %t_v500___ast_244___chali_box_value_1397
  %r78 = load i64, ptr @s_g_2_program__third
  %r79 = load i8, ptr @t_g_2_program__third
  %r80 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r78, ptr %r80
  %r81 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r79, ptr %r81
  %r82 = call i64 @chali_host_call(ptr @native_name1, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r83 = call i8 @chali_host_result_float()
  %r84 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r76, ptr %r84
  %r85 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r77, ptr %r85
  %r86 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 %r82, ptr %r86
  %r87 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 %r83, ptr %r87
  %r88 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 128, ptr %r88
  %r89 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r89
  %r90 = getelementptr i64, ptr %indirect_arguments, i64 3
  store i64 0, ptr %r90
  %r91 = getelementptr i8, ptr %native_argument_types, i64 3
  store i8 0, ptr %r91
  %r92 = getelementptr i64, ptr %indirect_arguments, i64 4
  store i64 0, ptr %r92
  %r93 = getelementptr i8, ptr %native_argument_types, i64 4
  store i8 0, ptr %r93
  %r94 = getelementptr i64, ptr %indirect_arguments, i64 5
  store i64 128, ptr %r94
  %r95 = getelementptr i8, ptr %native_argument_types, i64 5
  store i8 0, ptr %r95
  %r96 = getelementptr i64, ptr %indirect_arguments, i64 6
  store i64 152, ptr %r96
  %r97 = getelementptr i8, ptr %native_argument_types, i64 6
  store i8 0, ptr %r97
  %r98 = call i64 @chali_host_call(ptr @native_name2, i64 7, ptr %indirect_arguments, ptr %native_argument_types)
  %r99 = call i8 @chali_host_result_float()
  %r100 = load i64, ptr %s_v500___ast_244___chali_box_value_1397
  %r101 = load i8, ptr %t_v500___ast_244___chali_box_value_1397
  store i64 %r100, ptr %s_v506___ast_244___chali_tmp_o_1398
  store i8 %r101, ptr %t_v506___ast_244___chali_tmp_o_1398
  %r102 = load i64, ptr %s_v506___ast_244___chali_tmp_o_1398
  %r103 = load i8, ptr %t_v506___ast_244___chali_tmp_o_1398
  %r104 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r102, ptr %r104
  %r105 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r103, ptr %r105
  %r106 = call i64 @chali_host_call(ptr @native_name3, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r107 = call i8 @chali_host_result_float()
  store i64 %r106, ptr %s_v503___ast_244___chali_ret_v_1399
  store i8 %r107, ptr %t_v503___ast_244___chali_ret_v_1399
  %r108 = load i64, ptr %s_v506___ast_244___chali_tmp_o_1398
  %r109 = load i8, ptr %t_v506___ast_244___chali_tmp_o_1398
  %r110 = icmp ne i64 %r108, 0
  %r111 = zext i1 %r110 to i64
  %r112 = icmp ne i64 %r111, 0
  br i1 %r112, label %ast_block_19, label %ast_block_20
ast_block_19:
  %r113 = load i64, ptr %s_v506___ast_244___chali_tmp_o_1398
  %r114 = load i8, ptr %t_v506___ast_244___chali_tmp_o_1398
  call void @chali_owned_drop(i64 %r113)
  br label %ast_block_21
ast_block_20:
  br label %ast_block_21
ast_block_21:
  store i64 0, ptr %s_v506___ast_244___chali_tmp_o_1398
  store i8 0, ptr %t_v506___ast_244___chali_tmp_o_1398
  %r115 = load i64, ptr %s_v503___ast_244___chali_ret_v_1399
  %r116 = load i8, ptr %t_v503___ast_244___chali_ret_v_1399
  store i8 %r116, ptr @chali_return_float
  ret i64 %r115
ast_block_22:
  %r117 = load i64, ptr %s_v506___ast_244___chali_tmp_o_1398
  %r118 = load i8, ptr %t_v506___ast_244___chali_tmp_o_1398
  %r119 = icmp ne i64 %r117, 0
  %r120 = zext i1 %r119 to i64
  %r121 = icmp ne i64 %r120, 0
  br i1 %r121, label %ast_block_23, label %ast_block_24
ast_block_23:
  %r122 = load i64, ptr %s_v506___ast_244___chali_tmp_o_1398
  %r123 = load i8, ptr %t_v506___ast_244___chali_tmp_o_1398
  call void @chali_owned_drop(i64 %r122)
  br label %ast_block_25
ast_block_24:
  br label %ast_block_25
ast_block_25:
  store i64 0, ptr %s_v506___ast_244___chali_tmp_o_1398
  store i8 0, ptr %t_v506___ast_244___chali_tmp_o_1398
  br label %ast_block_15
ast_block_14:
  br label %ast_block_15
ast_block_15:
  %r124 = load i64, ptr %s_v507___ast_244___chali_tmp_o_1401
  %r125 = load i8, ptr %t_v507___ast_244___chali_tmp_o_1401
  %r126 = icmp ne i64 %r124, 0
  %r127 = zext i1 %r126 to i64
  %r128 = icmp ne i64 %r127, 0
  br i1 %r128, label %ast_block_26, label %ast_block_27
ast_block_26:
  %r129 = load i64, ptr %s_v507___ast_244___chali_tmp_o_1401
  %r130 = load i8, ptr %t_v507___ast_244___chali_tmp_o_1401
  call void @chali_owned_drop(i64 %r129)
  br label %ast_block_28
ast_block_27:
  br label %ast_block_28
ast_block_28:
  store i64 0, ptr %s_v507___ast_244___chali_tmp_o_1401
  store i8 0, ptr %t_v507___ast_244___chali_tmp_o_1401
  %r131 = call i64 @chali_host_call(ptr @native_name0, i64 0, ptr %indirect_arguments, ptr %native_argument_types)
  %r132 = call i8 @chali_host_result_float()
  store i64 %r131, ptr %s_v501___ast_244___chali_box_value_1400
  store i8 %r132, ptr %t_v501___ast_244___chali_box_value_1400
  %r133 = load i64, ptr %s_v501___ast_244___chali_box_value_1400
  %r134 = load i8, ptr %t_v501___ast_244___chali_box_value_1400
  %r135 = load i64, ptr @s_g_0_program__first
  %r136 = load i8, ptr @t_g_0_program__first
  %r137 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r135, ptr %r137
  %r138 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r136, ptr %r138
  %r139 = call i64 @chali_host_call(ptr @native_name1, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r140 = call i8 @chali_host_result_float()
  %r141 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r133, ptr %r141
  %r142 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r134, ptr %r142
  %r143 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 %r139, ptr %r143
  %r144 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 %r140, ptr %r144
  %r145 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 124, ptr %r145
  %r146 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r146
  %r147 = getelementptr i64, ptr %indirect_arguments, i64 3
  store i64 0, ptr %r147
  %r148 = getelementptr i8, ptr %native_argument_types, i64 3
  store i8 0, ptr %r148
  %r149 = getelementptr i64, ptr %indirect_arguments, i64 4
  store i64 0, ptr %r149
  %r150 = getelementptr i8, ptr %native_argument_types, i64 4
  store i8 0, ptr %r150
  %r151 = getelementptr i64, ptr %indirect_arguments, i64 5
  store i64 184, ptr %r151
  %r152 = getelementptr i8, ptr %native_argument_types, i64 5
  store i8 0, ptr %r152
  %r153 = getelementptr i64, ptr %indirect_arguments, i64 6
  store i64 200, ptr %r153
  %r154 = getelementptr i8, ptr %native_argument_types, i64 6
  store i8 0, ptr %r154
  %r155 = call i64 @chali_host_call(ptr @native_name2, i64 7, ptr %indirect_arguments, ptr %native_argument_types)
  %r156 = call i8 @chali_host_result_float()
  %r157 = load i64, ptr %s_v501___ast_244___chali_box_value_1400
  %r158 = load i8, ptr %t_v501___ast_244___chali_box_value_1400
  store i64 %r157, ptr %s_v507___ast_244___chali_tmp_o_1401
  store i8 %r158, ptr %t_v507___ast_244___chali_tmp_o_1401
  %r159 = load i64, ptr %s_v507___ast_244___chali_tmp_o_1401
  %r160 = load i8, ptr %t_v507___ast_244___chali_tmp_o_1401
  %r161 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r159, ptr %r161
  %r162 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r160, ptr %r162
  %r163 = call i64 @chali_host_call(ptr @native_name3, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r164 = call i8 @chali_host_result_float()
  store i64 %r163, ptr %s_v504___ast_244___chali_ret_v_1402
  store i8 %r164, ptr %t_v504___ast_244___chali_ret_v_1402
  %r165 = load i64, ptr %s_v507___ast_244___chali_tmp_o_1401
  %r166 = load i8, ptr %t_v507___ast_244___chali_tmp_o_1401
  %r167 = icmp ne i64 %r165, 0
  %r168 = zext i1 %r167 to i64
  %r169 = icmp ne i64 %r168, 0
  br i1 %r169, label %ast_block_29, label %ast_block_30
ast_block_29:
  %r170 = load i64, ptr %s_v507___ast_244___chali_tmp_o_1401
  %r171 = load i8, ptr %t_v507___ast_244___chali_tmp_o_1401
  call void @chali_owned_drop(i64 %r170)
  br label %ast_block_31
ast_block_30:
  br label %ast_block_31
ast_block_31:
  store i64 0, ptr %s_v507___ast_244___chali_tmp_o_1401
  store i8 0, ptr %t_v507___ast_244___chali_tmp_o_1401
  %r172 = load i64, ptr %s_v504___ast_244___chali_ret_v_1402
  %r173 = load i8, ptr %t_v504___ast_244___chali_ret_v_1402
  store i8 %r173, ptr @chali_return_float
  ret i64 %r172
ast_block_32:
  %r174 = load i64, ptr %s_v507___ast_244___chali_tmp_o_1401
  %r175 = load i8, ptr %t_v507___ast_244___chali_tmp_o_1401
  %r176 = icmp ne i64 %r174, 0
  %r177 = zext i1 %r176 to i64
  %r178 = icmp ne i64 %r177, 0
  br i1 %r178, label %ast_block_33, label %ast_block_34
ast_block_33:
  %r179 = load i64, ptr %s_v507___ast_244___chali_tmp_o_1401
  %r180 = load i8, ptr %t_v507___ast_244___chali_tmp_o_1401
  call void @chali_owned_drop(i64 %r179)
  br label %ast_block_35
ast_block_34:
  br label %ast_block_35
ast_block_35:
  store i64 0, ptr %s_v507___ast_244___chali_tmp_o_1401
  store i8 0, ptr %t_v507___ast_244___chali_tmp_o_1401
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define internal i64 @f_v245_program__invoke(i64 %p0, i64 %p1) uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [4 x i64]
  %native_argument_types = alloca [4 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v509___ast_245___chali_parameter_reference_1403 = alloca i64
  store i64 0, ptr %s_v509___ast_245___chali_parameter_reference_1403
  %t_v509___ast_245___chali_parameter_reference_1403 = alloca i8
  store i8 0, ptr %t_v509___ast_245___chali_parameter_reference_1403
  %s_v510___ast_245___chali_ret_v_1404 = alloca i64
  store i64 0, ptr %s_v510___ast_245___chali_ret_v_1404
  %t_v510___ast_245___chali_ret_v_1404 = alloca i8
  store i8 0, ptr %t_v510___ast_245___chali_ret_v_1404
  %s_v511___ast_245_operation_594 = alloca i64
  store i64 0, ptr %s_v511___ast_245_operation_594
  %t_v511___ast_245_operation_594 = alloca i8
  store i8 0, ptr %t_v511___ast_245_operation_594
  %s_v512___ast_245_value_595 = alloca i64
  store i64 0, ptr %s_v512___ast_245_value_595
  %t_v512___ast_245_value_595 = alloca i8
  store i8 0, ptr %t_v512___ast_245_value_595
  store i64 %p0, ptr %s_v511___ast_245_operation_594
  store i64 %p1, ptr %s_v512___ast_245_value_595
  %r0 = load i64, ptr %s_v511___ast_245_operation_594
  %r1 = load i8, ptr %t_v511___ast_245_operation_594
  %r2 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r0, ptr %r2
  %r3 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r1, ptr %r3
  %r4 = call i64 @chali_host_call(ptr @native_name3, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r5 = call i8 @chali_host_result_float()
  store i64 %r4, ptr %s_v511___ast_245_operation_594
  store i8 %r5, ptr %t_v511___ast_245_operation_594
  %r6 = call i64 @chali_alloc(i64 8)
  store i64 %r6, ptr %s_v509___ast_245___chali_parameter_reference_1403
  store i8 0, ptr %t_v509___ast_245___chali_parameter_reference_1403
  %r7 = load i64, ptr %s_v512___ast_245_value_595
  %r8 = load i8, ptr %t_v512___ast_245_value_595
  %r9 = load i64, ptr %s_v509___ast_245___chali_parameter_reference_1403
  %r10 = load i8, ptr %t_v509___ast_245___chali_parameter_reference_1403
  %r11 = call ptr @chali_primitive_address(i64 %r9, i64 8)
  store i64 %r7, ptr %r11, align 1
  %r12 = load i64, ptr %s_v511___ast_245_operation_594
  %r13 = load i8, ptr %t_v511___ast_245_operation_594
  %r14 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r12, ptr %r14
  %r15 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r13, ptr %r15
  %r16 = call i64 @chali_host_call(ptr @native_name1, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r17 = call i8 @chali_host_result_float()
  %r18 = load i64, ptr %s_v509___ast_245___chali_parameter_reference_1403
  %r19 = load i8, ptr %t_v509___ast_245___chali_parameter_reference_1403
  store ptr @f_v245_program__invoke, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 31, ptr @chali_source_line
  %r20 = call ptr @chali_primitive_address(i64 %r18, i64 8)
  %r21 = load i64, ptr %r20, align 1
  store ptr @f_v245_program__invoke, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 31, ptr @chali_source_line
  %r22 = call i64 @f_v477___chali_iface_NamedOperation_apply(i64 %r16, i64 1, i64 %r21)
  %r23 = load i8, ptr @chali_return_float
  store i64 %r22, ptr %s_v510___ast_245___chali_ret_v_1404
  store i8 %r23, ptr %t_v510___ast_245___chali_ret_v_1404
  %r24 = load i64, ptr %s_v511___ast_245_operation_594
  %r25 = load i8, ptr %t_v511___ast_245_operation_594
  %r26 = icmp ne i64 %r24, 0
  %r27 = zext i1 %r26 to i64
  %r28 = icmp ne i64 %r27, 0
  br i1 %r28, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r29 = load i64, ptr %s_v511___ast_245_operation_594
  %r30 = load i8, ptr %t_v511___ast_245_operation_594
  call void @chali_owned_drop(i64 %r29)
  br label %ast_block_2
ast_block_1:
  br label %ast_block_2
ast_block_2:
  store i64 0, ptr %s_v511___ast_245_operation_594
  store i8 0, ptr %t_v511___ast_245_operation_594
  %r31 = load i64, ptr %s_v509___ast_245___chali_parameter_reference_1403
  %r32 = load i8, ptr %t_v509___ast_245___chali_parameter_reference_1403
  %r33 = icmp ne i64 %r31, 0
  %r34 = zext i1 %r33 to i64
  %r35 = icmp ne i64 %r34, 0
  br i1 %r35, label %ast_block_3, label %ast_block_4
ast_block_3:
  %r36 = load i64, ptr %s_v509___ast_245___chali_parameter_reference_1403
  %r37 = load i8, ptr %t_v509___ast_245___chali_parameter_reference_1403
  call void @chali_owned_drop(i64 %r36)
  br label %ast_block_5
ast_block_4:
  br label %ast_block_5
ast_block_5:
  store i64 0, ptr %s_v509___ast_245___chali_parameter_reference_1403
  store i8 0, ptr %t_v509___ast_245___chali_parameter_reference_1403
  %r38 = load i64, ptr %s_v510___ast_245___chali_ret_v_1404
  %r39 = load i8, ptr %t_v510___ast_245___chali_ret_v_1404
  store i8 %r39, ptr @chali_return_float
  ret i64 %r38
ast_block_6:
  %r40 = load i64, ptr %s_v511___ast_245_operation_594
  %r41 = load i8, ptr %t_v511___ast_245_operation_594
  %r42 = icmp ne i64 %r40, 0
  %r43 = zext i1 %r42 to i64
  %r44 = icmp ne i64 %r43, 0
  br i1 %r44, label %ast_block_7, label %ast_block_8
ast_block_7:
  %r45 = load i64, ptr %s_v511___ast_245_operation_594
  %r46 = load i8, ptr %t_v511___ast_245_operation_594
  call void @chali_owned_drop(i64 %r45)
  br label %ast_block_9
ast_block_8:
  br label %ast_block_9
ast_block_9:
  store i64 0, ptr %s_v511___ast_245_operation_594
  store i8 0, ptr %t_v511___ast_245_operation_594
  %r47 = load i64, ptr %s_v509___ast_245___chali_parameter_reference_1403
  %r48 = load i8, ptr %t_v509___ast_245___chali_parameter_reference_1403
  %r49 = icmp ne i64 %r47, 0
  %r50 = zext i1 %r49 to i64
  %r51 = icmp ne i64 %r50, 0
  br i1 %r51, label %ast_block_10, label %ast_block_11
ast_block_10:
  %r52 = load i64, ptr %s_v509___ast_245___chali_parameter_reference_1403
  %r53 = load i8, ptr %t_v509___ast_245___chali_parameter_reference_1403
  call void @chali_owned_drop(i64 %r52)
  br label %ast_block_12
ast_block_11:
  br label %ast_block_12
ast_block_12:
  store i64 0, ptr %s_v509___ast_245___chali_parameter_reference_1403
  store i8 0, ptr %t_v509___ast_245___chali_parameter_reference_1403
  store i8 0, ptr @chali_return_float
  ret i64 0
ast_label_v513___chali_cleanup_program__invoke:
  %r54 = load i64, ptr %s_v511___ast_245_operation_594
  %r55 = load i8, ptr %t_v511___ast_245_operation_594
  %r56 = icmp ne i64 %r54, 0
  %r57 = zext i1 %r56 to i64
  %r58 = icmp ne i64 %r57, 0
  br i1 %r58, label %ast_block_13, label %ast_block_14
ast_block_13:
  %r59 = load i64, ptr %s_v511___ast_245_operation_594
  %r60 = load i8, ptr %t_v511___ast_245_operation_594
  call void @chali_owned_drop(i64 %r59)
  br label %ast_block_15
ast_block_14:
  br label %ast_block_15
ast_block_15:
  store i64 0, ptr %s_v511___ast_245_operation_594
  store i8 0, ptr %t_v511___ast_245_operation_594
  %r61 = load i64, ptr %s_v509___ast_245___chali_parameter_reference_1403
  %r62 = load i8, ptr %t_v509___ast_245___chali_parameter_reference_1403
  %r63 = icmp ne i64 %r61, 0
  %r64 = zext i1 %r63 to i64
  %r65 = icmp ne i64 %r64, 0
  br i1 %r65, label %ast_block_16, label %ast_block_17
ast_block_16:
  %r66 = load i64, ptr %s_v509___ast_245___chali_parameter_reference_1403
  %r67 = load i8, ptr %t_v509___ast_245___chali_parameter_reference_1403
  call void @chali_owned_drop(i64 %r66)
  br label %ast_block_18
ast_block_17:
  br label %ast_block_18
ast_block_18:
  store i64 0, ptr %s_v509___ast_245___chali_parameter_reference_1403
  store i8 0, ptr %t_v509___ast_245___chali_parameter_reference_1403
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
  %s_v514___ast_246___chali_ret_v_1406 = alloca i64
  store i64 0, ptr %s_v514___ast_246___chali_ret_v_1406
  %t_v514___ast_246___chali_ret_v_1406 = alloca i8
  store i8 0, ptr %t_v514___ast_246___chali_ret_v_1406
  %s_v515___ast_246_acc = alloca i64
  store i64 0, ptr %s_v515___ast_246_acc
  %t_v515___ast_246_acc = alloca i8
  store i8 0, ptr %t_v515___ast_246_acc
  %s_v516___ast_246_derived = alloca i64
  store i64 0, ptr %s_v516___ast_246_derived
  %t_v516___ast_246_derived = alloca i8
  store i8 0, ptr %t_v516___ast_246_derived
  %s_v517___ast_246_i_1405 = alloca i64
  store i64 0, ptr %s_v517___ast_246_i_1405
  %t_v517___ast_246_i_1405 = alloca i8
  store i8 0, ptr %t_v517___ast_246_i_1405
  %s_v518___ast_246_seed = alloca i64
  store i64 0, ptr %s_v518___ast_246_seed
  %t_v518___ast_246_seed = alloca i8
  store i8 0, ptr %t_v518___ast_246_seed
  store i64 %p0, ptr %s_v518___ast_246_seed
  %r0 = load i64, ptr %s_v518___ast_246_seed
  %r1 = load i8, ptr %t_v518___ast_246_seed
  store i64 %r0, ptr %s_v515___ast_246_acc
  store i8 %r1, ptr %t_v515___ast_246_acc
  store i64 0, ptr %s_v517___ast_246_i_1405
  store i8 0, ptr %t_v517___ast_246_i_1405
  br label %ast_label_v519___chali_loop_139
ast_label_v519___chali_loop_139:
  %r2 = load i64, ptr %s_v517___ast_246_i_1405
  %r3 = load i8, ptr %t_v517___ast_246_i_1405
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 35, ptr @chali_source_line
  %r4 = icmp slt i64 %r2, 262144
  %r5 = zext i1 %r4 to i64
  %r6 = icmp ne i64 %r5, 0
  br i1 %r6, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r7 = load i64, ptr %s_v517___ast_246_i_1405
  %r8 = load i8, ptr %t_v517___ast_246_i_1405
  %r9 = load i64, ptr %s_v518___ast_246_seed
  %r10 = load i8, ptr %t_v518___ast_246_seed
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 36, ptr @chali_source_line
  %r11 = add i64 %r7, %r9
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 36, ptr @chali_source_line
  %r12 = and i64 %r11, 3
  %r13 = call i64 @f_v244_program__choose(i64 %r12)
  %r14 = load i8, ptr @chali_return_float
  %r15 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r13, ptr %r15
  %r16 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r14, ptr %r16
  %r17 = call i64 @chali_host_call(ptr @native_name1, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r18 = call i8 @chali_host_result_float()
  store i64 %r17, ptr %s_v516___ast_246_derived
  store i8 %r18, ptr %t_v516___ast_246_derived
  %r19 = load i64, ptr %s_v516___ast_246_derived
  %r20 = load i8, ptr %t_v516___ast_246_derived
  %r21 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r19, ptr %r21
  %r22 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r20, ptr %r22
  %r23 = call i64 @chali_host_call(ptr @native_name1, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r24 = call i8 @chali_host_result_float()
  %r25 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r23, ptr %r25
  %r26 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r24, ptr %r26
  %r27 = call i64 @chali_host_call(ptr @native_name1, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r28 = call i8 @chali_host_result_float()
  %r29 = load i64, ptr %s_v515___ast_246_acc
  %r30 = load i8, ptr %t_v515___ast_246_acc
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 37, ptr @chali_source_line
  %r31 = call i64 @f_v245_program__invoke(i64 %r27, i64 %r29)
  %r32 = load i8, ptr @chali_return_float
  store i64 %r31, ptr %s_v515___ast_246_acc
  store i8 %r32, ptr %t_v515___ast_246_acc
  %r33 = load i64, ptr %s_v517___ast_246_i_1405
  %r34 = load i8, ptr %t_v517___ast_246_i_1405
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 38, ptr @chali_source_line
  %r35 = add i64 %r33, 1
  store i64 %r35, ptr %s_v517___ast_246_i_1405
  store i8 0, ptr %t_v517___ast_246_i_1405
  %r36 = load i64, ptr %s_v516___ast_246_derived
  %r37 = load i8, ptr %t_v516___ast_246_derived
  %r38 = icmp ne i64 %r36, 0
  %r39 = zext i1 %r38 to i64
  %r40 = icmp ne i64 %r39, 0
  br i1 %r40, label %ast_block_3, label %ast_block_4
ast_block_3:
  %r41 = load i64, ptr %s_v516___ast_246_derived
  %r42 = load i8, ptr %t_v516___ast_246_derived
  call void @chali_owned_drop(i64 %r41)
  br label %ast_block_5
ast_block_4:
  br label %ast_block_5
ast_block_5:
  store i64 0, ptr %s_v516___ast_246_derived
  store i8 0, ptr %t_v516___ast_246_derived
  br label %ast_block_2
ast_block_1:
  br label %ast_label_v520___chali_brk_140
ast_block_2:
  br label %ast_label_v519___chali_loop_139
ast_label_v520___chali_brk_140:
  %r43 = load i64, ptr %s_v515___ast_246_acc
  %r44 = load i8, ptr %t_v515___ast_246_acc
  store i64 %r43, ptr %s_v514___ast_246___chali_ret_v_1406
  store i8 %r44, ptr %t_v514___ast_246___chali_ret_v_1406
  %r45 = load i64, ptr %s_v514___ast_246___chali_ret_v_1406
  %r46 = load i8, ptr %t_v514___ast_246___chali_ret_v_1406
  store i8 %r46, ptr @chali_return_float
  ret i64 %r45
}
define internal i64 @f___chali_library_init() uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [3 x i64]
  %native_argument_types = alloca [3 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  store i64 32, ptr @s_g_0_program__first
  store i8 0, ptr @t_g_0_program__first
  %r0 = load i64, ptr @s_g_0_program__first
  %r1 = load i8, ptr @t_g_0_program__first
  %r2 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r0, ptr %r2
  %r3 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r1, ptr %r3
  %r4 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 16, ptr %r4
  %r5 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r5
  %r6 = call i64 @chali_host_call(ptr @native_name4, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r7 = call i8 @chali_host_result_float()
  %r8 = load i64, ptr @s_g_0_program__first
  %r9 = load i8, ptr @t_g_0_program__first
  %r10 = mul i64 0, 8
  %r11 = add i64 %r8, %r10
  %r12 = call ptr @chali_primitive_address(i64 %r11, i64 8)
  store i64 0, ptr %r12, align 1
  %r13 = load i64, ptr @s_g_0_program__first
  %r14 = load i8, ptr @t_g_0_program__first
  %r15 = mul i64 1, 8
  %r16 = add i64 %r13, %r15
  %r17 = call ptr @chali_primitive_address(i64 %r16, i64 8)
  store i64 0, ptr %r17, align 1
  store i64 48, ptr @s_g_1_program__second
  store i8 0, ptr @t_g_1_program__second
  %r18 = load i64, ptr @s_g_1_program__second
  %r19 = load i8, ptr @t_g_1_program__second
  %r20 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r18, ptr %r20
  %r21 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r19, ptr %r21
  %r22 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 16, ptr %r22
  %r23 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r23
  %r24 = call i64 @chali_host_call(ptr @native_name4, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r25 = call i8 @chali_host_result_float()
  %r26 = load i64, ptr @s_g_1_program__second
  %r27 = load i8, ptr @t_g_1_program__second
  %r28 = mul i64 0, 8
  %r29 = add i64 %r26, %r28
  %r30 = call ptr @chali_primitive_address(i64 %r29, i64 8)
  store i64 0, ptr %r30, align 1
  %r31 = load i64, ptr @s_g_1_program__second
  %r32 = load i8, ptr @t_g_1_program__second
  %r33 = mul i64 1, 8
  %r34 = add i64 %r31, %r33
  %r35 = call ptr @chali_primitive_address(i64 %r34, i64 8)
  store i64 0, ptr %r35, align 1
  store i64 64, ptr @s_g_2_program__third
  store i8 0, ptr @t_g_2_program__third
  %r36 = load i64, ptr @s_g_2_program__third
  %r37 = load i8, ptr @t_g_2_program__third
  %r38 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r36, ptr %r38
  %r39 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r37, ptr %r39
  %r40 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 16, ptr %r40
  %r41 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r41
  %r42 = call i64 @chali_host_call(ptr @native_name4, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r43 = call i8 @chali_host_result_float()
  %r44 = load i64, ptr @s_g_2_program__third
  %r45 = load i8, ptr @t_g_2_program__third
  %r46 = mul i64 0, 8
  %r47 = add i64 %r44, %r46
  %r48 = call ptr @chali_primitive_address(i64 %r47, i64 8)
  store i64 0, ptr %r48, align 1
  %r49 = load i64, ptr @s_g_2_program__third
  %r50 = load i8, ptr @t_g_2_program__third
  %r51 = mul i64 1, 8
  %r52 = add i64 %r49, %r51
  %r53 = call ptr @chali_primitive_address(i64 %r52, i64 8)
  store i64 0, ptr %r53, align 1
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define internal i64 @f___chali_library_finish() alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [1 x i64]
  %native_argument_types = alloca [1 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define internal i64 @f_v477___chali_iface_NamedOperation_apply(i64 %p0, i64 %p1, i64 %p2) uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [3 x i64]
  %native_argument_types = alloca [3 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v521___ast_477___chali_a0 = alloca i64
  store i64 0, ptr %s_v521___ast_477___chali_a0
  %t_v521___ast_477___chali_a0 = alloca i8
  store i8 0, ptr %t_v521___ast_477___chali_a0
  %s_v522___ast_477___chali_readonly = alloca i64
  store i64 0, ptr %s_v522___ast_477___chali_readonly
  %t_v522___ast_477___chali_readonly = alloca i8
  store i8 0, ptr %t_v522___ast_477___chali_readonly
  %s_v523___ast_477___chali_self = alloca i64
  store i64 0, ptr %s_v523___ast_477___chali_self
  %t_v523___ast_477___chali_self = alloca i8
  store i8 0, ptr %t_v523___ast_477___chali_self
  %s_v524___ast_477___chali_t = alloca i64
  store i64 0, ptr %s_v524___ast_477___chali_t
  %t_v524___ast_477___chali_t = alloca i8
  store i8 0, ptr %t_v524___ast_477___chali_t
  store i64 %p0, ptr %s_v523___ast_477___chali_self
  store i64 %p1, ptr %s_v522___ast_477___chali_readonly
  store i64 %p2, ptr %s_v521___ast_477___chali_a0
  %r0 = load i64, ptr %s_v523___ast_477___chali_self
  %r1 = load i8, ptr %t_v523___ast_477___chali_self
  %r2 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r0, ptr %r2
  %r3 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r1, ptr %r3
  %r4 = call i64 @chali_host_call(ptr @native_name5, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r5 = call i8 @chali_host_result_float()
  store i64 %r4, ptr %s_v524___ast_477___chali_t
  store i8 %r5, ptr %t_v524___ast_477___chali_t
  %r6 = load i64, ptr %s_v524___ast_477___chali_t
  %r7 = load i8, ptr %t_v524___ast_477___chali_t
  %r8 = icmp eq i64 %r6, 128
  %r9 = zext i1 %r8 to i64
  %r10 = icmp ne i64 %r9, 0
  br i1 %r10, label %ast_block_0, label %ast_block_1
ast_block_0:
  br label %ast_block_2
ast_block_1:
  %r11 = load i64, ptr %s_v524___ast_477___chali_t
  %r12 = load i8, ptr %t_v524___ast_477___chali_t
  %r13 = icmp eq i64 %r11, 127
  %r14 = zext i1 %r13 to i64
  %r15 = icmp ne i64 %r14, 0
  br label %ast_block_3
ast_block_3:
  br label %ast_block_2
ast_block_2:
  %r16 = phi i1 [ true, %ast_block_0 ], [ %r15, %ast_block_3 ]
  %r17 = zext i1 %r16 to i64
  %r18 = icmp ne i64 %r17, 0
  br i1 %r18, label %ast_block_4, label %ast_block_5
ast_block_4:
  %r19 = load i64, ptr %s_v523___ast_477___chali_self
  %r20 = load i8, ptr %t_v523___ast_477___chali_self
  %r21 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r19, ptr %r21
  %r22 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r20, ptr %r22
  %r23 = call i64 @chali_host_call(ptr @native_name6, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r24 = call i8 @chali_host_result_float()
  %r25 = load i64, ptr %s_v521___ast_477___chali_a0
  %r26 = load i8, ptr %t_v521___ast_477___chali_a0
  %r27 = call i64 @f_v240_ExclusiveOr_apply(i64 %r23, i64 %r25)
  %r28 = load i8, ptr @chali_return_float
  store i8 %r28, ptr @chali_return_float
  ret i64 %r27
ast_block_5:
  br label %ast_block_6
ast_block_6:
  %r29 = load i64, ptr %s_v524___ast_477___chali_t
  %r30 = load i8, ptr %t_v524___ast_477___chali_t
  %r31 = icmp eq i64 %r29, 124
  %r32 = zext i1 %r31 to i64
  %r33 = icmp ne i64 %r32, 0
  br i1 %r33, label %ast_block_7, label %ast_block_8
ast_block_7:
  br label %ast_block_9
ast_block_8:
  %r34 = load i64, ptr %s_v524___ast_477___chali_t
  %r35 = load i8, ptr %t_v524___ast_477___chali_t
  %r36 = icmp eq i64 %r34, 123
  %r37 = zext i1 %r36 to i64
  %r38 = icmp ne i64 %r37, 0
  br label %ast_block_10
ast_block_10:
  br label %ast_block_9
ast_block_9:
  %r39 = phi i1 [ true, %ast_block_7 ], [ %r38, %ast_block_10 ]
  %r40 = zext i1 %r39 to i64
  %r41 = icmp ne i64 %r40, 0
  br i1 %r41, label %ast_block_11, label %ast_block_12
ast_block_11:
  %r42 = load i64, ptr %s_v523___ast_477___chali_self
  %r43 = load i8, ptr %t_v523___ast_477___chali_self
  %r44 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r42, ptr %r44
  %r45 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r43, ptr %r45
  %r46 = call i64 @chali_host_call(ptr @native_name6, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r47 = call i8 @chali_host_result_float()
  %r48 = load i64, ptr %s_v521___ast_477___chali_a0
  %r49 = load i8, ptr %t_v521___ast_477___chali_a0
  %r50 = call i64 @f_v234_Multiply_apply(i64 %r46, i64 %r48)
  %r51 = load i8, ptr @chali_return_float
  store i8 %r51, ptr @chali_return_float
  ret i64 %r50
ast_block_12:
  br label %ast_block_13
ast_block_13:
  %r52 = load i64, ptr %s_v524___ast_477___chali_t
  %r53 = load i8, ptr %t_v524___ast_477___chali_t
  %r54 = icmp eq i64 %r52, 126
  %r55 = zext i1 %r54 to i64
  %r56 = icmp ne i64 %r55, 0
  br i1 %r56, label %ast_block_14, label %ast_block_15
ast_block_14:
  br label %ast_block_16
ast_block_15:
  %r57 = load i64, ptr %s_v524___ast_477___chali_t
  %r58 = load i8, ptr %t_v524___ast_477___chali_t
  %r59 = icmp eq i64 %r57, 125
  %r60 = zext i1 %r59 to i64
  %r61 = icmp ne i64 %r60, 0
  br label %ast_block_17
ast_block_17:
  br label %ast_block_16
ast_block_16:
  %r62 = phi i1 [ true, %ast_block_14 ], [ %r61, %ast_block_17 ]
  %r63 = zext i1 %r62 to i64
  %r64 = icmp ne i64 %r63, 0
  br i1 %r64, label %ast_block_18, label %ast_block_19
ast_block_18:
  %r65 = load i64, ptr %s_v523___ast_477___chali_self
  %r66 = load i8, ptr %t_v523___ast_477___chali_self
  %r67 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r65, ptr %r67
  %r68 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r66, ptr %r68
  %r69 = call i64 @chali_host_call(ptr @native_name6, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r70 = call i8 @chali_host_result_float()
  %r71 = load i64, ptr %s_v521___ast_477___chali_a0
  %r72 = load i8, ptr %t_v521___ast_477___chali_a0
  %r73 = call i64 @f_v237_Subtract_apply(i64 %r69, i64 %r71)
  %r74 = load i8, ptr @chali_return_float
  store i8 %r74, ptr @chali_return_float
  ret i64 %r73
ast_block_19:
  br label %ast_block_20
ast_block_20:
  %r75 = load i64, ptr %s_v524___ast_477___chali_t
  %r76 = load i8, ptr %t_v524___ast_477___chali_t
  %r77 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r75, ptr %r77
  %r78 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r76, ptr %r78
  %r79 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 8, ptr %r79
  %r80 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r80
  %r81 = call i64 @chali_host_call(ptr @native_name7, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r82 = call i8 @chali_host_result_float()
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define i64 @chali_invoke(i64 %function, i64 %count, ptr %arguments) {
entry:
  %target = call i64 @chali_function_target(i64 %function)
  %environment = call i64 @chali_function_environment(i64 %function)
  br label %candidate0
candidate0:
  call void @chali_abort(i64 0)
  unreachable
}
define i64 @chali_function_returns_real(i64 %function) {
entry:
  %target = call i64 @chali_function_target(i64 %function)
  br label %candidate0
candidate0:
  call void @chali_abort(i64 0)
  unreachable
}
define i64 @chali_export_dispatch(i64 %index, ptr %arguments) {
entry:
  switch i64 %index, label %bad [ i64 0, label %case0 i64 1, label %case1 ]
case0:
  %result0 = call i64 @__chali_impl_program__setup()
  ret i64 %result0
case1:
  %v1_0_p = getelementptr i64, ptr %arguments, i64 0
  %v1_0 = load i64, ptr %v1_0_p
  %result1 = call i64 @__chali_impl_program__work(i64 %v1_0)
  ret i64 %result1
bad:
  call void @chali_abort(i64 0)
  unreachable
}
define dllexport void @chali_init() {
entry:
  call void @chali_runtime_init()
  %initialized = call i64 @f___chali_library_init()
  ret void
}
define dllexport void @chali_finish() {
entry:
  %finished = call i64 @f___chali_library_finish()
  call void @chali_runtime_finish()
  ret void
}
