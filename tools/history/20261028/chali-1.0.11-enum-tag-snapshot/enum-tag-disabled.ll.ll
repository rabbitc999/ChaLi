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
@.str.28 = private unnamed_addr constant [59 x i8] c"\E5\BC\95\E7\94\A8\E5\B0\9A\E6\9C\AA\E7\BB\91\E5\AE\9A\E6\9C\89\E6\95\88\E5\85\83\E7\B4\A0\EF\BC\9B\E7\BC\BA\E7\9C\81\E5\80\BC\E8\AF\B7\E4\BD\BF\E7\94\A8 Option\00", align 1
@.str.29 = private unnamed_addr constant [6 x i8] c"panic\00", align 1

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
@chali_data_layout_size = constant i64 84
; AOT data image: original=12504 stored=12504 rows=5/5 segments=1
@chali_data_layout = constant [85 x i8] c"\43\48\4C\59\01\05\0A\61\73\74\5F\64\61\74\61\5F\30\08\03\30\08\0A\61\73\74\5F\64\61\74\61\5F\31\38\03\30\08\0A\61\73\74\5F\64\61\74\61\5F\32\68\03\30\08\0A\61\73\74\5F\64\61\74\61\5F\33\98\01\03\30\08\0A\61\73\74\5F\64\61\74\61\5F\34\D0\01\03\80\60\08\00"
@chali_data_image = constant [12504 x i8] c"\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\02\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\03\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00"
@chali_data_segment_count = constant i64 1
@chali_data_segments = constant [1 x { i64, i64, i64 }] [{ i64, i64, i64 } { i64 0, i64 0, i64 12504 }]
@chali_data_size = constant i64 12504
@chali_heap_size = constant i64 536870912
@chali_allocator_kind = constant i32 1
@source_file0 = private constant [37 x i8] c"\62\65\6E\63\68\6D\61\72\6B\73\2F\66\65\61\74\75\72\65\73\2F\65\6E\75\6D\5F\70\61\79\6C\6F\61\64\2E\63\68\6C\00"
define void @chali_code_anchor() uwtable section "chali_code" {
entry:
  ret void
}
@trace_anchor_name = private constant [1 x i8] zeroinitializer
@trace_name0 = private constant [25 x i8] c"\5F\5F\63\68\61\6C\69\5F\65\6E\75\6D\5F\45\76\65\6E\74\5F\45\6D\70\74\79\00"
@trace_name1 = private constant [23 x i8] c"\5F\5F\63\68\61\6C\69\5F\65\6E\75\6D\5F\45\76\65\6E\74\5F\4F\6E\65\00"
@trace_name2 = private constant [24 x i8] c"\5F\5F\63\68\61\6C\69\5F\65\6E\75\6D\5F\45\76\65\6E\74\5F\50\61\69\72\00"
@trace_name3 = private constant [26 x i8] c"\5F\5F\63\68\61\6C\69\5F\65\6E\75\6D\5F\45\76\65\6E\74\5F\52\65\63\6F\72\64\00"
@trace_name4 = private constant [15 x i8] c"\70\72\6F\67\72\61\6D\3A\3A\73\65\74\75\70\00"
@trace_name5 = private constant [14 x i8] c"\70\72\6F\67\72\61\6D\3A\3A\77\6F\72\6B\00"
@trace_name6 = private constant [21 x i8] c"\5F\5F\63\68\61\6C\69\5F\6C\69\62\72\61\72\79\5F\69\6E\69\74\00"
@trace_name7 = private constant [23 x i8] c"\5F\5F\63\68\61\6C\69\5F\6C\69\62\72\61\72\79\5F\66\69\6E\69\73\68\00"
@trace_name8 = private constant [20 x i8] c"\5F\5F\63\68\61\6C\69\5F\63\6F\70\79\5F\73\74\72\75\63\74\00"
@chali_trace_count = constant i64 10
@chali_trace_functions = constant [10 x ptr] [ptr @chali_code_anchor, ptr @f_v0___chali_enum_Event_Empty, ptr @f_v1___chali_enum_Event_One, ptr @f_v2___chali_enum_Event_Pair, ptr @f_v3___chali_enum_Event_Record, ptr @__chali_impl_program__setup, ptr @__chali_impl_program__work, ptr @f___chali_library_init, ptr @f___chali_library_finish, ptr @f_v474___chali_copy_struct]
@chali_trace_names = constant [10 x ptr] [ptr @trace_anchor_name, ptr @trace_name0, ptr @trace_name1, ptr @trace_name2, ptr @trace_name3, ptr @trace_name4, ptr @trace_name5, ptr @trace_name6, ptr @trace_name7, ptr @trace_name8]
@native_name0 = private constant [17 x i8] c"\5F\5F\73\74\64\5F\6D\6F\76\65\5F\62\65\67\69\6E\00"
@native_name1 = private constant [15 x i8] c"\5F\5F\73\74\64\5F\6D\6F\76\65\5F\65\6E\64\00"
@s_g_0_program__events = internal thread_local global i64 0
@t_g_0_program__events = internal thread_local global i8 0
define void @chali_reset_slots() {
entry:
  store i8 0, ptr @chali_return_float
  store ptr null, ptr @chali_source_function
  store ptr null, ptr @chali_source_file
  store i64 0, ptr @chali_source_line
  store i64 0, ptr @s_g_0_program__events
  store i8 0, ptr @t_g_0_program__events
  ret void
}
; AOT standard modules:
define i1 @chali_program_std_bind(ptr %registry, ptr %error) {
entry:
  ret i1 true
}
define internal i64 @f_v0___chali_enum_Event_Empty() alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [1 x i64]
  %native_argument_types = alloca [1 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v477___ast_0___chali_enum_value = alloca i64
  store i64 0, ptr %s_v477___ast_0___chali_enum_value
  %t_v477___ast_0___chali_enum_value = alloca i8
  store i8 0, ptr %t_v477___ast_0___chali_enum_value
  store i64 8, ptr %s_v477___ast_0___chali_enum_value
  store i8 0, ptr %t_v477___ast_0___chali_enum_value
  %r0 = load i64, ptr %s_v477___ast_0___chali_enum_value
  %r1 = load i8, ptr %t_v477___ast_0___chali_enum_value
  store i8 %r1, ptr @chali_return_float
  ret i64 %r0
}
define internal i64 @f_v1___chali_enum_Event_One(i64 %p0) alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [1 x i64]
  %native_argument_types = alloca [1 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v478___ast_1___chali_enum_arg_Event_One_0 = alloca i64
  store i64 0, ptr %s_v478___ast_1___chali_enum_arg_Event_One_0
  %t_v478___ast_1___chali_enum_arg_Event_One_0 = alloca i8
  store i8 0, ptr %t_v478___ast_1___chali_enum_arg_Event_One_0
  %s_v479___ast_1___chali_enum_value = alloca i64
  store i64 0, ptr %s_v479___ast_1___chali_enum_value
  %t_v479___ast_1___chali_enum_value = alloca i8
  store i8 0, ptr %t_v479___ast_1___chali_enum_value
  store i64 %p0, ptr %s_v478___ast_1___chali_enum_arg_Event_One_0
  store i64 56, ptr %s_v479___ast_1___chali_enum_value
  store i8 0, ptr %t_v479___ast_1___chali_enum_value
  %r0 = load i64, ptr %s_v478___ast_1___chali_enum_arg_Event_One_0
  %r1 = load i8, ptr %t_v478___ast_1___chali_enum_arg_Event_One_0
  %r2 = load i64, ptr %s_v479___ast_1___chali_enum_value
  %r3 = load i8, ptr %t_v479___ast_1___chali_enum_value
  %r4 = mul i64 1, 8
  %r5 = add i64 %r2, %r4
  %r6 = call ptr @chali_primitive_address(i64 %r5, i64 8)
  store i64 %r0, ptr %r6, align 1
  %r7 = load i64, ptr %s_v479___ast_1___chali_enum_value
  %r8 = load i8, ptr %t_v479___ast_1___chali_enum_value
  store i8 %r8, ptr @chali_return_float
  ret i64 %r7
}
define internal i64 @f_v2___chali_enum_Event_Pair(i64 %p0, i64 %p1) alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [1 x i64]
  %native_argument_types = alloca [1 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v480___ast_2___chali_enum_arg_Event_Pair_0 = alloca i64
  store i64 0, ptr %s_v480___ast_2___chali_enum_arg_Event_Pair_0
  %t_v480___ast_2___chali_enum_arg_Event_Pair_0 = alloca i8
  store i8 0, ptr %t_v480___ast_2___chali_enum_arg_Event_Pair_0
  %s_v481___ast_2___chali_enum_arg_Event_Pair_1 = alloca i64
  store i64 0, ptr %s_v481___ast_2___chali_enum_arg_Event_Pair_1
  %t_v481___ast_2___chali_enum_arg_Event_Pair_1 = alloca i8
  store i8 0, ptr %t_v481___ast_2___chali_enum_arg_Event_Pair_1
  %s_v482___ast_2___chali_enum_value = alloca i64
  store i64 0, ptr %s_v482___ast_2___chali_enum_value
  %t_v482___ast_2___chali_enum_value = alloca i8
  store i8 0, ptr %t_v482___ast_2___chali_enum_value
  store i64 %p0, ptr %s_v480___ast_2___chali_enum_arg_Event_Pair_0
  store i64 %p1, ptr %s_v481___ast_2___chali_enum_arg_Event_Pair_1
  store i64 104, ptr %s_v482___ast_2___chali_enum_value
  store i8 0, ptr %t_v482___ast_2___chali_enum_value
  %r0 = load i64, ptr %s_v480___ast_2___chali_enum_arg_Event_Pair_0
  %r1 = load i8, ptr %t_v480___ast_2___chali_enum_arg_Event_Pair_0
  %r2 = load i64, ptr %s_v482___ast_2___chali_enum_value
  %r3 = load i8, ptr %t_v482___ast_2___chali_enum_value
  %r4 = mul i64 2, 8
  %r5 = add i64 %r2, %r4
  %r6 = call ptr @chali_primitive_address(i64 %r5, i64 8)
  store i64 %r0, ptr %r6, align 1
  %r7 = load i64, ptr %s_v481___ast_2___chali_enum_arg_Event_Pair_1
  %r8 = load i8, ptr %t_v481___ast_2___chali_enum_arg_Event_Pair_1
  %r9 = load i64, ptr %s_v482___ast_2___chali_enum_value
  %r10 = load i8, ptr %t_v482___ast_2___chali_enum_value
  %r11 = mul i64 3, 8
  %r12 = add i64 %r9, %r11
  %r13 = call ptr @chali_primitive_address(i64 %r12, i64 8)
  store i64 %r7, ptr %r13, align 1
  %r14 = load i64, ptr %s_v482___ast_2___chali_enum_value
  %r15 = load i8, ptr %t_v482___ast_2___chali_enum_value
  store i8 %r15, ptr @chali_return_float
  ret i64 %r14
}
define internal i64 @f_v3___chali_enum_Event_Record(i64 %p0) alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [1 x i64]
  %native_argument_types = alloca [1 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v483___ast_3___chali_enum_arg_Event_Record_0 = alloca i64
  store i64 0, ptr %s_v483___ast_3___chali_enum_arg_Event_Record_0
  %t_v483___ast_3___chali_enum_arg_Event_Record_0 = alloca i8
  store i8 0, ptr %t_v483___ast_3___chali_enum_arg_Event_Record_0
  %s_v484___ast_3___chali_enum_value = alloca i64
  store i64 0, ptr %s_v484___ast_3___chali_enum_value
  %t_v484___ast_3___chali_enum_value = alloca i8
  store i8 0, ptr %t_v484___ast_3___chali_enum_value
  store i64 %p0, ptr %s_v483___ast_3___chali_enum_arg_Event_Record_0
  store i64 152, ptr %s_v484___ast_3___chali_enum_value
  store i8 0, ptr %t_v484___ast_3___chali_enum_value
  %r0 = load i64, ptr %s_v483___ast_3___chali_enum_arg_Event_Record_0
  %r1 = load i8, ptr %t_v483___ast_3___chali_enum_arg_Event_Record_0
  %r2 = mul i64 0, 8
  %r3 = add i64 %r0, %r2
  %r4 = call ptr @chali_primitive_address(i64 %r3, i64 8)
  %r5 = load i64, ptr %r4, align 1
  %r6 = load i64, ptr %s_v484___ast_3___chali_enum_value
  %r7 = load i8, ptr %t_v484___ast_3___chali_enum_value
  %r8 = mul i64 4, 8
  %r9 = add i64 %r6, %r8
  %r10 = call ptr @chali_primitive_address(i64 %r9, i64 8)
  store i64 %r5, ptr %r10, align 1
  %r11 = load i64, ptr %s_v483___ast_3___chali_enum_arg_Event_Record_0
  %r12 = load i8, ptr %t_v483___ast_3___chali_enum_arg_Event_Record_0
  %r13 = mul i64 1, 8
  %r14 = add i64 %r11, %r13
  %r15 = call ptr @chali_primitive_address(i64 %r14, i64 8)
  %r16 = load i64, ptr %r15, align 1
  %r17 = load i64, ptr %s_v484___ast_3___chali_enum_value
  %r18 = load i8, ptr %t_v484___ast_3___chali_enum_value
  %r19 = mul i64 5, 8
  %r20 = add i64 %r17, %r19
  %r21 = call ptr @chali_primitive_address(i64 %r20, i64 8)
  store i64 %r16, ptr %r21, align 1
  %r22 = load i64, ptr %s_v484___ast_3___chali_enum_value
  %r23 = load i8, ptr %t_v484___ast_3___chali_enum_value
  store i8 %r23, ptr @chali_return_float
  ret i64 %r22
}
define i64 @__chali_impl_program__setup() uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [4 x i64]
  %native_argument_types = alloca [4 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v485___ast_239___chali_borrowed_target = alloca i64
  store i64 0, ptr %s_v485___ast_239___chali_borrowed_target
  %t_v485___ast_239___chali_borrowed_target = alloca i8
  store i8 0, ptr %t_v485___ast_239___chali_borrowed_target
  %s_v486___ast_239___chali_borrowed_target_1312 = alloca i64
  store i64 0, ptr %s_v486___ast_239___chali_borrowed_target_1312
  %t_v486___ast_239___chali_borrowed_target_1312 = alloca i8
  store i8 0, ptr %t_v486___ast_239___chali_borrowed_target_1312
  %s_v487___ast_239___chali_borrowed_target_1316 = alloca i64
  store i64 0, ptr %s_v487___ast_239___chali_borrowed_target_1316
  %t_v487___ast_239___chali_borrowed_target_1316 = alloca i8
  store i8 0, ptr %t_v487___ast_239___chali_borrowed_target_1316
  %s_v488___ast_239___chali_borrowed_target_1320 = alloca i64
  store i64 0, ptr %s_v488___ast_239___chali_borrowed_target_1320
  %t_v488___ast_239___chali_borrowed_target_1320 = alloca i8
  store i8 0, ptr %t_v488___ast_239___chali_borrowed_target_1320
  %s_v489___ast_239___chali_borrowed_value = alloca i64
  store i64 0, ptr %s_v489___ast_239___chali_borrowed_value
  %t_v489___ast_239___chali_borrowed_value = alloca i8
  store i8 0, ptr %t_v489___ast_239___chali_borrowed_value
  %s_v490___ast_239___chali_borrowed_value_1311 = alloca i64
  store i64 0, ptr %s_v490___ast_239___chali_borrowed_value_1311
  %t_v490___ast_239___chali_borrowed_value_1311 = alloca i8
  store i8 0, ptr %t_v490___ast_239___chali_borrowed_value_1311
  %s_v491___ast_239___chali_borrowed_value_1315 = alloca i64
  store i64 0, ptr %s_v491___ast_239___chali_borrowed_value_1315
  %t_v491___ast_239___chali_borrowed_value_1315 = alloca i8
  store i8 0, ptr %t_v491___ast_239___chali_borrowed_value_1315
  %s_v492___ast_239___chali_borrowed_value_1319 = alloca i64
  store i64 0, ptr %s_v492___ast_239___chali_borrowed_value_1319
  %t_v492___ast_239___chali_borrowed_value_1319 = alloca i8
  store i8 0, ptr %t_v492___ast_239___chali_borrowed_value_1319
  %s_v493___ast_239___chali_move_source_1309 = alloca i64
  store i64 0, ptr %s_v493___ast_239___chali_move_source_1309
  %t_v493___ast_239___chali_move_source_1309 = alloca i8
  store i8 0, ptr %t_v493___ast_239___chali_move_source_1309
  %s_v494___ast_239___chali_move_source_1310 = alloca i64
  store i64 0, ptr %s_v494___ast_239___chali_move_source_1310
  %t_v494___ast_239___chali_move_source_1310 = alloca i8
  store i8 0, ptr %t_v494___ast_239___chali_move_source_1310
  %s_v495___ast_239___chali_move_source_1313 = alloca i64
  store i64 0, ptr %s_v495___ast_239___chali_move_source_1313
  %t_v495___ast_239___chali_move_source_1313 = alloca i8
  store i8 0, ptr %t_v495___ast_239___chali_move_source_1313
  %s_v496___ast_239___chali_move_source_1314 = alloca i64
  store i64 0, ptr %s_v496___ast_239___chali_move_source_1314
  %t_v496___ast_239___chali_move_source_1314 = alloca i8
  store i8 0, ptr %t_v496___ast_239___chali_move_source_1314
  %s_v497___ast_239___chali_move_source_1317 = alloca i64
  store i64 0, ptr %s_v497___ast_239___chali_move_source_1317
  %t_v497___ast_239___chali_move_source_1317 = alloca i8
  store i8 0, ptr %t_v497___ast_239___chali_move_source_1317
  %s_v498___ast_239___chali_move_source_1318 = alloca i64
  store i64 0, ptr %s_v498___ast_239___chali_move_source_1318
  %t_v498___ast_239___chali_move_source_1318 = alloca i8
  store i8 0, ptr %t_v498___ast_239___chali_move_source_1318
  %s_v499___ast_239___chali_move_source_1321 = alloca i64
  store i64 0, ptr %s_v499___ast_239___chali_move_source_1321
  %t_v499___ast_239___chali_move_source_1321 = alloca i8
  store i8 0, ptr %t_v499___ast_239___chali_move_source_1321
  %s_v500___ast_239___chali_move_source_1322 = alloca i64
  store i64 0, ptr %s_v500___ast_239___chali_move_source_1322
  %t_v500___ast_239___chali_move_source_1322 = alloca i8
  store i8 0, ptr %t_v500___ast_239___chali_move_source_1322
  %s_v501___ast_239_i_1307 = alloca i64
  store i64 0, ptr %s_v501___ast_239_i_1307
  %t_v501___ast_239_i_1307 = alloca i8
  store i8 0, ptr %t_v501___ast_239_i_1307
  %s_v502___ast_239_value_1308 = alloca i64
  store i64 0, ptr %s_v502___ast_239_value_1308
  %t_v502___ast_239_value_1308 = alloca i8
  store i8 0, ptr %t_v502___ast_239_value_1308
  store i64 0, ptr %s_v501___ast_239_i_1307
  store i8 0, ptr %t_v501___ast_239_i_1307
  br label %ast_label_v503___chali_loop_141
ast_label_v503___chali_loop_141:
  %r0 = load i64, ptr %s_v501___ast_239_i_1307
  %r1 = load i8, ptr %t_v501___ast_239_i_1307
  store ptr @__chali_impl_program__setup, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 9, ptr @chali_source_line
  %r2 = icmp slt i64 %r0, 256
  %r3 = zext i1 %r2 to i64
  %r4 = icmp ne i64 %r3, 0
  br i1 %r4, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r5 = load i64, ptr %s_v501___ast_239_i_1307
  %r6 = load i8, ptr %t_v501___ast_239_i_1307
  store ptr @__chali_impl_program__setup, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 10, ptr @chali_source_line
  %r7 = call i64 @chali_integer_divide(i64 %r5, i64 4, i32 0, i32 1)
  store ptr @__chali_impl_program__setup, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 10, ptr @chali_source_line
  %r8 = icmp eq i64 %r7, 0
  %r9 = zext i1 %r8 to i64
  %r10 = icmp ne i64 %r9, 0
  br i1 %r10, label %ast_block_3, label %ast_block_4
ast_block_3:
  %r11 = call i64 @chali_alloc(i64 48)
  store i64 %r11, ptr %s_v492___ast_239___chali_borrowed_value_1319
  store i8 0, ptr %t_v492___ast_239___chali_borrowed_value_1319
  %r12 = load i64, ptr %s_v492___ast_239___chali_borrowed_value_1319
  %r13 = load i8, ptr %t_v492___ast_239___chali_borrowed_value_1319
  call void @chali_host_memory_prepare(i64 8, i64 %r12, i64 0, i64 48, ptr %raw_memory_output, ptr %raw_memory_input)
  %r14 = load ptr, ptr %raw_memory_output
  %r15 = trunc i64 0 to i8
  call void @llvm.memset.p0.i64(ptr %r14, i8 %r15, i64 48, i1 false)
  %r16 = load i64, ptr %s_v492___ast_239___chali_borrowed_value_1319
  %r17 = load i8, ptr %t_v492___ast_239___chali_borrowed_value_1319
  %r18 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r16, ptr %r18
  %r19 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r17, ptr %r19
  %r20 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 48, ptr %r20
  %r21 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r21
  %r22 = call i64 @chali_host_call(ptr @native_name0, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r23 = call i8 @chali_host_result_float()
  store ptr @__chali_impl_program__setup, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 10, ptr @chali_source_line
  %r24 = call i64 @f_v0___chali_enum_Event_Empty()
  %r25 = load i8, ptr @chali_return_float
  store i64 %r24, ptr %s_v499___ast_239___chali_move_source_1321
  store i8 %r25, ptr %t_v499___ast_239___chali_move_source_1321
  %r26 = load i64, ptr %s_v492___ast_239___chali_borrowed_value_1319
  %r27 = load i8, ptr %t_v492___ast_239___chali_borrowed_value_1319
  %r28 = load i64, ptr %s_v499___ast_239___chali_move_source_1321
  %r29 = load i8, ptr %t_v499___ast_239___chali_move_source_1321
  %r30 = call i64 @chali_host_copy_words(i64 %r26, i64 %r28, i64 6)
  %r31 = load i64, ptr %s_v492___ast_239___chali_borrowed_value_1319
  %r32 = load i8, ptr %t_v492___ast_239___chali_borrowed_value_1319
  %r33 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r31, ptr %r33
  %r34 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r32, ptr %r34
  %r35 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 48, ptr %r35
  %r36 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r36
  %r37 = call i64 @chali_host_call(ptr @native_name0, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r38 = call i8 @chali_host_result_float()
  %r39 = load i64, ptr %s_v492___ast_239___chali_borrowed_value_1319
  %r40 = load i8, ptr %t_v492___ast_239___chali_borrowed_value_1319
  %r41 = load i64, ptr %s_v499___ast_239___chali_move_source_1321
  %r42 = load i8, ptr %t_v499___ast_239___chali_move_source_1321
  %r43 = icmp ne i64 %r39, %r41
  %r44 = zext i1 %r43 to i64
  %r45 = icmp ne i64 %r44, 0
  br i1 %r45, label %ast_block_6, label %ast_block_7
ast_block_6:
  %r46 = load i64, ptr %s_v499___ast_239___chali_move_source_1321
  %r47 = load i8, ptr %t_v499___ast_239___chali_move_source_1321
  %r48 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r46, ptr %r48
  %r49 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r47, ptr %r49
  %r50 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 48, ptr %r50
  %r51 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r51
  %r52 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 290, ptr %r52
  %r53 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r53
  %r54 = call i64 @chali_host_call(ptr @native_name1, i64 3, ptr %indirect_arguments, ptr %native_argument_types)
  %r55 = call i8 @chali_host_result_float()
  %r56 = load i64, ptr %s_v499___ast_239___chali_move_source_1321
  %r57 = load i8, ptr %t_v499___ast_239___chali_move_source_1321
  %r58 = mul i64 4, 8
  %r59 = add i64 %r56, %r58
  %r60 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r59, ptr %r60
  %r61 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 0, ptr %r61
  %r62 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 16, ptr %r62
  %r63 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r63
  %r64 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 34, ptr %r64
  %r65 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r65
  %r66 = call i64 @chali_host_call(ptr @native_name1, i64 3, ptr %indirect_arguments, ptr %native_argument_types)
  %r67 = call i8 @chali_host_result_float()
  br label %ast_block_8
ast_block_7:
  br label %ast_block_8
ast_block_8:
  %r68 = load i64, ptr @s_g_0_program__events
  %r69 = load i8, ptr @t_g_0_program__events
  %r70 = load i64, ptr %s_v501___ast_239_i_1307
  %r71 = load i8, ptr %t_v501___ast_239_i_1307
  %r72 = mul i64 %r70, 48
  %r73 = add i64 %r68, %r72
  store i64 %r73, ptr %s_v488___ast_239___chali_borrowed_target_1320
  store i8 0, ptr %t_v488___ast_239___chali_borrowed_target_1320
  %r74 = load i64, ptr %s_v492___ast_239___chali_borrowed_value_1319
  %r75 = load i8, ptr %t_v492___ast_239___chali_borrowed_value_1319
  store i64 %r74, ptr %s_v500___ast_239___chali_move_source_1322
  store i8 %r75, ptr %t_v500___ast_239___chali_move_source_1322
  %r76 = load i64, ptr %s_v488___ast_239___chali_borrowed_target_1320
  %r77 = load i8, ptr %t_v488___ast_239___chali_borrowed_target_1320
  %r78 = load i64, ptr %s_v500___ast_239___chali_move_source_1322
  %r79 = load i8, ptr %t_v500___ast_239___chali_move_source_1322
  %r80 = call i64 @chali_host_copy_words(i64 %r76, i64 %r78, i64 6)
  %r81 = load i64, ptr %s_v488___ast_239___chali_borrowed_target_1320
  %r82 = load i8, ptr %t_v488___ast_239___chali_borrowed_target_1320
  %r83 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r81, ptr %r83
  %r84 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r82, ptr %r84
  %r85 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 48, ptr %r85
  %r86 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r86
  %r87 = call i64 @chali_host_call(ptr @native_name0, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r88 = call i8 @chali_host_result_float()
  %r89 = load i64, ptr %s_v488___ast_239___chali_borrowed_target_1320
  %r90 = load i8, ptr %t_v488___ast_239___chali_borrowed_target_1320
  %r91 = load i64, ptr %s_v500___ast_239___chali_move_source_1322
  %r92 = load i8, ptr %t_v500___ast_239___chali_move_source_1322
  %r93 = icmp ne i64 %r89, %r91
  %r94 = zext i1 %r93 to i64
  %r95 = icmp ne i64 %r94, 0
  br i1 %r95, label %ast_block_9, label %ast_block_10
ast_block_9:
  %r96 = load i64, ptr %s_v500___ast_239___chali_move_source_1322
  %r97 = load i8, ptr %t_v500___ast_239___chali_move_source_1322
  %r98 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r96, ptr %r98
  %r99 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r97, ptr %r99
  %r100 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 48, ptr %r100
  %r101 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r101
  %r102 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 290, ptr %r102
  %r103 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r103
  %r104 = call i64 @chali_host_call(ptr @native_name1, i64 3, ptr %indirect_arguments, ptr %native_argument_types)
  %r105 = call i8 @chali_host_result_float()
  %r106 = load i64, ptr %s_v500___ast_239___chali_move_source_1322
  %r107 = load i8, ptr %t_v500___ast_239___chali_move_source_1322
  %r108 = mul i64 4, 8
  %r109 = add i64 %r106, %r108
  %r110 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r109, ptr %r110
  %r111 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 0, ptr %r111
  %r112 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 16, ptr %r112
  %r113 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r113
  %r114 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 34, ptr %r114
  %r115 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r115
  %r116 = call i64 @chali_host_call(ptr @native_name1, i64 3, ptr %indirect_arguments, ptr %native_argument_types)
  %r117 = call i8 @chali_host_result_float()
  br label %ast_block_11
ast_block_10:
  br label %ast_block_11
ast_block_11:
  %r118 = load i64, ptr %s_v492___ast_239___chali_borrowed_value_1319
  %r119 = load i8, ptr %t_v492___ast_239___chali_borrowed_value_1319
  %r120 = icmp ne i64 %r118, 0
  %r121 = zext i1 %r120 to i64
  %r122 = icmp ne i64 %r121, 0
  br i1 %r122, label %ast_block_12, label %ast_block_13
ast_block_12:
  %r123 = load i64, ptr %s_v492___ast_239___chali_borrowed_value_1319
  %r124 = load i8, ptr %t_v492___ast_239___chali_borrowed_value_1319
  call void @chali_owned_drop(i64 %r123)
  br label %ast_block_14
ast_block_13:
  br label %ast_block_14
ast_block_14:
  store i64 0, ptr %s_v492___ast_239___chali_borrowed_value_1319
  store i8 0, ptr %t_v492___ast_239___chali_borrowed_value_1319
  br label %ast_block_5
ast_block_4:
  %r125 = load i64, ptr %s_v501___ast_239_i_1307
  %r126 = load i8, ptr %t_v501___ast_239_i_1307
  store ptr @__chali_impl_program__setup, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 11, ptr @chali_source_line
  %r127 = call i64 @chali_integer_divide(i64 %r125, i64 4, i32 0, i32 1)
  store ptr @__chali_impl_program__setup, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 11, ptr @chali_source_line
  %r128 = icmp eq i64 %r127, 1
  %r129 = zext i1 %r128 to i64
  %r130 = icmp ne i64 %r129, 0
  br i1 %r130, label %ast_block_15, label %ast_block_16
ast_block_15:
  %r131 = call i64 @chali_alloc(i64 48)
  store i64 %r131, ptr %s_v491___ast_239___chali_borrowed_value_1315
  store i8 0, ptr %t_v491___ast_239___chali_borrowed_value_1315
  %r132 = load i64, ptr %s_v491___ast_239___chali_borrowed_value_1315
  %r133 = load i8, ptr %t_v491___ast_239___chali_borrowed_value_1315
  call void @chali_host_memory_prepare(i64 8, i64 %r132, i64 0, i64 48, ptr %raw_memory_output, ptr %raw_memory_input)
  %r134 = load ptr, ptr %raw_memory_output
  %r135 = trunc i64 0 to i8
  call void @llvm.memset.p0.i64(ptr %r134, i8 %r135, i64 48, i1 false)
  %r136 = load i64, ptr %s_v491___ast_239___chali_borrowed_value_1315
  %r137 = load i8, ptr %t_v491___ast_239___chali_borrowed_value_1315
  %r138 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r136, ptr %r138
  %r139 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r137, ptr %r139
  %r140 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 48, ptr %r140
  %r141 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r141
  %r142 = call i64 @chali_host_call(ptr @native_name0, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r143 = call i8 @chali_host_result_float()
  %r144 = load i64, ptr %s_v501___ast_239_i_1307
  %r145 = load i8, ptr %t_v501___ast_239_i_1307
  store ptr @__chali_impl_program__setup, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 11, ptr @chali_source_line
  %r146 = mul i64 %r144, 3
  store ptr @__chali_impl_program__setup, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 11, ptr @chali_source_line
  %r147 = add i64 %r146, 7
  store ptr @__chali_impl_program__setup, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 11, ptr @chali_source_line
  %r148 = call i64 @f_v1___chali_enum_Event_One(i64 %r147)
  %r149 = load i8, ptr @chali_return_float
  store i64 %r148, ptr %s_v497___ast_239___chali_move_source_1317
  store i8 %r149, ptr %t_v497___ast_239___chali_move_source_1317
  %r150 = load i64, ptr %s_v491___ast_239___chali_borrowed_value_1315
  %r151 = load i8, ptr %t_v491___ast_239___chali_borrowed_value_1315
  %r152 = load i64, ptr %s_v497___ast_239___chali_move_source_1317
  %r153 = load i8, ptr %t_v497___ast_239___chali_move_source_1317
  %r154 = call i64 @chali_host_copy_words(i64 %r150, i64 %r152, i64 6)
  %r155 = load i64, ptr %s_v491___ast_239___chali_borrowed_value_1315
  %r156 = load i8, ptr %t_v491___ast_239___chali_borrowed_value_1315
  %r157 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r155, ptr %r157
  %r158 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r156, ptr %r158
  %r159 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 48, ptr %r159
  %r160 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r160
  %r161 = call i64 @chali_host_call(ptr @native_name0, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r162 = call i8 @chali_host_result_float()
  %r163 = load i64, ptr %s_v491___ast_239___chali_borrowed_value_1315
  %r164 = load i8, ptr %t_v491___ast_239___chali_borrowed_value_1315
  %r165 = load i64, ptr %s_v497___ast_239___chali_move_source_1317
  %r166 = load i8, ptr %t_v497___ast_239___chali_move_source_1317
  %r167 = icmp ne i64 %r163, %r165
  %r168 = zext i1 %r167 to i64
  %r169 = icmp ne i64 %r168, 0
  br i1 %r169, label %ast_block_18, label %ast_block_19
ast_block_18:
  %r170 = load i64, ptr %s_v497___ast_239___chali_move_source_1317
  %r171 = load i8, ptr %t_v497___ast_239___chali_move_source_1317
  %r172 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r170, ptr %r172
  %r173 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r171, ptr %r173
  %r174 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 48, ptr %r174
  %r175 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r175
  %r176 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 290, ptr %r176
  %r177 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r177
  %r178 = call i64 @chali_host_call(ptr @native_name1, i64 3, ptr %indirect_arguments, ptr %native_argument_types)
  %r179 = call i8 @chali_host_result_float()
  %r180 = load i64, ptr %s_v497___ast_239___chali_move_source_1317
  %r181 = load i8, ptr %t_v497___ast_239___chali_move_source_1317
  %r182 = mul i64 4, 8
  %r183 = add i64 %r180, %r182
  %r184 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r183, ptr %r184
  %r185 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 0, ptr %r185
  %r186 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 16, ptr %r186
  %r187 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r187
  %r188 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 34, ptr %r188
  %r189 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r189
  %r190 = call i64 @chali_host_call(ptr @native_name1, i64 3, ptr %indirect_arguments, ptr %native_argument_types)
  %r191 = call i8 @chali_host_result_float()
  br label %ast_block_20
ast_block_19:
  br label %ast_block_20
ast_block_20:
  %r192 = load i64, ptr @s_g_0_program__events
  %r193 = load i8, ptr @t_g_0_program__events
  %r194 = load i64, ptr %s_v501___ast_239_i_1307
  %r195 = load i8, ptr %t_v501___ast_239_i_1307
  %r196 = mul i64 %r194, 48
  %r197 = add i64 %r192, %r196
  store i64 %r197, ptr %s_v487___ast_239___chali_borrowed_target_1316
  store i8 0, ptr %t_v487___ast_239___chali_borrowed_target_1316
  %r198 = load i64, ptr %s_v491___ast_239___chali_borrowed_value_1315
  %r199 = load i8, ptr %t_v491___ast_239___chali_borrowed_value_1315
  store i64 %r198, ptr %s_v498___ast_239___chali_move_source_1318
  store i8 %r199, ptr %t_v498___ast_239___chali_move_source_1318
  %r200 = load i64, ptr %s_v487___ast_239___chali_borrowed_target_1316
  %r201 = load i8, ptr %t_v487___ast_239___chali_borrowed_target_1316
  %r202 = load i64, ptr %s_v498___ast_239___chali_move_source_1318
  %r203 = load i8, ptr %t_v498___ast_239___chali_move_source_1318
  %r204 = call i64 @chali_host_copy_words(i64 %r200, i64 %r202, i64 6)
  %r205 = load i64, ptr %s_v487___ast_239___chali_borrowed_target_1316
  %r206 = load i8, ptr %t_v487___ast_239___chali_borrowed_target_1316
  %r207 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r205, ptr %r207
  %r208 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r206, ptr %r208
  %r209 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 48, ptr %r209
  %r210 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r210
  %r211 = call i64 @chali_host_call(ptr @native_name0, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r212 = call i8 @chali_host_result_float()
  %r213 = load i64, ptr %s_v487___ast_239___chali_borrowed_target_1316
  %r214 = load i8, ptr %t_v487___ast_239___chali_borrowed_target_1316
  %r215 = load i64, ptr %s_v498___ast_239___chali_move_source_1318
  %r216 = load i8, ptr %t_v498___ast_239___chali_move_source_1318
  %r217 = icmp ne i64 %r213, %r215
  %r218 = zext i1 %r217 to i64
  %r219 = icmp ne i64 %r218, 0
  br i1 %r219, label %ast_block_21, label %ast_block_22
ast_block_21:
  %r220 = load i64, ptr %s_v498___ast_239___chali_move_source_1318
  %r221 = load i8, ptr %t_v498___ast_239___chali_move_source_1318
  %r222 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r220, ptr %r222
  %r223 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r221, ptr %r223
  %r224 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 48, ptr %r224
  %r225 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r225
  %r226 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 290, ptr %r226
  %r227 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r227
  %r228 = call i64 @chali_host_call(ptr @native_name1, i64 3, ptr %indirect_arguments, ptr %native_argument_types)
  %r229 = call i8 @chali_host_result_float()
  %r230 = load i64, ptr %s_v498___ast_239___chali_move_source_1318
  %r231 = load i8, ptr %t_v498___ast_239___chali_move_source_1318
  %r232 = mul i64 4, 8
  %r233 = add i64 %r230, %r232
  %r234 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r233, ptr %r234
  %r235 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 0, ptr %r235
  %r236 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 16, ptr %r236
  %r237 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r237
  %r238 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 34, ptr %r238
  %r239 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r239
  %r240 = call i64 @chali_host_call(ptr @native_name1, i64 3, ptr %indirect_arguments, ptr %native_argument_types)
  %r241 = call i8 @chali_host_result_float()
  br label %ast_block_23
ast_block_22:
  br label %ast_block_23
ast_block_23:
  %r242 = load i64, ptr %s_v491___ast_239___chali_borrowed_value_1315
  %r243 = load i8, ptr %t_v491___ast_239___chali_borrowed_value_1315
  %r244 = icmp ne i64 %r242, 0
  %r245 = zext i1 %r244 to i64
  %r246 = icmp ne i64 %r245, 0
  br i1 %r246, label %ast_block_24, label %ast_block_25
ast_block_24:
  %r247 = load i64, ptr %s_v491___ast_239___chali_borrowed_value_1315
  %r248 = load i8, ptr %t_v491___ast_239___chali_borrowed_value_1315
  call void @chali_owned_drop(i64 %r247)
  br label %ast_block_26
ast_block_25:
  br label %ast_block_26
ast_block_26:
  store i64 0, ptr %s_v491___ast_239___chali_borrowed_value_1315
  store i8 0, ptr %t_v491___ast_239___chali_borrowed_value_1315
  br label %ast_block_17
ast_block_16:
  %r249 = load i64, ptr %s_v501___ast_239_i_1307
  %r250 = load i8, ptr %t_v501___ast_239_i_1307
  store ptr @__chali_impl_program__setup, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 12, ptr @chali_source_line
  %r251 = call i64 @chali_integer_divide(i64 %r249, i64 4, i32 0, i32 1)
  store ptr @__chali_impl_program__setup, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 12, ptr @chali_source_line
  %r252 = icmp eq i64 %r251, 2
  %r253 = zext i1 %r252 to i64
  %r254 = icmp ne i64 %r253, 0
  br i1 %r254, label %ast_block_27, label %ast_block_28
ast_block_27:
  %r255 = call i64 @chali_alloc(i64 48)
  store i64 %r255, ptr %s_v490___ast_239___chali_borrowed_value_1311
  store i8 0, ptr %t_v490___ast_239___chali_borrowed_value_1311
  %r256 = load i64, ptr %s_v490___ast_239___chali_borrowed_value_1311
  %r257 = load i8, ptr %t_v490___ast_239___chali_borrowed_value_1311
  call void @chali_host_memory_prepare(i64 8, i64 %r256, i64 0, i64 48, ptr %raw_memory_output, ptr %raw_memory_input)
  %r258 = load ptr, ptr %raw_memory_output
  %r259 = trunc i64 0 to i8
  call void @llvm.memset.p0.i64(ptr %r258, i8 %r259, i64 48, i1 false)
  %r260 = load i64, ptr %s_v490___ast_239___chali_borrowed_value_1311
  %r261 = load i8, ptr %t_v490___ast_239___chali_borrowed_value_1311
  %r262 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r260, ptr %r262
  %r263 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r261, ptr %r263
  %r264 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 48, ptr %r264
  %r265 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r265
  %r266 = call i64 @chali_host_call(ptr @native_name0, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r267 = call i8 @chali_host_result_float()
  %r268 = load i64, ptr %s_v501___ast_239_i_1307
  %r269 = load i8, ptr %t_v501___ast_239_i_1307
  store ptr @__chali_impl_program__setup, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 12, ptr @chali_source_line
  %r270 = mul i64 %r268, 5
  %r271 = load i64, ptr %s_v501___ast_239_i_1307
  %r272 = load i8, ptr %t_v501___ast_239_i_1307
  store ptr @__chali_impl_program__setup, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 12, ptr @chali_source_line
  %r273 = add i64 %r271, 11
  store ptr @__chali_impl_program__setup, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 12, ptr @chali_source_line
  %r274 = call i64 @f_v2___chali_enum_Event_Pair(i64 %r270, i64 %r273)
  %r275 = load i8, ptr @chali_return_float
  store i64 %r274, ptr %s_v495___ast_239___chali_move_source_1313
  store i8 %r275, ptr %t_v495___ast_239___chali_move_source_1313
  %r276 = load i64, ptr %s_v490___ast_239___chali_borrowed_value_1311
  %r277 = load i8, ptr %t_v490___ast_239___chali_borrowed_value_1311
  %r278 = load i64, ptr %s_v495___ast_239___chali_move_source_1313
  %r279 = load i8, ptr %t_v495___ast_239___chali_move_source_1313
  %r280 = call i64 @chali_host_copy_words(i64 %r276, i64 %r278, i64 6)
  %r281 = load i64, ptr %s_v490___ast_239___chali_borrowed_value_1311
  %r282 = load i8, ptr %t_v490___ast_239___chali_borrowed_value_1311
  %r283 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r281, ptr %r283
  %r284 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r282, ptr %r284
  %r285 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 48, ptr %r285
  %r286 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r286
  %r287 = call i64 @chali_host_call(ptr @native_name0, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r288 = call i8 @chali_host_result_float()
  %r289 = load i64, ptr %s_v490___ast_239___chali_borrowed_value_1311
  %r290 = load i8, ptr %t_v490___ast_239___chali_borrowed_value_1311
  %r291 = load i64, ptr %s_v495___ast_239___chali_move_source_1313
  %r292 = load i8, ptr %t_v495___ast_239___chali_move_source_1313
  %r293 = icmp ne i64 %r289, %r291
  %r294 = zext i1 %r293 to i64
  %r295 = icmp ne i64 %r294, 0
  br i1 %r295, label %ast_block_30, label %ast_block_31
ast_block_30:
  %r296 = load i64, ptr %s_v495___ast_239___chali_move_source_1313
  %r297 = load i8, ptr %t_v495___ast_239___chali_move_source_1313
  %r298 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r296, ptr %r298
  %r299 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r297, ptr %r299
  %r300 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 48, ptr %r300
  %r301 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r301
  %r302 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 290, ptr %r302
  %r303 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r303
  %r304 = call i64 @chali_host_call(ptr @native_name1, i64 3, ptr %indirect_arguments, ptr %native_argument_types)
  %r305 = call i8 @chali_host_result_float()
  %r306 = load i64, ptr %s_v495___ast_239___chali_move_source_1313
  %r307 = load i8, ptr %t_v495___ast_239___chali_move_source_1313
  %r308 = mul i64 4, 8
  %r309 = add i64 %r306, %r308
  %r310 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r309, ptr %r310
  %r311 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 0, ptr %r311
  %r312 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 16, ptr %r312
  %r313 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r313
  %r314 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 34, ptr %r314
  %r315 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r315
  %r316 = call i64 @chali_host_call(ptr @native_name1, i64 3, ptr %indirect_arguments, ptr %native_argument_types)
  %r317 = call i8 @chali_host_result_float()
  br label %ast_block_32
ast_block_31:
  br label %ast_block_32
ast_block_32:
  %r318 = load i64, ptr @s_g_0_program__events
  %r319 = load i8, ptr @t_g_0_program__events
  %r320 = load i64, ptr %s_v501___ast_239_i_1307
  %r321 = load i8, ptr %t_v501___ast_239_i_1307
  %r322 = mul i64 %r320, 48
  %r323 = add i64 %r318, %r322
  store i64 %r323, ptr %s_v486___ast_239___chali_borrowed_target_1312
  store i8 0, ptr %t_v486___ast_239___chali_borrowed_target_1312
  %r324 = load i64, ptr %s_v490___ast_239___chali_borrowed_value_1311
  %r325 = load i8, ptr %t_v490___ast_239___chali_borrowed_value_1311
  store i64 %r324, ptr %s_v496___ast_239___chali_move_source_1314
  store i8 %r325, ptr %t_v496___ast_239___chali_move_source_1314
  %r326 = load i64, ptr %s_v486___ast_239___chali_borrowed_target_1312
  %r327 = load i8, ptr %t_v486___ast_239___chali_borrowed_target_1312
  %r328 = load i64, ptr %s_v496___ast_239___chali_move_source_1314
  %r329 = load i8, ptr %t_v496___ast_239___chali_move_source_1314
  %r330 = call i64 @chali_host_copy_words(i64 %r326, i64 %r328, i64 6)
  %r331 = load i64, ptr %s_v486___ast_239___chali_borrowed_target_1312
  %r332 = load i8, ptr %t_v486___ast_239___chali_borrowed_target_1312
  %r333 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r331, ptr %r333
  %r334 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r332, ptr %r334
  %r335 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 48, ptr %r335
  %r336 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r336
  %r337 = call i64 @chali_host_call(ptr @native_name0, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r338 = call i8 @chali_host_result_float()
  %r339 = load i64, ptr %s_v486___ast_239___chali_borrowed_target_1312
  %r340 = load i8, ptr %t_v486___ast_239___chali_borrowed_target_1312
  %r341 = load i64, ptr %s_v496___ast_239___chali_move_source_1314
  %r342 = load i8, ptr %t_v496___ast_239___chali_move_source_1314
  %r343 = icmp ne i64 %r339, %r341
  %r344 = zext i1 %r343 to i64
  %r345 = icmp ne i64 %r344, 0
  br i1 %r345, label %ast_block_33, label %ast_block_34
ast_block_33:
  %r346 = load i64, ptr %s_v496___ast_239___chali_move_source_1314
  %r347 = load i8, ptr %t_v496___ast_239___chali_move_source_1314
  %r348 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r346, ptr %r348
  %r349 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r347, ptr %r349
  %r350 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 48, ptr %r350
  %r351 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r351
  %r352 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 290, ptr %r352
  %r353 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r353
  %r354 = call i64 @chali_host_call(ptr @native_name1, i64 3, ptr %indirect_arguments, ptr %native_argument_types)
  %r355 = call i8 @chali_host_result_float()
  %r356 = load i64, ptr %s_v496___ast_239___chali_move_source_1314
  %r357 = load i8, ptr %t_v496___ast_239___chali_move_source_1314
  %r358 = mul i64 4, 8
  %r359 = add i64 %r356, %r358
  %r360 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r359, ptr %r360
  %r361 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 0, ptr %r361
  %r362 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 16, ptr %r362
  %r363 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r363
  %r364 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 34, ptr %r364
  %r365 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r365
  %r366 = call i64 @chali_host_call(ptr @native_name1, i64 3, ptr %indirect_arguments, ptr %native_argument_types)
  %r367 = call i8 @chali_host_result_float()
  br label %ast_block_35
ast_block_34:
  br label %ast_block_35
ast_block_35:
  %r368 = load i64, ptr %s_v490___ast_239___chali_borrowed_value_1311
  %r369 = load i8, ptr %t_v490___ast_239___chali_borrowed_value_1311
  %r370 = icmp ne i64 %r368, 0
  %r371 = zext i1 %r370 to i64
  %r372 = icmp ne i64 %r371, 0
  br i1 %r372, label %ast_block_36, label %ast_block_37
ast_block_36:
  %r373 = load i64, ptr %s_v490___ast_239___chali_borrowed_value_1311
  %r374 = load i8, ptr %t_v490___ast_239___chali_borrowed_value_1311
  call void @chali_owned_drop(i64 %r373)
  br label %ast_block_38
ast_block_37:
  br label %ast_block_38
ast_block_38:
  store i64 0, ptr %s_v490___ast_239___chali_borrowed_value_1311
  store i8 0, ptr %t_v490___ast_239___chali_borrowed_value_1311
  br label %ast_block_29
ast_block_28:
  %r375 = call i64 @chali_alloc(i64 16)
  store i64 %r375, ptr %s_v502___ast_239_value_1308
  store i8 0, ptr %t_v502___ast_239_value_1308
  %r376 = load i64, ptr %s_v502___ast_239_value_1308
  %r377 = load i8, ptr %t_v502___ast_239_value_1308
  call void @chali_host_memory_prepare(i64 8, i64 %r376, i64 0, i64 16, ptr %raw_memory_output, ptr %raw_memory_input)
  %r378 = load ptr, ptr %raw_memory_output
  %r379 = trunc i64 0 to i8
  call void @llvm.memset.p0.i64(ptr %r378, i8 %r379, i64 16, i1 false)
  %r380 = load i64, ptr %s_v502___ast_239_value_1308
  %r381 = load i8, ptr %t_v502___ast_239_value_1308
  %r382 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r380, ptr %r382
  %r383 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r381, ptr %r383
  %r384 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 16, ptr %r384
  %r385 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r385
  %r386 = call i64 @chali_host_call(ptr @native_name0, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r387 = call i8 @chali_host_result_float()
  %r388 = load i64, ptr %s_v501___ast_239_i_1307
  %r389 = load i8, ptr %t_v501___ast_239_i_1307
  store ptr @__chali_impl_program__setup, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 13, ptr @chali_source_line
  %r390 = mul i64 %r388, 7
  %r391 = load i64, ptr %s_v502___ast_239_value_1308
  %r392 = load i8, ptr %t_v502___ast_239_value_1308
  %r393 = mul i64 0, 8
  %r394 = add i64 %r391, %r393
  %r395 = call ptr @chali_primitive_address(i64 %r394, i64 8)
  store i64 %r390, ptr %r395, align 1
  %r396 = load i64, ptr %s_v501___ast_239_i_1307
  %r397 = load i8, ptr %t_v501___ast_239_i_1307
  store ptr @__chali_impl_program__setup, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 13, ptr @chali_source_line
  %r398 = add i64 %r396, 13
  %r399 = load i64, ptr %s_v502___ast_239_value_1308
  %r400 = load i8, ptr %t_v502___ast_239_value_1308
  %r401 = mul i64 1, 8
  %r402 = add i64 %r399, %r401
  %r403 = call ptr @chali_primitive_address(i64 %r402, i64 8)
  store i64 %r398, ptr %r403, align 1
  %r404 = call i64 @chali_alloc(i64 48)
  store i64 %r404, ptr %s_v489___ast_239___chali_borrowed_value
  store i8 0, ptr %t_v489___ast_239___chali_borrowed_value
  %r405 = load i64, ptr %s_v489___ast_239___chali_borrowed_value
  %r406 = load i8, ptr %t_v489___ast_239___chali_borrowed_value
  call void @chali_host_memory_prepare(i64 8, i64 %r405, i64 0, i64 48, ptr %raw_memory_output, ptr %raw_memory_input)
  %r407 = load ptr, ptr %raw_memory_output
  %r408 = trunc i64 0 to i8
  call void @llvm.memset.p0.i64(ptr %r407, i8 %r408, i64 48, i1 false)
  %r409 = load i64, ptr %s_v489___ast_239___chali_borrowed_value
  %r410 = load i8, ptr %t_v489___ast_239___chali_borrowed_value
  %r411 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r409, ptr %r411
  %r412 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r410, ptr %r412
  %r413 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 48, ptr %r413
  %r414 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r414
  %r415 = call i64 @chali_host_call(ptr @native_name0, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r416 = call i8 @chali_host_result_float()
  %r417 = load i64, ptr %s_v502___ast_239_value_1308
  %r418 = load i8, ptr %t_v502___ast_239_value_1308
  store ptr @__chali_impl_program__setup, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 13, ptr @chali_source_line
  %r419 = call i64 @f_v3___chali_enum_Event_Record(i64 %r417)
  %r420 = load i8, ptr @chali_return_float
  store i64 %r419, ptr %s_v493___ast_239___chali_move_source_1309
  store i8 %r420, ptr %t_v493___ast_239___chali_move_source_1309
  %r421 = load i64, ptr %s_v489___ast_239___chali_borrowed_value
  %r422 = load i8, ptr %t_v489___ast_239___chali_borrowed_value
  %r423 = load i64, ptr %s_v493___ast_239___chali_move_source_1309
  %r424 = load i8, ptr %t_v493___ast_239___chali_move_source_1309
  %r425 = call i64 @chali_host_copy_words(i64 %r421, i64 %r423, i64 6)
  %r426 = load i64, ptr %s_v489___ast_239___chali_borrowed_value
  %r427 = load i8, ptr %t_v489___ast_239___chali_borrowed_value
  %r428 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r426, ptr %r428
  %r429 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r427, ptr %r429
  %r430 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 48, ptr %r430
  %r431 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r431
  %r432 = call i64 @chali_host_call(ptr @native_name0, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r433 = call i8 @chali_host_result_float()
  %r434 = load i64, ptr %s_v489___ast_239___chali_borrowed_value
  %r435 = load i8, ptr %t_v489___ast_239___chali_borrowed_value
  %r436 = load i64, ptr %s_v493___ast_239___chali_move_source_1309
  %r437 = load i8, ptr %t_v493___ast_239___chali_move_source_1309
  %r438 = icmp ne i64 %r434, %r436
  %r439 = zext i1 %r438 to i64
  %r440 = icmp ne i64 %r439, 0
  br i1 %r440, label %ast_block_39, label %ast_block_40
ast_block_39:
  %r441 = load i64, ptr %s_v493___ast_239___chali_move_source_1309
  %r442 = load i8, ptr %t_v493___ast_239___chali_move_source_1309
  %r443 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r441, ptr %r443
  %r444 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r442, ptr %r444
  %r445 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 48, ptr %r445
  %r446 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r446
  %r447 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 290, ptr %r447
  %r448 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r448
  %r449 = call i64 @chali_host_call(ptr @native_name1, i64 3, ptr %indirect_arguments, ptr %native_argument_types)
  %r450 = call i8 @chali_host_result_float()
  %r451 = load i64, ptr %s_v493___ast_239___chali_move_source_1309
  %r452 = load i8, ptr %t_v493___ast_239___chali_move_source_1309
  %r453 = mul i64 4, 8
  %r454 = add i64 %r451, %r453
  %r455 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r454, ptr %r455
  %r456 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 0, ptr %r456
  %r457 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 16, ptr %r457
  %r458 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r458
  %r459 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 34, ptr %r459
  %r460 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r460
  %r461 = call i64 @chali_host_call(ptr @native_name1, i64 3, ptr %indirect_arguments, ptr %native_argument_types)
  %r462 = call i8 @chali_host_result_float()
  br label %ast_block_41
ast_block_40:
  br label %ast_block_41
ast_block_41:
  %r463 = load i64, ptr @s_g_0_program__events
  %r464 = load i8, ptr @t_g_0_program__events
  %r465 = load i64, ptr %s_v501___ast_239_i_1307
  %r466 = load i8, ptr %t_v501___ast_239_i_1307
  %r467 = mul i64 %r465, 48
  %r468 = add i64 %r463, %r467
  store i64 %r468, ptr %s_v485___ast_239___chali_borrowed_target
  store i8 0, ptr %t_v485___ast_239___chali_borrowed_target
  %r469 = load i64, ptr %s_v489___ast_239___chali_borrowed_value
  %r470 = load i8, ptr %t_v489___ast_239___chali_borrowed_value
  store i64 %r469, ptr %s_v494___ast_239___chali_move_source_1310
  store i8 %r470, ptr %t_v494___ast_239___chali_move_source_1310
  %r471 = load i64, ptr %s_v485___ast_239___chali_borrowed_target
  %r472 = load i8, ptr %t_v485___ast_239___chali_borrowed_target
  %r473 = load i64, ptr %s_v494___ast_239___chali_move_source_1310
  %r474 = load i8, ptr %t_v494___ast_239___chali_move_source_1310
  %r475 = call i64 @chali_host_copy_words(i64 %r471, i64 %r473, i64 6)
  %r476 = load i64, ptr %s_v485___ast_239___chali_borrowed_target
  %r477 = load i8, ptr %t_v485___ast_239___chali_borrowed_target
  %r478 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r476, ptr %r478
  %r479 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r477, ptr %r479
  %r480 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 48, ptr %r480
  %r481 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r481
  %r482 = call i64 @chali_host_call(ptr @native_name0, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r483 = call i8 @chali_host_result_float()
  %r484 = load i64, ptr %s_v485___ast_239___chali_borrowed_target
  %r485 = load i8, ptr %t_v485___ast_239___chali_borrowed_target
  %r486 = load i64, ptr %s_v494___ast_239___chali_move_source_1310
  %r487 = load i8, ptr %t_v494___ast_239___chali_move_source_1310
  %r488 = icmp ne i64 %r484, %r486
  %r489 = zext i1 %r488 to i64
  %r490 = icmp ne i64 %r489, 0
  br i1 %r490, label %ast_block_42, label %ast_block_43
ast_block_42:
  %r491 = load i64, ptr %s_v494___ast_239___chali_move_source_1310
  %r492 = load i8, ptr %t_v494___ast_239___chali_move_source_1310
  %r493 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r491, ptr %r493
  %r494 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r492, ptr %r494
  %r495 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 48, ptr %r495
  %r496 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r496
  %r497 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 290, ptr %r497
  %r498 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r498
  %r499 = call i64 @chali_host_call(ptr @native_name1, i64 3, ptr %indirect_arguments, ptr %native_argument_types)
  %r500 = call i8 @chali_host_result_float()
  %r501 = load i64, ptr %s_v494___ast_239___chali_move_source_1310
  %r502 = load i8, ptr %t_v494___ast_239___chali_move_source_1310
  %r503 = mul i64 4, 8
  %r504 = add i64 %r501, %r503
  %r505 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r504, ptr %r505
  %r506 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 0, ptr %r506
  %r507 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 16, ptr %r507
  %r508 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r508
  %r509 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 34, ptr %r509
  %r510 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r510
  %r511 = call i64 @chali_host_call(ptr @native_name1, i64 3, ptr %indirect_arguments, ptr %native_argument_types)
  %r512 = call i8 @chali_host_result_float()
  br label %ast_block_44
ast_block_43:
  br label %ast_block_44
ast_block_44:
  %r513 = load i64, ptr %s_v489___ast_239___chali_borrowed_value
  %r514 = load i8, ptr %t_v489___ast_239___chali_borrowed_value
  %r515 = icmp ne i64 %r513, 0
  %r516 = zext i1 %r515 to i64
  %r517 = icmp ne i64 %r516, 0
  br i1 %r517, label %ast_block_45, label %ast_block_46
ast_block_45:
  %r518 = load i64, ptr %s_v489___ast_239___chali_borrowed_value
  %r519 = load i8, ptr %t_v489___ast_239___chali_borrowed_value
  call void @chali_owned_drop(i64 %r518)
  br label %ast_block_47
ast_block_46:
  br label %ast_block_47
ast_block_47:
  store i64 0, ptr %s_v489___ast_239___chali_borrowed_value
  store i8 0, ptr %t_v489___ast_239___chali_borrowed_value
  %r520 = load i64, ptr %s_v502___ast_239_value_1308
  %r521 = load i8, ptr %t_v502___ast_239_value_1308
  %r522 = icmp ne i64 %r520, 0
  %r523 = zext i1 %r522 to i64
  %r524 = icmp ne i64 %r523, 0
  br i1 %r524, label %ast_block_48, label %ast_block_49
ast_block_48:
  %r525 = load i64, ptr %s_v502___ast_239_value_1308
  %r526 = load i8, ptr %t_v502___ast_239_value_1308
  call void @chali_owned_drop(i64 %r525)
  br label %ast_block_50
ast_block_49:
  br label %ast_block_50
ast_block_50:
  store i64 0, ptr %s_v502___ast_239_value_1308
  store i8 0, ptr %t_v502___ast_239_value_1308
  br label %ast_block_29
ast_block_29:
  br label %ast_block_17
ast_block_17:
  br label %ast_block_5
ast_block_5:
  %r527 = load i64, ptr %s_v501___ast_239_i_1307
  %r528 = load i8, ptr %t_v501___ast_239_i_1307
  store ptr @__chali_impl_program__setup, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 14, ptr @chali_source_line
  %r529 = add i64 %r527, 1
  store i64 %r529, ptr %s_v501___ast_239_i_1307
  store i8 0, ptr %t_v501___ast_239_i_1307
  br label %ast_block_2
ast_block_1:
  br label %ast_label_v504___chali_brk_142
ast_block_2:
  br label %ast_label_v503___chali_loop_141
ast_label_v504___chali_brk_142:
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define i64 @__chali_impl_program__work(i64 %p0) uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [2 x i64]
  %native_argument_types = alloca [2 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v505___ast_240___chali_ret_v_1329 = alloca i64
  store i64 0, ptr %s_v505___ast_240___chali_ret_v_1329
  %t_v505___ast_240___chali_ret_v_1329 = alloca i8
  store i8 0, ptr %t_v505___ast_240___chali_ret_v_1329
  %s_v506___ast_240___chali_switch_enum = alloca i64
  store i64 0, ptr %s_v506___ast_240___chali_switch_enum
  %t_v506___ast_240___chali_switch_enum = alloca i8
  store i8 0, ptr %t_v506___ast_240___chali_switch_enum
  %s_v507___ast_240_acc = alloca i64
  store i64 0, ptr %s_v507___ast_240_acc
  %t_v507___ast_240_acc = alloca i8
  store i8 0, ptr %t_v507___ast_240_acc
  %s_v508___ast_240_event = alloca i64
  store i64 0, ptr %s_v508___ast_240_event
  %t_v508___ast_240_event = alloca i8
  store i8 0, ptr %t_v508___ast_240_event
  %s_v509___ast_240_i_1323 = alloca i64
  store i64 0, ptr %s_v509___ast_240_i_1323
  %t_v509___ast_240_i_1323 = alloca i8
  store i8 0, ptr %t_v509___ast_240_i_1323
  %s_v510___ast_240_index_1324 = alloca i64
  store i64 0, ptr %s_v510___ast_240_index_1324
  %t_v510___ast_240_index_1324 = alloca i8
  store i8 0, ptr %t_v510___ast_240_index_1324
  %s_v511___ast_240_left_1326 = alloca i64
  store i64 0, ptr %s_v511___ast_240_left_1326
  %t_v511___ast_240_left_1326 = alloca i8
  store i8 0, ptr %t_v511___ast_240_left_1326
  %s_v512___ast_240_number = alloca i64
  store i64 0, ptr %s_v512___ast_240_number
  %t_v512___ast_240_number = alloca i8
  store i8 0, ptr %t_v512___ast_240_number
  %s_v513___ast_240_payload_1328 = alloca i64
  store i64 0, ptr %s_v513___ast_240_payload_1328
  %t_v513___ast_240_payload_1328 = alloca i8
  store i8 0, ptr %t_v513___ast_240_payload_1328
  %s_v514___ast_240_right_1327 = alloca i64
  store i64 0, ptr %s_v514___ast_240_right_1327
  %t_v514___ast_240_right_1327 = alloca i8
  store i8 0, ptr %t_v514___ast_240_right_1327
  %s_v515___ast_240_seed = alloca i64
  store i64 0, ptr %s_v515___ast_240_seed
  %t_v515___ast_240_seed = alloca i8
  store i8 0, ptr %t_v515___ast_240_seed
  %s_v516___ast_240_value_1325 = alloca i64
  store i64 0, ptr %s_v516___ast_240_value_1325
  %t_v516___ast_240_value_1325 = alloca i8
  store i8 0, ptr %t_v516___ast_240_value_1325
  store i64 %p0, ptr %s_v515___ast_240_seed
  %r0 = load i64, ptr %s_v515___ast_240_seed
  %r1 = load i8, ptr %t_v515___ast_240_seed
  store i64 %r0, ptr %s_v507___ast_240_acc
  store i8 %r1, ptr %t_v507___ast_240_acc
  store i64 0, ptr %s_v509___ast_240_i_1323
  store i8 0, ptr %t_v509___ast_240_i_1323
  br label %ast_label_v517___chali_loop_143
ast_label_v517___chali_loop_143:
  %r2 = load i64, ptr %s_v509___ast_240_i_1323
  %r3 = load i8, ptr %t_v509___ast_240_i_1323
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 21, ptr @chali_source_line
  %r4 = icmp slt i64 %r2, 262144
  %r5 = zext i1 %r4 to i64
  %r6 = icmp ne i64 %r5, 0
  br i1 %r6, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r7 = load i64, ptr %s_v509___ast_240_i_1323
  %r8 = load i8, ptr %t_v509___ast_240_i_1323
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 22, ptr @chali_source_line
  %r9 = mul i64 %r7, 13
  %r10 = load i64, ptr %s_v507___ast_240_acc
  %r11 = load i8, ptr %t_v507___ast_240_acc
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 22, ptr @chali_source_line
  call void @chali_check_shift(i64 7)
  %r12 = ashr i64 %r10, 7
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 22, ptr @chali_source_line
  %r13 = and i64 %r12, 255
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 22, ptr @chali_source_line
  %r14 = add i64 %r9, %r13
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 22, ptr @chali_source_line
  %r15 = and i64 %r14, 255
  store i64 %r15, ptr %s_v510___ast_240_index_1324
  store i8 0, ptr %t_v510___ast_240_index_1324
  %r16 = load i64, ptr @s_g_0_program__events
  %r17 = load i8, ptr @t_g_0_program__events
  %r18 = load i64, ptr %s_v510___ast_240_index_1324
  %r19 = load i8, ptr %t_v510___ast_240_index_1324
  %r20 = mul i64 %r18, 48
  %r21 = add i64 %r16, %r20
  %r22 = call i64 @chali_require_reference(i64 %r21)
  store i64 %r22, ptr %s_v508___ast_240_event
  store i8 0, ptr %t_v508___ast_240_event
  store i64 0, ptr %s_v516___ast_240_value_1325
  store i8 0, ptr %t_v516___ast_240_value_1325
  %r23 = load i64, ptr %s_v508___ast_240_event
  %r24 = load i8, ptr %t_v508___ast_240_event
  %r25 = call i64 @chali_require_reference(i64 %r23)
  store i64 %r25, ptr %s_v506___ast_240___chali_switch_enum
  store i8 0, ptr %t_v506___ast_240___chali_switch_enum
  %r26 = load i64, ptr %s_v506___ast_240___chali_switch_enum
  %r27 = load i8, ptr %t_v506___ast_240___chali_switch_enum
  %r28 = mul i64 0, 8
  %r29 = add i64 %r26, %r28
  %r30 = call ptr @chali_primitive_address(i64 %r29, i64 8)
  %r31 = load i64, ptr %r30, align 1
  %r32 = icmp eq i64 %r31, 0
  %r33 = zext i1 %r32 to i64
  %r34 = icmp ne i64 %r33, 0
  br i1 %r34, label %ast_block_3, label %ast_block_4
ast_block_3:
  store i64 17, ptr %s_v516___ast_240_value_1325
  store i8 0, ptr %t_v516___ast_240_value_1325
  br label %ast_label_v518___chali_brk_145
ast_block_4:
  br label %ast_block_5
ast_block_5:
  br label %ast_label_v519___chali_match_next_146
ast_label_v519___chali_match_next_146:
  %r35 = load i64, ptr %s_v506___ast_240___chali_switch_enum
  %r36 = load i8, ptr %t_v506___ast_240___chali_switch_enum
  %r37 = mul i64 0, 8
  %r38 = add i64 %r35, %r37
  %r39 = call ptr @chali_primitive_address(i64 %r38, i64 8)
  %r40 = load i64, ptr %r39, align 1
  %r41 = icmp eq i64 %r40, 1
  %r42 = zext i1 %r41 to i64
  %r43 = icmp ne i64 %r42, 0
  br i1 %r43, label %ast_block_6, label %ast_block_7
ast_block_6:
  %r44 = load i64, ptr %s_v506___ast_240___chali_switch_enum
  %r45 = load i8, ptr %t_v506___ast_240___chali_switch_enum
  %r46 = mul i64 1, 8
  %r47 = add i64 %r44, %r46
  %r48 = call ptr @chali_primitive_address(i64 %r47, i64 8)
  %r49 = load i64, ptr %r48, align 1
  store i64 %r49, ptr %s_v512___ast_240_number
  store i8 0, ptr %t_v512___ast_240_number
  %r50 = load i64, ptr %s_v512___ast_240_number
  %r51 = load i8, ptr %t_v512___ast_240_number
  store i64 %r50, ptr %s_v516___ast_240_value_1325
  store i8 %r51, ptr %t_v516___ast_240_value_1325
  br label %ast_label_v518___chali_brk_145
ast_block_7:
  br label %ast_block_8
ast_block_8:
  br label %ast_label_v520___chali_match_next_147
ast_label_v520___chali_match_next_147:
  %r52 = load i64, ptr %s_v506___ast_240___chali_switch_enum
  %r53 = load i8, ptr %t_v506___ast_240___chali_switch_enum
  %r54 = mul i64 0, 8
  %r55 = add i64 %r52, %r54
  %r56 = call ptr @chali_primitive_address(i64 %r55, i64 8)
  %r57 = load i64, ptr %r56, align 1
  %r58 = icmp eq i64 %r57, 2
  %r59 = zext i1 %r58 to i64
  %r60 = icmp ne i64 %r59, 0
  br i1 %r60, label %ast_block_9, label %ast_block_10
ast_block_9:
  %r61 = load i64, ptr %s_v506___ast_240___chali_switch_enum
  %r62 = load i8, ptr %t_v506___ast_240___chali_switch_enum
  %r63 = mul i64 2, 8
  %r64 = add i64 %r61, %r63
  %r65 = call ptr @chali_primitive_address(i64 %r64, i64 8)
  %r66 = load i64, ptr %r65, align 1
  store i64 %r66, ptr %s_v511___ast_240_left_1326
  store i8 0, ptr %t_v511___ast_240_left_1326
  %r67 = load i64, ptr %s_v506___ast_240___chali_switch_enum
  %r68 = load i8, ptr %t_v506___ast_240___chali_switch_enum
  %r69 = mul i64 3, 8
  %r70 = add i64 %r67, %r69
  %r71 = call ptr @chali_primitive_address(i64 %r70, i64 8)
  %r72 = load i64, ptr %r71, align 1
  store i64 %r72, ptr %s_v514___ast_240_right_1327
  store i8 0, ptr %t_v514___ast_240_right_1327
  %r73 = load i64, ptr %s_v511___ast_240_left_1326
  %r74 = load i8, ptr %t_v511___ast_240_left_1326
  %r75 = load i64, ptr %s_v514___ast_240_right_1327
  %r76 = load i8, ptr %t_v514___ast_240_right_1327
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 28, ptr @chali_source_line
  %r77 = add i64 %r73, %r75
  store i64 %r77, ptr %s_v516___ast_240_value_1325
  store i8 0, ptr %t_v516___ast_240_value_1325
  br label %ast_label_v518___chali_brk_145
ast_block_10:
  br label %ast_block_11
ast_block_11:
  br label %ast_label_v521___chali_match_next_148
ast_label_v521___chali_match_next_148:
  %r78 = load i64, ptr %s_v506___ast_240___chali_switch_enum
  %r79 = load i8, ptr %t_v506___ast_240___chali_switch_enum
  %r80 = mul i64 0, 8
  %r81 = add i64 %r78, %r80
  %r82 = call ptr @chali_primitive_address(i64 %r81, i64 8)
  %r83 = load i64, ptr %r82, align 1
  %r84 = icmp eq i64 %r83, 3
  %r85 = zext i1 %r84 to i64
  %r86 = icmp ne i64 %r85, 0
  br i1 %r86, label %ast_block_12, label %ast_block_13
ast_block_12:
  %r87 = load i64, ptr %s_v506___ast_240___chali_switch_enum
  %r88 = load i8, ptr %t_v506___ast_240___chali_switch_enum
  %r89 = mul i64 4, 8
  %r90 = add i64 %r87, %r89
  store i64 %r90, ptr %s_v513___ast_240_payload_1328
  store i8 0, ptr %t_v513___ast_240_payload_1328
  %r91 = load i64, ptr %s_v513___ast_240_payload_1328
  %r92 = load i8, ptr %t_v513___ast_240_payload_1328
  %r93 = mul i64 0, 8
  %r94 = add i64 %r91, %r93
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 29, ptr @chali_source_line
  %r95 = call ptr @chali_primitive_address(i64 %r94, i64 8)
  %r96 = load i64, ptr %r95, align 1
  %r97 = load i64, ptr %s_v513___ast_240_payload_1328
  %r98 = load i8, ptr %t_v513___ast_240_payload_1328
  %r99 = mul i64 1, 8
  %r100 = add i64 %r97, %r99
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 29, ptr @chali_source_line
  %r101 = call ptr @chali_primitive_address(i64 %r100, i64 8)
  %r102 = load i64, ptr %r101, align 1
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 29, ptr @chali_source_line
  %r103 = add i64 %r96, %r102
  store i64 %r103, ptr %s_v516___ast_240_value_1325
  store i8 0, ptr %t_v516___ast_240_value_1325
  br label %ast_label_v518___chali_brk_145
ast_block_13:
  br label %ast_block_14
ast_block_14:
  br label %ast_label_v522___chali_match_next_149
ast_label_v522___chali_match_next_149:
  br label %ast_label_v518___chali_brk_145
ast_label_v518___chali_brk_145:
  %r104 = load i64, ptr %s_v507___ast_240_acc
  %r105 = load i8, ptr %t_v507___ast_240_acc
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 31, ptr @chali_source_line
  %r106 = mul i64 %r104, 33
  %r107 = load i64, ptr %s_v516___ast_240_value_1325
  %r108 = load i8, ptr %t_v516___ast_240_value_1325
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 31, ptr @chali_source_line
  %r109 = add i64 %r106, %r107
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 31, ptr @chali_source_line
  %r110 = and i64 %r109, 2147483647
  store i64 %r110, ptr %s_v507___ast_240_acc
  store i8 0, ptr %t_v507___ast_240_acc
  %r111 = load i64, ptr %s_v509___ast_240_i_1323
  %r112 = load i8, ptr %t_v509___ast_240_i_1323
  store ptr @__chali_impl_program__work, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 32, ptr @chali_source_line
  %r113 = add i64 %r111, 1
  store i64 %r113, ptr %s_v509___ast_240_i_1323
  store i8 0, ptr %t_v509___ast_240_i_1323
  br label %ast_block_2
ast_block_1:
  br label %ast_label_v523___chali_brk_144
ast_block_2:
  br label %ast_label_v517___chali_loop_143
ast_label_v523___chali_brk_144:
  %r114 = load i64, ptr %s_v507___ast_240_acc
  %r115 = load i8, ptr %t_v507___ast_240_acc
  store i64 %r114, ptr %s_v505___ast_240___chali_ret_v_1329
  store i8 %r115, ptr %t_v505___ast_240___chali_ret_v_1329
  %r116 = load i64, ptr %s_v505___ast_240___chali_ret_v_1329
  %r117 = load i8, ptr %t_v505___ast_240___chali_ret_v_1329
  store i8 %r117, ptr @chali_return_float
  ret i64 %r116
}
define internal i64 @f___chali_library_init() uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [3 x i64]
  %native_argument_types = alloca [3 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v524___ast_469___chali_array_init_index = alloca i64
  store i64 0, ptr %s_v524___ast_469___chali_array_init_index
  %t_v524___ast_469___chali_array_init_index = alloca i8
  store i8 0, ptr %t_v524___ast_469___chali_array_init_index
  store i64 208, ptr @s_g_0_program__events
  store i8 0, ptr @t_g_0_program__events
  store i64 0, ptr %s_v524___ast_469___chali_array_init_index
  store i8 0, ptr %t_v524___ast_469___chali_array_init_index
  br label %ast_label_v525___chali_array_init_1
ast_label_v525___chali_array_init_1:
  %r0 = load i64, ptr %s_v524___ast_469___chali_array_init_index
  %r1 = load i8, ptr %t_v524___ast_469___chali_array_init_index
  %r2 = icmp sge i64 %r0, 256
  %r3 = zext i1 %r2 to i64
  %r4 = icmp ne i64 %r3, 0
  br i1 %r4, label %ast_block_0, label %ast_block_1
ast_block_0:
  br label %ast_label_v526___chali_array_init_done_2
ast_block_1:
  br label %ast_block_2
ast_block_2:
  %r5 = load i64, ptr @s_g_0_program__events
  %r6 = load i8, ptr @t_g_0_program__events
  %r7 = load i64, ptr %s_v524___ast_469___chali_array_init_index
  %r8 = load i8, ptr %t_v524___ast_469___chali_array_init_index
  %r9 = mul i64 %r7, 48
  %r10 = add i64 %r5, %r9
  %r11 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r10, ptr %r11
  %r12 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 0, ptr %r12
  %r13 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 48, ptr %r13
  %r14 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r14
  %r15 = call i64 @chali_host_call(ptr @native_name0, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r16 = call i8 @chali_host_result_float()
  %r17 = load i64, ptr %s_v524___ast_469___chali_array_init_index
  %r18 = load i8, ptr %t_v524___ast_469___chali_array_init_index
  %r19 = add i64 %r17, 1
  store i64 %r19, ptr %s_v524___ast_469___chali_array_init_index
  store i8 0, ptr %t_v524___ast_469___chali_array_init_index
  br label %ast_label_v525___chali_array_init_1
ast_label_v526___chali_array_init_done_2:
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
  %r0 = load i64, ptr @s_g_0_program__events
  %r1 = load i8, ptr @t_g_0_program__events
  %r2 = icmp ne i64 %r0, 0
  %r3 = zext i1 %r2 to i64
  %r4 = icmp ne i64 %r3, 0
  br i1 %r4, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r5 = load i64, ptr @s_g_0_program__events
  %r6 = load i8, ptr @t_g_0_program__events
  call void @chali_owned_drop(i64 %r5)
  br label %ast_block_2
ast_block_1:
  br label %ast_block_2
ast_block_2:
  store i64 0, ptr @s_g_0_program__events
  store i8 0, ptr @t_g_0_program__events
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define internal i64 @f_v474___chali_copy_struct(i64 %p0, i64 %p1, i64 %p2) alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [1 x i64]
  %native_argument_types = alloca [1 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v527___ast_474___chali_dst = alloca i64
  store i64 0, ptr %s_v527___ast_474___chali_dst
  %t_v527___ast_474___chali_dst = alloca i8
  store i8 0, ptr %t_v527___ast_474___chali_dst
  %s_v528___ast_474___chali_k = alloca i64
  store i64 0, ptr %s_v528___ast_474___chali_k
  %t_v528___ast_474___chali_k = alloca i8
  store i8 0, ptr %t_v528___ast_474___chali_k
  %s_v529___ast_474___chali_slots = alloca i64
  store i64 0, ptr %s_v529___ast_474___chali_slots
  %t_v529___ast_474___chali_slots = alloca i8
  store i8 0, ptr %t_v529___ast_474___chali_slots
  %s_v530___ast_474___chali_src = alloca i64
  store i64 0, ptr %s_v530___ast_474___chali_src
  %t_v530___ast_474___chali_src = alloca i8
  store i8 0, ptr %t_v530___ast_474___chali_src
  store i64 %p0, ptr %s_v527___ast_474___chali_dst
  store i64 %p1, ptr %s_v530___ast_474___chali_src
  store i64 %p2, ptr %s_v529___ast_474___chali_slots
  store i64 0, ptr %s_v528___ast_474___chali_k
  store i8 0, ptr %t_v528___ast_474___chali_k
  br label %ast_block_0
ast_block_0:
  %r0 = load i64, ptr %s_v528___ast_474___chali_k
  %r1 = load i8, ptr %t_v528___ast_474___chali_k
  %r2 = load i64, ptr %s_v529___ast_474___chali_slots
  %r3 = load i8, ptr %t_v529___ast_474___chali_slots
  %r4 = icmp slt i64 %r0, %r2
  %r5 = zext i1 %r4 to i64
  %r6 = icmp ne i64 %r5, 0
  br i1 %r6, label %ast_block_1, label %ast_block_2
ast_block_1:
  %r7 = load i64, ptr %s_v530___ast_474___chali_src
  %r8 = load i8, ptr %t_v530___ast_474___chali_src
  %r9 = load i64, ptr %s_v528___ast_474___chali_k
  %r10 = load i8, ptr %t_v528___ast_474___chali_k
  %r11 = mul i64 %r9, 8
  %r12 = add i64 %r7, %r11
  %r13 = call ptr @chali_primitive_address(i64 %r12, i64 8)
  %r14 = load i64, ptr %r13, align 1
  %r15 = load i64, ptr %s_v527___ast_474___chali_dst
  %r16 = load i8, ptr %t_v527___ast_474___chali_dst
  %r17 = load i64, ptr %s_v528___ast_474___chali_k
  %r18 = load i8, ptr %t_v528___ast_474___chali_k
  %r19 = mul i64 %r17, 8
  %r20 = add i64 %r15, %r19
  %r21 = call ptr @chali_primitive_address(i64 %r20, i64 8)
  store i64 %r14, ptr %r21, align 1
  %r22 = load i64, ptr %s_v528___ast_474___chali_k
  %r23 = load i8, ptr %t_v528___ast_474___chali_k
  %r24 = add i64 %r22, 1
  %r25 = trunc i64 %r24 to i32
  %r26 = sext i32 %r25 to i64
  store i64 %r26, ptr %s_v528___ast_474___chali_k
  store i8 0, ptr %t_v528___ast_474___chali_k
  br label %ast_block_0
ast_block_2:
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
