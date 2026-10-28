; ModuleID = 'source/aot/runtime.c'
source_filename = "source/aot/runtime.c"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-w64-windows-gnu"

@.str = private unnamed_addr constant [15 x i8] c"chali aot: %s\0A\00", align 1
@.str.1 = private unnamed_addr constant [55 x i8] c"\E7\A7\BB\E4\BD\8D\E4\BD\8D\E6\95\B0\E8\B6\85\E5\87\BA\E6\95\B4\E6\95\B0\E4\BD\8D\E5\AE\BD\EF\BC\88\E6\9C\89\E6\95\88\E8\8C\83\E5\9B\B4 0..63\EF\BC\89\00", align 1
@.str.2 = private unnamed_addr constant [13 x i8] c"\E6\B5\AE\E7\82\B9\E9\99\A4\E9\9B\B6\00", align 1
@.str.3 = private unnamed_addr constant [13 x i8] c"\E6\95\B4\E6\95\B0\E9\99\A4\E9\9B\B6\00", align 1
@.str.4 = private unnamed_addr constant [72 x i8] c"\E6\95\B4\E6\95\B0\E9\99\A4\E6\B3\95\E6\BA\A2\E5\87\BA\EF\BC\9A\E6\9C\80\E5\B0\8F\E8\B4\9F\E6\95\B0\E9\99\A4\E4\BB\A5 -1 \E7\9A\84\E7\BB\93\E6\9E\9C\E8\B6\85\E5\87\BA int64 \E8\8C\83\E5\9B\B4\00", align 1
@.str.5 = private unnamed_addr constant [22 x i8] c"String\EF\BC\9A\E7\BB\93\E6\9E\9C\E8\BF\87\E9\95\BF\00", align 1
@.str.6 = private unnamed_addr constant [31 x i8] c"\E5\A4\8D\E5\88\B6\E5\AD\97\E7\AC\A6\E4\B8\B2\E6\97\B6\E5\86\85\E5\AD\98\E4\B8\8D\E8\B6\B3\00", align 1
@chali_data_size = external local_unnamed_addr constant i64, align 8
@.str.7 = private unnamed_addr constant [37 x i8] c"%s\E8\B6\8A\E7\95\8C\EF\BC\9A\E4\B8\8B\E6\A0\87 %lld\EF\BC\8C\E9\95\BF\E5\BA\A6 %lld\00", align 1
@.str.8 = private unnamed_addr constant [15 x i8] c"%s\EF\BC\9A\E7\A9\BA\E6\8C\87\E9\92\88\00", align 1
@chali_fmt_at = internal unnamed_addr global i64 0, align 8
@chali_fmt_pos = internal unnamed_addr global i64 0, align 8
@.str.10 = private unnamed_addr constant [8 x i8] c"hlLqjzt\00", align 1
@.str.11 = private unnamed_addr constant [6 x i8] c"%s.*s\00", align 1
@.str.12 = private unnamed_addr constant [9 x i8] c"gGeEfFaA\00", align 1
@.str.13 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str.14 = private unnamed_addr constant [3 x i8] c".3\00", align 1
@.str.15 = private unnamed_addr constant [5 x i8] c"%s%c\00", align 1
@.str.16 = private unnamed_addr constant [7 x i8] c"%sll%c\00", align 1
@.str.17 = private unnamed_addr constant [6 x i8] c"%slld\00", align 1
@.str.19 = private unnamed_addr constant [34 x i8] c"\E8\AF\BB\E5\8F\96\E6\A0\87\E5\87\86\E8\BE\93\E5\85\A5\E6\97\B6\E5\86\85\E5\AD\98\E4\B8\8D\E8\B6\B3\00", align 1
@.str.20 = private unnamed_addr constant [65 x i8] c"readLine \E4\B8\8D\E6\8E\A5\E5\8F\97\E9\9B\B6\E5\AD\97\E8\8A\82\EF\BC\8CString \E4\B8\8D\E8\83\BD\E5\8C\85\E5\90\AB\E5\86\85\E5\B5\8C\E9\9B\B6\E5\AD\97\E8\8A\82\00", align 1
@.str.21 = private unnamed_addr constant [22 x i8] c"\E6\A0\87\E5\87\86\E8\BE\93\E5\85\A5\E8\A1\8C\E8\BF\87\E9\95\BF\00", align 1
@.str.22 = private unnamed_addr constant [5 x i8] c"%.*f\00", align 1
@.str.23 = private unnamed_addr constant [25 x i8] c"\E6\B5\AE\E7\82\B9\E6\95\B0\E6\A0\BC\E5\BC\8F\E5\8C\96\E5\A4\B1\E8\B4\A5\00", align 1
@.str.24 = private unnamed_addr constant [4 x i8] c"%lf\00", align 1
@.str.25 = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@.str.26 = private unnamed_addr constant [3 x i8] c"%g\00", align 1
@.str.27 = private unnamed_addr constant [7 x i8] c"\E4\B8\8B\E6\A0\87\00", align 1
@.str.28 = private unnamed_addr constant [7 x i8] c"\E8\AE\BF\E9\97\AE\00", align 1
@.str.29 = private unnamed_addr constant [59 x i8] c"\E5\BC\95\E7\94\A8\E5\B0\9A\E6\9C\AA\E7\BB\91\E5\AE\9A\E6\9C\89\E6\95\88\E5\85\83\E7\B4\A0\EF\BC\9B\E7\BC\BA\E7\9C\81\E5\80\BC\E8\AF\B7\E4\BD\BF\E7\94\A8 Option\00", align 1
@.str.30 = private unnamed_addr constant [6 x i8] c"panic\00", align 1

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
define dso_local i64 @chali_string_new(ptr nofree noundef readonly captures(none) %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = icmp ugt i64 %1, 9223372036854775774
  br i1 %3, label %4, label %5

4:                                                ; preds = %2
  tail call void @chali_panic(ptr noundef nonnull @.str.5)
  unreachable

5:                                                ; preds = %2
  %6 = icmp eq i64 %1, 0
  %7 = tail call i64 @llvm.umax.i64(i64 %1, i64 1)
  %8 = tail call ptr @malloc(i64 noundef %7) #19
  %9 = icmp eq ptr %8, null
  br i1 %9, label %10, label %11

10:                                               ; preds = %5
  tail call void @chali_panic(ptr noundef nonnull @.str.6)
  unreachable

11:                                               ; preds = %5
  br i1 %6, label %13, label %12

12:                                               ; preds = %11
  tail call void @llvm.memcpy.p0.p0.i64(ptr nonnull align 1 %8, ptr align 1 %0, i64 %1, i1 false)
  br label %13

13:                                               ; preds = %12, %11
  %14 = icmp ult i64 %1, 17
  %15 = select i1 %14, i64 49, i64 32
  %16 = tail call i64 @chali_host_allocate(i64 noundef %15) #17
  br i1 %14, label %17, label %19

17:                                               ; preds = %13
  %18 = add nsw i64 %16, 32
  br label %22

19:                                               ; preds = %13
  %20 = add nuw nsw i64 %1, 1
  %21 = tail call i64 @chali_host_allocate(i64 noundef %20) #17
  br label %22

22:                                               ; preds = %19, %17
  %23 = phi i64 [ 16, %17 ], [ %1, %19 ]
  %24 = phi i64 [ %18, %17 ], [ %21, %19 ]
  %25 = tail call ptr @chali_host_address(i64 noundef %16, i64 noundef 32) #17
  store i64 -1, ptr %25, align 1
  %26 = getelementptr inbounds nuw i8, ptr %25, i64 8
  store i64 %1, ptr %26, align 1
  %27 = getelementptr inbounds nuw i8, ptr %25, i64 16
  store i64 %23, ptr %27, align 1
  %28 = getelementptr inbounds nuw i8, ptr %25, i64 24
  store i64 %24, ptr %28, align 1
  br i1 %6, label %31, label %29

29:                                               ; preds = %22
  %30 = tail call ptr @chali_host_address(i64 noundef %24, i64 noundef %1) #17
  tail call void @llvm.memcpy.p0.p0.i64(ptr align 1 %30, ptr nonnull align 1 %8, i64 %1, i1 false)
  br label %31

31:                                               ; preds = %29, %22
  %32 = add nsw i64 %24, %1
  %33 = tail call ptr @chali_host_address(i64 noundef %32, i64 noundef 1) #17
  store i8 0, ptr %33, align 1, !tbaa !12
  tail call void @free(ptr noundef %8)
  ret i64 %16
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
  br i1 %2, label %17, label %3

3:                                                ; preds = %1
  %4 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %5 = icmp slt i64 %0, %4
  br i1 %5, label %6, label %9

6:                                                ; preds = %3
  %7 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #17
  %8 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %7) #17
  br label %17

9:                                                ; preds = %3
  %10 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
  %11 = load i64, ptr %10, align 1
  %12 = icmp eq i64 %11, -1
  br i1 %12, label %13, label %17

13:                                               ; preds = %9
  %14 = add nsw i64 %0, 8
  %15 = tail call ptr @chali_host_address(i64 noundef %14, i64 noundef 8) #17
  %16 = load i64, ptr %15, align 1
  br label %17

17:                                               ; preds = %13, %9, %1, %6
  %18 = phi i64 [ %8, %6 ], [ 0, %1 ], [ %16, %13 ], [ %11, %9 ]
  ret i64 %18
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare dso_local i64 @strlen(ptr noundef captures(none)) local_unnamed_addr #9

; Function Attrs: nounwind uwtable
define dso_local ptr @chali_string_bytes(i64 noundef %0) local_unnamed_addr #0 {
  %2 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %3 = icmp slt i64 %0, %2
  br i1 %3, label %14, label %4

4:                                                ; preds = %1
  %5 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
  %6 = load i64, ptr %5, align 1
  %7 = icmp eq i64 %6, -1
  br i1 %7, label %8, label %12

8:                                                ; preds = %4
  %9 = add nsw i64 %0, 24
  %10 = tail call ptr @chali_host_address(i64 noundef %9, i64 noundef 8) #17
  %11 = load i64, ptr %10, align 1
  br label %14

12:                                               ; preds = %4
  %13 = add nsw i64 %0, 8
  br label %14

14:                                               ; preds = %1, %8, %12
  %15 = phi i64 [ %13, %12 ], [ %11, %8 ], [ %0, %1 ]
  %16 = tail call ptr @chali_host_address(i64 noundef %15, i64 noundef 0) #17
  ret ptr %16
}

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_string_literal(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #17
  %3 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %2) #17
  %4 = tail call i64 @chali_string_new(ptr noundef nonnull %2, i64 noundef %3)
  ret i64 %4
}

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_string_copy(i64 noundef %0) local_unnamed_addr #0 {
  %2 = icmp eq i64 %0, 0
  br i1 %2, label %33, label %3

3:                                                ; preds = %1
  %4 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %5 = icmp slt i64 %0, %4
  br i1 %5, label %16, label %6

6:                                                ; preds = %3
  %7 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
  %8 = load i64, ptr %7, align 1
  %9 = icmp eq i64 %8, -1
  br i1 %9, label %10, label %14

10:                                               ; preds = %6
  %11 = add nsw i64 %0, 24
  %12 = tail call ptr @chali_host_address(i64 noundef %11, i64 noundef 8) #17
  %13 = load i64, ptr %12, align 1
  br label %16

14:                                               ; preds = %6
  %15 = add nsw i64 %0, 8
  br label %16

16:                                               ; preds = %3, %10, %14
  %17 = phi i64 [ %15, %14 ], [ %13, %10 ], [ %0, %3 ]
  %18 = tail call ptr @chali_host_address(i64 noundef %17, i64 noundef 0) #17
  br i1 %5, label %19, label %22

19:                                               ; preds = %16
  %20 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #17
  %21 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %20) #17
  br label %30

22:                                               ; preds = %16
  %23 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
  %24 = load i64, ptr %23, align 1
  %25 = icmp eq i64 %24, -1
  br i1 %25, label %26, label %30

26:                                               ; preds = %22
  %27 = add nsw i64 %0, 8
  %28 = tail call ptr @chali_host_address(i64 noundef %27, i64 noundef 8) #17
  %29 = load i64, ptr %28, align 1
  br label %30

30:                                               ; preds = %19, %22, %26
  %31 = phi i64 [ %21, %19 ], [ %24, %22 ], [ %29, %26 ]
  %32 = tail call i64 @chali_string_new(ptr noundef %18, i64 noundef %31)
  br label %33

33:                                               ; preds = %1, %30
  %34 = phi i64 [ %32, %30 ], [ 0, %1 ]
  ret i64 %34
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_string_drop(i64 noundef %0) local_unnamed_addr #0 {
  %2 = icmp eq i64 %0, 0
  %3 = load i64, ptr @chali_data_size, align 8
  %4 = icmp slt i64 %0, %3
  %5 = select i1 %2, i1 true, i1 %4
  br i1 %5, label %17, label %6

6:                                                ; preds = %1
  %7 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
  %8 = load i64, ptr %7, align 1
  %9 = icmp eq i64 %8, -1
  br i1 %9, label %10, label %17

10:                                               ; preds = %6
  %11 = add nsw i64 %0, 24
  %12 = tail call ptr @chali_host_address(i64 noundef %11, i64 noundef 8) #17
  %13 = load i64, ptr %12, align 1
  %14 = add nsw i64 %0, 32
  %15 = icmp eq i64 %13, %14
  br i1 %15, label %17, label %16

16:                                               ; preds = %10
  tail call void @chali_host_drop(i64 noundef %13) #17
  br label %17

17:                                               ; preds = %10, %16, %6, %1
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
  %9 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %4, i64 noundef 128, ptr noundef nonnull @.str.7, ptr noundef %2, i64 noundef %0, i64 noundef %1) #17
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
  %6 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i64 noundef 128, ptr noundef nonnull @.str.8, ptr noundef %1) #17
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
  br i1 %5, label %16, label %6

6:                                                ; preds = %2
  %7 = tail call ptr @chali_host_address(i64 noundef %3, i64 noundef 8) #17
  %8 = load i64, ptr %7, align 1
  %9 = icmp eq i64 %8, -1
  br i1 %9, label %10, label %14

10:                                               ; preds = %6
  %11 = add nsw i64 %3, 24
  %12 = tail call ptr @chali_host_address(i64 noundef %11, i64 noundef 8) #17
  %13 = load i64, ptr %12, align 1
  br label %16

14:                                               ; preds = %6
  %15 = add nsw i64 %3, 8
  br label %16

16:                                               ; preds = %2, %10, %14
  %17 = phi i64 [ %15, %14 ], [ %13, %10 ], [ %3, %2 ]
  %18 = tail call ptr @chali_host_address(i64 noundef %17, i64 noundef 0) #17
  %19 = load i64, ptr @chali_fmt_pos, align 8, !tbaa !13
  br label %20

20:                                               ; preds = %28, %16
  %21 = phi i64 [ %19, %16 ], [ %29, %28 ]
  %22 = getelementptr inbounds nuw i8, ptr %18, i64 %21
  %23 = load i8, ptr %22, align 1, !tbaa !12
  switch i8 %23, label %28 [
    i8 0, label %30
    i8 37, label %30
    i8 123, label %24
  ]

24:                                               ; preds = %20
  %25 = getelementptr i8, ptr %22, i64 1
  %26 = load i8, ptr %25, align 1, !tbaa !12
  %27 = icmp eq i8 %26, 125
  br i1 %27, label %30, label %28

28:                                               ; preds = %20, %24
  %29 = add i64 %21, 1
  br label %20, !llvm.loop !15

30:                                               ; preds = %20, %20, %24
  %31 = icmp ugt i64 %21, %19
  br i1 %31, label %32, label %37

32:                                               ; preds = %30
  %33 = getelementptr inbounds nuw i8, ptr %18, i64 %19
  %34 = sub nuw i64 %21, %19
  %35 = tail call ptr @__acrt_iob_func(i32 noundef 1) #17
  %36 = tail call i64 @fwrite(ptr noundef %33, i64 noundef 1, i64 noundef %34, ptr noundef %35)
  store i64 %21, ptr @chali_fmt_pos, align 8, !tbaa !13
  br label %37

37:                                               ; preds = %32, %30
  %38 = load i8, ptr %22, align 1, !tbaa !12
  switch i8 %38, label %83 [
    i8 0, label %39
    i8 123, label %40
  ]

39:                                               ; preds = %37
  store i8 0, ptr %0, align 1, !tbaa !12
  br label %142

40:                                               ; preds = %37
  %41 = add i64 %21, 1
  br label %42

42:                                               ; preds = %46, %40
  %43 = phi i64 [ %41, %40 ], [ %47, %46 ]
  %44 = getelementptr inbounds nuw i8, ptr %18, i64 %43
  %45 = load i8, ptr %44, align 1, !tbaa !12
  switch i8 %45, label %46 [
    i8 0, label %48
    i8 125, label %48
  ]

46:                                               ; preds = %42
  %47 = add i64 %43, 1
  br label %42, !llvm.loop !18

48:                                               ; preds = %42, %42
  %49 = getelementptr inbounds nuw i8, ptr %18, i64 %43
  %50 = icmp eq i8 %45, 125
  %51 = zext i1 %50 to i64
  %52 = add i64 %43, %51
  store i64 %52, ptr @chali_fmt_pos, align 8, !tbaa !13
  store i8 37, ptr %0, align 1, !tbaa !12
  %53 = getelementptr inbounds nuw i8, ptr %0, i64 1
  store i8 0, ptr %53, align 1, !tbaa !12
  %54 = getelementptr inbounds nuw i8, ptr %18, i64 %41
  %55 = load i8, ptr %54, align 1, !tbaa !12
  %56 = icmp eq i8 %55, 58
  %57 = add i64 %21, 2
  %58 = select i1 %56, i64 %57, i64 %41
  %59 = icmp ugt i64 %43, %58
  br i1 %59, label %60, label %142

60:                                               ; preds = %48
  %61 = add i64 %58, 1
  %62 = icmp ult i64 %61, %43
  %63 = icmp ugt i64 %1, 3
  %64 = and i1 %62, %63
  br i1 %64, label %70, label %65

65:                                               ; preds = %70, %60
  %66 = phi i64 [ 1, %60 ], [ %76, %70 ]
  %67 = getelementptr inbounds nuw i8, ptr %0, i64 %66
  store i8 0, ptr %67, align 1, !tbaa !12
  %68 = getelementptr i8, ptr %49, i64 -1
  %69 = load i8, ptr %68, align 1, !tbaa !12
  br label %142

70:                                               ; preds = %60, %70
  %71 = phi i64 [ %78, %70 ], [ %61, %60 ]
  %72 = phi i64 [ %71, %70 ], [ %58, %60 ]
  %73 = phi i64 [ %76, %70 ], [ 1, %60 ]
  %74 = getelementptr inbounds nuw i8, ptr %18, i64 %72
  %75 = load i8, ptr %74, align 1, !tbaa !12
  %76 = add nuw i64 %73, 1
  %77 = getelementptr inbounds nuw i8, ptr %0, i64 %73
  store i8 %75, ptr %77, align 1, !tbaa !12
  %78 = add nuw i64 %71, 1
  %79 = icmp ult i64 %78, %43
  %80 = add nuw i64 %73, 3
  %81 = icmp ult i64 %80, %1
  %82 = and i1 %79, %81
  br i1 %82, label %70, label %65, !llvm.loop !19

83:                                               ; preds = %37
  %84 = add i64 %21, 1
  %85 = getelementptr inbounds nuw i8, ptr %18, i64 %84
  %86 = load i8, ptr %85, align 1, !tbaa !12
  %87 = icmp eq i8 %86, 37
  br i1 %87, label %88, label %92

88:                                               ; preds = %83
  %89 = tail call ptr @__acrt_iob_func(i32 noundef 1) #17
  %90 = tail call i32 @fputc(i32 noundef 37, ptr noundef %89)
  %91 = add i64 %21, 2
  store i64 %91, ptr @chali_fmt_pos, align 8, !tbaa !13
  store i8 0, ptr %0, align 1, !tbaa !12
  br label %142

92:                                               ; preds = %83
  store i8 37, ptr %0, align 1, !tbaa !12
  %93 = getelementptr inbounds nuw i8, ptr %18, i64 %84
  %94 = load i8, ptr %93, align 1, !tbaa !12
  %95 = icmp eq i8 %94, 0
  br i1 %95, label %116, label %96

96:                                               ; preds = %92, %109
  %97 = phi i8 [ %114, %109 ], [ %94, %92 ]
  %98 = phi i64 [ %111, %109 ], [ 1, %92 ]
  %99 = phi i64 [ %110, %109 ], [ %84, %92 ]
  %100 = zext nneg i8 %97 to i64
  %101 = icmp ult i8 %97, 64
  %102 = shl nuw i64 1, %100
  %103 = and i64 %102, 288063289038995457
  %104 = icmp ne i64 %103, 0
  %105 = select i1 %101, i1 %104, i1 false
  %106 = add nuw i64 %98, 2
  %107 = icmp ult i64 %106, %1
  %108 = select i1 %105, i1 %107, i1 false
  br i1 %108, label %109, label %116

109:                                              ; preds = %96
  %110 = add i64 %99, 1
  %111 = add nuw i64 %98, 1
  %112 = getelementptr inbounds nuw i8, ptr %0, i64 %98
  store i8 %97, ptr %112, align 1, !tbaa !12
  %113 = getelementptr inbounds nuw i8, ptr %18, i64 %110
  %114 = load i8, ptr %113, align 1, !tbaa !12
  %115 = icmp eq i8 %114, 0
  br i1 %115, label %116, label %96, !llvm.loop !20

116:                                              ; preds = %109, %96, %92
  %117 = phi i64 [ %84, %92 ], [ %110, %109 ], [ %99, %96 ]
  %118 = phi i64 [ 1, %92 ], [ %111, %109 ], [ %98, %96 ]
  %119 = getelementptr inbounds nuw i8, ptr %18, i64 %117
  %120 = load i8, ptr %119, align 1, !tbaa !12
  %121 = icmp eq i8 %120, 0
  br i1 %121, label %133, label %122

122:                                              ; preds = %116, %128
  %123 = phi i8 [ %131, %128 ], [ %120, %116 ]
  %124 = phi i64 [ %129, %128 ], [ %117, %116 ]
  %125 = sext i8 %123 to i32
  %126 = tail call ptr @memchr(ptr noundef nonnull dereferenceable(1) @.str.10, i32 %125, i64 8)
  %127 = icmp eq ptr %126, null
  br i1 %127, label %133, label %128

128:                                              ; preds = %122
  %129 = add i64 %124, 1
  %130 = getelementptr inbounds nuw i8, ptr %18, i64 %129
  %131 = load i8, ptr %130, align 1, !tbaa !12
  %132 = icmp eq i8 %131, 0
  br i1 %132, label %133, label %122, !llvm.loop !21

133:                                              ; preds = %122, %128, %116
  %134 = phi i64 [ %117, %116 ], [ %124, %122 ], [ %129, %128 ]
  %135 = getelementptr inbounds nuw i8, ptr %18, i64 %134
  %136 = getelementptr inbounds nuw i8, ptr %0, i64 %118
  store i8 0, ptr %136, align 1, !tbaa !12
  %137 = load i8, ptr %135, align 1, !tbaa !12
  %138 = icmp eq i8 %137, 0
  br i1 %138, label %139, label %140

139:                                              ; preds = %133
  store i64 %134, ptr @chali_fmt_pos, align 8, !tbaa !13
  br label %142

140:                                              ; preds = %133
  %141 = add i64 %134, 1
  store i64 %141, ptr @chali_fmt_pos, align 8, !tbaa !13
  br label %142

142:                                              ; preds = %88, %140, %139, %65, %48, %39
  %143 = phi i8 [ 0, %39 ], [ 63, %48 ], [ %69, %65 ], [ 37, %88 ], [ 0, %139 ], [ %137, %140 ]
  %144 = zext i8 %143 to i32
  ret i32 %144
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
  switch i32 %0, label %51 [
    i32 115, label %7
    i32 99, label %43
    i32 63, label %48
  ]

7:                                                ; preds = %4
  %8 = icmp eq i64 %2, 0
  br i1 %8, label %76, label %9

9:                                                ; preds = %7
  %10 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef 64, ptr noundef nonnull @.str.11, ptr noundef %1) #17
  %11 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %12 = icmp slt i64 %2, %11
  br i1 %12, label %13, label %16

13:                                               ; preds = %9
  %14 = tail call ptr @chali_host_address(i64 noundef %2, i64 noundef 1) #17
  %15 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %14) #17
  br label %24

16:                                               ; preds = %9
  %17 = tail call ptr @chali_host_address(i64 noundef %2, i64 noundef 8) #17
  %18 = load i64, ptr %17, align 1
  %19 = icmp eq i64 %18, -1
  br i1 %19, label %20, label %24

20:                                               ; preds = %16
  %21 = add nsw i64 %2, 8
  %22 = tail call ptr @chali_host_address(i64 noundef %21, i64 noundef 8) #17
  %23 = load i64, ptr %22, align 1
  br label %24

24:                                               ; preds = %13, %16, %20
  %25 = phi i64 [ %15, %13 ], [ %18, %16 ], [ %23, %20 ]
  %26 = trunc i64 %25 to i32
  %27 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %28 = icmp slt i64 %2, %27
  br i1 %28, label %39, label %29

29:                                               ; preds = %24
  %30 = tail call ptr @chali_host_address(i64 noundef %2, i64 noundef 8) #17
  %31 = load i64, ptr %30, align 1
  %32 = icmp eq i64 %31, -1
  br i1 %32, label %33, label %37

33:                                               ; preds = %29
  %34 = add nsw i64 %2, 24
  %35 = tail call ptr @chali_host_address(i64 noundef %34, i64 noundef 8) #17
  %36 = load i64, ptr %35, align 1
  br label %39

37:                                               ; preds = %29
  %38 = add nsw i64 %2, 8
  br label %39

39:                                               ; preds = %24, %33, %37
  %40 = phi i64 [ %38, %37 ], [ %36, %33 ], [ %2, %24 ]
  %41 = tail call ptr @chali_host_address(i64 noundef %40, i64 noundef 0) #17
  %42 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %5, i32 noundef %26, ptr noundef %41)
  br label %76

43:                                               ; preds = %4
  %44 = trunc i64 %2 to i32
  %45 = and i32 %44, 255
  %46 = tail call ptr @__acrt_iob_func(i32 noundef 1) #17
  %47 = tail call i32 @fputc(i32 noundef %45, ptr noundef %46)
  br label %76

48:                                               ; preds = %4
  %49 = icmp eq i32 %3, 0
  %50 = select i1 %49, i32 100, i32 103
  br label %51

51:                                               ; preds = %4, %48
  %52 = phi i32 [ %50, %48 ], [ %0, %4 ]
  %53 = tail call ptr @memchr(ptr noundef nonnull dereferenceable(1) @.str.12, i32 %52, i64 9)
  %54 = icmp eq ptr %53, null
  br i1 %54, label %69, label %55

55:                                               ; preds = %51
  %56 = bitcast i64 %2 to double
  %57 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %6, i64 noundef 40, ptr noundef nonnull @.str.13, ptr noundef %1) #17
  %58 = call ptr @strchr(ptr noundef nonnull dereferenceable(1) %6, i32 noundef 46) #17
  %59 = icmp eq ptr %58, null
  br i1 %59, label %60, label %64

60:                                               ; preds = %55
  %61 = call i64 @strlen(ptr noundef nonnull dereferenceable(1) %6) #17
  %62 = sub i64 39, %61
  %63 = call ptr @strncat(ptr noundef nonnull dereferenceable(1) %6, ptr noundef nonnull @.str.14, i64 noundef %62) #17
  br label %64

64:                                               ; preds = %60, %55
  %65 = shl i32 %52, 24
  %66 = ashr exact i32 %65, 24
  %67 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef 64, ptr noundef nonnull @.str.15, ptr noundef nonnull %6, i32 noundef %66) #17
  %68 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %5, double noundef %56)
  br label %76

69:                                               ; preds = %51
  switch i32 %52, label %73 [
    i32 120, label %70
    i32 117, label %70
    i32 111, label %70
    i32 88, label %70
  ]

70:                                               ; preds = %69, %69, %69, %69
  %71 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef 64, ptr noundef nonnull @.str.16, ptr noundef %1, i32 noundef %52) #17
  %72 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef %2)
  br label %76

73:                                               ; preds = %69
  %74 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef 64, ptr noundef nonnull @.str.17, ptr noundef %1) #17
  %75 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef %2)
  br label %76

76:                                               ; preds = %7, %73, %70, %64, %43, %39
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
  br i1 %5, label %40, label %6

6:                                                ; preds = %1
  %7 = trunc nuw i32 %4 to i8
  switch i8 %7, label %8 [
    i8 37, label %40
    i8 0, label %40
  ]

8:                                                ; preds = %6
  %9 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %10 = icmp slt i64 %0, %9
  br i1 %10, label %21, label %11

11:                                               ; preds = %8
  %12 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
  %13 = load i64, ptr %12, align 1
  %14 = icmp eq i64 %13, -1
  br i1 %14, label %15, label %19

15:                                               ; preds = %11
  %16 = add nsw i64 %0, 24
  %17 = tail call ptr @chali_host_address(i64 noundef %16, i64 noundef 8) #17
  %18 = load i64, ptr %17, align 1
  br label %21

19:                                               ; preds = %11
  %20 = add nsw i64 %0, 8
  br label %21

21:                                               ; preds = %8, %15, %19
  %22 = phi i64 [ %20, %19 ], [ %18, %15 ], [ %0, %8 ]
  %23 = tail call ptr @chali_host_address(i64 noundef %22, i64 noundef 0) #17
  br i1 %10, label %24, label %27

24:                                               ; preds = %21
  %25 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #17
  %26 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %25) #17
  br label %35

27:                                               ; preds = %21
  %28 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
  %29 = load i64, ptr %28, align 1
  %30 = icmp eq i64 %29, -1
  br i1 %30, label %31, label %35

31:                                               ; preds = %27
  %32 = add nsw i64 %0, 8
  %33 = tail call ptr @chali_host_address(i64 noundef %32, i64 noundef 8) #17
  %34 = load i64, ptr %33, align 1
  br label %35

35:                                               ; preds = %24, %27, %31
  %36 = phi i64 [ %26, %24 ], [ %29, %27 ], [ %34, %31 ]
  %37 = trunc i64 %36 to i32
  %38 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i64 noundef 48, ptr noundef nonnull @.str.11, ptr noundef nonnull %2) #17
  %39 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %3, i32 noundef %37, ptr noundef %23)
  br label %40

40:                                               ; preds = %1, %6, %6, %35
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #17
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #17
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_finish() local_unnamed_addr #0 {
  %1 = load i64, ptr @chali_fmt_at, align 8, !tbaa !13
  %2 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %3 = icmp slt i64 %1, %2
  br i1 %3, label %14, label %4

4:                                                ; preds = %0
  %5 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 8) #17
  %6 = load i64, ptr %5, align 1
  %7 = icmp eq i64 %6, -1
  br i1 %7, label %8, label %12

8:                                                ; preds = %4
  %9 = add nsw i64 %1, 24
  %10 = tail call ptr @chali_host_address(i64 noundef %9, i64 noundef 8) #17
  %11 = load i64, ptr %10, align 1
  br label %14

12:                                               ; preds = %4
  %13 = add nsw i64 %1, 8
  br label %14

14:                                               ; preds = %0, %8, %12
  %15 = phi i64 [ %13, %12 ], [ %11, %8 ], [ %1, %0 ]
  %16 = tail call ptr @chali_host_address(i64 noundef %15, i64 noundef 0) #17
  %17 = load i64, ptr @chali_fmt_pos, align 8, !tbaa !13
  br label %18

18:                                               ; preds = %26, %14
  %19 = phi i64 [ %17, %14 ], [ %31, %26 ]
  %20 = getelementptr inbounds nuw i8, ptr %16, i64 %19
  %21 = load i8, ptr %20, align 1, !tbaa !12
  switch i8 %21, label %32 [
    i8 0, label %34
    i8 37, label %22
  ]

22:                                               ; preds = %18
  %23 = getelementptr i8, ptr %20, i64 1
  %24 = load i8, ptr %23, align 1, !tbaa !12
  %25 = icmp eq i8 %24, 37
  br i1 %25, label %26, label %32

26:                                               ; preds = %22, %32
  %27 = phi i32 [ %33, %32 ], [ 37, %22 ]
  %28 = phi i64 [ 1, %32 ], [ 2, %22 ]
  %29 = tail call ptr @__acrt_iob_func(i32 noundef 1) #17
  %30 = tail call i32 @fputc(i32 noundef %27, ptr noundef %29)
  %31 = add i64 %19, %28
  br label %18, !llvm.loop !22

32:                                               ; preds = %18, %22
  %33 = sext i8 %21 to i32
  br label %26

34:                                               ; preds = %18
  store i64 %19, ptr @chali_fmt_pos, align 8, !tbaa !13
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 -1, 2) i64 @chali_string_cmp(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = icmp eq i64 %0, 0
  br i1 %3, label %18, label %4

4:                                                ; preds = %2
  %5 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %6 = icmp slt i64 %0, %5
  br i1 %6, label %7, label %10

7:                                                ; preds = %4
  %8 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #17
  %9 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %8) #17
  br label %18

10:                                               ; preds = %4
  %11 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
  %12 = load i64, ptr %11, align 1
  %13 = icmp eq i64 %12, -1
  br i1 %13, label %14, label %18

14:                                               ; preds = %10
  %15 = add nsw i64 %0, 8
  %16 = tail call ptr @chali_host_address(i64 noundef %15, i64 noundef 8) #17
  %17 = load i64, ptr %16, align 1
  br label %18

18:                                               ; preds = %14, %10, %7, %2
  %19 = phi i64 [ 0, %2 ], [ %9, %7 ], [ %12, %10 ], [ %17, %14 ]
  %20 = icmp eq i64 %1, 0
  br i1 %20, label %35, label %21

21:                                               ; preds = %18
  %22 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %23 = icmp slt i64 %1, %22
  br i1 %23, label %24, label %27

24:                                               ; preds = %21
  %25 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 1) #17
  %26 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %25) #17
  br label %35

27:                                               ; preds = %21
  %28 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 8) #17
  %29 = load i64, ptr %28, align 1
  %30 = icmp eq i64 %29, -1
  br i1 %30, label %31, label %35

31:                                               ; preds = %27
  %32 = add nsw i64 %1, 8
  %33 = tail call ptr @chali_host_address(i64 noundef %32, i64 noundef 8) #17
  %34 = load i64, ptr %33, align 1
  br label %35

35:                                               ; preds = %31, %27, %24, %18
  %36 = phi i64 [ 0, %18 ], [ %26, %24 ], [ %29, %27 ], [ %34, %31 ]
  %37 = tail call i64 @llvm.umin.i64(i64 %19, i64 %36)
  %38 = icmp eq i64 %37, 0
  br i1 %38, label %70, label %39

39:                                               ; preds = %35
  %40 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %41 = icmp slt i64 %0, %40
  br i1 %41, label %52, label %42

42:                                               ; preds = %39
  %43 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
  %44 = load i64, ptr %43, align 1
  %45 = icmp eq i64 %44, -1
  br i1 %45, label %46, label %50

46:                                               ; preds = %42
  %47 = add nsw i64 %0, 24
  %48 = tail call ptr @chali_host_address(i64 noundef %47, i64 noundef 8) #17
  %49 = load i64, ptr %48, align 1
  br label %52

50:                                               ; preds = %42
  %51 = add nsw i64 %0, 8
  br label %52

52:                                               ; preds = %39, %46, %50
  %53 = phi i64 [ %51, %50 ], [ %49, %46 ], [ %0, %39 ]
  %54 = tail call ptr @chali_host_address(i64 noundef %53, i64 noundef 0) #17
  %55 = icmp slt i64 %1, %40
  br i1 %55, label %66, label %56

56:                                               ; preds = %52
  %57 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 8) #17
  %58 = load i64, ptr %57, align 1
  %59 = icmp eq i64 %58, -1
  br i1 %59, label %60, label %64

60:                                               ; preds = %56
  %61 = add nsw i64 %1, 24
  %62 = tail call ptr @chali_host_address(i64 noundef %61, i64 noundef 8) #17
  %63 = load i64, ptr %62, align 1
  br label %66

64:                                               ; preds = %56
  %65 = add nsw i64 %1, 8
  br label %66

66:                                               ; preds = %52, %60, %64
  %67 = phi i64 [ %65, %64 ], [ %63, %60 ], [ %1, %52 ]
  %68 = tail call ptr @chali_host_address(i64 noundef %67, i64 noundef 0) #17
  %69 = tail call i32 @memcmp(ptr noundef %54, ptr noundef %68, i64 noundef %37)
  br label %70

70:                                               ; preds = %35, %66
  %71 = phi i32 [ %69, %66 ], [ 0, %35 ]
  %72 = icmp eq i32 %71, 0
  br i1 %72, label %76, label %73

73:                                               ; preds = %70
  %74 = icmp sgt i32 %71, -1
  %75 = select i1 %74, i64 1, i64 -1
  br label %78

76:                                               ; preds = %70
  %77 = tail call i64 @llvm.ucmp.i64.i64(i64 %19, i64 %36)
  br label %78

78:                                               ; preds = %76, %73
  %79 = phi i64 [ %75, %73 ], [ %77, %76 ]
  ret i64 %79
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare dso_local i32 @memcmp(ptr noundef captures(none), ptr noundef captures(none), i64 noundef) local_unnamed_addr #9

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_string_sub(i64 noundef %0, i64 noundef %1, i64 noundef %2) local_unnamed_addr #0 {
  %4 = icmp eq i64 %0, 0
  br i1 %4, label %5, label %13

5:                                                ; preds = %3
  %6 = tail call i64 @chali_host_allocate(i64 noundef 49) #17
  %7 = add nsw i64 %6, 32
  %8 = tail call ptr @chali_host_address(i64 noundef %6, i64 noundef 32) #17
  store i64 -1, ptr %8, align 1
  %9 = getelementptr inbounds nuw i8, ptr %8, i64 8
  store i64 0, ptr %9, align 1
  %10 = getelementptr inbounds nuw i8, ptr %8, i64 16
  store i64 16, ptr %10, align 1
  %11 = getelementptr inbounds nuw i8, ptr %8, i64 24
  store i64 %7, ptr %11, align 1
  %12 = tail call ptr @chali_host_address(i64 noundef %7, i64 noundef 1) #17
  store i8 0, ptr %12, align 1, !tbaa !12
  br label %51

13:                                               ; preds = %3
  %14 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %15 = icmp slt i64 %0, %14
  br i1 %15, label %16, label %19

16:                                               ; preds = %13
  %17 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #17
  %18 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %17) #17
  br label %27

19:                                               ; preds = %13
  %20 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
  %21 = load i64, ptr %20, align 1
  %22 = icmp eq i64 %21, -1
  br i1 %22, label %23, label %27

23:                                               ; preds = %19
  %24 = add nsw i64 %0, 8
  %25 = tail call ptr @chali_host_address(i64 noundef %24, i64 noundef 8) #17
  %26 = load i64, ptr %25, align 1
  br label %27

27:                                               ; preds = %16, %19, %23
  %28 = phi i64 [ %18, %16 ], [ %21, %19 ], [ %26, %23 ]
  %29 = tail call i64 @llvm.smax.i64(i64 %1, i64 0)
  %30 = tail call i64 @llvm.smax.i64(i64 %2, i64 0)
  %31 = tail call i64 @llvm.smin.i64(i64 %29, i64 %28)
  %32 = sub nsw i64 %28, %31
  %33 = tail call i64 @llvm.smin.i64(i64 %30, i64 %32)
  %34 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %35 = icmp slt i64 %0, %34
  br i1 %35, label %46, label %36

36:                                               ; preds = %27
  %37 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
  %38 = load i64, ptr %37, align 1
  %39 = icmp eq i64 %38, -1
  br i1 %39, label %40, label %44

40:                                               ; preds = %36
  %41 = add nsw i64 %0, 24
  %42 = tail call ptr @chali_host_address(i64 noundef %41, i64 noundef 8) #17
  %43 = load i64, ptr %42, align 1
  br label %46

44:                                               ; preds = %36
  %45 = add nsw i64 %0, 8
  br label %46

46:                                               ; preds = %27, %40, %44
  %47 = phi i64 [ %45, %44 ], [ %43, %40 ], [ %0, %27 ]
  %48 = tail call ptr @chali_host_address(i64 noundef %47, i64 noundef 0) #17
  %49 = getelementptr inbounds i8, ptr %48, i64 %31
  %50 = tail call i64 @chali_string_new(ptr noundef %49, i64 noundef %33)
  br label %51

51:                                               ; preds = %46, %5
  %52 = phi i64 [ %6, %5 ], [ %50, %46 ]
  ret i64 %52
}

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_string_pos(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = icmp eq i64 %0, 0
  %4 = icmp eq i64 %1, 0
  %5 = or i1 %3, %4
  br i1 %5, label %77, label %6

6:                                                ; preds = %2
  %7 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %8 = icmp slt i64 %0, %7
  br i1 %8, label %9, label %12

9:                                                ; preds = %6
  %10 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #17
  %11 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %10) #17
  br label %20

12:                                               ; preds = %6
  %13 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
  %14 = load i64, ptr %13, align 1
  %15 = icmp eq i64 %14, -1
  br i1 %15, label %16, label %20

16:                                               ; preds = %12
  %17 = add nsw i64 %0, 8
  %18 = tail call ptr @chali_host_address(i64 noundef %17, i64 noundef 8) #17
  %19 = load i64, ptr %18, align 1
  br label %20

20:                                               ; preds = %16, %12, %9
  %21 = phi i64 [ %11, %9 ], [ %14, %12 ], [ %19, %16 ]
  %22 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %23 = icmp slt i64 %1, %22
  br i1 %23, label %24, label %27

24:                                               ; preds = %20
  %25 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 1) #17
  %26 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %25) #17
  br label %35

27:                                               ; preds = %20
  %28 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 8) #17
  %29 = load i64, ptr %28, align 1
  %30 = icmp eq i64 %29, -1
  br i1 %30, label %31, label %35

31:                                               ; preds = %27
  %32 = add nsw i64 %1, 8
  %33 = tail call ptr @chali_host_address(i64 noundef %32, i64 noundef 8) #17
  %34 = load i64, ptr %33, align 1
  br label %35

35:                                               ; preds = %24, %27, %31
  %36 = phi i64 [ %26, %24 ], [ %29, %27 ], [ %34, %31 ]
  %37 = icmp eq i64 %36, 0
  br i1 %37, label %77, label %38

38:                                               ; preds = %35
  %39 = icmp sgt i64 %36, %21
  br i1 %39, label %77, label %40

40:                                               ; preds = %38
  %41 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %42 = icmp slt i64 %0, %41
  %43 = add nsw i64 %0, 8
  %44 = add nsw i64 %0, 24
  %45 = icmp slt i64 %1, %41
  %46 = add nsw i64 %1, 8
  %47 = add nsw i64 %1, 24
  br label %48

48:                                               ; preds = %40, %73
  %49 = phi i64 [ 0, %40 ], [ %74, %73 ]
  br i1 %42, label %57, label %50

50:                                               ; preds = %48
  %51 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
  %52 = load i64, ptr %51, align 1
  %53 = icmp eq i64 %52, -1
  br i1 %53, label %54, label %57

54:                                               ; preds = %50
  %55 = tail call ptr @chali_host_address(i64 noundef %44, i64 noundef 8) #17
  %56 = load i64, ptr %55, align 1
  br label %57

57:                                               ; preds = %50, %48, %54
  %58 = phi i64 [ %0, %48 ], [ %56, %54 ], [ %43, %50 ]
  %59 = tail call ptr @chali_host_address(i64 noundef %58, i64 noundef 0) #17
  %60 = getelementptr inbounds nuw i8, ptr %59, i64 %49
  br i1 %45, label %68, label %61

61:                                               ; preds = %57
  %62 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 8) #17
  %63 = load i64, ptr %62, align 1
  %64 = icmp eq i64 %63, -1
  br i1 %64, label %65, label %68

65:                                               ; preds = %61
  %66 = tail call ptr @chali_host_address(i64 noundef %47, i64 noundef 8) #17
  %67 = load i64, ptr %66, align 1
  br label %68

68:                                               ; preds = %61, %57, %65
  %69 = phi i64 [ %1, %57 ], [ %67, %65 ], [ %46, %61 ]
  %70 = tail call ptr @chali_host_address(i64 noundef %69, i64 noundef 0) #17
  %71 = tail call i32 @memcmp(ptr noundef %60, ptr noundef %70, i64 noundef %36)
  %72 = icmp eq i32 %71, 0
  br i1 %72, label %77, label %73

73:                                               ; preds = %68
  %74 = add nuw nsw i64 %49, 1
  %75 = add nsw i64 %74, %36
  %76 = icmp sgt i64 %75, %21
  br i1 %76, label %77, label %48, !llvm.loop !23

77:                                               ; preds = %68, %73, %38, %35, %2
  %78 = phi i64 [ -1, %2 ], [ 0, %35 ], [ -1, %38 ], [ -1, %73 ], [ %49, %68 ]
  ret i64 %78
}

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_read_line() local_unnamed_addr #0 {
  %1 = tail call dereferenceable_or_null(256) ptr @malloc(i64 noundef 256) #19
  %2 = icmp eq ptr %1, null
  br i1 %2, label %3, label %4

3:                                                ; preds = %0
  tail call void @chali_panic(ptr noundef nonnull @.str.19)
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
  tail call void @chali_panic(ptr noundef nonnull @.str.20)
  unreachable

11:                                               ; preds = %4
  %12 = icmp eq i64 %6, %7
  br i1 %12, label %13, label %21

13:                                               ; preds = %11
  %14 = icmp slt i64 %7, 0
  br i1 %14, label %15, label %16

15:                                               ; preds = %13
  tail call void @chali_panic(ptr noundef nonnull @.str.21)
  unreachable

16:                                               ; preds = %13
  %17 = shl nuw i64 %7, 1
  %18 = tail call ptr @realloc(ptr noundef %5, i64 noundef %17) #20
  %19 = icmp eq ptr %18, null
  br i1 %19, label %20, label %21

20:                                               ; preds = %16
  tail call void @free(ptr noundef %5)
  tail call void @chali_panic(ptr noundef nonnull @.str.19)
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
define dso_local i64 @chali_string_fixed(double noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = alloca [256 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #17
  %4 = icmp slt i64 %1, 0
  %5 = trunc i64 %1 to i32
  %6 = select i1 %4, i32 3, i32 %5
  %7 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i64 noundef 256, ptr noundef nonnull @.str.22, i32 noundef %6, double noundef %0) #17
  %8 = icmp slt i32 %7, 0
  br i1 %8, label %9, label %10

9:                                                ; preds = %2
  tail call void @chali_panic(ptr noundef nonnull @.str.23)
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
  %11 = call i32 (ptr, ptr, ...) @sscanf(ptr noundef nonnull %2, ptr noundef nonnull @.str.24, ptr noundef nonnull %3)
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
  %17 = call i32 (ptr, ptr, ...) @sscanf(ptr noundef nonnull %2, ptr noundef nonnull @.str.25, ptr noundef nonnull %4)
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
define dso_local i64 @chali_string_of_int(i64 noundef %0) local_unnamed_addr #0 {
  %2 = alloca [32 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #17
  %3 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %2, i64 noundef 32, ptr noundef nonnull @.str.25, i64 noundef %0) #17
  %4 = call i64 @strlen(ptr noundef nonnull dereferenceable(1) %2) #17
  %5 = call i64 @chali_string_new(ptr noundef nonnull %2, i64 noundef %4)
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #17
  ret i64 %5
}

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_string_of_double(double noundef %0) local_unnamed_addr #0 {
  %2 = alloca [64 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #17
  %3 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %2, i64 noundef 64, ptr noundef nonnull @.str.26, double noundef %0) #17
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
  %8 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i64 noundef 128, ptr noundef nonnull @.str.7, ptr noundef nonnull @.str.27, i64 noundef %0, i64 noundef %1) #17
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
  %5 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %2, i64 noundef 128, ptr noundef nonnull @.str.8, ptr noundef nonnull @.str.28) #17
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
  tail call void @chali_panic(ptr noundef nonnull @.str.29)
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
  tail call void @chali_panic(ptr noundef nonnull @.str.30)
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
  tail call void @chali_panic(ptr noundef nonnull @.str.30)
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
  br i1 %4, label %15, label %5

5:                                                ; preds = %2
  %6 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
  %7 = load i64, ptr %6, align 1
  %8 = icmp eq i64 %7, -1
  br i1 %8, label %9, label %13

9:                                                ; preds = %5
  %10 = add nsw i64 %0, 24
  %11 = tail call ptr @chali_host_address(i64 noundef %10, i64 noundef 8) #17
  %12 = load i64, ptr %11, align 1
  br label %15

13:                                               ; preds = %5
  %14 = add nsw i64 %0, 8
  br label %15

15:                                               ; preds = %2, %9, %13
  %16 = phi i64 [ %14, %13 ], [ %12, %9 ], [ %0, %2 ]
  %17 = tail call ptr @chali_host_address(i64 noundef %16, i64 noundef 0) #17
  %18 = getelementptr inbounds i8, ptr %17, i64 %1
  %19 = load i8, ptr %18, align 1, !tbaa !12
  %20 = zext i8 %19 to i64
  ret i64 %20
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
  br i1 %3, label %18, label %4

4:                                                ; preds = %2
  %5 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %6 = icmp slt i64 %0, %5
  br i1 %6, label %7, label %10

7:                                                ; preds = %4
  %8 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #17
  %9 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %8) #17
  br label %18

10:                                               ; preds = %4
  %11 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
  %12 = load i64, ptr %11, align 1
  %13 = icmp eq i64 %12, -1
  br i1 %13, label %14, label %18

14:                                               ; preds = %10
  %15 = add nsw i64 %0, 8
  %16 = tail call ptr @chali_host_address(i64 noundef %15, i64 noundef 8) #17
  %17 = load i64, ptr %16, align 1
  br label %18

18:                                               ; preds = %2, %7, %10, %14
  %19 = phi i64 [ %9, %7 ], [ 0, %2 ], [ %17, %14 ], [ %12, %10 ]
  %20 = icmp eq i64 %1, 0
  br i1 %20, label %35, label %21

21:                                               ; preds = %18
  %22 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %23 = icmp slt i64 %1, %22
  br i1 %23, label %24, label %27

24:                                               ; preds = %21
  %25 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 1) #17
  %26 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %25) #17
  br label %35

27:                                               ; preds = %21
  %28 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 8) #17
  %29 = load i64, ptr %28, align 1
  %30 = icmp eq i64 %29, -1
  br i1 %30, label %31, label %35

31:                                               ; preds = %27
  %32 = add nsw i64 %1, 8
  %33 = tail call ptr @chali_host_address(i64 noundef %32, i64 noundef 8) #17
  %34 = load i64, ptr %33, align 1
  br label %35

35:                                               ; preds = %18, %24, %27, %31
  %36 = phi i64 [ %26, %24 ], [ 0, %18 ], [ %34, %31 ], [ %29, %27 ]
  %37 = add i64 %36, %19
  %38 = add i64 %37, 8
  %39 = tail call i64 @chali_host_allocate(i64 noundef %38) #17
  %40 = tail call ptr @chali_host_address(i64 noundef %39, i64 noundef 8) #17
  store i64 %37, ptr %40, align 1
  %41 = add nsw i64 %39, 8
  %42 = tail call ptr @chali_host_address(i64 noundef %41, i64 noundef %19) #17
  %43 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %44 = icmp slt i64 %0, %43
  br i1 %44, label %55, label %45

45:                                               ; preds = %35
  %46 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #17
  %47 = load i64, ptr %46, align 1
  %48 = icmp eq i64 %47, -1
  br i1 %48, label %49, label %53

49:                                               ; preds = %45
  %50 = add nsw i64 %0, 24
  %51 = tail call ptr @chali_host_address(i64 noundef %50, i64 noundef 8) #17
  %52 = load i64, ptr %51, align 1
  br label %55

53:                                               ; preds = %45
  %54 = add nsw i64 %0, 8
  br label %55

55:                                               ; preds = %35, %49, %53
  %56 = phi i64 [ %54, %53 ], [ %52, %49 ], [ %0, %35 ]
  %57 = tail call ptr @chali_host_address(i64 noundef %56, i64 noundef 0) #17
  tail call void @llvm.memcpy.p0.p0.i64(ptr align 1 %42, ptr align 1 %57, i64 %19, i1 false)
  %58 = add i64 %41, %19
  %59 = tail call ptr @chali_host_address(i64 noundef %58, i64 noundef %36) #17
  %60 = icmp slt i64 %1, %43
  br i1 %60, label %71, label %61

61:                                               ; preds = %55
  %62 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 8) #17
  %63 = load i64, ptr %62, align 1
  %64 = icmp eq i64 %63, -1
  br i1 %64, label %65, label %69

65:                                               ; preds = %61
  %66 = add nsw i64 %1, 24
  %67 = tail call ptr @chali_host_address(i64 noundef %66, i64 noundef 8) #17
  %68 = load i64, ptr %67, align 1
  br label %71

69:                                               ; preds = %61
  %70 = add nsw i64 %1, 8
  br label %71

71:                                               ; preds = %55, %65, %69
  %72 = phi i64 [ %70, %69 ], [ %68, %65 ], [ %1, %55 ]
  %73 = tail call ptr @chali_host_address(i64 noundef %72, i64 noundef 0) #17
  tail call void @llvm.memcpy.p0.p0.i64(ptr align 1 %59, ptr align 1 %73, i64 %36, i1 false)
  ret i64 %39
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
