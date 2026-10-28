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
define dso_local void @chali_init() local_unnamed_addr #0 {
  tail call void @chali_host_init() #17
  ret void
}

declare dso_local void @chali_host_init() local_unnamed_addr #1

; Function Attrs: nounwind uwtable
define dso_local void @chali_finish() local_unnamed_addr #0 {
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
declare i64 @chali_host_object_scalar(i64, i64)
declare ptr @chali_host_data_snapshot(ptr nocapture writeonly)
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
define internal ptr @chali_data_address(i64 %address, i64 %count, ptr %base, i64 %size) alwaysinline {
entry:
  %present = icmp ne ptr %base, null
  %inside = icmp ult i64 %address, %size
  %remaining = sub i64 %size, %address
  %fits = icmp ule i64 %count, %remaining
  %range = and i1 %inside, %fits
  %valid = and i1 %present, %range
  br i1 %valid, label %fast, label %slow
fast:
  %physical = getelementptr i8, ptr %base, i64 %address
  ret ptr %physical
slow:
  %resolved = call ptr @chali_primitive_address(i64 %address, i64 %count)
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
; AOT data image: original=224 stored=224 rows=10/10 segments=1
@chali_data_layout = constant [161 x i8] c"\43\48\4C\59\01\0A\0A\61\73\74\5F\64\61\74\61\5F\30\08\03\06\08\0A\61\73\74\5F\64\61\74\61\5F\31\10\03\07\08\0A\61\73\74\5F\64\61\74\61\5F\32\18\03\06\08\0A\61\73\74\5F\64\61\74\61\5F\33\20\03\35\08\0A\61\73\74\5F\64\61\74\61\5F\34\58\03\1C\08\0A\61\73\74\5F\64\61\74\61\5F\35\78\03\1C\08\0A\61\73\74\5F\64\61\74\61\5F\36\98\01\03\16\08\0A\61\73\74\5F\64\61\74\61\5F\37\B0\01\03\17\08\0A\61\73\74\5F\64\61\74\61\5F\38\C8\01\03\08\08\0A\61\73\74\5F\64\61\74\61\5F\39\D0\01\03\08\08\00"
@chali_data_image = constant [224 x i8] c"\00\00\00\00\00\00\00\00\66\69\72\73\74\00\00\00\73\65\63\6F\6E\64\00\00\74\68\69\72\64\00\00\00\62\6F\75\6E\64\65\64\20\62\75\66\66\65\72\20\67\72\6F\77\74\68\20\61\6E\64\20\72\65\74\61\69\6E\65\64\20\4C\69\73\74\20\63\61\70\61\63\69\74\79\20\4F\4B\0A\00\00\00\00\E7\BC\93\E5\86\B2\E5\8C\BA\E5\AE\B9\E9\87\8F\E5\8F\82\E6\95\B0\E6\97\A0\E6\95\88\00\00\00\00\00\E7\BC\93\E5\86\B2\E5\8C\BA\E5\AE\B9\E9\87\8F\E8\B6\85\E5\87\BA\E4\B8\8A\E9\99\90\00\00\00\00\00\53\74\72\69\6E\67\EF\BC\9A\E7\BB\93\E6\9E\9C\E8\BF\87\E9\95\BF\00\00\00\61\73\73\65\72\74\28\78\29\20\E7\9A\84\20\78\20\E4\B8\BA\E5\81\87\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00"
@chali_data_segment_count = constant i64 1
@chali_data_segments = constant [1 x { i64, i64, i64 }] [{ i64, i64, i64 } { i64 0, i64 0, i64 224 }]
@chali_data_size = constant i64 224
@chali_heap_size = constant i64 536870912
@chali_allocator_kind = constant i32 1
@source_file0 = private constant [19 x i8] c"\3C\E5\86\85\E5\BB\BA\20\73\74\64\3A\3A\6C\61\6E\67\3E\00"
@source_file1 = private constant [37 x i8] c"\74\65\73\74\73\2F\66\69\78\74\75\72\65\73\2F\61\6F\74\5F\62\75\66\66\65\72\5F\67\72\6F\77\74\68\2E\63\68\6C\00"
define void @chali_code_anchor() uwtable section "chali_code" {
entry:
  ret void
}
@trace_anchor_name = private constant [1 x i8] zeroinitializer
@trace_name0 = private constant [28 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\5F\5F\62\6F\6F\74\53\74\72\69\6E\67\42\79\74\65\00"
@trace_name1 = private constant [30 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\5F\5F\62\6F\6F\74\53\74\72\69\6E\67\4C\65\6E\67\74\68\00"
@trace_name2 = private constant [30 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\5F\5F\62\6F\6F\74\53\74\72\69\6E\67\42\75\66\66\65\72\00"
@trace_name3 = private constant [33 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\5F\5F\62\6F\6F\74\53\74\72\69\6E\67\43\6F\70\79\42\79\74\65\73\00"
@trace_name4 = private constant [28 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\5F\5F\62\6F\6F\74\53\74\72\69\6E\67\43\6F\70\79\00"
@trace_name5 = private constant [28 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\5F\5F\62\6F\6F\74\53\74\72\69\6E\67\44\72\6F\70\00"
@trace_name6 = private constant [31 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\5F\5F\62\6F\6F\74\53\74\72\69\6E\67\43\6F\6D\70\61\72\65\00"
@trace_name7 = private constant [23 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\61\73\73\65\72\74\24\62\6F\6F\6C\00"
@trace_name8 = private constant [41 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\61\73\73\65\72\74\24\62\6F\6F\6C\24\73\74\64\3A\3A\6C\61\6E\67\3A\3A\53\74\72\69\6E\67\00"
@trace_name9 = private constant [32 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\5F\5F\62\6F\6F\74\42\75\66\66\65\72\43\61\70\61\63\69\74\79\00"
@trace_name10 = private constant [29 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\5F\5F\62\6F\6F\74\4C\69\73\74\52\65\73\65\72\76\65\00"
@trace_name11 = private constant [28 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\5F\5F\62\6F\6F\74\4C\69\73\74\52\65\6D\6F\76\65\00"
@trace_name12 = private constant [27 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\5F\5F\62\6F\6F\74\4C\69\73\74\43\6C\65\61\72\00"
@trace_name13 = private constant [26 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\5F\5F\62\6F\6F\74\4C\69\73\74\43\6F\70\79\00"
@trace_name14 = private constant [27 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\5F\5F\62\6F\6F\74\4C\69\73\74\50\75\73\68\49\00"
@trace_name15 = private constant [26 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\5F\5F\62\6F\6F\74\4C\69\73\74\50\6F\70\49\00"
@trace_name16 = private constant [26 x i8] c"\70\72\6F\67\72\61\6D\3A\3A\72\65\66\65\72\65\6E\63\65\41\64\64\72\65\73\73\00"
@trace_name17 = private constant [5 x i8] c"\6D\61\69\6E\00"
@trace_name18 = private constant [37 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\70\6F\69\6E\74\65\72\46\72\6F\6D\41\64\64\72\65\73\73\5F\5F\69\6E\74\36\34\00"
@trace_name19 = private constant [37 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\70\6F\69\6E\74\65\72\46\72\6F\6D\41\64\64\72\65\73\73\5F\5F\75\69\6E\74\38\00"
@trace_name20 = private constant [20 x i8] c"\5F\5F\63\68\61\6C\69\5F\63\6F\70\79\5F\73\74\72\75\63\74\00"
@chali_trace_count = constant i64 22
@chali_trace_functions = constant [22 x ptr] [ptr @chali_code_anchor, ptr @f_v49_std__lang____bootStringByte, ptr @f_v56_std__lang____bootStringLength, ptr @f_v57_std__lang____bootStringBuffer, ptr @f_v58_std__lang____bootStringCopyBytes, ptr @f_v60_std__lang____bootStringCopy, ptr @f_v61_std__lang____bootStringDrop, ptr @f_v64_std__lang____bootStringCompare, ptr @f_v162_std__lang__assert_bool, ptr @f_v163_std__lang__assert_bool_std__lang__String, ptr @f_v169_std__lang____bootBufferCapacity, ptr @f_v170_std__lang____bootListReserve, ptr @f_v171_std__lang____bootListRemove, ptr @f_v172_std__lang____bootListClear, ptr @f_v173_std__lang____bootListCopy, ptr @f_v174_std__lang____bootListPushI, ptr @f_v176_std__lang____bootListPopI, ptr @f_v235_program__referenceAddress, ptr @f_main, ptr @f_v271_std__lang__pointerFromAddress__int64, ptr @f_v273_std__lang__pointerFromAddress__uint8, ptr @f_v468___chali_copy_struct]
@chali_trace_names = constant [22 x ptr] [ptr @trace_anchor_name, ptr @trace_name0, ptr @trace_name1, ptr @trace_name2, ptr @trace_name3, ptr @trace_name4, ptr @trace_name5, ptr @trace_name6, ptr @trace_name7, ptr @trace_name8, ptr @trace_name9, ptr @trace_name10, ptr @trace_name11, ptr @trace_name12, ptr @trace_name13, ptr @trace_name14, ptr @trace_name15, ptr @trace_name16, ptr @trace_name17, ptr @trace_name18, ptr @trace_name19, ptr @trace_name20]
@native_name1 = private constant [17 x i8] c"\5F\5F\73\74\64\5F\6D\6F\76\65\5F\62\65\67\69\6E\00"
@native_name2 = private constant [15 x i8] c"\5F\5F\73\74\64\5F\6D\6F\76\65\5F\65\6E\64\00"
@native_name0 = private constant [12 x i8] c"\5F\5F\73\74\64\5F\70\61\6E\69\63\00"
@chali_abi_lit0 = private constant [52 x i8] c"\62\6F\75\6E\64\65\64\20\62\75\66\66\65\72\20\67\72\6F\77\74\68\20\61\6E\64\20\72\65\74\61\69\6E\65\64\20\4C\69\73\74\20\63\61\70\61\63\69\74\79\20\4F\4B\0A"
define void @chali_reset_slots() {
entry:
  store i8 0, ptr @chali_return_float
  store ptr null, ptr @chali_source_function
  store ptr null, ptr @chali_source_file
  store i64 0, ptr @chali_source_line
  ret void
}
define i1 @chali_program_abi_bind(ptr %registry, ptr %error) {
entry:
  ret i1 true
}
; AOT standard modules:
define i1 @chali_program_std_bind(ptr %registry, ptr %error) {
entry:
  ret i1 true
}
define internal i64 @f_v49_std__lang____bootStringByte(i64 %p0, i64 %p1) alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [2 x i64]
  %native_argument_types = alloca [2 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v471___ast_49___chali_ret_v_652 = alloca i64
  store i64 0, ptr %s_v471___ast_49___chali_ret_v_652
  %t_v471___ast_49___chali_ret_v_652 = alloca i8
  store i8 0, ptr %t_v471___ast_49___chali_ret_v_652
  %s_v472___ast_49_address = alloca i64
  store i64 0, ptr %s_v472___ast_49_address
  %t_v472___ast_49_address = alloca i8
  store i8 0, ptr %t_v472___ast_49_address
  %s_v473___ast_49_index_114 = alloca i64
  store i64 0, ptr %s_v473___ast_49_index_114
  %t_v473___ast_49_index_114 = alloca i8
  store i8 0, ptr %t_v473___ast_49_index_114
  %s_v474___ast_49_value_651 = alloca i64
  store i64 0, ptr %s_v474___ast_49_value_651
  %t_v474___ast_49_value_651 = alloca i8
  store i8 0, ptr %t_v474___ast_49_value_651
  store i64 %p0, ptr %s_v472___ast_49_address
  store i64 %p1, ptr %s_v473___ast_49_index_114
  %r0 = load i64, ptr %s_v472___ast_49_address
  %r1 = load i8, ptr %t_v472___ast_49_address
  %r2 = load i64, ptr %s_v473___ast_49_index_114
  %r3 = load i8, ptr %t_v473___ast_49_index_114
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 13, ptr @chali_source_line
  %r4 = add i64 %r0, %r2
  %r5 = call i64 @chali_require_reference(i64 %r4)
  store i64 %r5, ptr %s_v474___ast_49_value_651
  store i8 0, ptr %t_v474___ast_49_value_651
  %r6 = load i64, ptr %s_v474___ast_49_value_651
  %r7 = load i8, ptr %t_v474___ast_49_value_651
  %r8 = call i64 @chali_require_reference(i64 %r6)
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 14, ptr @chali_source_line
  %r9 = call ptr @chali_primitive_address(i64 %r8, i64 1)
  %r10 = load i8, ptr %r9, align 1
  %r11 = zext i8 %r10 to i64
  store i64 %r11, ptr %s_v471___ast_49___chali_ret_v_652
  store i8 0, ptr %t_v471___ast_49___chali_ret_v_652
  %r12 = load i64, ptr %s_v471___ast_49___chali_ret_v_652
  %r13 = load i8, ptr %t_v471___ast_49___chali_ret_v_652
  store i8 %r13, ptr @chali_return_float
  ret i64 %r12
}
define internal i64 @f_v56_std__lang____bootStringLength(i64 %p0) alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [2 x i64]
  %native_argument_types = alloca [2 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v475___ast_56___chali_ret_v_715 = alloca i64
  store i64 0, ptr %s_v475___ast_56___chali_ret_v_715
  %t_v475___ast_56___chali_ret_v_715 = alloca i8
  store i8 0, ptr %t_v475___ast_56___chali_ret_v_715
  %s_v476___ast_56_address_130 = alloca i64
  store i64 0, ptr %s_v476___ast_56_address_130
  %t_v476___ast_56_address_130 = alloca i8
  store i8 0, ptr %t_v476___ast_56_address_130
  store i64 %p0, ptr %s_v476___ast_56_address_130
  %r0 = load i64, ptr %s_v476___ast_56_address_130
  %r1 = load i8, ptr %t_v476___ast_56_address_130
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 161, ptr @chali_source_line
  %r2 = call i64 @chali_host_string_storage(i64 0, i64 %r0)
  store i64 %r2, ptr %s_v475___ast_56___chali_ret_v_715
  store i8 0, ptr %t_v475___ast_56___chali_ret_v_715
  %r3 = load i64, ptr %s_v475___ast_56___chali_ret_v_715
  %r4 = load i8, ptr %t_v475___ast_56___chali_ret_v_715
  store i8 %r4, ptr @chali_return_float
  ret i64 %r3
}
define internal i64 @f_v57_std__lang____bootStringBuffer(i64 %p0) uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [4 x i64]
  %native_argument_types = alloca [4 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v477___ast_57___chali_move_source_717 = alloca i64
  store i64 0, ptr %s_v477___ast_57___chali_move_source_717
  %t_v477___ast_57___chali_move_source_717 = alloca i8
  store i8 0, ptr %t_v477___ast_57___chali_move_source_717
  %s_v478___ast_57___chali_ret_v_718 = alloca i64
  store i64 0, ptr %s_v478___ast_57___chali_ret_v_718
  %t_v478___ast_57___chali_ret_v_718 = alloca i8
  store i8 0, ptr %t_v478___ast_57___chali_ret_v_718
  %s_v479___ast_57___chali_tmp_agg_716 = alloca i64
  store i64 0, ptr %s_v479___ast_57___chali_tmp_agg_716
  %t_v479___ast_57___chali_tmp_agg_716 = alloca i8
  store i8 0, ptr %t_v479___ast_57___chali_tmp_agg_716
  %s_v480___ast_57_header = alloca i64
  store i64 0, ptr %s_v480___ast_57_header
  %t_v480___ast_57_header = alloca i8
  store i8 0, ptr %t_v480___ast_57_header
  %s_v481___ast_57_length_131 = alloca i64
  store i64 0, ptr %s_v481___ast_57_length_131
  %t_v481___ast_57_length_131 = alloca i8
  store i8 0, ptr %t_v481___ast_57_length_131
  %s_v482___ast_57_result = alloca i64
  store i64 0, ptr %s_v482___ast_57_result
  %t_v482___ast_57_result = alloca i8
  store i8 0, ptr %t_v482___ast_57_result
  store i64 %p0, ptr %s_v481___ast_57_length_131
  %r0 = load i64, ptr %s_v481___ast_57_length_131
  %r1 = load i8, ptr %t_v481___ast_57_length_131
  store ptr @f_v57_std__lang____bootStringBuffer, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 165, ptr @chali_source_line
  %r2 = icmp sge i64 %r0, 0
  %r3 = zext i1 %r2 to i64
  %r4 = icmp ne i64 %r3, 0
  br i1 %r4, label %ast_block_1, label %ast_block_0
ast_block_0:
  br label %ast_block_2
ast_block_1:
  %r5 = load i64, ptr %s_v481___ast_57_length_131
  %r6 = load i8, ptr %t_v481___ast_57_length_131
  store ptr @f_v57_std__lang____bootStringBuffer, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 165, ptr @chali_source_line
  %r7 = icmp sle i64 %r5, 9223372036854775799
  %r8 = zext i1 %r7 to i64
  %r9 = icmp ne i64 %r8, 0
  br label %ast_block_3
ast_block_3:
  br label %ast_block_2
ast_block_2:
  %r10 = phi i1 [ false, %ast_block_0 ], [ %r9, %ast_block_3 ]
  %r11 = zext i1 %r10 to i64
  %r12 = icmp ne i64 %r11, 0
  %r13 = xor i1 %r12, true
  %r14 = zext i1 %r13 to i64
  %r15 = icmp ne i64 %r14, 0
  br i1 %r15, label %ast_block_4, label %ast_block_5
ast_block_4:
  store ptr @f_v57_std__lang____bootStringBuffer, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 165, ptr @chali_source_line
  %r16 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 152, ptr %r16
  %r17 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 0, ptr %r17
  %r18 = call i64 @chali_host_call(ptr @native_name0, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r19 = call i8 @chali_host_result_float()
  br label %ast_block_6
ast_block_5:
  br label %ast_block_6
ast_block_6:
  store ptr @f_v57_std__lang____bootStringBuffer, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 166, ptr @chali_source_line
  %r20 = load i64, ptr %s_v481___ast_57_length_131
  %r21 = load i8, ptr %t_v481___ast_57_length_131
  store ptr @f_v57_std__lang____bootStringBuffer, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 166, ptr @chali_source_line
  %r22 = add i64 %r20, 8
  %r23 = call i64 @chali_alloc(i64 %r22)
  store i64 %r23, ptr %s_v482___ast_57_result
  store i8 0, ptr %t_v482___ast_57_result
  %r24 = call i64 @chali_alloc(i64 8)
  store i64 %r24, ptr %s_v479___ast_57___chali_tmp_agg_716
  store i8 0, ptr %t_v479___ast_57___chali_tmp_agg_716
  %r25 = load i64, ptr %s_v479___ast_57___chali_tmp_agg_716
  %r26 = load i8, ptr %t_v479___ast_57___chali_tmp_agg_716
  call void @chali_host_memory_prepare(i64 8, i64 %r25, i64 0, i64 8, ptr %raw_memory_output, ptr %raw_memory_input)
  %r27 = load ptr, ptr %raw_memory_output
  %r28 = trunc i64 0 to i8
  call void @llvm.memset.p0.i64(ptr %r27, i8 %r28, i64 8, i1 false)
  %r29 = load i64, ptr %s_v479___ast_57___chali_tmp_agg_716
  %r30 = load i8, ptr %t_v479___ast_57___chali_tmp_agg_716
  %r31 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r29, ptr %r31
  %r32 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r30, ptr %r32
  %r33 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 8, ptr %r33
  %r34 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r34
  %r35 = call i64 @chali_host_call(ptr @native_name1, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r36 = call i8 @chali_host_result_float()
  %r37 = load i64, ptr %s_v482___ast_57_result
  %r38 = load i8, ptr %t_v482___ast_57_result
  store ptr @f_v57_std__lang____bootStringBuffer, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 167, ptr @chali_source_line
  %r39 = call i64 @f_v271_std__lang__pointerFromAddress__int64(i64 %r37)
  %r40 = load i8, ptr @chali_return_float
  store i64 %r39, ptr %s_v477___ast_57___chali_move_source_717
  store i8 %r40, ptr %t_v477___ast_57___chali_move_source_717
  %r41 = load i64, ptr %s_v479___ast_57___chali_tmp_agg_716
  %r42 = load i8, ptr %t_v479___ast_57___chali_tmp_agg_716
  %r43 = load i64, ptr %s_v477___ast_57___chali_move_source_717
  %r44 = load i8, ptr %t_v477___ast_57___chali_move_source_717
  %r45 = call i64 @chali_host_copy_words(i64 %r41, i64 %r43, i64 1)
  %r46 = load i64, ptr %s_v479___ast_57___chali_tmp_agg_716
  %r47 = load i8, ptr %t_v479___ast_57___chali_tmp_agg_716
  %r48 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r46, ptr %r48
  %r49 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r47, ptr %r49
  %r50 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 8, ptr %r50
  %r51 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r51
  %r52 = call i64 @chali_host_call(ptr @native_name1, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r53 = call i8 @chali_host_result_float()
  %r54 = load i64, ptr %s_v479___ast_57___chali_tmp_agg_716
  %r55 = load i8, ptr %t_v479___ast_57___chali_tmp_agg_716
  %r56 = load i64, ptr %s_v477___ast_57___chali_move_source_717
  %r57 = load i8, ptr %t_v477___ast_57___chali_move_source_717
  %r58 = icmp ne i64 %r54, %r56
  %r59 = zext i1 %r58 to i64
  %r60 = icmp ne i64 %r59, 0
  br i1 %r60, label %ast_block_7, label %ast_block_8
ast_block_7:
  %r61 = load i64, ptr %s_v477___ast_57___chali_move_source_717
  %r62 = load i8, ptr %t_v477___ast_57___chali_move_source_717
  %r63 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r61, ptr %r63
  %r64 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r62, ptr %r64
  %r65 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 8, ptr %r65
  %r66 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r66
  %r67 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 49, ptr %r67
  %r68 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r68
  %r69 = call i64 @chali_host_call(ptr @native_name2, i64 3, ptr %indirect_arguments, ptr %native_argument_types)
  %r70 = call i8 @chali_host_result_float()
  br label %ast_block_9
ast_block_8:
  br label %ast_block_9
ast_block_9:
  %r71 = load i64, ptr %s_v479___ast_57___chali_tmp_agg_716
  %r72 = load i8, ptr %t_v479___ast_57___chali_tmp_agg_716
  %r73 = call ptr @chali_primitive_address(i64 %r71, i64 8)
  %r74 = load i64, ptr %r73, align 1
  %r75 = call i64 @chali_require_reference(i64 %r74)
  store i64 %r75, ptr %s_v480___ast_57_header
  store i8 0, ptr %t_v480___ast_57_header
  %r76 = load i64, ptr %s_v481___ast_57_length_131
  %r77 = load i8, ptr %t_v481___ast_57_length_131
  %r78 = load i64, ptr %s_v480___ast_57_header
  %r79 = load i8, ptr %t_v480___ast_57_header
  %r80 = call i64 @chali_require_reference(i64 %r78)
  %r81 = call ptr @chali_primitive_address(i64 %r80, i64 8)
  store i64 %r76, ptr %r81, align 1
  %r82 = load i64, ptr %s_v482___ast_57_result
  %r83 = load i8, ptr %t_v482___ast_57_result
  store i64 %r82, ptr %s_v478___ast_57___chali_ret_v_718
  store i8 %r83, ptr %t_v478___ast_57___chali_ret_v_718
  %r84 = load i64, ptr %s_v479___ast_57___chali_tmp_agg_716
  %r85 = load i8, ptr %t_v479___ast_57___chali_tmp_agg_716
  %r86 = icmp ne i64 %r84, 0
  %r87 = zext i1 %r86 to i64
  %r88 = icmp ne i64 %r87, 0
  br i1 %r88, label %ast_block_10, label %ast_block_11
ast_block_10:
  %r89 = load i64, ptr %s_v479___ast_57___chali_tmp_agg_716
  %r90 = load i8, ptr %t_v479___ast_57___chali_tmp_agg_716
  call void @chali_owned_drop(i64 %r89)
  br label %ast_block_12
ast_block_11:
  br label %ast_block_12
ast_block_12:
  store i64 0, ptr %s_v479___ast_57___chali_tmp_agg_716
  store i8 0, ptr %t_v479___ast_57___chali_tmp_agg_716
  %r91 = load i64, ptr %s_v478___ast_57___chali_ret_v_718
  %r92 = load i8, ptr %t_v478___ast_57___chali_ret_v_718
  store i8 %r92, ptr @chali_return_float
  ret i64 %r91
ast_block_13:
  %r93 = load i64, ptr %s_v479___ast_57___chali_tmp_agg_716
  %r94 = load i8, ptr %t_v479___ast_57___chali_tmp_agg_716
  %r95 = icmp ne i64 %r93, 0
  %r96 = zext i1 %r95 to i64
  %r97 = icmp ne i64 %r96, 0
  br i1 %r97, label %ast_block_14, label %ast_block_15
ast_block_14:
  %r98 = load i64, ptr %s_v479___ast_57___chali_tmp_agg_716
  %r99 = load i8, ptr %t_v479___ast_57___chali_tmp_agg_716
  call void @chali_owned_drop(i64 %r98)
  br label %ast_block_16
ast_block_15:
  br label %ast_block_16
ast_block_16:
  store i64 0, ptr %s_v479___ast_57___chali_tmp_agg_716
  store i8 0, ptr %t_v479___ast_57___chali_tmp_agg_716
  store i8 0, ptr @chali_return_float
  ret i64 0
ast_label_v483___chali_cleanup_std__lang____bootStringBuffer:
  %r100 = load i64, ptr %s_v479___ast_57___chali_tmp_agg_716
  %r101 = load i8, ptr %t_v479___ast_57___chali_tmp_agg_716
  %r102 = icmp ne i64 %r100, 0
  %r103 = zext i1 %r102 to i64
  %r104 = icmp ne i64 %r103, 0
  br i1 %r104, label %ast_block_17, label %ast_block_18
ast_block_17:
  %r105 = load i64, ptr %s_v479___ast_57___chali_tmp_agg_716
  %r106 = load i8, ptr %t_v479___ast_57___chali_tmp_agg_716
  call void @chali_owned_drop(i64 %r105)
  br label %ast_block_19
ast_block_18:
  br label %ast_block_19
ast_block_19:
  store i64 0, ptr %s_v479___ast_57___chali_tmp_agg_716
  store i8 0, ptr %t_v479___ast_57___chali_tmp_agg_716
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define internal i64 @f_v58_std__lang____bootStringCopyBytes(i64 %p0, i64 %p1, i64 %p2) uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [4 x i64]
  %native_argument_types = alloca [4 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v484___ast_58___chali_memory_argument = alloca i64
  store i64 0, ptr %s_v484___ast_58___chali_memory_argument
  %t_v484___ast_58___chali_memory_argument = alloca i8
  store i8 0, ptr %t_v484___ast_58___chali_memory_argument
  %s_v485___ast_58___chali_memory_argument_723 = alloca i64
  store i64 0, ptr %s_v485___ast_58___chali_memory_argument_723
  %t_v485___ast_58___chali_memory_argument_723 = alloca i8
  store i8 0, ptr %t_v485___ast_58___chali_memory_argument_723
  %s_v486___ast_58___chali_memory_argument_724 = alloca i64
  store i64 0, ptr %s_v486___ast_58___chali_memory_argument_724
  %t_v486___ast_58___chali_memory_argument_724 = alloca i8
  store i8 0, ptr %t_v486___ast_58___chali_memory_argument_724
  %s_v487___ast_58___chali_move_source_720 = alloca i64
  store i64 0, ptr %s_v487___ast_58___chali_move_source_720
  %t_v487___ast_58___chali_move_source_720 = alloca i8
  store i8 0, ptr %t_v487___ast_58___chali_move_source_720
  %s_v488___ast_58___chali_move_source_722 = alloca i64
  store i64 0, ptr %s_v488___ast_58___chali_move_source_722
  %t_v488___ast_58___chali_move_source_722 = alloca i8
  store i8 0, ptr %t_v488___ast_58___chali_move_source_722
  %s_v489___ast_58___chali_tmp_agg_719 = alloca i64
  store i64 0, ptr %s_v489___ast_58___chali_tmp_agg_719
  %t_v489___ast_58___chali_tmp_agg_719 = alloca i8
  store i8 0, ptr %t_v489___ast_58___chali_tmp_agg_719
  %s_v490___ast_58___chali_tmp_agg_721 = alloca i64
  store i64 0, ptr %s_v490___ast_58___chali_tmp_agg_721
  %t_v490___ast_58___chali_tmp_agg_721 = alloca i8
  store i8 0, ptr %t_v490___ast_58___chali_tmp_agg_721
  %s_v491___ast_58_destination = alloca i64
  store i64 0, ptr %s_v491___ast_58_destination
  %t_v491___ast_58_destination = alloca i8
  store i8 0, ptr %t_v491___ast_58_destination
  %s_v492___ast_58_length_132 = alloca i64
  store i64 0, ptr %s_v492___ast_58_length_132
  %t_v492___ast_58_length_132 = alloca i8
  store i8 0, ptr %t_v492___ast_58_length_132
  %s_v493___ast_58_source = alloca i64
  store i64 0, ptr %s_v493___ast_58_source
  %t_v493___ast_58_source = alloca i8
  store i8 0, ptr %t_v493___ast_58_source
  store i64 %p0, ptr %s_v491___ast_58_destination
  store i64 %p1, ptr %s_v493___ast_58_source
  store i64 %p2, ptr %s_v492___ast_58_length_132
  %r0 = load i64, ptr %s_v492___ast_58_length_132
  %r1 = load i8, ptr %t_v492___ast_58_length_132
  store ptr @f_v58_std__lang____bootStringCopyBytes, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 172, ptr @chali_source_line
  %r2 = icmp sgt i64 %r0, 0
  %r3 = zext i1 %r2 to i64
  %r4 = icmp ne i64 %r3, 0
  br i1 %r4, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r5 = call i64 @chali_alloc(i64 8)
  store i64 %r5, ptr %s_v489___ast_58___chali_tmp_agg_719
  store i8 0, ptr %t_v489___ast_58___chali_tmp_agg_719
  %r6 = load i64, ptr %s_v489___ast_58___chali_tmp_agg_719
  %r7 = load i8, ptr %t_v489___ast_58___chali_tmp_agg_719
  call void @chali_host_memory_prepare(i64 8, i64 %r6, i64 0, i64 8, ptr %raw_memory_output, ptr %raw_memory_input)
  %r8 = load ptr, ptr %raw_memory_output
  %r9 = trunc i64 0 to i8
  call void @llvm.memset.p0.i64(ptr %r8, i8 %r9, i64 8, i1 false)
  %r10 = load i64, ptr %s_v489___ast_58___chali_tmp_agg_719
  %r11 = load i8, ptr %t_v489___ast_58___chali_tmp_agg_719
  %r12 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r10, ptr %r12
  %r13 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r11, ptr %r13
  %r14 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 8, ptr %r14
  %r15 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r15
  %r16 = call i64 @chali_host_call(ptr @native_name1, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r17 = call i8 @chali_host_result_float()
  %r18 = call i64 @chali_alloc(i64 8)
  store i64 %r18, ptr %s_v490___ast_58___chali_tmp_agg_721
  store i8 0, ptr %t_v490___ast_58___chali_tmp_agg_721
  %r19 = load i64, ptr %s_v490___ast_58___chali_tmp_agg_721
  %r20 = load i8, ptr %t_v490___ast_58___chali_tmp_agg_721
  call void @chali_host_memory_prepare(i64 8, i64 %r19, i64 0, i64 8, ptr %raw_memory_output, ptr %raw_memory_input)
  %r21 = load ptr, ptr %raw_memory_output
  %r22 = trunc i64 0 to i8
  call void @llvm.memset.p0.i64(ptr %r21, i8 %r22, i64 8, i1 false)
  %r23 = load i64, ptr %s_v490___ast_58___chali_tmp_agg_721
  %r24 = load i8, ptr %t_v490___ast_58___chali_tmp_agg_721
  %r25 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r23, ptr %r25
  %r26 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r24, ptr %r26
  %r27 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 8, ptr %r27
  %r28 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r28
  %r29 = call i64 @chali_host_call(ptr @native_name1, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r30 = call i8 @chali_host_result_float()
  %r31 = load i64, ptr %s_v491___ast_58_destination
  %r32 = load i8, ptr %t_v491___ast_58_destination
  store ptr @f_v58_std__lang____bootStringCopyBytes, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 172, ptr @chali_source_line
  %r33 = call i64 @f_v273_std__lang__pointerFromAddress__uint8(i64 %r31)
  %r34 = load i8, ptr @chali_return_float
  store i64 %r33, ptr %s_v487___ast_58___chali_move_source_720
  store i8 %r34, ptr %t_v487___ast_58___chali_move_source_720
  %r35 = load i64, ptr %s_v489___ast_58___chali_tmp_agg_719
  %r36 = load i8, ptr %t_v489___ast_58___chali_tmp_agg_719
  %r37 = load i64, ptr %s_v487___ast_58___chali_move_source_720
  %r38 = load i8, ptr %t_v487___ast_58___chali_move_source_720
  %r39 = call i64 @chali_host_copy_words(i64 %r35, i64 %r37, i64 1)
  %r40 = load i64, ptr %s_v489___ast_58___chali_tmp_agg_719
  %r41 = load i8, ptr %t_v489___ast_58___chali_tmp_agg_719
  %r42 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r40, ptr %r42
  %r43 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r41, ptr %r43
  %r44 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 8, ptr %r44
  %r45 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r45
  %r46 = call i64 @chali_host_call(ptr @native_name1, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r47 = call i8 @chali_host_result_float()
  %r48 = load i64, ptr %s_v489___ast_58___chali_tmp_agg_719
  %r49 = load i8, ptr %t_v489___ast_58___chali_tmp_agg_719
  %r50 = load i64, ptr %s_v487___ast_58___chali_move_source_720
  %r51 = load i8, ptr %t_v487___ast_58___chali_move_source_720
  %r52 = icmp ne i64 %r48, %r50
  %r53 = zext i1 %r52 to i64
  %r54 = icmp ne i64 %r53, 0
  br i1 %r54, label %ast_block_3, label %ast_block_4
ast_block_3:
  %r55 = load i64, ptr %s_v487___ast_58___chali_move_source_720
  %r56 = load i8, ptr %t_v487___ast_58___chali_move_source_720
  %r57 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r55, ptr %r57
  %r58 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r56, ptr %r58
  %r59 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 8, ptr %r59
  %r60 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r60
  %r61 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 47, ptr %r61
  %r62 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r62
  %r63 = call i64 @chali_host_call(ptr @native_name2, i64 3, ptr %indirect_arguments, ptr %native_argument_types)
  %r64 = call i8 @chali_host_result_float()
  br label %ast_block_5
ast_block_4:
  br label %ast_block_5
ast_block_5:
  %r65 = load i64, ptr %s_v489___ast_58___chali_tmp_agg_719
  %r66 = load i8, ptr %t_v489___ast_58___chali_tmp_agg_719
  store i64 %r65, ptr %s_v484___ast_58___chali_memory_argument
  store i8 %r66, ptr %t_v484___ast_58___chali_memory_argument
  %r67 = load i64, ptr %s_v493___ast_58_source
  %r68 = load i8, ptr %t_v493___ast_58_source
  store ptr @f_v58_std__lang____bootStringCopyBytes, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 172, ptr @chali_source_line
  %r69 = call i64 @f_v273_std__lang__pointerFromAddress__uint8(i64 %r67)
  %r70 = load i8, ptr @chali_return_float
  store i64 %r69, ptr %s_v488___ast_58___chali_move_source_722
  store i8 %r70, ptr %t_v488___ast_58___chali_move_source_722
  %r71 = load i64, ptr %s_v490___ast_58___chali_tmp_agg_721
  %r72 = load i8, ptr %t_v490___ast_58___chali_tmp_agg_721
  %r73 = load i64, ptr %s_v488___ast_58___chali_move_source_722
  %r74 = load i8, ptr %t_v488___ast_58___chali_move_source_722
  %r75 = call i64 @chali_host_copy_words(i64 %r71, i64 %r73, i64 1)
  %r76 = load i64, ptr %s_v490___ast_58___chali_tmp_agg_721
  %r77 = load i8, ptr %t_v490___ast_58___chali_tmp_agg_721
  %r78 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r76, ptr %r78
  %r79 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r77, ptr %r79
  %r80 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 8, ptr %r80
  %r81 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r81
  %r82 = call i64 @chali_host_call(ptr @native_name1, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r83 = call i8 @chali_host_result_float()
  %r84 = load i64, ptr %s_v490___ast_58___chali_tmp_agg_721
  %r85 = load i8, ptr %t_v490___ast_58___chali_tmp_agg_721
  %r86 = load i64, ptr %s_v488___ast_58___chali_move_source_722
  %r87 = load i8, ptr %t_v488___ast_58___chali_move_source_722
  %r88 = icmp ne i64 %r84, %r86
  %r89 = zext i1 %r88 to i64
  %r90 = icmp ne i64 %r89, 0
  br i1 %r90, label %ast_block_6, label %ast_block_7
ast_block_6:
  %r91 = load i64, ptr %s_v488___ast_58___chali_move_source_722
  %r92 = load i8, ptr %t_v488___ast_58___chali_move_source_722
  %r93 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r91, ptr %r93
  %r94 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r92, ptr %r94
  %r95 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 8, ptr %r95
  %r96 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r96
  %r97 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 47, ptr %r97
  %r98 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r98
  %r99 = call i64 @chali_host_call(ptr @native_name2, i64 3, ptr %indirect_arguments, ptr %native_argument_types)
  %r100 = call i8 @chali_host_result_float()
  br label %ast_block_8
ast_block_7:
  br label %ast_block_8
ast_block_8:
  %r101 = load i64, ptr %s_v490___ast_58___chali_tmp_agg_721
  %r102 = load i8, ptr %t_v490___ast_58___chali_tmp_agg_721
  store i64 %r101, ptr %s_v485___ast_58___chali_memory_argument_723
  store i8 %r102, ptr %t_v485___ast_58___chali_memory_argument_723
  %r103 = load i64, ptr %s_v492___ast_58_length_132
  %r104 = load i8, ptr %t_v492___ast_58_length_132
  store i64 %r103, ptr %s_v486___ast_58___chali_memory_argument_724
  store i8 %r104, ptr %t_v486___ast_58___chali_memory_argument_724
  %r105 = load i64, ptr %s_v484___ast_58___chali_memory_argument
  %r106 = load i8, ptr %t_v484___ast_58___chali_memory_argument
  %r107 = call ptr @chali_primitive_address(i64 %r105, i64 8)
  %r108 = load i64, ptr %r107, align 1
  %r109 = load i64, ptr %s_v485___ast_58___chali_memory_argument_723
  %r110 = load i8, ptr %t_v485___ast_58___chali_memory_argument_723
  %r111 = call ptr @chali_primitive_address(i64 %r109, i64 8)
  %r112 = load i64, ptr %r111, align 1
  %r113 = load i64, ptr %s_v486___ast_58___chali_memory_argument_724
  %r114 = load i8, ptr %t_v486___ast_58___chali_memory_argument_724
  call void @chali_host_memory_prepare(i64 7, i64 %r108, i64 %r112, i64 %r113, ptr %raw_memory_output, ptr %raw_memory_input)
  %r115 = load ptr, ptr %raw_memory_output
  %r116 = load ptr, ptr %raw_memory_input
  call void @llvm.memcpy.p0.p0.i64(ptr %r115, ptr %r116, i64 %r113, i1 false)
  %r117 = load i64, ptr %s_v490___ast_58___chali_tmp_agg_721
  %r118 = load i8, ptr %t_v490___ast_58___chali_tmp_agg_721
  %r119 = icmp ne i64 %r117, 0
  %r120 = zext i1 %r119 to i64
  %r121 = icmp ne i64 %r120, 0
  br i1 %r121, label %ast_block_9, label %ast_block_10
ast_block_9:
  %r122 = load i64, ptr %s_v490___ast_58___chali_tmp_agg_721
  %r123 = load i8, ptr %t_v490___ast_58___chali_tmp_agg_721
  call void @chali_owned_drop(i64 %r122)
  br label %ast_block_11
ast_block_10:
  br label %ast_block_11
ast_block_11:
  store i64 0, ptr %s_v490___ast_58___chali_tmp_agg_721
  store i8 0, ptr %t_v490___ast_58___chali_tmp_agg_721
  %r124 = load i64, ptr %s_v489___ast_58___chali_tmp_agg_719
  %r125 = load i8, ptr %t_v489___ast_58___chali_tmp_agg_719
  %r126 = icmp ne i64 %r124, 0
  %r127 = zext i1 %r126 to i64
  %r128 = icmp ne i64 %r127, 0
  br i1 %r128, label %ast_block_12, label %ast_block_13
ast_block_12:
  %r129 = load i64, ptr %s_v489___ast_58___chali_tmp_agg_719
  %r130 = load i8, ptr %t_v489___ast_58___chali_tmp_agg_719
  call void @chali_owned_drop(i64 %r129)
  br label %ast_block_14
ast_block_13:
  br label %ast_block_14
ast_block_14:
  store i64 0, ptr %s_v489___ast_58___chali_tmp_agg_719
  store i8 0, ptr %t_v489___ast_58___chali_tmp_agg_719
  br label %ast_block_2
ast_block_1:
  br label %ast_block_2
ast_block_2:
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define internal i64 @f_v60_std__lang____bootStringCopy(i64 %p0) uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [4 x i64]
  %native_argument_types = alloca [4 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v494___ast_60___chali_ret_v_728 = alloca i64
  store i64 0, ptr %s_v494___ast_60___chali_ret_v_728
  %t_v494___ast_60___chali_ret_v_728 = alloca i8
  store i8 0, ptr %t_v494___ast_60___chali_ret_v_728
  %s_v495___ast_60_length_726 = alloca i64
  store i64 0, ptr %s_v495___ast_60_length_726
  %t_v495___ast_60_length_726 = alloca i8
  store i8 0, ptr %t_v495___ast_60_length_726
  %s_v496___ast_60_result_727 = alloca i64
  store i64 0, ptr %s_v496___ast_60_result_727
  %t_v496___ast_60_result_727 = alloca i8
  store i8 0, ptr %t_v496___ast_60_result_727
  %s_v497___ast_60_source_133 = alloca i64
  store i64 0, ptr %s_v497___ast_60_source_133
  %t_v497___ast_60_source_133 = alloca i8
  store i8 0, ptr %t_v497___ast_60_source_133
  store i64 %p0, ptr %s_v497___ast_60_source_133
  %r0 = load i64, ptr %s_v497___ast_60_source_133
  %r1 = load i8, ptr %t_v497___ast_60_source_133
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 178, ptr @chali_source_line
  %r2 = call i64 @f_v56_std__lang____bootStringLength(i64 %r0)
  %r3 = load i8, ptr @chali_return_float
  store i64 %r2, ptr %s_v495___ast_60_length_726
  store i8 %r3, ptr %t_v495___ast_60_length_726
  %r4 = load i64, ptr %s_v495___ast_60_length_726
  %r5 = load i8, ptr %t_v495___ast_60_length_726
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 178, ptr @chali_source_line
  %r6 = call i64 @f_v57_std__lang____bootStringBuffer(i64 %r4)
  %r7 = load i8, ptr @chali_return_float
  store i64 %r6, ptr %s_v496___ast_60_result_727
  store i8 %r7, ptr %t_v496___ast_60_result_727
  %r8 = load i64, ptr %s_v496___ast_60_result_727
  %r9 = load i8, ptr %t_v496___ast_60_result_727
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 179, ptr @chali_source_line
  %r10 = add i64 %r8, 8
  %r11 = load i64, ptr %s_v497___ast_60_source_133
  %r12 = load i8, ptr %t_v497___ast_60_source_133
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 179, ptr @chali_source_line
  %r13 = call i64 @chali_host_string_storage(i64 1, i64 %r11)
  %r14 = load i64, ptr %s_v495___ast_60_length_726
  %r15 = load i8, ptr %t_v495___ast_60_length_726
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 179, ptr @chali_source_line
  %r16 = call i64 @f_v58_std__lang____bootStringCopyBytes(i64 %r10, i64 %r13, i64 %r14)
  %r17 = load i8, ptr @chali_return_float
  %r18 = load i64, ptr %s_v496___ast_60_result_727
  %r19 = load i8, ptr %t_v496___ast_60_result_727
  store i64 %r18, ptr %s_v494___ast_60___chali_ret_v_728
  store i8 %r19, ptr %t_v494___ast_60___chali_ret_v_728
  %r20 = load i64, ptr %s_v494___ast_60___chali_ret_v_728
  %r21 = load i8, ptr %t_v494___ast_60___chali_ret_v_728
  store i8 %r21, ptr @chali_return_float
  ret i64 %r20
}
define internal i64 @f_v61_std__lang____bootStringDrop(i64 %p0) alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [1 x i64]
  %native_argument_types = alloca [1 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v498___ast_61___chali_ret_v_729 = alloca i64
  store i64 0, ptr %s_v498___ast_61___chali_ret_v_729
  %t_v498___ast_61___chali_ret_v_729 = alloca i8
  store i8 0, ptr %t_v498___ast_61___chali_ret_v_729
  %s_v499___ast_61_address_134 = alloca i64
  store i64 0, ptr %s_v499___ast_61_address_134
  %t_v499___ast_61_address_134 = alloca i8
  store i8 0, ptr %t_v499___ast_61_address_134
  store i64 %p0, ptr %s_v499___ast_61_address_134
  %r0 = load i64, ptr %s_v499___ast_61_address_134
  %r1 = load i8, ptr %t_v499___ast_61_address_134
  call void @chali_owned_drop(i64 %r0)
  store i64 0, ptr %s_v498___ast_61___chali_ret_v_729
  store i8 0, ptr %t_v498___ast_61___chali_ret_v_729
  %r2 = load i64, ptr %s_v498___ast_61___chali_ret_v_729
  %r3 = load i8, ptr %t_v498___ast_61___chali_ret_v_729
  store i8 %r3, ptr @chali_return_float
  ret i64 %r2
}
define internal i64 @f_v64_std__lang____bootStringCompare(i64 %p0, i64 %p1) alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [3 x i64]
  %native_argument_types = alloca [3 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v500___ast_64___chali_ret_v_737 = alloca i64
  store i64 0, ptr %s_v500___ast_64___chali_ret_v_737
  %t_v500___ast_64___chali_ret_v_737 = alloca i8
  store i8 0, ptr %t_v500___ast_64___chali_ret_v_737
  %s_v501___ast_64___chali_ret_v_738 = alloca i64
  store i64 0, ptr %s_v501___ast_64___chali_ret_v_738
  %t_v501___ast_64___chali_ret_v_738 = alloca i8
  store i8 0, ptr %t_v501___ast_64___chali_ret_v_738
  %s_v502___ast_64___chali_ret_v_739 = alloca i64
  store i64 0, ptr %s_v502___ast_64___chali_ret_v_739
  %t_v502___ast_64___chali_ret_v_739 = alloca i8
  store i8 0, ptr %t_v502___ast_64___chali_ret_v_739
  %s_v503___ast_64___chali_ret_v_740 = alloca i64
  store i64 0, ptr %s_v503___ast_64___chali_ret_v_740
  %t_v503___ast_64___chali_ret_v_740 = alloca i8
  store i8 0, ptr %t_v503___ast_64___chali_ret_v_740
  %s_v504___ast_64___chali_ret_v_741 = alloca i64
  store i64 0, ptr %s_v504___ast_64___chali_ret_v_741
  %t_v504___ast_64___chali_ret_v_741 = alloca i8
  store i8 0, ptr %t_v504___ast_64___chali_ret_v_741
  %s_v505___ast_64_a_734 = alloca i64
  store i64 0, ptr %s_v505___ast_64_a_734
  %t_v505___ast_64_a_734 = alloca i8
  store i8 0, ptr %t_v505___ast_64_a_734
  %s_v506___ast_64_b_735 = alloca i64
  store i64 0, ptr %s_v506___ast_64_b_735
  %t_v506___ast_64_b_735 = alloca i8
  store i8 0, ptr %t_v506___ast_64_b_735
  %s_v507___ast_64_i_736 = alloca i64
  store i64 0, ptr %s_v507___ast_64_i_736
  %t_v507___ast_64_i_736 = alloca i8
  store i8 0, ptr %t_v507___ast_64_i_736
  %s_v508___ast_64_left_140 = alloca i64
  store i64 0, ptr %s_v508___ast_64_left_140
  %t_v508___ast_64_left_140 = alloca i8
  store i8 0, ptr %t_v508___ast_64_left_140
  %s_v509___ast_64_right_141 = alloca i64
  store i64 0, ptr %s_v509___ast_64_right_141
  %t_v509___ast_64_right_141 = alloca i8
  store i8 0, ptr %t_v509___ast_64_right_141
  %s_v510___ast_64_x = alloca i64
  store i64 0, ptr %s_v510___ast_64_x
  %t_v510___ast_64_x = alloca i8
  store i8 0, ptr %t_v510___ast_64_x
  %s_v511___ast_64_y = alloca i64
  store i64 0, ptr %s_v511___ast_64_y
  %t_v511___ast_64_y = alloca i8
  store i8 0, ptr %t_v511___ast_64_y
  store i64 %p0, ptr %s_v508___ast_64_left_140
  store i64 %p1, ptr %s_v509___ast_64_right_141
  %r0 = load i64, ptr %s_v508___ast_64_left_140
  %r1 = load i8, ptr %t_v508___ast_64_left_140
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 198, ptr @chali_source_line
  %r2 = call i64 @f_v56_std__lang____bootStringLength(i64 %r0)
  %r3 = load i8, ptr @chali_return_float
  store i64 %r2, ptr %s_v505___ast_64_a_734
  store i8 %r3, ptr %t_v505___ast_64_a_734
  %r4 = load i64, ptr %s_v509___ast_64_right_141
  %r5 = load i8, ptr %t_v509___ast_64_right_141
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 198, ptr @chali_source_line
  %r6 = call i64 @f_v56_std__lang____bootStringLength(i64 %r4)
  %r7 = load i8, ptr @chali_return_float
  store i64 %r6, ptr %s_v506___ast_64_b_735
  store i8 %r7, ptr %t_v506___ast_64_b_735
  store i64 0, ptr %s_v507___ast_64_i_736
  store i8 0, ptr %t_v507___ast_64_i_736
  %r8 = load i64, ptr %s_v508___ast_64_left_140
  %r9 = load i8, ptr %t_v508___ast_64_left_140
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 199, ptr @chali_source_line
  %r10 = call i64 @chali_host_string_storage(i64 1, i64 %r8)
  store i64 %r10, ptr %s_v508___ast_64_left_140
  store i8 0, ptr %t_v508___ast_64_left_140
  %r11 = load i64, ptr %s_v509___ast_64_right_141
  %r12 = load i8, ptr %t_v509___ast_64_right_141
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 199, ptr @chali_source_line
  %r13 = call i64 @chali_host_string_storage(i64 1, i64 %r11)
  store i64 %r13, ptr %s_v509___ast_64_right_141
  store i8 0, ptr %t_v509___ast_64_right_141
  br label %ast_label_v512___chali_loop_23
ast_label_v512___chali_loop_23:
  %r14 = load i64, ptr %s_v507___ast_64_i_736
  %r15 = load i8, ptr %t_v507___ast_64_i_736
  %r16 = load i64, ptr %s_v505___ast_64_a_734
  %r17 = load i8, ptr %t_v505___ast_64_a_734
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 200, ptr @chali_source_line
  %r18 = icmp slt i64 %r14, %r16
  %r19 = zext i1 %r18 to i64
  %r20 = icmp ne i64 %r19, 0
  br i1 %r20, label %ast_block_1, label %ast_block_0
ast_block_0:
  br label %ast_block_2
ast_block_1:
  %r21 = load i64, ptr %s_v507___ast_64_i_736
  %r22 = load i8, ptr %t_v507___ast_64_i_736
  %r23 = load i64, ptr %s_v506___ast_64_b_735
  %r24 = load i8, ptr %t_v506___ast_64_b_735
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 200, ptr @chali_source_line
  %r25 = icmp slt i64 %r21, %r23
  %r26 = zext i1 %r25 to i64
  %r27 = icmp ne i64 %r26, 0
  br label %ast_block_3
ast_block_3:
  br label %ast_block_2
ast_block_2:
  %r28 = phi i1 [ false, %ast_block_0 ], [ %r27, %ast_block_3 ]
  %r29 = zext i1 %r28 to i64
  %r30 = icmp ne i64 %r29, 0
  br i1 %r30, label %ast_block_4, label %ast_block_5
ast_block_4:
  %r31 = load i64, ptr %s_v508___ast_64_left_140
  %r32 = load i8, ptr %t_v508___ast_64_left_140
  %r33 = load i64, ptr %s_v507___ast_64_i_736
  %r34 = load i8, ptr %t_v507___ast_64_i_736
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 201, ptr @chali_source_line
  %r35 = call i64 @f_v49_std__lang____bootStringByte(i64 %r31, i64 %r33)
  %r36 = load i8, ptr @chali_return_float
  store i64 %r35, ptr %s_v510___ast_64_x
  store i8 %r36, ptr %t_v510___ast_64_x
  %r37 = load i64, ptr %s_v509___ast_64_right_141
  %r38 = load i8, ptr %t_v509___ast_64_right_141
  %r39 = load i64, ptr %s_v507___ast_64_i_736
  %r40 = load i8, ptr %t_v507___ast_64_i_736
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 201, ptr @chali_source_line
  %r41 = call i64 @f_v49_std__lang____bootStringByte(i64 %r37, i64 %r39)
  %r42 = load i8, ptr @chali_return_float
  store i64 %r41, ptr %s_v511___ast_64_y
  store i8 %r42, ptr %t_v511___ast_64_y
  %r43 = load i64, ptr %s_v510___ast_64_x
  %r44 = load i8, ptr %t_v510___ast_64_x
  %r45 = load i64, ptr %s_v511___ast_64_y
  %r46 = load i8, ptr %t_v511___ast_64_y
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 202, ptr @chali_source_line
  %r47 = icmp slt i64 %r43, %r45
  %r48 = zext i1 %r47 to i64
  %r49 = icmp ne i64 %r48, 0
  br i1 %r49, label %ast_block_7, label %ast_block_8
ast_block_7:
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 202, ptr @chali_source_line
  %r50 = sub i64 0, 1
  store i64 %r50, ptr %s_v500___ast_64___chali_ret_v_737
  store i8 0, ptr %t_v500___ast_64___chali_ret_v_737
  %r51 = load i64, ptr %s_v500___ast_64___chali_ret_v_737
  %r52 = load i8, ptr %t_v500___ast_64___chali_ret_v_737
  store i8 %r52, ptr @chali_return_float
  ret i64 %r51
ast_block_8:
  br label %ast_block_9
ast_block_9:
  %r53 = load i64, ptr %s_v510___ast_64_x
  %r54 = load i8, ptr %t_v510___ast_64_x
  %r55 = load i64, ptr %s_v511___ast_64_y
  %r56 = load i8, ptr %t_v511___ast_64_y
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 202, ptr @chali_source_line
  %r57 = icmp sgt i64 %r53, %r55
  %r58 = zext i1 %r57 to i64
  %r59 = icmp ne i64 %r58, 0
  br i1 %r59, label %ast_block_10, label %ast_block_11
ast_block_10:
  store i64 1, ptr %s_v501___ast_64___chali_ret_v_738
  store i8 0, ptr %t_v501___ast_64___chali_ret_v_738
  %r60 = load i64, ptr %s_v501___ast_64___chali_ret_v_738
  %r61 = load i8, ptr %t_v501___ast_64___chali_ret_v_738
  store i8 %r61, ptr @chali_return_float
  ret i64 %r60
ast_block_11:
  br label %ast_block_12
ast_block_12:
  %r62 = load i64, ptr %s_v507___ast_64_i_736
  %r63 = load i8, ptr %t_v507___ast_64_i_736
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 202, ptr @chali_source_line
  %r64 = add i64 %r62, 1
  store i64 %r64, ptr %s_v507___ast_64_i_736
  store i8 0, ptr %t_v507___ast_64_i_736
  br label %ast_block_6
ast_block_5:
  br label %ast_label_v513___chali_brk_24
ast_block_6:
  br label %ast_label_v512___chali_loop_23
ast_label_v513___chali_brk_24:
  %r65 = load i64, ptr %s_v505___ast_64_a_734
  %r66 = load i8, ptr %t_v505___ast_64_a_734
  %r67 = load i64, ptr %s_v506___ast_64_b_735
  %r68 = load i8, ptr %t_v506___ast_64_b_735
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 204, ptr @chali_source_line
  %r69 = icmp slt i64 %r65, %r67
  %r70 = zext i1 %r69 to i64
  %r71 = icmp ne i64 %r70, 0
  br i1 %r71, label %ast_block_13, label %ast_block_14
ast_block_13:
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 204, ptr @chali_source_line
  %r72 = sub i64 0, 1
  store i64 %r72, ptr %s_v502___ast_64___chali_ret_v_739
  store i8 0, ptr %t_v502___ast_64___chali_ret_v_739
  %r73 = load i64, ptr %s_v502___ast_64___chali_ret_v_739
  %r74 = load i8, ptr %t_v502___ast_64___chali_ret_v_739
  store i8 %r74, ptr @chali_return_float
  ret i64 %r73
ast_block_14:
  br label %ast_block_15
ast_block_15:
  %r75 = load i64, ptr %s_v505___ast_64_a_734
  %r76 = load i8, ptr %t_v505___ast_64_a_734
  %r77 = load i64, ptr %s_v506___ast_64_b_735
  %r78 = load i8, ptr %t_v506___ast_64_b_735
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 204, ptr @chali_source_line
  %r79 = icmp sgt i64 %r75, %r77
  %r80 = zext i1 %r79 to i64
  %r81 = icmp ne i64 %r80, 0
  br i1 %r81, label %ast_block_16, label %ast_block_17
ast_block_16:
  store i64 1, ptr %s_v503___ast_64___chali_ret_v_740
  store i8 0, ptr %t_v503___ast_64___chali_ret_v_740
  %r82 = load i64, ptr %s_v503___ast_64___chali_ret_v_740
  %r83 = load i8, ptr %t_v503___ast_64___chali_ret_v_740
  store i8 %r83, ptr @chali_return_float
  ret i64 %r82
ast_block_17:
  br label %ast_block_18
ast_block_18:
  store i64 0, ptr %s_v504___ast_64___chali_ret_v_741
  store i8 0, ptr %t_v504___ast_64___chali_ret_v_741
  %r84 = load i64, ptr %s_v504___ast_64___chali_ret_v_741
  %r85 = load i8, ptr %t_v504___ast_64___chali_ret_v_741
  store i8 %r85, ptr @chali_return_float
  ret i64 %r84
}
define internal i64 @f_v162_std__lang__assert_bool(i64 %p0) alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [2 x i64]
  %native_argument_types = alloca [2 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v514___ast_162_condition = alloca i64
  store i64 0, ptr %s_v514___ast_162_condition
  %t_v514___ast_162_condition = alloca i8
  store i8 0, ptr %t_v514___ast_162_condition
  store i64 %p0, ptr %s_v514___ast_162_condition
  %r0 = load i64, ptr %s_v514___ast_162_condition
  %r1 = load i8, ptr %t_v514___ast_162_condition
  %r2 = icmp ne i64 %r0, 0
  %r3 = xor i1 %r2, true
  %r4 = zext i1 %r3 to i64
  %r5 = icmp ne i64 %r4, 0
  br i1 %r5, label %ast_block_0, label %ast_block_1
ast_block_0:
  store ptr @f_v162_std__lang__assert_bool, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 782, ptr @chali_source_line
  %r6 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 176, ptr %r6
  %r7 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 0, ptr %r7
  %r8 = call i64 @chali_host_call(ptr @native_name0, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r9 = call i8 @chali_host_result_float()
  br label %ast_block_2
ast_block_1:
  br label %ast_block_2
ast_block_2:
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define internal i64 @f_v163_std__lang__assert_bool_std__lang__String(i64 %p0, i64 %p1) uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [2 x i64]
  %native_argument_types = alloca [2 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v515___ast_163_condition_226 = alloca i64
  store i64 0, ptr %s_v515___ast_163_condition_226
  %t_v515___ast_163_condition_226 = alloca i8
  store i8 0, ptr %t_v515___ast_163_condition_226
  %s_v516___ast_163_message_227 = alloca i64
  store i64 0, ptr %s_v516___ast_163_message_227
  %t_v516___ast_163_message_227 = alloca i8
  store i8 0, ptr %t_v516___ast_163_message_227
  store i64 %p0, ptr %s_v515___ast_163_condition_226
  store i64 %p1, ptr %s_v516___ast_163_message_227
  %r0 = load i64, ptr %s_v516___ast_163_message_227
  %r1 = load i8, ptr %t_v516___ast_163_message_227
  %r2 = call i64 @f_v60_std__lang____bootStringCopy(i64 %r0)
  %r3 = load i8, ptr @chali_return_float
  store i64 %r2, ptr %s_v516___ast_163_message_227
  store i8 %r3, ptr %t_v516___ast_163_message_227
  %r4 = load i64, ptr %s_v515___ast_163_condition_226
  %r5 = load i8, ptr %t_v515___ast_163_condition_226
  %r6 = icmp ne i64 %r4, 0
  %r7 = xor i1 %r6, true
  %r8 = zext i1 %r7 to i64
  %r9 = icmp ne i64 %r8, 0
  br i1 %r9, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r10 = load i64, ptr %s_v516___ast_163_message_227
  %r11 = load i8, ptr %t_v516___ast_163_message_227
  store ptr @f_v163_std__lang__assert_bool_std__lang__String, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 786, ptr @chali_source_line
  %r12 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r10, ptr %r12
  %r13 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r11, ptr %r13
  %r14 = call i64 @chali_host_call(ptr @native_name0, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r15 = call i8 @chali_host_result_float()
  br label %ast_block_2
ast_block_1:
  br label %ast_block_2
ast_block_2:
  %r16 = load i64, ptr %s_v516___ast_163_message_227
  %r17 = load i8, ptr %t_v516___ast_163_message_227
  %r18 = icmp ne i64 %r16, 0
  %r19 = zext i1 %r18 to i64
  %r20 = icmp ne i64 %r19, 0
  br i1 %r20, label %ast_block_3, label %ast_block_4
ast_block_3:
  %r21 = load i64, ptr %s_v516___ast_163_message_227
  %r22 = load i8, ptr %t_v516___ast_163_message_227
  %r23 = call i64 @f_v61_std__lang____bootStringDrop(i64 %r21)
  %r24 = load i8, ptr @chali_return_float
  br label %ast_block_5
ast_block_4:
  br label %ast_block_5
ast_block_5:
  store i64 0, ptr %s_v516___ast_163_message_227
  store i8 0, ptr %t_v516___ast_163_message_227
  store i8 0, ptr @chali_return_float
  ret i64 0
ast_label_v517___chali_cleanup_std__lang__assert_bool_std__lang__String:
  %r25 = load i64, ptr %s_v516___ast_163_message_227
  %r26 = load i8, ptr %t_v516___ast_163_message_227
  %r27 = icmp ne i64 %r25, 0
  %r28 = zext i1 %r27 to i64
  %r29 = icmp ne i64 %r28, 0
  br i1 %r29, label %ast_block_6, label %ast_block_7
ast_block_6:
  %r30 = load i64, ptr %s_v516___ast_163_message_227
  %r31 = load i8, ptr %t_v516___ast_163_message_227
  %r32 = call i64 @f_v61_std__lang____bootStringDrop(i64 %r30)
  %r33 = load i8, ptr @chali_return_float
  br label %ast_block_8
ast_block_7:
  br label %ast_block_8
ast_block_8:
  store i64 0, ptr %s_v516___ast_163_message_227
  store i8 0, ptr %t_v516___ast_163_message_227
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define internal i64 @f_v169_std__lang____bootBufferCapacity(i64 %p0, i64 %p1, i64 %p2, i64 %p3, i64 %p4) uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [3 x i64]
  %native_argument_types = alloca [3 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v518___ast_169___chali_ret_v_1008 = alloca i64
  store i64 0, ptr %s_v518___ast_169___chali_ret_v_1008
  %t_v518___ast_169___chali_ret_v_1008 = alloca i8
  store i8 0, ptr %t_v518___ast_169___chali_ret_v_1008
  %s_v519___ast_169___chali_ret_v_1009 = alloca i64
  store i64 0, ptr %s_v519___ast_169___chali_ret_v_1009
  %t_v519___ast_169___chali_ret_v_1009 = alloca i8
  store i8 0, ptr %t_v519___ast_169___chali_ret_v_1009
  %s_v520___ast_169_capacity_252 = alloca i64
  store i64 0, ptr %s_v520___ast_169_capacity_252
  %t_v520___ast_169_capacity_252 = alloca i8
  store i8 0, ptr %t_v520___ast_169_capacity_252
  %s_v521___ast_169_grown = alloca i64
  store i64 0, ptr %s_v521___ast_169_grown
  %t_v521___ast_169_grown = alloca i8
  store i8 0, ptr %t_v521___ast_169_grown
  %s_v522___ast_169_length_251 = alloca i64
  store i64 0, ptr %s_v522___ast_169_length_251
  %t_v522___ast_169_length_251 = alloca i8
  store i8 0, ptr %t_v522___ast_169_length_251
  %s_v523___ast_169_maximum = alloca i64
  store i64 0, ptr %s_v523___ast_169_maximum
  %t_v523___ast_169_maximum = alloca i8
  store i8 0, ptr %t_v523___ast_169_maximum
  %s_v524___ast_169_minimum = alloca i64
  store i64 0, ptr %s_v524___ast_169_minimum
  %t_v524___ast_169_minimum = alloca i8
  store i8 0, ptr %t_v524___ast_169_minimum
  %s_v525___ast_169_need = alloca i64
  store i64 0, ptr %s_v525___ast_169_need
  %t_v525___ast_169_need = alloca i8
  store i8 0, ptr %t_v525___ast_169_need
  %s_v526___ast_169_required = alloca i64
  store i64 0, ptr %s_v526___ast_169_required
  %t_v526___ast_169_required = alloca i8
  store i8 0, ptr %t_v526___ast_169_required
  store i64 %p0, ptr %s_v522___ast_169_length_251
  store i64 %p1, ptr %s_v525___ast_169_need
  store i64 %p2, ptr %s_v520___ast_169_capacity_252
  store i64 %p3, ptr %s_v524___ast_169_minimum
  store i64 %p4, ptr %s_v523___ast_169_maximum
  %r0 = load i64, ptr %s_v522___ast_169_length_251
  %r1 = load i8, ptr %t_v522___ast_169_length_251
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1390, ptr @chali_source_line
  %r2 = icmp sge i64 %r0, 0
  %r3 = zext i1 %r2 to i64
  %r4 = icmp ne i64 %r3, 0
  br i1 %r4, label %ast_block_1, label %ast_block_0
ast_block_0:
  br label %ast_block_2
ast_block_1:
  %r5 = load i64, ptr %s_v525___ast_169_need
  %r6 = load i8, ptr %t_v525___ast_169_need
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1390, ptr @chali_source_line
  %r7 = icmp sge i64 %r5, 0
  %r8 = zext i1 %r7 to i64
  %r9 = icmp ne i64 %r8, 0
  br label %ast_block_3
ast_block_3:
  br label %ast_block_2
ast_block_2:
  %r10 = phi i1 [ false, %ast_block_0 ], [ %r9, %ast_block_3 ]
  %r11 = zext i1 %r10 to i64
  %r12 = icmp ne i64 %r11, 0
  br i1 %r12, label %ast_block_5, label %ast_block_4
ast_block_4:
  br label %ast_block_6
ast_block_5:
  %r13 = load i64, ptr %s_v520___ast_169_capacity_252
  %r14 = load i8, ptr %t_v520___ast_169_capacity_252
  %r15 = load i64, ptr %s_v522___ast_169_length_251
  %r16 = load i8, ptr %t_v522___ast_169_length_251
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1390, ptr @chali_source_line
  %r17 = icmp sge i64 %r13, %r15
  %r18 = zext i1 %r17 to i64
  %r19 = icmp ne i64 %r18, 0
  br label %ast_block_7
ast_block_7:
  br label %ast_block_6
ast_block_6:
  %r20 = phi i1 [ false, %ast_block_4 ], [ %r19, %ast_block_7 ]
  %r21 = zext i1 %r20 to i64
  %r22 = icmp ne i64 %r21, 0
  br i1 %r22, label %ast_block_9, label %ast_block_8
ast_block_8:
  br label %ast_block_10
ast_block_9:
  %r23 = load i64, ptr %s_v524___ast_169_minimum
  %r24 = load i8, ptr %t_v524___ast_169_minimum
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1390, ptr @chali_source_line
  %r25 = icmp sge i64 %r23, 0
  %r26 = zext i1 %r25 to i64
  %r27 = icmp ne i64 %r26, 0
  br label %ast_block_11
ast_block_11:
  br label %ast_block_10
ast_block_10:
  %r28 = phi i1 [ false, %ast_block_8 ], [ %r27, %ast_block_11 ]
  %r29 = zext i1 %r28 to i64
  %r30 = icmp ne i64 %r29, 0
  br i1 %r30, label %ast_block_13, label %ast_block_12
ast_block_12:
  br label %ast_block_14
ast_block_13:
  %r31 = load i64, ptr %s_v523___ast_169_maximum
  %r32 = load i8, ptr %t_v523___ast_169_maximum
  %r33 = load i64, ptr %s_v524___ast_169_minimum
  %r34 = load i8, ptr %t_v524___ast_169_minimum
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1390, ptr @chali_source_line
  %r35 = icmp sge i64 %r31, %r33
  %r36 = zext i1 %r35 to i64
  %r37 = icmp ne i64 %r36, 0
  br label %ast_block_15
ast_block_15:
  br label %ast_block_14
ast_block_14:
  %r38 = phi i1 [ false, %ast_block_12 ], [ %r37, %ast_block_15 ]
  %r39 = zext i1 %r38 to i64
  %r40 = icmp ne i64 %r39, 0
  br i1 %r40, label %ast_block_17, label %ast_block_16
ast_block_16:
  br label %ast_block_18
ast_block_17:
  %r41 = load i64, ptr %s_v520___ast_169_capacity_252
  %r42 = load i8, ptr %t_v520___ast_169_capacity_252
  %r43 = load i64, ptr %s_v523___ast_169_maximum
  %r44 = load i8, ptr %t_v523___ast_169_maximum
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1390, ptr @chali_source_line
  %r45 = icmp sle i64 %r41, %r43
  %r46 = zext i1 %r45 to i64
  %r47 = icmp ne i64 %r46, 0
  br label %ast_block_19
ast_block_19:
  br label %ast_block_18
ast_block_18:
  %r48 = phi i1 [ false, %ast_block_16 ], [ %r47, %ast_block_19 ]
  %r49 = zext i1 %r48 to i64
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1390, ptr @chali_source_line
  %r50 = call i64 @f_v163_std__lang__assert_bool_std__lang__String(i64 %r49, i64 88)
  %r51 = load i8, ptr @chali_return_float
  %r52 = load i64, ptr %s_v525___ast_169_need
  %r53 = load i8, ptr %t_v525___ast_169_need
  %r54 = load i64, ptr %s_v523___ast_169_maximum
  %r55 = load i8, ptr %t_v523___ast_169_maximum
  %r56 = load i64, ptr %s_v522___ast_169_length_251
  %r57 = load i8, ptr %t_v522___ast_169_length_251
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1391, ptr @chali_source_line
  %r58 = sub i64 %r54, %r56
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1391, ptr @chali_source_line
  %r59 = icmp sle i64 %r52, %r58
  %r60 = zext i1 %r59 to i64
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1391, ptr @chali_source_line
  %r61 = call i64 @f_v163_std__lang__assert_bool_std__lang__String(i64 %r60, i64 120)
  %r62 = load i8, ptr @chali_return_float
  %r63 = load i64, ptr %s_v522___ast_169_length_251
  %r64 = load i8, ptr %t_v522___ast_169_length_251
  %r65 = load i64, ptr %s_v525___ast_169_need
  %r66 = load i8, ptr %t_v525___ast_169_need
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1392, ptr @chali_source_line
  %r67 = add i64 %r63, %r65
  store i64 %r67, ptr %s_v526___ast_169_required
  store i8 0, ptr %t_v526___ast_169_required
  %r68 = load i64, ptr %s_v526___ast_169_required
  %r69 = load i8, ptr %t_v526___ast_169_required
  %r70 = load i64, ptr %s_v520___ast_169_capacity_252
  %r71 = load i8, ptr %t_v520___ast_169_capacity_252
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1393, ptr @chali_source_line
  %r72 = icmp sle i64 %r68, %r70
  %r73 = zext i1 %r72 to i64
  %r74 = icmp ne i64 %r73, 0
  br i1 %r74, label %ast_block_20, label %ast_block_21
ast_block_20:
  %r75 = load i64, ptr %s_v520___ast_169_capacity_252
  %r76 = load i8, ptr %t_v520___ast_169_capacity_252
  store i64 %r75, ptr %s_v518___ast_169___chali_ret_v_1008
  store i8 %r76, ptr %t_v518___ast_169___chali_ret_v_1008
  %r77 = load i64, ptr %s_v518___ast_169___chali_ret_v_1008
  %r78 = load i8, ptr %t_v518___ast_169___chali_ret_v_1008
  store i8 %r78, ptr @chali_return_float
  ret i64 %r77
ast_block_21:
  br label %ast_block_22
ast_block_22:
  %r79 = load i64, ptr %s_v523___ast_169_maximum
  %r80 = load i8, ptr %t_v523___ast_169_maximum
  store i64 %r79, ptr %s_v521___ast_169_grown
  store i8 %r80, ptr %t_v521___ast_169_grown
  %r81 = load i64, ptr %s_v520___ast_169_capacity_252
  %r82 = load i8, ptr %t_v520___ast_169_capacity_252
  %r83 = load i64, ptr %s_v523___ast_169_maximum
  %r84 = load i8, ptr %t_v523___ast_169_maximum
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1395, ptr @chali_source_line
  %r85 = call i64 @chali_integer_divide(i64 %r83, i64 2, i32 0, i32 0)
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1395, ptr @chali_source_line
  %r86 = icmp sle i64 %r81, %r85
  %r87 = zext i1 %r86 to i64
  %r88 = icmp ne i64 %r87, 0
  br i1 %r88, label %ast_block_23, label %ast_block_24
ast_block_23:
  %r89 = load i64, ptr %s_v520___ast_169_capacity_252
  %r90 = load i8, ptr %t_v520___ast_169_capacity_252
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1395, ptr @chali_source_line
  %r91 = mul i64 %r89, 2
  store i64 %r91, ptr %s_v521___ast_169_grown
  store i8 0, ptr %t_v521___ast_169_grown
  br label %ast_block_25
ast_block_24:
  br label %ast_block_25
ast_block_25:
  %r92 = load i64, ptr %s_v521___ast_169_grown
  %r93 = load i8, ptr %t_v521___ast_169_grown
  %r94 = load i64, ptr %s_v526___ast_169_required
  %r95 = load i8, ptr %t_v526___ast_169_required
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1396, ptr @chali_source_line
  %r96 = icmp slt i64 %r92, %r94
  %r97 = zext i1 %r96 to i64
  %r98 = icmp ne i64 %r97, 0
  br i1 %r98, label %ast_block_26, label %ast_block_27
ast_block_26:
  %r99 = load i64, ptr %s_v526___ast_169_required
  %r100 = load i8, ptr %t_v526___ast_169_required
  store i64 %r99, ptr %s_v521___ast_169_grown
  store i8 %r100, ptr %t_v521___ast_169_grown
  br label %ast_block_28
ast_block_27:
  br label %ast_block_28
ast_block_28:
  %r101 = load i64, ptr %s_v521___ast_169_grown
  %r102 = load i8, ptr %t_v521___ast_169_grown
  %r103 = load i64, ptr %s_v524___ast_169_minimum
  %r104 = load i8, ptr %t_v524___ast_169_minimum
  store ptr @f_v169_std__lang____bootBufferCapacity, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1397, ptr @chali_source_line
  %r105 = icmp slt i64 %r101, %r103
  %r106 = zext i1 %r105 to i64
  %r107 = icmp ne i64 %r106, 0
  br i1 %r107, label %ast_block_29, label %ast_block_30
ast_block_29:
  %r108 = load i64, ptr %s_v524___ast_169_minimum
  %r109 = load i8, ptr %t_v524___ast_169_minimum
  store i64 %r108, ptr %s_v521___ast_169_grown
  store i8 %r109, ptr %t_v521___ast_169_grown
  br label %ast_block_31
ast_block_30:
  br label %ast_block_31
ast_block_31:
  %r110 = load i64, ptr %s_v521___ast_169_grown
  %r111 = load i8, ptr %t_v521___ast_169_grown
  store i64 %r110, ptr %s_v519___ast_169___chali_ret_v_1009
  store i8 %r111, ptr %t_v519___ast_169___chali_ret_v_1009
  %r112 = load i64, ptr %s_v519___ast_169___chali_ret_v_1009
  %r113 = load i8, ptr %t_v519___ast_169___chali_ret_v_1009
  store i8 %r113, ptr @chali_return_float
  ret i64 %r112
}
define internal i64 @f_v170_std__lang____bootListReserve(i64 %p0, i64 %p1) uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [6 x i64]
  %native_argument_types = alloca [6 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v527___ast_170___chali_ret_v_1011 = alloca i64
  store i64 0, ptr %s_v527___ast_170___chali_ret_v_1011
  %t_v527___ast_170___chali_ret_v_1011 = alloca i8
  store i8 0, ptr %t_v527___ast_170___chali_ret_v_1011
  %s_v528___ast_170_capacity_1010 = alloca i64
  store i64 0, ptr %s_v528___ast_170_capacity_1010
  %t_v528___ast_170_capacity_1010 = alloca i8
  store i8 0, ptr %t_v528___ast_170_capacity_1010
  %s_v529___ast_170_handle_254 = alloca i64
  store i64 0, ptr %s_v529___ast_170_handle_254
  %t_v529___ast_170_handle_254 = alloca i8
  store i8 0, ptr %t_v529___ast_170_handle_254
  %s_v530___ast_170_maximum_1012 = alloca i64
  store i64 0, ptr %s_v530___ast_170_maximum_1012
  %t_v530___ast_170_maximum_1012 = alloca i8
  store i8 0, ptr %t_v530___ast_170_maximum_1012
  %s_v531___ast_170_minimum_1013 = alloca i64
  store i64 0, ptr %s_v531___ast_170_minimum_1013
  %t_v531___ast_170_minimum_1013 = alloca i8
  store i8 0, ptr %t_v531___ast_170_minimum_1013
  %s_v532___ast_170_requested = alloca i64
  store i64 0, ptr %s_v532___ast_170_requested
  %t_v532___ast_170_requested = alloca i8
  store i8 0, ptr %t_v532___ast_170_requested
  store i64 %p0, ptr %s_v529___ast_170_handle_254
  store i64 %p1, ptr %s_v532___ast_170_requested
  %r0 = load i64, ptr %s_v529___ast_170_handle_254
  %r1 = load i8, ptr %t_v529___ast_170_handle_254
  store ptr @f_v170_std__lang____bootListReserve, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1403, ptr @chali_source_line
  %r2 = zext i8 %r1 to i64
  %r3 = shl i64 %r2, 0
  %r4 = or i64 0, %r3
  %r5 = or i64 %r4, 0
  %r6 = call i64 @chali_host_list_storage_scalar(i64 1, i64 %r0, i64 0, i64 0, i64 0, i64 0, i64 %r5, ptr %collection_result_kind)
  %r7 = load i8, ptr %collection_result_kind
  store i64 %r6, ptr %s_v528___ast_170_capacity_1010
  store i8 %r7, ptr %t_v528___ast_170_capacity_1010
  %r8 = load i64, ptr %s_v532___ast_170_requested
  %r9 = load i8, ptr %t_v532___ast_170_requested
  %r10 = load i64, ptr %s_v528___ast_170_capacity_1010
  %r11 = load i8, ptr %t_v528___ast_170_capacity_1010
  store ptr @f_v170_std__lang____bootListReserve, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1404, ptr @chali_source_line
  %r12 = icmp sle i64 %r8, %r10
  %r13 = zext i1 %r12 to i64
  %r14 = icmp ne i64 %r13, 0
  br i1 %r14, label %ast_block_0, label %ast_block_1
ast_block_0:
  store i64 0, ptr %s_v527___ast_170___chali_ret_v_1011
  store i8 0, ptr %t_v527___ast_170___chali_ret_v_1011
  %r15 = load i64, ptr %s_v527___ast_170___chali_ret_v_1011
  %r16 = load i8, ptr %t_v527___ast_170___chali_ret_v_1011
  store i8 %r16, ptr @chali_return_float
  ret i64 %r15
ast_block_1:
  br label %ast_block_2
ast_block_2:
  %r17 = load i64, ptr %s_v529___ast_170_handle_254
  %r18 = load i8, ptr %t_v529___ast_170_handle_254
  store ptr @f_v170_std__lang____bootListReserve, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1405, ptr @chali_source_line
  %r19 = zext i8 %r18 to i64
  %r20 = shl i64 %r19, 0
  %r21 = or i64 0, %r20
  %r22 = or i64 %r21, 0
  %r23 = call i64 @chali_host_list_storage_scalar(i64 17, i64 %r17, i64 0, i64 0, i64 0, i64 0, i64 %r22, ptr %collection_result_kind)
  %r24 = load i8, ptr %collection_result_kind
  store i64 %r23, ptr %s_v530___ast_170_maximum_1012
  store i8 %r24, ptr %t_v530___ast_170_maximum_1012
  store i64 8, ptr %s_v531___ast_170_minimum_1013
  store i8 0, ptr %t_v531___ast_170_minimum_1013
  %r25 = load i64, ptr %s_v531___ast_170_minimum_1013
  %r26 = load i8, ptr %t_v531___ast_170_minimum_1013
  %r27 = load i64, ptr %s_v530___ast_170_maximum_1012
  %r28 = load i8, ptr %t_v530___ast_170_maximum_1012
  store ptr @f_v170_std__lang____bootListReserve, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1406, ptr @chali_source_line
  %r29 = icmp sgt i64 %r25, %r27
  %r30 = zext i1 %r29 to i64
  %r31 = icmp ne i64 %r30, 0
  br i1 %r31, label %ast_block_3, label %ast_block_4
ast_block_3:
  %r32 = load i64, ptr %s_v530___ast_170_maximum_1012
  %r33 = load i8, ptr %t_v530___ast_170_maximum_1012
  store i64 %r32, ptr %s_v531___ast_170_minimum_1013
  store i8 %r33, ptr %t_v531___ast_170_minimum_1013
  br label %ast_block_5
ast_block_4:
  br label %ast_block_5
ast_block_5:
  %r34 = load i64, ptr %s_v529___ast_170_handle_254
  %r35 = load i8, ptr %t_v529___ast_170_handle_254
  %r36 = load i64, ptr %s_v532___ast_170_requested
  %r37 = load i8, ptr %t_v532___ast_170_requested
  %r38 = load i64, ptr %s_v528___ast_170_capacity_1010
  %r39 = load i8, ptr %t_v528___ast_170_capacity_1010
  %r40 = load i64, ptr %s_v531___ast_170_minimum_1013
  %r41 = load i8, ptr %t_v531___ast_170_minimum_1013
  %r42 = load i64, ptr %s_v530___ast_170_maximum_1012
  %r43 = load i8, ptr %t_v530___ast_170_maximum_1012
  store ptr @f_v170_std__lang____bootListReserve, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1407, ptr @chali_source_line
  %r44 = call i64 @f_v169_std__lang____bootBufferCapacity(i64 0, i64 %r36, i64 %r38, i64 %r40, i64 %r42)
  %r45 = load i8, ptr @chali_return_float
  store ptr @f_v170_std__lang____bootListReserve, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1407, ptr @chali_source_line
  %r46 = zext i8 %r35 to i64
  %r47 = shl i64 %r46, 0
  %r48 = or i64 0, %r47
  %r49 = zext i8 %r45 to i64
  %r50 = shl i64 %r49, 1
  %r51 = or i64 %r48, %r50
  %r52 = or i64 %r51, 0
  %r53 = call i64 @chali_host_list_storage_scalar(i64 2, i64 %r34, i64 %r44, i64 0, i64 0, i64 0, i64 %r52, ptr %collection_result_kind)
  %r54 = load i8, ptr %collection_result_kind
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define internal i64 @f_v171_std__lang____bootListRemove(i64 %p0, i64 %p1) alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [4 x i64]
  %native_argument_types = alloca [4 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v533___ast_171___chali_ret_v_1016 = alloca i64
  store i64 0, ptr %s_v533___ast_171___chali_ret_v_1016
  %t_v533___ast_171___chali_ret_v_1016 = alloca i8
  store i8 0, ptr %t_v533___ast_171___chali_ret_v_1016
  %s_v534___ast_171_handle_255 = alloca i64
  store i64 0, ptr %s_v534___ast_171_handle_255
  %t_v534___ast_171_handle_255 = alloca i8
  store i8 0, ptr %t_v534___ast_171_handle_255
  %s_v535___ast_171_i_1015 = alloca i64
  store i64 0, ptr %s_v535___ast_171_i_1015
  %t_v535___ast_171_i_1015 = alloca i8
  store i8 0, ptr %t_v535___ast_171_i_1015
  %s_v536___ast_171_index_256 = alloca i64
  store i64 0, ptr %s_v536___ast_171_index_256
  %t_v536___ast_171_index_256 = alloca i8
  store i8 0, ptr %t_v536___ast_171_index_256
  %s_v537___ast_171_size_1014 = alloca i64
  store i64 0, ptr %s_v537___ast_171_size_1014
  %t_v537___ast_171_size_1014 = alloca i8
  store i8 0, ptr %t_v537___ast_171_size_1014
  store i64 %p0, ptr %s_v534___ast_171_handle_255
  store i64 %p1, ptr %s_v536___ast_171_index_256
  %r0 = load i64, ptr %s_v534___ast_171_handle_255
  %r1 = load i8, ptr %t_v534___ast_171_handle_255
  %r2 = load i64, ptr %s_v536___ast_171_index_256
  %r3 = load i8, ptr %t_v536___ast_171_index_256
  store ptr @f_v171_std__lang____bootListRemove, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1411, ptr @chali_source_line
  %r4 = zext i8 %r1 to i64
  %r5 = shl i64 %r4, 0
  %r6 = or i64 0, %r5
  %r7 = zext i8 %r3 to i64
  %r8 = shl i64 %r7, 2
  %r9 = or i64 %r6, %r8
  %r10 = or i64 %r9, 0
  %r11 = call i64 @chali_host_list_storage_scalar(i64 0, i64 %r0, i64 2, i64 %r2, i64 0, i64 0, i64 %r10, ptr %collection_result_kind)
  %r12 = load i8, ptr %collection_result_kind
  store i64 %r11, ptr %s_v537___ast_171_size_1014
  store i8 %r12, ptr %t_v537___ast_171_size_1014
  %r13 = load i64, ptr %s_v536___ast_171_index_256
  %r14 = load i8, ptr %t_v536___ast_171_index_256
  store i64 %r13, ptr %s_v535___ast_171_i_1015
  store i8 %r14, ptr %t_v535___ast_171_i_1015
  br label %ast_label_v538___chali_loop_69
ast_label_v538___chali_loop_69:
  %r15 = load i64, ptr %s_v535___ast_171_i_1015
  %r16 = load i8, ptr %t_v535___ast_171_i_1015
  store ptr @f_v171_std__lang____bootListRemove, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1412, ptr @chali_source_line
  %r17 = add i64 %r15, 1
  %r18 = load i64, ptr %s_v537___ast_171_size_1014
  %r19 = load i8, ptr %t_v537___ast_171_size_1014
  store ptr @f_v171_std__lang____bootListRemove, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1412, ptr @chali_source_line
  %r20 = icmp slt i64 %r17, %r18
  %r21 = zext i1 %r20 to i64
  %r22 = icmp ne i64 %r21, 0
  br i1 %r22, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r23 = load i64, ptr %s_v534___ast_171_handle_255
  %r24 = load i8, ptr %t_v534___ast_171_handle_255
  %r25 = load i64, ptr %s_v535___ast_171_i_1015
  %r26 = load i8, ptr %t_v535___ast_171_i_1015
  %r27 = load i64, ptr %s_v535___ast_171_i_1015
  %r28 = load i8, ptr %t_v535___ast_171_i_1015
  store ptr @f_v171_std__lang____bootListRemove, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1412, ptr @chali_source_line
  %r29 = add i64 %r27, 1
  store ptr @f_v171_std__lang____bootListRemove, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1412, ptr @chali_source_line
  %r30 = zext i8 %r24 to i64
  %r31 = shl i64 %r30, 0
  %r32 = or i64 0, %r31
  %r33 = zext i8 %r26 to i64
  %r34 = shl i64 %r33, 1
  %r35 = or i64 %r32, %r34
  %r36 = or i64 %r35, 0
  %r37 = call i64 @chali_host_list_storage_scalar(i64 3, i64 %r23, i64 %r25, i64 %r29, i64 0, i64 0, i64 %r36, ptr %collection_result_kind)
  %r38 = load i8, ptr %collection_result_kind
  %r39 = load i64, ptr %s_v535___ast_171_i_1015
  %r40 = load i8, ptr %t_v535___ast_171_i_1015
  store ptr @f_v171_std__lang____bootListRemove, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1412, ptr @chali_source_line
  %r41 = add i64 %r39, 1
  store i64 %r41, ptr %s_v535___ast_171_i_1015
  store i8 0, ptr %t_v535___ast_171_i_1015
  br label %ast_block_2
ast_block_1:
  br label %ast_label_v539___chali_brk_70
ast_block_2:
  br label %ast_label_v538___chali_loop_69
ast_label_v539___chali_brk_70:
  %r42 = load i64, ptr %s_v534___ast_171_handle_255
  %r43 = load i8, ptr %t_v534___ast_171_handle_255
  store ptr @f_v171_std__lang____bootListRemove, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1413, ptr @chali_source_line
  %r44 = zext i8 %r43 to i64
  %r45 = shl i64 %r44, 0
  %r46 = or i64 0, %r45
  %r47 = or i64 %r46, 0
  %r48 = call i64 @chali_host_list_storage_scalar(i64 4, i64 %r42, i64 0, i64 0, i64 0, i64 0, i64 %r47, ptr %collection_result_kind)
  %r49 = load i8, ptr %collection_result_kind
  store i64 0, ptr %s_v533___ast_171___chali_ret_v_1016
  store i8 0, ptr %t_v533___ast_171___chali_ret_v_1016
  %r50 = load i64, ptr %s_v533___ast_171___chali_ret_v_1016
  %r51 = load i8, ptr %t_v533___ast_171___chali_ret_v_1016
  store i8 %r51, ptr @chali_return_float
  ret i64 %r50
}
define internal i64 @f_v172_std__lang____bootListClear(i64 %p0) alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [4 x i64]
  %native_argument_types = alloca [4 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v540___ast_172___chali_ret_v_1018 = alloca i64
  store i64 0, ptr %s_v540___ast_172___chali_ret_v_1018
  %t_v540___ast_172___chali_ret_v_1018 = alloca i8
  store i8 0, ptr %t_v540___ast_172___chali_ret_v_1018
  %s_v541___ast_172_handle_257 = alloca i64
  store i64 0, ptr %s_v541___ast_172_handle_257
  %t_v541___ast_172_handle_257 = alloca i8
  store i8 0, ptr %t_v541___ast_172_handle_257
  %s_v542___ast_172_size_1017 = alloca i64
  store i64 0, ptr %s_v542___ast_172_size_1017
  %t_v542___ast_172_size_1017 = alloca i8
  store i8 0, ptr %t_v542___ast_172_size_1017
  store i64 %p0, ptr %s_v541___ast_172_handle_257
  %r0 = load i64, ptr %s_v541___ast_172_handle_257
  %r1 = load i8, ptr %t_v541___ast_172_handle_257
  store ptr @f_v172_std__lang____bootListClear, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1417, ptr @chali_source_line
  %r2 = zext i8 %r1 to i64
  %r3 = shl i64 %r2, 0
  %r4 = or i64 0, %r3
  %r5 = or i64 %r4, 0
  %r6 = call i64 @chali_host_list_storage_scalar(i64 0, i64 %r0, i64 4, i64 0, i64 0, i64 0, i64 %r5, ptr %collection_result_kind)
  %r7 = load i8, ptr %collection_result_kind
  store i64 %r6, ptr %s_v542___ast_172_size_1017
  store i8 %r7, ptr %t_v542___ast_172_size_1017
  br label %ast_label_v543___chali_loop_71
ast_label_v543___chali_loop_71:
  %r8 = load i64, ptr %s_v542___ast_172_size_1017
  %r9 = load i8, ptr %t_v542___ast_172_size_1017
  store ptr @f_v172_std__lang____bootListClear, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1418, ptr @chali_source_line
  %r10 = icmp sgt i64 %r8, 0
  %r11 = zext i1 %r10 to i64
  %r12 = icmp ne i64 %r11, 0
  br i1 %r12, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r13 = load i64, ptr %s_v541___ast_172_handle_257
  %r14 = load i8, ptr %t_v541___ast_172_handle_257
  store ptr @f_v172_std__lang____bootListClear, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1418, ptr @chali_source_line
  %r15 = zext i8 %r14 to i64
  %r16 = shl i64 %r15, 0
  %r17 = or i64 0, %r16
  %r18 = or i64 %r17, 0
  %r19 = call i64 @chali_host_list_storage_scalar(i64 4, i64 %r13, i64 0, i64 0, i64 0, i64 0, i64 %r18, ptr %collection_result_kind)
  %r20 = load i8, ptr %collection_result_kind
  %r21 = load i64, ptr %s_v542___ast_172_size_1017
  %r22 = load i8, ptr %t_v542___ast_172_size_1017
  store ptr @f_v172_std__lang____bootListClear, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1418, ptr @chali_source_line
  %r23 = sub i64 %r21, 1
  store i64 %r23, ptr %s_v542___ast_172_size_1017
  store i8 0, ptr %t_v542___ast_172_size_1017
  br label %ast_block_2
ast_block_1:
  br label %ast_label_v544___chali_brk_72
ast_block_2:
  br label %ast_label_v543___chali_loop_71
ast_label_v544___chali_brk_72:
  store i64 0, ptr %s_v540___ast_172___chali_ret_v_1018
  store i8 0, ptr %t_v540___ast_172___chali_ret_v_1018
  %r24 = load i64, ptr %s_v540___ast_172___chali_ret_v_1018
  %r25 = load i8, ptr %t_v540___ast_172___chali_ret_v_1018
  store i8 %r25, ptr @chali_return_float
  ret i64 %r24
}
define internal i64 @f_v173_std__lang____bootListCopy(i64 %p0) uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [4 x i64]
  %native_argument_types = alloca [4 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v545___ast_173___chali_ret_v_1022 = alloca i64
  store i64 0, ptr %s_v545___ast_173___chali_ret_v_1022
  %t_v545___ast_173___chali_ret_v_1022 = alloca i8
  store i8 0, ptr %t_v545___ast_173___chali_ret_v_1022
  %s_v546___ast_173_i_1021 = alloca i64
  store i64 0, ptr %s_v546___ast_173_i_1021
  %t_v546___ast_173_i_1021 = alloca i8
  store i8 0, ptr %t_v546___ast_173_i_1021
  %s_v547___ast_173_result_1019 = alloca i64
  store i64 0, ptr %s_v547___ast_173_result_1019
  %t_v547___ast_173_result_1019 = alloca i8
  store i8 0, ptr %t_v547___ast_173_result_1019
  %s_v548___ast_173_size_1020 = alloca i64
  store i64 0, ptr %s_v548___ast_173_size_1020
  %t_v548___ast_173_size_1020 = alloca i8
  store i8 0, ptr %t_v548___ast_173_size_1020
  %s_v549___ast_173_source_258 = alloca i64
  store i64 0, ptr %s_v549___ast_173_source_258
  %t_v549___ast_173_source_258 = alloca i8
  store i8 0, ptr %t_v549___ast_173_source_258
  store i64 %p0, ptr %s_v549___ast_173_source_258
  store ptr @f_v173_std__lang____bootListCopy, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1422, ptr @chali_source_line
  %r0 = or i64 0, 0
  %r1 = call i64 @chali_host_list_storage_scalar(i64 9, i64 0, i64 0, i64 0, i64 0, i64 0, i64 %r0, ptr %collection_result_kind)
  %r2 = load i8, ptr %collection_result_kind
  store i64 %r1, ptr %s_v547___ast_173_result_1019
  store i8 %r2, ptr %t_v547___ast_173_result_1019
  %r3 = load i64, ptr %s_v549___ast_173_source_258
  %r4 = load i8, ptr %t_v549___ast_173_source_258
  store ptr @f_v173_std__lang____bootListCopy, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1422, ptr @chali_source_line
  %r5 = zext i8 %r4 to i64
  %r6 = shl i64 %r5, 0
  %r7 = or i64 0, %r6
  %r8 = or i64 %r7, 0
  %r9 = call i64 @chali_host_list_storage_scalar(i64 10, i64 %r3, i64 0, i64 0, i64 0, i64 0, i64 %r8, ptr %collection_result_kind)
  %r10 = load i8, ptr %collection_result_kind
  store i64 %r9, ptr %s_v548___ast_173_size_1020
  store i8 %r10, ptr %t_v548___ast_173_size_1020
  %r11 = load i64, ptr %s_v547___ast_173_result_1019
  %r12 = load i8, ptr %t_v547___ast_173_result_1019
  %r13 = load i64, ptr %s_v548___ast_173_size_1020
  %r14 = load i8, ptr %t_v548___ast_173_size_1020
  store ptr @f_v173_std__lang____bootListCopy, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1422, ptr @chali_source_line
  %r15 = call i64 @f_v170_std__lang____bootListReserve(i64 %r11, i64 %r13)
  %r16 = load i8, ptr @chali_return_float
  store i64 0, ptr %s_v546___ast_173_i_1021
  store i8 0, ptr %t_v546___ast_173_i_1021
  br label %ast_label_v550___chali_loop_73
ast_label_v550___chali_loop_73:
  %r17 = load i64, ptr %s_v546___ast_173_i_1021
  %r18 = load i8, ptr %t_v546___ast_173_i_1021
  %r19 = load i64, ptr %s_v548___ast_173_size_1020
  %r20 = load i8, ptr %t_v548___ast_173_size_1020
  store ptr @f_v173_std__lang____bootListCopy, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1423, ptr @chali_source_line
  %r21 = icmp slt i64 %r17, %r19
  %r22 = zext i1 %r21 to i64
  %r23 = icmp ne i64 %r22, 0
  br i1 %r23, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r24 = load i64, ptr %s_v547___ast_173_result_1019
  %r25 = load i8, ptr %t_v547___ast_173_result_1019
  %r26 = load i64, ptr %s_v549___ast_173_source_258
  %r27 = load i8, ptr %t_v549___ast_173_source_258
  %r28 = load i64, ptr %s_v546___ast_173_i_1021
  %r29 = load i8, ptr %t_v546___ast_173_i_1021
  store ptr @f_v173_std__lang____bootListCopy, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1423, ptr @chali_source_line
  %r30 = zext i8 %r25 to i64
  %r31 = shl i64 %r30, 0
  %r32 = or i64 0, %r31
  %r33 = zext i8 %r27 to i64
  %r34 = shl i64 %r33, 1
  %r35 = or i64 %r32, %r34
  %r36 = zext i8 %r29 to i64
  %r37 = shl i64 %r36, 2
  %r38 = or i64 %r35, %r37
  %r39 = or i64 %r38, 0
  %r40 = call i64 @chali_host_list_storage_scalar(i64 6, i64 %r24, i64 %r26, i64 %r28, i64 0, i64 0, i64 %r39, ptr %collection_result_kind)
  %r41 = load i8, ptr %collection_result_kind
  %r42 = load i64, ptr %s_v546___ast_173_i_1021
  %r43 = load i8, ptr %t_v546___ast_173_i_1021
  store ptr @f_v173_std__lang____bootListCopy, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1423, ptr @chali_source_line
  %r44 = add i64 %r42, 1
  store i64 %r44, ptr %s_v546___ast_173_i_1021
  store i8 0, ptr %t_v546___ast_173_i_1021
  br label %ast_block_2
ast_block_1:
  br label %ast_label_v551___chali_brk_74
ast_block_2:
  br label %ast_label_v550___chali_loop_73
ast_label_v551___chali_brk_74:
  %r45 = load i64, ptr %s_v547___ast_173_result_1019
  %r46 = load i8, ptr %t_v547___ast_173_result_1019
  store i64 %r45, ptr %s_v545___ast_173___chali_ret_v_1022
  store i8 %r46, ptr %t_v545___ast_173___chali_ret_v_1022
  %r47 = load i64, ptr %s_v545___ast_173___chali_ret_v_1022
  %r48 = load i8, ptr %t_v545___ast_173___chali_ret_v_1022
  store i8 %r48, ptr @chali_return_float
  ret i64 %r47
}
define internal i64 @f_v174_std__lang____bootListPushI(i64 %p0, i64 %p1, i64 %p2, i64 %p3) uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [5 x i64]
  %native_argument_types = alloca [5 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v552___ast_174___chali_ret_v_1024 = alloca i64
  store i64 0, ptr %s_v552___ast_174___chali_ret_v_1024
  %t_v552___ast_174___chali_ret_v_1024 = alloca i8
  store i8 0, ptr %t_v552___ast_174___chali_ret_v_1024
  %s_v553___ast_174_handle_270 = alloca i64
  store i64 0, ptr %s_v553___ast_174_handle_270
  %t_v553___ast_174_handle_270 = alloca i8
  store i8 0, ptr %t_v553___ast_174_handle_270
  %s_v554___ast_174_layout_273 = alloca i64
  store i64 0, ptr %s_v554___ast_174_layout_273
  %t_v554___ast_174_layout_273 = alloca i8
  store i8 0, ptr %t_v554___ast_174_layout_273
  %s_v555___ast_174_size_1023 = alloca i64
  store i64 0, ptr %s_v555___ast_174_size_1023
  %t_v555___ast_174_size_1023 = alloca i8
  store i8 0, ptr %t_v555___ast_174_size_1023
  %s_v556___ast_174_text_272 = alloca i64
  store i64 0, ptr %s_v556___ast_174_text_272
  %t_v556___ast_174_text_272 = alloca i8
  store i8 0, ptr %t_v556___ast_174_text_272
  %s_v557___ast_174_value_271 = alloca i64
  store i64 0, ptr %s_v557___ast_174_value_271
  %t_v557___ast_174_value_271 = alloca i8
  store i8 0, ptr %t_v557___ast_174_value_271
  store i64 %p0, ptr %s_v553___ast_174_handle_270
  store i64 %p1, ptr %s_v557___ast_174_value_271
  store i64 %p2, ptr %s_v556___ast_174_text_272
  store i64 %p3, ptr %s_v554___ast_174_layout_273
  %r0 = load i64, ptr %s_v553___ast_174_handle_270
  %r1 = load i8, ptr %t_v553___ast_174_handle_270
  store ptr @f_v174_std__lang____bootListPushI, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1431, ptr @chali_source_line
  %r2 = zext i8 %r1 to i64
  %r3 = shl i64 %r2, 0
  %r4 = or i64 0, %r3
  %r5 = or i64 %r4, 0
  %r6 = call i64 @chali_host_list_storage_scalar(i64 0, i64 %r0, i64 0, i64 0, i64 0, i64 0, i64 %r5, ptr %collection_result_kind)
  %r7 = load i8, ptr %collection_result_kind
  store i64 %r6, ptr %s_v555___ast_174_size_1023
  store i8 %r7, ptr %t_v555___ast_174_size_1023
  %r8 = load i64, ptr %s_v553___ast_174_handle_270
  %r9 = load i8, ptr %t_v553___ast_174_handle_270
  %r10 = load i64, ptr %s_v555___ast_174_size_1023
  %r11 = load i8, ptr %t_v555___ast_174_size_1023
  store ptr @f_v174_std__lang____bootListPushI, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1431, ptr @chali_source_line
  %r12 = add i64 %r10, 1
  store ptr @f_v174_std__lang____bootListPushI, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1431, ptr @chali_source_line
  %r13 = call i64 @f_v170_std__lang____bootListReserve(i64 %r8, i64 %r12)
  %r14 = load i8, ptr @chali_return_float
  %r15 = load i64, ptr %s_v553___ast_174_handle_270
  %r16 = load i8, ptr %t_v553___ast_174_handle_270
  %r17 = load i64, ptr %s_v557___ast_174_value_271
  %r18 = load i8, ptr %t_v557___ast_174_value_271
  %r19 = load i64, ptr %s_v556___ast_174_text_272
  %r20 = load i8, ptr %t_v556___ast_174_text_272
  %r21 = load i64, ptr %s_v554___ast_174_layout_273
  %r22 = load i8, ptr %t_v554___ast_174_layout_273
  store ptr @f_v174_std__lang____bootListPushI, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1432, ptr @chali_source_line
  %r23 = zext i8 %r16 to i64
  %r24 = shl i64 %r23, 0
  %r25 = or i64 0, %r24
  %r26 = zext i8 %r18 to i64
  %r27 = shl i64 %r26, 1
  %r28 = or i64 %r25, %r27
  %r29 = zext i8 %r20 to i64
  %r30 = shl i64 %r29, 2
  %r31 = or i64 %r28, %r30
  %r32 = zext i8 %r22 to i64
  %r33 = shl i64 %r32, 3
  %r34 = or i64 %r31, %r33
  %r35 = or i64 %r34, 0
  %r36 = call i64 @chali_host_list_storage_scalar(i64 7, i64 %r15, i64 %r17, i64 %r19, i64 %r21, i64 0, i64 %r35, ptr %collection_result_kind)
  %r37 = load i8, ptr %collection_result_kind
  store i64 %r36, ptr %s_v552___ast_174___chali_ret_v_1024
  store i8 %r37, ptr %t_v552___ast_174___chali_ret_v_1024
  %r38 = load i64, ptr %s_v552___ast_174___chali_ret_v_1024
  %r39 = load i8, ptr %t_v552___ast_174___chali_ret_v_1024
  store i8 %r39, ptr @chali_return_float
  ret i64 %r38
}
define internal i64 @f_v176_std__lang____bootListPopI(i64 %p0, i64 %p1, i64 %p2) alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [4 x i64]
  %native_argument_types = alloca [4 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v558___ast_176___chali_ret_v_1028 = alloca i64
  store i64 0, ptr %s_v558___ast_176___chali_ret_v_1028
  %t_v558___ast_176___chali_ret_v_1028 = alloca i8
  store i8 0, ptr %t_v558___ast_176___chali_ret_v_1028
  %s_v559___ast_176_handle_279 = alloca i64
  store i64 0, ptr %s_v559___ast_176_handle_279
  %t_v559___ast_176_handle_279 = alloca i8
  store i8 0, ptr %t_v559___ast_176_handle_279
  %s_v560___ast_176_layout_281 = alloca i64
  store i64 0, ptr %s_v560___ast_176_layout_281
  %t_v560___ast_176_layout_281 = alloca i8
  store i8 0, ptr %t_v560___ast_176_layout_281
  %s_v561___ast_176_text_280 = alloca i64
  store i64 0, ptr %s_v561___ast_176_text_280
  %t_v561___ast_176_text_280 = alloca i8
  store i8 0, ptr %t_v561___ast_176_text_280
  store i64 %p0, ptr %s_v559___ast_176_handle_279
  store i64 %p1, ptr %s_v561___ast_176_text_280
  store i64 %p2, ptr %s_v560___ast_176_layout_281
  %r0 = load i64, ptr %s_v559___ast_176_handle_279
  %r1 = load i8, ptr %t_v559___ast_176_handle_279
  store ptr @f_v176_std__lang____bootListPopI, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1442, ptr @chali_source_line
  %r2 = zext i8 %r1 to i64
  %r3 = shl i64 %r2, 0
  %r4 = or i64 0, %r3
  %r5 = or i64 %r4, 0
  %r6 = call i64 @chali_host_list_storage_scalar(i64 0, i64 %r0, i64 3, i64 0, i64 0, i64 0, i64 %r5, ptr %collection_result_kind)
  %r7 = load i8, ptr %collection_result_kind
  %r8 = load i64, ptr %s_v559___ast_176_handle_279
  %r9 = load i8, ptr %t_v559___ast_176_handle_279
  %r10 = load i64, ptr %s_v561___ast_176_text_280
  %r11 = load i8, ptr %t_v561___ast_176_text_280
  %r12 = load i64, ptr %s_v560___ast_176_layout_281
  %r13 = load i8, ptr %t_v560___ast_176_layout_281
  store ptr @f_v176_std__lang____bootListPopI, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 1442, ptr @chali_source_line
  %r14 = zext i8 %r9 to i64
  %r15 = shl i64 %r14, 0
  %r16 = or i64 0, %r15
  %r17 = zext i8 %r11 to i64
  %r18 = shl i64 %r17, 1
  %r19 = or i64 %r16, %r18
  %r20 = zext i8 %r13 to i64
  %r21 = shl i64 %r20, 2
  %r22 = or i64 %r19, %r21
  %r23 = or i64 %r22, 0
  %r24 = call i64 @chali_host_list_storage_scalar(i64 8, i64 %r8, i64 %r10, i64 %r12, i64 0, i64 0, i64 %r23, ptr %collection_result_kind)
  %r25 = load i8, ptr %collection_result_kind
  store i64 %r24, ptr %s_v558___ast_176___chali_ret_v_1028
  store i8 %r25, ptr %t_v558___ast_176___chali_ret_v_1028
  %r26 = load i64, ptr %s_v558___ast_176___chali_ret_v_1028
  %r27 = load i8, ptr %t_v558___ast_176___chali_ret_v_1028
  store i8 %r27, ptr @chali_return_float
  ret i64 %r26
}
define internal i64 @f_v235_program__referenceAddress(i64 %p0) alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [2 x i64]
  %native_argument_types = alloca [2 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v562___ast_235___chali_ret_v_1302 = alloca i64
  store i64 0, ptr %s_v562___ast_235___chali_ret_v_1302
  %t_v562___ast_235___chali_ret_v_1302 = alloca i8
  store i8 0, ptr %t_v562___ast_235___chali_ret_v_1302
  %s_v563___ast_235_value_593 = alloca i64
  store i64 0, ptr %s_v563___ast_235_value_593
  %t_v563___ast_235_value_593 = alloca i8
  store i8 0, ptr %t_v563___ast_235_value_593
  store i64 %p0, ptr %s_v563___ast_235_value_593
  %r0 = load i64, ptr %s_v563___ast_235_value_593
  %r1 = load i8, ptr %t_v563___ast_235_value_593
  %r2 = call i64 @chali_require_reference(i64 %r0)
  store i64 %r2, ptr %s_v562___ast_235___chali_ret_v_1302
  store i8 0, ptr %t_v562___ast_235___chali_ret_v_1302
  %r3 = load i64, ptr %s_v562___ast_235___chali_ret_v_1302
  %r4 = load i8, ptr %t_v562___ast_235___chali_ret_v_1302
  store i8 %r4, ptr @chali_return_float
  ret i64 %r3
}
define internal i64 @f_main() uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [6 x i64]
  %native_argument_types = alloca [6 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v564___ast_236___chali_reference_storage_1303 = alloca i64
  store i64 0, ptr %s_v564___ast_236___chali_reference_storage_1303
  %t_v564___ast_236___chali_reference_storage_1303 = alloca i8
  store i8 0, ptr %t_v564___ast_236___chali_reference_storage_1303
  %s_v565___ast_236___chali_reference_storage_1304 = alloca i64
  store i64 0, ptr %s_v565___ast_236___chali_reference_storage_1304
  %t_v565___ast_236___chali_reference_storage_1304 = alloca i8
  store i8 0, ptr %t_v565___ast_236___chali_reference_storage_1304
  %s_v566___ast_236___chali_tmp_s_1309 = alloca i64
  store i64 0, ptr %s_v566___ast_236___chali_tmp_s_1309
  %t_v566___ast_236___chali_tmp_s_1309 = alloca i8
  store i8 0, ptr %t_v566___ast_236___chali_tmp_s_1309
  %s_v567___ast_236___chali_tmp_s_1310 = alloca i64
  store i64 0, ptr %s_v567___ast_236___chali_tmp_s_1310
  %t_v567___ast_236___chali_tmp_s_1310 = alloca i8
  store i8 0, ptr %t_v567___ast_236___chali_tmp_s_1310
  %s_v568___ast_236___chali_tmp_s_1311 = alloca i64
  store i64 0, ptr %s_v568___ast_236___chali_tmp_s_1311
  %t_v568___ast_236___chali_tmp_s_1311 = alloca i8
  store i8 0, ptr %t_v568___ast_236___chali_tmp_s_1311
  %s_v569___ast_236_copy = alloca i64
  store i64 0, ptr %s_v569___ast_236_copy
  %t_v569___ast_236_copy = alloca i8
  store i8 0, ptr %t_v569___ast_236_copy
  %s_v570___ast_236_handle_1305 = alloca i64
  store i64 0, ptr %s_v570___ast_236_handle_1305
  %t_v570___ast_236_handle_1305 = alloca i8
  store i8 0, ptr %t_v570___ast_236_handle_1305
  %s_v571___ast_236_i_1308 = alloca i64
  store i64 0, ptr %s_v571___ast_236_i_1308
  %t_v571___ast_236_i_1308 = alloca i8
  store i8 0, ptr %t_v571___ast_236_i_1308
  %s_v572___ast_236_local = alloca i64
  store i64 0, ptr %s_v572___ast_236_local
  %t_v572___ast_236_local = alloca i8
  store i8 0, ptr %t_v572___ast_236_local
  %s_v573___ast_236_maximum_1307 = alloca i64
  store i64 0, ptr %s_v573___ast_236_maximum_1307
  %t_v573___ast_236_maximum_1307 = alloca i8
  store i8 0, ptr %t_v573___ast_236_maximum_1307
  %s_v574___ast_236_raw_1306 = alloca i64
  store i64 0, ptr %s_v574___ast_236_raw_1306
  %t_v574___ast_236_raw_1306 = alloca i8
  store i8 0, ptr %t_v574___ast_236_raw_1306
  %s_v575___ast_236_saved = alloca i64
  store i64 0, ptr %s_v575___ast_236_saved
  %t_v575___ast_236_saved = alloca i8
  store i8 0, ptr %t_v575___ast_236_saved
  %s_v576___ast_236_values = alloca i64
  store i64 0, ptr %s_v576___ast_236_values
  %t_v576___ast_236_values = alloca i8
  store i8 0, ptr %t_v576___ast_236_values
  %s_v577___ast_236_words = alloca i64
  store i64 0, ptr %s_v577___ast_236_words
  %t_v577___ast_236_words = alloca i8
  store i8 0, ptr %t_v577___ast_236_words
  store i64 17, ptr %s_v572___ast_236_local
  store i8 0, ptr %t_v572___ast_236_local
  %r0 = call i64 @chali_alloc(i64 8)
  store i64 %r0, ptr %s_v564___ast_236___chali_reference_storage_1303
  store i8 0, ptr %t_v564___ast_236___chali_reference_storage_1303
  %r1 = load i64, ptr %s_v572___ast_236_local
  %r2 = load i8, ptr %t_v572___ast_236_local
  %r3 = load i64, ptr %s_v564___ast_236___chali_reference_storage_1303
  %r4 = load i8, ptr %t_v564___ast_236___chali_reference_storage_1303
  %r5 = call ptr @chali_primitive_address(i64 %r3, i64 8)
  store i64 %r1, ptr %r5, align 1
  %r6 = load i64, ptr %s_v564___ast_236___chali_reference_storage_1303
  %r7 = load i8, ptr %t_v564___ast_236___chali_reference_storage_1303
  %r8 = call i64 @chali_require_reference(i64 %r6)
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 8, ptr @chali_source_line
  %r9 = call i64 @f_v235_program__referenceAddress(i64 %r8)
  %r10 = load i8, ptr @chali_return_float
  %r11 = load i64, ptr %s_v564___ast_236___chali_reference_storage_1303
  %r12 = load i8, ptr %t_v564___ast_236___chali_reference_storage_1303
  %r13 = call i64 @chali_require_reference(i64 %r11)
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 8, ptr @chali_source_line
  %r14 = icmp eq i64 %r9, %r13
  %r15 = zext i1 %r14 to i64
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 8, ptr @chali_source_line
  %r16 = call i64 @f_v162_std__lang__assert_bool(i64 %r15)
  %r17 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 9, ptr @chali_source_line
  %r18 = call i64 @f_v169_std__lang____bootBufferCapacity(i64 0, i64 0, i64 0, i64 8, i64 100)
  %r19 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 9, ptr @chali_source_line
  %r20 = icmp eq i64 %r18, 0
  %r21 = zext i1 %r20 to i64
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 9, ptr @chali_source_line
  %r22 = call i64 @f_v162_std__lang__assert_bool(i64 %r21)
  %r23 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 10, ptr @chali_source_line
  %r24 = call i64 @f_v169_std__lang____bootBufferCapacity(i64 0, i64 1, i64 0, i64 8, i64 100)
  %r25 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 10, ptr @chali_source_line
  %r26 = icmp eq i64 %r24, 8
  %r27 = zext i1 %r26 to i64
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 10, ptr @chali_source_line
  %r28 = call i64 @f_v162_std__lang__assert_bool(i64 %r27)
  %r29 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 11, ptr @chali_source_line
  %r30 = call i64 @f_v169_std__lang____bootBufferCapacity(i64 3, i64 5, i64 8, i64 8, i64 100)
  %r31 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 11, ptr @chali_source_line
  %r32 = icmp eq i64 %r30, 8
  %r33 = zext i1 %r32 to i64
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 11, ptr @chali_source_line
  %r34 = call i64 @f_v162_std__lang__assert_bool(i64 %r33)
  %r35 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 12, ptr @chali_source_line
  %r36 = call i64 @f_v169_std__lang____bootBufferCapacity(i64 8, i64 1, i64 8, i64 8, i64 100)
  %r37 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 12, ptr @chali_source_line
  %r38 = icmp eq i64 %r36, 16
  %r39 = zext i1 %r38 to i64
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 12, ptr @chali_source_line
  %r40 = call i64 @f_v162_std__lang__assert_bool(i64 %r39)
  %r41 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 13, ptr @chali_source_line
  %r42 = call i64 @f_v169_std__lang____bootBufferCapacity(i64 8, i64 40, i64 8, i64 8, i64 100)
  %r43 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 13, ptr @chali_source_line
  %r44 = icmp eq i64 %r42, 48
  %r45 = zext i1 %r44 to i64
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 13, ptr @chali_source_line
  %r46 = call i64 @f_v162_std__lang__assert_bool(i64 %r45)
  %r47 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 14, ptr @chali_source_line
  %r48 = call i64 @f_v169_std__lang____bootBufferCapacity(i64 60, i64 1, i64 60, i64 8, i64 100)
  %r49 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 14, ptr @chali_source_line
  %r50 = icmp eq i64 %r48, 100
  %r51 = zext i1 %r50 to i64
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 14, ptr @chali_source_line
  %r52 = call i64 @f_v162_std__lang__assert_bool(i64 %r51)
  %r53 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 15, ptr @chali_source_line
  %r54 = call i64 @f_v169_std__lang____bootBufferCapacity(i64 0, i64 1, i64 0, i64 3, i64 3)
  %r55 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 15, ptr @chali_source_line
  %r56 = icmp eq i64 %r54, 3
  %r57 = zext i1 %r56 to i64
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 15, ptr @chali_source_line
  %r58 = call i64 @f_v162_std__lang__assert_bool(i64 %r57)
  %r59 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 16, ptr @chali_source_line
  %r60 = call i64 @f_v169_std__lang____bootBufferCapacity(i64 9223372036854775806, i64 1, i64 9223372036854775806, i64 0, i64 9223372036854775807)
  %r61 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 16, ptr @chali_source_line
  %r62 = icmp eq i64 %r60, 9223372036854775807
  %r63 = zext i1 %r62 to i64
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 16, ptr @chali_source_line
  %r64 = call i64 @f_v162_std__lang__assert_bool(i64 %r63)
  %r65 = load i8, ptr @chali_return_float
  %r66 = or i64 0, 0
  %r67 = call i64 @chali_host_list_storage_scalar(i64 9, i64 0, i64 0, i64 0, i64 0, i64 0, i64 %r66, ptr %collection_result_kind)
  %r68 = load i8, ptr %collection_result_kind
  store i64 %r67, ptr %s_v576___ast_236_values
  store i8 %r68, ptr %t_v576___ast_236_values
  %r69 = call i64 @chali_alloc(i64 8)
  store i64 %r69, ptr %s_v565___ast_236___chali_reference_storage_1304
  store i8 0, ptr %t_v565___ast_236___chali_reference_storage_1304
  %r70 = load i64, ptr %s_v576___ast_236_values
  %r71 = load i8, ptr %t_v576___ast_236_values
  %r72 = load i64, ptr %s_v565___ast_236___chali_reference_storage_1304
  %r73 = load i8, ptr %t_v565___ast_236___chali_reference_storage_1304
  %r74 = call ptr @chali_primitive_address(i64 %r72, i64 8)
  store i64 %r70, ptr %r74, align 1
  store i64 0, ptr %s_v570___ast_236_handle_1305
  store i8 0, ptr %t_v570___ast_236_handle_1305
  %r75 = load i64, ptr %s_v565___ast_236___chali_reference_storage_1304
  %r76 = load i8, ptr %t_v565___ast_236___chali_reference_storage_1304
  %r77 = call i64 @chali_require_reference(i64 %r75)
  %r78 = call i64 @chali_require_reference(i64 %r77)
  store i64 %r78, ptr %s_v574___ast_236_raw_1306
  store i8 0, ptr %t_v574___ast_236_raw_1306
  %r79 = load i64, ptr %s_v574___ast_236_raw_1306
  %r80 = load i8, ptr %t_v574___ast_236_raw_1306
  %r81 = call i64 @chali_require_reference(i64 %r79)
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 20, ptr @chali_source_line
  %r82 = call ptr @chali_primitive_address(i64 %r81, i64 8)
  %r83 = load i64, ptr %r82, align 1
  store i64 %r83, ptr %s_v570___ast_236_handle_1305
  store i8 0, ptr %t_v570___ast_236_handle_1305
  %r84 = load i64, ptr %s_v570___ast_236_handle_1305
  %r85 = load i8, ptr %t_v570___ast_236_handle_1305
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 21, ptr @chali_source_line
  %r86 = zext i8 %r85 to i64
  %r87 = shl i64 %r86, 0
  %r88 = or i64 0, %r87
  %r89 = or i64 %r88, 0
  %r90 = call i64 @chali_host_list_storage_scalar(i64 17, i64 %r84, i64 0, i64 0, i64 0, i64 0, i64 %r89, ptr %collection_result_kind)
  %r91 = load i8, ptr %collection_result_kind
  store i64 %r90, ptr %s_v573___ast_236_maximum_1307
  store i8 %r91, ptr %t_v573___ast_236_maximum_1307
  %r92 = load i64, ptr %s_v573___ast_236_maximum_1307
  %r93 = load i8, ptr %t_v573___ast_236_maximum_1307
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 22, ptr @chali_source_line
  %r94 = icmp sgt i64 %r92, 8
  %r95 = zext i1 %r94 to i64
  %r96 = icmp ne i64 %r95, 0
  br i1 %r96, label %ast_block_1, label %ast_block_0
ast_block_0:
  br label %ast_block_2
ast_block_1:
  %r97 = load i64, ptr %s_v573___ast_236_maximum_1307
  %r98 = load i8, ptr %t_v573___ast_236_maximum_1307
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 22, ptr @chali_source_line
  %r99 = icmp slt i64 %r97, 9223372036854775807
  %r100 = zext i1 %r99 to i64
  %r101 = icmp ne i64 %r100, 0
  br label %ast_block_3
ast_block_3:
  br label %ast_block_2
ast_block_2:
  %r102 = phi i1 [ false, %ast_block_0 ], [ %r101, %ast_block_3 ]
  %r103 = zext i1 %r102 to i64
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 22, ptr @chali_source_line
  %r104 = call i64 @f_v162_std__lang__assert_bool(i64 %r103)
  %r105 = load i8, ptr @chali_return_float
  %r106 = load i64, ptr %s_v570___ast_236_handle_1305
  %r107 = load i8, ptr %t_v570___ast_236_handle_1305
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 23, ptr @chali_source_line
  %r108 = call i64 @f_v170_std__lang____bootListReserve(i64 %r106, i64 33)
  %r109 = load i8, ptr @chali_return_float
  %r110 = load i64, ptr %s_v570___ast_236_handle_1305
  %r111 = load i8, ptr %t_v570___ast_236_handle_1305
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 24, ptr @chali_source_line
  %r112 = zext i8 %r111 to i64
  %r113 = shl i64 %r112, 0
  %r114 = or i64 0, %r113
  %r115 = or i64 %r114, 0
  %r116 = call i64 @chali_host_list_storage_scalar(i64 1, i64 %r110, i64 0, i64 0, i64 0, i64 0, i64 %r115, ptr %collection_result_kind)
  %r117 = load i8, ptr %collection_result_kind
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 24, ptr @chali_source_line
  %r118 = icmp eq i64 %r116, 33
  %r119 = zext i1 %r118 to i64
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 24, ptr @chali_source_line
  %r120 = call i64 @f_v162_std__lang__assert_bool(i64 %r119)
  %r121 = load i8, ptr @chali_return_float
  store i64 0, ptr %s_v571___ast_236_i_1308
  store i8 0, ptr %t_v571___ast_236_i_1308
  br label %ast_label_v578___chali_loop_137
ast_label_v578___chali_loop_137:
  %r122 = load i64, ptr %s_v571___ast_236_i_1308
  %r123 = load i8, ptr %t_v571___ast_236_i_1308
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 27, ptr @chali_source_line
  %r124 = icmp slt i64 %r122, 33
  %r125 = zext i1 %r124 to i64
  %r126 = icmp ne i64 %r125, 0
  br i1 %r126, label %ast_block_4, label %ast_block_5
ast_block_4:
  %r127 = load i64, ptr %s_v565___ast_236___chali_reference_storage_1304
  %r128 = load i8, ptr %t_v565___ast_236___chali_reference_storage_1304
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 27, ptr @chali_source_line
  %r129 = call ptr @chali_primitive_address(i64 %r127, i64 8)
  %r130 = load i64, ptr %r129, align 1
  %r131 = load i64, ptr %s_v571___ast_236_i_1308
  %r132 = load i8, ptr %t_v571___ast_236_i_1308
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 27, ptr @chali_source_line
  %r133 = call i64 @f_v174_std__lang____bootListPushI(i64 %r130, i64 %r131, i64 0, i64 0)
  %r134 = load i8, ptr @chali_return_float
  %r135 = load i64, ptr %s_v571___ast_236_i_1308
  %r136 = load i8, ptr %t_v571___ast_236_i_1308
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 27, ptr @chali_source_line
  %r137 = add i64 %r135, 1
  store i64 %r137, ptr %s_v571___ast_236_i_1308
  store i8 0, ptr %t_v571___ast_236_i_1308
  br label %ast_block_6
ast_block_5:
  br label %ast_label_v579___chali_brk_138
ast_block_6:
  br label %ast_label_v578___chali_loop_137
ast_label_v579___chali_brk_138:
  %r138 = load i64, ptr %s_v565___ast_236___chali_reference_storage_1304
  %r139 = load i8, ptr %t_v565___ast_236___chali_reference_storage_1304
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 28, ptr @chali_source_line
  %r140 = call ptr @chali_primitive_address(i64 %r138, i64 8)
  %r141 = load i64, ptr %r140, align 1
  %r142 = call i64 @f_v173_std__lang____bootListCopy(i64 %r141)
  %r143 = load i8, ptr @chali_return_float
  store i64 %r142, ptr %s_v569___ast_236_copy
  store i8 %r143, ptr %t_v569___ast_236_copy
  %r144 = load i64, ptr %s_v565___ast_236___chali_reference_storage_1304
  %r145 = load i8, ptr %t_v565___ast_236___chali_reference_storage_1304
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 29, ptr @chali_source_line
  %r146 = call ptr @chali_primitive_address(i64 %r144, i64 8)
  %r147 = load i64, ptr %r146, align 1
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 29, ptr @chali_source_line
  %r148 = call i64 @f_v171_std__lang____bootListRemove(i64 %r147, i64 0)
  %r149 = load i8, ptr @chali_return_float
  %r150 = load i64, ptr %s_v565___ast_236___chali_reference_storage_1304
  %r151 = load i8, ptr %t_v565___ast_236___chali_reference_storage_1304
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 29, ptr @chali_source_line
  %r152 = call ptr @chali_primitive_address(i64 %r150, i64 8)
  %r153 = load i64, ptr %r152, align 1
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 29, ptr @chali_source_line
  %r154 = call i64 @f_v176_std__lang____bootListPopI(i64 %r153, i64 0, i64 0)
  %r155 = load i8, ptr @chali_return_float
  %r156 = load i64, ptr %s_v565___ast_236___chali_reference_storage_1304
  %r157 = load i8, ptr %t_v565___ast_236___chali_reference_storage_1304
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 29, ptr @chali_source_line
  %r158 = call ptr @chali_primitive_address(i64 %r156, i64 8)
  %r159 = load i64, ptr %r158, align 1
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 29, ptr @chali_source_line
  %r160 = call i64 @f_v172_std__lang____bootListClear(i64 %r159)
  %r161 = load i8, ptr @chali_return_float
  %r162 = load i64, ptr %s_v570___ast_236_handle_1305
  %r163 = load i8, ptr %t_v570___ast_236_handle_1305
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 30, ptr @chali_source_line
  %r164 = zext i8 %r163 to i64
  %r165 = shl i64 %r164, 0
  %r166 = or i64 0, %r165
  %r167 = or i64 %r166, 0
  %r168 = call i64 @chali_host_list_storage_scalar(i64 1, i64 %r162, i64 0, i64 0, i64 0, i64 0, i64 %r167, ptr %collection_result_kind)
  %r169 = load i8, ptr %collection_result_kind
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 30, ptr @chali_source_line
  %r170 = icmp eq i64 %r168, 33
  %r171 = zext i1 %r170 to i64
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 30, ptr @chali_source_line
  %r172 = call i64 @f_v162_std__lang__assert_bool(i64 %r171)
  %r173 = load i8, ptr @chali_return_float
  %r174 = load i64, ptr %s_v565___ast_236___chali_reference_storage_1304
  %r175 = load i8, ptr %t_v565___ast_236___chali_reference_storage_1304
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 31, ptr @chali_source_line
  %r176 = call ptr @chali_primitive_address(i64 %r174, i64 8)
  %r177 = load i64, ptr %r176, align 1
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 31, ptr @chali_source_line
  %r178 = call i64 @f_v174_std__lang____bootListPushI(i64 %r177, i64 99, i64 0, i64 0)
  %r179 = load i8, ptr @chali_return_float
  %r180 = load i64, ptr %s_v565___ast_236___chali_reference_storage_1304
  %r181 = load i8, ptr %t_v565___ast_236___chali_reference_storage_1304
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 32, ptr @chali_source_line
  %r182 = call ptr @chali_primitive_address(i64 %r180, i64 8)
  %r183 = load i64, ptr %r182, align 1
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 32, ptr @chali_source_line
  %r184 = or i64 0, 0
  %r185 = call i64 @chali_host_list_storage_scalar(i64 11, i64 %r183, i64 0, i64 0, i64 0, i64 0, i64 %r184, ptr %collection_result_kind)
  %r186 = load i8, ptr %collection_result_kind
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 32, ptr @chali_source_line
  %r187 = icmp eq i64 %r185, 99
  %r188 = zext i1 %r187 to i64
  %r189 = icmp ne i64 %r188, 0
  br i1 %r189, label %ast_block_8, label %ast_block_7
ast_block_7:
  br label %ast_block_9
ast_block_8:
  %r190 = load i64, ptr %s_v569___ast_236_copy
  %r191 = load i8, ptr %t_v569___ast_236_copy
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 32, ptr @chali_source_line
  %r192 = zext i8 %r191 to i64
  %r193 = shl i64 %r192, 0
  %r194 = or i64 0, %r193
  %r195 = or i64 %r194, 0
  %r196 = call i64 @chali_host_list_storage_scalar(i64 10, i64 %r190, i64 0, i64 0, i64 0, i64 0, i64 %r195, ptr %collection_result_kind)
  %r197 = load i8, ptr %collection_result_kind
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 32, ptr @chali_source_line
  %r198 = icmp eq i64 %r196, 33
  %r199 = zext i1 %r198 to i64
  %r200 = icmp ne i64 %r199, 0
  br label %ast_block_10
ast_block_10:
  br label %ast_block_9
ast_block_9:
  %r201 = phi i1 [ false, %ast_block_7 ], [ %r200, %ast_block_10 ]
  %r202 = zext i1 %r201 to i64
  %r203 = icmp ne i64 %r202, 0
  br i1 %r203, label %ast_block_12, label %ast_block_11
ast_block_11:
  br label %ast_block_13
ast_block_12:
  %r204 = load i64, ptr %s_v569___ast_236_copy
  %r205 = load i8, ptr %t_v569___ast_236_copy
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 32, ptr @chali_source_line
  %r206 = zext i8 %r205 to i64
  %r207 = shl i64 %r206, 0
  %r208 = or i64 0, %r207
  %r209 = or i64 %r208, 0
  %r210 = call i64 @chali_host_list_storage_scalar(i64 11, i64 %r204, i64 0, i64 0, i64 0, i64 0, i64 %r209, ptr %collection_result_kind)
  %r211 = load i8, ptr %collection_result_kind
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 32, ptr @chali_source_line
  %r212 = icmp eq i64 %r210, 0
  %r213 = zext i1 %r212 to i64
  %r214 = icmp ne i64 %r213, 0
  br label %ast_block_14
ast_block_14:
  br label %ast_block_13
ast_block_13:
  %r215 = phi i1 [ false, %ast_block_11 ], [ %r214, %ast_block_14 ]
  %r216 = zext i1 %r215 to i64
  %r217 = icmp ne i64 %r216, 0
  br i1 %r217, label %ast_block_16, label %ast_block_15
ast_block_15:
  br label %ast_block_17
ast_block_16:
  %r218 = load i64, ptr %s_v569___ast_236_copy
  %r219 = load i8, ptr %t_v569___ast_236_copy
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 32, ptr @chali_source_line
  %r220 = zext i8 %r219 to i64
  %r221 = shl i64 %r220, 0
  %r222 = or i64 0, %r221
  %r223 = or i64 %r222, 0
  %r224 = call i64 @chali_host_list_storage_scalar(i64 11, i64 %r218, i64 32, i64 0, i64 0, i64 0, i64 %r223, ptr %collection_result_kind)
  %r225 = load i8, ptr %collection_result_kind
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 32, ptr @chali_source_line
  %r226 = icmp eq i64 %r224, 32
  %r227 = zext i1 %r226 to i64
  %r228 = icmp ne i64 %r227, 0
  br label %ast_block_18
ast_block_18:
  br label %ast_block_17
ast_block_17:
  %r229 = phi i1 [ false, %ast_block_15 ], [ %r228, %ast_block_18 ]
  %r230 = zext i1 %r229 to i64
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 32, ptr @chali_source_line
  %r231 = call i64 @f_v162_std__lang__assert_bool(i64 %r230)
  %r232 = load i8, ptr @chali_return_float
  %r233 = load i64, ptr %s_v570___ast_236_handle_1305
  %r234 = load i8, ptr %t_v570___ast_236_handle_1305
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 33, ptr @chali_source_line
  %r235 = zext i8 %r234 to i64
  %r236 = shl i64 %r235, 0
  %r237 = or i64 0, %r236
  %r238 = or i64 %r237, 0
  %r239 = call i64 @chali_host_list_storage_scalar(i64 1, i64 %r233, i64 0, i64 0, i64 0, i64 0, i64 %r238, ptr %collection_result_kind)
  %r240 = load i8, ptr %collection_result_kind
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 33, ptr @chali_source_line
  %r241 = icmp eq i64 %r239, 33
  %r242 = zext i1 %r241 to i64
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 33, ptr @chali_source_line
  %r243 = call i64 @f_v162_std__lang__assert_bool(i64 %r242)
  %r244 = load i8, ptr @chali_return_float
  %r245 = or i64 0, 0
  %r246 = call i64 @chali_host_list_storage_scalar(i64 9, i64 0, i64 0, i64 0, i64 0, i64 0, i64 %r245, ptr %collection_result_kind)
  %r247 = load i8, ptr %collection_result_kind
  store i64 %r246, ptr %s_v577___ast_236_words
  store i8 %r247, ptr %t_v577___ast_236_words
  %r248 = load i64, ptr %s_v577___ast_236_words
  %r249 = load i8, ptr %t_v577___ast_236_words
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 34, ptr @chali_source_line
  %r250 = call i64 @f_v174_std__lang____bootListPushI(i64 %r248, i64 8, i64 1, i64 0)
  %r251 = load i8, ptr @chali_return_float
  %r252 = load i64, ptr %s_v577___ast_236_words
  %r253 = load i8, ptr %t_v577___ast_236_words
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 34, ptr @chali_source_line
  %r254 = call i64 @f_v174_std__lang____bootListPushI(i64 %r252, i64 16, i64 1, i64 0)
  %r255 = load i8, ptr @chali_return_float
  %r256 = load i64, ptr %s_v577___ast_236_words
  %r257 = load i8, ptr %t_v577___ast_236_words
  %r258 = call i64 @f_v173_std__lang____bootListCopy(i64 %r256)
  %r259 = load i8, ptr @chali_return_float
  store i64 %r258, ptr %s_v575___ast_236_saved
  store i8 %r259, ptr %t_v575___ast_236_saved
  %r260 = load i64, ptr %s_v577___ast_236_words
  %r261 = load i8, ptr %t_v577___ast_236_words
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 35, ptr @chali_source_line
  %r262 = call i64 @f_v172_std__lang____bootListClear(i64 %r260)
  %r263 = load i8, ptr @chali_return_float
  %r264 = load i64, ptr %s_v577___ast_236_words
  %r265 = load i8, ptr %t_v577___ast_236_words
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 35, ptr @chali_source_line
  %r266 = call i64 @f_v174_std__lang____bootListPushI(i64 %r264, i64 24, i64 1, i64 0)
  %r267 = load i8, ptr @chali_return_float
  %r268 = load i64, ptr %s_v566___ast_236___chali_tmp_s_1309
  %r269 = load i8, ptr %t_v566___ast_236___chali_tmp_s_1309
  %r270 = icmp ne i64 %r268, 0
  %r271 = zext i1 %r270 to i64
  %r272 = icmp ne i64 %r271, 0
  br i1 %r272, label %ast_block_19, label %ast_block_20
ast_block_19:
  %r273 = load i64, ptr %s_v566___ast_236___chali_tmp_s_1309
  %r274 = load i8, ptr %t_v566___ast_236___chali_tmp_s_1309
  %r275 = call i64 @f_v61_std__lang____bootStringDrop(i64 %r273)
  %r276 = load i8, ptr @chali_return_float
  br label %ast_block_21
ast_block_20:
  br label %ast_block_21
ast_block_21:
  store i64 0, ptr %s_v566___ast_236___chali_tmp_s_1309
  store i8 0, ptr %t_v566___ast_236___chali_tmp_s_1309
  %r277 = load i64, ptr %s_v575___ast_236_saved
  %r278 = load i8, ptr %t_v575___ast_236_saved
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 36, ptr @chali_source_line
  %r279 = zext i8 %r278 to i64
  %r280 = shl i64 %r279, 0
  %r281 = or i64 0, %r280
  %r282 = or i64 %r281, 0
  %r283 = call i64 @chali_host_list_storage_scalar(i64 11, i64 %r277, i64 0, i64 1, i64 0, i64 0, i64 %r282, ptr %collection_result_kind)
  %r284 = load i8, ptr %collection_result_kind
  store i64 %r283, ptr %s_v566___ast_236___chali_tmp_s_1309
  store i8 %r284, ptr %t_v566___ast_236___chali_tmp_s_1309
  %r285 = load i64, ptr %s_v566___ast_236___chali_tmp_s_1309
  %r286 = load i8, ptr %t_v566___ast_236___chali_tmp_s_1309
  %r287 = call i64 @f_v64_std__lang____bootStringCompare(i64 %r285, i64 8)
  %r288 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 36, ptr @chali_source_line
  %r289 = icmp eq i64 %r287, 0
  %r290 = zext i1 %r289 to i64
  %r291 = icmp ne i64 %r290, 0
  br i1 %r291, label %ast_block_23, label %ast_block_22
ast_block_22:
  br label %ast_block_24
ast_block_23:
  %r292 = load i64, ptr %s_v567___ast_236___chali_tmp_s_1310
  %r293 = load i8, ptr %t_v567___ast_236___chali_tmp_s_1310
  %r294 = icmp ne i64 %r292, 0
  %r295 = zext i1 %r294 to i64
  %r296 = icmp ne i64 %r295, 0
  br i1 %r296, label %ast_block_25, label %ast_block_26
ast_block_25:
  %r297 = load i64, ptr %s_v567___ast_236___chali_tmp_s_1310
  %r298 = load i8, ptr %t_v567___ast_236___chali_tmp_s_1310
  %r299 = call i64 @f_v61_std__lang____bootStringDrop(i64 %r297)
  %r300 = load i8, ptr @chali_return_float
  br label %ast_block_27
ast_block_26:
  br label %ast_block_27
ast_block_27:
  store i64 0, ptr %s_v567___ast_236___chali_tmp_s_1310
  store i8 0, ptr %t_v567___ast_236___chali_tmp_s_1310
  %r301 = load i64, ptr %s_v575___ast_236_saved
  %r302 = load i8, ptr %t_v575___ast_236_saved
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 36, ptr @chali_source_line
  %r303 = zext i8 %r302 to i64
  %r304 = shl i64 %r303, 0
  %r305 = or i64 0, %r304
  %r306 = or i64 %r305, 0
  %r307 = call i64 @chali_host_list_storage_scalar(i64 11, i64 %r301, i64 1, i64 1, i64 0, i64 0, i64 %r306, ptr %collection_result_kind)
  %r308 = load i8, ptr %collection_result_kind
  store i64 %r307, ptr %s_v567___ast_236___chali_tmp_s_1310
  store i8 %r308, ptr %t_v567___ast_236___chali_tmp_s_1310
  %r309 = load i64, ptr %s_v567___ast_236___chali_tmp_s_1310
  %r310 = load i8, ptr %t_v567___ast_236___chali_tmp_s_1310
  %r311 = call i64 @f_v64_std__lang____bootStringCompare(i64 %r309, i64 16)
  %r312 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 36, ptr @chali_source_line
  %r313 = icmp eq i64 %r311, 0
  %r314 = zext i1 %r313 to i64
  %r315 = icmp ne i64 %r314, 0
  br label %ast_block_28
ast_block_28:
  br label %ast_block_24
ast_block_24:
  %r316 = phi i1 [ false, %ast_block_22 ], [ %r315, %ast_block_28 ]
  %r317 = zext i1 %r316 to i64
  %r318 = icmp ne i64 %r317, 0
  br i1 %r318, label %ast_block_30, label %ast_block_29
ast_block_29:
  br label %ast_block_31
ast_block_30:
  %r319 = load i64, ptr %s_v568___ast_236___chali_tmp_s_1311
  %r320 = load i8, ptr %t_v568___ast_236___chali_tmp_s_1311
  %r321 = icmp ne i64 %r319, 0
  %r322 = zext i1 %r321 to i64
  %r323 = icmp ne i64 %r322, 0
  br i1 %r323, label %ast_block_32, label %ast_block_33
ast_block_32:
  %r324 = load i64, ptr %s_v568___ast_236___chali_tmp_s_1311
  %r325 = load i8, ptr %t_v568___ast_236___chali_tmp_s_1311
  %r326 = call i64 @f_v61_std__lang____bootStringDrop(i64 %r324)
  %r327 = load i8, ptr @chali_return_float
  br label %ast_block_34
ast_block_33:
  br label %ast_block_34
ast_block_34:
  store i64 0, ptr %s_v568___ast_236___chali_tmp_s_1311
  store i8 0, ptr %t_v568___ast_236___chali_tmp_s_1311
  %r328 = load i64, ptr %s_v577___ast_236_words
  %r329 = load i8, ptr %t_v577___ast_236_words
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 36, ptr @chali_source_line
  %r330 = zext i8 %r329 to i64
  %r331 = shl i64 %r330, 0
  %r332 = or i64 0, %r331
  %r333 = or i64 %r332, 0
  %r334 = call i64 @chali_host_list_storage_scalar(i64 11, i64 %r328, i64 0, i64 1, i64 0, i64 0, i64 %r333, ptr %collection_result_kind)
  %r335 = load i8, ptr %collection_result_kind
  store i64 %r334, ptr %s_v568___ast_236___chali_tmp_s_1311
  store i8 %r335, ptr %t_v568___ast_236___chali_tmp_s_1311
  %r336 = load i64, ptr %s_v568___ast_236___chali_tmp_s_1311
  %r337 = load i8, ptr %t_v568___ast_236___chali_tmp_s_1311
  %r338 = call i64 @f_v64_std__lang____bootStringCompare(i64 %r336, i64 24)
  %r339 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 36, ptr @chali_source_line
  %r340 = icmp eq i64 %r338, 0
  %r341 = zext i1 %r340 to i64
  %r342 = icmp ne i64 %r341, 0
  br label %ast_block_35
ast_block_35:
  br label %ast_block_31
ast_block_31:
  %r343 = phi i1 [ false, %ast_block_29 ], [ %r342, %ast_block_35 ]
  %r344 = zext i1 %r343 to i64
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 36, ptr @chali_source_line
  %r345 = call i64 @f_v162_std__lang__assert_bool(i64 %r344)
  %r346 = load i8, ptr @chali_return_float
  %r347 = load i64, ptr %s_v568___ast_236___chali_tmp_s_1311
  %r348 = load i8, ptr %t_v568___ast_236___chali_tmp_s_1311
  %r349 = icmp ne i64 %r347, 0
  %r350 = zext i1 %r349 to i64
  %r351 = icmp ne i64 %r350, 0
  br i1 %r351, label %ast_block_36, label %ast_block_37
ast_block_36:
  %r352 = load i64, ptr %s_v568___ast_236___chali_tmp_s_1311
  %r353 = load i8, ptr %t_v568___ast_236___chali_tmp_s_1311
  %r354 = call i64 @f_v61_std__lang____bootStringDrop(i64 %r352)
  %r355 = load i8, ptr @chali_return_float
  br label %ast_block_38
ast_block_37:
  br label %ast_block_38
ast_block_38:
  store i64 0, ptr %s_v568___ast_236___chali_tmp_s_1311
  store i8 0, ptr %t_v568___ast_236___chali_tmp_s_1311
  %r356 = load i64, ptr %s_v567___ast_236___chali_tmp_s_1310
  %r357 = load i8, ptr %t_v567___ast_236___chali_tmp_s_1310
  %r358 = icmp ne i64 %r356, 0
  %r359 = zext i1 %r358 to i64
  %r360 = icmp ne i64 %r359, 0
  br i1 %r360, label %ast_block_39, label %ast_block_40
ast_block_39:
  %r361 = load i64, ptr %s_v567___ast_236___chali_tmp_s_1310
  %r362 = load i8, ptr %t_v567___ast_236___chali_tmp_s_1310
  %r363 = call i64 @f_v61_std__lang____bootStringDrop(i64 %r361)
  %r364 = load i8, ptr @chali_return_float
  br label %ast_block_41
ast_block_40:
  br label %ast_block_41
ast_block_41:
  store i64 0, ptr %s_v567___ast_236___chali_tmp_s_1310
  store i8 0, ptr %t_v567___ast_236___chali_tmp_s_1310
  %r365 = load i64, ptr %s_v566___ast_236___chali_tmp_s_1309
  %r366 = load i8, ptr %t_v566___ast_236___chali_tmp_s_1309
  %r367 = icmp ne i64 %r365, 0
  %r368 = zext i1 %r367 to i64
  %r369 = icmp ne i64 %r368, 0
  br i1 %r369, label %ast_block_42, label %ast_block_43
ast_block_42:
  %r370 = load i64, ptr %s_v566___ast_236___chali_tmp_s_1309
  %r371 = load i8, ptr %t_v566___ast_236___chali_tmp_s_1309
  %r372 = call i64 @f_v61_std__lang____bootStringDrop(i64 %r370)
  %r373 = load i8, ptr @chali_return_float
  br label %ast_block_44
ast_block_43:
  br label %ast_block_44
ast_block_44:
  store i64 0, ptr %s_v566___ast_236___chali_tmp_s_1309
  store i8 0, ptr %t_v566___ast_236___chali_tmp_s_1309
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 37, ptr @chali_source_line
  %r374 = call ptr @chali_standard_stream(i32 1)
  %r375 = getelementptr [52 x i8], ptr @chali_abi_lit0, i64 0, i64 0
  %r376 = call i64 @fwrite(ptr %r375, i64 1, i64 52, ptr %r374)
  %r377 = load i64, ptr %s_v575___ast_236_saved
  %r378 = load i8, ptr %t_v575___ast_236_saved
  %r379 = icmp ne i64 %r377, 0
  %r380 = zext i1 %r379 to i64
  %r381 = icmp ne i64 %r380, 0
  br i1 %r381, label %ast_block_45, label %ast_block_46
ast_block_45:
  %r382 = load i64, ptr %s_v575___ast_236_saved
  %r383 = load i8, ptr %t_v575___ast_236_saved
  call void @chali_owned_drop(i64 %r382)
  br label %ast_block_47
ast_block_46:
  br label %ast_block_47
ast_block_47:
  store i64 0, ptr %s_v575___ast_236_saved
  store i8 0, ptr %t_v575___ast_236_saved
  %r384 = load i64, ptr %s_v577___ast_236_words
  %r385 = load i8, ptr %t_v577___ast_236_words
  %r386 = icmp ne i64 %r384, 0
  %r387 = zext i1 %r386 to i64
  %r388 = icmp ne i64 %r387, 0
  br i1 %r388, label %ast_block_48, label %ast_block_49
ast_block_48:
  %r389 = load i64, ptr %s_v577___ast_236_words
  %r390 = load i8, ptr %t_v577___ast_236_words
  call void @chali_owned_drop(i64 %r389)
  br label %ast_block_50
ast_block_49:
  br label %ast_block_50
ast_block_50:
  store i64 0, ptr %s_v577___ast_236_words
  store i8 0, ptr %t_v577___ast_236_words
  %r391 = load i64, ptr %s_v569___ast_236_copy
  %r392 = load i8, ptr %t_v569___ast_236_copy
  %r393 = icmp ne i64 %r391, 0
  %r394 = zext i1 %r393 to i64
  %r395 = icmp ne i64 %r394, 0
  br i1 %r395, label %ast_block_51, label %ast_block_52
ast_block_51:
  %r396 = load i64, ptr %s_v569___ast_236_copy
  %r397 = load i8, ptr %t_v569___ast_236_copy
  call void @chali_owned_drop(i64 %r396)
  br label %ast_block_53
ast_block_52:
  br label %ast_block_53
ast_block_53:
  store i64 0, ptr %s_v569___ast_236_copy
  store i8 0, ptr %t_v569___ast_236_copy
  %r398 = load i64, ptr %s_v565___ast_236___chali_reference_storage_1304
  %r399 = load i8, ptr %t_v565___ast_236___chali_reference_storage_1304
  %r400 = call ptr @chali_primitive_address(i64 %r398, i64 8)
  %r401 = load i64, ptr %r400, align 1
  %r402 = icmp ne i64 %r401, 0
  %r403 = zext i1 %r402 to i64
  %r404 = icmp ne i64 %r403, 0
  br i1 %r404, label %ast_block_54, label %ast_block_55
ast_block_54:
  %r405 = load i64, ptr %s_v565___ast_236___chali_reference_storage_1304
  %r406 = load i8, ptr %t_v565___ast_236___chali_reference_storage_1304
  %r407 = call ptr @chali_primitive_address(i64 %r405, i64 8)
  %r408 = load i64, ptr %r407, align 1
  call void @chali_owned_drop(i64 %r408)
  br label %ast_block_56
ast_block_55:
  br label %ast_block_56
ast_block_56:
  %r409 = load i64, ptr %s_v565___ast_236___chali_reference_storage_1304
  %r410 = load i8, ptr %t_v565___ast_236___chali_reference_storage_1304
  %r411 = call ptr @chali_primitive_address(i64 %r409, i64 8)
  store i64 0, ptr %r411, align 1
  %r412 = load i64, ptr %s_v564___ast_236___chali_reference_storage_1303
  %r413 = load i8, ptr %t_v564___ast_236___chali_reference_storage_1303
  %r414 = icmp ne i64 %r412, 0
  %r415 = zext i1 %r414 to i64
  %r416 = icmp ne i64 %r415, 0
  br i1 %r416, label %ast_block_57, label %ast_block_58
ast_block_57:
  %r417 = load i64, ptr %s_v564___ast_236___chali_reference_storage_1303
  %r418 = load i8, ptr %t_v564___ast_236___chali_reference_storage_1303
  call void @chali_owned_drop(i64 %r417)
  br label %ast_block_59
ast_block_58:
  br label %ast_block_59
ast_block_59:
  store i64 0, ptr %s_v564___ast_236___chali_reference_storage_1303
  store i8 0, ptr %t_v564___ast_236___chali_reference_storage_1303
  %r419 = load i64, ptr %s_v565___ast_236___chali_reference_storage_1304
  %r420 = load i8, ptr %t_v565___ast_236___chali_reference_storage_1304
  %r421 = icmp ne i64 %r419, 0
  %r422 = zext i1 %r421 to i64
  %r423 = icmp ne i64 %r422, 0
  br i1 %r423, label %ast_block_60, label %ast_block_61
ast_block_60:
  %r424 = load i64, ptr %s_v565___ast_236___chali_reference_storage_1304
  %r425 = load i8, ptr %t_v565___ast_236___chali_reference_storage_1304
  call void @chali_owned_drop(i64 %r424)
  br label %ast_block_62
ast_block_61:
  br label %ast_block_62
ast_block_62:
  store i64 0, ptr %s_v565___ast_236___chali_reference_storage_1304
  store i8 0, ptr %t_v565___ast_236___chali_reference_storage_1304
  store i8 0, ptr @chali_return_float
  ret i64 0
ast_label_v580___chali_cleanup_main:
  %r426 = load i64, ptr %s_v575___ast_236_saved
  %r427 = load i8, ptr %t_v575___ast_236_saved
  %r428 = icmp ne i64 %r426, 0
  %r429 = zext i1 %r428 to i64
  %r430 = icmp ne i64 %r429, 0
  br i1 %r430, label %ast_block_63, label %ast_block_64
ast_block_63:
  %r431 = load i64, ptr %s_v575___ast_236_saved
  %r432 = load i8, ptr %t_v575___ast_236_saved
  call void @chali_owned_drop(i64 %r431)
  br label %ast_block_65
ast_block_64:
  br label %ast_block_65
ast_block_65:
  store i64 0, ptr %s_v575___ast_236_saved
  store i8 0, ptr %t_v575___ast_236_saved
  %r433 = load i64, ptr %s_v577___ast_236_words
  %r434 = load i8, ptr %t_v577___ast_236_words
  %r435 = icmp ne i64 %r433, 0
  %r436 = zext i1 %r435 to i64
  %r437 = icmp ne i64 %r436, 0
  br i1 %r437, label %ast_block_66, label %ast_block_67
ast_block_66:
  %r438 = load i64, ptr %s_v577___ast_236_words
  %r439 = load i8, ptr %t_v577___ast_236_words
  call void @chali_owned_drop(i64 %r438)
  br label %ast_block_68
ast_block_67:
  br label %ast_block_68
ast_block_68:
  store i64 0, ptr %s_v577___ast_236_words
  store i8 0, ptr %t_v577___ast_236_words
  %r440 = load i64, ptr %s_v569___ast_236_copy
  %r441 = load i8, ptr %t_v569___ast_236_copy
  %r442 = icmp ne i64 %r440, 0
  %r443 = zext i1 %r442 to i64
  %r444 = icmp ne i64 %r443, 0
  br i1 %r444, label %ast_block_69, label %ast_block_70
ast_block_69:
  %r445 = load i64, ptr %s_v569___ast_236_copy
  %r446 = load i8, ptr %t_v569___ast_236_copy
  call void @chali_owned_drop(i64 %r445)
  br label %ast_block_71
ast_block_70:
  br label %ast_block_71
ast_block_71:
  store i64 0, ptr %s_v569___ast_236_copy
  store i8 0, ptr %t_v569___ast_236_copy
  %r447 = load i64, ptr %s_v565___ast_236___chali_reference_storage_1304
  %r448 = load i8, ptr %t_v565___ast_236___chali_reference_storage_1304
  %r449 = icmp ne i64 %r447, 0
  %r450 = zext i1 %r449 to i64
  %r451 = icmp ne i64 %r450, 0
  br i1 %r451, label %ast_block_72, label %ast_block_73
ast_block_72:
  %r452 = load i64, ptr %s_v565___ast_236___chali_reference_storage_1304
  %r453 = load i8, ptr %t_v565___ast_236___chali_reference_storage_1304
  call void @chali_owned_drop(i64 %r452)
  br label %ast_block_74
ast_block_73:
  br label %ast_block_74
ast_block_74:
  store i64 0, ptr %s_v565___ast_236___chali_reference_storage_1304
  store i8 0, ptr %t_v565___ast_236___chali_reference_storage_1304
  %r454 = load i64, ptr %s_v565___ast_236___chali_reference_storage_1304
  %r455 = load i8, ptr %t_v565___ast_236___chali_reference_storage_1304
  %r456 = call ptr @chali_primitive_address(i64 %r454, i64 8)
  %r457 = load i64, ptr %r456, align 1
  %r458 = icmp ne i64 %r457, 0
  %r459 = zext i1 %r458 to i64
  %r460 = icmp ne i64 %r459, 0
  br i1 %r460, label %ast_block_75, label %ast_block_76
ast_block_75:
  %r461 = load i64, ptr %s_v565___ast_236___chali_reference_storage_1304
  %r462 = load i8, ptr %t_v565___ast_236___chali_reference_storage_1304
  %r463 = call ptr @chali_primitive_address(i64 %r461, i64 8)
  %r464 = load i64, ptr %r463, align 1
  call void @chali_owned_drop(i64 %r464)
  br label %ast_block_77
ast_block_76:
  br label %ast_block_77
ast_block_77:
  %r465 = load i64, ptr %s_v565___ast_236___chali_reference_storage_1304
  %r466 = load i8, ptr %t_v565___ast_236___chali_reference_storage_1304
  %r467 = call ptr @chali_primitive_address(i64 %r465, i64 8)
  store i64 0, ptr %r467, align 1
  %r468 = load i64, ptr %s_v564___ast_236___chali_reference_storage_1303
  %r469 = load i8, ptr %t_v564___ast_236___chali_reference_storage_1303
  %r470 = icmp ne i64 %r468, 0
  %r471 = zext i1 %r470 to i64
  %r472 = icmp ne i64 %r471, 0
  br i1 %r472, label %ast_block_78, label %ast_block_79
ast_block_78:
  %r473 = load i64, ptr %s_v564___ast_236___chali_reference_storage_1303
  %r474 = load i8, ptr %t_v564___ast_236___chali_reference_storage_1303
  call void @chali_owned_drop(i64 %r473)
  br label %ast_block_80
ast_block_79:
  br label %ast_block_80
ast_block_80:
  store i64 0, ptr %s_v564___ast_236___chali_reference_storage_1303
  store i8 0, ptr %t_v564___ast_236___chali_reference_storage_1303
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define internal i64 @f_v271_std__lang__pointerFromAddress__int64(i64 %p0) alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [4 x i64]
  %native_argument_types = alloca [4 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v581___ast_271___chali_ret_v_1402 = alloca i64
  store i64 0, ptr %s_v581___ast_271___chali_ret_v_1402
  %t_v581___ast_271___chali_ret_v_1402 = alloca i8
  store i8 0, ptr %t_v581___ast_271___chali_ret_v_1402
  %s_v582___ast_271_address_626 = alloca i64
  store i64 0, ptr %s_v582___ast_271_address_626
  %t_v582___ast_271_address_626 = alloca i8
  store i8 0, ptr %t_v582___ast_271_address_626
  %s_v583___ast_271_result_1401 = alloca i64
  store i64 0, ptr %s_v583___ast_271_result_1401
  %t_v583___ast_271_result_1401 = alloca i8
  store i8 0, ptr %t_v583___ast_271_result_1401
  store i64 %p0, ptr %s_v582___ast_271_address_626
  %r0 = call i64 @chali_alloc(i64 8)
  store i64 %r0, ptr %s_v583___ast_271_result_1401
  store i8 0, ptr %t_v583___ast_271_result_1401
  %r1 = load i64, ptr %s_v583___ast_271_result_1401
  %r2 = load i8, ptr %t_v583___ast_271_result_1401
  call void @chali_host_memory_prepare(i64 8, i64 %r1, i64 0, i64 8, ptr %raw_memory_output, ptr %raw_memory_input)
  %r3 = load ptr, ptr %raw_memory_output
  %r4 = trunc i64 0 to i8
  call void @llvm.memset.p0.i64(ptr %r3, i8 %r4, i64 8, i1 false)
  %r5 = load i64, ptr %s_v583___ast_271_result_1401
  %r6 = load i8, ptr %t_v583___ast_271_result_1401
  %r7 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r5, ptr %r7
  %r8 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r6, ptr %r8
  %r9 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 8, ptr %r9
  %r10 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r10
  %r11 = call i64 @chali_host_call(ptr @native_name1, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r12 = call i8 @chali_host_result_float()
  %r13 = load i64, ptr %s_v582___ast_271_address_626
  %r14 = load i8, ptr %t_v582___ast_271_address_626
  %r15 = load i64, ptr %s_v583___ast_271_result_1401
  %r16 = load i8, ptr %t_v583___ast_271_result_1401
  %r17 = mul i64 0, 8
  %r18 = add i64 %r15, %r17
  %r19 = call ptr @chali_primitive_address(i64 %r18, i64 8)
  store i64 %r13, ptr %r19, align 1
  store i64 200, ptr %s_v581___ast_271___chali_ret_v_1402
  store i8 0, ptr %t_v581___ast_271___chali_ret_v_1402
  %r20 = load i64, ptr %s_v581___ast_271___chali_ret_v_1402
  %r21 = load i8, ptr %t_v581___ast_271___chali_ret_v_1402
  %r22 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r20, ptr %r22
  %r23 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r21, ptr %r23
  %r24 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 8, ptr %r24
  %r25 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r25
  %r26 = call i64 @chali_host_call(ptr @native_name1, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r27 = call i8 @chali_host_result_float()
  %r28 = load i64, ptr %s_v581___ast_271___chali_ret_v_1402
  %r29 = load i8, ptr %t_v581___ast_271___chali_ret_v_1402
  %r30 = load i64, ptr %s_v583___ast_271_result_1401
  %r31 = load i8, ptr %t_v583___ast_271_result_1401
  %r32 = call i64 @chali_host_copy_words(i64 %r28, i64 %r30, i64 1)
  %r33 = load i64, ptr %s_v581___ast_271___chali_ret_v_1402
  %r34 = load i8, ptr %t_v581___ast_271___chali_ret_v_1402
  %r35 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r33, ptr %r35
  %r36 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r34, ptr %r36
  %r37 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 8, ptr %r37
  %r38 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r38
  %r39 = call i64 @chali_host_call(ptr @native_name1, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r40 = call i8 @chali_host_result_float()
  %r41 = load i64, ptr %s_v583___ast_271_result_1401
  %r42 = load i8, ptr %t_v583___ast_271_result_1401
  %r43 = icmp ne i64 %r41, 0
  %r44 = zext i1 %r43 to i64
  %r45 = icmp ne i64 %r44, 0
  br i1 %r45, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r46 = load i64, ptr %s_v583___ast_271_result_1401
  %r47 = load i8, ptr %t_v583___ast_271_result_1401
  call void @chali_owned_drop(i64 %r46)
  br label %ast_block_2
ast_block_1:
  br label %ast_block_2
ast_block_2:
  store i64 0, ptr %s_v583___ast_271_result_1401
  store i8 0, ptr %t_v583___ast_271_result_1401
  %r48 = load i64, ptr %s_v581___ast_271___chali_ret_v_1402
  %r49 = load i8, ptr %t_v581___ast_271___chali_ret_v_1402
  store i8 %r49, ptr @chali_return_float
  ret i64 %r48
ast_block_3:
  %r50 = load i64, ptr %s_v583___ast_271_result_1401
  %r51 = load i8, ptr %t_v583___ast_271_result_1401
  %r52 = icmp ne i64 %r50, 0
  %r53 = zext i1 %r52 to i64
  %r54 = icmp ne i64 %r53, 0
  br i1 %r54, label %ast_block_4, label %ast_block_5
ast_block_4:
  %r55 = load i64, ptr %s_v583___ast_271_result_1401
  %r56 = load i8, ptr %t_v583___ast_271_result_1401
  call void @chali_owned_drop(i64 %r55)
  br label %ast_block_6
ast_block_5:
  br label %ast_block_6
ast_block_6:
  store i64 0, ptr %s_v583___ast_271_result_1401
  store i8 0, ptr %t_v583___ast_271_result_1401
  store i8 0, ptr @chali_return_float
  ret i64 0
ast_label_v584___chali_cleanup_std__lang__pointerFromAddress__int64:
  %r57 = load i64, ptr %s_v583___ast_271_result_1401
  %r58 = load i8, ptr %t_v583___ast_271_result_1401
  %r59 = icmp ne i64 %r57, 0
  %r60 = zext i1 %r59 to i64
  %r61 = icmp ne i64 %r60, 0
  br i1 %r61, label %ast_block_7, label %ast_block_8
ast_block_7:
  %r62 = load i64, ptr %s_v583___ast_271_result_1401
  %r63 = load i8, ptr %t_v583___ast_271_result_1401
  call void @chali_owned_drop(i64 %r62)
  br label %ast_block_9
ast_block_8:
  br label %ast_block_9
ast_block_9:
  store i64 0, ptr %s_v583___ast_271_result_1401
  store i8 0, ptr %t_v583___ast_271_result_1401
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define internal i64 @f_v273_std__lang__pointerFromAddress__uint8(i64 %p0) alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [4 x i64]
  %native_argument_types = alloca [4 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v585___ast_273___chali_ret_v_1406 = alloca i64
  store i64 0, ptr %s_v585___ast_273___chali_ret_v_1406
  %t_v585___ast_273___chali_ret_v_1406 = alloca i8
  store i8 0, ptr %t_v585___ast_273___chali_ret_v_1406
  %s_v586___ast_273_address_630 = alloca i64
  store i64 0, ptr %s_v586___ast_273_address_630
  %t_v586___ast_273_address_630 = alloca i8
  store i8 0, ptr %t_v586___ast_273_address_630
  %s_v587___ast_273_result_1405 = alloca i64
  store i64 0, ptr %s_v587___ast_273_result_1405
  %t_v587___ast_273_result_1405 = alloca i8
  store i8 0, ptr %t_v587___ast_273_result_1405
  store i64 %p0, ptr %s_v586___ast_273_address_630
  %r0 = call i64 @chali_alloc(i64 8)
  store i64 %r0, ptr %s_v587___ast_273_result_1405
  store i8 0, ptr %t_v587___ast_273_result_1405
  %r1 = load i64, ptr %s_v587___ast_273_result_1405
  %r2 = load i8, ptr %t_v587___ast_273_result_1405
  call void @chali_host_memory_prepare(i64 8, i64 %r1, i64 0, i64 8, ptr %raw_memory_output, ptr %raw_memory_input)
  %r3 = load ptr, ptr %raw_memory_output
  %r4 = trunc i64 0 to i8
  call void @llvm.memset.p0.i64(ptr %r3, i8 %r4, i64 8, i1 false)
  %r5 = load i64, ptr %s_v587___ast_273_result_1405
  %r6 = load i8, ptr %t_v587___ast_273_result_1405
  %r7 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r5, ptr %r7
  %r8 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r6, ptr %r8
  %r9 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 8, ptr %r9
  %r10 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r10
  %r11 = call i64 @chali_host_call(ptr @native_name1, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r12 = call i8 @chali_host_result_float()
  %r13 = load i64, ptr %s_v586___ast_273_address_630
  %r14 = load i8, ptr %t_v586___ast_273_address_630
  %r15 = load i64, ptr %s_v587___ast_273_result_1405
  %r16 = load i8, ptr %t_v587___ast_273_result_1405
  %r17 = mul i64 0, 8
  %r18 = add i64 %r15, %r17
  %r19 = call ptr @chali_primitive_address(i64 %r18, i64 8)
  store i64 %r13, ptr %r19, align 1
  store i64 208, ptr %s_v585___ast_273___chali_ret_v_1406
  store i8 0, ptr %t_v585___ast_273___chali_ret_v_1406
  %r20 = load i64, ptr %s_v585___ast_273___chali_ret_v_1406
  %r21 = load i8, ptr %t_v585___ast_273___chali_ret_v_1406
  %r22 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r20, ptr %r22
  %r23 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r21, ptr %r23
  %r24 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 8, ptr %r24
  %r25 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r25
  %r26 = call i64 @chali_host_call(ptr @native_name1, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r27 = call i8 @chali_host_result_float()
  %r28 = load i64, ptr %s_v585___ast_273___chali_ret_v_1406
  %r29 = load i8, ptr %t_v585___ast_273___chali_ret_v_1406
  %r30 = load i64, ptr %s_v587___ast_273_result_1405
  %r31 = load i8, ptr %t_v587___ast_273_result_1405
  %r32 = call i64 @chali_host_copy_words(i64 %r28, i64 %r30, i64 1)
  %r33 = load i64, ptr %s_v585___ast_273___chali_ret_v_1406
  %r34 = load i8, ptr %t_v585___ast_273___chali_ret_v_1406
  %r35 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r33, ptr %r35
  %r36 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r34, ptr %r36
  %r37 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 8, ptr %r37
  %r38 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r38
  %r39 = call i64 @chali_host_call(ptr @native_name1, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r40 = call i8 @chali_host_result_float()
  %r41 = load i64, ptr %s_v587___ast_273_result_1405
  %r42 = load i8, ptr %t_v587___ast_273_result_1405
  %r43 = icmp ne i64 %r41, 0
  %r44 = zext i1 %r43 to i64
  %r45 = icmp ne i64 %r44, 0
  br i1 %r45, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r46 = load i64, ptr %s_v587___ast_273_result_1405
  %r47 = load i8, ptr %t_v587___ast_273_result_1405
  call void @chali_owned_drop(i64 %r46)
  br label %ast_block_2
ast_block_1:
  br label %ast_block_2
ast_block_2:
  store i64 0, ptr %s_v587___ast_273_result_1405
  store i8 0, ptr %t_v587___ast_273_result_1405
  %r48 = load i64, ptr %s_v585___ast_273___chali_ret_v_1406
  %r49 = load i8, ptr %t_v585___ast_273___chali_ret_v_1406
  store i8 %r49, ptr @chali_return_float
  ret i64 %r48
ast_block_3:
  %r50 = load i64, ptr %s_v587___ast_273_result_1405
  %r51 = load i8, ptr %t_v587___ast_273_result_1405
  %r52 = icmp ne i64 %r50, 0
  %r53 = zext i1 %r52 to i64
  %r54 = icmp ne i64 %r53, 0
  br i1 %r54, label %ast_block_4, label %ast_block_5
ast_block_4:
  %r55 = load i64, ptr %s_v587___ast_273_result_1405
  %r56 = load i8, ptr %t_v587___ast_273_result_1405
  call void @chali_owned_drop(i64 %r55)
  br label %ast_block_6
ast_block_5:
  br label %ast_block_6
ast_block_6:
  store i64 0, ptr %s_v587___ast_273_result_1405
  store i8 0, ptr %t_v587___ast_273_result_1405
  store i8 0, ptr @chali_return_float
  ret i64 0
ast_label_v588___chali_cleanup_std__lang__pointerFromAddress__uint8:
  %r57 = load i64, ptr %s_v587___ast_273_result_1405
  %r58 = load i8, ptr %t_v587___ast_273_result_1405
  %r59 = icmp ne i64 %r57, 0
  %r60 = zext i1 %r59 to i64
  %r61 = icmp ne i64 %r60, 0
  br i1 %r61, label %ast_block_7, label %ast_block_8
ast_block_7:
  %r62 = load i64, ptr %s_v587___ast_273_result_1405
  %r63 = load i8, ptr %t_v587___ast_273_result_1405
  call void @chali_owned_drop(i64 %r62)
  br label %ast_block_9
ast_block_8:
  br label %ast_block_9
ast_block_9:
  store i64 0, ptr %s_v587___ast_273_result_1405
  store i8 0, ptr %t_v587___ast_273_result_1405
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define internal i64 @f_v468___chali_copy_struct(i64 %p0, i64 %p1, i64 %p2) alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [1 x i64]
  %native_argument_types = alloca [1 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v589___ast_468___chali_dst = alloca i64
  store i64 0, ptr %s_v589___ast_468___chali_dst
  %t_v589___ast_468___chali_dst = alloca i8
  store i8 0, ptr %t_v589___ast_468___chali_dst
  %s_v590___ast_468___chali_k = alloca i64
  store i64 0, ptr %s_v590___ast_468___chali_k
  %t_v590___ast_468___chali_k = alloca i8
  store i8 0, ptr %t_v590___ast_468___chali_k
  %s_v591___ast_468___chali_slots = alloca i64
  store i64 0, ptr %s_v591___ast_468___chali_slots
  %t_v591___ast_468___chali_slots = alloca i8
  store i8 0, ptr %t_v591___ast_468___chali_slots
  %s_v592___ast_468___chali_src = alloca i64
  store i64 0, ptr %s_v592___ast_468___chali_src
  %t_v592___ast_468___chali_src = alloca i8
  store i8 0, ptr %t_v592___ast_468___chali_src
  store i64 %p0, ptr %s_v589___ast_468___chali_dst
  store i64 %p1, ptr %s_v592___ast_468___chali_src
  store i64 %p2, ptr %s_v591___ast_468___chali_slots
  store i64 0, ptr %s_v590___ast_468___chali_k
  store i8 0, ptr %t_v590___ast_468___chali_k
  br label %ast_block_0
ast_block_0:
  %r0 = load i64, ptr %s_v590___ast_468___chali_k
  %r1 = load i8, ptr %t_v590___ast_468___chali_k
  %r2 = load i64, ptr %s_v591___ast_468___chali_slots
  %r3 = load i8, ptr %t_v591___ast_468___chali_slots
  %r4 = icmp slt i64 %r0, %r2
  %r5 = zext i1 %r4 to i64
  %r6 = icmp ne i64 %r5, 0
  br i1 %r6, label %ast_block_1, label %ast_block_2
ast_block_1:
  %r7 = load i64, ptr %s_v592___ast_468___chali_src
  %r8 = load i8, ptr %t_v592___ast_468___chali_src
  %r9 = load i64, ptr %s_v590___ast_468___chali_k
  %r10 = load i8, ptr %t_v590___ast_468___chali_k
  %r11 = mul i64 %r9, 8
  %r12 = add i64 %r7, %r11
  %r13 = call ptr @chali_primitive_address(i64 %r12, i64 8)
  %r14 = load i64, ptr %r13, align 1
  %r15 = load i64, ptr %s_v589___ast_468___chali_dst
  %r16 = load i8, ptr %t_v589___ast_468___chali_dst
  %r17 = load i64, ptr %s_v590___ast_468___chali_k
  %r18 = load i8, ptr %t_v590___ast_468___chali_k
  %r19 = mul i64 %r17, 8
  %r20 = add i64 %r15, %r19
  %r21 = call ptr @chali_primitive_address(i64 %r20, i64 8)
  store i64 %r14, ptr %r21, align 1
  %r22 = load i64, ptr %s_v590___ast_468___chali_k
  %r23 = load i8, ptr %t_v590___ast_468___chali_k
  %r24 = add i64 %r22, 1
  %r25 = trunc i64 %r24 to i32
  %r26 = sext i32 %r25 to i64
  store i64 %r26, ptr %s_v590___ast_468___chali_k
  store i8 0, ptr %t_v590___ast_468___chali_k
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
define i32 @main(i32 %argc, ptr %argv) {
entry:
  call void @chali_init()
  call void @chali_host_process_arguments(i32 %argc, ptr %argv)
  %result = call i64 @f_main()
  call void @chali_finish()
  %code = trunc i64 %result to i32
  ret i32 %code
}
