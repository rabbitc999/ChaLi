; ModuleID = 'source/aot/runtime.c'
source_filename = "source/aot/runtime.c"
target datalayout = "e-m:e-p:32:32-Fi8-i64:64-v128:64:128-a:0:32-n32-S64"
target triple = "thumbv7em-unknown-none-eabi"

@stdin = external dso_local local_unnamed_addr global ptr, align 4
@stdout = external dso_local local_unnamed_addr global ptr, align 4
@stderr = external dso_local local_unnamed_addr global ptr, align 4
@.str = private unnamed_addr constant [55 x i8] c"\E7\A7\BB\E4\BD\8D\E4\BD\8D\E6\95\B0\E8\B6\85\E5\87\BA\E6\95\B4\E6\95\B0\E4\BD\8D\E5\AE\BD\EF\BC\88\E6\9C\89\E6\95\88\E8\8C\83\E5\9B\B4 0..63\EF\BC\89\00", align 1
@.str.1 = private unnamed_addr constant [13 x i8] c"\E6\B5\AE\E7\82\B9\E9\99\A4\E9\9B\B6\00", align 1
@.str.2 = private unnamed_addr constant [13 x i8] c"\E6\95\B4\E6\95\B0\E9\99\A4\E9\9B\B6\00", align 1
@.str.3 = private unnamed_addr constant [72 x i8] c"\E6\95\B4\E6\95\B0\E9\99\A4\E6\B3\95\E6\BA\A2\E5\87\BA\EF\BC\9A\E6\9C\80\E5\B0\8F\E8\B4\9F\E6\95\B0\E9\99\A4\E4\BB\A5 -1 \E7\9A\84\E7\BB\93\E6\9E\9C\E8\B6\85\E5\87\BA int64 \E8\8C\83\E5\9B\B4\00", align 1
@.str.5 = private unnamed_addr constant [31 x i8] c"\E5\A4\8D\E5\88\B6\E5\AD\97\E7\AC\A6\E4\B8\B2\E6\97\B6\E5\86\85\E5\AD\98\E4\B8\8D\E8\B6\B3\00", align 1
@chali_data_size = external dso_local local_unnamed_addr constant i64, align 8
@.str.6 = private unnamed_addr constant [37 x i8] c"%s\E8\B6\8A\E7\95\8C\EF\BC\9A\E4\B8\8B\E6\A0\87 %lld\EF\BC\8C\E9\95\BF\E5\BA\A6 %lld\00", align 1
@.str.7 = private unnamed_addr constant [15 x i8] c"%s\EF\BC\9A\E7\A9\BA\E6\8C\87\E9\92\88\00", align 1
@chali_fmt_at = internal unnamed_addr global i64 0, align 8
@chali_fmt_pos = internal unnamed_addr global i32 0, align 4
@.str.8 = private unnamed_addr constant [16 x i8] c"-+ #0123456789.\00", align 1
@.str.9 = private unnamed_addr constant [8 x i8] c"hlLqjzt\00", align 1
@.str.10 = private unnamed_addr constant [6 x i8] c"%s.*s\00", align 1
@.str.11 = private unnamed_addr constant [9 x i8] c"gGeEfFaA\00", align 1
@.str.12 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str.13 = private unnamed_addr constant [3 x i8] c".3\00", align 1
@.str.14 = private unnamed_addr constant [5 x i8] c"%s%c\00", align 1
@.str.15 = private unnamed_addr constant [7 x i8] c"%sll%c\00", align 1
@.str.16 = private unnamed_addr constant [6 x i8] c"%slld\00", align 1
@.str.17 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
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

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(read, argmem: none, inaccessiblemem: none, target_mem: none)
define dso_local ptr @chali_standard_stream(i32 noundef %0) local_unnamed_addr #0 {
  %2 = icmp eq i32 %0, 0
  br i1 %2, label %3, label %5

3:                                                ; preds = %1
  %4 = load ptr, ptr @stdin, align 4, !tbaa !9
  br label %10

5:                                                ; preds = %1
  %6 = icmp eq i32 %0, 1
  %7 = load ptr, ptr @stdout, align 4
  %8 = load ptr, ptr @stderr, align 4
  %9 = select i1 %6, ptr %7, ptr %8
  br label %10

10:                                               ; preds = %5, %3
  %11 = phi ptr [ %4, %3 ], [ %9, %5 ]
  ret ptr %11
}

; Function Attrs: noreturn nounwind
define dso_local void @chali_panic(ptr noundef %0) local_unnamed_addr #1 {
  tail call void @chali_mcu_panic(ptr noundef %0) #8
  unreachable
}

declare dso_local void @chali_mcu_panic(ptr noundef) local_unnamed_addr #2

; Function Attrs: nounwind
define dso_local void @chali_check_shift(i64 noundef %0) local_unnamed_addr #3 {
  %2 = icmp ugt i64 %0, 63
  br i1 %2, label %3, label %4

3:                                                ; preds = %1
  tail call void @chali_mcu_panic(ptr noundef nonnull @.str) #8
  unreachable

4:                                                ; preds = %1
  ret void
}

; Function Attrs: nounwind
define dso_local void @chali_check_real_divisor(double noundef %0) local_unnamed_addr #3 {
  %2 = fcmp oeq double %0, 0.000000e+00
  br i1 %2, label %3, label %4

3:                                                ; preds = %1
  tail call void @chali_mcu_panic(ptr noundef nonnull @.str.1) #8
  unreachable

4:                                                ; preds = %1
  ret void
}

; Function Attrs: nounwind
define dso_local noundef i64 @chali_integer_divide(i64 noundef %0, i64 noundef %1, i32 noundef %2, i32 noundef %3) local_unnamed_addr #3 {
  %5 = icmp eq i64 %1, 0
  br i1 %5, label %6, label %7

6:                                                ; preds = %4
  tail call void @chali_mcu_panic(ptr noundef nonnull @.str.2) #8
  unreachable

7:                                                ; preds = %4
  %8 = icmp eq i32 %2, 0
  br i1 %8, label %15, label %9

9:                                                ; preds = %7
  %10 = icmp eq i32 %3, 0
  br i1 %10, label %13, label %11

11:                                               ; preds = %9
  %12 = urem i64 %0, %1
  br label %27

13:                                               ; preds = %9
  %14 = udiv i64 %0, %1
  br label %27

15:                                               ; preds = %7
  %16 = icmp eq i64 %0, -9223372036854775808
  %17 = icmp eq i64 %1, -1
  %18 = and i1 %16, %17
  %19 = icmp eq i32 %3, 0
  br i1 %18, label %20, label %22

20:                                               ; preds = %15
  br i1 %19, label %21, label %27

21:                                               ; preds = %20
  tail call void @chali_mcu_panic(ptr noundef nonnull @.str.3) #8
  unreachable

22:                                               ; preds = %15
  br i1 %19, label %25, label %23

23:                                               ; preds = %22
  %24 = srem i64 %0, %1
  br label %27

25:                                               ; preds = %22
  %26 = sdiv i64 %0, %1
  br label %27

27:                                               ; preds = %23, %25, %20, %11, %13
  %28 = phi i64 [ 0, %20 ], [ %14, %13 ], [ %12, %11 ], [ %24, %23 ], [ %26, %25 ]
  ret i64 %28
}

; Function Attrs: nounwind
define dso_local void @chali_init() local_unnamed_addr #3 {
  tail call void @chali_host_init() #8
  ret void
}

declare dso_local void @chali_host_init() local_unnamed_addr #2

; Function Attrs: nounwind
define dso_local void @chali_finish() local_unnamed_addr #3 {
  %1 = load ptr, ptr @stdout, align 4, !tbaa !9
  %2 = tail call i32 @fflush(ptr noundef %1) #8
  tail call void @chali_host_finish() #8
  ret void
}

declare dso_local i32 @fflush(ptr noundef) local_unnamed_addr #2

declare dso_local void @chali_host_finish() local_unnamed_addr #2

; Function Attrs: nounwind
define dso_local i64 @chali_alloc(i64 noundef %0) local_unnamed_addr #3 {
  %2 = tail call i64 @chali_host_allocate(i64 noundef %0) #8
  ret i64 %2
}

declare dso_local i64 @chali_host_allocate(i64 noundef) local_unnamed_addr #2

; Function Attrs: nounwind
define dso_local void @chali_drop(i64 noundef %0) local_unnamed_addr #3 {
  tail call void @chali_host_drop(i64 noundef %0) #8
  ret void
}

declare dso_local void @chali_host_drop(i64 noundef) local_unnamed_addr #2

; Function Attrs: nounwind
define dso_local i64 @chali_load_i64(i64 noundef %0) local_unnamed_addr #3 {
  %2 = alloca i64, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #9
  store i64 0, ptr %2, align 8, !tbaa !12
  %3 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #8
  %4 = call ptr @memcpy(ptr noundef nonnull %2, ptr noundef %3, i32 noundef 8) #8
  %5 = load i64, ptr %2, align 8, !tbaa !12
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #9
  ret i64 %5
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(ptr captures(none)) #4

declare dso_local ptr @memcpy(ptr noundef, ptr noundef, i32 noundef) local_unnamed_addr #2

declare dso_local ptr @chali_host_address(i64 noundef, i64 noundef) local_unnamed_addr #2

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(ptr captures(none)) #4

; Function Attrs: nounwind
define dso_local void @chali_store_i64(i64 noundef %0, i64 noundef %1) local_unnamed_addr #3 {
  %3 = alloca i64, align 8
  store i64 %1, ptr %3, align 8, !tbaa !12
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #8
  %5 = call ptr @memcpy(ptr noundef %4, ptr noundef nonnull %3, i32 noundef 8) #8
  ret void
}

; Function Attrs: nounwind
define dso_local double @chali_load_f64(i64 noundef %0) local_unnamed_addr #3 {
  %2 = alloca i64, align 8
  %3 = alloca i64, align 8
  %4 = alloca double, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #9
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #9
  store i64 0, ptr %2, align 8, !tbaa !12
  %5 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #8
  %6 = call ptr @memcpy(ptr noundef nonnull %2, ptr noundef %5, i32 noundef 8) #8
  %7 = load i64, ptr %2, align 8, !tbaa !12
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #9
  store i64 %7, ptr %3, align 8, !tbaa !12
  call void @llvm.lifetime.start.p0(ptr nonnull %4) #9
  store double 0.000000e+00, ptr %4, align 8, !tbaa !14
  %8 = call ptr @memcpy(ptr noundef nonnull %4, ptr noundef nonnull %3, i32 noundef 8) #8
  %9 = load double, ptr %4, align 8, !tbaa !14
  call void @llvm.lifetime.end.p0(ptr nonnull %4) #9
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #9
  ret double %9
}

; Function Attrs: nounwind
define dso_local void @chali_store_f64(i64 noundef %0, double noundef %1) local_unnamed_addr #3 {
  %3 = alloca i64, align 8
  %4 = alloca double, align 8
  %5 = alloca i64, align 8
  store double %1, ptr %4, align 8, !tbaa !14
  call void @llvm.lifetime.start.p0(ptr nonnull %5) #9
  store i64 0, ptr %5, align 8, !tbaa !12
  %6 = call ptr @memcpy(ptr noundef nonnull %5, ptr noundef nonnull %4, i32 noundef 8) #8
  %7 = load i64, ptr %5, align 8, !tbaa !12
  call void @llvm.lifetime.start.p0(ptr nonnull %3)
  store i64 %7, ptr %3, align 8, !tbaa !12
  %8 = call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #8
  %9 = call ptr @memcpy(ptr noundef %8, ptr noundef nonnull %3, i32 noundef 8) #8
  call void @llvm.lifetime.end.p0(ptr nonnull %3)
  call void @llvm.lifetime.end.p0(ptr nonnull %5) #9
  ret void
}

; Function Attrs: nounwind
define dso_local range(i64 0, 256) i64 @chali_load_u8(i64 noundef %0) local_unnamed_addr #3 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #8
  %3 = load i8, ptr %2, align 1, !tbaa !16
  %4 = zext i8 %3 to i64
  ret i64 %4
}

; Function Attrs: nounwind
define dso_local void @chali_store_u8(i64 noundef %0, i64 noundef %1) local_unnamed_addr #3 {
  %3 = trunc i64 %1 to i8
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #8
  store i8 %3, ptr %4, align 1, !tbaa !16
  ret void
}

; Function Attrs: nounwind
define dso_local range(i64 -2147483648, 2147483648) i64 @chali_load_i32(i64 noundef %0) local_unnamed_addr #3 {
  %2 = alloca i32, align 4
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #9
  store i32 0, ptr %2, align 4, !tbaa !17
  %3 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 4) #8
  %4 = call ptr @memcpy(ptr noundef nonnull %2, ptr noundef %3, i32 noundef 4) #8
  %5 = load i32, ptr %2, align 4, !tbaa !17
  %6 = sext i32 %5 to i64
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #9
  ret i64 %6
}

; Function Attrs: nounwind
define dso_local void @chali_store_i32(i64 noundef %0, i64 noundef %1) local_unnamed_addr #3 {
  %3 = alloca i32, align 4
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #9
  %4 = trunc i64 %1 to i32
  store i32 %4, ptr %3, align 4, !tbaa !17
  %5 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 4) #8
  %6 = call ptr @memcpy(ptr noundef %5, ptr noundef nonnull %3, i32 noundef 4) #8
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #9
  ret void
}

; Function Attrs: nounwind
define dso_local range(i64 -32768, 32768) i64 @chali_load_i16(i64 noundef %0) local_unnamed_addr #3 {
  %2 = alloca i16, align 2
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #9
  store i16 0, ptr %2, align 2, !tbaa !18
  %3 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 2) #8
  %4 = call ptr @memcpy(ptr noundef nonnull %2, ptr noundef %3, i32 noundef 2) #8
  %5 = load i16, ptr %2, align 2, !tbaa !18
  %6 = sext i16 %5 to i64
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #9
  ret i64 %6
}

; Function Attrs: nounwind
define dso_local void @chali_store_i16(i64 noundef %0, i64 noundef %1) local_unnamed_addr #3 {
  %3 = alloca i16, align 2
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #9
  %4 = trunc i64 %1 to i16
  store i16 %4, ptr %3, align 2, !tbaa !18
  %5 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 2) #8
  %6 = call ptr @memcpy(ptr noundef %5, ptr noundef nonnull %3, i32 noundef 2) #8
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #9
  ret void
}

; Function Attrs: nounwind
define dso_local i64 @chali_string_new(ptr noundef %0, i32 noundef %1) local_unnamed_addr #3 {
  %3 = alloca [4 x i64], align 8
  %4 = zext i32 %1 to i64
  %5 = icmp eq i32 %1, 0
  %6 = tail call i32 @llvm.umax.i32(i32 %1, i32 1)
  %7 = tail call ptr @malloc(i32 noundef %6) #8
  %8 = icmp eq ptr %7, null
  br i1 %8, label %9, label %10

9:                                                ; preds = %2
  tail call void @chali_mcu_panic(ptr noundef nonnull @.str.5) #8
  unreachable

10:                                               ; preds = %2
  br i1 %5, label %13, label %11

11:                                               ; preds = %10
  %12 = tail call ptr @memcpy(ptr noundef nonnull %7, ptr noundef %0, i32 noundef %1) #8
  br label %13

13:                                               ; preds = %11, %10
  %14 = icmp ult i32 %1, 17
  %15 = select i1 %14, i32 49, i32 32
  %16 = zext nneg i32 %15 to i64
  %17 = tail call i64 @chali_host_allocate(i64 noundef %16) #8
  br i1 %14, label %18, label %20

18:                                               ; preds = %13
  %19 = add nsw i64 %17, 32
  br label %24

20:                                               ; preds = %13
  %21 = add i32 %1, 1
  %22 = zext i32 %21 to i64
  %23 = tail call i64 @chali_host_allocate(i64 noundef %22) #8
  br label %24

24:                                               ; preds = %20, %18
  %25 = phi i64 [ 16, %18 ], [ %4, %20 ]
  %26 = phi i64 [ %19, %18 ], [ %23, %20 ]
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #9
  store i64 -1, ptr %3, align 8, !tbaa !12
  %27 = getelementptr inbounds nuw i8, ptr %3, i32 8
  store i64 %4, ptr %27, align 8, !tbaa !12
  %28 = getelementptr inbounds nuw i8, ptr %3, i32 16
  store i64 %25, ptr %28, align 8, !tbaa !12
  %29 = getelementptr inbounds nuw i8, ptr %3, i32 24
  store i64 %26, ptr %29, align 8, !tbaa !12
  %30 = tail call ptr @chali_host_address(i64 noundef %17, i64 noundef 32) #8
  %31 = call ptr @memcpy(ptr noundef %30, ptr noundef nonnull %3, i32 noundef 32) #8
  br i1 %5, label %35, label %32

32:                                               ; preds = %24
  %33 = call ptr @chali_host_address(i64 noundef %26, i64 noundef %4) #8
  %34 = call ptr @memcpy(ptr noundef %33, ptr noundef nonnull %7, i32 noundef %1) #8
  br label %35

35:                                               ; preds = %32, %24
  %36 = add nsw i64 %26, %4
  %37 = call ptr @chali_host_address(i64 noundef %36, i64 noundef 1) #8
  store i8 0, ptr %37, align 1, !tbaa !16
  call void @free(ptr noundef nonnull %7) #8
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #9
  ret i64 %17
}

declare dso_local ptr @malloc(i32 noundef) local_unnamed_addr #2

declare dso_local void @free(ptr noundef) local_unnamed_addr #2

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define dso_local range(i32 0, 2) i32 @chali_on_heap(i64 noundef %0) local_unnamed_addr #5 {
  %2 = load i64, ptr @chali_data_size, align 8, !tbaa !12
  %3 = icmp sge i64 %0, %2
  %4 = zext i1 %3 to i32
  ret i32 %4
}

; Function Attrs: nounwind
define dso_local i64 @chali_string_length(i64 noundef %0) local_unnamed_addr #3 {
  %2 = alloca i64, align 8
  %3 = alloca i64, align 8
  %4 = icmp eq i64 %0, 0
  br i1 %4, label %22, label %5

5:                                                ; preds = %1
  %6 = load i64, ptr @chali_data_size, align 8, !tbaa !12
  %7 = icmp slt i64 %0, %6
  br i1 %7, label %8, label %12

8:                                                ; preds = %5
  %9 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #8
  %10 = tail call i32 @strlen(ptr noundef %9) #8
  %11 = zext i32 %10 to i64
  br label %22

12:                                               ; preds = %5
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #9
  store i64 0, ptr %3, align 8, !tbaa !12
  %13 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #8
  %14 = call ptr @memcpy(ptr noundef nonnull %3, ptr noundef %13, i32 noundef 8) #8
  %15 = load i64, ptr %3, align 8, !tbaa !12
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #9
  %16 = icmp eq i64 %15, -1
  br i1 %16, label %17, label %22

17:                                               ; preds = %12
  %18 = add nsw i64 %0, 8
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #9
  store i64 0, ptr %2, align 8, !tbaa !12
  %19 = call ptr @chali_host_address(i64 noundef %18, i64 noundef 8) #8
  %20 = call ptr @memcpy(ptr noundef nonnull %2, ptr noundef %19, i32 noundef 8) #8
  %21 = load i64, ptr %2, align 8, !tbaa !12
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #9
  br label %22

22:                                               ; preds = %17, %12, %1, %8
  %23 = phi i64 [ %11, %8 ], [ 0, %1 ], [ %21, %17 ], [ %15, %12 ]
  ret i64 %23
}

declare dso_local i32 @strlen(ptr noundef) local_unnamed_addr #2

; Function Attrs: nounwind
define dso_local ptr @chali_string_bytes(i64 noundef %0) local_unnamed_addr #3 {
  %2 = alloca i64, align 8
  %3 = alloca i64, align 8
  %4 = load i64, ptr @chali_data_size, align 8, !tbaa !12
  %5 = icmp slt i64 %0, %4
  br i1 %5, label %18, label %6

6:                                                ; preds = %1
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #9
  store i64 0, ptr %3, align 8, !tbaa !12
  %7 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #8
  %8 = call ptr @memcpy(ptr noundef nonnull %3, ptr noundef %7, i32 noundef 8) #8
  %9 = load i64, ptr %3, align 8, !tbaa !12
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #9
  %10 = icmp eq i64 %9, -1
  br i1 %10, label %11, label %16

11:                                               ; preds = %6
  %12 = add nsw i64 %0, 24
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #9
  store i64 0, ptr %2, align 8, !tbaa !12
  %13 = call ptr @chali_host_address(i64 noundef %12, i64 noundef 8) #8
  %14 = call ptr @memcpy(ptr noundef nonnull %2, ptr noundef %13, i32 noundef 8) #8
  %15 = load i64, ptr %2, align 8, !tbaa !12
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #9
  br label %18

16:                                               ; preds = %6
  %17 = add nsw i64 %0, 8
  br label %18

18:                                               ; preds = %1, %11, %16
  %19 = phi i64 [ %17, %16 ], [ %15, %11 ], [ %0, %1 ]
  %20 = call ptr @chali_host_address(i64 noundef %19, i64 noundef 0) #8
  ret ptr %20
}

; Function Attrs: nounwind
define dso_local i64 @chali_string_literal(i64 noundef %0) local_unnamed_addr #3 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #8
  %3 = tail call i32 @strlen(ptr noundef %2) #8
  %4 = tail call i64 @chali_string_new(ptr noundef %2, i32 noundef %3) #10
  ret i64 %4
}

; Function Attrs: nounwind
define dso_local i64 @chali_string_copy(i64 noundef %0) local_unnamed_addr #3 {
  %2 = icmp eq i64 %0, 0
  br i1 %2, label %8, label %3

3:                                                ; preds = %1
  %4 = tail call ptr @chali_string_bytes(i64 noundef %0) #10
  %5 = tail call i64 @chali_string_length(i64 noundef %0) #10
  %6 = trunc i64 %5 to i32
  %7 = tail call i64 @chali_string_new(ptr noundef %4, i32 noundef %6) #10
  br label %8

8:                                                ; preds = %1, %3
  %9 = phi i64 [ %7, %3 ], [ 0, %1 ]
  ret i64 %9
}

; Function Attrs: nounwind
define dso_local void @chali_string_drop(i64 noundef %0) local_unnamed_addr #3 {
  %2 = alloca i64, align 8
  %3 = alloca i64, align 8
  %4 = icmp eq i64 %0, 0
  %5 = load i64, ptr @chali_data_size, align 8
  %6 = icmp slt i64 %0, %5
  %7 = select i1 %4, i1 true, i1 %6
  br i1 %7, label %21, label %8

8:                                                ; preds = %1
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #9
  store i64 0, ptr %3, align 8, !tbaa !12
  %9 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #8
  %10 = call ptr @memcpy(ptr noundef nonnull %3, ptr noundef %9, i32 noundef 8) #8
  %11 = load i64, ptr %3, align 8, !tbaa !12
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #9
  %12 = icmp eq i64 %11, -1
  br i1 %12, label %13, label %21

13:                                               ; preds = %8
  %14 = add nsw i64 %0, 24
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #9
  store i64 0, ptr %2, align 8, !tbaa !12
  %15 = call ptr @chali_host_address(i64 noundef %14, i64 noundef 8) #8
  %16 = call ptr @memcpy(ptr noundef nonnull %2, ptr noundef %15, i32 noundef 8) #8
  %17 = load i64, ptr %2, align 8, !tbaa !12
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #9
  %18 = add nsw i64 %0, 32
  %19 = icmp eq i64 %17, %18
  br i1 %19, label %21, label %20

20:                                               ; preds = %13
  call void @chali_host_drop(i64 noundef %17) #8
  br label %21

21:                                               ; preds = %13, %20, %8, %1
  call void @chali_host_drop(i64 noundef %0) #8
  ret void
}

; Function Attrs: nounwind
define dso_local range(i64 -9223372036854775800, -9223372036854775808) i64 @chali_new_array(i64 noundef %0, i64 noundef %1) local_unnamed_addr #3 {
  %3 = alloca i64, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #9
  %4 = tail call i64 @llvm.smax.i64(i64 %0, i64 0)
  store i64 %4, ptr %3, align 8, !tbaa !12
  %5 = mul i64 %4, %1
  %6 = add i64 %5, 8
  %7 = tail call i64 @chali_host_allocate(i64 noundef %6) #8
  %8 = tail call ptr @chali_host_address(i64 noundef %7, i64 noundef 8) #8
  %9 = call ptr @memcpy(ptr noundef %8, ptr noundef nonnull %3, i32 noundef 8) #8
  %10 = add nsw i64 %7, 8
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #9
  ret i64 %10
}

; Function Attrs: nounwind
define dso_local void @chali_bounds(i64 noundef %0, i64 noundef %1, ptr noundef %2) local_unnamed_addr #3 {
  %4 = alloca [128 x i8], align 1
  %5 = icmp sgt i64 %0, -1
  %6 = icmp slt i64 %0, %1
  %7 = and i1 %5, %6
  br i1 %7, label %10, label %8

8:                                                ; preds = %3
  call void @llvm.lifetime.start.p0(ptr nonnull %4) #9
  %9 = call i32 (ptr, i32, ptr, ...) @snprintf(ptr noundef nonnull %4, i32 noundef 128, ptr noundef nonnull @.str.6, ptr noundef %2, i64 noundef %0, i64 noundef %1) #8
  call void @chali_mcu_panic(ptr noundef nonnull %4) #8
  unreachable

10:                                               ; preds = %3
  ret void
}

declare dso_local i32 @snprintf(ptr noundef, i32 noundef, ptr noundef, ...) local_unnamed_addr #2

; Function Attrs: nounwind
define dso_local void @chali_null(i64 noundef %0, ptr noundef %1) local_unnamed_addr #3 {
  %3 = alloca [128 x i8], align 1
  %4 = icmp eq i64 %0, 0
  br i1 %4, label %5, label %7

5:                                                ; preds = %2
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #9
  %6 = call i32 (ptr, i32, ptr, ...) @snprintf(ptr noundef nonnull %3, i32 noundef 128, ptr noundef nonnull @.str.7, ptr noundef %1) #8
  call void @chali_mcu_panic(ptr noundef nonnull %3) #8
  unreachable

7:                                                ; preds = %2
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none, target_mem: none)
define dso_local void @chali_fmt_begin(i64 noundef %0) local_unnamed_addr #6 {
  store i64 %0, ptr @chali_fmt_at, align 8, !tbaa !12
  store i32 0, ptr @chali_fmt_pos, align 4, !tbaa !17
  ret void
}

; Function Attrs: nounwind
define dso_local range(i32 0, 256) i32 @chali_fmt_scan(ptr nofree noundef writeonly captures(none) %0, i32 noundef %1) local_unnamed_addr #3 {
  %3 = load i64, ptr @chali_fmt_at, align 8, !tbaa !12
  %4 = tail call ptr @chali_string_bytes(i64 noundef %3) #10
  %5 = load i32, ptr @chali_fmt_pos, align 4, !tbaa !17
  br label %6

6:                                                ; preds = %14, %2
  %7 = phi i32 [ %5, %2 ], [ %15, %14 ]
  %8 = getelementptr inbounds nuw i8, ptr %4, i32 %7
  %9 = load i8, ptr %8, align 1, !tbaa !16
  switch i8 %9, label %14 [
    i8 0, label %16
    i8 37, label %16
    i8 123, label %10
  ]

10:                                               ; preds = %6
  %11 = getelementptr i8, ptr %8, i32 1
  %12 = load i8, ptr %11, align 1, !tbaa !16
  %13 = icmp eq i8 %12, 125
  br i1 %13, label %16, label %14

14:                                               ; preds = %6, %10
  %15 = add i32 %7, 1
  br label %6, !llvm.loop !20

16:                                               ; preds = %6, %6, %10
  %17 = icmp ugt i32 %7, %5
  br i1 %17, label %18, label %23

18:                                               ; preds = %16
  %19 = getelementptr inbounds nuw i8, ptr %4, i32 %5
  %20 = sub nuw i32 %7, %5
  %21 = load ptr, ptr @stdout, align 4, !tbaa !9
  %22 = tail call i32 @fwrite(ptr noundef %19, i32 noundef 1, i32 noundef %20, ptr noundef %21) #8
  store i32 %7, ptr @chali_fmt_pos, align 4, !tbaa !17
  br label %23

23:                                               ; preds = %18, %16
  %24 = load i8, ptr %8, align 1, !tbaa !16
  switch i8 %24, label %69 [
    i8 0, label %25
    i8 123, label %26
  ]

25:                                               ; preds = %23
  store i8 0, ptr %0, align 1, !tbaa !16
  br label %127

26:                                               ; preds = %23
  %27 = add i32 %7, 1
  br label %28

28:                                               ; preds = %32, %26
  %29 = phi i32 [ %27, %26 ], [ %33, %32 ]
  %30 = getelementptr inbounds nuw i8, ptr %4, i32 %29
  %31 = load i8, ptr %30, align 1, !tbaa !16
  switch i8 %31, label %32 [
    i8 0, label %34
    i8 125, label %34
  ]

32:                                               ; preds = %28
  %33 = add i32 %29, 1
  br label %28, !llvm.loop !23

34:                                               ; preds = %28, %28
  %35 = getelementptr inbounds nuw i8, ptr %4, i32 %29
  %36 = icmp eq i8 %31, 125
  %37 = zext i1 %36 to i32
  %38 = add i32 %29, %37
  store i32 %38, ptr @chali_fmt_pos, align 4, !tbaa !17
  store i8 37, ptr %0, align 1, !tbaa !16
  %39 = getelementptr inbounds nuw i8, ptr %0, i32 1
  store i8 0, ptr %39, align 1, !tbaa !16
  %40 = getelementptr inbounds nuw i8, ptr %4, i32 %27
  %41 = load i8, ptr %40, align 1, !tbaa !16
  %42 = icmp eq i8 %41, 58
  %43 = add i32 %7, 2
  %44 = select i1 %42, i32 %43, i32 %27
  %45 = icmp ugt i32 %29, %44
  br i1 %45, label %46, label %127

46:                                               ; preds = %34
  %47 = add i32 %44, 1
  %48 = icmp ult i32 %47, %29
  %49 = icmp ugt i32 %1, 3
  %50 = and i1 %48, %49
  br i1 %50, label %56, label %51

51:                                               ; preds = %56, %46
  %52 = phi i32 [ 1, %46 ], [ %62, %56 ]
  %53 = getelementptr inbounds nuw i8, ptr %0, i32 %52
  store i8 0, ptr %53, align 1, !tbaa !16
  %54 = getelementptr i8, ptr %35, i32 -1
  %55 = load i8, ptr %54, align 1, !tbaa !16
  br label %127

56:                                               ; preds = %46, %56
  %57 = phi i32 [ %64, %56 ], [ %47, %46 ]
  %58 = phi i32 [ %57, %56 ], [ %44, %46 ]
  %59 = phi i32 [ %62, %56 ], [ 1, %46 ]
  %60 = getelementptr inbounds nuw i8, ptr %4, i32 %58
  %61 = load i8, ptr %60, align 1, !tbaa !16
  %62 = add nuw i32 %59, 1
  %63 = getelementptr inbounds nuw i8, ptr %0, i32 %59
  store i8 %61, ptr %63, align 1, !tbaa !16
  %64 = add nuw i32 %57, 1
  %65 = icmp ult i32 %64, %29
  %66 = add nuw i32 %59, 3
  %67 = icmp ult i32 %66, %1
  %68 = and i1 %65, %67
  br i1 %68, label %56, label %51, !llvm.loop !24

69:                                               ; preds = %23
  %70 = add i32 %7, 1
  %71 = getelementptr inbounds nuw i8, ptr %4, i32 %70
  %72 = load i8, ptr %71, align 1, !tbaa !16
  %73 = icmp eq i8 %72, 37
  br i1 %73, label %74, label %78

74:                                               ; preds = %69
  %75 = load ptr, ptr @stdout, align 4, !tbaa !9
  %76 = tail call i32 @fputc(i32 noundef 37, ptr noundef %75) #8
  %77 = add i32 %7, 2
  store i32 %77, ptr @chali_fmt_pos, align 4, !tbaa !17
  store i8 0, ptr %0, align 1, !tbaa !16
  br label %127

78:                                               ; preds = %69
  store i8 37, ptr %0, align 1, !tbaa !16
  %79 = getelementptr inbounds nuw i8, ptr %4, i32 %70
  %80 = load i8, ptr %79, align 1, !tbaa !16
  %81 = icmp eq i8 %80, 0
  br i1 %81, label %101, label %82

82:                                               ; preds = %78, %93
  %83 = phi i8 [ %99, %93 ], [ %80, %78 ]
  %84 = phi ptr [ %98, %93 ], [ %79, %78 ]
  %85 = phi i32 [ %96, %93 ], [ 1, %78 ]
  %86 = phi i32 [ %94, %93 ], [ %70, %78 ]
  %87 = zext i8 %83 to i32
  %88 = tail call ptr @strchr(ptr noundef nonnull @.str.8, i32 noundef %87) #8
  %89 = icmp ne ptr %88, null
  %90 = add nuw i32 %85, 2
  %91 = icmp ult i32 %90, %1
  %92 = select i1 %89, i1 %91, i1 false
  br i1 %92, label %93, label %101

93:                                               ; preds = %82
  %94 = add i32 %86, 1
  %95 = load i8, ptr %84, align 1, !tbaa !16
  %96 = add nuw i32 %85, 1
  %97 = getelementptr inbounds nuw i8, ptr %0, i32 %85
  store i8 %95, ptr %97, align 1, !tbaa !16
  %98 = getelementptr inbounds nuw i8, ptr %4, i32 %94
  %99 = load i8, ptr %98, align 1, !tbaa !16
  %100 = icmp eq i8 %99, 0
  br i1 %100, label %101, label %82, !llvm.loop !25

101:                                              ; preds = %93, %82, %78
  %102 = phi i32 [ %70, %78 ], [ %94, %93 ], [ %86, %82 ]
  %103 = phi i32 [ 1, %78 ], [ %96, %93 ], [ %85, %82 ]
  %104 = getelementptr inbounds nuw i8, ptr %4, i32 %102
  %105 = load i8, ptr %104, align 1, !tbaa !16
  %106 = icmp eq i8 %105, 0
  br i1 %106, label %118, label %107

107:                                              ; preds = %101, %113
  %108 = phi i8 [ %116, %113 ], [ %105, %101 ]
  %109 = phi i32 [ %114, %113 ], [ %102, %101 ]
  %110 = zext i8 %108 to i32
  %111 = tail call ptr @strchr(ptr noundef nonnull @.str.9, i32 noundef %110) #8
  %112 = icmp eq ptr %111, null
  br i1 %112, label %118, label %113

113:                                              ; preds = %107
  %114 = add i32 %109, 1
  %115 = getelementptr inbounds nuw i8, ptr %4, i32 %114
  %116 = load i8, ptr %115, align 1, !tbaa !16
  %117 = icmp eq i8 %116, 0
  br i1 %117, label %118, label %107, !llvm.loop !26

118:                                              ; preds = %107, %113, %101
  %119 = phi i32 [ %102, %101 ], [ %109, %107 ], [ %114, %113 ]
  %120 = getelementptr inbounds nuw i8, ptr %4, i32 %119
  %121 = getelementptr inbounds nuw i8, ptr %0, i32 %103
  store i8 0, ptr %121, align 1, !tbaa !16
  %122 = load i8, ptr %120, align 1, !tbaa !16
  %123 = icmp eq i8 %122, 0
  br i1 %123, label %124, label %125

124:                                              ; preds = %118
  store i32 %119, ptr @chali_fmt_pos, align 4, !tbaa !17
  br label %127

125:                                              ; preds = %118
  %126 = add i32 %119, 1
  store i32 %126, ptr @chali_fmt_pos, align 4, !tbaa !17
  br label %127

127:                                              ; preds = %74, %125, %124, %51, %34, %25
  %128 = phi i8 [ 0, %25 ], [ 63, %34 ], [ %55, %51 ], [ 37, %74 ], [ 0, %124 ], [ %122, %125 ]
  %129 = zext i8 %128 to i32
  ret i32 %129
}

declare dso_local i32 @fwrite(ptr noundef, i32 noundef, i32 noundef, ptr noundef) local_unnamed_addr #2

declare dso_local i32 @fputc(i32 noundef, ptr noundef) local_unnamed_addr #2

declare dso_local ptr @strchr(ptr noundef, i32 noundef) local_unnamed_addr #2

; Function Attrs: nounwind
define dso_local void @chali_fmt_emit(i32 noundef %0, ptr noundef %1, i64 noundef %2, i32 noundef %3) local_unnamed_addr #3 {
  %5 = alloca i64, align 8
  %6 = alloca [64 x i8], align 1
  %7 = alloca [40 x i8], align 1
  %8 = alloca double, align 8
  store i64 %2, ptr %5, align 8, !tbaa !12
  call void @llvm.lifetime.start.p0(ptr nonnull %6) #9
  call void @llvm.lifetime.start.p0(ptr nonnull %7) #9
  switch i32 %0, label %27 [
    i32 115, label %9
    i32 99, label %19
    i32 63, label %24
  ]

9:                                                ; preds = %4
  %10 = icmp eq i64 %2, 0
  br i1 %10, label %54, label %11

11:                                               ; preds = %9
  %12 = call i32 (ptr, i32, ptr, ...) @snprintf(ptr noundef nonnull %6, i32 noundef 64, ptr noundef nonnull @.str.10, ptr noundef %1) #8
  %13 = load i64, ptr %5, align 8, !tbaa !12
  %14 = call i64 @chali_string_length(i64 noundef %13) #10
  %15 = trunc i64 %14 to i32
  %16 = load i64, ptr %5, align 8, !tbaa !12
  %17 = call ptr @chali_string_bytes(i64 noundef %16) #10
  %18 = call i32 (ptr, ...) @printf(ptr noundef nonnull %6, i32 noundef %15, ptr noundef %17) #8
  br label %54

19:                                               ; preds = %4
  %20 = trunc i64 %2 to i32
  %21 = and i32 %20, 255
  %22 = load ptr, ptr @stdout, align 4, !tbaa !9
  %23 = tail call i32 @fputc(i32 noundef %21, ptr noundef %22) #8
  br label %54

24:                                               ; preds = %4
  %25 = icmp eq i32 %3, 0
  %26 = select i1 %25, i32 100, i32 103
  br label %27

27:                                               ; preds = %4, %24
  %28 = phi i32 [ %26, %24 ], [ %0, %4 ]
  %29 = tail call ptr @strchr(ptr noundef nonnull @.str.11, i32 noundef %28) #8
  %30 = icmp eq ptr %29, null
  br i1 %30, label %45, label %31

31:                                               ; preds = %27
  call void @llvm.lifetime.start.p0(ptr nonnull %8) #9
  store double 0.000000e+00, ptr %8, align 8, !tbaa !14
  %32 = call ptr @memcpy(ptr noundef nonnull %8, ptr noundef nonnull %5, i32 noundef 8) #8
  %33 = call i32 (ptr, i32, ptr, ...) @snprintf(ptr noundef nonnull %7, i32 noundef 40, ptr noundef nonnull @.str.12, ptr noundef %1) #8
  %34 = call ptr @strchr(ptr noundef nonnull %7, i32 noundef 46) #8
  %35 = icmp eq ptr %34, null
  br i1 %35, label %36, label %40

36:                                               ; preds = %31
  %37 = call i32 @strlen(ptr noundef nonnull %7) #8
  %38 = sub i32 39, %37
  %39 = call ptr @strncat(ptr noundef nonnull %7, ptr noundef nonnull @.str.13, i32 noundef %38) #8
  br label %40

40:                                               ; preds = %36, %31
  %41 = and i32 %28, 255
  %42 = call i32 (ptr, i32, ptr, ...) @snprintf(ptr noundef nonnull %6, i32 noundef 64, ptr noundef nonnull @.str.14, ptr noundef nonnull %7, i32 noundef %41) #8
  %43 = load double, ptr %8, align 8, !tbaa !14
  %44 = call i32 (ptr, ...) @printf(ptr noundef nonnull %6, double noundef %43) #8
  call void @llvm.lifetime.end.p0(ptr nonnull %8) #9
  br label %54

45:                                               ; preds = %27
  switch i32 %28, label %50 [
    i32 120, label %46
    i32 117, label %46
    i32 111, label %46
    i32 88, label %46
  ]

46:                                               ; preds = %45, %45, %45, %45
  %47 = call i32 (ptr, i32, ptr, ...) @snprintf(ptr noundef nonnull %6, i32 noundef 64, ptr noundef nonnull @.str.15, ptr noundef %1, i32 noundef %28) #8
  %48 = load i64, ptr %5, align 8, !tbaa !12
  %49 = call i32 (ptr, ...) @printf(ptr noundef nonnull %6, i64 noundef %48) #8
  br label %54

50:                                               ; preds = %45
  %51 = call i32 (ptr, i32, ptr, ...) @snprintf(ptr noundef nonnull %6, i32 noundef 64, ptr noundef nonnull @.str.16, ptr noundef %1) #8
  %52 = load i64, ptr %5, align 8, !tbaa !12
  %53 = call i32 (ptr, ...) @printf(ptr noundef nonnull %6, i64 noundef %52) #8
  br label %54

54:                                               ; preds = %9, %50, %46, %40, %19, %11
  call void @llvm.lifetime.end.p0(ptr nonnull %7) #9
  call void @llvm.lifetime.end.p0(ptr nonnull %6) #9
  ret void
}

declare dso_local i32 @printf(ptr noundef, ...) local_unnamed_addr #2

declare dso_local ptr @strncat(ptr noundef, ptr noundef, i32 noundef) local_unnamed_addr #2

; Function Attrs: nounwind
define dso_local void @chali_fmt_int(i64 noundef %0) local_unnamed_addr #3 {
  %2 = alloca [32 x i8], align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #9
  %3 = call i32 @chali_fmt_scan(ptr noundef nonnull %2, i32 noundef 32) #10
  %4 = trunc nuw i32 %3 to i8
  switch i8 %4, label %5 [
    i8 37, label %6
    i8 0, label %6
  ]

5:                                                ; preds = %1
  call void @chali_fmt_emit(i32 noundef %3, ptr noundef nonnull %2, i64 noundef %0, i32 noundef 0) #10
  br label %6

6:                                                ; preds = %1, %1, %5
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #9
  ret void
}

; Function Attrs: nounwind
define dso_local void @chali_fmt_uint(i64 noundef %0) local_unnamed_addr #3 {
  %2 = alloca [32 x i8], align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #9
  %3 = call i32 @chali_fmt_scan(ptr noundef nonnull %2, i32 noundef 32) #10
  %4 = trunc nuw i32 %3 to i8
  switch i8 %4, label %5 [
    i8 37, label %6
    i8 0, label %6
  ]

5:                                                ; preds = %1
  call void @chali_fmt_emit(i32 noundef %3, ptr noundef nonnull %2, i64 noundef %0, i32 noundef 0) #10
  br label %6

6:                                                ; preds = %1, %1, %5
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #9
  ret void
}

; Function Attrs: nounwind
define dso_local void @chali_fmt_char(i64 noundef %0) local_unnamed_addr #3 {
  %2 = alloca [32 x i8], align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #9
  %3 = call i32 @chali_fmt_scan(ptr noundef nonnull %2, i32 noundef 32) #10
  %4 = trunc nuw i32 %3 to i8
  switch i8 %4, label %5 [
    i8 37, label %6
    i8 0, label %6
  ]

5:                                                ; preds = %1
  call void @chali_fmt_emit(i32 noundef %3, ptr noundef nonnull %2, i64 noundef %0, i32 noundef 0) #10
  br label %6

6:                                                ; preds = %1, %1, %5
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #9
  ret void
}

; Function Attrs: nounwind
define dso_local void @chali_fmt_double(double noundef %0) local_unnamed_addr #3 {
  %2 = alloca double, align 8
  %3 = alloca [32 x i8], align 1
  %4 = alloca i64, align 8
  store double %0, ptr %2, align 8, !tbaa !14
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #9
  call void @llvm.lifetime.start.p0(ptr nonnull %4) #9
  store i64 0, ptr %4, align 8, !tbaa !12
  %5 = call i32 @chali_fmt_scan(ptr noundef nonnull %3, i32 noundef 32) #10
  %6 = trunc nuw i32 %5 to i8
  switch i8 %6, label %7 [
    i8 37, label %10
    i8 0, label %10
  ]

7:                                                ; preds = %1
  %8 = call ptr @memcpy(ptr noundef nonnull %4, ptr noundef nonnull %2, i32 noundef 8) #8
  %9 = load i64, ptr %4, align 8, !tbaa !12
  call void @chali_fmt_emit(i32 noundef %5, ptr noundef nonnull %3, i64 noundef %9, i32 noundef 1) #10
  br label %10

10:                                               ; preds = %1, %1, %7
  call void @llvm.lifetime.end.p0(ptr nonnull %4) #9
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #9
  ret void
}

; Function Attrs: nounwind
define dso_local void @chali_fmt_string(i64 noundef %0) local_unnamed_addr #3 {
  %2 = alloca [32 x i8], align 1
  %3 = alloca [48 x i8], align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #9
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #9
  %4 = call i32 @chali_fmt_scan(ptr noundef nonnull %2, i32 noundef 32) #10
  %5 = icmp eq i64 %0, 0
  br i1 %5, label %14, label %6

6:                                                ; preds = %1
  %7 = trunc nuw i32 %4 to i8
  switch i8 %7, label %8 [
    i8 37, label %14
    i8 0, label %14
  ]

8:                                                ; preds = %6
  %9 = tail call ptr @chali_string_bytes(i64 noundef %0) #10
  %10 = tail call i64 @chali_string_length(i64 noundef %0) #10
  %11 = trunc i64 %10 to i32
  %12 = call i32 (ptr, i32, ptr, ...) @snprintf(ptr noundef nonnull %3, i32 noundef 48, ptr noundef nonnull @.str.10, ptr noundef nonnull %2) #8
  %13 = call i32 (ptr, ...) @printf(ptr noundef nonnull %3, i32 noundef %11, ptr noundef %9) #8
  br label %14

14:                                               ; preds = %1, %6, %6, %8
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #9
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #9
  ret void
}

; Function Attrs: nounwind
define dso_local void @chali_fmt_finish() local_unnamed_addr #3 {
  %1 = load i64, ptr @chali_fmt_at, align 8, !tbaa !12
  %2 = tail call ptr @chali_string_bytes(i64 noundef %1) #10
  %3 = load i32, ptr @chali_fmt_pos, align 4, !tbaa !17
  br label %4

4:                                                ; preds = %12, %0
  %5 = phi i32 [ %3, %0 ], [ %17, %12 ]
  %6 = getelementptr inbounds nuw i8, ptr %2, i32 %5
  %7 = load i8, ptr %6, align 1, !tbaa !16
  switch i8 %7, label %18 [
    i8 0, label %20
    i8 37, label %8
  ]

8:                                                ; preds = %4
  %9 = getelementptr i8, ptr %6, i32 1
  %10 = load i8, ptr %9, align 1, !tbaa !16
  %11 = icmp eq i8 %10, 37
  br i1 %11, label %12, label %18

12:                                               ; preds = %8, %18
  %13 = phi i32 [ %19, %18 ], [ 37, %8 ]
  %14 = phi i32 [ 1, %18 ], [ 2, %8 ]
  %15 = load ptr, ptr @stdout, align 4, !tbaa !9
  %16 = tail call i32 @fputc(i32 noundef %13, ptr noundef %15) #8
  %17 = add i32 %5, %14
  br label %4, !llvm.loop !27

18:                                               ; preds = %4, %8
  %19 = zext i8 %7 to i32
  br label %12

20:                                               ; preds = %4
  store i32 %5, ptr @chali_fmt_pos, align 4, !tbaa !17
  ret void
}

; Function Attrs: nounwind
define dso_local range(i64 -1, 2) i64 @chali_string_cmp(i64 noundef %0, i64 noundef %1) local_unnamed_addr #3 {
  %3 = icmp eq i64 %0, 0
  br i1 %3, label %7, label %4

4:                                                ; preds = %2
  %5 = tail call i64 @chali_string_length(i64 noundef %0) #10
  %6 = trunc i64 %5 to i32
  br label %7

7:                                                ; preds = %2, %4
  %8 = phi i32 [ %6, %4 ], [ 0, %2 ]
  %9 = icmp eq i64 %1, 0
  br i1 %9, label %13, label %10

10:                                               ; preds = %7
  %11 = tail call i64 @chali_string_length(i64 noundef %1) #10
  %12 = trunc i64 %11 to i32
  br label %13

13:                                               ; preds = %7, %10
  %14 = phi i32 [ %12, %10 ], [ 0, %7 ]
  %15 = tail call i32 @llvm.umin.i32(i32 %8, i32 %14)
  %16 = icmp eq i32 %15, 0
  br i1 %16, label %21, label %17

17:                                               ; preds = %13
  %18 = tail call ptr @chali_string_bytes(i64 noundef %0) #10
  %19 = tail call ptr @chali_string_bytes(i64 noundef %1) #10
  %20 = tail call i32 @memcmp(ptr noundef %18, ptr noundef %19, i32 noundef %15) #8
  br label %21

21:                                               ; preds = %13, %17
  %22 = phi i32 [ %20, %17 ], [ 0, %13 ]
  %23 = icmp eq i32 %22, 0
  %24 = icmp sgt i32 %22, -1
  %25 = select i1 %24, i32 1, i32 -1
  %26 = tail call i32 @llvm.ucmp.i32.i32(i32 %8, i32 %14)
  %27 = select i1 %23, i32 %26, i32 %25
  %28 = sext i32 %27 to i64
  ret i64 %28
}

declare dso_local i32 @memcmp(ptr noundef, ptr noundef, i32 noundef) local_unnamed_addr #2

; Function Attrs: nounwind
define dso_local i64 @chali_string_sub(i64 noundef %0, i64 noundef %1, i64 noundef %2) local_unnamed_addr #3 {
  %4 = icmp eq i64 %0, 0
  br i1 %4, label %5, label %7

5:                                                ; preds = %3
  %6 = tail call i64 @chali_string_new(ptr noundef nonnull @.str.17, i32 noundef 0) #10
  br label %20

7:                                                ; preds = %3
  %8 = tail call i64 @chali_string_length(i64 noundef %0) #10
  %9 = tail call i64 @llvm.smax.i64(i64 %1, i64 0)
  %10 = tail call i64 @llvm.smax.i64(i64 %2, i64 0)
  %11 = tail call i64 @llvm.smin.i64(i64 %9, i64 %8)
  %12 = sub nsw i64 %8, %11
  %13 = icmp sgt i64 %2, %12
  %14 = select i1 %13, i64 %12, i64 %10
  %15 = trunc i64 %14 to i32
  %16 = tail call ptr @chali_string_bytes(i64 noundef %0) #10
  %17 = trunc i64 %11 to i32
  %18 = getelementptr inbounds i8, ptr %16, i32 %17
  %19 = tail call i64 @chali_string_new(ptr noundef %18, i32 noundef %15) #10
  br label %20

20:                                               ; preds = %7, %5
  %21 = phi i64 [ %6, %5 ], [ %19, %7 ]
  ret i64 %21
}

; Function Attrs: nounwind
define dso_local i64 @chali_string_pos(i64 noundef %0, i64 noundef %1) local_unnamed_addr #3 {
  %3 = icmp eq i64 %0, 0
  %4 = icmp eq i64 %1, 0
  %5 = or i1 %3, %4
  br i1 %5, label %26, label %6

6:                                                ; preds = %2
  %7 = tail call i64 @chali_string_length(i64 noundef %0) #10
  %8 = tail call i64 @chali_string_length(i64 noundef %1) #10
  %9 = icmp eq i64 %8, 0
  br i1 %9, label %26, label %10

10:                                               ; preds = %6
  %11 = icmp sgt i64 %8, %7
  br i1 %11, label %26, label %12

12:                                               ; preds = %10
  %13 = trunc i64 %8 to i32
  br label %14

14:                                               ; preds = %12, %22
  %15 = phi i64 [ 0, %12 ], [ %23, %22 ]
  %16 = tail call ptr @chali_string_bytes(i64 noundef %0) #10
  %17 = trunc i64 %15 to i32
  %18 = getelementptr inbounds i8, ptr %16, i32 %17
  %19 = tail call ptr @chali_string_bytes(i64 noundef %1) #10
  %20 = tail call i32 @memcmp(ptr noundef %18, ptr noundef %19, i32 noundef %13) #8
  %21 = icmp eq i32 %20, 0
  br i1 %21, label %26, label %22

22:                                               ; preds = %14
  %23 = add nuw nsw i64 %15, 1
  %24 = add nsw i64 %23, %8
  %25 = icmp sgt i64 %24, %7
  br i1 %25, label %26, label %14, !llvm.loop !28

26:                                               ; preds = %14, %22, %10, %6, %2
  %27 = phi i64 [ -1, %2 ], [ 0, %6 ], [ -1, %10 ], [ -1, %22 ], [ %15, %14 ]
  ret i64 %27
}

; Function Attrs: nounwind
define dso_local i64 @chali_read_line() local_unnamed_addr #3 {
  %1 = tail call ptr @malloc(i32 noundef 256) #8
  %2 = icmp eq ptr %1, null
  br i1 %2, label %3, label %4

3:                                                ; preds = %0
  tail call void @chali_mcu_panic(ptr noundef nonnull @.str.18) #8
  unreachable

4:                                                ; preds = %0, %21
  %5 = phi ptr [ %22, %21 ], [ %1, %0 ]
  %6 = phi i32 [ %25, %21 ], [ 0, %0 ]
  %7 = phi i32 [ %23, %21 ], [ 256, %0 ]
  %8 = load ptr, ptr @stdin, align 4, !tbaa !9
  %9 = tail call i32 @fgetc(ptr noundef %8) #8
  switch i32 %9, label %11 [
    i32 -1, label %27
    i32 10, label %27
    i32 0, label %10
  ]

10:                                               ; preds = %4
  tail call void @free(ptr noundef %5) #8
  tail call void @chali_mcu_panic(ptr noundef nonnull @.str.19) #8
  unreachable

11:                                               ; preds = %4
  %12 = icmp eq i32 %6, %7
  br i1 %12, label %13, label %21

13:                                               ; preds = %11
  %14 = icmp slt i32 %7, 0
  br i1 %14, label %15, label %16

15:                                               ; preds = %13
  tail call void @chali_mcu_panic(ptr noundef nonnull @.str.20) #8
  unreachable

16:                                               ; preds = %13
  %17 = shl nuw i32 %7, 1
  %18 = tail call ptr @realloc(ptr noundef %5, i32 noundef %17) #8
  %19 = icmp eq ptr %18, null
  br i1 %19, label %20, label %21

20:                                               ; preds = %16
  tail call void @free(ptr noundef %5) #8
  tail call void @chali_mcu_panic(ptr noundef nonnull @.str.18) #8
  unreachable

21:                                               ; preds = %16, %11
  %22 = phi ptr [ %5, %11 ], [ %18, %16 ]
  %23 = phi i32 [ %7, %11 ], [ %17, %16 ]
  %24 = trunc i32 %9 to i8
  %25 = add i32 %6, 1
  %26 = getelementptr inbounds nuw i8, ptr %22, i32 %6
  store i8 %24, ptr %26, align 1, !tbaa !16
  br label %4, !llvm.loop !29

27:                                               ; preds = %4, %4
  %28 = icmp eq i32 %9, 10
  %29 = icmp ne i32 %6, 0
  %30 = select i1 %28, i1 %29, i1 false
  br i1 %30, label %31, label %38

31:                                               ; preds = %27
  %32 = getelementptr i8, ptr %5, i32 %6
  %33 = getelementptr i8, ptr %32, i32 -1
  %34 = load i8, ptr %33, align 1, !tbaa !16
  %35 = icmp eq i8 %34, 13
  %36 = sext i1 %35 to i32
  %37 = add i32 %6, %36
  br label %38

38:                                               ; preds = %31, %27
  %39 = phi i32 [ %6, %27 ], [ %37, %31 ]
  %40 = tail call i64 @chali_string_new(ptr noundef %5, i32 noundef %39) #10
  tail call void @free(ptr noundef %5) #8
  ret i64 %40
}

declare dso_local i32 @fgetc(ptr noundef) local_unnamed_addr #2

declare dso_local ptr @realloc(ptr noundef, i32 noundef) local_unnamed_addr #2

; Function Attrs: nounwind
define dso_local i64 @chali_string_fixed(double noundef %0, i64 noundef %1) local_unnamed_addr #3 {
  %3 = alloca [256 x i8], align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #9
  %4 = icmp slt i64 %1, 0
  %5 = trunc i64 %1 to i32
  %6 = select i1 %4, i32 3, i32 %5
  %7 = call i32 (ptr, i32, ptr, ...) @snprintf(ptr noundef nonnull %3, i32 noundef 256, ptr noundef nonnull @.str.21, i32 noundef %6, double noundef %0) #8
  %8 = icmp slt i32 %7, 0
  br i1 %8, label %9, label %10

9:                                                ; preds = %2
  call void @chali_mcu_panic(ptr noundef nonnull @.str.22) #8
  unreachable

10:                                               ; preds = %2
  %11 = call i32 @llvm.umin.i32(i32 %7, i32 255)
  %12 = call i64 @chali_string_new(ptr noundef nonnull %3, i32 noundef %11) #10
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #9
  ret i64 %12
}

; Function Attrs: nounwind
define dso_local i64 @chali_read_number(i32 noundef %0) local_unnamed_addr #3 {
  %2 = alloca [256 x i8], align 1
  %3 = alloca double, align 8
  %4 = alloca i64, align 8
  %5 = alloca double, align 8
  %6 = alloca i64, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #9
  %7 = load ptr, ptr @stdin, align 4, !tbaa !9
  %8 = call ptr @fgets(ptr noundef nonnull %2, i32 noundef 256, ptr noundef %7) #8
  %9 = icmp eq ptr %8, null
  br i1 %9, label %26, label %10

10:                                               ; preds = %1
  %11 = icmp eq i32 %0, 0
  br i1 %11, label %20, label %12

12:                                               ; preds = %10
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #9
  store double 0.000000e+00, ptr %3, align 8, !tbaa !14
  %13 = call i32 (ptr, ptr, ...) @sscanf(ptr noundef nonnull %2, ptr noundef nonnull @.str.23, ptr noundef nonnull %3) #8
  %14 = icmp eq i32 %13, 1
  br i1 %14, label %16, label %15

15:                                               ; preds = %12
  store double 0.000000e+00, ptr %3, align 8, !tbaa !14
  br label %16

16:                                               ; preds = %15, %12
  call void @llvm.lifetime.start.p0(ptr nonnull %4) #9
  store i64 0, ptr %4, align 8, !tbaa !12
  call void @llvm.lifetime.start.p0(ptr nonnull %5) #9
  %17 = load double, ptr %3, align 8, !tbaa !14
  store double %17, ptr %5, align 8, !tbaa !14
  %18 = call ptr @memcpy(ptr noundef nonnull %4, ptr noundef nonnull %5, i32 noundef 8) #8
  %19 = load i64, ptr %4, align 8, !tbaa !12
  call void @llvm.lifetime.end.p0(ptr nonnull %5) #9
  call void @llvm.lifetime.end.p0(ptr nonnull %4) #9
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #9
  br label %26

20:                                               ; preds = %10
  call void @llvm.lifetime.start.p0(ptr nonnull %6) #9
  store i64 0, ptr %6, align 8, !tbaa !12
  %21 = call i32 (ptr, ptr, ...) @sscanf(ptr noundef nonnull %2, ptr noundef nonnull @.str.24, ptr noundef nonnull %6) #8
  %22 = icmp eq i32 %21, 1
  br i1 %22, label %24, label %23

23:                                               ; preds = %20
  store i64 0, ptr %6, align 8, !tbaa !12
  br label %24

24:                                               ; preds = %23, %20
  %25 = load i64, ptr %6, align 8, !tbaa !12
  call void @llvm.lifetime.end.p0(ptr nonnull %6) #9
  br label %26

26:                                               ; preds = %1, %24, %16
  %27 = phi i64 [ %25, %24 ], [ %19, %16 ], [ 0, %1 ]
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #9
  ret i64 %27
}

declare dso_local ptr @fgets(ptr noundef, i32 noundef, ptr noundef) local_unnamed_addr #2

declare dso_local i32 @sscanf(ptr noundef, ptr noundef, ...) local_unnamed_addr #2

; Function Attrs: nounwind
define dso_local range(i64 0, 2) i64 @chali_stdin_eof() local_unnamed_addr #3 {
  %1 = load ptr, ptr @stdin, align 4, !tbaa !9
  %2 = tail call i32 @feof(ptr noundef %1) #8
  %3 = icmp ne i32 %2, 0
  %4 = zext i1 %3 to i64
  ret i64 %4
}

declare dso_local i32 @feof(ptr noundef) local_unnamed_addr #2

; Function Attrs: nounwind
define dso_local i64 @chali_string_of_int(i64 noundef %0) local_unnamed_addr #3 {
  %2 = alloca [32 x i8], align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #9
  %3 = call i32 (ptr, i32, ptr, ...) @snprintf(ptr noundef nonnull %2, i32 noundef 32, ptr noundef nonnull @.str.24, i64 noundef %0) #8
  %4 = call i32 @strlen(ptr noundef nonnull %2) #8
  %5 = call i64 @chali_string_new(ptr noundef nonnull %2, i32 noundef %4) #10
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #9
  ret i64 %5
}

; Function Attrs: nounwind
define dso_local i64 @chali_string_of_double(double noundef %0) local_unnamed_addr #3 {
  %2 = alloca [64 x i8], align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #9
  %3 = call i32 (ptr, i32, ptr, ...) @snprintf(ptr noundef nonnull %2, i32 noundef 64, ptr noundef nonnull @.str.25, double noundef %0) #8
  %4 = call i32 @strlen(ptr noundef nonnull %2) #8
  %5 = call i64 @chali_string_new(ptr noundef nonnull %2, i32 noundef %4) #10
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #9
  ret i64 %5
}

; Function Attrs: nounwind
define dso_local void @chali_check(i64 noundef %0, i64 noundef %1) local_unnamed_addr #3 {
  %3 = alloca [128 x i8], align 1
  %4 = icmp sgt i64 %0, -1
  %5 = icmp slt i64 %0, %1
  %6 = and i1 %4, %5
  br i1 %6, label %9, label %7

7:                                                ; preds = %2
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #9
  %8 = call i32 (ptr, i32, ptr, ...) @snprintf(ptr noundef nonnull %3, i32 noundef 128, ptr noundef nonnull @.str.6, ptr noundef nonnull @.str.26, i64 noundef %0, i64 noundef %1) #8
  call void @chali_mcu_panic(ptr noundef nonnull %3) #8
  unreachable

9:                                                ; preds = %2
  ret void
}

; Function Attrs: nounwind
define dso_local void @chali_check_pointer(i64 noundef %0) local_unnamed_addr #3 {
  %2 = alloca [128 x i8], align 1
  %3 = icmp eq i64 %0, 0
  br i1 %3, label %4, label %6

4:                                                ; preds = %1
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #9
  %5 = call i32 (ptr, i32, ptr, ...) @snprintf(ptr noundef nonnull %2, i32 noundef 128, ptr noundef nonnull @.str.7, ptr noundef nonnull @.str.27) #8
  call void @chali_mcu_panic(ptr noundef nonnull %2) #8
  unreachable

6:                                                ; preds = %1
  ret void
}

; Function Attrs: nounwind
define dso_local range(i64 1, 0) i64 @chali_require_reference(i64 noundef returned %0) local_unnamed_addr #3 {
  %2 = icmp eq i64 %0, 0
  br i1 %2, label %3, label %4

3:                                                ; preds = %1
  tail call void @chali_mcu_panic(ptr noundef nonnull @.str.28) #8
  unreachable

4:                                                ; preds = %1
  ret i64 %0
}

; Function Attrs: nounwind
define dso_local void @chali_put_char(i64 noundef %0) local_unnamed_addr #3 {
  %2 = trunc i64 %0 to i32
  %3 = and i32 %2, 255
  %4 = load ptr, ptr @stdout, align 4, !tbaa !9
  %5 = tail call i32 @fputc(i32 noundef %3, ptr noundef %4) #8
  ret void
}

; Function Attrs: noreturn nounwind
define dso_local void @chali_abort(i64 noundef %0) local_unnamed_addr #1 {
  %2 = icmp eq i64 %0, 0
  br i1 %2, label %23, label %3

3:                                                ; preds = %1
  store i64 %0, ptr @chali_fmt_at, align 8, !tbaa !12
  store i32 0, ptr @chali_fmt_pos, align 4, !tbaa !17
  %4 = tail call ptr @chali_string_bytes(i64 noundef %0) #10
  %5 = load i32, ptr @chali_fmt_pos, align 4, !tbaa !17
  br label %6

6:                                                ; preds = %14, %3
  %7 = phi i32 [ %5, %3 ], [ %19, %14 ]
  %8 = getelementptr inbounds nuw i8, ptr %4, i32 %7
  %9 = load i8, ptr %8, align 1, !tbaa !16
  switch i8 %9, label %20 [
    i8 0, label %22
    i8 37, label %10
  ]

10:                                               ; preds = %6
  %11 = getelementptr i8, ptr %8, i32 1
  %12 = load i8, ptr %11, align 1, !tbaa !16
  %13 = icmp eq i8 %12, 37
  br i1 %13, label %14, label %20

14:                                               ; preds = %10, %20
  %15 = phi i32 [ %21, %20 ], [ 37, %10 ]
  %16 = phi i32 [ 1, %20 ], [ 2, %10 ]
  %17 = load ptr, ptr @stdout, align 4, !tbaa !9
  %18 = tail call i32 @fputc(i32 noundef %15, ptr noundef %17) #8
  %19 = add i32 %7, %16
  br label %6, !llvm.loop !27

20:                                               ; preds = %10, %6
  %21 = zext i8 %9 to i32
  br label %14

22:                                               ; preds = %6
  store i32 %7, ptr @chali_fmt_pos, align 4, !tbaa !17
  br label %23

23:                                               ; preds = %22, %1
  tail call void @chali_mcu_panic(ptr noundef nonnull @.str.29) #8
  unreachable
}

; Function Attrs: nounwind
define dso_local void @chali_assert(i64 noundef %0, i64 noundef %1) local_unnamed_addr #3 {
  %3 = icmp eq i64 %0, 0
  br i1 %3, label %4, label %27

4:                                                ; preds = %2
  %5 = icmp eq i64 %1, 0
  br i1 %5, label %26, label %6

6:                                                ; preds = %4
  store i64 %1, ptr @chali_fmt_at, align 8, !tbaa !12
  store i32 0, ptr @chali_fmt_pos, align 4, !tbaa !17
  %7 = tail call ptr @chali_string_bytes(i64 noundef %1) #10
  %8 = load i32, ptr @chali_fmt_pos, align 4, !tbaa !17
  br label %9

9:                                                ; preds = %17, %6
  %10 = phi i32 [ %8, %6 ], [ %22, %17 ]
  %11 = getelementptr inbounds nuw i8, ptr %7, i32 %10
  %12 = load i8, ptr %11, align 1, !tbaa !16
  switch i8 %12, label %23 [
    i8 0, label %25
    i8 37, label %13
  ]

13:                                               ; preds = %9
  %14 = getelementptr i8, ptr %11, i32 1
  %15 = load i8, ptr %14, align 1, !tbaa !16
  %16 = icmp eq i8 %15, 37
  br i1 %16, label %17, label %23

17:                                               ; preds = %13, %23
  %18 = phi i32 [ %24, %23 ], [ 37, %13 ]
  %19 = phi i32 [ 1, %23 ], [ 2, %13 ]
  %20 = load ptr, ptr @stdout, align 4, !tbaa !9
  %21 = tail call i32 @fputc(i32 noundef %18, ptr noundef %20) #8
  %22 = add i32 %10, %19
  br label %9, !llvm.loop !27

23:                                               ; preds = %13, %9
  %24 = zext i8 %12 to i32
  br label %17

25:                                               ; preds = %9
  store i32 %10, ptr @chali_fmt_pos, align 4, !tbaa !17
  br label %26

26:                                               ; preds = %25, %4
  tail call void @chali_mcu_panic(ptr noundef nonnull @.str.29) #8
  unreachable

27:                                               ; preds = %2
  ret void
}

; Function Attrs: nounwind
define dso_local noundef i64 @chali_copy_bytes(i64 noundef returned %0, i64 noundef %1, i64 noundef %2) local_unnamed_addr #3 {
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef %2) #8
  %5 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef %2) #8
  %6 = trunc i64 %2 to i32
  %7 = tail call ptr @memmove(ptr noundef %4, ptr noundef %5, i32 noundef %6) #8
  ret i64 %0
}

declare dso_local ptr @memmove(ptr noundef, ptr noundef, i32 noundef) local_unnamed_addr #2

; Function Attrs: nounwind
define dso_local noundef i64 @chali_set_bytes(i64 noundef returned %0, i64 noundef %1, i64 noundef %2) local_unnamed_addr #3 {
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef %2) #8
  %5 = trunc i64 %1 to i32
  %6 = trunc i64 %2 to i32
  %7 = tail call ptr @memset(ptr noundef %4, i32 noundef %5, i32 noundef %6) #8
  ret i64 %0
}

declare dso_local ptr @memset(ptr noundef, i32 noundef, i32 noundef) local_unnamed_addr #2

; Function Attrs: nounwind
define dso_local range(i64 0, 256) i64 @chali_string_at(i64 noundef %0, i64 noundef %1) local_unnamed_addr #3 {
  %3 = tail call ptr @chali_string_bytes(i64 noundef %0) #10
  %4 = trunc i64 %1 to i32
  %5 = getelementptr inbounds i8, ptr %3, i32 %4
  %6 = load i8, ptr %5, align 1, !tbaa !16
  %7 = zext i8 %6 to i64
  ret i64 %7
}

; Function Attrs: nounwind
define dso_local void @chali_string_set(i64 noundef %0, i64 noundef %1, i64 noundef %2) local_unnamed_addr #3 {
  %4 = add nsw i64 %0, 8
  %5 = add nsw i64 %4, %1
  %6 = trunc i64 %2 to i8
  %7 = tail call ptr @chali_host_address(i64 noundef %5, i64 noundef 1) #8
  store i8 %6, ptr %7, align 1, !tbaa !16
  ret void
}

; Function Attrs: nounwind
define dso_local noundef i64 @chali_string_concat(i64 noundef %0, i64 noundef %1) local_unnamed_addr #3 {
  %3 = alloca i64, align 8
  %4 = tail call i64 @chali_string_length(i64 noundef %0) #10
  %5 = trunc i64 %4 to i32
  %6 = tail call i64 @chali_string_length(i64 noundef %1) #10
  %7 = trunc i64 %6 to i32
  %8 = add i32 %7, %5
  %9 = add i32 %8, 8
  %10 = zext i32 %9 to i64
  %11 = tail call i64 @chali_host_allocate(i64 noundef %10) #8
  %12 = zext i32 %8 to i64
  call void @llvm.lifetime.start.p0(ptr nonnull %3)
  store i64 %12, ptr %3, align 8, !tbaa !12
  %13 = tail call ptr @chali_host_address(i64 noundef %11, i64 noundef 8) #8
  %14 = call ptr @memcpy(ptr noundef %13, ptr noundef nonnull %3, i32 noundef 8) #8
  call void @llvm.lifetime.end.p0(ptr nonnull %3)
  %15 = add nsw i64 %11, 8
  %16 = and i64 %4, 4294967295
  %17 = call ptr @chali_host_address(i64 noundef %15, i64 noundef %16) #8
  %18 = call ptr @chali_string_bytes(i64 noundef %0) #10
  %19 = call ptr @memcpy(ptr noundef %17, ptr noundef %18, i32 noundef %5) #8
  %20 = add nsw i64 %15, %16
  %21 = and i64 %6, 4294967295
  %22 = call ptr @chali_host_address(i64 noundef %20, i64 noundef %21) #8
  %23 = call ptr @chali_string_bytes(i64 noundef %1) #10
  %24 = call ptr @memcpy(ptr noundef %22, ptr noundef %23, i32 noundef %7) #8
  ret i64 %11
}

; Function Attrs: nounwind
define dso_local void @chali_owned_drop(i64 noundef %0) local_unnamed_addr #3 {
  tail call void @chali_host_drop(i64 noundef %0) #8
  ret void
}

; Function Attrs: nounwind
define dso_local i64 @chali_function_target(i64 noundef %0) local_unnamed_addr #3 {
  %2 = tail call i64 @chali_host_function(i64 noundef %0, i32 noundef 0) #8
  ret i64 %2
}

declare dso_local i64 @chali_host_function(i64 noundef, i32 noundef) local_unnamed_addr #2

; Function Attrs: nounwind
define dso_local i64 @chali_function_environment(i64 noundef %0) local_unnamed_addr #3 {
  %2 = tail call i64 @chali_host_function(i64 noundef %0, i32 noundef 1) #8
  ret i64 %2
}

; Function Attrs: nounwind
define dso_local range(i64 0, 2) i64 @chali_function_match(i64 noundef %0, ptr noundef %1) local_unnamed_addr #3 {
  %3 = icmp slt i64 %0, 1
  br i1 %3, label %20, label %4

4:                                                ; preds = %2
  %5 = and i64 %0, 4294967295
  %6 = load i64, ptr @chali_data_size, align 8, !tbaa !12
  %7 = icmp ult i64 %5, %6
  br i1 %7, label %8, label %20

8:                                                ; preds = %4
  %9 = tail call i32 @strlen(ptr noundef %1) #8
  %10 = zext i32 %9 to i64
  %11 = sub nuw i64 %6, %5
  %12 = icmp ugt i64 %11, %10
  br i1 %12, label %13, label %20

13:                                               ; preds = %8
  %14 = add i32 %9, 1
  %15 = zext i32 %14 to i64
  %16 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef %15) #8
  %17 = tail call i32 @memcmp(ptr noundef %16, ptr noundef %1, i32 noundef %14) #8
  %18 = icmp eq i32 %17, 0
  %19 = zext i1 %18 to i64
  br label %20

20:                                               ; preds = %13, %8, %2, %4
  %21 = phi i64 [ 0, %2 ], [ 0, %4 ], [ %19, %13 ], [ 0, %8 ]
  ret i64 %21
}

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.umax.i32(i32, i32) #7

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.smax.i64(i64, i64) #7

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.umin.i32(i32, i32) #7

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare range(i32 -1, 2) i32 @llvm.ucmp.i32.i32(i32, i32) #7

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.smin.i64(i64, i64) #7

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(read, argmem: none, inaccessiblemem: none, target_mem: none) "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="cortex-m4" "target-features"="+armv7e-m,+dsp,+hwdiv,+soft-float,+thumb-mode,-aes,-bf16,-cdecp0,-cdecp1,-cdecp2,-cdecp3,-cdecp4,-cdecp5,-cdecp6,-cdecp7,-crc,-crypto,-d32,-dotprod,-fp-armv8,-fp-armv8d16,-fp-armv8d16sp,-fp-armv8sp,-fp16,-fp16fml,-fp64,-fpregs,-fullfp16,-hwdiv-arm,-i8mm,-lob,-mve,-mve.fp,-neon,-pacbti,-ras,-sb,-sha2,-vfp2,-vfp2sp,-vfp3,-vfp3d16,-vfp3d16sp,-vfp3sp,-vfp4,-vfp4d16,-vfp4d16sp,-vfp4sp" "use-soft-float"="true" }
attributes #1 = { noreturn nounwind "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="cortex-m4" "target-features"="+armv7e-m,+dsp,+hwdiv,+soft-float,+thumb-mode,-aes,-bf16,-cdecp0,-cdecp1,-cdecp2,-cdecp3,-cdecp4,-cdecp5,-cdecp6,-cdecp7,-crc,-crypto,-d32,-dotprod,-fp-armv8,-fp-armv8d16,-fp-armv8d16sp,-fp-armv8sp,-fp16,-fp16fml,-fp64,-fpregs,-fullfp16,-hwdiv-arm,-i8mm,-lob,-mve,-mve.fp,-neon,-pacbti,-ras,-sb,-sha2,-vfp2,-vfp2sp,-vfp3,-vfp3d16,-vfp3d16sp,-vfp3sp,-vfp4,-vfp4d16,-vfp4d16sp,-vfp4sp" "use-soft-float"="true" }
attributes #2 = { "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="cortex-m4" "target-features"="+armv7e-m,+dsp,+hwdiv,+soft-float,+thumb-mode,-aes,-bf16,-cdecp0,-cdecp1,-cdecp2,-cdecp3,-cdecp4,-cdecp5,-cdecp6,-cdecp7,-crc,-crypto,-d32,-dotprod,-fp-armv8,-fp-armv8d16,-fp-armv8d16sp,-fp-armv8sp,-fp16,-fp16fml,-fp64,-fpregs,-fullfp16,-hwdiv-arm,-i8mm,-lob,-mve,-mve.fp,-neon,-pacbti,-ras,-sb,-sha2,-vfp2,-vfp2sp,-vfp3,-vfp3d16,-vfp3d16sp,-vfp3sp,-vfp4,-vfp4d16,-vfp4d16sp,-vfp4sp" "use-soft-float"="true" }
attributes #3 = { nounwind "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="cortex-m4" "target-features"="+armv7e-m,+dsp,+hwdiv,+soft-float,+thumb-mode,-aes,-bf16,-cdecp0,-cdecp1,-cdecp2,-cdecp3,-cdecp4,-cdecp5,-cdecp6,-cdecp7,-crc,-crypto,-d32,-dotprod,-fp-armv8,-fp-armv8d16,-fp-armv8d16sp,-fp-armv8sp,-fp16,-fp16fml,-fp64,-fpregs,-fullfp16,-hwdiv-arm,-i8mm,-lob,-mve,-mve.fp,-neon,-pacbti,-ras,-sb,-sha2,-vfp2,-vfp2sp,-vfp3,-vfp3d16,-vfp3d16sp,-vfp3sp,-vfp4,-vfp4d16,-vfp4d16sp,-vfp4sp" "use-soft-float"="true" }
attributes #4 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #5 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="cortex-m4" "target-features"="+armv7e-m,+dsp,+hwdiv,+soft-float,+thumb-mode,-aes,-bf16,-cdecp0,-cdecp1,-cdecp2,-cdecp3,-cdecp4,-cdecp5,-cdecp6,-cdecp7,-crc,-crypto,-d32,-dotprod,-fp-armv8,-fp-armv8d16,-fp-armv8d16sp,-fp-armv8sp,-fp16,-fp16fml,-fp64,-fpregs,-fullfp16,-hwdiv-arm,-i8mm,-lob,-mve,-mve.fp,-neon,-pacbti,-ras,-sb,-sha2,-vfp2,-vfp2sp,-vfp3,-vfp3d16,-vfp3d16sp,-vfp3sp,-vfp4,-vfp4d16,-vfp4d16sp,-vfp4sp" "use-soft-float"="true" }
attributes #6 = { mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none, target_mem: none) "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="cortex-m4" "target-features"="+armv7e-m,+dsp,+hwdiv,+soft-float,+thumb-mode,-aes,-bf16,-cdecp0,-cdecp1,-cdecp2,-cdecp3,-cdecp4,-cdecp5,-cdecp6,-cdecp7,-crc,-crypto,-d32,-dotprod,-fp-armv8,-fp-armv8d16,-fp-armv8d16sp,-fp-armv8sp,-fp16,-fp16fml,-fp64,-fpregs,-fullfp16,-hwdiv-arm,-i8mm,-lob,-mve,-mve.fp,-neon,-pacbti,-ras,-sb,-sha2,-vfp2,-vfp2sp,-vfp3,-vfp3d16,-vfp3d16sp,-vfp3sp,-vfp4,-vfp4d16,-vfp4d16sp,-vfp4sp" "use-soft-float"="true" }
attributes #7 = { nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none) }
attributes #8 = { nobuiltin nounwind "no-builtins" }
attributes #9 = { nounwind }
attributes #10 = { nobuiltin "no-builtins" }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}
!llvm.errno.tbaa = !{!4}

!0 = !{i32 1, !"min_enum_size", i32 4}
!1 = !{i32 4, !"arm-eabi-fp-denormal", i32 1}
!2 = !{i32 8, !"arm-eabi-fp-number-model", i32 3}
!3 = !{!"clang version 23.1.4 (https://github.com/llvm/llvm-project.git 7a941dfb2877a0b8cf30323fefa85ad019e0b0f5)"}
!4 = !{!5, !6, i64 0}
!5 = !{!"__libc_errno", !6, i64 0}
!6 = !{!"int", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C/C++ TBAA"}
!9 = !{!10, !10, i64 0}
!10 = !{!"p1 _ZTS9ChaliFile", !11, i64 0}
!11 = !{!"any pointer", !7, i64 0}
!12 = !{!13, !13, i64 0}
!13 = !{!"long long", !7, i64 0}
!14 = !{!15, !15, i64 0}
!15 = !{!"double", !7, i64 0}
!16 = !{!7, !7, i64 0}
!17 = !{!6, !6, i64 0}
!18 = !{!19, !19, i64 0}
!19 = !{!"short", !7, i64 0}
!20 = distinct !{!20, !21, !22}
!21 = !{!"llvm.loop.mustprogress"}
!22 = !{!"llvm.loop.unroll.disable"}
!23 = distinct !{!23, !21, !22}
!24 = distinct !{!24, !21, !22}
!25 = distinct !{!25, !21, !22}
!26 = distinct !{!26, !21, !22}
!27 = distinct !{!27, !21, !22}
!28 = distinct !{!28, !21, !22}
!29 = distinct !{!29, !21, !22}
