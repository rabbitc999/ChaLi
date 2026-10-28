; ModuleID = 'source/aot/runtime.c'
source_filename = "source/aot/runtime.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@stdin = external local_unnamed_addr global ptr, align 8
@stdout = external local_unnamed_addr global ptr, align 8
@stderr = external local_unnamed_addr global ptr, align 8
@.str = private unnamed_addr constant [15 x i8] c"chali aot: %s\0A\00", align 1
@.str.1 = private unnamed_addr constant [55 x i8] c"\E7\A7\BB\E4\BD\8D\E4\BD\8D\E6\95\B0\E8\B6\85\E5\87\BA\E6\95\B4\E6\95\B0\E4\BD\8D\E5\AE\BD\EF\BC\88\E6\9C\89\E6\95\88\E8\8C\83\E5\9B\B4 0..63\EF\BC\89\00", align 1
@.str.2 = private unnamed_addr constant [13 x i8] c"\E6\B5\AE\E7\82\B9\E9\99\A4\E9\9B\B6\00", align 1
@.str.3 = private unnamed_addr constant [13 x i8] c"\E6\95\B4\E6\95\B0\E9\99\A4\E9\9B\B6\00", align 1
@.str.4 = private unnamed_addr constant [72 x i8] c"\E6\95\B4\E6\95\B0\E9\99\A4\E6\B3\95\E6\BA\A2\E5\87\BA\EF\BC\9A\E6\9C\80\E5\B0\8F\E8\B4\9F\E6\95\B0\E9\99\A4\E4\BB\A5 -1 \E7\9A\84\E7\BB\93\E6\9E\9C\E8\B6\85\E5\87\BA int64 \E8\8C\83\E5\9B\B4\00", align 1
@.str.5 = private unnamed_addr constant [31 x i8] c"\E5\A4\8D\E5\88\B6\E5\AD\97\E7\AC\A6\E4\B8\B2\E6\97\B6\E5\86\85\E5\AD\98\E4\B8\8D\E8\B6\B3\00", align 1
@chali_data_size = external local_unnamed_addr constant i64, align 8
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

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(read, argmem: none, inaccessiblemem: none, target_mem: none) uwtable
define dso_local ptr @chali_standard_stream(i32 noundef %0) local_unnamed_addr #0 {
  %2 = icmp eq i32 %0, 0
  br i1 %2, label %3, label %5

3:                                                ; preds = %1
  %4 = load ptr, ptr @stdin, align 8, !tbaa !9
  br label %10

5:                                                ; preds = %1
  %6 = icmp eq i32 %0, 1
  %7 = load ptr, ptr @stdout, align 8
  %8 = load ptr, ptr @stderr, align 8
  %9 = select i1 %6, ptr %7, ptr %8
  br label %10

10:                                               ; preds = %5, %3
  %11 = phi ptr [ %4, %3 ], [ %9, %5 ]
  ret ptr %11
}

; Function Attrs: cold noreturn nounwind uwtable
define dso_local void @chali_panic(ptr noundef %0) local_unnamed_addr #1 {
  %2 = load ptr, ptr @stdout, align 8, !tbaa !9
  %3 = tail call i32 @fflush(ptr noundef %2)
  %4 = load ptr, ptr @stderr, align 8, !tbaa !9
  %5 = tail call i32 (ptr, ptr, ...) @fprintf(ptr noundef %4, ptr noundef nonnull @.str, ptr noundef %0) #19
  tail call void @chali_host_trace() #20
  tail call void @exit(i32 noundef 1) #21
  unreachable
}

; Function Attrs: nofree nounwind
declare noundef i32 @fflush(ptr noundef captures(none)) local_unnamed_addr #2

; Function Attrs: nofree nounwind
declare noundef i32 @fprintf(ptr noundef captures(none), ptr noundef readonly captures(none), ...) local_unnamed_addr #2

declare void @chali_host_trace() local_unnamed_addr #3

; Function Attrs: nofree noreturn
declare void @exit(i32 noundef) local_unnamed_addr #4

; Function Attrs: nounwind uwtable
define dso_local void @chali_check_shift(i64 noundef %0) local_unnamed_addr #5 {
  %2 = icmp ugt i64 %0, 63
  br i1 %2, label %3, label %4

3:                                                ; preds = %1
  tail call void @chali_panic(ptr noundef nonnull @.str.1)
  unreachable

4:                                                ; preds = %1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_check_real_divisor(double noundef %0) local_unnamed_addr #5 {
  %2 = fcmp oeq double %0, 0.000000e+00
  br i1 %2, label %3, label %4

3:                                                ; preds = %1
  tail call void @chali_panic(ptr noundef nonnull @.str.2)
  unreachable

4:                                                ; preds = %1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_integer_divide(i64 noundef %0, i64 noundef %1, i32 noundef %2, i32 noundef %3) local_unnamed_addr #5 {
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
define dso_local void @chali_init() local_unnamed_addr #5 {
  tail call void @chali_host_init() #20
  ret void
}

declare void @chali_host_init() local_unnamed_addr #3

; Function Attrs: nounwind uwtable
define dso_local void @chali_finish() local_unnamed_addr #5 {
  %1 = load ptr, ptr @stdout, align 8, !tbaa !9
  %2 = tail call i32 @fflush(ptr noundef %1)
  tail call void @chali_host_finish() #20
  ret void
}

declare void @chali_host_finish() local_unnamed_addr #3

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_alloc(i64 noundef %0) local_unnamed_addr #5 {
  %2 = tail call i64 @chali_host_allocate(i64 noundef %0) #20
  ret i64 %2
}

declare i64 @chali_host_allocate(i64 noundef) local_unnamed_addr #3

; Function Attrs: nounwind uwtable
define dso_local void @chali_drop(i64 noundef %0) local_unnamed_addr #5 {
  tail call void @chali_host_drop(i64 noundef %0) #20
  ret void
}

declare void @chali_host_drop(i64 noundef) local_unnamed_addr #3

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_load_i64(i64 noundef %0) local_unnamed_addr #5 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #20
  %3 = load i64, ptr %2, align 1
  ret i64 %3
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(ptr captures(none)) #6

declare ptr @chali_host_address(i64 noundef, i64 noundef) local_unnamed_addr #3

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias writeonly captures(none), ptr noalias readonly captures(none), i64, i1 immarg) #6

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(ptr captures(none)) #6

; Function Attrs: nounwind uwtable
define dso_local void @chali_store_i64(i64 noundef %0, i64 noundef %1) local_unnamed_addr #5 {
  %3 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #20
  store i64 %1, ptr %3, align 1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local double @chali_load_f64(i64 noundef %0) local_unnamed_addr #5 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #20
  %3 = load double, ptr %2, align 1
  ret double %3
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_store_f64(i64 noundef %0, double noundef %1) local_unnamed_addr #5 {
  %3 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #20
  store double %1, ptr %3, align 1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 0, 256) i64 @chali_load_u8(i64 noundef %0) local_unnamed_addr #5 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #20
  %3 = load i8, ptr %2, align 1, !tbaa !12
  %4 = zext i8 %3 to i64
  ret i64 %4
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_store_u8(i64 noundef %0, i64 noundef %1) local_unnamed_addr #5 {
  %3 = trunc i64 %1 to i8
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #20
  store i8 %3, ptr %4, align 1, !tbaa !12
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 -2147483648, 2147483648) i64 @chali_load_i32(i64 noundef %0) local_unnamed_addr #5 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 4) #20
  %3 = load i32, ptr %2, align 1
  %4 = sext i32 %3 to i64
  ret i64 %4
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_store_i32(i64 noundef %0, i64 noundef %1) local_unnamed_addr #5 {
  %3 = trunc i64 %1 to i32
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 4) #20
  store i32 %3, ptr %4, align 1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 -32768, 32768) i64 @chali_load_i16(i64 noundef %0) local_unnamed_addr #5 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 2) #20
  %3 = load i16, ptr %2, align 1
  %4 = sext i16 %3 to i64
  ret i64 %4
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_store_i16(i64 noundef %0, i64 noundef %1) local_unnamed_addr #5 {
  %3 = trunc i64 %1 to i16
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 2) #20
  store i16 %3, ptr %4, align 1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_new(ptr nofree noundef readonly captures(none) %0, i64 noundef %1) local_unnamed_addr #5 {
  %3 = icmp eq i64 %1, 0
  %4 = tail call i64 @llvm.umax.i64(i64 %1, i64 1)
  %5 = tail call ptr @malloc(i64 noundef %4) #22
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
  %12 = tail call i64 @chali_host_allocate(i64 noundef %11) #20
  %13 = tail call ptr @chali_host_address(i64 noundef %12, i64 noundef 8) #20
  store i64 %1, ptr %13, align 1
  br i1 %3, label %17, label %14

14:                                               ; preds = %10
  %15 = add nsw i64 %12, 8
  %16 = tail call ptr @chali_host_address(i64 noundef %15, i64 noundef %1) #20
  tail call void @llvm.memcpy.p0.p0.i64(ptr align 1 %16, ptr nonnull align 1 %5, i64 %1, i1 false)
  br label %17

17:                                               ; preds = %14, %10
  %18 = add i64 %1, 8
  %19 = add i64 %18, %12
  %20 = tail call ptr @chali_host_address(i64 noundef %19, i64 noundef 1) #20
  store i8 0, ptr %20, align 1, !tbaa !12
  tail call void @free(ptr noundef %5)
  ret i64 %12
}

; Function Attrs: mustprogress nofree nounwind willreturn allockind("alloc,uninitialized") allocsize(0) memory(inaccessiblemem: readwrite, errnomem: write)
declare noalias noundef ptr @malloc(i64 noundef) local_unnamed_addr #7

; Function Attrs: mustprogress nounwind willreturn allockind("free") memory(argmem: readwrite, inaccessiblemem: readwrite)
declare void @free(ptr allocptr noundef captures(none)) local_unnamed_addr #8

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable
define dso_local range(i32 0, 2) i32 @chali_on_heap(i64 noundef %0) local_unnamed_addr #9 {
  %2 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %3 = icmp sge i64 %0, %2
  %4 = zext i1 %3 to i32
  ret i32 %4
}

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_string_length(i64 noundef %0) local_unnamed_addr #5 {
  %2 = icmp eq i64 %0, 0
  br i1 %2, label %12, label %3

3:                                                ; preds = %1
  %4 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %5 = icmp slt i64 %0, %4
  br i1 %5, label %9, label %6

6:                                                ; preds = %3
  %7 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #20
  %8 = load i64, ptr %7, align 1
  br label %12

9:                                                ; preds = %3
  %10 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #20
  %11 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %10) #20
  br label %12

12:                                               ; preds = %6, %9, %1
  %13 = phi i64 [ 0, %1 ], [ %8, %6 ], [ %11, %9 ]
  ret i64 %13
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare i64 @strlen(ptr noundef captures(none)) local_unnamed_addr #10

; Function Attrs: nounwind uwtable
define dso_local ptr @chali_string_bytes(i64 noundef %0) local_unnamed_addr #5 {
  %2 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %3 = icmp slt i64 %0, %2
  %4 = select i1 %3, i64 0, i64 8
  %5 = add nsw i64 %4, %0
  %6 = tail call ptr @chali_host_address(i64 noundef %5, i64 noundef 0) #20
  ret ptr %6
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_literal(i64 noundef %0) local_unnamed_addr #5 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #20
  %3 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %2) #20
  %4 = tail call i64 @chali_string_new(ptr noundef nonnull %2, i64 noundef %3)
  ret i64 %4
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_copy(i64 noundef %0) local_unnamed_addr #5 {
  %2 = icmp eq i64 %0, 0
  br i1 %2, label %18, label %3

3:                                                ; preds = %1
  %4 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %5 = icmp slt i64 %0, %4
  %6 = select i1 %5, i64 0, i64 8
  %7 = add nsw i64 %6, %0
  %8 = tail call ptr @chali_host_address(i64 noundef %7, i64 noundef 0) #20
  br i1 %5, label %12, label %9

9:                                                ; preds = %3
  %10 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #20
  %11 = load i64, ptr %10, align 1
  br label %15

12:                                               ; preds = %3
  %13 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #20
  %14 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %13) #20
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
define dso_local void @chali_string_drop(i64 noundef %0) local_unnamed_addr #5 {
  tail call void @chali_host_drop(i64 noundef %0) #20
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 -9223372036854775800, -9223372036854775808) i64 @chali_new_array(i64 noundef %0, i64 noundef %1) local_unnamed_addr #5 {
  %3 = tail call i64 @llvm.smax.i64(i64 %0, i64 0)
  %4 = mul i64 %3, %1
  %5 = add i64 %4, 8
  %6 = tail call i64 @chali_host_allocate(i64 noundef %5) #20
  %7 = tail call ptr @chali_host_address(i64 noundef %6, i64 noundef 8) #20
  store i64 %3, ptr %7, align 1
  %8 = add nsw i64 %6, 8
  ret i64 %8
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_bounds(i64 noundef %0, i64 noundef %1, ptr noundef %2) local_unnamed_addr #5 {
  %4 = alloca [128 x i8], align 16
  %5 = icmp sgt i64 %0, -1
  %6 = icmp slt i64 %0, %1
  %7 = and i1 %5, %6
  br i1 %7, label %10, label %8

8:                                                ; preds = %3
  call void @llvm.lifetime.start.p0(ptr nonnull %4) #20
  %9 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %4, i64 noundef 128, ptr noundef nonnull @.str.6, ptr noundef %2, i64 noundef %0, i64 noundef %1) #20
  call void @chali_panic(ptr noundef nonnull %4)
  unreachable

10:                                               ; preds = %3
  ret void
}

; Function Attrs: nofree nounwind
declare noundef i32 @snprintf(ptr noalias noundef writeonly captures(none), i64 noundef, ptr noundef readonly captures(none), ...) local_unnamed_addr #2

; Function Attrs: nounwind uwtable
define dso_local void @chali_null(i64 noundef %0, ptr noundef %1) local_unnamed_addr #5 {
  %3 = alloca [128 x i8], align 16
  %4 = icmp eq i64 %0, 0
  br i1 %4, label %5, label %7

5:                                                ; preds = %2
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #20
  %6 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i64 noundef 128, ptr noundef nonnull @.str.7, ptr noundef %1) #20
  call void @chali_panic(ptr noundef nonnull %3)
  unreachable

7:                                                ; preds = %2
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none, target_mem: none) uwtable
define dso_local void @chali_fmt_begin(i64 noundef %0) local_unnamed_addr #11 {
  store i64 %0, ptr @chali_fmt_at, align 8, !tbaa !15
  store i64 0, ptr @chali_fmt_pos, align 8, !tbaa !13
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i32 0, 256) i32 @chali_fmt_scan(ptr nofree noundef writeonly captures(none) %0, i64 noundef %1) local_unnamed_addr #5 {
  %3 = load i64, ptr @chali_fmt_at, align 8, !tbaa !15
  %4 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %5 = icmp slt i64 %3, %4
  %6 = select i1 %5, i64 0, i64 8
  %7 = add nsw i64 %6, %3
  %8 = tail call ptr @chali_host_address(i64 noundef %7, i64 noundef 0) #20
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
  br label %10, !llvm.loop !17

20:                                               ; preds = %10, %10, %14
  %21 = icmp ugt i64 %11, %9
  br i1 %21, label %22, label %27

22:                                               ; preds = %20
  %23 = getelementptr inbounds nuw i8, ptr %8, i64 %9
  %24 = sub nuw i64 %11, %9
  %25 = load ptr, ptr @stdout, align 8, !tbaa !9
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
  br label %32, !llvm.loop !20

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
  br i1 %72, label %60, label %55, !llvm.loop !21

73:                                               ; preds = %27
  %74 = add i64 %11, 1
  %75 = getelementptr inbounds nuw i8, ptr %8, i64 %74
  %76 = load i8, ptr %75, align 1, !tbaa !12
  %77 = icmp eq i8 %76, 37
  br i1 %77, label %78, label %82

78:                                               ; preds = %73
  %79 = load ptr, ptr @stdout, align 8, !tbaa !9
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
  br i1 %105, label %106, label %86, !llvm.loop !22

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
  br i1 %122, label %123, label %112, !llvm.loop !23

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
declare noundef i64 @fwrite(ptr noundef readonly captures(none), i64 noundef, i64 noundef, ptr noundef captures(none)) local_unnamed_addr #2

; Function Attrs: nofree nounwind
declare noundef i32 @fputc(i32 noundef, ptr noundef captures(none)) local_unnamed_addr #2

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare ptr @strchr(ptr noundef, i32 noundef) local_unnamed_addr #10

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_emit(i32 noundef %0, ptr noundef %1, i64 noundef %2, i32 noundef %3) local_unnamed_addr #5 {
  %5 = alloca [64 x i8], align 16
  %6 = alloca [40 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %5) #20
  call void @llvm.lifetime.start.p0(ptr nonnull %6) #20
  switch i32 %0, label %36 [
    i32 115, label %7
    i32 99, label %28
    i32 63, label %33
  ]

7:                                                ; preds = %4
  %8 = icmp eq i64 %2, 0
  br i1 %8, label %61, label %9

9:                                                ; preds = %7
  %10 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef 64, ptr noundef nonnull @.str.10, ptr noundef %1) #20
  %11 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %12 = icmp slt i64 %2, %11
  br i1 %12, label %16, label %13

13:                                               ; preds = %9
  %14 = tail call ptr @chali_host_address(i64 noundef %2, i64 noundef 8) #20
  %15 = load i64, ptr %14, align 1
  br label %19

16:                                               ; preds = %9
  %17 = tail call ptr @chali_host_address(i64 noundef %2, i64 noundef 1) #20
  %18 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %17) #20
  br label %19

19:                                               ; preds = %13, %16
  %20 = phi i64 [ %18, %16 ], [ %15, %13 ]
  %21 = trunc i64 %20 to i32
  %22 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %23 = icmp slt i64 %2, %22
  %24 = select i1 %23, i64 0, i64 8
  %25 = add nsw i64 %24, %2
  %26 = tail call ptr @chali_host_address(i64 noundef %25, i64 noundef 0) #20
  %27 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %5, i32 noundef %21, ptr noundef %26)
  br label %61

28:                                               ; preds = %4
  %29 = trunc i64 %2 to i32
  %30 = and i32 %29, 255
  %31 = load ptr, ptr @stdout, align 8, !tbaa !9
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
  %42 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %6, i64 noundef 40, ptr noundef nonnull @.str.12, ptr noundef %1) #20
  %43 = call ptr @strchr(ptr noundef nonnull dereferenceable(1) %6, i32 noundef 46) #20
  %44 = icmp eq ptr %43, null
  br i1 %44, label %45, label %49

45:                                               ; preds = %40
  %46 = call i64 @strlen(ptr noundef nonnull dereferenceable(1) %6) #20
  %47 = sub i64 39, %46
  %48 = call ptr @strncat(ptr noundef nonnull dereferenceable(1) %6, ptr noundef nonnull @.str.13, i64 noundef %47) #20
  br label %49

49:                                               ; preds = %45, %40
  %50 = shl i32 %37, 24
  %51 = ashr exact i32 %50, 24
  %52 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef 64, ptr noundef nonnull @.str.14, ptr noundef nonnull %6, i32 noundef %51) #20
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
  %56 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef 64, ptr noundef nonnull @.str.15, ptr noundef %1, i32 noundef %37) #20
  %57 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef %2)
  br label %61

58:                                               ; preds = %54
  %59 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef 64, ptr noundef nonnull @.str.16, ptr noundef %1) #20
  %60 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef %2)
  br label %61

61:                                               ; preds = %7, %58, %55, %49, %28, %19
  call void @llvm.lifetime.end.p0(ptr nonnull %6) #20
  call void @llvm.lifetime.end.p0(ptr nonnull %5) #20
  ret void
}

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr noundef readonly captures(none), ...) local_unnamed_addr #2

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare ptr @strncat(ptr noalias noundef returned, ptr noalias noundef readonly captures(none), i64 noundef) local_unnamed_addr #12

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_int(i64 noundef %0) local_unnamed_addr #5 {
  %2 = alloca [32 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #20
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
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #20
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_uint(i64 noundef %0) local_unnamed_addr #5 {
  %2 = alloca [32 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #20
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
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #20
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_char(i64 noundef %0) local_unnamed_addr #5 {
  %2 = alloca [32 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #20
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
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #20
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_double(double noundef %0) local_unnamed_addr #5 {
  %2 = alloca [32 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #20
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
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #20
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_string(i64 noundef %0) local_unnamed_addr #5 {
  %2 = alloca [32 x i8], align 16
  %3 = alloca [48 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #20
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #20
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
  %13 = tail call ptr @chali_host_address(i64 noundef %12, i64 noundef 0) #20
  br i1 %10, label %17, label %14

14:                                               ; preds = %8
  %15 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #20
  %16 = load i64, ptr %15, align 1
  br label %20

17:                                               ; preds = %8
  %18 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #20
  %19 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %18) #20
  br label %20

20:                                               ; preds = %14, %17
  %21 = phi i64 [ %19, %17 ], [ %16, %14 ]
  %22 = trunc i64 %21 to i32
  %23 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i64 noundef 48, ptr noundef nonnull @.str.10, ptr noundef nonnull %2) #20
  %24 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %3, i32 noundef %22, ptr noundef %13)
  br label %25

25:                                               ; preds = %1, %6, %6, %20
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #20
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #20
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_finish() local_unnamed_addr #5 {
  %1 = load i64, ptr @chali_fmt_at, align 8, !tbaa !15
  %2 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %3 = icmp slt i64 %1, %2
  %4 = select i1 %3, i64 0, i64 8
  %5 = add nsw i64 %4, %1
  %6 = tail call ptr @chali_host_address(i64 noundef %5, i64 noundef 0) #20
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
  %19 = load ptr, ptr @stdout, align 8, !tbaa !9
  %20 = tail call i32 @fputc(i32 noundef %17, ptr noundef %19)
  %21 = add i64 %9, %18
  br label %8, !llvm.loop !24

22:                                               ; preds = %8, %12
  %23 = sext i8 %11 to i32
  br label %16

24:                                               ; preds = %8
  store i64 %9, ptr @chali_fmt_pos, align 8, !tbaa !13
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 -1, 2) i64 @chali_string_cmp(i64 noundef %0, i64 noundef %1) local_unnamed_addr #5 {
  %3 = icmp eq i64 %0, 0
  br i1 %3, label %13, label %4

4:                                                ; preds = %2
  %5 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %6 = icmp slt i64 %0, %5
  br i1 %6, label %10, label %7

7:                                                ; preds = %4
  %8 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #20
  %9 = load i64, ptr %8, align 1
  br label %13

10:                                               ; preds = %4
  %11 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #20
  %12 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %11) #20
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
  %20 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 8) #20
  %21 = load i64, ptr %20, align 1
  br label %25

22:                                               ; preds = %16
  %23 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 1) #20
  %24 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %23) #20
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
  %34 = tail call ptr @chali_host_address(i64 noundef %33, i64 noundef 0) #20
  %35 = icmp slt i64 %1, %30
  %36 = select i1 %35, i64 0, i64 8
  %37 = add nsw i64 %36, %1
  %38 = tail call ptr @chali_host_address(i64 noundef %37, i64 noundef 0) #20
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
declare i32 @memcmp(ptr noundef captures(none), ptr noundef captures(none), i64 noundef) local_unnamed_addr #10

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_sub(i64 noundef %0, i64 noundef %1, i64 noundef %2) local_unnamed_addr #5 {
  %4 = icmp eq i64 %0, 0
  br i1 %4, label %5, label %10

5:                                                ; preds = %3
  %6 = tail call i64 @chali_host_allocate(i64 noundef 9) #20
  %7 = tail call ptr @chali_host_address(i64 noundef %6, i64 noundef 8) #20
  store i64 0, ptr %7, align 1
  %8 = add i64 %6, 8
  %9 = tail call ptr @chali_host_address(i64 noundef %8, i64 noundef 1) #20
  store i8 0, ptr %9, align 1, !tbaa !12
  br label %33

10:                                               ; preds = %3
  %11 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %12 = icmp slt i64 %0, %11
  br i1 %12, label %16, label %13

13:                                               ; preds = %10
  %14 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #20
  %15 = load i64, ptr %14, align 1
  br label %19

16:                                               ; preds = %10
  %17 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #20
  %18 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %17) #20
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
  %30 = tail call ptr @chali_host_address(i64 noundef %29, i64 noundef 0) #20
  %31 = getelementptr inbounds i8, ptr %30, i64 %23
  %32 = tail call i64 @chali_string_new(ptr noundef %31, i64 noundef %25)
  br label %33

33:                                               ; preds = %19, %5
  %34 = phi i64 [ %6, %5 ], [ %32, %19 ]
  ret i64 %34
}

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_string_pos(i64 noundef %0, i64 noundef %1) local_unnamed_addr #5 {
  %3 = icmp eq i64 %0, 0
  %4 = icmp eq i64 %1, 0
  %5 = or i1 %3, %4
  br i1 %5, label %49, label %6

6:                                                ; preds = %2
  %7 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %8 = icmp slt i64 %0, %7
  br i1 %8, label %12, label %9

9:                                                ; preds = %6
  %10 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #20
  %11 = load i64, ptr %10, align 1
  br label %15

12:                                               ; preds = %6
  %13 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #20
  %14 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %13) #20
  br label %15

15:                                               ; preds = %12, %9
  %16 = phi i64 [ %14, %12 ], [ %11, %9 ]
  %17 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %18 = icmp slt i64 %1, %17
  br i1 %18, label %22, label %19

19:                                               ; preds = %15
  %20 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 8) #20
  %21 = load i64, ptr %20, align 1
  br label %25

22:                                               ; preds = %15
  %23 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 1) #20
  %24 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %23) #20
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
  %40 = tail call ptr @chali_host_address(i64 noundef %34, i64 noundef 0) #20
  %41 = getelementptr inbounds nuw i8, ptr %40, i64 %39
  %42 = tail call ptr @chali_host_address(i64 noundef %37, i64 noundef 0) #20
  %43 = tail call i32 @bcmp(ptr %41, ptr %42, i64 %26)
  %44 = icmp eq i32 %43, 0
  br i1 %44, label %49, label %45

45:                                               ; preds = %38
  %46 = add nuw nsw i64 %39, 1
  %47 = add nsw i64 %46, %26
  %48 = icmp sgt i64 %47, %16
  br i1 %48, label %49, label %38, !llvm.loop !25

49:                                               ; preds = %38, %45, %28, %25, %2
  %50 = phi i64 [ -1, %2 ], [ 0, %25 ], [ -1, %28 ], [ -1, %45 ], [ %39, %38 ]
  ret i64 %50
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_read_line() local_unnamed_addr #5 {
  %1 = tail call dereferenceable_or_null(256) ptr @malloc(i64 noundef 256) #22
  %2 = icmp eq ptr %1, null
  br i1 %2, label %3, label %4

3:                                                ; preds = %0
  tail call void @chali_panic(ptr noundef nonnull @.str.18)
  unreachable

4:                                                ; preds = %0, %21
  %5 = phi ptr [ %22, %21 ], [ %1, %0 ]
  %6 = phi i64 [ %25, %21 ], [ 0, %0 ]
  %7 = phi i64 [ %23, %21 ], [ 256, %0 ]
  %8 = load ptr, ptr @stdin, align 8, !tbaa !9
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
  %18 = tail call ptr @realloc(ptr noundef %5, i64 noundef %17) #23
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
  br label %4, !llvm.loop !26

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
declare noundef i32 @fgetc(ptr noundef captures(none)) local_unnamed_addr #2

; Function Attrs: mustprogress nounwind willreturn allockind("realloc") allocsize(1) memory(argmem: readwrite, inaccessiblemem: readwrite, errnomem: write)
declare noalias noundef ptr @realloc(ptr allocptr noundef captures(none), i64 noundef) local_unnamed_addr #13

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_fixed(double noundef %0, i64 noundef %1) local_unnamed_addr #5 {
  %3 = alloca [256 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #20
  %4 = icmp slt i64 %1, 0
  %5 = trunc i64 %1 to i32
  %6 = select i1 %4, i32 3, i32 %5
  %7 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i64 noundef 256, ptr noundef nonnull @.str.21, i32 noundef %6, double noundef %0) #20
  %8 = icmp slt i32 %7, 0
  br i1 %8, label %9, label %10

9:                                                ; preds = %2
  tail call void @chali_panic(ptr noundef nonnull @.str.22)
  unreachable

10:                                               ; preds = %2
  %11 = tail call i32 @llvm.umin.i32(i32 %7, i32 255)
  %12 = zext nneg i32 %11 to i64
  %13 = call i64 @chali_string_new(ptr noundef nonnull %3, i64 noundef %12)
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #20
  ret i64 %13
}

; Function Attrs: nofree nounwind uwtable
define dso_local i64 @chali_read_number(i32 noundef %0) local_unnamed_addr #14 {
  %2 = alloca [256 x i8], align 16
  %3 = alloca double, align 8
  %4 = alloca i64, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #20
  %5 = load ptr, ptr @stdin, align 8, !tbaa !9
  %6 = call ptr @fgets(ptr noundef nonnull %2, i32 noundef 256, ptr noundef %5)
  %7 = icmp eq ptr %6, null
  br i1 %7, label %22, label %8

8:                                                ; preds = %1
  %9 = icmp eq i32 %0, 0
  br i1 %9, label %16, label %10

10:                                               ; preds = %8
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #20
  store double 0.000000e+00, ptr %3, align 8, !tbaa !27
  %11 = call i32 (ptr, ptr, ...) @sscanf(ptr noundef nonnull %2, ptr noundef nonnull @.str.23, ptr noundef nonnull %3)
  %12 = icmp eq i32 %11, 1
  br i1 %12, label %14, label %13

13:                                               ; preds = %10
  store double 0.000000e+00, ptr %3, align 8, !tbaa !27
  br label %14

14:                                               ; preds = %13, %10
  %15 = load i64, ptr %3, align 8, !tbaa !27
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #20
  br label %22

16:                                               ; preds = %8
  call void @llvm.lifetime.start.p0(ptr nonnull %4) #20
  store i64 0, ptr %4, align 8, !tbaa !15
  %17 = call i32 (ptr, ptr, ...) @sscanf(ptr noundef nonnull %2, ptr noundef nonnull @.str.24, ptr noundef nonnull %4)
  %18 = icmp eq i32 %17, 1
  br i1 %18, label %20, label %19

19:                                               ; preds = %16
  store i64 0, ptr %4, align 8, !tbaa !15
  br label %20

20:                                               ; preds = %19, %16
  %21 = load i64, ptr %4, align 8, !tbaa !15
  call void @llvm.lifetime.end.p0(ptr nonnull %4) #20
  br label %22

22:                                               ; preds = %1, %20, %14
  %23 = phi i64 [ %21, %20 ], [ %15, %14 ], [ 0, %1 ]
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #20
  ret i64 %23
}

; Function Attrs: nofree nounwind
declare noundef ptr @fgets(ptr noundef writeonly, i32 noundef, ptr noundef captures(none)) local_unnamed_addr #2

; Function Attrs: nofree nounwind
declare noundef i32 @sscanf(ptr noundef readonly captures(none), ptr noundef readonly captures(none), ...) local_unnamed_addr #2

; Function Attrs: nofree nounwind uwtable
define dso_local range(i64 0, 2) i64 @chali_stdin_eof() local_unnamed_addr #14 {
  %1 = load ptr, ptr @stdin, align 8, !tbaa !9
  %2 = tail call i32 @feof(ptr noundef %1)
  %3 = icmp ne i32 %2, 0
  %4 = zext i1 %3 to i64
  ret i64 %4
}

; Function Attrs: nofree nounwind
declare noundef i32 @feof(ptr noundef captures(none)) local_unnamed_addr #2

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_of_int(i64 noundef %0) local_unnamed_addr #5 {
  %2 = alloca [32 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #20
  %3 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %2, i64 noundef 32, ptr noundef nonnull @.str.24, i64 noundef %0) #20
  %4 = call i64 @strlen(ptr noundef nonnull dereferenceable(1) %2) #20
  %5 = call i64 @chali_string_new(ptr noundef nonnull %2, i64 noundef %4)
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #20
  ret i64 %5
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_of_double(double noundef %0) local_unnamed_addr #5 {
  %2 = alloca [64 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #20
  %3 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %2, i64 noundef 64, ptr noundef nonnull @.str.25, double noundef %0) #20
  %4 = call i64 @strlen(ptr noundef nonnull dereferenceable(1) %2) #20
  %5 = call i64 @chali_string_new(ptr noundef nonnull %2, i64 noundef %4)
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #20
  ret i64 %5
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_check(i64 noundef %0, i64 noundef %1) local_unnamed_addr #5 {
  %3 = alloca [128 x i8], align 16
  %4 = icmp sgt i64 %0, -1
  %5 = icmp slt i64 %0, %1
  %6 = and i1 %4, %5
  br i1 %6, label %9, label %7

7:                                                ; preds = %2
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #20
  %8 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i64 noundef 128, ptr noundef nonnull @.str.6, ptr noundef nonnull @.str.26, i64 noundef %0, i64 noundef %1) #20
  call void @chali_panic(ptr noundef nonnull %3)
  unreachable

9:                                                ; preds = %2
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_check_pointer(i64 noundef %0) local_unnamed_addr #5 {
  %2 = alloca [128 x i8], align 16
  %3 = icmp eq i64 %0, 0
  br i1 %3, label %4, label %6

4:                                                ; preds = %1
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #20
  %5 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %2, i64 noundef 128, ptr noundef nonnull @.str.7, ptr noundef nonnull @.str.27) #20
  call void @chali_panic(ptr noundef nonnull %2)
  unreachable

6:                                                ; preds = %1
  ret void
}

; Function Attrs: nofree nounwind uwtable
define dso_local void @chali_put_char(i64 noundef %0) local_unnamed_addr #14 {
  %2 = trunc i64 %0 to i32
  %3 = and i32 %2, 255
  %4 = load ptr, ptr @stdout, align 8, !tbaa !9
  %5 = tail call i32 @fputc(i32 noundef %3, ptr noundef %4)
  ret void
}

; Function Attrs: noreturn nounwind uwtable
define dso_local void @chali_abort(i64 noundef %0) local_unnamed_addr #15 {
  %2 = icmp eq i64 %0, 0
  br i1 %2, label %27, label %3

3:                                                ; preds = %1
  store i64 %0, ptr @chali_fmt_at, align 8, !tbaa !15
  store i64 0, ptr @chali_fmt_pos, align 8, !tbaa !13
  %4 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %5 = icmp slt i64 %0, %4
  %6 = select i1 %5, i64 0, i64 8
  %7 = add nsw i64 %6, %0
  %8 = tail call ptr @chali_host_address(i64 noundef %7, i64 noundef 0) #20
  %9 = load i64, ptr @chali_fmt_pos, align 8, !tbaa !13
  br label %10

10:                                               ; preds = %18, %3
  %11 = phi i64 [ %9, %3 ], [ %23, %18 ]
  %12 = getelementptr inbounds nuw i8, ptr %8, i64 %11
  %13 = load i8, ptr %12, align 1, !tbaa !12
  switch i8 %13, label %24 [
    i8 0, label %26
    i8 37, label %14
  ]

14:                                               ; preds = %10
  %15 = getelementptr i8, ptr %12, i64 1
  %16 = load i8, ptr %15, align 1, !tbaa !12
  %17 = icmp eq i8 %16, 37
  br i1 %17, label %18, label %24

18:                                               ; preds = %14, %24
  %19 = phi i32 [ %25, %24 ], [ 37, %14 ]
  %20 = phi i64 [ 1, %24 ], [ 2, %14 ]
  %21 = load ptr, ptr @stdout, align 8, !tbaa !9
  %22 = tail call i32 @fputc(i32 noundef %19, ptr noundef %21)
  %23 = add i64 %11, %20
  br label %10, !llvm.loop !24

24:                                               ; preds = %14, %10
  %25 = sext i8 %13 to i32
  br label %18

26:                                               ; preds = %10
  store i64 %11, ptr @chali_fmt_pos, align 8, !tbaa !13
  br label %27

27:                                               ; preds = %26, %1
  tail call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_assert(i64 noundef %0, i64 noundef %1) local_unnamed_addr #5 {
  %3 = icmp eq i64 %0, 0
  br i1 %3, label %4, label %5

4:                                                ; preds = %2
  tail call void @chali_abort(i64 noundef %1)
  unreachable

5:                                                ; preds = %2
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_copy_bytes(i64 noundef returned %0, i64 noundef %1, i64 noundef %2) local_unnamed_addr #5 {
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef %2) #20
  %5 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef %2) #20
  tail call void @llvm.memmove.p0.p0.i64(ptr align 1 %4, ptr align 1 %5, i64 %2, i1 false)
  ret i64 %0
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memmove.p0.p0.i64(ptr writeonly captures(none), ptr readonly captures(none), i64, i1 immarg) #6

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_set_bytes(i64 noundef returned %0, i64 noundef %1, i64 noundef %2) local_unnamed_addr #5 {
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef %2) #20
  %5 = trunc i64 %1 to i8
  tail call void @llvm.memset.p0.i64(ptr align 1 %4, i8 %5, i64 %2, i1 false)
  ret i64 %0
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #16

; Function Attrs: nounwind uwtable
define dso_local range(i64 0, 256) i64 @chali_string_at(i64 noundef %0, i64 noundef %1) local_unnamed_addr #5 {
  %3 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %4 = icmp slt i64 %0, %3
  %5 = select i1 %4, i64 0, i64 8
  %6 = add nsw i64 %5, %0
  %7 = tail call ptr @chali_host_address(i64 noundef %6, i64 noundef 0) #20
  %8 = getelementptr inbounds i8, ptr %7, i64 %1
  %9 = load i8, ptr %8, align 1, !tbaa !12
  %10 = zext i8 %9 to i64
  ret i64 %10
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_string_set(i64 noundef %0, i64 noundef %1, i64 noundef %2) local_unnamed_addr #5 {
  %4 = add nsw i64 %0, 8
  %5 = add nsw i64 %4, %1
  %6 = trunc i64 %2 to i8
  %7 = tail call ptr @chali_host_address(i64 noundef %5, i64 noundef 1) #20
  store i8 %6, ptr %7, align 1, !tbaa !12
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_concat(i64 noundef %0, i64 noundef %1) local_unnamed_addr #5 {
  %3 = icmp eq i64 %0, 0
  br i1 %3, label %13, label %4

4:                                                ; preds = %2
  %5 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %6 = icmp slt i64 %0, %5
  br i1 %6, label %10, label %7

7:                                                ; preds = %4
  %8 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #20
  %9 = load i64, ptr %8, align 1
  br label %13

10:                                               ; preds = %4
  %11 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #20
  %12 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %11) #20
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
  %20 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 8) #20
  %21 = load i64, ptr %20, align 1
  br label %25

22:                                               ; preds = %16
  %23 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 1) #20
  %24 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %23) #20
  br label %25

25:                                               ; preds = %13, %19, %22
  %26 = phi i64 [ 0, %13 ], [ %21, %19 ], [ %24, %22 ]
  %27 = add i64 %26, %14
  %28 = add i64 %27, 8
  %29 = tail call i64 @chali_host_allocate(i64 noundef %28) #20
  %30 = tail call ptr @chali_host_address(i64 noundef %29, i64 noundef 8) #20
  store i64 %27, ptr %30, align 1
  %31 = add nsw i64 %29, 8
  %32 = tail call ptr @chali_host_address(i64 noundef %31, i64 noundef %14) #20
  %33 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %34 = icmp slt i64 %0, %33
  %35 = select i1 %34, i64 0, i64 8
  %36 = add nsw i64 %35, %0
  %37 = tail call ptr @chali_host_address(i64 noundef %36, i64 noundef 0) #20
  tail call void @llvm.memcpy.p0.p0.i64(ptr align 1 %32, ptr align 1 %37, i64 %14, i1 false)
  %38 = add i64 %31, %14
  %39 = tail call ptr @chali_host_address(i64 noundef %38, i64 noundef %26) #20
  %40 = icmp slt i64 %1, %33
  %41 = select i1 %40, i64 0, i64 8
  %42 = add nsw i64 %41, %1
  %43 = tail call ptr @chali_host_address(i64 noundef %42, i64 noundef 0) #20
  tail call void @llvm.memcpy.p0.p0.i64(ptr align 1 %39, ptr align 1 %43, i64 %26, i1 false)
  ret i64 %29
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_owned_drop(i64 noundef %0) local_unnamed_addr #5 {
  tail call void @chali_host_drop(i64 noundef %0) #20
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_function_target(i64 noundef %0) local_unnamed_addr #5 {
  %2 = tail call i64 @chali_host_function(i64 noundef %0, i32 noundef 0) #20
  ret i64 %2
}

declare i64 @chali_host_function(i64 noundef, i32 noundef) local_unnamed_addr #3

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_function_environment(i64 noundef %0) local_unnamed_addr #5 {
  %2 = tail call i64 @chali_host_function(i64 noundef %0, i32 noundef 1) #20
  ret i64 %2
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 0, 2) i64 @chali_function_match(i64 noundef %0, ptr nofree noundef readonly captures(none) %1) local_unnamed_addr #5 {
  %3 = icmp slt i64 %0, 1
  br i1 %3, label %17, label %4

4:                                                ; preds = %2
  %5 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %6 = icmp ult i64 %0, %5
  br i1 %6, label %7, label %17

7:                                                ; preds = %4
  %8 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %1) #20
  %9 = sub nuw i64 %5, %0
  %10 = icmp ult i64 %8, %9
  br i1 %10, label %11, label %17

11:                                               ; preds = %7
  %12 = add nuw i64 %8, 1
  %13 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef %12) #20
  %14 = tail call i32 @bcmp(ptr noundef nonnull dereferenceable(1) %13, ptr noundef nonnull dereferenceable(1) %1, i64 %12)
  %15 = icmp eq i32 %14, 0
  %16 = zext i1 %15 to i64
  br label %17

17:                                               ; preds = %11, %7, %2, %4
  %18 = phi i64 [ 0, %2 ], [ 0, %4 ], [ %16, %11 ], [ 0, %7 ]
  ret i64 %18
}

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.umax.i64(i64, i64) #17

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.smax.i64(i64, i64) #17

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare ptr @memchr(ptr, i32, i64) local_unnamed_addr #18

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.umin.i64(i64, i64) #17

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare i32 @bcmp(ptr captures(none), ptr captures(none), i64) local_unnamed_addr #18

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.umin.i32(i32, i32) #17

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare range(i64 -1, 2) i64 @llvm.ucmp.i64.i64(i64, i64) #17

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.smin.i64(i64, i64) #17

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(read, argmem: none, inaccessiblemem: none, target_mem: none) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { cold noreturn nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nofree nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { nofree noreturn "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #6 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #7 = { mustprogress nofree nounwind willreturn allockind("alloc,uninitialized") allocsize(0) memory(inaccessiblemem: readwrite, errnomem: write) "alloc-family"="malloc" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #8 = { mustprogress nounwind willreturn allockind("free") memory(argmem: readwrite, inaccessiblemem: readwrite) "alloc-family"="malloc" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #9 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #10 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: read) "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #11 = { mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none, target_mem: none) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #12 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #13 = { mustprogress nounwind willreturn allockind("realloc") allocsize(1) memory(argmem: readwrite, inaccessiblemem: readwrite, errnomem: write) "alloc-family"="malloc" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #14 = { nofree nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #15 = { noreturn nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #16 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: write) }
attributes #17 = { nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none) }
attributes #18 = { nocallback nofree nosync nounwind willreturn memory(argmem: read) }
attributes #19 = { cold nounwind }
attributes #20 = { nounwind }
attributes #21 = { cold noreturn nounwind }
attributes #22 = { allocsize(0) }
attributes #23 = { allocsize(1) }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}
!llvm.errno.tbaa = !{!4}

!0 = !{i32 8, !"PIC Level", i32 2}
!1 = !{i32 7, !"PIE Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 2}
!3 = !{!"clang version 23.1.4 (https://github.com/llvm/llvm-project.git 7a941dfb2877a0b8cf30323fefa85ad019e0b0f5)"}
!4 = !{!5, !6, i64 0}
!5 = !{!"__libc_errno", !6, i64 0}
!6 = !{!"int", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C/C++ TBAA"}
!9 = !{!10, !10, i64 0}
!10 = !{!"p1 _ZTS9ChaliFile", !11, i64 0}
!11 = !{!"any pointer", !7, i64 0}
!12 = !{!7, !7, i64 0}
!13 = !{!14, !14, i64 0}
!14 = !{!"long", !7, i64 0}
!15 = !{!16, !16, i64 0}
!16 = !{!"long long", !7, i64 0}
!17 = distinct !{!17, !18, !19}
!18 = !{!"llvm.loop.mustprogress"}
!19 = !{!"llvm.loop.unroll.disable"}
!20 = distinct !{!20, !18, !19}
!21 = distinct !{!21, !18, !19}
!22 = distinct !{!22, !18, !19}
!23 = distinct !{!23, !18, !19}
!24 = distinct !{!24, !18, !19}
!25 = distinct !{!25, !18, !19}
!26 = distinct !{!26, !18, !19}
!27 = !{!28, !28, i64 0}
!28 = !{!"double", !7, i64 0}
