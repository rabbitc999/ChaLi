; ChaLi LLVM AOT
; checks: on
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
@chali_data_layout_size = constant i64 281
; AOT data image: original=352 stored=352 rows=17/17 segments=1
@chali_data_layout = constant [282 x i8] c"\43\48\4C\59\01\11\0A\61\73\74\5F\64\61\74\61\5F\30\08\03\12\08\0A\61\73\74\5F\64\61\74\61\5F\31\20\03\11\08\0A\61\73\74\5F\64\61\74\61\5F\32\38\03\0C\08\0A\61\73\74\5F\64\61\74\61\5F\33\48\03\37\08\0A\61\73\74\5F\64\61\74\61\5F\34\80\01\03\0D\08\0A\61\73\74\5F\64\61\74\61\5F\35\90\01\03\10\08\0A\61\73\74\5F\64\61\74\61\5F\36\A0\01\03\06\08\0A\61\73\74\5F\64\61\74\61\5F\37\A8\01\03\07\08\0A\61\73\74\5F\64\61\74\61\5F\38\B0\01\03\0B\08\0A\61\73\74\5F\64\61\74\61\5F\39\C0\01\03\19\08\0B\61\73\74\5F\64\61\74\61\5F\31\30\E0\01\03\0E\08\0B\61\73\74\5F\64\61\74\61\5F\31\31\F0\01\03\07\08\0B\61\73\74\5F\64\61\74\61\5F\31\32\F8\01\03\1E\08\0B\61\73\74\5F\64\61\74\61\5F\31\33\98\02\03\16\08\0B\61\73\74\5F\64\61\74\61\5F\31\34\B0\02\03\17\08\0B\61\73\74\5F\64\61\74\61\5F\31\35\C8\02\03\08\08\0B\61\73\74\5F\64\61\74\61\5F\31\36\D0\02\03\08\08\00"
@chali_data_image = constant [352 x i8] c"\00\00\00\00\00\00\00\00\4E\61\6D\65\64\52\65\61\64\65\72\2E\65\78\74\72\61\00\00\00\00\00\00\00\4E\61\6D\65\64\52\65\61\64\65\72\2E\72\65\61\64\00\00\00\00\00\00\00\00\52\65\61\64\65\72\2E\72\65\61\64\00\00\00\00\00\E5\8F\AA\E8\AF\BB\E6\8E\A5\E6\94\B6\E8\80\85\E4\B8\8D\E8\83\BD\E8\B0\83\E7\94\A8\E4\BF\AE\E6\94\B9\E6\96\B9\E6\B3\95\EF\BC\9A\57\72\69\74\65\72\2E\77\72\69\74\65\00\00\57\72\69\74\65\72\2E\77\72\69\74\65\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\66\69\72\73\74\00\00\00\73\65\63\6F\6E\64\00\00\56\61\6C\75\65\20\6D\75\74\26\00\00\00\00\00\00\52\31\30\3A\56\61\6C\75\65\20\6D\75\74\26\30\3A\30\3A\38\3A\30\3A\30\3A\00\00\00\00\00\00\00\00\52\65\61\64\65\72\20\63\6F\6E\73\74\26\00\00\00\67\6C\6F\62\61\6C\00\00\6F\62\6A\65\63\74\20\73\63\61\6C\61\72\20\64\65\73\63\72\69\70\74\6F\72\73\20\4F\4B\0A\00\00\00\53\74\72\69\6E\67\EF\BC\9A\E7\BB\93\E6\9E\9C\E8\BF\87\E9\95\BF\00\00\00\61\73\73\65\72\74\28\78\29\20\E7\9A\84\20\78\20\E4\B8\BA\E5\81\87\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00"
@chali_data_segment_count = constant i64 1
@chali_data_segments = constant [1 x { i64, i64, i64 }] [{ i64, i64, i64 } { i64 0, i64 0, i64 352 }]
@chali_data_size = constant i64 352
@chali_heap_size = constant i64 536870912
@chali_allocator_kind = constant i32 1
@source_file0 = private constant [19 x i8] c"\3C\E5\86\85\E5\BB\BA\20\73\74\64\3A\3A\6C\61\6E\67\3E\00"
@source_file1 = private constant [38 x i8] c"\74\65\73\74\73\2F\66\69\78\74\75\72\65\73\2F\61\6F\74\5F\6F\62\6A\65\63\74\5F\73\63\61\6C\61\72\73\2E\63\68\6C\00"
define void @chali_code_anchor() uwtable section "chali_code" {
entry:
  ret void
}
@trace_anchor_name = private constant [1 x i8] zeroinitializer
@trace_name0 = private constant [30 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\5F\5F\62\6F\6F\74\53\74\72\69\6E\67\4C\65\6E\67\74\68\00"
@trace_name1 = private constant [30 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\5F\5F\62\6F\6F\74\53\74\72\69\6E\67\42\75\66\66\65\72\00"
@trace_name2 = private constant [33 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\5F\5F\62\6F\6F\74\53\74\72\69\6E\67\43\6F\70\79\42\79\74\65\73\00"
@trace_name3 = private constant [28 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\5F\5F\62\6F\6F\74\53\74\72\69\6E\67\43\6F\70\79\00"
@trace_name4 = private constant [28 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\5F\5F\62\6F\6F\74\53\74\72\69\6E\67\44\72\6F\70\00"
@trace_name5 = private constant [25 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\53\74\72\69\6E\67\2E\6C\65\6E\67\74\68\00"
@trace_name6 = private constant [23 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\61\73\73\65\72\74\24\62\6F\6F\6C\00"
@trace_name7 = private constant [11 x i8] c"\56\61\6C\75\65\2E\72\65\61\64\00"
@trace_name8 = private constant [12 x i8] c"\56\61\6C\75\65\2E\65\78\74\72\61\00"
@trace_name9 = private constant [12 x i8] c"\56\61\6C\75\65\2E\77\72\69\74\65\00"
@trace_name10 = private constant [18 x i8] c"\70\72\6F\67\72\61\6D\3A\3A\72\65\74\75\72\6E\65\64\00"
@trace_name11 = private constant [17 x i8] c"\70\72\6F\67\72\61\6D\3A\3A\63\6F\6E\73\75\6D\65\00"
@trace_name12 = private constant [5 x i8] c"\6D\61\69\6E\00"
@trace_name13 = private constant [37 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\70\6F\69\6E\74\65\72\46\72\6F\6D\41\64\64\72\65\73\73\5F\5F\75\69\6E\74\38\00"
@trace_name14 = private constant [37 x i8] c"\73\74\64\3A\3A\6C\61\6E\67\3A\3A\70\6F\69\6E\74\65\72\46\72\6F\6D\41\64\64\72\65\73\73\5F\5F\69\6E\74\36\34\00"
@trace_name15 = private constant [32 x i8] c"\5F\5F\63\68\61\6C\69\5F\69\66\61\63\65\2E\4E\61\6D\65\64\52\65\61\64\65\72\2E\65\78\74\72\61\00"
@trace_name16 = private constant [31 x i8] c"\5F\5F\63\68\61\6C\69\5F\69\66\61\63\65\2E\4E\61\6D\65\64\52\65\61\64\65\72\2E\72\65\61\64\00"
@trace_name17 = private constant [26 x i8] c"\5F\5F\63\68\61\6C\69\5F\69\66\61\63\65\2E\52\65\61\64\65\72\2E\72\65\61\64\00"
@trace_name18 = private constant [27 x i8] c"\5F\5F\63\68\61\6C\69\5F\69\66\61\63\65\2E\57\72\69\74\65\72\2E\77\72\69\74\65\00"
@trace_name19 = private constant [22 x i8] c"\5F\5F\63\68\61\6C\69\5F\61\67\67\5F\72\65\6C\5F\56\61\6C\75\65\00"
@trace_name20 = private constant [20 x i8] c"\5F\5F\63\68\61\6C\69\5F\63\6F\70\79\5F\73\74\72\75\63\74\00"
@chali_trace_count = constant i64 22
@chali_trace_functions = constant [22 x ptr] [ptr @chali_code_anchor, ptr @f_v56_std__lang____bootStringLength, ptr @f_v57_std__lang____bootStringBuffer, ptr @f_v58_std__lang____bootStringCopyBytes, ptr @f_v60_std__lang____bootStringCopy, ptr @f_v61_std__lang____bootStringDrop, ptr @f_v75_std__lang__String_length, ptr @f_v162_std__lang__assert_bool, ptr @f_v234_Value_read, ptr @f_v235_Value_extra, ptr @f_v236_Value_write, ptr @f_v238_program__returned, ptr @f_v239_program__consume, ptr @f_main, ptr @f_v274_std__lang__pointerFromAddress__uint8, ptr @f_v282_std__lang__pointerFromAddress__int64, ptr @f_v469___chali_iface_NamedReader_extra, ptr @f_v470___chali_iface_NamedReader_read, ptr @f_v471___chali_iface_Reader_read, ptr @f_v472___chali_iface_Writer_write, ptr @f_v475___chali_agg_rel_Value, ptr @f_v477___chali_copy_struct]
@chali_trace_names = constant [22 x ptr] [ptr @trace_anchor_name, ptr @trace_name0, ptr @trace_name1, ptr @trace_name2, ptr @trace_name3, ptr @trace_name4, ptr @trace_name5, ptr @trace_name6, ptr @trace_name7, ptr @trace_name8, ptr @trace_name9, ptr @trace_name10, ptr @trace_name11, ptr @trace_name12, ptr @trace_name13, ptr @trace_name14, ptr @trace_name15, ptr @trace_name16, ptr @trace_name17, ptr @trace_name18, ptr @trace_name19, ptr @trace_name20]
@native_name10 = private constant [17 x i8] c"\5F\5F\73\74\64\5F\69\66\61\63\65\5F\66\61\69\6C\00"
@native_name1 = private constant [17 x i8] c"\5F\5F\73\74\64\5F\6D\6F\76\65\5F\62\65\67\69\6E\00"
@native_name2 = private constant [15 x i8] c"\5F\5F\73\74\64\5F\6D\6F\76\65\5F\65\6E\64\00"
@native_name8 = private constant [16 x i8] c"\5F\5F\73\74\64\5F\6D\6F\76\65\5F\6C\69\76\65\00"
@native_name6 = private constant [15 x i8] c"\5F\5F\73\74\64\5F\6F\62\6A\5F\61\64\64\72\00"
@native_name7 = private constant [18 x i8] c"\5F\5F\73\74\64\5F\6F\62\6A\5F\67\65\74\5F\70\74\72\00"
@native_name9 = private constant [15 x i8] c"\5F\5F\73\74\64\5F\6F\62\6A\5F\6E\61\6D\65\00"
@native_name3 = private constant [14 x i8] c"\5F\5F\73\74\64\5F\6F\62\6A\5F\6E\65\77\00"
@native_name4 = private constant [14 x i8] c"\5F\5F\73\74\64\5F\6F\62\6A\5F\73\65\74\00"
@native_name5 = private constant [18 x i8] c"\5F\5F\73\74\64\5F\6F\62\6A\65\63\74\5F\63\6F\70\79\00"
@native_name0 = private constant [12 x i8] c"\5F\5F\73\74\64\5F\70\61\6E\69\63\00"
@chali_abi_lit0 = private constant [29 x i8] c"\6F\62\6A\65\63\74\20\73\63\61\6C\61\72\20\64\65\73\63\72\69\70\74\6F\72\73\20\4F\4B\0A"
@s_g_0_program__global = internal thread_local global i64 0
@t_g_0_program__global = internal thread_local global i8 0
define void @chali_reset_slots() {
entry:
  store i8 0, ptr @chali_return_float
  store ptr null, ptr @chali_source_function
  store ptr null, ptr @chali_source_file
  store i64 0, ptr @chali_source_line
  store i64 0, ptr @s_g_0_program__global
  store i8 0, ptr @t_g_0_program__global
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
define internal i64 @f_v56_std__lang____bootStringLength(i64 %p0) alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [2 x i64]
  %native_argument_types = alloca [2 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v480___ast_56___chali_ret_v_724 = alloca i64
  store i64 0, ptr %s_v480___ast_56___chali_ret_v_724
  %t_v480___ast_56___chali_ret_v_724 = alloca i8
  store i8 0, ptr %t_v480___ast_56___chali_ret_v_724
  %s_v481___ast_56_address_130 = alloca i64
  store i64 0, ptr %s_v481___ast_56_address_130
  %t_v481___ast_56_address_130 = alloca i8
  store i8 0, ptr %t_v481___ast_56_address_130
  store i64 %p0, ptr %s_v481___ast_56_address_130
  %r0 = load i64, ptr %s_v481___ast_56_address_130
  %r1 = load i8, ptr %t_v481___ast_56_address_130
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 152, ptr @chali_source_line
  %r2 = call i64 @chali_host_string_storage(i64 0, i64 %r0)
  store i64 %r2, ptr %s_v480___ast_56___chali_ret_v_724
  store i8 0, ptr %t_v480___ast_56___chali_ret_v_724
  %r3 = load i64, ptr %s_v480___ast_56___chali_ret_v_724
  %r4 = load i8, ptr %t_v480___ast_56___chali_ret_v_724
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
  %s_v482___ast_57___chali_move_source_726 = alloca i64
  store i64 0, ptr %s_v482___ast_57___chali_move_source_726
  %t_v482___ast_57___chali_move_source_726 = alloca i8
  store i8 0, ptr %t_v482___ast_57___chali_move_source_726
  %s_v483___ast_57___chali_ret_v_727 = alloca i64
  store i64 0, ptr %s_v483___ast_57___chali_ret_v_727
  %t_v483___ast_57___chali_ret_v_727 = alloca i8
  store i8 0, ptr %t_v483___ast_57___chali_ret_v_727
  %s_v484___ast_57___chali_tmp_agg_725 = alloca i64
  store i64 0, ptr %s_v484___ast_57___chali_tmp_agg_725
  %t_v484___ast_57___chali_tmp_agg_725 = alloca i8
  store i8 0, ptr %t_v484___ast_57___chali_tmp_agg_725
  %s_v485___ast_57_header = alloca i64
  store i64 0, ptr %s_v485___ast_57_header
  %t_v485___ast_57_header = alloca i8
  store i8 0, ptr %t_v485___ast_57_header
  %s_v486___ast_57_length_131 = alloca i64
  store i64 0, ptr %s_v486___ast_57_length_131
  %t_v486___ast_57_length_131 = alloca i8
  store i8 0, ptr %t_v486___ast_57_length_131
  %s_v487___ast_57_result = alloca i64
  store i64 0, ptr %s_v487___ast_57_result
  %t_v487___ast_57_result = alloca i8
  store i8 0, ptr %t_v487___ast_57_result
  store i64 %p0, ptr %s_v486___ast_57_length_131
  %r0 = load i64, ptr %s_v486___ast_57_length_131
  %r1 = load i8, ptr %t_v486___ast_57_length_131
  store ptr @f_v57_std__lang____bootStringBuffer, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 156, ptr @chali_source_line
  %r2 = icmp sge i64 %r0, 0
  %r3 = zext i1 %r2 to i64
  %r4 = icmp ne i64 %r3, 0
  br i1 %r4, label %ast_block_1, label %ast_block_0
ast_block_0:
  br label %ast_block_2
ast_block_1:
  %r5 = load i64, ptr %s_v486___ast_57_length_131
  %r6 = load i8, ptr %t_v486___ast_57_length_131
  store ptr @f_v57_std__lang____bootStringBuffer, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 156, ptr @chali_source_line
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
  store i64 156, ptr @chali_source_line
  %r16 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 280, ptr %r16
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
  store i64 157, ptr @chali_source_line
  %r20 = load i64, ptr %s_v486___ast_57_length_131
  %r21 = load i8, ptr %t_v486___ast_57_length_131
  store ptr @f_v57_std__lang____bootStringBuffer, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 157, ptr @chali_source_line
  %r22 = add i64 %r20, 8
  %r23 = call i64 @chali_alloc(i64 %r22)
  store i64 %r23, ptr %s_v487___ast_57_result
  store i8 0, ptr %t_v487___ast_57_result
  %r24 = call i64 @chali_alloc(i64 8)
  store i64 %r24, ptr %s_v484___ast_57___chali_tmp_agg_725
  store i8 0, ptr %t_v484___ast_57___chali_tmp_agg_725
  %r25 = load i64, ptr %s_v484___ast_57___chali_tmp_agg_725
  %r26 = load i8, ptr %t_v484___ast_57___chali_tmp_agg_725
  call void @chali_host_memory_prepare(i64 8, i64 %r25, i64 0, i64 8, ptr %raw_memory_output, ptr %raw_memory_input)
  %r27 = load ptr, ptr %raw_memory_output
  %r28 = trunc i64 0 to i8
  call void @llvm.memset.p0.i64(ptr %r27, i8 %r28, i64 8, i1 false)
  %r29 = load i64, ptr %s_v484___ast_57___chali_tmp_agg_725
  %r30 = load i8, ptr %t_v484___ast_57___chali_tmp_agg_725
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
  %r37 = load i64, ptr %s_v487___ast_57_result
  %r38 = load i8, ptr %t_v487___ast_57_result
  store ptr @f_v57_std__lang____bootStringBuffer, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 158, ptr @chali_source_line
  %r39 = call i64 @f_v282_std__lang__pointerFromAddress__int64(i64 %r37)
  %r40 = load i8, ptr @chali_return_float
  store i64 %r39, ptr %s_v482___ast_57___chali_move_source_726
  store i8 %r40, ptr %t_v482___ast_57___chali_move_source_726
  %r41 = load i64, ptr %s_v484___ast_57___chali_tmp_agg_725
  %r42 = load i8, ptr %t_v484___ast_57___chali_tmp_agg_725
  %r43 = load i64, ptr %s_v482___ast_57___chali_move_source_726
  %r44 = load i8, ptr %t_v482___ast_57___chali_move_source_726
  %r45 = call i64 @chali_host_copy_words(i64 %r41, i64 %r43, i64 1)
  %r46 = load i64, ptr %s_v484___ast_57___chali_tmp_agg_725
  %r47 = load i8, ptr %t_v484___ast_57___chali_tmp_agg_725
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
  %r54 = load i64, ptr %s_v484___ast_57___chali_tmp_agg_725
  %r55 = load i8, ptr %t_v484___ast_57___chali_tmp_agg_725
  %r56 = load i64, ptr %s_v482___ast_57___chali_move_source_726
  %r57 = load i8, ptr %t_v482___ast_57___chali_move_source_726
  %r58 = icmp ne i64 %r54, %r56
  %r59 = zext i1 %r58 to i64
  %r60 = icmp ne i64 %r59, 0
  br i1 %r60, label %ast_block_7, label %ast_block_8
ast_block_7:
  %r61 = load i64, ptr %s_v482___ast_57___chali_move_source_726
  %r62 = load i8, ptr %t_v482___ast_57___chali_move_source_726
  %r63 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r61, ptr %r63
  %r64 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r62, ptr %r64
  %r65 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 8, ptr %r65
  %r66 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r66
  %r67 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 46, ptr %r67
  %r68 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r68
  %r69 = call i64 @chali_host_call(ptr @native_name2, i64 3, ptr %indirect_arguments, ptr %native_argument_types)
  %r70 = call i8 @chali_host_result_float()
  br label %ast_block_9
ast_block_8:
  br label %ast_block_9
ast_block_9:
  %r71 = load i64, ptr %s_v484___ast_57___chali_tmp_agg_725
  %r72 = load i8, ptr %t_v484___ast_57___chali_tmp_agg_725
  %r73 = call ptr @chali_primitive_address(i64 %r71, i64 8)
  %r74 = load i64, ptr %r73, align 1
  %r75 = call i64 @chali_require_reference(i64 %r74)
  store i64 %r75, ptr %s_v485___ast_57_header
  store i8 0, ptr %t_v485___ast_57_header
  %r76 = load i64, ptr %s_v486___ast_57_length_131
  %r77 = load i8, ptr %t_v486___ast_57_length_131
  %r78 = load i64, ptr %s_v485___ast_57_header
  %r79 = load i8, ptr %t_v485___ast_57_header
  %r80 = call i64 @chali_require_reference(i64 %r78)
  %r81 = call ptr @chali_primitive_address(i64 %r80, i64 8)
  store i64 %r76, ptr %r81, align 1
  %r82 = load i64, ptr %s_v487___ast_57_result
  %r83 = load i8, ptr %t_v487___ast_57_result
  store i64 %r82, ptr %s_v483___ast_57___chali_ret_v_727
  store i8 %r83, ptr %t_v483___ast_57___chali_ret_v_727
  %r84 = load i64, ptr %s_v484___ast_57___chali_tmp_agg_725
  %r85 = load i8, ptr %t_v484___ast_57___chali_tmp_agg_725
  %r86 = icmp ne i64 %r84, 0
  %r87 = zext i1 %r86 to i64
  %r88 = icmp ne i64 %r87, 0
  br i1 %r88, label %ast_block_10, label %ast_block_11
ast_block_10:
  %r89 = load i64, ptr %s_v484___ast_57___chali_tmp_agg_725
  %r90 = load i8, ptr %t_v484___ast_57___chali_tmp_agg_725
  call void @chali_owned_drop(i64 %r89)
  br label %ast_block_12
ast_block_11:
  br label %ast_block_12
ast_block_12:
  store i64 0, ptr %s_v484___ast_57___chali_tmp_agg_725
  store i8 0, ptr %t_v484___ast_57___chali_tmp_agg_725
  %r91 = load i64, ptr %s_v483___ast_57___chali_ret_v_727
  %r92 = load i8, ptr %t_v483___ast_57___chali_ret_v_727
  store i8 %r92, ptr @chali_return_float
  ret i64 %r91
ast_block_13:
  %r93 = load i64, ptr %s_v484___ast_57___chali_tmp_agg_725
  %r94 = load i8, ptr %t_v484___ast_57___chali_tmp_agg_725
  %r95 = icmp ne i64 %r93, 0
  %r96 = zext i1 %r95 to i64
  %r97 = icmp ne i64 %r96, 0
  br i1 %r97, label %ast_block_14, label %ast_block_15
ast_block_14:
  %r98 = load i64, ptr %s_v484___ast_57___chali_tmp_agg_725
  %r99 = load i8, ptr %t_v484___ast_57___chali_tmp_agg_725
  call void @chali_owned_drop(i64 %r98)
  br label %ast_block_16
ast_block_15:
  br label %ast_block_16
ast_block_16:
  store i64 0, ptr %s_v484___ast_57___chali_tmp_agg_725
  store i8 0, ptr %t_v484___ast_57___chali_tmp_agg_725
  store i8 0, ptr @chali_return_float
  ret i64 0
ast_label_v488___chali_cleanup_std__lang____bootStringBuffer:
  %r100 = load i64, ptr %s_v484___ast_57___chali_tmp_agg_725
  %r101 = load i8, ptr %t_v484___ast_57___chali_tmp_agg_725
  %r102 = icmp ne i64 %r100, 0
  %r103 = zext i1 %r102 to i64
  %r104 = icmp ne i64 %r103, 0
  br i1 %r104, label %ast_block_17, label %ast_block_18
ast_block_17:
  %r105 = load i64, ptr %s_v484___ast_57___chali_tmp_agg_725
  %r106 = load i8, ptr %t_v484___ast_57___chali_tmp_agg_725
  call void @chali_owned_drop(i64 %r105)
  br label %ast_block_19
ast_block_18:
  br label %ast_block_19
ast_block_19:
  store i64 0, ptr %s_v484___ast_57___chali_tmp_agg_725
  store i8 0, ptr %t_v484___ast_57___chali_tmp_agg_725
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
  %s_v489___ast_58___chali_memory_argument = alloca i64
  store i64 0, ptr %s_v489___ast_58___chali_memory_argument
  %t_v489___ast_58___chali_memory_argument = alloca i8
  store i8 0, ptr %t_v489___ast_58___chali_memory_argument
  %s_v490___ast_58___chali_memory_argument_732 = alloca i64
  store i64 0, ptr %s_v490___ast_58___chali_memory_argument_732
  %t_v490___ast_58___chali_memory_argument_732 = alloca i8
  store i8 0, ptr %t_v490___ast_58___chali_memory_argument_732
  %s_v491___ast_58___chali_memory_argument_733 = alloca i64
  store i64 0, ptr %s_v491___ast_58___chali_memory_argument_733
  %t_v491___ast_58___chali_memory_argument_733 = alloca i8
  store i8 0, ptr %t_v491___ast_58___chali_memory_argument_733
  %s_v492___ast_58___chali_move_source_729 = alloca i64
  store i64 0, ptr %s_v492___ast_58___chali_move_source_729
  %t_v492___ast_58___chali_move_source_729 = alloca i8
  store i8 0, ptr %t_v492___ast_58___chali_move_source_729
  %s_v493___ast_58___chali_move_source_731 = alloca i64
  store i64 0, ptr %s_v493___ast_58___chali_move_source_731
  %t_v493___ast_58___chali_move_source_731 = alloca i8
  store i8 0, ptr %t_v493___ast_58___chali_move_source_731
  %s_v494___ast_58___chali_tmp_agg_728 = alloca i64
  store i64 0, ptr %s_v494___ast_58___chali_tmp_agg_728
  %t_v494___ast_58___chali_tmp_agg_728 = alloca i8
  store i8 0, ptr %t_v494___ast_58___chali_tmp_agg_728
  %s_v495___ast_58___chali_tmp_agg_730 = alloca i64
  store i64 0, ptr %s_v495___ast_58___chali_tmp_agg_730
  %t_v495___ast_58___chali_tmp_agg_730 = alloca i8
  store i8 0, ptr %t_v495___ast_58___chali_tmp_agg_730
  %s_v496___ast_58_destination = alloca i64
  store i64 0, ptr %s_v496___ast_58_destination
  %t_v496___ast_58_destination = alloca i8
  store i8 0, ptr %t_v496___ast_58_destination
  %s_v497___ast_58_length_132 = alloca i64
  store i64 0, ptr %s_v497___ast_58_length_132
  %t_v497___ast_58_length_132 = alloca i8
  store i8 0, ptr %t_v497___ast_58_length_132
  %s_v498___ast_58_source = alloca i64
  store i64 0, ptr %s_v498___ast_58_source
  %t_v498___ast_58_source = alloca i8
  store i8 0, ptr %t_v498___ast_58_source
  store i64 %p0, ptr %s_v496___ast_58_destination
  store i64 %p1, ptr %s_v498___ast_58_source
  store i64 %p2, ptr %s_v497___ast_58_length_132
  %r0 = load i64, ptr %s_v497___ast_58_length_132
  %r1 = load i8, ptr %t_v497___ast_58_length_132
  store ptr @f_v58_std__lang____bootStringCopyBytes, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 163, ptr @chali_source_line
  %r2 = icmp sgt i64 %r0, 0
  %r3 = zext i1 %r2 to i64
  %r4 = icmp ne i64 %r3, 0
  br i1 %r4, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r5 = call i64 @chali_alloc(i64 8)
  store i64 %r5, ptr %s_v494___ast_58___chali_tmp_agg_728
  store i8 0, ptr %t_v494___ast_58___chali_tmp_agg_728
  %r6 = load i64, ptr %s_v494___ast_58___chali_tmp_agg_728
  %r7 = load i8, ptr %t_v494___ast_58___chali_tmp_agg_728
  call void @chali_host_memory_prepare(i64 8, i64 %r6, i64 0, i64 8, ptr %raw_memory_output, ptr %raw_memory_input)
  %r8 = load ptr, ptr %raw_memory_output
  %r9 = trunc i64 0 to i8
  call void @llvm.memset.p0.i64(ptr %r8, i8 %r9, i64 8, i1 false)
  %r10 = load i64, ptr %s_v494___ast_58___chali_tmp_agg_728
  %r11 = load i8, ptr %t_v494___ast_58___chali_tmp_agg_728
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
  store i64 %r18, ptr %s_v495___ast_58___chali_tmp_agg_730
  store i8 0, ptr %t_v495___ast_58___chali_tmp_agg_730
  %r19 = load i64, ptr %s_v495___ast_58___chali_tmp_agg_730
  %r20 = load i8, ptr %t_v495___ast_58___chali_tmp_agg_730
  call void @chali_host_memory_prepare(i64 8, i64 %r19, i64 0, i64 8, ptr %raw_memory_output, ptr %raw_memory_input)
  %r21 = load ptr, ptr %raw_memory_output
  %r22 = trunc i64 0 to i8
  call void @llvm.memset.p0.i64(ptr %r21, i8 %r22, i64 8, i1 false)
  %r23 = load i64, ptr %s_v495___ast_58___chali_tmp_agg_730
  %r24 = load i8, ptr %t_v495___ast_58___chali_tmp_agg_730
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
  %r31 = load i64, ptr %s_v496___ast_58_destination
  %r32 = load i8, ptr %t_v496___ast_58_destination
  store ptr @f_v58_std__lang____bootStringCopyBytes, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 163, ptr @chali_source_line
  %r33 = call i64 @f_v274_std__lang__pointerFromAddress__uint8(i64 %r31)
  %r34 = load i8, ptr @chali_return_float
  store i64 %r33, ptr %s_v492___ast_58___chali_move_source_729
  store i8 %r34, ptr %t_v492___ast_58___chali_move_source_729
  %r35 = load i64, ptr %s_v494___ast_58___chali_tmp_agg_728
  %r36 = load i8, ptr %t_v494___ast_58___chali_tmp_agg_728
  %r37 = load i64, ptr %s_v492___ast_58___chali_move_source_729
  %r38 = load i8, ptr %t_v492___ast_58___chali_move_source_729
  %r39 = call i64 @chali_host_copy_words(i64 %r35, i64 %r37, i64 1)
  %r40 = load i64, ptr %s_v494___ast_58___chali_tmp_agg_728
  %r41 = load i8, ptr %t_v494___ast_58___chali_tmp_agg_728
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
  %r48 = load i64, ptr %s_v494___ast_58___chali_tmp_agg_728
  %r49 = load i8, ptr %t_v494___ast_58___chali_tmp_agg_728
  %r50 = load i64, ptr %s_v492___ast_58___chali_move_source_729
  %r51 = load i8, ptr %t_v492___ast_58___chali_move_source_729
  %r52 = icmp ne i64 %r48, %r50
  %r53 = zext i1 %r52 to i64
  %r54 = icmp ne i64 %r53, 0
  br i1 %r54, label %ast_block_3, label %ast_block_4
ast_block_3:
  %r55 = load i64, ptr %s_v492___ast_58___chali_move_source_729
  %r56 = load i8, ptr %t_v492___ast_58___chali_move_source_729
  %r57 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r55, ptr %r57
  %r58 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r56, ptr %r58
  %r59 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 8, ptr %r59
  %r60 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r60
  %r61 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 49, ptr %r61
  %r62 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r62
  %r63 = call i64 @chali_host_call(ptr @native_name2, i64 3, ptr %indirect_arguments, ptr %native_argument_types)
  %r64 = call i8 @chali_host_result_float()
  br label %ast_block_5
ast_block_4:
  br label %ast_block_5
ast_block_5:
  %r65 = load i64, ptr %s_v494___ast_58___chali_tmp_agg_728
  %r66 = load i8, ptr %t_v494___ast_58___chali_tmp_agg_728
  store i64 %r65, ptr %s_v489___ast_58___chali_memory_argument
  store i8 %r66, ptr %t_v489___ast_58___chali_memory_argument
  %r67 = load i64, ptr %s_v498___ast_58_source
  %r68 = load i8, ptr %t_v498___ast_58_source
  store ptr @f_v58_std__lang____bootStringCopyBytes, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 163, ptr @chali_source_line
  %r69 = call i64 @f_v274_std__lang__pointerFromAddress__uint8(i64 %r67)
  %r70 = load i8, ptr @chali_return_float
  store i64 %r69, ptr %s_v493___ast_58___chali_move_source_731
  store i8 %r70, ptr %t_v493___ast_58___chali_move_source_731
  %r71 = load i64, ptr %s_v495___ast_58___chali_tmp_agg_730
  %r72 = load i8, ptr %t_v495___ast_58___chali_tmp_agg_730
  %r73 = load i64, ptr %s_v493___ast_58___chali_move_source_731
  %r74 = load i8, ptr %t_v493___ast_58___chali_move_source_731
  %r75 = call i64 @chali_host_copy_words(i64 %r71, i64 %r73, i64 1)
  %r76 = load i64, ptr %s_v495___ast_58___chali_tmp_agg_730
  %r77 = load i8, ptr %t_v495___ast_58___chali_tmp_agg_730
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
  %r84 = load i64, ptr %s_v495___ast_58___chali_tmp_agg_730
  %r85 = load i8, ptr %t_v495___ast_58___chali_tmp_agg_730
  %r86 = load i64, ptr %s_v493___ast_58___chali_move_source_731
  %r87 = load i8, ptr %t_v493___ast_58___chali_move_source_731
  %r88 = icmp ne i64 %r84, %r86
  %r89 = zext i1 %r88 to i64
  %r90 = icmp ne i64 %r89, 0
  br i1 %r90, label %ast_block_6, label %ast_block_7
ast_block_6:
  %r91 = load i64, ptr %s_v493___ast_58___chali_move_source_731
  %r92 = load i8, ptr %t_v493___ast_58___chali_move_source_731
  %r93 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r91, ptr %r93
  %r94 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r92, ptr %r94
  %r95 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 8, ptr %r95
  %r96 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r96
  %r97 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 49, ptr %r97
  %r98 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r98
  %r99 = call i64 @chali_host_call(ptr @native_name2, i64 3, ptr %indirect_arguments, ptr %native_argument_types)
  %r100 = call i8 @chali_host_result_float()
  br label %ast_block_8
ast_block_7:
  br label %ast_block_8
ast_block_8:
  %r101 = load i64, ptr %s_v495___ast_58___chali_tmp_agg_730
  %r102 = load i8, ptr %t_v495___ast_58___chali_tmp_agg_730
  store i64 %r101, ptr %s_v490___ast_58___chali_memory_argument_732
  store i8 %r102, ptr %t_v490___ast_58___chali_memory_argument_732
  %r103 = load i64, ptr %s_v497___ast_58_length_132
  %r104 = load i8, ptr %t_v497___ast_58_length_132
  store i64 %r103, ptr %s_v491___ast_58___chali_memory_argument_733
  store i8 %r104, ptr %t_v491___ast_58___chali_memory_argument_733
  %r105 = load i64, ptr %s_v489___ast_58___chali_memory_argument
  %r106 = load i8, ptr %t_v489___ast_58___chali_memory_argument
  %r107 = call ptr @chali_primitive_address(i64 %r105, i64 8)
  %r108 = load i64, ptr %r107, align 1
  %r109 = load i64, ptr %s_v490___ast_58___chali_memory_argument_732
  %r110 = load i8, ptr %t_v490___ast_58___chali_memory_argument_732
  %r111 = call ptr @chali_primitive_address(i64 %r109, i64 8)
  %r112 = load i64, ptr %r111, align 1
  %r113 = load i64, ptr %s_v491___ast_58___chali_memory_argument_733
  %r114 = load i8, ptr %t_v491___ast_58___chali_memory_argument_733
  call void @chali_host_memory_prepare(i64 7, i64 %r108, i64 %r112, i64 %r113, ptr %raw_memory_output, ptr %raw_memory_input)
  %r115 = load ptr, ptr %raw_memory_output
  %r116 = load ptr, ptr %raw_memory_input
  call void @llvm.memcpy.p0.p0.i64(ptr %r115, ptr %r116, i64 %r113, i1 false)
  %r117 = load i64, ptr %s_v495___ast_58___chali_tmp_agg_730
  %r118 = load i8, ptr %t_v495___ast_58___chali_tmp_agg_730
  %r119 = icmp ne i64 %r117, 0
  %r120 = zext i1 %r119 to i64
  %r121 = icmp ne i64 %r120, 0
  br i1 %r121, label %ast_block_9, label %ast_block_10
ast_block_9:
  %r122 = load i64, ptr %s_v495___ast_58___chali_tmp_agg_730
  %r123 = load i8, ptr %t_v495___ast_58___chali_tmp_agg_730
  call void @chali_owned_drop(i64 %r122)
  br label %ast_block_11
ast_block_10:
  br label %ast_block_11
ast_block_11:
  store i64 0, ptr %s_v495___ast_58___chali_tmp_agg_730
  store i8 0, ptr %t_v495___ast_58___chali_tmp_agg_730
  %r124 = load i64, ptr %s_v494___ast_58___chali_tmp_agg_728
  %r125 = load i8, ptr %t_v494___ast_58___chali_tmp_agg_728
  %r126 = icmp ne i64 %r124, 0
  %r127 = zext i1 %r126 to i64
  %r128 = icmp ne i64 %r127, 0
  br i1 %r128, label %ast_block_12, label %ast_block_13
ast_block_12:
  %r129 = load i64, ptr %s_v494___ast_58___chali_tmp_agg_728
  %r130 = load i8, ptr %t_v494___ast_58___chali_tmp_agg_728
  call void @chali_owned_drop(i64 %r129)
  br label %ast_block_14
ast_block_13:
  br label %ast_block_14
ast_block_14:
  store i64 0, ptr %s_v494___ast_58___chali_tmp_agg_728
  store i8 0, ptr %t_v494___ast_58___chali_tmp_agg_728
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
  %s_v499___ast_60___chali_ret_v_737 = alloca i64
  store i64 0, ptr %s_v499___ast_60___chali_ret_v_737
  %t_v499___ast_60___chali_ret_v_737 = alloca i8
  store i8 0, ptr %t_v499___ast_60___chali_ret_v_737
  %s_v500___ast_60_length_735 = alloca i64
  store i64 0, ptr %s_v500___ast_60_length_735
  %t_v500___ast_60_length_735 = alloca i8
  store i8 0, ptr %t_v500___ast_60_length_735
  %s_v501___ast_60_result_736 = alloca i64
  store i64 0, ptr %s_v501___ast_60_result_736
  %t_v501___ast_60_result_736 = alloca i8
  store i8 0, ptr %t_v501___ast_60_result_736
  %s_v502___ast_60_source_133 = alloca i64
  store i64 0, ptr %s_v502___ast_60_source_133
  %t_v502___ast_60_source_133 = alloca i8
  store i8 0, ptr %t_v502___ast_60_source_133
  store i64 %p0, ptr %s_v502___ast_60_source_133
  %r0 = load i64, ptr %s_v502___ast_60_source_133
  %r1 = load i8, ptr %t_v502___ast_60_source_133
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 169, ptr @chali_source_line
  %r2 = call i64 @f_v56_std__lang____bootStringLength(i64 %r0)
  %r3 = load i8, ptr @chali_return_float
  store i64 %r2, ptr %s_v500___ast_60_length_735
  store i8 %r3, ptr %t_v500___ast_60_length_735
  %r4 = load i64, ptr %s_v500___ast_60_length_735
  %r5 = load i8, ptr %t_v500___ast_60_length_735
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 169, ptr @chali_source_line
  %r6 = call i64 @f_v57_std__lang____bootStringBuffer(i64 %r4)
  %r7 = load i8, ptr @chali_return_float
  store i64 %r6, ptr %s_v501___ast_60_result_736
  store i8 %r7, ptr %t_v501___ast_60_result_736
  %r8 = load i64, ptr %s_v501___ast_60_result_736
  %r9 = load i8, ptr %t_v501___ast_60_result_736
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 170, ptr @chali_source_line
  %r10 = add i64 %r8, 8
  %r11 = load i64, ptr %s_v502___ast_60_source_133
  %r12 = load i8, ptr %t_v502___ast_60_source_133
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 170, ptr @chali_source_line
  %r13 = call i64 @chali_host_string_storage(i64 1, i64 %r11)
  %r14 = load i64, ptr %s_v500___ast_60_length_735
  %r15 = load i8, ptr %t_v500___ast_60_length_735
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 170, ptr @chali_source_line
  %r16 = call i64 @f_v58_std__lang____bootStringCopyBytes(i64 %r10, i64 %r13, i64 %r14)
  %r17 = load i8, ptr @chali_return_float
  %r18 = load i64, ptr %s_v501___ast_60_result_736
  %r19 = load i8, ptr %t_v501___ast_60_result_736
  store i64 %r18, ptr %s_v499___ast_60___chali_ret_v_737
  store i8 %r19, ptr %t_v499___ast_60___chali_ret_v_737
  %r20 = load i64, ptr %s_v499___ast_60___chali_ret_v_737
  %r21 = load i8, ptr %t_v499___ast_60___chali_ret_v_737
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
  %s_v503___ast_61___chali_ret_v_738 = alloca i64
  store i64 0, ptr %s_v503___ast_61___chali_ret_v_738
  %t_v503___ast_61___chali_ret_v_738 = alloca i8
  store i8 0, ptr %t_v503___ast_61___chali_ret_v_738
  %s_v504___ast_61_address_134 = alloca i64
  store i64 0, ptr %s_v504___ast_61_address_134
  %t_v504___ast_61_address_134 = alloca i8
  store i8 0, ptr %t_v504___ast_61_address_134
  store i64 %p0, ptr %s_v504___ast_61_address_134
  %r0 = load i64, ptr %s_v504___ast_61_address_134
  %r1 = load i8, ptr %t_v504___ast_61_address_134
  call void @chali_owned_drop(i64 %r0)
  store i64 0, ptr %s_v503___ast_61___chali_ret_v_738
  store i8 0, ptr %t_v503___ast_61___chali_ret_v_738
  %r2 = load i64, ptr %s_v503___ast_61___chali_ret_v_738
  %r3 = load i8, ptr %t_v503___ast_61___chali_ret_v_738
  store i8 %r3, ptr @chali_return_float
  ret i64 %r2
}
define internal i64 @f_v75_std__lang__String_length(i64 %p0) alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [2 x i64]
  %native_argument_types = alloca [2 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v505___ast_75___chali_ret_v_872 = alloca i64
  store i64 0, ptr %s_v505___ast_75___chali_ret_v_872
  %t_v505___ast_75___chali_ret_v_872 = alloca i8
  store i8 0, ptr %t_v505___ast_75___chali_ret_v_872
  %s_v506___ast_75_self = alloca i64
  store i64 0, ptr %s_v506___ast_75_self
  %t_v506___ast_75_self = alloca i8
  store i8 0, ptr %t_v506___ast_75_self
  store i64 %p0, ptr %s_v506___ast_75_self
  %r0 = load i64, ptr %s_v506___ast_75_self
  %r1 = load i8, ptr %t_v506___ast_75_self
  store ptr @f_v75_std__lang__String_length, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 408, ptr @chali_source_line
  %r2 = call i64 @f_v56_std__lang____bootStringLength(i64 %r0)
  %r3 = load i8, ptr @chali_return_float
  store i64 %r2, ptr %s_v505___ast_75___chali_ret_v_872
  store i8 %r3, ptr %t_v505___ast_75___chali_ret_v_872
  %r4 = load i64, ptr %s_v505___ast_75___chali_ret_v_872
  %r5 = load i8, ptr %t_v505___ast_75___chali_ret_v_872
  store i8 %r5, ptr @chali_return_float
  ret i64 %r4
}
define internal i64 @f_v162_std__lang__assert_bool(i64 %p0) alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [2 x i64]
  %native_argument_types = alloca [2 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v507___ast_162_condition = alloca i64
  store i64 0, ptr %s_v507___ast_162_condition
  %t_v507___ast_162_condition = alloca i8
  store i8 0, ptr %t_v507___ast_162_condition
  store i64 %p0, ptr %s_v507___ast_162_condition
  %r0 = load i64, ptr %s_v507___ast_162_condition
  %r1 = load i8, ptr %t_v507___ast_162_condition
  %r2 = icmp ne i64 %r0, 0
  %r3 = xor i1 %r2, true
  %r4 = zext i1 %r3 to i64
  %r5 = icmp ne i64 %r4, 0
  br i1 %r5, label %ast_block_0, label %ast_block_1
ast_block_0:
  store ptr @f_v162_std__lang__assert_bool, ptr @chali_source_function
  store ptr @source_file0, ptr @chali_source_file
  store i64 773, ptr @chali_source_line
  %r6 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 304, ptr %r6
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
define internal i64 @f_v234_Value_read(i64 %p0) alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [1 x i64]
  %native_argument_types = alloca [1 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v508___ast_234___chali_ret_v_1309 = alloca i64
  store i64 0, ptr %s_v508___ast_234___chali_ret_v_1309
  %t_v508___ast_234___chali_ret_v_1309 = alloca i8
  store i8 0, ptr %t_v508___ast_234___chali_ret_v_1309
  %s_v509___ast_234_self = alloca i64
  store i64 0, ptr %s_v509___ast_234_self
  %t_v509___ast_234_self = alloca i8
  store i8 0, ptr %t_v509___ast_234_self
  store i64 %p0, ptr %s_v509___ast_234_self
  %r0 = load i64, ptr %s_v509___ast_234_self
  %r1 = load i8, ptr %t_v509___ast_234_self
  %r2 = mul i64 0, 8
  %r3 = add i64 %r0, %r2
  store ptr @f_v234_Value_read, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 8, ptr @chali_source_line
  %r4 = call ptr @chali_primitive_address(i64 %r3, i64 8)
  %r5 = load i64, ptr %r4, align 1
  store i64 %r5, ptr %s_v508___ast_234___chali_ret_v_1309
  store i8 0, ptr %t_v508___ast_234___chali_ret_v_1309
  %r6 = load i64, ptr %s_v508___ast_234___chali_ret_v_1309
  %r7 = load i8, ptr %t_v508___ast_234___chali_ret_v_1309
  store i8 %r7, ptr @chali_return_float
  ret i64 %r6
}
define internal i64 @f_v235_Value_extra(i64 %p0) alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [2 x i64]
  %native_argument_types = alloca [2 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v510___ast_235___chali_ret_v_1310 = alloca i64
  store i64 0, ptr %s_v510___ast_235___chali_ret_v_1310
  %t_v510___ast_235___chali_ret_v_1310 = alloca i8
  store i8 0, ptr %t_v510___ast_235___chali_ret_v_1310
  %s_v511___ast_235_self = alloca i64
  store i64 0, ptr %s_v511___ast_235_self
  %t_v511___ast_235_self = alloca i8
  store i8 0, ptr %t_v511___ast_235_self
  store i64 %p0, ptr %s_v511___ast_235_self
  %r0 = load i64, ptr %s_v511___ast_235_self
  %r1 = load i8, ptr %t_v511___ast_235_self
  %r2 = mul i64 1, 8
  %r3 = add i64 %r0, %r2
  store ptr @f_v235_Value_extra, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 9, ptr @chali_source_line
  %r4 = call ptr @chali_primitive_address(i64 %r3, i64 8)
  %r5 = load i64, ptr %r4, align 1
  store ptr @f_v235_Value_extra, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 9, ptr @chali_source_line
  %r6 = call i64 @f_v75_std__lang__String_length(i64 %r5)
  %r7 = load i8, ptr @chali_return_float
  store i64 %r6, ptr %s_v510___ast_235___chali_ret_v_1310
  store i8 %r7, ptr %t_v510___ast_235___chali_ret_v_1310
  %r8 = load i64, ptr %s_v510___ast_235___chali_ret_v_1310
  %r9 = load i8, ptr %t_v510___ast_235___chali_ret_v_1310
  store i8 %r9, ptr @chali_return_float
  ret i64 %r8
}
define internal i64 @f_v236_Value_write(i64 %p0, i64 %p1) alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [1 x i64]
  %native_argument_types = alloca [1 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v512___ast_236_next = alloca i64
  store i64 0, ptr %s_v512___ast_236_next
  %t_v512___ast_236_next = alloca i8
  store i8 0, ptr %t_v512___ast_236_next
  %s_v513___ast_236_self = alloca i64
  store i64 0, ptr %s_v513___ast_236_self
  %t_v513___ast_236_self = alloca i8
  store i8 0, ptr %t_v513___ast_236_self
  store i64 %p0, ptr %s_v513___ast_236_self
  store i64 %p1, ptr %s_v512___ast_236_next
  %r0 = load i64, ptr %s_v512___ast_236_next
  %r1 = load i8, ptr %t_v512___ast_236_next
  %r2 = load i64, ptr %s_v513___ast_236_self
  %r3 = load i8, ptr %t_v513___ast_236_self
  %r4 = mul i64 0, 8
  %r5 = add i64 %r2, %r4
  %r6 = call ptr @chali_primitive_address(i64 %r5, i64 8)
  store i64 %r0, ptr %r6, align 1
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define internal i64 @f_v238_program__returned() uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [8 x i64]
  %native_argument_types = alloca [8 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v514___ast_238___chali_box_value = alloca i64
  store i64 0, ptr %s_v514___ast_238___chali_box_value
  %t_v514___ast_238___chali_box_value = alloca i8
  store i8 0, ptr %t_v514___ast_238___chali_box_value
  %s_v515___ast_238___chali_ret_v_1313 = alloca i64
  store i64 0, ptr %s_v515___ast_238___chali_ret_v_1313
  %t_v515___ast_238___chali_ret_v_1313 = alloca i8
  store i8 0, ptr %t_v515___ast_238___chali_ret_v_1313
  %s_v516___ast_238___chali_tmp_o_1312 = alloca i64
  store i64 0, ptr %s_v516___ast_238___chali_tmp_o_1312
  %t_v516___ast_238___chali_tmp_o_1312 = alloca i8
  store i8 0, ptr %t_v516___ast_238___chali_tmp_o_1312
  %r0 = load i64, ptr %s_v516___ast_238___chali_tmp_o_1312
  %r1 = load i8, ptr %t_v516___ast_238___chali_tmp_o_1312
  %r2 = icmp ne i64 %r0, 0
  %r3 = zext i1 %r2 to i64
  %r4 = icmp ne i64 %r3, 0
  br i1 %r4, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r5 = load i64, ptr %s_v516___ast_238___chali_tmp_o_1312
  %r6 = load i8, ptr %t_v516___ast_238___chali_tmp_o_1312
  call void @chali_owned_drop(i64 %r5)
  br label %ast_block_2
ast_block_1:
  br label %ast_block_2
ast_block_2:
  store i64 0, ptr %s_v516___ast_238___chali_tmp_o_1312
  store i8 0, ptr %t_v516___ast_238___chali_tmp_o_1312
  %r7 = call i64 @chali_host_call(ptr @native_name3, i64 0, ptr %indirect_arguments, ptr %native_argument_types)
  %r8 = call i8 @chali_host_result_float()
  store i64 %r7, ptr %s_v514___ast_238___chali_box_value
  store i8 %r8, ptr %t_v514___ast_238___chali_box_value
  %r9 = load i64, ptr %s_v514___ast_238___chali_box_value
  %r10 = load i8, ptr %t_v514___ast_238___chali_box_value
  %r11 = load i64, ptr @s_g_0_program__global
  %r12 = load i8, ptr @t_g_0_program__global
  %r13 = call i64 @chali_require_reference(i64 %r11)
  %r14 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r9, ptr %r14
  %r15 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r10, ptr %r15
  %r16 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 %r13, ptr %r16
  %r17 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r17
  %r18 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 123, ptr %r18
  %r19 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r19
  %r20 = getelementptr i64, ptr %indirect_arguments, i64 3
  store i64 0, ptr %r20
  %r21 = getelementptr i8, ptr %native_argument_types, i64 3
  store i8 0, ptr %r21
  %r22 = getelementptr i64, ptr %indirect_arguments, i64 4
  store i64 0, ptr %r22
  %r23 = getelementptr i8, ptr %native_argument_types, i64 4
  store i8 0, ptr %r23
  %r24 = getelementptr i64, ptr %indirect_arguments, i64 5
  store i64 176, ptr %r24
  %r25 = getelementptr i8, ptr %native_argument_types, i64 5
  store i8 0, ptr %r25
  %r26 = getelementptr i64, ptr %indirect_arguments, i64 6
  store i64 192, ptr %r26
  %r27 = getelementptr i8, ptr %native_argument_types, i64 6
  store i8 0, ptr %r27
  %r28 = call i64 @chali_host_call(ptr @native_name4, i64 7, ptr %indirect_arguments, ptr %native_argument_types)
  %r29 = call i8 @chali_host_result_float()
  %r30 = load i64, ptr %s_v514___ast_238___chali_box_value
  %r31 = load i8, ptr %t_v514___ast_238___chali_box_value
  store i64 %r30, ptr %s_v516___ast_238___chali_tmp_o_1312
  store i8 %r31, ptr %t_v516___ast_238___chali_tmp_o_1312
  %r32 = load i64, ptr %s_v516___ast_238___chali_tmp_o_1312
  %r33 = load i8, ptr %t_v516___ast_238___chali_tmp_o_1312
  %r34 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r32, ptr %r34
  %r35 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r33, ptr %r35
  %r36 = call i64 @chali_host_call(ptr @native_name5, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r37 = call i8 @chali_host_result_float()
  store i64 %r36, ptr %s_v515___ast_238___chali_ret_v_1313
  store i8 %r37, ptr %t_v515___ast_238___chali_ret_v_1313
  %r38 = load i64, ptr %s_v516___ast_238___chali_tmp_o_1312
  %r39 = load i8, ptr %t_v516___ast_238___chali_tmp_o_1312
  %r40 = icmp ne i64 %r38, 0
  %r41 = zext i1 %r40 to i64
  %r42 = icmp ne i64 %r41, 0
  br i1 %r42, label %ast_block_3, label %ast_block_4
ast_block_3:
  %r43 = load i64, ptr %s_v516___ast_238___chali_tmp_o_1312
  %r44 = load i8, ptr %t_v516___ast_238___chali_tmp_o_1312
  call void @chali_owned_drop(i64 %r43)
  br label %ast_block_5
ast_block_4:
  br label %ast_block_5
ast_block_5:
  store i64 0, ptr %s_v516___ast_238___chali_tmp_o_1312
  store i8 0, ptr %t_v516___ast_238___chali_tmp_o_1312
  %r45 = load i64, ptr %s_v515___ast_238___chali_ret_v_1313
  %r46 = load i8, ptr %t_v515___ast_238___chali_ret_v_1313
  store i8 %r46, ptr @chali_return_float
  ret i64 %r45
ast_block_6:
  %r47 = load i64, ptr %s_v516___ast_238___chali_tmp_o_1312
  %r48 = load i8, ptr %t_v516___ast_238___chali_tmp_o_1312
  %r49 = icmp ne i64 %r47, 0
  %r50 = zext i1 %r49 to i64
  %r51 = icmp ne i64 %r50, 0
  br i1 %r51, label %ast_block_7, label %ast_block_8
ast_block_7:
  %r52 = load i64, ptr %s_v516___ast_238___chali_tmp_o_1312
  %r53 = load i8, ptr %t_v516___ast_238___chali_tmp_o_1312
  call void @chali_owned_drop(i64 %r52)
  br label %ast_block_9
ast_block_8:
  br label %ast_block_9
ast_block_9:
  store i64 0, ptr %s_v516___ast_238___chali_tmp_o_1312
  store i8 0, ptr %t_v516___ast_238___chali_tmp_o_1312
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define internal i64 @f_v239_program__consume(i64 %p0) uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [3 x i64]
  %native_argument_types = alloca [3 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v517___ast_239___chali_ret_v_1314 = alloca i64
  store i64 0, ptr %s_v517___ast_239___chali_ret_v_1314
  %t_v517___ast_239___chali_ret_v_1314 = alloca i8
  store i8 0, ptr %t_v517___ast_239___chali_ret_v_1314
  %s_v518___ast_239_view = alloca i64
  store i64 0, ptr %s_v518___ast_239_view
  %t_v518___ast_239_view = alloca i8
  store i8 0, ptr %t_v518___ast_239_view
  store i64 %p0, ptr %s_v518___ast_239_view
  %r0 = load i64, ptr %s_v518___ast_239_view
  %r1 = load i8, ptr %t_v518___ast_239_view
  %r2 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r0, ptr %r2
  %r3 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r1, ptr %r3
  %r4 = call i64 @chali_host_call(ptr @native_name5, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r5 = call i8 @chali_host_result_float()
  store i64 %r4, ptr %s_v518___ast_239_view
  store i8 %r5, ptr %t_v518___ast_239_view
  %r6 = load i64, ptr %s_v518___ast_239_view
  %r7 = load i8, ptr %t_v518___ast_239_view
  %r8 = call i64 @chali_require_reference(i64 %r6)
  store ptr @f_v239_program__consume, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 14, ptr @chali_source_line
  %r9 = call i64 @f_v470___chali_iface_NamedReader_read(i64 %r8, i64 1)
  %r10 = load i8, ptr @chali_return_float
  %r11 = load i64, ptr %s_v518___ast_239_view
  %r12 = load i8, ptr %t_v518___ast_239_view
  %r13 = call i64 @chali_require_reference(i64 %r11)
  store ptr @f_v239_program__consume, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 14, ptr @chali_source_line
  %r14 = call i64 @f_v469___chali_iface_NamedReader_extra(i64 %r13, i64 1)
  %r15 = load i8, ptr @chali_return_float
  store ptr @f_v239_program__consume, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 14, ptr @chali_source_line
  %r16 = add i64 %r9, %r14
  store i64 %r16, ptr %s_v517___ast_239___chali_ret_v_1314
  store i8 0, ptr %t_v517___ast_239___chali_ret_v_1314
  %r17 = load i64, ptr %s_v518___ast_239_view
  %r18 = load i8, ptr %t_v518___ast_239_view
  %r19 = icmp ne i64 %r17, 0
  %r20 = zext i1 %r19 to i64
  %r21 = icmp ne i64 %r20, 0
  br i1 %r21, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r22 = load i64, ptr %s_v518___ast_239_view
  %r23 = load i8, ptr %t_v518___ast_239_view
  call void @chali_owned_drop(i64 %r22)
  br label %ast_block_2
ast_block_1:
  br label %ast_block_2
ast_block_2:
  store i64 0, ptr %s_v518___ast_239_view
  store i8 0, ptr %t_v518___ast_239_view
  %r24 = load i64, ptr %s_v517___ast_239___chali_ret_v_1314
  %r25 = load i8, ptr %t_v517___ast_239___chali_ret_v_1314
  store i8 %r25, ptr @chali_return_float
  ret i64 %r24
ast_block_3:
  %r26 = load i64, ptr %s_v518___ast_239_view
  %r27 = load i8, ptr %t_v518___ast_239_view
  %r28 = icmp ne i64 %r26, 0
  %r29 = zext i1 %r28 to i64
  %r30 = icmp ne i64 %r29, 0
  br i1 %r30, label %ast_block_4, label %ast_block_5
ast_block_4:
  %r31 = load i64, ptr %s_v518___ast_239_view
  %r32 = load i8, ptr %t_v518___ast_239_view
  call void @chali_owned_drop(i64 %r31)
  br label %ast_block_6
ast_block_5:
  br label %ast_block_6
ast_block_6:
  store i64 0, ptr %s_v518___ast_239_view
  store i8 0, ptr %t_v518___ast_239_view
  store i8 0, ptr @chali_return_float
  ret i64 0
ast_label_v519___chali_cleanup_program__consume:
  %r33 = load i64, ptr %s_v518___ast_239_view
  %r34 = load i8, ptr %t_v518___ast_239_view
  %r35 = icmp ne i64 %r33, 0
  %r36 = zext i1 %r35 to i64
  %r37 = icmp ne i64 %r36, 0
  br i1 %r37, label %ast_block_7, label %ast_block_8
ast_block_7:
  %r38 = load i64, ptr %s_v518___ast_239_view
  %r39 = load i8, ptr %t_v518___ast_239_view
  call void @chali_owned_drop(i64 %r38)
  br label %ast_block_9
ast_block_8:
  br label %ast_block_9
ast_block_9:
  store i64 0, ptr %s_v518___ast_239_view
  store i8 0, ptr %t_v518___ast_239_view
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define internal i64 @f_main() uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [8 x i64]
  %native_argument_types = alloca [8 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v520___ast_240___chali_box_value_1322 = alloca i64
  store i64 0, ptr %s_v520___ast_240___chali_box_value_1322
  %t_v520___ast_240___chali_box_value_1322 = alloca i8
  store i8 0, ptr %t_v520___ast_240___chali_box_value_1322
  %s_v521___ast_240___chali_box_value_1324 = alloca i64
  store i64 0, ptr %s_v521___ast_240___chali_box_value_1324
  %t_v521___ast_240___chali_box_value_1324 = alloca i8
  store i8 0, ptr %t_v521___ast_240___chali_box_value_1324
  %s_v522___ast_240___chali_box_value_1326 = alloca i64
  store i64 0, ptr %s_v522___ast_240___chali_box_value_1326
  %t_v522___ast_240___chali_box_value_1326 = alloca i8
  store i8 0, ptr %t_v522___ast_240___chali_box_value_1326
  %s_v523___ast_240___chali_elem_1317 = alloca i64
  store i64 0, ptr %s_v523___ast_240___chali_elem_1317
  %t_v523___ast_240___chali_elem_1317 = alloca i8
  store i8 0, ptr %t_v523___ast_240___chali_elem_1317
  %s_v524___ast_240___chali_elem_1320 = alloca i64
  store i64 0, ptr %s_v524___ast_240___chali_elem_1320
  %t_v524___ast_240___chali_elem_1320 = alloca i8
  store i8 0, ptr %t_v524___ast_240___chali_elem_1320
  %s_v525___ast_240___chali_elem_1330 = alloca i64
  store i64 0, ptr %s_v525___ast_240___chali_elem_1330
  %t_v525___ast_240___chali_elem_1330 = alloca i8
  store i8 0, ptr %t_v525___ast_240___chali_elem_1330
  %s_v526___ast_240___chali_elem_value_1316 = alloca i64
  store i64 0, ptr %s_v526___ast_240___chali_elem_value_1316
  %t_v526___ast_240___chali_elem_value_1316 = alloca i8
  store i8 0, ptr %t_v526___ast_240___chali_elem_value_1316
  %s_v527___ast_240___chali_elem_value_1319 = alloca i64
  store i64 0, ptr %s_v527___ast_240___chali_elem_value_1319
  %t_v527___ast_240___chali_elem_value_1319 = alloca i8
  store i8 0, ptr %t_v527___ast_240___chali_elem_value_1319
  %s_v528___ast_240___chali_elem_value_1329 = alloca i64
  store i64 0, ptr %s_v528___ast_240___chali_elem_value_1329
  %t_v528___ast_240___chali_elem_value_1329 = alloca i8
  store i8 0, ptr %t_v528___ast_240___chali_elem_value_1329
  %s_v529___ast_240___chali_new_v_1328 = alloca i64
  store i64 0, ptr %s_v529___ast_240___chali_new_v_1328
  %t_v529___ast_240___chali_new_v_1328 = alloca i8
  store i8 0, ptr %t_v529___ast_240___chali_new_v_1328
  %s_v530___ast_240___chali_tmp_o_1323 = alloca i64
  store i64 0, ptr %s_v530___ast_240___chali_tmp_o_1323
  %t_v530___ast_240___chali_tmp_o_1323 = alloca i8
  store i8 0, ptr %t_v530___ast_240___chali_tmp_o_1323
  %s_v531___ast_240___chali_tmp_o_1325 = alloca i64
  store i64 0, ptr %s_v531___ast_240___chali_tmp_o_1325
  %t_v531___ast_240___chali_tmp_o_1325 = alloca i8
  store i8 0, ptr %t_v531___ast_240___chali_tmp_o_1325
  %s_v532___ast_240___chali_tmp_o_1327 = alloca i64
  store i64 0, ptr %s_v532___ast_240___chali_tmp_o_1327
  %t_v532___ast_240___chali_tmp_o_1327 = alloca i8
  store i8 0, ptr %t_v532___ast_240___chali_tmp_o_1327
  %s_v533___ast_240_box = alloca i64
  store i64 0, ptr %s_v533___ast_240_box
  %t_v533___ast_240_box = alloca i8
  store i8 0, ptr %t_v533___ast_240_box
  %s_v534___ast_240_boxed = alloca i64
  store i64 0, ptr %s_v534___ast_240_boxed
  %t_v534___ast_240_boxed = alloca i8
  store i8 0, ptr %t_v534___ast_240_boxed
  %s_v535___ast_240_copied = alloca i64
  store i64 0, ptr %s_v535___ast_240_copied
  %t_v535___ast_240_copied = alloca i8
  store i8 0, ptr %t_v535___ast_240_copied
  %s_v536___ast_240_first_1315 = alloca i64
  store i64 0, ptr %s_v536___ast_240_first_1315
  %t_v536___ast_240_first_1315 = alloca i8
  store i8 0, ptr %t_v536___ast_240_first_1315
  %s_v537___ast_240_result_1331 = alloca i64
  store i64 0, ptr %s_v537___ast_240_result_1331
  %t_v537___ast_240_result_1331 = alloca i8
  store i8 0, ptr %t_v537___ast_240_result_1331
  %s_v538___ast_240_saved = alloca i64
  store i64 0, ptr %s_v538___ast_240_saved
  %t_v538___ast_240_saved = alloca i8
  store i8 0, ptr %t_v538___ast_240_saved
  %s_v539___ast_240_second_1318 = alloca i64
  store i64 0, ptr %s_v539___ast_240_second_1318
  %t_v539___ast_240_second_1318 = alloca i8
  store i8 0, ptr %t_v539___ast_240_second_1318
  %s_v540___ast_240_view_1321 = alloca i64
  store i64 0, ptr %s_v540___ast_240_view_1321
  %t_v540___ast_240_view_1321 = alloca i8
  store i8 0, ptr %t_v540___ast_240_view_1321
  %s_v541___ast_240_writer = alloca i64
  store i64 0, ptr %s_v541___ast_240_writer
  %t_v541___ast_240_writer = alloca i8
  store i8 0, ptr %t_v541___ast_240_writer
  store i64 144, ptr @s_g_0_program__global
  store i8 0, ptr @t_g_0_program__global
  %r0 = load i64, ptr @s_g_0_program__global
  %r1 = load i8, ptr @t_g_0_program__global
  %r2 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r0, ptr %r2
  %r3 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r1, ptr %r3
  %r4 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 16, ptr %r4
  %r5 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r5
  %r6 = call i64 @chali_host_call(ptr @native_name1, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r7 = call i8 @chali_host_result_float()
  %r8 = load i64, ptr @s_g_0_program__global
  %r9 = load i8, ptr @t_g_0_program__global
  %r10 = mul i64 0, 8
  %r11 = add i64 %r8, %r10
  %r12 = call ptr @chali_primitive_address(i64 %r11, i64 8)
  store i64 0, ptr %r12, align 1
  %r13 = load i64, ptr @s_g_0_program__global
  %r14 = load i8, ptr @t_g_0_program__global
  %r15 = mul i64 1, 8
  %r16 = add i64 %r13, %r15
  %r17 = call ptr @chali_primitive_address(i64 %r16, i64 8)
  store i64 0, ptr %r17, align 1
  %r18 = call i64 @chali_alloc(i64 16)
  store i64 %r18, ptr %s_v536___ast_240_first_1315
  store i8 0, ptr %t_v536___ast_240_first_1315
  %r19 = load i64, ptr %s_v536___ast_240_first_1315
  %r20 = load i8, ptr %t_v536___ast_240_first_1315
  call void @chali_host_memory_prepare(i64 8, i64 %r19, i64 0, i64 16, ptr %raw_memory_output, ptr %raw_memory_input)
  %r21 = load ptr, ptr %raw_memory_output
  %r22 = trunc i64 0 to i8
  call void @llvm.memset.p0.i64(ptr %r21, i8 %r22, i64 16, i1 false)
  %r23 = load i64, ptr %s_v536___ast_240_first_1315
  %r24 = load i8, ptr %t_v536___ast_240_first_1315
  %r25 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r23, ptr %r25
  %r26 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r24, ptr %r26
  %r27 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 16, ptr %r27
  %r28 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r28
  %r29 = call i64 @chali_host_call(ptr @native_name1, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r30 = call i8 @chali_host_result_float()
  %r31 = load i64, ptr %s_v536___ast_240_first_1315
  %r32 = load i8, ptr %t_v536___ast_240_first_1315
  %r33 = mul i64 0, 8
  %r34 = add i64 %r31, %r33
  %r35 = call ptr @chali_primitive_address(i64 %r34, i64 8)
  store i64 7, ptr %r35, align 1
  %r36 = call i64 @f_v60_std__lang____bootStringCopy(i64 160)
  %r37 = load i8, ptr @chali_return_float
  store i64 %r36, ptr %s_v526___ast_240___chali_elem_value_1316
  store i8 %r37, ptr %t_v526___ast_240___chali_elem_value_1316
  %r38 = load i64, ptr %s_v536___ast_240_first_1315
  %r39 = load i8, ptr %t_v536___ast_240_first_1315
  %r40 = mul i64 1, 8
  %r41 = add i64 %r38, %r40
  store i64 %r41, ptr %s_v523___ast_240___chali_elem_1317
  store i8 0, ptr %t_v523___ast_240___chali_elem_1317
  %r42 = load i64, ptr %s_v523___ast_240___chali_elem_1317
  %r43 = load i8, ptr %t_v523___ast_240___chali_elem_1317
  %r44 = call ptr @chali_primitive_address(i64 %r42, i64 8)
  %r45 = load i64, ptr %r44, align 1
  %r46 = icmp ne i64 %r45, 0
  %r47 = zext i1 %r46 to i64
  %r48 = icmp ne i64 %r47, 0
  br i1 %r48, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r49 = load i64, ptr %s_v523___ast_240___chali_elem_1317
  %r50 = load i8, ptr %t_v523___ast_240___chali_elem_1317
  %r51 = call ptr @chali_primitive_address(i64 %r49, i64 8)
  %r52 = load i64, ptr %r51, align 1
  %r53 = call i64 @f_v61_std__lang____bootStringDrop(i64 %r52)
  %r54 = load i8, ptr @chali_return_float
  br label %ast_block_2
ast_block_1:
  br label %ast_block_2
ast_block_2:
  %r55 = load i64, ptr %s_v526___ast_240___chali_elem_value_1316
  %r56 = load i8, ptr %t_v526___ast_240___chali_elem_value_1316
  %r57 = load i64, ptr %s_v523___ast_240___chali_elem_1317
  %r58 = load i8, ptr %t_v523___ast_240___chali_elem_1317
  %r59 = call ptr @chali_primitive_address(i64 %r57, i64 8)
  store i64 %r55, ptr %r59, align 1
  %r60 = call i64 @chali_alloc(i64 16)
  store i64 %r60, ptr %s_v539___ast_240_second_1318
  store i8 0, ptr %t_v539___ast_240_second_1318
  %r61 = load i64, ptr %s_v539___ast_240_second_1318
  %r62 = load i8, ptr %t_v539___ast_240_second_1318
  call void @chali_host_memory_prepare(i64 8, i64 %r61, i64 0, i64 16, ptr %raw_memory_output, ptr %raw_memory_input)
  %r63 = load ptr, ptr %raw_memory_output
  %r64 = trunc i64 0 to i8
  call void @llvm.memset.p0.i64(ptr %r63, i8 %r64, i64 16, i1 false)
  %r65 = load i64, ptr %s_v539___ast_240_second_1318
  %r66 = load i8, ptr %t_v539___ast_240_second_1318
  %r67 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r65, ptr %r67
  %r68 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r66, ptr %r68
  %r69 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 16, ptr %r69
  %r70 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r70
  %r71 = call i64 @chali_host_call(ptr @native_name1, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r72 = call i8 @chali_host_result_float()
  %r73 = load i64, ptr %s_v539___ast_240_second_1318
  %r74 = load i8, ptr %t_v539___ast_240_second_1318
  %r75 = mul i64 0, 8
  %r76 = add i64 %r73, %r75
  %r77 = call ptr @chali_primitive_address(i64 %r76, i64 8)
  store i64 19, ptr %r77, align 1
  %r78 = call i64 @f_v60_std__lang____bootStringCopy(i64 168)
  %r79 = load i8, ptr @chali_return_float
  store i64 %r78, ptr %s_v527___ast_240___chali_elem_value_1319
  store i8 %r79, ptr %t_v527___ast_240___chali_elem_value_1319
  %r80 = load i64, ptr %s_v539___ast_240_second_1318
  %r81 = load i8, ptr %t_v539___ast_240_second_1318
  %r82 = mul i64 1, 8
  %r83 = add i64 %r80, %r82
  store i64 %r83, ptr %s_v524___ast_240___chali_elem_1320
  store i8 0, ptr %t_v524___ast_240___chali_elem_1320
  %r84 = load i64, ptr %s_v524___ast_240___chali_elem_1320
  %r85 = load i8, ptr %t_v524___ast_240___chali_elem_1320
  %r86 = call ptr @chali_primitive_address(i64 %r84, i64 8)
  %r87 = load i64, ptr %r86, align 1
  %r88 = icmp ne i64 %r87, 0
  %r89 = zext i1 %r88 to i64
  %r90 = icmp ne i64 %r89, 0
  br i1 %r90, label %ast_block_3, label %ast_block_4
ast_block_3:
  %r91 = load i64, ptr %s_v524___ast_240___chali_elem_1320
  %r92 = load i8, ptr %t_v524___ast_240___chali_elem_1320
  %r93 = call ptr @chali_primitive_address(i64 %r91, i64 8)
  %r94 = load i64, ptr %r93, align 1
  %r95 = call i64 @f_v61_std__lang____bootStringDrop(i64 %r94)
  %r96 = load i8, ptr @chali_return_float
  br label %ast_block_5
ast_block_4:
  br label %ast_block_5
ast_block_5:
  %r97 = load i64, ptr %s_v527___ast_240___chali_elem_value_1319
  %r98 = load i8, ptr %t_v527___ast_240___chali_elem_value_1319
  %r99 = load i64, ptr %s_v524___ast_240___chali_elem_1320
  %r100 = load i8, ptr %t_v524___ast_240___chali_elem_1320
  %r101 = call ptr @chali_primitive_address(i64 %r99, i64 8)
  store i64 %r97, ptr %r101, align 1
  %r102 = load i64, ptr %s_v530___ast_240___chali_tmp_o_1323
  %r103 = load i8, ptr %t_v530___ast_240___chali_tmp_o_1323
  %r104 = icmp ne i64 %r102, 0
  %r105 = zext i1 %r104 to i64
  %r106 = icmp ne i64 %r105, 0
  br i1 %r106, label %ast_block_6, label %ast_block_7
ast_block_6:
  %r107 = load i64, ptr %s_v530___ast_240___chali_tmp_o_1323
  %r108 = load i8, ptr %t_v530___ast_240___chali_tmp_o_1323
  call void @chali_owned_drop(i64 %r107)
  br label %ast_block_8
ast_block_7:
  br label %ast_block_8
ast_block_8:
  store i64 0, ptr %s_v530___ast_240___chali_tmp_o_1323
  store i8 0, ptr %t_v530___ast_240___chali_tmp_o_1323
  %r109 = call i64 @chali_host_call(ptr @native_name3, i64 0, ptr %indirect_arguments, ptr %native_argument_types)
  %r110 = call i8 @chali_host_result_float()
  store i64 %r109, ptr %s_v520___ast_240___chali_box_value_1322
  store i8 %r110, ptr %t_v520___ast_240___chali_box_value_1322
  %r111 = load i64, ptr %s_v520___ast_240___chali_box_value_1322
  %r112 = load i8, ptr %t_v520___ast_240___chali_box_value_1322
  %r113 = load i64, ptr %s_v536___ast_240_first_1315
  %r114 = load i8, ptr %t_v536___ast_240_first_1315
  %r115 = call i64 @chali_require_reference(i64 %r113)
  %r116 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r111, ptr %r116
  %r117 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r112, ptr %r117
  %r118 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 %r115, ptr %r118
  %r119 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r119
  %r120 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 123, ptr %r120
  %r121 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r121
  %r122 = getelementptr i64, ptr %indirect_arguments, i64 3
  store i64 0, ptr %r122
  %r123 = getelementptr i8, ptr %native_argument_types, i64 3
  store i8 0, ptr %r123
  %r124 = getelementptr i64, ptr %indirect_arguments, i64 4
  store i64 0, ptr %r124
  %r125 = getelementptr i8, ptr %native_argument_types, i64 4
  store i8 0, ptr %r125
  %r126 = getelementptr i64, ptr %indirect_arguments, i64 5
  store i64 176, ptr %r126
  %r127 = getelementptr i8, ptr %native_argument_types, i64 5
  store i8 0, ptr %r127
  %r128 = getelementptr i64, ptr %indirect_arguments, i64 6
  store i64 192, ptr %r128
  %r129 = getelementptr i8, ptr %native_argument_types, i64 6
  store i8 0, ptr %r129
  %r130 = call i64 @chali_host_call(ptr @native_name4, i64 7, ptr %indirect_arguments, ptr %native_argument_types)
  %r131 = call i8 @chali_host_result_float()
  %r132 = load i64, ptr %s_v520___ast_240___chali_box_value_1322
  %r133 = load i8, ptr %t_v520___ast_240___chali_box_value_1322
  store i64 %r132, ptr %s_v530___ast_240___chali_tmp_o_1323
  store i8 %r133, ptr %t_v530___ast_240___chali_tmp_o_1323
  %r134 = load i64, ptr %s_v530___ast_240___chali_tmp_o_1323
  %r135 = load i8, ptr %t_v530___ast_240___chali_tmp_o_1323
  %r136 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r134, ptr %r136
  %r137 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r135, ptr %r137
  %r138 = call i64 @chali_host_call(ptr @native_name5, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r139 = call i8 @chali_host_result_float()
  store i64 %r138, ptr %s_v540___ast_240_view_1321
  store i8 %r139, ptr %t_v540___ast_240_view_1321
  %r140 = load i64, ptr %s_v540___ast_240_view_1321
  %r141 = load i8, ptr %t_v540___ast_240_view_1321
  %r142 = call i64 @chali_require_reference(i64 %r140)
  %r143 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r142, ptr %r143
  %r144 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 0, ptr %r144
  %r145 = call i64 @chali_host_call(ptr @native_name5, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r146 = call i8 @chali_host_result_float()
  store i64 %r145, ptr %s_v538___ast_240_saved
  store i8 %r146, ptr %t_v538___ast_240_saved
  %r147 = load i64, ptr %s_v540___ast_240_view_1321
  %r148 = load i8, ptr %t_v540___ast_240_view_1321
  %r149 = call i64 @chali_require_reference(i64 %r147)
  %r150 = call i64 @chali_require_reference(i64 %r149)
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 21, ptr @chali_source_line
  %r151 = call i64 @f_v239_program__consume(i64 %r150)
  %r152 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 21, ptr @chali_source_line
  %r153 = icmp eq i64 %r151, 12
  %r154 = zext i1 %r153 to i64
  %r155 = icmp ne i64 %r154, 0
  br i1 %r155, label %ast_block_10, label %ast_block_9
ast_block_9:
  br label %ast_block_11
ast_block_10:
  %r156 = load i64, ptr %s_v538___ast_240_saved
  %r157 = load i8, ptr %t_v538___ast_240_saved
  %r158 = call i64 @chali_require_reference(i64 %r156)
  %r159 = call i64 @chali_require_reference(i64 %r158)
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 21, ptr @chali_source_line
  %r160 = call i64 @f_v239_program__consume(i64 %r159)
  %r161 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 21, ptr @chali_source_line
  %r162 = icmp eq i64 %r160, 12
  %r163 = zext i1 %r162 to i64
  %r164 = icmp ne i64 %r163, 0
  br label %ast_block_12
ast_block_12:
  br label %ast_block_11
ast_block_11:
  %r165 = phi i1 [ false, %ast_block_9 ], [ %r164, %ast_block_12 ]
  %r166 = zext i1 %r165 to i64
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 21, ptr @chali_source_line
  %r167 = call i64 @f_v162_std__lang__assert_bool(i64 %r166)
  %r168 = load i8, ptr @chali_return_float
  %r169 = load i64, ptr %s_v540___ast_240_view_1321
  %r170 = load i8, ptr %t_v540___ast_240_view_1321
  %r171 = call i64 @chali_require_reference(i64 %r169)
  %r172 = call i64 @chali_require_reference(i64 %r171)
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 22, ptr @chali_source_line
  %r173 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r172, ptr %r173
  %r174 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 0, ptr %r174
  %r175 = call i64 @chali_host_call(ptr @native_name6, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r176 = call i8 @chali_host_result_float()
  %r177 = load i64, ptr %s_v536___ast_240_first_1315
  %r178 = load i8, ptr %t_v536___ast_240_first_1315
  %r179 = call i64 @chali_require_reference(i64 %r177)
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 22, ptr @chali_source_line
  %r180 = icmp eq i64 %r175, %r179
  %r181 = zext i1 %r180 to i64
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 22, ptr @chali_source_line
  %r182 = call i64 @f_v162_std__lang__assert_bool(i64 %r181)
  %r183 = load i8, ptr @chali_return_float
  %r184 = load i64, ptr %s_v531___ast_240___chali_tmp_o_1325
  %r185 = load i8, ptr %t_v531___ast_240___chali_tmp_o_1325
  %r186 = icmp ne i64 %r184, 0
  %r187 = zext i1 %r186 to i64
  %r188 = icmp ne i64 %r187, 0
  br i1 %r188, label %ast_block_13, label %ast_block_14
ast_block_13:
  %r189 = load i64, ptr %s_v531___ast_240___chali_tmp_o_1325
  %r190 = load i8, ptr %t_v531___ast_240___chali_tmp_o_1325
  call void @chali_owned_drop(i64 %r189)
  br label %ast_block_15
ast_block_14:
  br label %ast_block_15
ast_block_15:
  store i64 0, ptr %s_v531___ast_240___chali_tmp_o_1325
  store i8 0, ptr %t_v531___ast_240___chali_tmp_o_1325
  %r191 = call i64 @chali_host_call(ptr @native_name3, i64 0, ptr %indirect_arguments, ptr %native_argument_types)
  %r192 = call i8 @chali_host_result_float()
  store i64 %r191, ptr %s_v521___ast_240___chali_box_value_1324
  store i8 %r192, ptr %t_v521___ast_240___chali_box_value_1324
  %r193 = load i64, ptr %s_v521___ast_240___chali_box_value_1324
  %r194 = load i8, ptr %t_v521___ast_240___chali_box_value_1324
  %r195 = load i64, ptr %s_v536___ast_240_first_1315
  %r196 = load i8, ptr %t_v536___ast_240_first_1315
  %r197 = call i64 @chali_require_reference(i64 %r195)
  %r198 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r193, ptr %r198
  %r199 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r194, ptr %r199
  %r200 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 %r197, ptr %r200
  %r201 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r201
  %r202 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 123, ptr %r202
  %r203 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r203
  %r204 = getelementptr i64, ptr %indirect_arguments, i64 3
  store i64 0, ptr %r204
  %r205 = getelementptr i8, ptr %native_argument_types, i64 3
  store i8 0, ptr %r205
  %r206 = getelementptr i64, ptr %indirect_arguments, i64 4
  store i64 0, ptr %r206
  %r207 = getelementptr i8, ptr %native_argument_types, i64 4
  store i8 0, ptr %r207
  %r208 = getelementptr i64, ptr %indirect_arguments, i64 5
  store i64 176, ptr %r208
  %r209 = getelementptr i8, ptr %native_argument_types, i64 5
  store i8 0, ptr %r209
  %r210 = getelementptr i64, ptr %indirect_arguments, i64 6
  store i64 192, ptr %r210
  %r211 = getelementptr i8, ptr %native_argument_types, i64 6
  store i8 0, ptr %r211
  %r212 = call i64 @chali_host_call(ptr @native_name4, i64 7, ptr %indirect_arguments, ptr %native_argument_types)
  %r213 = call i8 @chali_host_result_float()
  %r214 = load i64, ptr %s_v521___ast_240___chali_box_value_1324
  %r215 = load i8, ptr %t_v521___ast_240___chali_box_value_1324
  store i64 %r214, ptr %s_v531___ast_240___chali_tmp_o_1325
  store i8 %r215, ptr %t_v531___ast_240___chali_tmp_o_1325
  %r216 = load i64, ptr %s_v531___ast_240___chali_tmp_o_1325
  %r217 = load i8, ptr %t_v531___ast_240___chali_tmp_o_1325
  %r218 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r216, ptr %r218
  %r219 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r217, ptr %r219
  %r220 = call i64 @chali_host_call(ptr @native_name5, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r221 = call i8 @chali_host_result_float()
  store i64 %r220, ptr %s_v541___ast_240_writer
  store i8 %r221, ptr %t_v541___ast_240_writer
  %r222 = load i64, ptr %s_v541___ast_240_writer
  %r223 = load i8, ptr %t_v541___ast_240_writer
  %r224 = call i64 @chali_require_reference(i64 %r222)
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 24, ptr @chali_source_line
  %r225 = call i64 @f_v472___chali_iface_Writer_write(i64 %r224, i64 0, i64 11)
  %r226 = load i8, ptr @chali_return_float
  %r227 = load i64, ptr %s_v538___ast_240_saved
  %r228 = load i8, ptr %t_v538___ast_240_saved
  %r229 = call i64 @chali_require_reference(i64 %r227)
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 25, ptr @chali_source_line
  %r230 = call i64 @f_v470___chali_iface_NamedReader_read(i64 %r229, i64 1)
  %r231 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 25, ptr @chali_source_line
  %r232 = icmp eq i64 %r230, 11
  %r233 = zext i1 %r232 to i64
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 25, ptr @chali_source_line
  %r234 = call i64 @f_v162_std__lang__assert_bool(i64 %r233)
  %r235 = load i8, ptr @chali_return_float
  %r236 = load i64, ptr %s_v532___ast_240___chali_tmp_o_1327
  %r237 = load i8, ptr %t_v532___ast_240___chali_tmp_o_1327
  %r238 = icmp ne i64 %r236, 0
  %r239 = zext i1 %r238 to i64
  %r240 = icmp ne i64 %r239, 0
  br i1 %r240, label %ast_block_16, label %ast_block_17
ast_block_16:
  %r241 = load i64, ptr %s_v532___ast_240___chali_tmp_o_1327
  %r242 = load i8, ptr %t_v532___ast_240___chali_tmp_o_1327
  call void @chali_owned_drop(i64 %r241)
  br label %ast_block_18
ast_block_17:
  br label %ast_block_18
ast_block_18:
  store i64 0, ptr %s_v532___ast_240___chali_tmp_o_1327
  store i8 0, ptr %t_v532___ast_240___chali_tmp_o_1327
  %r243 = call i64 @chali_host_call(ptr @native_name3, i64 0, ptr %indirect_arguments, ptr %native_argument_types)
  %r244 = call i8 @chali_host_result_float()
  store i64 %r243, ptr %s_v522___ast_240___chali_box_value_1326
  store i8 %r244, ptr %t_v522___ast_240___chali_box_value_1326
  %r245 = load i64, ptr %s_v522___ast_240___chali_box_value_1326
  %r246 = load i8, ptr %t_v522___ast_240___chali_box_value_1326
  %r247 = load i64, ptr %s_v539___ast_240_second_1318
  %r248 = load i8, ptr %t_v539___ast_240_second_1318
  %r249 = call i64 @chali_require_reference(i64 %r247)
  %r250 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r245, ptr %r250
  %r251 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r246, ptr %r251
  %r252 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 %r249, ptr %r252
  %r253 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r253
  %r254 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 123, ptr %r254
  %r255 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r255
  %r256 = getelementptr i64, ptr %indirect_arguments, i64 3
  store i64 0, ptr %r256
  %r257 = getelementptr i8, ptr %native_argument_types, i64 3
  store i8 0, ptr %r257
  %r258 = getelementptr i64, ptr %indirect_arguments, i64 4
  store i64 0, ptr %r258
  %r259 = getelementptr i8, ptr %native_argument_types, i64 4
  store i8 0, ptr %r259
  %r260 = getelementptr i64, ptr %indirect_arguments, i64 5
  store i64 176, ptr %r260
  %r261 = getelementptr i8, ptr %native_argument_types, i64 5
  store i8 0, ptr %r261
  %r262 = getelementptr i64, ptr %indirect_arguments, i64 6
  store i64 192, ptr %r262
  %r263 = getelementptr i8, ptr %native_argument_types, i64 6
  store i8 0, ptr %r263
  %r264 = call i64 @chali_host_call(ptr @native_name4, i64 7, ptr %indirect_arguments, ptr %native_argument_types)
  %r265 = call i8 @chali_host_result_float()
  %r266 = load i64, ptr %s_v522___ast_240___chali_box_value_1326
  %r267 = load i8, ptr %t_v522___ast_240___chali_box_value_1326
  store i64 %r266, ptr %s_v532___ast_240___chali_tmp_o_1327
  store i8 %r267, ptr %t_v532___ast_240___chali_tmp_o_1327
  %r268 = load i64, ptr %s_v532___ast_240___chali_tmp_o_1327
  %r269 = load i8, ptr %t_v532___ast_240___chali_tmp_o_1327
  %r270 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r268, ptr %r270
  %r271 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r269, ptr %r271
  %r272 = call i64 @chali_host_call(ptr @native_name5, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r273 = call i8 @chali_host_result_float()
  store i64 %r272, ptr %s_v529___ast_240___chali_new_v_1328
  store i8 %r273, ptr %t_v529___ast_240___chali_new_v_1328
  %r274 = load i64, ptr %s_v540___ast_240_view_1321
  %r275 = load i8, ptr %t_v540___ast_240_view_1321
  %r276 = icmp ne i64 %r274, 0
  %r277 = zext i1 %r276 to i64
  %r278 = icmp ne i64 %r277, 0
  br i1 %r278, label %ast_block_19, label %ast_block_20
ast_block_19:
  %r279 = load i64, ptr %s_v540___ast_240_view_1321
  %r280 = load i8, ptr %t_v540___ast_240_view_1321
  call void @chali_owned_drop(i64 %r279)
  br label %ast_block_21
ast_block_20:
  br label %ast_block_21
ast_block_21:
  store i64 0, ptr %s_v540___ast_240_view_1321
  store i8 0, ptr %t_v540___ast_240_view_1321
  %r281 = load i64, ptr %s_v529___ast_240___chali_new_v_1328
  %r282 = load i8, ptr %t_v529___ast_240___chali_new_v_1328
  store i64 %r281, ptr %s_v540___ast_240_view_1321
  store i8 %r282, ptr %t_v540___ast_240_view_1321
  %r283 = load i64, ptr %s_v532___ast_240___chali_tmp_o_1327
  %r284 = load i8, ptr %t_v532___ast_240___chali_tmp_o_1327
  %r285 = icmp ne i64 %r283, 0
  %r286 = zext i1 %r285 to i64
  %r287 = icmp ne i64 %r286, 0
  br i1 %r287, label %ast_block_22, label %ast_block_23
ast_block_22:
  %r288 = load i64, ptr %s_v532___ast_240___chali_tmp_o_1327
  %r289 = load i8, ptr %t_v532___ast_240___chali_tmp_o_1327
  call void @chali_owned_drop(i64 %r288)
  br label %ast_block_24
ast_block_23:
  br label %ast_block_24
ast_block_24:
  store i64 0, ptr %s_v532___ast_240___chali_tmp_o_1327
  store i8 0, ptr %t_v532___ast_240___chali_tmp_o_1327
  %r290 = load i64, ptr %s_v540___ast_240_view_1321
  %r291 = load i8, ptr %t_v540___ast_240_view_1321
  %r292 = call i64 @chali_require_reference(i64 %r290)
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 27, ptr @chali_source_line
  %r293 = call i64 @f_v470___chali_iface_NamedReader_read(i64 %r292, i64 1)
  %r294 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 27, ptr @chali_source_line
  %r295 = icmp eq i64 %r293, 19
  %r296 = zext i1 %r295 to i64
  %r297 = icmp ne i64 %r296, 0
  br i1 %r297, label %ast_block_26, label %ast_block_25
ast_block_25:
  br label %ast_block_27
ast_block_26:
  %r298 = load i64, ptr %s_v538___ast_240_saved
  %r299 = load i8, ptr %t_v538___ast_240_saved
  %r300 = call i64 @chali_require_reference(i64 %r298)
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 27, ptr @chali_source_line
  %r301 = call i64 @f_v470___chali_iface_NamedReader_read(i64 %r300, i64 1)
  %r302 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 27, ptr @chali_source_line
  %r303 = icmp eq i64 %r301, 11
  %r304 = zext i1 %r303 to i64
  %r305 = icmp ne i64 %r304, 0
  br label %ast_block_28
ast_block_28:
  br label %ast_block_27
ast_block_27:
  %r306 = phi i1 [ false, %ast_block_25 ], [ %r305, %ast_block_28 ]
  %r307 = zext i1 %r306 to i64
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 27, ptr @chali_source_line
  %r308 = call i64 @f_v162_std__lang__assert_bool(i64 %r307)
  %r309 = load i8, ptr @chali_return_float
  %r310 = call i64 @chali_host_call(ptr @native_name3, i64 0, ptr %indirect_arguments, ptr %native_argument_types)
  %r311 = call i8 @chali_host_result_float()
  store i64 %r310, ptr %s_v533___ast_240_box
  store i8 %r311, ptr %t_v533___ast_240_box
  %r312 = load i64, ptr %s_v533___ast_240_box
  %r313 = load i8, ptr %t_v533___ast_240_box
  %r314 = load i64, ptr %s_v536___ast_240_first_1315
  %r315 = load i8, ptr %t_v536___ast_240_first_1315
  %r316 = call i64 @chali_require_reference(i64 %r314)
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 28, ptr @chali_source_line
  %r317 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r312, ptr %r317
  %r318 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r313, ptr %r318
  %r319 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 %r316, ptr %r319
  %r320 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r320
  %r321 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 123, ptr %r321
  %r322 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r322
  %r323 = getelementptr i64, ptr %indirect_arguments, i64 3
  store i64 0, ptr %r323
  %r324 = getelementptr i8, ptr %native_argument_types, i64 3
  store i8 0, ptr %r324
  %r325 = getelementptr i64, ptr %indirect_arguments, i64 4
  store i64 0, ptr %r325
  %r326 = getelementptr i8, ptr %native_argument_types, i64 4
  store i8 0, ptr %r326
  %r327 = getelementptr i64, ptr %indirect_arguments, i64 5
  store i64 176, ptr %r327
  %r328 = getelementptr i8, ptr %native_argument_types, i64 5
  store i8 0, ptr %r328
  %r329 = getelementptr i64, ptr %indirect_arguments, i64 6
  store i64 192, ptr %r329
  %r330 = getelementptr i8, ptr %native_argument_types, i64 6
  store i8 0, ptr %r330
  %r331 = call i64 @chali_host_call(ptr @native_name4, i64 7, ptr %indirect_arguments, ptr %native_argument_types)
  %r332 = call i8 @chali_host_result_float()
  %r333 = load i64, ptr %s_v533___ast_240_box
  %r334 = load i8, ptr %t_v533___ast_240_box
  %r335 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r333, ptr %r335
  %r336 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r334, ptr %r336
  %r337 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 224, ptr %r337
  %r338 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r338
  %r339 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 0, ptr %r339
  %r340 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r340
  %r341 = getelementptr i64, ptr %indirect_arguments, i64 3
  store i64 0, ptr %r341
  %r342 = getelementptr i8, ptr %native_argument_types, i64 3
  store i8 0, ptr %r342
  %r343 = getelementptr i64, ptr %indirect_arguments, i64 4
  store i64 1, ptr %r343
  %r344 = getelementptr i8, ptr %native_argument_types, i64 4
  store i8 0, ptr %r344
  %r345 = getelementptr i64, ptr %indirect_arguments, i64 5
  store i64 122, ptr %r345
  %r346 = getelementptr i8, ptr %native_argument_types, i64 5
  store i8 0, ptr %r346
  %r347 = getelementptr i64, ptr %indirect_arguments, i64 6
  store i64 123, ptr %r347
  %r348 = getelementptr i8, ptr %native_argument_types, i64 6
  store i8 0, ptr %r348
  %r349 = call i64 @chali_host_call(ptr @native_name7, i64 7, ptr %indirect_arguments, ptr %native_argument_types)
  %r350 = call i8 @chali_host_result_float()
  %r351 = call i64 @chali_require_reference(i64 %r349)
  store i64 %r351, ptr %s_v534___ast_240_boxed
  store i8 0, ptr %t_v534___ast_240_boxed
  %r352 = load i64, ptr %s_v534___ast_240_boxed
  %r353 = load i8, ptr %t_v534___ast_240_boxed
  %r354 = call i64 @chali_require_reference(i64 %r352)
  %r355 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r354, ptr %r355
  %r356 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 0, ptr %r356
  %r357 = call i64 @chali_host_call(ptr @native_name5, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r358 = call i8 @chali_host_result_float()
  store i64 %r357, ptr %s_v535___ast_240_copied
  store i8 %r358, ptr %t_v535___ast_240_copied
  %r359 = load i64, ptr %s_v534___ast_240_boxed
  %r360 = load i8, ptr %t_v534___ast_240_boxed
  %r361 = call i64 @chali_require_reference(i64 %r359)
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 31, ptr @chali_source_line
  %r362 = call i64 @f_v471___chali_iface_Reader_read(i64 %r361, i64 1)
  %r363 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 31, ptr @chali_source_line
  %r364 = icmp eq i64 %r362, 11
  %r365 = zext i1 %r364 to i64
  %r366 = icmp ne i64 %r365, 0
  br i1 %r366, label %ast_block_30, label %ast_block_29
ast_block_29:
  br label %ast_block_31
ast_block_30:
  %r367 = load i64, ptr %s_v535___ast_240_copied
  %r368 = load i8, ptr %t_v535___ast_240_copied
  %r369 = call i64 @chali_require_reference(i64 %r367)
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 31, ptr @chali_source_line
  %r370 = call i64 @f_v471___chali_iface_Reader_read(i64 %r369, i64 1)
  %r371 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 31, ptr @chali_source_line
  %r372 = icmp eq i64 %r370, 11
  %r373 = zext i1 %r372 to i64
  %r374 = icmp ne i64 %r373, 0
  br label %ast_block_32
ast_block_32:
  br label %ast_block_31
ast_block_31:
  %r375 = phi i1 [ false, %ast_block_29 ], [ %r374, %ast_block_32 ]
  %r376 = zext i1 %r375 to i64
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 31, ptr @chali_source_line
  %r377 = call i64 @f_v162_std__lang__assert_bool(i64 %r376)
  %r378 = load i8, ptr @chali_return_float
  %r379 = load i64, ptr @s_g_0_program__global
  %r380 = load i8, ptr @t_g_0_program__global
  %r381 = mul i64 0, 8
  %r382 = add i64 %r379, %r381
  %r383 = call ptr @chali_primitive_address(i64 %r382, i64 8)
  store i64 23, ptr %r383, align 1
  %r384 = call i64 @f_v60_std__lang____bootStringCopy(i64 240)
  %r385 = load i8, ptr @chali_return_float
  store i64 %r384, ptr %s_v528___ast_240___chali_elem_value_1329
  store i8 %r385, ptr %t_v528___ast_240___chali_elem_value_1329
  %r386 = load i64, ptr @s_g_0_program__global
  %r387 = load i8, ptr @t_g_0_program__global
  %r388 = mul i64 1, 8
  %r389 = add i64 %r386, %r388
  store i64 %r389, ptr %s_v525___ast_240___chali_elem_1330
  store i8 0, ptr %t_v525___ast_240___chali_elem_1330
  %r390 = load i64, ptr %s_v525___ast_240___chali_elem_1330
  %r391 = load i8, ptr %t_v525___ast_240___chali_elem_1330
  %r392 = call ptr @chali_primitive_address(i64 %r390, i64 8)
  %r393 = load i64, ptr %r392, align 1
  %r394 = icmp ne i64 %r393, 0
  %r395 = zext i1 %r394 to i64
  %r396 = icmp ne i64 %r395, 0
  br i1 %r396, label %ast_block_33, label %ast_block_34
ast_block_33:
  %r397 = load i64, ptr %s_v525___ast_240___chali_elem_1330
  %r398 = load i8, ptr %t_v525___ast_240___chali_elem_1330
  %r399 = call ptr @chali_primitive_address(i64 %r397, i64 8)
  %r400 = load i64, ptr %r399, align 1
  %r401 = call i64 @f_v61_std__lang____bootStringDrop(i64 %r400)
  %r402 = load i8, ptr @chali_return_float
  br label %ast_block_35
ast_block_34:
  br label %ast_block_35
ast_block_35:
  %r403 = load i64, ptr %s_v528___ast_240___chali_elem_value_1329
  %r404 = load i8, ptr %t_v528___ast_240___chali_elem_value_1329
  %r405 = load i64, ptr %s_v525___ast_240___chali_elem_1330
  %r406 = load i8, ptr %t_v525___ast_240___chali_elem_1330
  %r407 = call ptr @chali_primitive_address(i64 %r405, i64 8)
  store i64 %r403, ptr %r407, align 1
  %r408 = call i64 @f_v238_program__returned()
  %r409 = load i8, ptr @chali_return_float
  %r410 = call i64 @chali_require_reference(i64 %r408)
  store i64 %r410, ptr %s_v537___ast_240_result_1331
  store i8 0, ptr %t_v537___ast_240_result_1331
  %r411 = load i64, ptr %s_v537___ast_240_result_1331
  %r412 = load i8, ptr %t_v537___ast_240_result_1331
  %r413 = call i64 @chali_require_reference(i64 %r411)
  %r414 = call i64 @chali_require_reference(i64 %r413)
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 34, ptr @chali_source_line
  %r415 = call i64 @f_v239_program__consume(i64 %r414)
  %r416 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 34, ptr @chali_source_line
  %r417 = icmp eq i64 %r415, 29
  %r418 = zext i1 %r417 to i64
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 34, ptr @chali_source_line
  %r419 = call i64 @f_v162_std__lang__assert_bool(i64 %r418)
  %r420 = load i8, ptr @chali_return_float
  store ptr @f_main, ptr @chali_source_function
  store ptr @source_file1, ptr @chali_source_file
  store i64 35, ptr @chali_source_line
  %r421 = call ptr @chali_standard_stream(i32 1)
  %r422 = getelementptr [29 x i8], ptr @chali_abi_lit0, i64 0, i64 0
  %r423 = call i64 @fwrite(ptr %r422, i64 1, i64 29, ptr %r421)
  %r424 = load i64, ptr %s_v539___ast_240_second_1318
  %r425 = load i8, ptr %t_v539___ast_240_second_1318
  %r426 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r424, ptr %r426
  %r427 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r425, ptr %r427
  %r428 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 16, ptr %r428
  %r429 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r429
  %r430 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 34, ptr %r430
  %r431 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r431
  %r432 = call i64 @chali_host_call(ptr @native_name8, i64 3, ptr %indirect_arguments, ptr %native_argument_types)
  %r433 = call i8 @chali_host_result_float()
  %r434 = icmp ne i64 %r432, 0
  br i1 %r434, label %ast_block_36, label %ast_block_37
ast_block_36:
  %r435 = load i64, ptr %s_v539___ast_240_second_1318
  %r436 = load i8, ptr %t_v539___ast_240_second_1318
  %r437 = call i64 @f_v475___chali_agg_rel_Value(i64 %r435)
  %r438 = load i8, ptr @chali_return_float
  br label %ast_block_38
ast_block_37:
  br label %ast_block_38
ast_block_38:
  %r439 = load i64, ptr %s_v536___ast_240_first_1315
  %r440 = load i8, ptr %t_v536___ast_240_first_1315
  %r441 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r439, ptr %r441
  %r442 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r440, ptr %r442
  %r443 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 16, ptr %r443
  %r444 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r444
  %r445 = getelementptr i64, ptr %indirect_arguments, i64 2
  store i64 34, ptr %r445
  %r446 = getelementptr i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r446
  %r447 = call i64 @chali_host_call(ptr @native_name8, i64 3, ptr %indirect_arguments, ptr %native_argument_types)
  %r448 = call i8 @chali_host_result_float()
  %r449 = icmp ne i64 %r447, 0
  br i1 %r449, label %ast_block_39, label %ast_block_40
ast_block_39:
  %r450 = load i64, ptr %s_v536___ast_240_first_1315
  %r451 = load i8, ptr %t_v536___ast_240_first_1315
  %r452 = call i64 @f_v475___chali_agg_rel_Value(i64 %r450)
  %r453 = load i8, ptr @chali_return_float
  br label %ast_block_41
ast_block_40:
  br label %ast_block_41
ast_block_41:
  %r454 = load i64, ptr %s_v537___ast_240_result_1331
  %r455 = load i8, ptr %t_v537___ast_240_result_1331
  %r456 = icmp ne i64 %r454, 0
  %r457 = zext i1 %r456 to i64
  %r458 = icmp ne i64 %r457, 0
  br i1 %r458, label %ast_block_42, label %ast_block_43
ast_block_42:
  %r459 = load i64, ptr %s_v537___ast_240_result_1331
  %r460 = load i8, ptr %t_v537___ast_240_result_1331
  call void @chali_owned_drop(i64 %r459)
  br label %ast_block_44
ast_block_43:
  br label %ast_block_44
ast_block_44:
  store i64 0, ptr %s_v537___ast_240_result_1331
  store i8 0, ptr %t_v537___ast_240_result_1331
  %r461 = load i64, ptr %s_v535___ast_240_copied
  %r462 = load i8, ptr %t_v535___ast_240_copied
  %r463 = icmp ne i64 %r461, 0
  %r464 = zext i1 %r463 to i64
  %r465 = icmp ne i64 %r464, 0
  br i1 %r465, label %ast_block_45, label %ast_block_46
ast_block_45:
  %r466 = load i64, ptr %s_v535___ast_240_copied
  %r467 = load i8, ptr %t_v535___ast_240_copied
  call void @chali_owned_drop(i64 %r466)
  br label %ast_block_47
ast_block_46:
  br label %ast_block_47
ast_block_47:
  store i64 0, ptr %s_v535___ast_240_copied
  store i8 0, ptr %t_v535___ast_240_copied
  %r468 = load i64, ptr %s_v534___ast_240_boxed
  %r469 = load i8, ptr %t_v534___ast_240_boxed
  %r470 = icmp ne i64 %r468, 0
  %r471 = zext i1 %r470 to i64
  %r472 = icmp ne i64 %r471, 0
  br i1 %r472, label %ast_block_48, label %ast_block_49
ast_block_48:
  %r473 = load i64, ptr %s_v534___ast_240_boxed
  %r474 = load i8, ptr %t_v534___ast_240_boxed
  call void @chali_owned_drop(i64 %r473)
  br label %ast_block_50
ast_block_49:
  br label %ast_block_50
ast_block_50:
  store i64 0, ptr %s_v534___ast_240_boxed
  store i8 0, ptr %t_v534___ast_240_boxed
  %r475 = load i64, ptr %s_v533___ast_240_box
  %r476 = load i8, ptr %t_v533___ast_240_box
  %r477 = icmp ne i64 %r475, 0
  %r478 = zext i1 %r477 to i64
  %r479 = icmp ne i64 %r478, 0
  br i1 %r479, label %ast_block_51, label %ast_block_52
ast_block_51:
  %r480 = load i64, ptr %s_v533___ast_240_box
  %r481 = load i8, ptr %t_v533___ast_240_box
  call void @chali_owned_drop(i64 %r480)
  br label %ast_block_53
ast_block_52:
  br label %ast_block_53
ast_block_53:
  store i64 0, ptr %s_v533___ast_240_box
  store i8 0, ptr %t_v533___ast_240_box
  %r482 = load i64, ptr %s_v541___ast_240_writer
  %r483 = load i8, ptr %t_v541___ast_240_writer
  %r484 = icmp ne i64 %r482, 0
  %r485 = zext i1 %r484 to i64
  %r486 = icmp ne i64 %r485, 0
  br i1 %r486, label %ast_block_54, label %ast_block_55
ast_block_54:
  %r487 = load i64, ptr %s_v541___ast_240_writer
  %r488 = load i8, ptr %t_v541___ast_240_writer
  call void @chali_owned_drop(i64 %r487)
  br label %ast_block_56
ast_block_55:
  br label %ast_block_56
ast_block_56:
  store i64 0, ptr %s_v541___ast_240_writer
  store i8 0, ptr %t_v541___ast_240_writer
  %r489 = load i64, ptr %s_v531___ast_240___chali_tmp_o_1325
  %r490 = load i8, ptr %t_v531___ast_240___chali_tmp_o_1325
  %r491 = icmp ne i64 %r489, 0
  %r492 = zext i1 %r491 to i64
  %r493 = icmp ne i64 %r492, 0
  br i1 %r493, label %ast_block_57, label %ast_block_58
ast_block_57:
  %r494 = load i64, ptr %s_v531___ast_240___chali_tmp_o_1325
  %r495 = load i8, ptr %t_v531___ast_240___chali_tmp_o_1325
  call void @chali_owned_drop(i64 %r494)
  br label %ast_block_59
ast_block_58:
  br label %ast_block_59
ast_block_59:
  store i64 0, ptr %s_v531___ast_240___chali_tmp_o_1325
  store i8 0, ptr %t_v531___ast_240___chali_tmp_o_1325
  %r496 = load i64, ptr %s_v538___ast_240_saved
  %r497 = load i8, ptr %t_v538___ast_240_saved
  %r498 = icmp ne i64 %r496, 0
  %r499 = zext i1 %r498 to i64
  %r500 = icmp ne i64 %r499, 0
  br i1 %r500, label %ast_block_60, label %ast_block_61
ast_block_60:
  %r501 = load i64, ptr %s_v538___ast_240_saved
  %r502 = load i8, ptr %t_v538___ast_240_saved
  call void @chali_owned_drop(i64 %r501)
  br label %ast_block_62
ast_block_61:
  br label %ast_block_62
ast_block_62:
  store i64 0, ptr %s_v538___ast_240_saved
  store i8 0, ptr %t_v538___ast_240_saved
  %r503 = load i64, ptr %s_v540___ast_240_view_1321
  %r504 = load i8, ptr %t_v540___ast_240_view_1321
  %r505 = icmp ne i64 %r503, 0
  %r506 = zext i1 %r505 to i64
  %r507 = icmp ne i64 %r506, 0
  br i1 %r507, label %ast_block_63, label %ast_block_64
ast_block_63:
  %r508 = load i64, ptr %s_v540___ast_240_view_1321
  %r509 = load i8, ptr %t_v540___ast_240_view_1321
  call void @chali_owned_drop(i64 %r508)
  br label %ast_block_65
ast_block_64:
  br label %ast_block_65
ast_block_65:
  store i64 0, ptr %s_v540___ast_240_view_1321
  store i8 0, ptr %t_v540___ast_240_view_1321
  %r510 = load i64, ptr %s_v530___ast_240___chali_tmp_o_1323
  %r511 = load i8, ptr %t_v530___ast_240___chali_tmp_o_1323
  %r512 = icmp ne i64 %r510, 0
  %r513 = zext i1 %r512 to i64
  %r514 = icmp ne i64 %r513, 0
  br i1 %r514, label %ast_block_66, label %ast_block_67
ast_block_66:
  %r515 = load i64, ptr %s_v530___ast_240___chali_tmp_o_1323
  %r516 = load i8, ptr %t_v530___ast_240___chali_tmp_o_1323
  call void @chali_owned_drop(i64 %r515)
  br label %ast_block_68
ast_block_67:
  br label %ast_block_68
ast_block_68:
  store i64 0, ptr %s_v530___ast_240___chali_tmp_o_1323
  store i8 0, ptr %t_v530___ast_240___chali_tmp_o_1323
  %r517 = load i64, ptr %s_v539___ast_240_second_1318
  %r518 = load i8, ptr %t_v539___ast_240_second_1318
  %r519 = icmp ne i64 %r517, 0
  %r520 = zext i1 %r519 to i64
  %r521 = icmp ne i64 %r520, 0
  br i1 %r521, label %ast_block_69, label %ast_block_70
ast_block_69:
  %r522 = load i64, ptr %s_v539___ast_240_second_1318
  %r523 = load i8, ptr %t_v539___ast_240_second_1318
  call void @chali_owned_drop(i64 %r522)
  br label %ast_block_71
ast_block_70:
  br label %ast_block_71
ast_block_71:
  store i64 0, ptr %s_v539___ast_240_second_1318
  store i8 0, ptr %t_v539___ast_240_second_1318
  %r524 = load i64, ptr %s_v536___ast_240_first_1315
  %r525 = load i8, ptr %t_v536___ast_240_first_1315
  %r526 = icmp ne i64 %r524, 0
  %r527 = zext i1 %r526 to i64
  %r528 = icmp ne i64 %r527, 0
  br i1 %r528, label %ast_block_72, label %ast_block_73
ast_block_72:
  %r529 = load i64, ptr %s_v536___ast_240_first_1315
  %r530 = load i8, ptr %t_v536___ast_240_first_1315
  call void @chali_owned_drop(i64 %r529)
  br label %ast_block_74
ast_block_73:
  br label %ast_block_74
ast_block_74:
  store i64 0, ptr %s_v536___ast_240_first_1315
  store i8 0, ptr %t_v536___ast_240_first_1315
  %r531 = load i64, ptr @s_g_0_program__global
  %r532 = load i8, ptr @t_g_0_program__global
  %r533 = call i64 @f_v475___chali_agg_rel_Value(i64 %r531)
  %r534 = load i8, ptr @chali_return_float
  store i8 0, ptr @chali_return_float
  ret i64 0
ast_label_v542___chali_cleanup_main:
  %r535 = load i64, ptr %s_v537___ast_240_result_1331
  %r536 = load i8, ptr %t_v537___ast_240_result_1331
  %r537 = icmp ne i64 %r535, 0
  %r538 = zext i1 %r537 to i64
  %r539 = icmp ne i64 %r538, 0
  br i1 %r539, label %ast_block_75, label %ast_block_76
ast_block_75:
  %r540 = load i64, ptr %s_v537___ast_240_result_1331
  %r541 = load i8, ptr %t_v537___ast_240_result_1331
  call void @chali_owned_drop(i64 %r540)
  br label %ast_block_77
ast_block_76:
  br label %ast_block_77
ast_block_77:
  store i64 0, ptr %s_v537___ast_240_result_1331
  store i8 0, ptr %t_v537___ast_240_result_1331
  %r542 = load i64, ptr %s_v535___ast_240_copied
  %r543 = load i8, ptr %t_v535___ast_240_copied
  %r544 = icmp ne i64 %r542, 0
  %r545 = zext i1 %r544 to i64
  %r546 = icmp ne i64 %r545, 0
  br i1 %r546, label %ast_block_78, label %ast_block_79
ast_block_78:
  %r547 = load i64, ptr %s_v535___ast_240_copied
  %r548 = load i8, ptr %t_v535___ast_240_copied
  call void @chali_owned_drop(i64 %r547)
  br label %ast_block_80
ast_block_79:
  br label %ast_block_80
ast_block_80:
  store i64 0, ptr %s_v535___ast_240_copied
  store i8 0, ptr %t_v535___ast_240_copied
  %r549 = load i64, ptr %s_v534___ast_240_boxed
  %r550 = load i8, ptr %t_v534___ast_240_boxed
  %r551 = icmp ne i64 %r549, 0
  %r552 = zext i1 %r551 to i64
  %r553 = icmp ne i64 %r552, 0
  br i1 %r553, label %ast_block_81, label %ast_block_82
ast_block_81:
  %r554 = load i64, ptr %s_v534___ast_240_boxed
  %r555 = load i8, ptr %t_v534___ast_240_boxed
  call void @chali_owned_drop(i64 %r554)
  br label %ast_block_83
ast_block_82:
  br label %ast_block_83
ast_block_83:
  store i64 0, ptr %s_v534___ast_240_boxed
  store i8 0, ptr %t_v534___ast_240_boxed
  %r556 = load i64, ptr %s_v533___ast_240_box
  %r557 = load i8, ptr %t_v533___ast_240_box
  %r558 = icmp ne i64 %r556, 0
  %r559 = zext i1 %r558 to i64
  %r560 = icmp ne i64 %r559, 0
  br i1 %r560, label %ast_block_84, label %ast_block_85
ast_block_84:
  %r561 = load i64, ptr %s_v533___ast_240_box
  %r562 = load i8, ptr %t_v533___ast_240_box
  call void @chali_owned_drop(i64 %r561)
  br label %ast_block_86
ast_block_85:
  br label %ast_block_86
ast_block_86:
  store i64 0, ptr %s_v533___ast_240_box
  store i8 0, ptr %t_v533___ast_240_box
  %r563 = load i64, ptr %s_v541___ast_240_writer
  %r564 = load i8, ptr %t_v541___ast_240_writer
  %r565 = icmp ne i64 %r563, 0
  %r566 = zext i1 %r565 to i64
  %r567 = icmp ne i64 %r566, 0
  br i1 %r567, label %ast_block_87, label %ast_block_88
ast_block_87:
  %r568 = load i64, ptr %s_v541___ast_240_writer
  %r569 = load i8, ptr %t_v541___ast_240_writer
  call void @chali_owned_drop(i64 %r568)
  br label %ast_block_89
ast_block_88:
  br label %ast_block_89
ast_block_89:
  store i64 0, ptr %s_v541___ast_240_writer
  store i8 0, ptr %t_v541___ast_240_writer
  %r570 = load i64, ptr %s_v531___ast_240___chali_tmp_o_1325
  %r571 = load i8, ptr %t_v531___ast_240___chali_tmp_o_1325
  %r572 = icmp ne i64 %r570, 0
  %r573 = zext i1 %r572 to i64
  %r574 = icmp ne i64 %r573, 0
  br i1 %r574, label %ast_block_90, label %ast_block_91
ast_block_90:
  %r575 = load i64, ptr %s_v531___ast_240___chali_tmp_o_1325
  %r576 = load i8, ptr %t_v531___ast_240___chali_tmp_o_1325
  call void @chali_owned_drop(i64 %r575)
  br label %ast_block_92
ast_block_91:
  br label %ast_block_92
ast_block_92:
  store i64 0, ptr %s_v531___ast_240___chali_tmp_o_1325
  store i8 0, ptr %t_v531___ast_240___chali_tmp_o_1325
  %r577 = load i64, ptr %s_v538___ast_240_saved
  %r578 = load i8, ptr %t_v538___ast_240_saved
  %r579 = icmp ne i64 %r577, 0
  %r580 = zext i1 %r579 to i64
  %r581 = icmp ne i64 %r580, 0
  br i1 %r581, label %ast_block_93, label %ast_block_94
ast_block_93:
  %r582 = load i64, ptr %s_v538___ast_240_saved
  %r583 = load i8, ptr %t_v538___ast_240_saved
  call void @chali_owned_drop(i64 %r582)
  br label %ast_block_95
ast_block_94:
  br label %ast_block_95
ast_block_95:
  store i64 0, ptr %s_v538___ast_240_saved
  store i8 0, ptr %t_v538___ast_240_saved
  %r584 = load i64, ptr %s_v540___ast_240_view_1321
  %r585 = load i8, ptr %t_v540___ast_240_view_1321
  %r586 = icmp ne i64 %r584, 0
  %r587 = zext i1 %r586 to i64
  %r588 = icmp ne i64 %r587, 0
  br i1 %r588, label %ast_block_96, label %ast_block_97
ast_block_96:
  %r589 = load i64, ptr %s_v540___ast_240_view_1321
  %r590 = load i8, ptr %t_v540___ast_240_view_1321
  call void @chali_owned_drop(i64 %r589)
  br label %ast_block_98
ast_block_97:
  br label %ast_block_98
ast_block_98:
  store i64 0, ptr %s_v540___ast_240_view_1321
  store i8 0, ptr %t_v540___ast_240_view_1321
  %r591 = load i64, ptr %s_v530___ast_240___chali_tmp_o_1323
  %r592 = load i8, ptr %t_v530___ast_240___chali_tmp_o_1323
  %r593 = icmp ne i64 %r591, 0
  %r594 = zext i1 %r593 to i64
  %r595 = icmp ne i64 %r594, 0
  br i1 %r595, label %ast_block_99, label %ast_block_100
ast_block_99:
  %r596 = load i64, ptr %s_v530___ast_240___chali_tmp_o_1323
  %r597 = load i8, ptr %t_v530___ast_240___chali_tmp_o_1323
  call void @chali_owned_drop(i64 %r596)
  br label %ast_block_101
ast_block_100:
  br label %ast_block_101
ast_block_101:
  store i64 0, ptr %s_v530___ast_240___chali_tmp_o_1323
  store i8 0, ptr %t_v530___ast_240___chali_tmp_o_1323
  %r598 = load i64, ptr %s_v539___ast_240_second_1318
  %r599 = load i8, ptr %t_v539___ast_240_second_1318
  %r600 = call i64 @f_v475___chali_agg_rel_Value(i64 %r598)
  %r601 = load i8, ptr @chali_return_float
  %r602 = load i64, ptr %s_v539___ast_240_second_1318
  %r603 = load i8, ptr %t_v539___ast_240_second_1318
  %r604 = icmp ne i64 %r602, 0
  %r605 = zext i1 %r604 to i64
  %r606 = icmp ne i64 %r605, 0
  br i1 %r606, label %ast_block_102, label %ast_block_103
ast_block_102:
  %r607 = load i64, ptr %s_v539___ast_240_second_1318
  %r608 = load i8, ptr %t_v539___ast_240_second_1318
  call void @chali_owned_drop(i64 %r607)
  br label %ast_block_104
ast_block_103:
  br label %ast_block_104
ast_block_104:
  store i64 0, ptr %s_v539___ast_240_second_1318
  store i8 0, ptr %t_v539___ast_240_second_1318
  %r609 = load i64, ptr %s_v536___ast_240_first_1315
  %r610 = load i8, ptr %t_v536___ast_240_first_1315
  %r611 = call i64 @f_v475___chali_agg_rel_Value(i64 %r609)
  %r612 = load i8, ptr @chali_return_float
  %r613 = load i64, ptr %s_v536___ast_240_first_1315
  %r614 = load i8, ptr %t_v536___ast_240_first_1315
  %r615 = icmp ne i64 %r613, 0
  %r616 = zext i1 %r615 to i64
  %r617 = icmp ne i64 %r616, 0
  br i1 %r617, label %ast_block_105, label %ast_block_106
ast_block_105:
  %r618 = load i64, ptr %s_v536___ast_240_first_1315
  %r619 = load i8, ptr %t_v536___ast_240_first_1315
  call void @chali_owned_drop(i64 %r618)
  br label %ast_block_107
ast_block_106:
  br label %ast_block_107
ast_block_107:
  store i64 0, ptr %s_v536___ast_240_first_1315
  store i8 0, ptr %t_v536___ast_240_first_1315
  %r620 = load i64, ptr @s_g_0_program__global
  %r621 = load i8, ptr @t_g_0_program__global
  %r622 = call i64 @f_v475___chali_agg_rel_Value(i64 %r620)
  %r623 = load i8, ptr @chali_return_float
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define internal i64 @f_v274_std__lang__pointerFromAddress__uint8(i64 %p0) alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [4 x i64]
  %native_argument_types = alloca [4 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v543___ast_274___chali_ret_v_1419 = alloca i64
  store i64 0, ptr %s_v543___ast_274___chali_ret_v_1419
  %t_v543___ast_274___chali_ret_v_1419 = alloca i8
  store i8 0, ptr %t_v543___ast_274___chali_ret_v_1419
  %s_v544___ast_274_address_621 = alloca i64
  store i64 0, ptr %s_v544___ast_274_address_621
  %t_v544___ast_274_address_621 = alloca i8
  store i8 0, ptr %t_v544___ast_274_address_621
  %s_v545___ast_274_result_1418 = alloca i64
  store i64 0, ptr %s_v545___ast_274_result_1418
  %t_v545___ast_274_result_1418 = alloca i8
  store i8 0, ptr %t_v545___ast_274_result_1418
  store i64 %p0, ptr %s_v544___ast_274_address_621
  %r0 = call i64 @chali_alloc(i64 8)
  store i64 %r0, ptr %s_v545___ast_274_result_1418
  store i8 0, ptr %t_v545___ast_274_result_1418
  %r1 = load i64, ptr %s_v545___ast_274_result_1418
  %r2 = load i8, ptr %t_v545___ast_274_result_1418
  call void @chali_host_memory_prepare(i64 8, i64 %r1, i64 0, i64 8, ptr %raw_memory_output, ptr %raw_memory_input)
  %r3 = load ptr, ptr %raw_memory_output
  %r4 = trunc i64 0 to i8
  call void @llvm.memset.p0.i64(ptr %r3, i8 %r4, i64 8, i1 false)
  %r5 = load i64, ptr %s_v545___ast_274_result_1418
  %r6 = load i8, ptr %t_v545___ast_274_result_1418
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
  %r13 = load i64, ptr %s_v544___ast_274_address_621
  %r14 = load i8, ptr %t_v544___ast_274_address_621
  %r15 = load i64, ptr %s_v545___ast_274_result_1418
  %r16 = load i8, ptr %t_v545___ast_274_result_1418
  %r17 = mul i64 0, 8
  %r18 = add i64 %r15, %r17
  %r19 = call ptr @chali_primitive_address(i64 %r18, i64 8)
  store i64 %r13, ptr %r19, align 1
  store i64 336, ptr %s_v543___ast_274___chali_ret_v_1419
  store i8 0, ptr %t_v543___ast_274___chali_ret_v_1419
  %r20 = load i64, ptr %s_v543___ast_274___chali_ret_v_1419
  %r21 = load i8, ptr %t_v543___ast_274___chali_ret_v_1419
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
  %r28 = load i64, ptr %s_v543___ast_274___chali_ret_v_1419
  %r29 = load i8, ptr %t_v543___ast_274___chali_ret_v_1419
  %r30 = load i64, ptr %s_v545___ast_274_result_1418
  %r31 = load i8, ptr %t_v545___ast_274_result_1418
  %r32 = call i64 @chali_host_copy_words(i64 %r28, i64 %r30, i64 1)
  %r33 = load i64, ptr %s_v543___ast_274___chali_ret_v_1419
  %r34 = load i8, ptr %t_v543___ast_274___chali_ret_v_1419
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
  %r41 = load i64, ptr %s_v545___ast_274_result_1418
  %r42 = load i8, ptr %t_v545___ast_274_result_1418
  %r43 = icmp ne i64 %r41, 0
  %r44 = zext i1 %r43 to i64
  %r45 = icmp ne i64 %r44, 0
  br i1 %r45, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r46 = load i64, ptr %s_v545___ast_274_result_1418
  %r47 = load i8, ptr %t_v545___ast_274_result_1418
  call void @chali_owned_drop(i64 %r46)
  br label %ast_block_2
ast_block_1:
  br label %ast_block_2
ast_block_2:
  store i64 0, ptr %s_v545___ast_274_result_1418
  store i8 0, ptr %t_v545___ast_274_result_1418
  %r48 = load i64, ptr %s_v543___ast_274___chali_ret_v_1419
  %r49 = load i8, ptr %t_v543___ast_274___chali_ret_v_1419
  store i8 %r49, ptr @chali_return_float
  ret i64 %r48
ast_block_3:
  %r50 = load i64, ptr %s_v545___ast_274_result_1418
  %r51 = load i8, ptr %t_v545___ast_274_result_1418
  %r52 = icmp ne i64 %r50, 0
  %r53 = zext i1 %r52 to i64
  %r54 = icmp ne i64 %r53, 0
  br i1 %r54, label %ast_block_4, label %ast_block_5
ast_block_4:
  %r55 = load i64, ptr %s_v545___ast_274_result_1418
  %r56 = load i8, ptr %t_v545___ast_274_result_1418
  call void @chali_owned_drop(i64 %r55)
  br label %ast_block_6
ast_block_5:
  br label %ast_block_6
ast_block_6:
  store i64 0, ptr %s_v545___ast_274_result_1418
  store i8 0, ptr %t_v545___ast_274_result_1418
  store i8 0, ptr @chali_return_float
  ret i64 0
ast_label_v546___chali_cleanup_std__lang__pointerFromAddress__uint8:
  %r57 = load i64, ptr %s_v545___ast_274_result_1418
  %r58 = load i8, ptr %t_v545___ast_274_result_1418
  %r59 = icmp ne i64 %r57, 0
  %r60 = zext i1 %r59 to i64
  %r61 = icmp ne i64 %r60, 0
  br i1 %r61, label %ast_block_7, label %ast_block_8
ast_block_7:
  %r62 = load i64, ptr %s_v545___ast_274_result_1418
  %r63 = load i8, ptr %t_v545___ast_274_result_1418
  call void @chali_owned_drop(i64 %r62)
  br label %ast_block_9
ast_block_8:
  br label %ast_block_9
ast_block_9:
  store i64 0, ptr %s_v545___ast_274_result_1418
  store i8 0, ptr %t_v545___ast_274_result_1418
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define internal i64 @f_v282_std__lang__pointerFromAddress__int64(i64 %p0) alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [4 x i64]
  %native_argument_types = alloca [4 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v547___ast_282___chali_ret_v_1436 = alloca i64
  store i64 0, ptr %s_v547___ast_282___chali_ret_v_1436
  %t_v547___ast_282___chali_ret_v_1436 = alloca i8
  store i8 0, ptr %t_v547___ast_282___chali_ret_v_1436
  %s_v548___ast_282_address_624 = alloca i64
  store i64 0, ptr %s_v548___ast_282_address_624
  %t_v548___ast_282_address_624 = alloca i8
  store i8 0, ptr %t_v548___ast_282_address_624
  %s_v549___ast_282_result_1435 = alloca i64
  store i64 0, ptr %s_v549___ast_282_result_1435
  %t_v549___ast_282_result_1435 = alloca i8
  store i8 0, ptr %t_v549___ast_282_result_1435
  store i64 %p0, ptr %s_v548___ast_282_address_624
  %r0 = call i64 @chali_alloc(i64 8)
  store i64 %r0, ptr %s_v549___ast_282_result_1435
  store i8 0, ptr %t_v549___ast_282_result_1435
  %r1 = load i64, ptr %s_v549___ast_282_result_1435
  %r2 = load i8, ptr %t_v549___ast_282_result_1435
  call void @chali_host_memory_prepare(i64 8, i64 %r1, i64 0, i64 8, ptr %raw_memory_output, ptr %raw_memory_input)
  %r3 = load ptr, ptr %raw_memory_output
  %r4 = trunc i64 0 to i8
  call void @llvm.memset.p0.i64(ptr %r3, i8 %r4, i64 8, i1 false)
  %r5 = load i64, ptr %s_v549___ast_282_result_1435
  %r6 = load i8, ptr %t_v549___ast_282_result_1435
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
  %r13 = load i64, ptr %s_v548___ast_282_address_624
  %r14 = load i8, ptr %t_v548___ast_282_address_624
  %r15 = load i64, ptr %s_v549___ast_282_result_1435
  %r16 = load i8, ptr %t_v549___ast_282_result_1435
  %r17 = mul i64 0, 8
  %r18 = add i64 %r15, %r17
  %r19 = call ptr @chali_primitive_address(i64 %r18, i64 8)
  store i64 %r13, ptr %r19, align 1
  store i64 328, ptr %s_v547___ast_282___chali_ret_v_1436
  store i8 0, ptr %t_v547___ast_282___chali_ret_v_1436
  %r20 = load i64, ptr %s_v547___ast_282___chali_ret_v_1436
  %r21 = load i8, ptr %t_v547___ast_282___chali_ret_v_1436
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
  %r28 = load i64, ptr %s_v547___ast_282___chali_ret_v_1436
  %r29 = load i8, ptr %t_v547___ast_282___chali_ret_v_1436
  %r30 = load i64, ptr %s_v549___ast_282_result_1435
  %r31 = load i8, ptr %t_v549___ast_282_result_1435
  %r32 = call i64 @chali_host_copy_words(i64 %r28, i64 %r30, i64 1)
  %r33 = load i64, ptr %s_v547___ast_282___chali_ret_v_1436
  %r34 = load i8, ptr %t_v547___ast_282___chali_ret_v_1436
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
  %r41 = load i64, ptr %s_v549___ast_282_result_1435
  %r42 = load i8, ptr %t_v549___ast_282_result_1435
  %r43 = icmp ne i64 %r41, 0
  %r44 = zext i1 %r43 to i64
  %r45 = icmp ne i64 %r44, 0
  br i1 %r45, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r46 = load i64, ptr %s_v549___ast_282_result_1435
  %r47 = load i8, ptr %t_v549___ast_282_result_1435
  call void @chali_owned_drop(i64 %r46)
  br label %ast_block_2
ast_block_1:
  br label %ast_block_2
ast_block_2:
  store i64 0, ptr %s_v549___ast_282_result_1435
  store i8 0, ptr %t_v549___ast_282_result_1435
  %r48 = load i64, ptr %s_v547___ast_282___chali_ret_v_1436
  %r49 = load i8, ptr %t_v547___ast_282___chali_ret_v_1436
  store i8 %r49, ptr @chali_return_float
  ret i64 %r48
ast_block_3:
  %r50 = load i64, ptr %s_v549___ast_282_result_1435
  %r51 = load i8, ptr %t_v549___ast_282_result_1435
  %r52 = icmp ne i64 %r50, 0
  %r53 = zext i1 %r52 to i64
  %r54 = icmp ne i64 %r53, 0
  br i1 %r54, label %ast_block_4, label %ast_block_5
ast_block_4:
  %r55 = load i64, ptr %s_v549___ast_282_result_1435
  %r56 = load i8, ptr %t_v549___ast_282_result_1435
  call void @chali_owned_drop(i64 %r55)
  br label %ast_block_6
ast_block_5:
  br label %ast_block_6
ast_block_6:
  store i64 0, ptr %s_v549___ast_282_result_1435
  store i8 0, ptr %t_v549___ast_282_result_1435
  store i8 0, ptr @chali_return_float
  ret i64 0
ast_label_v550___chali_cleanup_std__lang__pointerFromAddress__int64:
  %r57 = load i64, ptr %s_v549___ast_282_result_1435
  %r58 = load i8, ptr %t_v549___ast_282_result_1435
  %r59 = icmp ne i64 %r57, 0
  %r60 = zext i1 %r59 to i64
  %r61 = icmp ne i64 %r60, 0
  br i1 %r61, label %ast_block_7, label %ast_block_8
ast_block_7:
  %r62 = load i64, ptr %s_v549___ast_282_result_1435
  %r63 = load i8, ptr %t_v549___ast_282_result_1435
  call void @chali_owned_drop(i64 %r62)
  br label %ast_block_9
ast_block_8:
  br label %ast_block_9
ast_block_9:
  store i64 0, ptr %s_v549___ast_282_result_1435
  store i8 0, ptr %t_v549___ast_282_result_1435
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define internal i64 @f_v469___chali_iface_NamedReader_extra(i64 %p0, i64 %p1) uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [3 x i64]
  %native_argument_types = alloca [3 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v551___ast_469___chali_readonly = alloca i64
  store i64 0, ptr %s_v551___ast_469___chali_readonly
  %t_v551___ast_469___chali_readonly = alloca i8
  store i8 0, ptr %t_v551___ast_469___chali_readonly
  %s_v552___ast_469___chali_self = alloca i64
  store i64 0, ptr %s_v552___ast_469___chali_self
  %t_v552___ast_469___chali_self = alloca i8
  store i8 0, ptr %t_v552___ast_469___chali_self
  %s_v553___ast_469___chali_t = alloca i64
  store i64 0, ptr %s_v553___ast_469___chali_t
  %t_v553___ast_469___chali_t = alloca i8
  store i8 0, ptr %t_v553___ast_469___chali_t
  store i64 %p0, ptr %s_v552___ast_469___chali_self
  store i64 %p1, ptr %s_v551___ast_469___chali_readonly
  %r0 = load i64, ptr %s_v552___ast_469___chali_self
  %r1 = load i8, ptr %t_v552___ast_469___chali_self
  %r2 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r0, ptr %r2
  %r3 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r1, ptr %r3
  %r4 = call i64 @chali_host_call(ptr @native_name9, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r5 = call i8 @chali_host_result_float()
  store i64 %r4, ptr %s_v553___ast_469___chali_t
  store i8 %r5, ptr %t_v553___ast_469___chali_t
  %r6 = load i64, ptr %s_v553___ast_469___chali_t
  %r7 = load i8, ptr %t_v553___ast_469___chali_t
  %r8 = icmp eq i64 %r6, 123
  %r9 = zext i1 %r8 to i64
  %r10 = icmp ne i64 %r9, 0
  br i1 %r10, label %ast_block_0, label %ast_block_1
ast_block_0:
  br label %ast_block_2
ast_block_1:
  %r11 = load i64, ptr %s_v553___ast_469___chali_t
  %r12 = load i8, ptr %t_v553___ast_469___chali_t
  %r13 = icmp eq i64 %r11, 122
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
  %r19 = load i64, ptr %s_v552___ast_469___chali_self
  %r20 = load i8, ptr %t_v552___ast_469___chali_self
  %r21 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r19, ptr %r21
  %r22 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r20, ptr %r22
  %r23 = call i64 @chali_host_call(ptr @native_name6, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r24 = call i8 @chali_host_result_float()
  %r25 = call i64 @f_v235_Value_extra(i64 %r23)
  %r26 = load i8, ptr @chali_return_float
  store i8 %r26, ptr @chali_return_float
  ret i64 %r25
ast_block_5:
  br label %ast_block_6
ast_block_6:
  %r27 = load i64, ptr %s_v553___ast_469___chali_t
  %r28 = load i8, ptr %t_v553___ast_469___chali_t
  %r29 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r27, ptr %r29
  %r30 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r28, ptr %r30
  %r31 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 8, ptr %r31
  %r32 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r32
  %r33 = call i64 @chali_host_call(ptr @native_name10, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r34 = call i8 @chali_host_result_float()
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define internal i64 @f_v470___chali_iface_NamedReader_read(i64 %p0, i64 %p1) uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [3 x i64]
  %native_argument_types = alloca [3 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v554___ast_470___chali_readonly = alloca i64
  store i64 0, ptr %s_v554___ast_470___chali_readonly
  %t_v554___ast_470___chali_readonly = alloca i8
  store i8 0, ptr %t_v554___ast_470___chali_readonly
  %s_v555___ast_470___chali_self = alloca i64
  store i64 0, ptr %s_v555___ast_470___chali_self
  %t_v555___ast_470___chali_self = alloca i8
  store i8 0, ptr %t_v555___ast_470___chali_self
  %s_v556___ast_470___chali_t = alloca i64
  store i64 0, ptr %s_v556___ast_470___chali_t
  %t_v556___ast_470___chali_t = alloca i8
  store i8 0, ptr %t_v556___ast_470___chali_t
  store i64 %p0, ptr %s_v555___ast_470___chali_self
  store i64 %p1, ptr %s_v554___ast_470___chali_readonly
  %r0 = load i64, ptr %s_v555___ast_470___chali_self
  %r1 = load i8, ptr %t_v555___ast_470___chali_self
  %r2 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r0, ptr %r2
  %r3 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r1, ptr %r3
  %r4 = call i64 @chali_host_call(ptr @native_name9, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r5 = call i8 @chali_host_result_float()
  store i64 %r4, ptr %s_v556___ast_470___chali_t
  store i8 %r5, ptr %t_v556___ast_470___chali_t
  %r6 = load i64, ptr %s_v556___ast_470___chali_t
  %r7 = load i8, ptr %t_v556___ast_470___chali_t
  %r8 = icmp eq i64 %r6, 123
  %r9 = zext i1 %r8 to i64
  %r10 = icmp ne i64 %r9, 0
  br i1 %r10, label %ast_block_0, label %ast_block_1
ast_block_0:
  br label %ast_block_2
ast_block_1:
  %r11 = load i64, ptr %s_v556___ast_470___chali_t
  %r12 = load i8, ptr %t_v556___ast_470___chali_t
  %r13 = icmp eq i64 %r11, 122
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
  %r19 = load i64, ptr %s_v555___ast_470___chali_self
  %r20 = load i8, ptr %t_v555___ast_470___chali_self
  %r21 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r19, ptr %r21
  %r22 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r20, ptr %r22
  %r23 = call i64 @chali_host_call(ptr @native_name6, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r24 = call i8 @chali_host_result_float()
  %r25 = call i64 @f_v234_Value_read(i64 %r23)
  %r26 = load i8, ptr @chali_return_float
  store i8 %r26, ptr @chali_return_float
  ret i64 %r25
ast_block_5:
  br label %ast_block_6
ast_block_6:
  %r27 = load i64, ptr %s_v556___ast_470___chali_t
  %r28 = load i8, ptr %t_v556___ast_470___chali_t
  %r29 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r27, ptr %r29
  %r30 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r28, ptr %r30
  %r31 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 32, ptr %r31
  %r32 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r32
  %r33 = call i64 @chali_host_call(ptr @native_name10, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r34 = call i8 @chali_host_result_float()
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define internal i64 @f_v471___chali_iface_Reader_read(i64 %p0, i64 %p1) uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [3 x i64]
  %native_argument_types = alloca [3 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v557___ast_471___chali_readonly = alloca i64
  store i64 0, ptr %s_v557___ast_471___chali_readonly
  %t_v557___ast_471___chali_readonly = alloca i8
  store i8 0, ptr %t_v557___ast_471___chali_readonly
  %s_v558___ast_471___chali_self = alloca i64
  store i64 0, ptr %s_v558___ast_471___chali_self
  %t_v558___ast_471___chali_self = alloca i8
  store i8 0, ptr %t_v558___ast_471___chali_self
  %s_v559___ast_471___chali_t = alloca i64
  store i64 0, ptr %s_v559___ast_471___chali_t
  %t_v559___ast_471___chali_t = alloca i8
  store i8 0, ptr %t_v559___ast_471___chali_t
  store i64 %p0, ptr %s_v558___ast_471___chali_self
  store i64 %p1, ptr %s_v557___ast_471___chali_readonly
  %r0 = load i64, ptr %s_v558___ast_471___chali_self
  %r1 = load i8, ptr %t_v558___ast_471___chali_self
  %r2 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r0, ptr %r2
  %r3 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r1, ptr %r3
  %r4 = call i64 @chali_host_call(ptr @native_name9, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r5 = call i8 @chali_host_result_float()
  store i64 %r4, ptr %s_v559___ast_471___chali_t
  store i8 %r5, ptr %t_v559___ast_471___chali_t
  %r6 = load i64, ptr %s_v559___ast_471___chali_t
  %r7 = load i8, ptr %t_v559___ast_471___chali_t
  %r8 = icmp eq i64 %r6, 123
  %r9 = zext i1 %r8 to i64
  %r10 = icmp ne i64 %r9, 0
  br i1 %r10, label %ast_block_0, label %ast_block_1
ast_block_0:
  br label %ast_block_2
ast_block_1:
  %r11 = load i64, ptr %s_v559___ast_471___chali_t
  %r12 = load i8, ptr %t_v559___ast_471___chali_t
  %r13 = icmp eq i64 %r11, 122
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
  %r19 = load i64, ptr %s_v558___ast_471___chali_self
  %r20 = load i8, ptr %t_v558___ast_471___chali_self
  %r21 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r19, ptr %r21
  %r22 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r20, ptr %r22
  %r23 = call i64 @chali_host_call(ptr @native_name6, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r24 = call i8 @chali_host_result_float()
  %r25 = call i64 @f_v234_Value_read(i64 %r23)
  %r26 = load i8, ptr @chali_return_float
  store i8 %r26, ptr @chali_return_float
  ret i64 %r25
ast_block_5:
  br label %ast_block_6
ast_block_6:
  %r27 = load i64, ptr %s_v559___ast_471___chali_t
  %r28 = load i8, ptr %t_v559___ast_471___chali_t
  %r29 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r27, ptr %r29
  %r30 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r28, ptr %r30
  %r31 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 56, ptr %r31
  %r32 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r32
  %r33 = call i64 @chali_host_call(ptr @native_name10, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r34 = call i8 @chali_host_result_float()
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define internal i64 @f_v472___chali_iface_Writer_write(i64 %p0, i64 %p1, i64 %p2) uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [3 x i64]
  %native_argument_types = alloca [3 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v560___ast_472___chali_a0 = alloca i64
  store i64 0, ptr %s_v560___ast_472___chali_a0
  %t_v560___ast_472___chali_a0 = alloca i8
  store i8 0, ptr %t_v560___ast_472___chali_a0
  %s_v561___ast_472___chali_readonly = alloca i64
  store i64 0, ptr %s_v561___ast_472___chali_readonly
  %t_v561___ast_472___chali_readonly = alloca i8
  store i8 0, ptr %t_v561___ast_472___chali_readonly
  %s_v562___ast_472___chali_self = alloca i64
  store i64 0, ptr %s_v562___ast_472___chali_self
  %t_v562___ast_472___chali_self = alloca i8
  store i8 0, ptr %t_v562___ast_472___chali_self
  %s_v563___ast_472___chali_t = alloca i64
  store i64 0, ptr %s_v563___ast_472___chali_t
  %t_v563___ast_472___chali_t = alloca i8
  store i8 0, ptr %t_v563___ast_472___chali_t
  store i64 %p0, ptr %s_v562___ast_472___chali_self
  store i64 %p1, ptr %s_v561___ast_472___chali_readonly
  store i64 %p2, ptr %s_v560___ast_472___chali_a0
  %r0 = load i64, ptr %s_v562___ast_472___chali_self
  %r1 = load i8, ptr %t_v562___ast_472___chali_self
  %r2 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r0, ptr %r2
  %r3 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r1, ptr %r3
  %r4 = call i64 @chali_host_call(ptr @native_name9, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r5 = call i8 @chali_host_result_float()
  store i64 %r4, ptr %s_v563___ast_472___chali_t
  store i8 %r5, ptr %t_v563___ast_472___chali_t
  %r6 = load i64, ptr %s_v563___ast_472___chali_t
  %r7 = load i8, ptr %t_v563___ast_472___chali_t
  %r8 = icmp eq i64 %r6, 123
  %r9 = zext i1 %r8 to i64
  %r10 = icmp ne i64 %r9, 0
  br i1 %r10, label %ast_block_0, label %ast_block_1
ast_block_0:
  br label %ast_block_2
ast_block_1:
  %r11 = load i64, ptr %s_v563___ast_472___chali_t
  %r12 = load i8, ptr %t_v563___ast_472___chali_t
  %r13 = icmp eq i64 %r11, 122
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
  %r19 = load i64, ptr %s_v561___ast_472___chali_readonly
  %r20 = load i8, ptr %t_v561___ast_472___chali_readonly
  %r21 = icmp ne i64 %r19, 0
  br i1 %r21, label %ast_block_7, label %ast_block_8
ast_block_7:
  %r22 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 72, ptr %r22
  %r23 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 0, ptr %r23
  %r24 = call i64 @chali_host_call(ptr @native_name0, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r25 = call i8 @chali_host_result_float()
  br label %ast_block_9
ast_block_8:
  br label %ast_block_9
ast_block_9:
  %r26 = load i64, ptr %s_v562___ast_472___chali_self
  %r27 = load i8, ptr %t_v562___ast_472___chali_self
  %r28 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r26, ptr %r28
  %r29 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r27, ptr %r29
  %r30 = call i64 @chali_host_call(ptr @native_name6, i64 1, ptr %indirect_arguments, ptr %native_argument_types)
  %r31 = call i8 @chali_host_result_float()
  %r32 = load i64, ptr %s_v560___ast_472___chali_a0
  %r33 = load i8, ptr %t_v560___ast_472___chali_a0
  %r34 = call i64 @f_v236_Value_write(i64 %r30, i64 %r32)
  %r35 = load i8, ptr @chali_return_float
  store i8 %r35, ptr @chali_return_float
  ret i64 %r34
ast_block_5:
  br label %ast_block_6
ast_block_6:
  %r36 = load i64, ptr %s_v563___ast_472___chali_t
  %r37 = load i8, ptr %t_v563___ast_472___chali_t
  %r38 = getelementptr i64, ptr %indirect_arguments, i64 0
  store i64 %r36, ptr %r38
  %r39 = getelementptr i8, ptr %native_argument_types, i64 0
  store i8 %r37, ptr %r39
  %r40 = getelementptr i64, ptr %indirect_arguments, i64 1
  store i64 128, ptr %r40
  %r41 = getelementptr i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r41
  %r42 = call i64 @chali_host_call(ptr @native_name10, i64 2, ptr %indirect_arguments, ptr %native_argument_types)
  %r43 = call i8 @chali_host_result_float()
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define internal i64 @f_v475___chali_agg_rel_Value(i64 %p0) alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [2 x i64]
  %native_argument_types = alloca [2 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v564___ast_475___chali_agg = alloca i64
  store i64 0, ptr %s_v564___ast_475___chali_agg
  %t_v564___ast_475___chali_agg = alloca i8
  store i8 0, ptr %t_v564___ast_475___chali_agg
  store i64 %p0, ptr %s_v564___ast_475___chali_agg
  %r0 = load i64, ptr %s_v564___ast_475___chali_agg
  %r1 = load i8, ptr %t_v564___ast_475___chali_agg
  %r2 = mul i64 1, 8
  %r3 = add i64 %r0, %r2
  %r4 = call ptr @chali_primitive_address(i64 %r3, i64 8)
  %r5 = load i64, ptr %r4, align 1
  %r6 = icmp ne i64 %r5, 0
  %r7 = zext i1 %r6 to i64
  %r8 = icmp ne i64 %r7, 0
  br i1 %r8, label %ast_block_0, label %ast_block_1
ast_block_0:
  %r9 = load i64, ptr %s_v564___ast_475___chali_agg
  %r10 = load i8, ptr %t_v564___ast_475___chali_agg
  %r11 = mul i64 1, 8
  %r12 = add i64 %r9, %r11
  %r13 = call ptr @chali_primitive_address(i64 %r12, i64 8)
  %r14 = load i64, ptr %r13, align 1
  %r15 = call i64 @f_v61_std__lang____bootStringDrop(i64 %r14)
  %r16 = load i8, ptr @chali_return_float
  br label %ast_block_2
ast_block_1:
  br label %ast_block_2
ast_block_2:
  %r17 = load i64, ptr %s_v564___ast_475___chali_agg
  %r18 = load i8, ptr %t_v564___ast_475___chali_agg
  %r19 = mul i64 1, 8
  %r20 = add i64 %r17, %r19
  %r21 = call ptr @chali_primitive_address(i64 %r20, i64 8)
  store i64 0, ptr %r21, align 1
  store i8 0, ptr @chali_return_float
  ret i64 0
}
define internal i64 @f_v477___chali_copy_struct(i64 %p0, i64 %p1, i64 %p2) alwaysinline uwtable section "chali_code" {
entry:
  %indirect_arguments = alloca [1 x i64]
  %native_argument_types = alloca [1 x i8]
  %raw_memory_output = alloca ptr
  %raw_memory_input = alloca ptr
  %collection_result_kind = alloca i8
  %s_v565___ast_477___chali_dst = alloca i64
  store i64 0, ptr %s_v565___ast_477___chali_dst
  %t_v565___ast_477___chali_dst = alloca i8
  store i8 0, ptr %t_v565___ast_477___chali_dst
  %s_v566___ast_477___chali_k = alloca i64
  store i64 0, ptr %s_v566___ast_477___chali_k
  %t_v566___ast_477___chali_k = alloca i8
  store i8 0, ptr %t_v566___ast_477___chali_k
  %s_v567___ast_477___chali_slots = alloca i64
  store i64 0, ptr %s_v567___ast_477___chali_slots
  %t_v567___ast_477___chali_slots = alloca i8
  store i8 0, ptr %t_v567___ast_477___chali_slots
  %s_v568___ast_477___chali_src = alloca i64
  store i64 0, ptr %s_v568___ast_477___chali_src
  %t_v568___ast_477___chali_src = alloca i8
  store i8 0, ptr %t_v568___ast_477___chali_src
  store i64 %p0, ptr %s_v565___ast_477___chali_dst
  store i64 %p1, ptr %s_v568___ast_477___chali_src
  store i64 %p2, ptr %s_v567___ast_477___chali_slots
  store i64 0, ptr %s_v566___ast_477___chali_k
  store i8 0, ptr %t_v566___ast_477___chali_k
  br label %ast_block_0
ast_block_0:
  %r0 = load i64, ptr %s_v566___ast_477___chali_k
  %r1 = load i8, ptr %t_v566___ast_477___chali_k
  %r2 = load i64, ptr %s_v567___ast_477___chali_slots
  %r3 = load i8, ptr %t_v567___ast_477___chali_slots
  %r4 = icmp slt i64 %r0, %r2
  %r5 = zext i1 %r4 to i64
  %r6 = icmp ne i64 %r5, 0
  br i1 %r6, label %ast_block_1, label %ast_block_2
ast_block_1:
  %r7 = load i64, ptr %s_v568___ast_477___chali_src
  %r8 = load i8, ptr %t_v568___ast_477___chali_src
  %r9 = load i64, ptr %s_v566___ast_477___chali_k
  %r10 = load i8, ptr %t_v566___ast_477___chali_k
  %r11 = mul i64 %r9, 8
  %r12 = add i64 %r7, %r11
  %r13 = call ptr @chali_primitive_address(i64 %r12, i64 8)
  %r14 = load i64, ptr %r13, align 1
  %r15 = load i64, ptr %s_v565___ast_477___chali_dst
  %r16 = load i8, ptr %t_v565___ast_477___chali_dst
  %r17 = load i64, ptr %s_v566___ast_477___chali_k
  %r18 = load i8, ptr %t_v566___ast_477___chali_k
  %r19 = mul i64 %r17, 8
  %r20 = add i64 %r15, %r19
  %r21 = call ptr @chali_primitive_address(i64 %r20, i64 8)
  store i64 %r14, ptr %r21, align 1
  %r22 = load i64, ptr %s_v566___ast_477___chali_k
  %r23 = load i8, ptr %t_v566___ast_477___chali_k
  %r24 = add i64 %r22, 1
  %r25 = trunc i64 %r24 to i32
  %r26 = sext i32 %r25 to i64
  store i64 %r26, ptr %s_v566___ast_477___chali_k
  store i8 0, ptr %t_v566___ast_477___chali_k
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
