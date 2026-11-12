; ModuleID = 'source/aot/runtime.c'
source_filename = "source/aot/runtime.c"
target datalayout = "e-m:m-p:32:32-i8:8:32-i16:16:32-i64:64-n32-S64"
target triple = "mipsel-unknown-linux-gnu"

@stdin = external local_unnamed_addr global ptr, align 4
@stdout = external local_unnamed_addr global ptr, align 4
@stderr = external local_unnamed_addr global ptr, align 4
@.str = private unnamed_addr constant [17 x i8] c"chali build: %s\0A\00", align 1
@.str.1 = private unnamed_addr constant [55 x i8] c"\E7\A7\BB\E4\BD\8D\E4\BD\8D\E6\95\B0\E8\B6\85\E5\87\BA\E6\95\B4\E6\95\B0\E4\BD\8D\E5\AE\BD\EF\BC\88\E6\9C\89\E6\95\88\E8\8C\83\E5\9B\B4 0..63\EF\BC\89\00", align 1
@.str.2 = private unnamed_addr constant [13 x i8] c"\E6\B5\AE\E7\82\B9\E9\99\A4\E9\9B\B6\00", align 1
@.str.3 = private unnamed_addr constant [13 x i8] c"\E6\95\B4\E6\95\B0\E9\99\A4\E9\9B\B6\00", align 1
@.str.4 = private unnamed_addr constant [72 x i8] c"\E6\95\B4\E6\95\B0\E9\99\A4\E6\B3\95\E6\BA\A2\E5\87\BA\EF\BC\9A\E6\9C\80\E5\B0\8F\E8\B4\9F\E6\95\B0\E9\99\A4\E4\BB\A5 -1 \E7\9A\84\E7\BB\93\E6\9E\9C\E8\B6\85\E5\87\BA int64 \E8\8C\83\E5\9B\B4\00", align 1
@.str.6 = private unnamed_addr constant [31 x i8] c"\E5\A4\8D\E5\88\B6\E5\AD\97\E7\AC\A6\E4\B8\B2\E6\97\B6\E5\86\85\E5\AD\98\E4\B8\8D\E8\B6\B3\00", align 1
@chali_data_size = external local_unnamed_addr constant i64, align 8
@.str.7 = private unnamed_addr constant [37 x i8] c"%s\E8\B6\8A\E7\95\8C\EF\BC\9A\E4\B8\8B\E6\A0\87 %lld\EF\BC\8C\E9\95\BF\E5\BA\A6 %lld\00", align 1
@.str.8 = private unnamed_addr constant [15 x i8] c"%s\EF\BC\9A\E7\A9\BA\E6\8C\87\E9\92\88\00", align 1
@chali_fmt_at = internal unnamed_addr global i64 0, align 8
@chali_fmt_pos = internal unnamed_addr global i32 0, align 4
@.str.9 = private unnamed_addr constant [16 x i8] c"-+ #0123456789.\00", align 1
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

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(read, argmem: none, inaccessiblemem: none, target_mem: none) uwtable(sync)
define dso_local ptr @chali_standard_stream(i32 noundef signext %0) local_unnamed_addr #0 {
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

; Function Attrs: cold noreturn uwtable(sync)
define dso_local void @chali_panic(ptr noundef %0) local_unnamed_addr #1 {
  tail call void @chali_host_raise_callback_error(ptr noundef %0)
  %2 = load ptr, ptr @stdout, align 4, !tbaa !9
  %3 = tail call i32 @fflush(ptr noundef %2)
  %4 = load ptr, ptr @stderr, align 4, !tbaa !9
  %5 = tail call i32 (ptr, ptr, ...) @fprintf(ptr noundef %4, ptr noundef nonnull @.str, ptr noundef %0) #19
  tail call void @chali_host_trace()
  tail call void @exit(i32 noundef signext 1) #20
  unreachable
}

declare void @chali_host_raise_callback_error(ptr noundef) local_unnamed_addr #2

; Function Attrs: nofree nounwind
declare noundef i32 @fflush(ptr noundef captures(none)) local_unnamed_addr #3

; Function Attrs: nofree nounwind
declare noundef i32 @fprintf(ptr noundef captures(none), ptr noundef readonly captures(none), ...) local_unnamed_addr #3

declare void @chali_host_trace() local_unnamed_addr #2

; Function Attrs: nofree noreturn
declare void @exit(i32 noundef signext) local_unnamed_addr #4

; Function Attrs: uwtable(sync)
define dso_local void @chali_check_shift(i64 noundef signext %0) local_unnamed_addr #5 {
  %2 = icmp ugt i64 %0, 63
  br i1 %2, label %3, label %4

3:                                                ; preds = %1
  tail call void @chali_panic(ptr noundef nonnull @.str.1)
  unreachable

4:                                                ; preds = %1
  ret void
}

; Function Attrs: uwtable(sync)
define dso_local void @chali_check_real_divisor(double noundef %0) local_unnamed_addr #5 {
  %2 = fcmp oeq double %0, 0.000000e+00
  br i1 %2, label %3, label %4

3:                                                ; preds = %1
  tail call void @chali_panic(ptr noundef nonnull @.str.2)
  unreachable

4:                                                ; preds = %1
  ret void
}

; Function Attrs: uwtable(sync)
define dso_local noundef i64 @chali_integer_divide(i64 noundef signext %0, i64 noundef signext %1, i32 noundef signext %2, i32 noundef signext %3) local_unnamed_addr #5 {
  %5 = icmp eq i64 %1, 0
  br i1 %5, label %6, label %7

6:                                                ; preds = %4
  tail call void @chali_panic(ptr noundef nonnull @.str.3)
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
  tail call void @chali_panic(ptr noundef nonnull @.str.4)
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

; Function Attrs: uwtable(sync)
define dso_local void @chali_init() local_unnamed_addr #5 {
  tail call void @chali_host_init()
  ret void
}

declare void @chali_host_init() local_unnamed_addr #2

; Function Attrs: uwtable(sync)
define dso_local void @chali_finish() local_unnamed_addr #5 {
  %1 = load ptr, ptr @stdout, align 4, !tbaa !9
  %2 = tail call i32 @fflush(ptr noundef %1)
  tail call void @chali_host_finish()
  ret void
}

declare void @chali_host_finish() local_unnamed_addr #2

; Function Attrs: uwtable(sync)
define dso_local i64 @chali_alloc(i64 noundef zeroext %0) local_unnamed_addr #5 {
  %2 = tail call i64 @chali_host_allocate(i64 noundef zeroext %0)
  ret i64 %2
}

declare i64 @chali_host_allocate(i64 noundef zeroext) local_unnamed_addr #2

; Function Attrs: uwtable(sync)
define dso_local void @chali_drop(i64 noundef signext %0) local_unnamed_addr #5 {
  tail call void @chali_host_drop(i64 noundef signext %0)
  ret void
}

declare void @chali_host_drop(i64 noundef signext) local_unnamed_addr #2

; Function Attrs: uwtable(sync)
define dso_local i64 @chali_load_i64(i64 noundef signext %0) local_unnamed_addr #5 {
  %2 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 8)
  %3 = load i64, ptr %2, align 1
  ret i64 %3
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(ptr captures(none)) #6

declare ptr @chali_host_address(i64 noundef signext, i64 noundef zeroext) local_unnamed_addr #2

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i32(ptr noalias writeonly captures(none), ptr noalias readonly captures(none), i32, i1 immarg) #6

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(ptr captures(none)) #6

; Function Attrs: uwtable(sync)
define dso_local void @chali_store_i64(i64 noundef signext %0, i64 noundef signext %1) local_unnamed_addr #5 {
  %3 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 8)
  store i64 %1, ptr %3, align 1
  ret void
}

; Function Attrs: uwtable(sync)
define dso_local double @chali_load_f64(i64 noundef signext %0) local_unnamed_addr #5 {
  %2 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 8)
  %3 = load double, ptr %2, align 1
  ret double %3
}

; Function Attrs: uwtable(sync)
define dso_local void @chali_store_f64(i64 noundef signext %0, double noundef %1) local_unnamed_addr #5 {
  %3 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 8)
  store double %1, ptr %3, align 1
  ret void
}

; Function Attrs: uwtable(sync)
define dso_local range(i64 0, 256) i64 @chali_load_u8(i64 noundef signext %0) local_unnamed_addr #5 {
  %2 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 1)
  %3 = load i8, ptr %2, align 1, !tbaa !12
  %4 = zext i8 %3 to i64
  ret i64 %4
}

; Function Attrs: uwtable(sync)
define dso_local void @chali_store_u8(i64 noundef signext %0, i64 noundef signext %1) local_unnamed_addr #5 {
  %3 = trunc i64 %1 to i8
  %4 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 1)
  store i8 %3, ptr %4, align 1, !tbaa !12
  ret void
}

; Function Attrs: uwtable(sync)
define dso_local range(i64 -2147483648, 2147483648) i64 @chali_load_i32(i64 noundef signext %0) local_unnamed_addr #5 {
  %2 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 4)
  %3 = load i32, ptr %2, align 1
  %4 = sext i32 %3 to i64
  ret i64 %4
}

; Function Attrs: uwtable(sync)
define dso_local void @chali_store_i32(i64 noundef signext %0, i64 noundef signext %1) local_unnamed_addr #5 {
  %3 = trunc i64 %1 to i32
  %4 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 4)
  store i32 %3, ptr %4, align 1
  ret void
}

; Function Attrs: uwtable(sync)
define dso_local range(i64 -32768, 32768) i64 @chali_load_i16(i64 noundef signext %0) local_unnamed_addr #5 {
  %2 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 2)
  %3 = load i16, ptr %2, align 1
  %4 = sext i16 %3 to i64
  ret i64 %4
}

; Function Attrs: uwtable(sync)
define dso_local void @chali_store_i16(i64 noundef signext %0, i64 noundef signext %1) local_unnamed_addr #5 {
  %3 = trunc i64 %1 to i16
  %4 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 2)
  store i16 %3, ptr %4, align 1
  ret void
}

; Function Attrs: uwtable(sync)
define dso_local i64 @chali_string_new(ptr nofree noundef readonly captures(none) %0, i32 noundef signext %1) local_unnamed_addr #5 {
  %3 = zext i32 %1 to i64
  %4 = icmp eq i32 %1, 0
  %5 = tail call i32 @llvm.umax.i32(i32 %1, i32 1)
  %6 = tail call ptr @malloc(i32 noundef signext %5) #21
  %7 = icmp eq ptr %6, null
  br i1 %7, label %8, label %9

8:                                                ; preds = %2
  tail call void @chali_panic(ptr noundef nonnull @.str.6)
  unreachable

9:                                                ; preds = %2
  br i1 %4, label %11, label %10

10:                                               ; preds = %9
  tail call void @llvm.memcpy.p0.p0.i32(ptr nonnull align 1 %6, ptr align 1 %0, i32 %1, i1 false)
  br label %11

11:                                               ; preds = %10, %9
  %12 = icmp ult i32 %1, 17
  %13 = select i1 %12, i32 49, i32 32
  %14 = zext nneg i32 %13 to i64
  %15 = tail call i64 @chali_host_allocate(i64 noundef zeroext %14)
  br i1 %12, label %16, label %18

16:                                               ; preds = %11
  %17 = add nsw i64 %15, 32
  br label %22

18:                                               ; preds = %11
  %19 = add i32 %1, 1
  %20 = zext i32 %19 to i64
  %21 = tail call i64 @chali_host_allocate(i64 noundef zeroext %20)
  br label %22

22:                                               ; preds = %18, %16
  %23 = phi i64 [ 16, %16 ], [ %3, %18 ]
  %24 = phi i64 [ %17, %16 ], [ %21, %18 ]
  %25 = tail call ptr @chali_host_address(i64 noundef signext %15, i64 noundef zeroext 32)
  store i64 -1, ptr %25, align 1
  %26 = getelementptr inbounds nuw i8, ptr %25, i32 8
  store i64 %3, ptr %26, align 1
  %27 = getelementptr inbounds nuw i8, ptr %25, i32 16
  store i64 %23, ptr %27, align 1
  %28 = getelementptr inbounds nuw i8, ptr %25, i32 24
  store i64 %24, ptr %28, align 1
  br i1 %4, label %31, label %29

29:                                               ; preds = %22
  %30 = tail call ptr @chali_host_address(i64 noundef signext %24, i64 noundef zeroext %3)
  tail call void @llvm.memcpy.p0.p0.i32(ptr align 1 %30, ptr nonnull align 1 %6, i32 %1, i1 false)
  br label %31

31:                                               ; preds = %29, %22
  %32 = add nsw i64 %24, %3
  %33 = tail call ptr @chali_host_address(i64 noundef signext %32, i64 noundef zeroext 1)
  store i8 0, ptr %33, align 1, !tbaa !12
  tail call void @free(ptr noundef %6)
  ret i64 %15
}

; Function Attrs: mustprogress nofree nounwind willreturn allockind("alloc,uninitialized") allocsize(0) memory(inaccessiblemem: readwrite, errnomem: write)
declare noalias noundef ptr @malloc(i32 noundef signext) local_unnamed_addr #7

; Function Attrs: mustprogress nounwind willreturn allockind("free") memory(argmem: readwrite, inaccessiblemem: readwrite)
declare void @free(ptr allocptr noundef captures(none)) local_unnamed_addr #8

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable(sync)
define dso_local range(i32 0, 2) i32 @chali_on_heap(i64 noundef signext %0) local_unnamed_addr #9 {
  %2 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %3 = icmp sge i64 %0, %2
  %4 = zext i1 %3 to i32
  ret i32 %4
}

; Function Attrs: uwtable(sync)
define dso_local i64 @chali_string_length(i64 noundef signext %0) local_unnamed_addr #5 {
  %2 = icmp eq i64 %0, 0
  br i1 %2, label %18, label %3

3:                                                ; preds = %1
  %4 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %5 = icmp slt i64 %0, %4
  br i1 %5, label %6, label %10

6:                                                ; preds = %3
  %7 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 1)
  %8 = tail call i32 @strlen(ptr noundef nonnull dereferenceable(1) %7) #22
  %9 = zext i32 %8 to i64
  br label %18

10:                                               ; preds = %3
  %11 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 8)
  %12 = load i64, ptr %11, align 1
  %13 = icmp eq i64 %12, -1
  br i1 %13, label %14, label %18

14:                                               ; preds = %10
  %15 = add nsw i64 %0, 8
  %16 = tail call ptr @chali_host_address(i64 noundef signext %15, i64 noundef zeroext 8)
  %17 = load i64, ptr %16, align 1
  br label %18

18:                                               ; preds = %14, %10, %1, %6
  %19 = phi i64 [ %9, %6 ], [ 0, %1 ], [ %17, %14 ], [ %12, %10 ]
  ret i64 %19
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare i32 @strlen(ptr noundef captures(none)) local_unnamed_addr #10

; Function Attrs: uwtable(sync)
define dso_local ptr @chali_string_bytes(i64 noundef signext %0) local_unnamed_addr #5 {
  %2 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %3 = icmp slt i64 %0, %2
  br i1 %3, label %14, label %4

4:                                                ; preds = %1
  %5 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 8)
  %6 = load i64, ptr %5, align 1
  %7 = icmp eq i64 %6, -1
  br i1 %7, label %8, label %12

8:                                                ; preds = %4
  %9 = add nsw i64 %0, 24
  %10 = tail call ptr @chali_host_address(i64 noundef signext %9, i64 noundef zeroext 8)
  %11 = load i64, ptr %10, align 1
  br label %14

12:                                               ; preds = %4
  %13 = add nsw i64 %0, 8
  br label %14

14:                                               ; preds = %1, %8, %12
  %15 = phi i64 [ %13, %12 ], [ %11, %8 ], [ %0, %1 ]
  %16 = tail call ptr @chali_host_address(i64 noundef signext %15, i64 noundef zeroext 0)
  ret ptr %16
}

; Function Attrs: uwtable(sync)
define dso_local i64 @chali_string_literal(i64 noundef signext %0) local_unnamed_addr #5 {
  %2 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 1)
  %3 = tail call i32 @strlen(ptr noundef nonnull dereferenceable(1) %2) #22
  %4 = tail call i64 @chali_string_new(ptr noundef nonnull %2, i32 noundef signext %3)
  ret i64 %4
}

; Function Attrs: uwtable(sync)
define dso_local i64 @chali_string_copy(i64 noundef signext %0) local_unnamed_addr #5 {
  %2 = icmp eq i64 %0, 0
  br i1 %2, label %35, label %3

3:                                                ; preds = %1
  %4 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %5 = icmp slt i64 %0, %4
  br i1 %5, label %16, label %6

6:                                                ; preds = %3
  %7 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 8)
  %8 = load i64, ptr %7, align 1
  %9 = icmp eq i64 %8, -1
  br i1 %9, label %10, label %14

10:                                               ; preds = %6
  %11 = add nsw i64 %0, 24
  %12 = tail call ptr @chali_host_address(i64 noundef signext %11, i64 noundef zeroext 8)
  %13 = load i64, ptr %12, align 1
  br label %16

14:                                               ; preds = %6
  %15 = add nsw i64 %0, 8
  br label %16

16:                                               ; preds = %3, %10, %14
  %17 = phi i64 [ %15, %14 ], [ %13, %10 ], [ %0, %3 ]
  %18 = tail call ptr @chali_host_address(i64 noundef signext %17, i64 noundef zeroext 0)
  br i1 %5, label %19, label %22

19:                                               ; preds = %16
  %20 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 1)
  %21 = tail call i32 @strlen(ptr noundef nonnull dereferenceable(1) %20) #22
  br label %32

22:                                               ; preds = %16
  %23 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 8)
  %24 = load i64, ptr %23, align 1
  %25 = icmp eq i64 %24, -1
  %26 = trunc i64 %24 to i32
  br i1 %25, label %27, label %32

27:                                               ; preds = %22
  %28 = add nsw i64 %0, 8
  %29 = tail call ptr @chali_host_address(i64 noundef signext %28, i64 noundef zeroext 8)
  %30 = load i64, ptr %29, align 1
  %31 = trunc i64 %30 to i32
  br label %32

32:                                               ; preds = %19, %22, %27
  %33 = phi i32 [ %21, %19 ], [ %26, %22 ], [ %31, %27 ]
  %34 = tail call i64 @chali_string_new(ptr noundef %18, i32 noundef signext %33)
  br label %35

35:                                               ; preds = %1, %32
  %36 = phi i64 [ %34, %32 ], [ 0, %1 ]
  ret i64 %36
}

; Function Attrs: uwtable(sync)
define dso_local void @chali_string_drop(i64 noundef signext %0) local_unnamed_addr #5 {
  %2 = icmp eq i64 %0, 0
  %3 = load i64, ptr @chali_data_size, align 8
  %4 = icmp slt i64 %0, %3
  %5 = select i1 %2, i1 true, i1 %4
  br i1 %5, label %17, label %6

6:                                                ; preds = %1
  %7 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 8)
  %8 = load i64, ptr %7, align 1
  %9 = icmp eq i64 %8, -1
  br i1 %9, label %10, label %17

10:                                               ; preds = %6
  %11 = add nsw i64 %0, 24
  %12 = tail call ptr @chali_host_address(i64 noundef signext %11, i64 noundef zeroext 8)
  %13 = load i64, ptr %12, align 1
  %14 = add nsw i64 %0, 32
  %15 = icmp eq i64 %13, %14
  br i1 %15, label %17, label %16

16:                                               ; preds = %10
  tail call void @chali_host_drop(i64 noundef signext %13)
  br label %17

17:                                               ; preds = %10, %16, %6, %1
  tail call void @chali_host_drop(i64 noundef signext %0)
  ret void
}

; Function Attrs: uwtable(sync)
define dso_local range(i64 -9223372036854775800, -9223372036854775808) i64 @chali_new_array(i64 noundef signext %0, i64 noundef signext %1) local_unnamed_addr #5 {
  %3 = tail call i64 @llvm.smax.i64(i64 %0, i64 0)
  %4 = mul i64 %3, %1
  %5 = add i64 %4, 8
  %6 = tail call i64 @chali_host_allocate(i64 noundef zeroext %5)
  %7 = tail call ptr @chali_host_address(i64 noundef signext %6, i64 noundef zeroext 8)
  store i64 %3, ptr %7, align 1
  %8 = add nsw i64 %6, 8
  ret i64 %8
}

; Function Attrs: uwtable(sync)
define dso_local void @chali_bounds(i64 noundef signext %0, i64 noundef signext %1, ptr noundef %2) local_unnamed_addr #5 {
  %4 = alloca [128 x i8], align 1
  %5 = icmp sgt i64 %0, -1
  %6 = icmp slt i64 %0, %1
  %7 = and i1 %5, %6
  br i1 %7, label %10, label %8

8:                                                ; preds = %3
  call void @llvm.lifetime.start.p0(ptr nonnull %4) #22
  %9 = call i32 (ptr, i32, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %4, i32 noundef signext 128, ptr noundef nonnull @.str.7, ptr noundef %2, i64 noundef signext %0, i64 noundef signext %1) #22
  call void @chali_panic(ptr noundef nonnull %4)
  unreachable

10:                                               ; preds = %3
  ret void
}

; Function Attrs: nofree nounwind
declare noundef i32 @snprintf(ptr noalias noundef writeonly captures(none), i32 noundef signext, ptr noundef readonly captures(none), ...) local_unnamed_addr #3

; Function Attrs: uwtable(sync)
define dso_local void @chali_null(i64 noundef signext %0, ptr noundef %1) local_unnamed_addr #5 {
  %3 = alloca [128 x i8], align 1
  %4 = icmp eq i64 %0, 0
  br i1 %4, label %5, label %7

5:                                                ; preds = %2
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #22
  %6 = call i32 (ptr, i32, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i32 noundef signext 128, ptr noundef nonnull @.str.8, ptr noundef %1) #22
  call void @chali_panic(ptr noundef nonnull %3)
  unreachable

7:                                                ; preds = %2
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none, target_mem: none) uwtable(sync)
define dso_local void @chali_fmt_begin(i64 noundef signext %0) local_unnamed_addr #11 {
  store i64 %0, ptr @chali_fmt_at, align 8, !tbaa !13
  store i32 0, ptr @chali_fmt_pos, align 4, !tbaa !15
  ret void
}

; Function Attrs: uwtable(sync)
define dso_local range(i32 0, 256) i32 @chali_fmt_scan(ptr nofree noundef writeonly captures(none) %0, i32 noundef signext %1) local_unnamed_addr #5 {
  %3 = load i64, ptr @chali_fmt_at, align 8, !tbaa !13
  %4 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %5 = icmp slt i64 %3, %4
  br i1 %5, label %16, label %6

6:                                                ; preds = %2
  %7 = tail call ptr @chali_host_address(i64 noundef signext %3, i64 noundef zeroext 8)
  %8 = load i64, ptr %7, align 1
  %9 = icmp eq i64 %8, -1
  br i1 %9, label %10, label %14

10:                                               ; preds = %6
  %11 = add nsw i64 %3, 24
  %12 = tail call ptr @chali_host_address(i64 noundef signext %11, i64 noundef zeroext 8)
  %13 = load i64, ptr %12, align 1
  br label %16

14:                                               ; preds = %6
  %15 = add nsw i64 %3, 8
  br label %16

16:                                               ; preds = %2, %10, %14
  %17 = phi i64 [ %15, %14 ], [ %13, %10 ], [ %3, %2 ]
  %18 = tail call ptr @chali_host_address(i64 noundef signext %17, i64 noundef zeroext 0)
  %19 = load i32, ptr @chali_fmt_pos, align 4, !tbaa !15
  br label %20

20:                                               ; preds = %28, %16
  %21 = phi i32 [ %19, %16 ], [ %29, %28 ]
  %22 = getelementptr inbounds nuw i8, ptr %18, i32 %21
  %23 = load i8, ptr %22, align 1, !tbaa !12
  switch i8 %23, label %28 [
    i8 0, label %30
    i8 37, label %30
    i8 123, label %24
  ]

24:                                               ; preds = %20
  %25 = getelementptr i8, ptr %22, i32 1
  %26 = load i8, ptr %25, align 1, !tbaa !12
  %27 = icmp eq i8 %26, 125
  br i1 %27, label %30, label %28

28:                                               ; preds = %20, %24
  %29 = add i32 %21, 1
  br label %20, !llvm.loop !16

30:                                               ; preds = %20, %20, %24
  %31 = icmp ugt i32 %21, %19
  br i1 %31, label %32, label %37

32:                                               ; preds = %30
  %33 = getelementptr inbounds nuw i8, ptr %18, i32 %19
  %34 = sub nuw i32 %21, %19
  %35 = load ptr, ptr @stdout, align 4, !tbaa !9
  %36 = tail call i32 @fwrite(ptr noundef %33, i32 noundef signext 1, i32 noundef signext %34, ptr noundef %35)
  store i32 %21, ptr @chali_fmt_pos, align 4, !tbaa !15
  br label %37

37:                                               ; preds = %32, %30
  %38 = load i8, ptr %22, align 1, !tbaa !12
  switch i8 %38, label %83 [
    i8 0, label %39
    i8 123, label %40
  ]

39:                                               ; preds = %37
  store i8 0, ptr %0, align 1, !tbaa !12
  br label %139

40:                                               ; preds = %37
  %41 = add i32 %21, 1
  br label %42

42:                                               ; preds = %46, %40
  %43 = phi i32 [ %41, %40 ], [ %47, %46 ]
  %44 = getelementptr inbounds nuw i8, ptr %18, i32 %43
  %45 = load i8, ptr %44, align 1, !tbaa !12
  switch i8 %45, label %46 [
    i8 0, label %48
    i8 125, label %48
  ]

46:                                               ; preds = %42
  %47 = add i32 %43, 1
  br label %42, !llvm.loop !19

48:                                               ; preds = %42, %42
  %49 = getelementptr inbounds nuw i8, ptr %18, i32 %43
  %50 = icmp eq i8 %45, 125
  %51 = zext i1 %50 to i32
  %52 = add i32 %43, %51
  store i32 %52, ptr @chali_fmt_pos, align 4, !tbaa !15
  store i8 37, ptr %0, align 1, !tbaa !12
  %53 = getelementptr inbounds nuw i8, ptr %0, i32 1
  store i8 0, ptr %53, align 1, !tbaa !12
  %54 = getelementptr inbounds nuw i8, ptr %18, i32 %41
  %55 = load i8, ptr %54, align 1, !tbaa !12
  %56 = icmp eq i8 %55, 58
  %57 = add i32 %21, 2
  %58 = select i1 %56, i32 %57, i32 %41
  %59 = icmp ugt i32 %43, %58
  br i1 %59, label %60, label %139

60:                                               ; preds = %48
  %61 = add i32 %58, 1
  %62 = icmp ult i32 %61, %43
  %63 = icmp ugt i32 %1, 3
  %64 = and i1 %62, %63
  br i1 %64, label %70, label %65

65:                                               ; preds = %70, %60
  %66 = phi i32 [ 1, %60 ], [ %76, %70 ]
  %67 = getelementptr inbounds nuw i8, ptr %0, i32 %66
  store i8 0, ptr %67, align 1, !tbaa !12
  %68 = getelementptr i8, ptr %49, i32 -1
  %69 = load i8, ptr %68, align 1, !tbaa !12
  br label %139

70:                                               ; preds = %60, %70
  %71 = phi i32 [ %78, %70 ], [ %61, %60 ]
  %72 = phi i32 [ %71, %70 ], [ %58, %60 ]
  %73 = phi i32 [ %76, %70 ], [ 1, %60 ]
  %74 = getelementptr inbounds nuw i8, ptr %18, i32 %72
  %75 = load i8, ptr %74, align 1, !tbaa !12
  %76 = add nuw i32 %73, 1
  %77 = getelementptr inbounds nuw i8, ptr %0, i32 %73
  store i8 %75, ptr %77, align 1, !tbaa !12
  %78 = add nuw i32 %71, 1
  %79 = icmp ult i32 %78, %43
  %80 = add nuw i32 %73, 3
  %81 = icmp ult i32 %80, %1
  %82 = and i1 %79, %81
  br i1 %82, label %70, label %65, !llvm.loop !20

83:                                               ; preds = %37
  %84 = add i32 %21, 1
  %85 = getelementptr inbounds nuw i8, ptr %18, i32 %84
  %86 = load i8, ptr %85, align 1, !tbaa !12
  %87 = icmp eq i8 %86, 37
  br i1 %87, label %88, label %92

88:                                               ; preds = %83
  %89 = load ptr, ptr @stdout, align 4, !tbaa !9
  %90 = tail call i32 @fputc(i32 noundef signext 37, ptr noundef %89)
  %91 = add i32 %21, 2
  store i32 %91, ptr @chali_fmt_pos, align 4, !tbaa !15
  store i8 0, ptr %0, align 1, !tbaa !12
  br label %139

92:                                               ; preds = %83
  store i8 37, ptr %0, align 1, !tbaa !12
  %93 = getelementptr inbounds nuw i8, ptr %18, i32 %84
  %94 = load i8, ptr %93, align 1, !tbaa !12
  %95 = icmp eq i8 %94, 0
  br i1 %95, label %113, label %96

96:                                               ; preds = %92, %106
  %97 = phi i8 [ %111, %106 ], [ %94, %92 ]
  %98 = phi i32 [ %108, %106 ], [ 1, %92 ]
  %99 = phi i32 [ %107, %106 ], [ %84, %92 ]
  %100 = sext i8 %97 to i32
  %101 = tail call ptr @memchr(ptr noundef nonnull dereferenceable(1) @.str.9, i32 %100, i32 16)
  %102 = icmp ne ptr %101, null
  %103 = add nuw i32 %98, 2
  %104 = icmp ult i32 %103, %1
  %105 = select i1 %102, i1 %104, i1 false
  br i1 %105, label %106, label %113

106:                                              ; preds = %96
  %107 = add i32 %99, 1
  %108 = add nuw i32 %98, 1
  %109 = getelementptr inbounds nuw i8, ptr %0, i32 %98
  store i8 %97, ptr %109, align 1, !tbaa !12
  %110 = getelementptr inbounds nuw i8, ptr %18, i32 %107
  %111 = load i8, ptr %110, align 1, !tbaa !12
  %112 = icmp eq i8 %111, 0
  br i1 %112, label %113, label %96, !llvm.loop !21

113:                                              ; preds = %106, %96, %92
  %114 = phi i32 [ %84, %92 ], [ %107, %106 ], [ %99, %96 ]
  %115 = phi i32 [ 1, %92 ], [ %108, %106 ], [ %98, %96 ]
  %116 = getelementptr inbounds nuw i8, ptr %18, i32 %114
  %117 = load i8, ptr %116, align 1, !tbaa !12
  %118 = icmp eq i8 %117, 0
  br i1 %118, label %130, label %119

119:                                              ; preds = %113, %125
  %120 = phi i8 [ %128, %125 ], [ %117, %113 ]
  %121 = phi i32 [ %126, %125 ], [ %114, %113 ]
  %122 = sext i8 %120 to i32
  %123 = tail call ptr @memchr(ptr noundef nonnull dereferenceable(1) @.str.10, i32 %122, i32 8)
  %124 = icmp eq ptr %123, null
  br i1 %124, label %130, label %125

125:                                              ; preds = %119
  %126 = add i32 %121, 1
  %127 = getelementptr inbounds nuw i8, ptr %18, i32 %126
  %128 = load i8, ptr %127, align 1, !tbaa !12
  %129 = icmp eq i8 %128, 0
  br i1 %129, label %130, label %119, !llvm.loop !22

130:                                              ; preds = %119, %125, %113
  %131 = phi i32 [ %114, %113 ], [ %121, %119 ], [ %126, %125 ]
  %132 = getelementptr inbounds nuw i8, ptr %18, i32 %131
  %133 = getelementptr inbounds nuw i8, ptr %0, i32 %115
  store i8 0, ptr %133, align 1, !tbaa !12
  %134 = load i8, ptr %132, align 1, !tbaa !12
  %135 = icmp eq i8 %134, 0
  br i1 %135, label %136, label %137

136:                                              ; preds = %130
  store i32 %131, ptr @chali_fmt_pos, align 4, !tbaa !15
  br label %139

137:                                              ; preds = %130
  %138 = add i32 %131, 1
  store i32 %138, ptr @chali_fmt_pos, align 4, !tbaa !15
  br label %139

139:                                              ; preds = %88, %137, %136, %65, %48, %39
  %140 = phi i8 [ 0, %39 ], [ 63, %48 ], [ %69, %65 ], [ 37, %88 ], [ 0, %136 ], [ %134, %137 ]
  %141 = zext i8 %140 to i32
  ret i32 %141
}

; Function Attrs: nofree nounwind
declare noundef i32 @fwrite(ptr noundef readonly captures(none), i32 noundef signext, i32 noundef signext, ptr noundef captures(none)) local_unnamed_addr #3

; Function Attrs: nofree nounwind
declare noundef i32 @fputc(i32 noundef signext, ptr noundef captures(none)) local_unnamed_addr #3

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare ptr @strchr(ptr noundef, i32 noundef signext) local_unnamed_addr #10

; Function Attrs: uwtable(sync)
define dso_local void @chali_fmt_emit(i32 noundef signext %0, ptr noundef %1, i64 noundef signext %2, i32 noundef signext %3) local_unnamed_addr #5 {
  %5 = alloca [64 x i8], align 1
  %6 = alloca [40 x i8], align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %5) #22
  call void @llvm.lifetime.start.p0(ptr nonnull %6) #22
  switch i32 %0, label %52 [
    i32 115, label %7
    i32 99, label %44
    i32 63, label %49
  ]

7:                                                ; preds = %4
  %8 = icmp eq i64 %2, 0
  br i1 %8, label %77, label %9

9:                                                ; preds = %7
  %10 = call i32 (ptr, i32, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i32 noundef signext 64, ptr noundef nonnull @.str.11, ptr noundef %1) #22
  %11 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %12 = icmp slt i64 %2, %11
  br i1 %12, label %13, label %16

13:                                               ; preds = %9
  %14 = tail call ptr @chali_host_address(i64 noundef signext %2, i64 noundef zeroext 1)
  %15 = tail call i32 @strlen(ptr noundef nonnull dereferenceable(1) %14) #22
  br label %26

16:                                               ; preds = %9
  %17 = tail call ptr @chali_host_address(i64 noundef signext %2, i64 noundef zeroext 8)
  %18 = load i64, ptr %17, align 1
  %19 = icmp eq i64 %18, -1
  %20 = trunc i64 %18 to i32
  br i1 %19, label %21, label %26

21:                                               ; preds = %16
  %22 = add nsw i64 %2, 8
  %23 = tail call ptr @chali_host_address(i64 noundef signext %22, i64 noundef zeroext 8)
  %24 = load i64, ptr %23, align 1
  %25 = trunc i64 %24 to i32
  br label %26

26:                                               ; preds = %13, %16, %21
  %27 = phi i32 [ %15, %13 ], [ %20, %16 ], [ %25, %21 ]
  %28 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %29 = icmp slt i64 %2, %28
  br i1 %29, label %40, label %30

30:                                               ; preds = %26
  %31 = tail call ptr @chali_host_address(i64 noundef signext %2, i64 noundef zeroext 8)
  %32 = load i64, ptr %31, align 1
  %33 = icmp eq i64 %32, -1
  br i1 %33, label %34, label %38

34:                                               ; preds = %30
  %35 = add nsw i64 %2, 24
  %36 = tail call ptr @chali_host_address(i64 noundef signext %35, i64 noundef zeroext 8)
  %37 = load i64, ptr %36, align 1
  br label %40

38:                                               ; preds = %30
  %39 = add nsw i64 %2, 8
  br label %40

40:                                               ; preds = %26, %34, %38
  %41 = phi i64 [ %39, %38 ], [ %37, %34 ], [ %2, %26 ]
  %42 = tail call ptr @chali_host_address(i64 noundef signext %41, i64 noundef zeroext 0)
  %43 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %5, i32 noundef signext %27, ptr noundef %42)
  br label %77

44:                                               ; preds = %4
  %45 = trunc i64 %2 to i32
  %46 = and i32 %45, 255
  %47 = load ptr, ptr @stdout, align 4, !tbaa !9
  %48 = tail call i32 @fputc(i32 noundef signext %46, ptr noundef %47)
  br label %77

49:                                               ; preds = %4
  %50 = icmp eq i32 %3, 0
  %51 = select i1 %50, i32 100, i32 103
  br label %52

52:                                               ; preds = %4, %49
  %53 = phi i32 [ %51, %49 ], [ %0, %4 ]
  %54 = tail call ptr @memchr(ptr noundef nonnull dereferenceable(1) @.str.12, i32 %53, i32 9)
  %55 = icmp eq ptr %54, null
  br i1 %55, label %70, label %56

56:                                               ; preds = %52
  %57 = bitcast i64 %2 to double
  %58 = call i32 (ptr, i32, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %6, i32 noundef signext 40, ptr noundef nonnull @.str.13, ptr noundef %1) #22
  %59 = call ptr @strchr(ptr noundef nonnull dereferenceable(1) %6, i32 noundef signext 46) #22
  %60 = icmp eq ptr %59, null
  br i1 %60, label %61, label %65

61:                                               ; preds = %56
  %62 = call i32 @strlen(ptr noundef nonnull dereferenceable(1) %6) #22
  %63 = sub i32 39, %62
  %64 = call ptr @strncat(ptr noundef nonnull dereferenceable(1) %6, ptr noundef nonnull @.str.14, i32 noundef signext %63) #22
  br label %65

65:                                               ; preds = %61, %56
  %66 = shl i32 %53, 24
  %67 = ashr exact i32 %66, 24
  %68 = call i32 (ptr, i32, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i32 noundef signext 64, ptr noundef nonnull @.str.15, ptr noundef nonnull %6, i32 noundef signext %67) #22
  %69 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %5, double noundef %57)
  br label %77

70:                                               ; preds = %52
  switch i32 %53, label %74 [
    i32 120, label %71
    i32 117, label %71
    i32 111, label %71
    i32 88, label %71
  ]

71:                                               ; preds = %70, %70, %70, %70
  %72 = call i32 (ptr, i32, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i32 noundef signext 64, ptr noundef nonnull @.str.16, ptr noundef %1, i32 noundef signext %53) #22
  %73 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef zeroext %2)
  br label %77

74:                                               ; preds = %70
  %75 = call i32 (ptr, i32, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i32 noundef signext 64, ptr noundef nonnull @.str.17, ptr noundef %1) #22
  %76 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef signext %2)
  br label %77

77:                                               ; preds = %7, %74, %71, %65, %44, %40
  call void @llvm.lifetime.end.p0(ptr nonnull %6) #22
  call void @llvm.lifetime.end.p0(ptr nonnull %5) #22
  ret void
}

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr noundef readonly captures(none), ...) local_unnamed_addr #3

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare ptr @strncat(ptr noalias noundef returned, ptr noalias noundef readonly captures(none), i32 noundef signext) local_unnamed_addr #12

; Function Attrs: uwtable(sync)
define dso_local void @chali_fmt_int(i64 noundef signext %0) local_unnamed_addr #5 {
  %2 = alloca [32 x i8], align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #22
  %3 = call i32 @chali_fmt_scan(ptr noundef nonnull %2, i32 noundef signext 32)
  %4 = trunc nuw i32 %3 to i8
  switch i8 %4, label %5 [
    i8 37, label %6
    i8 0, label %6
  ]

5:                                                ; preds = %1
  call void @chali_fmt_emit(i32 noundef signext %3, ptr noundef nonnull %2, i64 noundef signext %0, i32 noundef signext 0)
  br label %6

6:                                                ; preds = %1, %1, %5
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #22
  ret void
}

; Function Attrs: uwtable(sync)
define dso_local void @chali_fmt_uint(i64 noundef signext %0) local_unnamed_addr #5 {
  %2 = alloca [32 x i8], align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #22
  %3 = call i32 @chali_fmt_scan(ptr noundef nonnull %2, i32 noundef signext 32)
  %4 = trunc nuw i32 %3 to i8
  switch i8 %4, label %5 [
    i8 37, label %6
    i8 0, label %6
  ]

5:                                                ; preds = %1
  call void @chali_fmt_emit(i32 noundef signext %3, ptr noundef nonnull %2, i64 noundef signext %0, i32 noundef signext 0)
  br label %6

6:                                                ; preds = %1, %1, %5
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #22
  ret void
}

; Function Attrs: uwtable(sync)
define dso_local void @chali_fmt_char(i64 noundef signext %0) local_unnamed_addr #5 {
  %2 = alloca [32 x i8], align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #22
  %3 = call i32 @chali_fmt_scan(ptr noundef nonnull %2, i32 noundef signext 32)
  %4 = trunc nuw i32 %3 to i8
  switch i8 %4, label %5 [
    i8 37, label %6
    i8 0, label %6
  ]

5:                                                ; preds = %1
  call void @chali_fmt_emit(i32 noundef signext %3, ptr noundef nonnull %2, i64 noundef signext %0, i32 noundef signext 0)
  br label %6

6:                                                ; preds = %1, %1, %5
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #22
  ret void
}

; Function Attrs: uwtable(sync)
define dso_local void @chali_fmt_double(double noundef %0) local_unnamed_addr #5 {
  %2 = alloca [32 x i8], align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #22
  %3 = call i32 @chali_fmt_scan(ptr noundef nonnull %2, i32 noundef signext 32)
  %4 = trunc nuw i32 %3 to i8
  switch i8 %4, label %5 [
    i8 37, label %7
    i8 0, label %7
  ]

5:                                                ; preds = %1
  %6 = bitcast double %0 to i64
  call void @chali_fmt_emit(i32 noundef signext %3, ptr noundef nonnull %2, i64 noundef signext %6, i32 noundef signext 1)
  br label %7

7:                                                ; preds = %1, %1, %5
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #22
  ret void
}

; Function Attrs: uwtable(sync)
define dso_local void @chali_fmt_string(i64 noundef signext %0) local_unnamed_addr #5 {
  %2 = alloca [32 x i8], align 1
  %3 = alloca [48 x i8], align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #22
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #22
  %4 = call i32 @chali_fmt_scan(ptr noundef nonnull %2, i32 noundef signext 32)
  %5 = icmp eq i64 %0, 0
  br i1 %5, label %41, label %6

6:                                                ; preds = %1
  %7 = trunc nuw i32 %4 to i8
  switch i8 %7, label %8 [
    i8 37, label %41
    i8 0, label %41
  ]

8:                                                ; preds = %6
  %9 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %10 = icmp slt i64 %0, %9
  br i1 %10, label %21, label %11

11:                                               ; preds = %8
  %12 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 8)
  %13 = load i64, ptr %12, align 1
  %14 = icmp eq i64 %13, -1
  br i1 %14, label %15, label %19

15:                                               ; preds = %11
  %16 = add nsw i64 %0, 24
  %17 = tail call ptr @chali_host_address(i64 noundef signext %16, i64 noundef zeroext 8)
  %18 = load i64, ptr %17, align 1
  br label %21

19:                                               ; preds = %11
  %20 = add nsw i64 %0, 8
  br label %21

21:                                               ; preds = %8, %15, %19
  %22 = phi i64 [ %20, %19 ], [ %18, %15 ], [ %0, %8 ]
  %23 = tail call ptr @chali_host_address(i64 noundef signext %22, i64 noundef zeroext 0)
  br i1 %10, label %24, label %27

24:                                               ; preds = %21
  %25 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 1)
  %26 = tail call i32 @strlen(ptr noundef nonnull dereferenceable(1) %25) #22
  br label %37

27:                                               ; preds = %21
  %28 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 8)
  %29 = load i64, ptr %28, align 1
  %30 = icmp eq i64 %29, -1
  %31 = trunc i64 %29 to i32
  br i1 %30, label %32, label %37

32:                                               ; preds = %27
  %33 = add nsw i64 %0, 8
  %34 = tail call ptr @chali_host_address(i64 noundef signext %33, i64 noundef zeroext 8)
  %35 = load i64, ptr %34, align 1
  %36 = trunc i64 %35 to i32
  br label %37

37:                                               ; preds = %24, %27, %32
  %38 = phi i32 [ %26, %24 ], [ %31, %27 ], [ %36, %32 ]
  %39 = call i32 (ptr, i32, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i32 noundef signext 48, ptr noundef nonnull @.str.11, ptr noundef nonnull %2) #22
  %40 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %3, i32 noundef signext %38, ptr noundef %23)
  br label %41

41:                                               ; preds = %1, %6, %6, %37
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #22
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #22
  ret void
}

; Function Attrs: uwtable(sync)
define dso_local void @chali_fmt_finish() local_unnamed_addr #5 {
  %1 = load i64, ptr @chali_fmt_at, align 8, !tbaa !13
  %2 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %3 = icmp slt i64 %1, %2
  br i1 %3, label %14, label %4

4:                                                ; preds = %0
  %5 = tail call ptr @chali_host_address(i64 noundef signext %1, i64 noundef zeroext 8)
  %6 = load i64, ptr %5, align 1
  %7 = icmp eq i64 %6, -1
  br i1 %7, label %8, label %12

8:                                                ; preds = %4
  %9 = add nsw i64 %1, 24
  %10 = tail call ptr @chali_host_address(i64 noundef signext %9, i64 noundef zeroext 8)
  %11 = load i64, ptr %10, align 1
  br label %14

12:                                               ; preds = %4
  %13 = add nsw i64 %1, 8
  br label %14

14:                                               ; preds = %0, %8, %12
  %15 = phi i64 [ %13, %12 ], [ %11, %8 ], [ %1, %0 ]
  %16 = tail call ptr @chali_host_address(i64 noundef signext %15, i64 noundef zeroext 0)
  %17 = load i32, ptr @chali_fmt_pos, align 4, !tbaa !15
  br label %18

18:                                               ; preds = %26, %14
  %19 = phi i32 [ %17, %14 ], [ %31, %26 ]
  %20 = getelementptr inbounds nuw i8, ptr %16, i32 %19
  %21 = load i8, ptr %20, align 1, !tbaa !12
  switch i8 %21, label %32 [
    i8 0, label %34
    i8 37, label %22
  ]

22:                                               ; preds = %18
  %23 = getelementptr i8, ptr %20, i32 1
  %24 = load i8, ptr %23, align 1, !tbaa !12
  %25 = icmp eq i8 %24, 37
  br i1 %25, label %26, label %32

26:                                               ; preds = %22, %32
  %27 = phi i32 [ %33, %32 ], [ 37, %22 ]
  %28 = phi i32 [ 1, %32 ], [ 2, %22 ]
  %29 = load ptr, ptr @stdout, align 4, !tbaa !9
  %30 = tail call i32 @fputc(i32 noundef signext %27, ptr noundef %29)
  %31 = add i32 %19, %28
  br label %18, !llvm.loop !23

32:                                               ; preds = %18, %22
  %33 = sext i8 %21 to i32
  br label %26

34:                                               ; preds = %18
  store i32 %19, ptr @chali_fmt_pos, align 4, !tbaa !15
  ret void
}

; Function Attrs: uwtable(sync)
define dso_local range(i64 -1, 2) i64 @chali_string_cmp(i64 noundef signext %0, i64 noundef signext %1) local_unnamed_addr #5 {
  %3 = icmp eq i64 %0, 0
  br i1 %3, label %20, label %4

4:                                                ; preds = %2
  %5 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %6 = icmp slt i64 %0, %5
  br i1 %6, label %7, label %10

7:                                                ; preds = %4
  %8 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 1)
  %9 = tail call i32 @strlen(ptr noundef nonnull dereferenceable(1) %8) #22
  br label %20

10:                                               ; preds = %4
  %11 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 8)
  %12 = load i64, ptr %11, align 1
  %13 = icmp eq i64 %12, -1
  %14 = trunc i64 %12 to i32
  br i1 %13, label %15, label %20

15:                                               ; preds = %10
  %16 = add nsw i64 %0, 8
  %17 = tail call ptr @chali_host_address(i64 noundef signext %16, i64 noundef zeroext 8)
  %18 = load i64, ptr %17, align 1
  %19 = trunc i64 %18 to i32
  br label %20

20:                                               ; preds = %15, %10, %7, %2
  %21 = phi i32 [ 0, %2 ], [ %9, %7 ], [ %14, %10 ], [ %19, %15 ]
  %22 = icmp eq i64 %1, 0
  br i1 %22, label %39, label %23

23:                                               ; preds = %20
  %24 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %25 = icmp slt i64 %1, %24
  br i1 %25, label %26, label %29

26:                                               ; preds = %23
  %27 = tail call ptr @chali_host_address(i64 noundef signext %1, i64 noundef zeroext 1)
  %28 = tail call i32 @strlen(ptr noundef nonnull dereferenceable(1) %27) #22
  br label %39

29:                                               ; preds = %23
  %30 = tail call ptr @chali_host_address(i64 noundef signext %1, i64 noundef zeroext 8)
  %31 = load i64, ptr %30, align 1
  %32 = icmp eq i64 %31, -1
  %33 = trunc i64 %31 to i32
  br i1 %32, label %34, label %39

34:                                               ; preds = %29
  %35 = add nsw i64 %1, 8
  %36 = tail call ptr @chali_host_address(i64 noundef signext %35, i64 noundef zeroext 8)
  %37 = load i64, ptr %36, align 1
  %38 = trunc i64 %37 to i32
  br label %39

39:                                               ; preds = %34, %29, %26, %20
  %40 = phi i32 [ 0, %20 ], [ %28, %26 ], [ %33, %29 ], [ %38, %34 ]
  %41 = tail call i32 @llvm.umin.i32(i32 %21, i32 %40)
  %42 = icmp eq i32 %41, 0
  br i1 %42, label %74, label %43

43:                                               ; preds = %39
  %44 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %45 = icmp slt i64 %0, %44
  br i1 %45, label %56, label %46

46:                                               ; preds = %43
  %47 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 8)
  %48 = load i64, ptr %47, align 1
  %49 = icmp eq i64 %48, -1
  br i1 %49, label %50, label %54

50:                                               ; preds = %46
  %51 = add nsw i64 %0, 24
  %52 = tail call ptr @chali_host_address(i64 noundef signext %51, i64 noundef zeroext 8)
  %53 = load i64, ptr %52, align 1
  br label %56

54:                                               ; preds = %46
  %55 = add nsw i64 %0, 8
  br label %56

56:                                               ; preds = %43, %50, %54
  %57 = phi i64 [ %55, %54 ], [ %53, %50 ], [ %0, %43 ]
  %58 = tail call ptr @chali_host_address(i64 noundef signext %57, i64 noundef zeroext 0)
  %59 = icmp slt i64 %1, %44
  br i1 %59, label %70, label %60

60:                                               ; preds = %56
  %61 = tail call ptr @chali_host_address(i64 noundef signext %1, i64 noundef zeroext 8)
  %62 = load i64, ptr %61, align 1
  %63 = icmp eq i64 %62, -1
  br i1 %63, label %64, label %68

64:                                               ; preds = %60
  %65 = add nsw i64 %1, 24
  %66 = tail call ptr @chali_host_address(i64 noundef signext %65, i64 noundef zeroext 8)
  %67 = load i64, ptr %66, align 1
  br label %70

68:                                               ; preds = %60
  %69 = add nsw i64 %1, 8
  br label %70

70:                                               ; preds = %56, %64, %68
  %71 = phi i64 [ %69, %68 ], [ %67, %64 ], [ %1, %56 ]
  %72 = tail call ptr @chali_host_address(i64 noundef signext %71, i64 noundef zeroext 0)
  %73 = tail call i32 @memcmp(ptr noundef %58, ptr noundef %72, i32 noundef signext %41)
  br label %74

74:                                               ; preds = %39, %70
  %75 = phi i32 [ %73, %70 ], [ 0, %39 ]
  %76 = icmp eq i32 %75, 0
  %77 = icmp sgt i32 %75, -1
  %78 = select i1 %77, i32 1, i32 -1
  %79 = tail call i32 @llvm.ucmp.i32.i32(i32 %21, i32 %40)
  %80 = select i1 %76, i32 %79, i32 %78
  %81 = sext i32 %80 to i64
  ret i64 %81
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare i32 @memcmp(ptr noundef captures(none), ptr noundef captures(none), i32 noundef signext) local_unnamed_addr #10

; Function Attrs: uwtable(sync)
define dso_local i64 @chali_string_sub(i64 noundef signext %0, i64 noundef signext %1, i64 noundef signext %2) local_unnamed_addr #5 {
  %4 = icmp eq i64 %0, 0
  br i1 %4, label %5, label %13

5:                                                ; preds = %3
  %6 = tail call i64 @chali_host_allocate(i64 noundef zeroext 49)
  %7 = add nsw i64 %6, 32
  %8 = tail call ptr @chali_host_address(i64 noundef signext %6, i64 noundef zeroext 32)
  store i64 -1, ptr %8, align 1
  %9 = getelementptr inbounds nuw i8, ptr %8, i32 8
  store i64 0, ptr %9, align 1
  %10 = getelementptr inbounds nuw i8, ptr %8, i32 16
  store i64 16, ptr %10, align 1
  %11 = getelementptr inbounds nuw i8, ptr %8, i32 24
  store i64 %7, ptr %11, align 1
  %12 = tail call ptr @chali_host_address(i64 noundef signext %7, i64 noundef zeroext 1)
  store i8 0, ptr %12, align 1, !tbaa !12
  br label %55

13:                                               ; preds = %3
  %14 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %15 = icmp slt i64 %0, %14
  br i1 %15, label %16, label %20

16:                                               ; preds = %13
  %17 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 1)
  %18 = tail call i32 @strlen(ptr noundef nonnull dereferenceable(1) %17) #22
  %19 = zext i32 %18 to i64
  br label %28

20:                                               ; preds = %13
  %21 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 8)
  %22 = load i64, ptr %21, align 1
  %23 = icmp eq i64 %22, -1
  br i1 %23, label %24, label %28

24:                                               ; preds = %20
  %25 = add nsw i64 %0, 8
  %26 = tail call ptr @chali_host_address(i64 noundef signext %25, i64 noundef zeroext 8)
  %27 = load i64, ptr %26, align 1
  br label %28

28:                                               ; preds = %16, %20, %24
  %29 = phi i64 [ %19, %16 ], [ %22, %20 ], [ %27, %24 ]
  %30 = tail call i64 @llvm.smax.i64(i64 %1, i64 0)
  %31 = tail call i64 @llvm.smax.i64(i64 %2, i64 0)
  %32 = tail call i64 @llvm.smin.i64(i64 %30, i64 %29)
  %33 = sub nsw i64 %29, %32
  %34 = icmp sgt i64 %2, %33
  %35 = select i1 %34, i64 %33, i64 %31
  %36 = trunc i64 %35 to i32
  %37 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %38 = icmp slt i64 %0, %37
  br i1 %38, label %49, label %39

39:                                               ; preds = %28
  %40 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 8)
  %41 = load i64, ptr %40, align 1
  %42 = icmp eq i64 %41, -1
  br i1 %42, label %43, label %47

43:                                               ; preds = %39
  %44 = add nsw i64 %0, 24
  %45 = tail call ptr @chali_host_address(i64 noundef signext %44, i64 noundef zeroext 8)
  %46 = load i64, ptr %45, align 1
  br label %49

47:                                               ; preds = %39
  %48 = add nsw i64 %0, 8
  br label %49

49:                                               ; preds = %28, %43, %47
  %50 = phi i64 [ %48, %47 ], [ %46, %43 ], [ %0, %28 ]
  %51 = tail call ptr @chali_host_address(i64 noundef signext %50, i64 noundef zeroext 0)
  %52 = trunc i64 %32 to i32
  %53 = getelementptr inbounds i8, ptr %51, i32 %52
  %54 = tail call i64 @chali_string_new(ptr noundef %53, i32 noundef signext %36)
  br label %55

55:                                               ; preds = %49, %5
  %56 = phi i64 [ %6, %5 ], [ %54, %49 ]
  ret i64 %56
}

; Function Attrs: uwtable(sync)
define dso_local i64 @chali_string_pos(i64 noundef signext %0, i64 noundef signext %1) local_unnamed_addr #5 {
  %3 = icmp eq i64 %0, 0
  %4 = icmp eq i64 %1, 0
  %5 = or i1 %3, %4
  br i1 %5, label %81, label %6

6:                                                ; preds = %2
  %7 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %8 = icmp slt i64 %0, %7
  br i1 %8, label %9, label %13

9:                                                ; preds = %6
  %10 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 1)
  %11 = tail call i32 @strlen(ptr noundef nonnull dereferenceable(1) %10) #22
  %12 = zext i32 %11 to i64
  br label %21

13:                                               ; preds = %6
  %14 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 8)
  %15 = load i64, ptr %14, align 1
  %16 = icmp eq i64 %15, -1
  br i1 %16, label %17, label %21

17:                                               ; preds = %13
  %18 = add nsw i64 %0, 8
  %19 = tail call ptr @chali_host_address(i64 noundef signext %18, i64 noundef zeroext 8)
  %20 = load i64, ptr %19, align 1
  br label %21

21:                                               ; preds = %17, %13, %9
  %22 = phi i64 [ %12, %9 ], [ %15, %13 ], [ %20, %17 ]
  %23 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %24 = icmp slt i64 %1, %23
  br i1 %24, label %25, label %29

25:                                               ; preds = %21
  %26 = tail call ptr @chali_host_address(i64 noundef signext %1, i64 noundef zeroext 1)
  %27 = tail call i32 @strlen(ptr noundef nonnull dereferenceable(1) %26) #22
  %28 = zext i32 %27 to i64
  br label %37

29:                                               ; preds = %21
  %30 = tail call ptr @chali_host_address(i64 noundef signext %1, i64 noundef zeroext 8)
  %31 = load i64, ptr %30, align 1
  %32 = icmp eq i64 %31, -1
  br i1 %32, label %33, label %37

33:                                               ; preds = %29
  %34 = add nsw i64 %1, 8
  %35 = tail call ptr @chali_host_address(i64 noundef signext %34, i64 noundef zeroext 8)
  %36 = load i64, ptr %35, align 1
  br label %37

37:                                               ; preds = %25, %29, %33
  %38 = phi i64 [ %28, %25 ], [ %31, %29 ], [ %36, %33 ]
  %39 = icmp eq i64 %38, 0
  br i1 %39, label %81, label %40

40:                                               ; preds = %37
  %41 = icmp sgt i64 %38, %22
  br i1 %41, label %81, label %42

42:                                               ; preds = %40
  %43 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %44 = icmp slt i64 %0, %43
  %45 = add nsw i64 %0, 8
  %46 = add nsw i64 %0, 24
  %47 = icmp slt i64 %1, %43
  %48 = add nsw i64 %1, 8
  %49 = add nsw i64 %1, 24
  %50 = trunc i64 %38 to i32
  br label %51

51:                                               ; preds = %42, %77
  %52 = phi i64 [ 0, %42 ], [ %78, %77 ]
  br i1 %44, label %60, label %53

53:                                               ; preds = %51
  %54 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 8)
  %55 = load i64, ptr %54, align 1
  %56 = icmp eq i64 %55, -1
  br i1 %56, label %57, label %60

57:                                               ; preds = %53
  %58 = tail call ptr @chali_host_address(i64 noundef signext %46, i64 noundef zeroext 8)
  %59 = load i64, ptr %58, align 1
  br label %60

60:                                               ; preds = %53, %51, %57
  %61 = phi i64 [ %0, %51 ], [ %59, %57 ], [ %45, %53 ]
  %62 = tail call ptr @chali_host_address(i64 noundef signext %61, i64 noundef zeroext 0)
  %63 = trunc i64 %52 to i32
  %64 = getelementptr inbounds i8, ptr %62, i32 %63
  br i1 %47, label %72, label %65

65:                                               ; preds = %60
  %66 = tail call ptr @chali_host_address(i64 noundef signext %1, i64 noundef zeroext 8)
  %67 = load i64, ptr %66, align 1
  %68 = icmp eq i64 %67, -1
  br i1 %68, label %69, label %72

69:                                               ; preds = %65
  %70 = tail call ptr @chali_host_address(i64 noundef signext %49, i64 noundef zeroext 8)
  %71 = load i64, ptr %70, align 1
  br label %72

72:                                               ; preds = %65, %60, %69
  %73 = phi i64 [ %1, %60 ], [ %71, %69 ], [ %48, %65 ]
  %74 = tail call ptr @chali_host_address(i64 noundef signext %73, i64 noundef zeroext 0)
  %75 = tail call i32 @bcmp(ptr %64, ptr %74, i32 %50)
  %76 = icmp eq i32 %75, 0
  br i1 %76, label %81, label %77

77:                                               ; preds = %72
  %78 = add nuw nsw i64 %52, 1
  %79 = add nsw i64 %78, %38
  %80 = icmp sgt i64 %79, %22
  br i1 %80, label %81, label %51, !llvm.loop !24

81:                                               ; preds = %72, %77, %40, %37, %2
  %82 = phi i64 [ -1, %2 ], [ 0, %37 ], [ -1, %40 ], [ -1, %77 ], [ %52, %72 ]
  ret i64 %82
}

; Function Attrs: uwtable(sync)
define dso_local i64 @chali_read_line() local_unnamed_addr #5 {
  %1 = tail call dereferenceable_or_null(256) ptr @malloc(i32 noundef signext 256) #21
  %2 = icmp eq ptr %1, null
  br i1 %2, label %3, label %4

3:                                                ; preds = %0
  tail call void @chali_panic(ptr noundef nonnull @.str.19)
  unreachable

4:                                                ; preds = %0, %21
  %5 = phi ptr [ %22, %21 ], [ %1, %0 ]
  %6 = phi i32 [ %25, %21 ], [ 0, %0 ]
  %7 = phi i32 [ %23, %21 ], [ 256, %0 ]
  %8 = load ptr, ptr @stdin, align 4, !tbaa !9
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
  %12 = icmp eq i32 %6, %7
  br i1 %12, label %13, label %21

13:                                               ; preds = %11
  %14 = icmp slt i32 %7, 0
  br i1 %14, label %15, label %16

15:                                               ; preds = %13
  tail call void @chali_panic(ptr noundef nonnull @.str.21)
  unreachable

16:                                               ; preds = %13
  %17 = shl nuw i32 %7, 1
  %18 = tail call ptr @realloc(ptr noundef %5, i32 noundef signext %17) #23
  %19 = icmp eq ptr %18, null
  br i1 %19, label %20, label %21

20:                                               ; preds = %16
  tail call void @free(ptr noundef %5)
  tail call void @chali_panic(ptr noundef nonnull @.str.19)
  unreachable

21:                                               ; preds = %16, %11
  %22 = phi ptr [ %5, %11 ], [ %18, %16 ]
  %23 = phi i32 [ %7, %11 ], [ %17, %16 ]
  %24 = trunc i32 %9 to i8
  %25 = add i32 %6, 1
  %26 = getelementptr inbounds nuw i8, ptr %22, i32 %6
  store i8 %24, ptr %26, align 1, !tbaa !12
  br label %4, !llvm.loop !25

27:                                               ; preds = %4, %4
  %28 = icmp eq i32 %9, 10
  %29 = icmp ne i32 %6, 0
  %30 = select i1 %28, i1 %29, i1 false
  br i1 %30, label %31, label %38

31:                                               ; preds = %27
  %32 = getelementptr i8, ptr %5, i32 %6
  %33 = getelementptr i8, ptr %32, i32 -1
  %34 = load i8, ptr %33, align 1, !tbaa !12
  %35 = icmp eq i8 %34, 13
  %36 = sext i1 %35 to i32
  %37 = add i32 %6, %36
  br label %38

38:                                               ; preds = %31, %27
  %39 = phi i32 [ %6, %27 ], [ %37, %31 ]
  %40 = tail call i64 @chali_string_new(ptr noundef %5, i32 noundef signext %39)
  tail call void @free(ptr noundef %5)
  ret i64 %40
}

; Function Attrs: nofree nounwind
declare noundef i32 @fgetc(ptr noundef captures(none)) local_unnamed_addr #3

; Function Attrs: mustprogress nounwind willreturn allockind("realloc") allocsize(1) memory(argmem: readwrite, inaccessiblemem: readwrite, errnomem: write)
declare noalias noundef ptr @realloc(ptr allocptr noundef captures(none), i32 noundef signext) local_unnamed_addr #13

; Function Attrs: uwtable(sync)
define dso_local i64 @chali_string_fixed(double noundef %0, i64 noundef signext %1) local_unnamed_addr #5 {
  %3 = alloca [256 x i8], align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #22
  %4 = icmp slt i64 %1, 0
  %5 = trunc i64 %1 to i32
  %6 = select i1 %4, i32 3, i32 %5
  %7 = call i32 (ptr, i32, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i32 noundef signext 256, ptr noundef nonnull @.str.22, i32 noundef signext %6, double noundef %0) #22
  %8 = icmp slt i32 %7, 0
  br i1 %8, label %9, label %10

9:                                                ; preds = %2
  tail call void @chali_panic(ptr noundef nonnull @.str.23)
  unreachable

10:                                               ; preds = %2
  %11 = tail call i32 @llvm.umin.i32(i32 %7, i32 255)
  %12 = call i64 @chali_string_new(ptr noundef nonnull %3, i32 noundef signext %11)
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #22
  ret i64 %12
}

; Function Attrs: nofree nounwind uwtable(sync)
define dso_local i64 @chali_read_number(i32 noundef signext %0) local_unnamed_addr #14 {
  %2 = alloca [256 x i8], align 1
  %3 = alloca double, align 8
  %4 = alloca i64, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #22
  %5 = load ptr, ptr @stdin, align 4, !tbaa !9
  %6 = call ptr @fgets(ptr noundef nonnull %2, i32 noundef signext 256, ptr noundef %5)
  %7 = icmp eq ptr %6, null
  br i1 %7, label %22, label %8

8:                                                ; preds = %1
  %9 = icmp eq i32 %0, 0
  br i1 %9, label %16, label %10

10:                                               ; preds = %8
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #22
  store double 0.000000e+00, ptr %3, align 8, !tbaa !26
  %11 = call i32 (ptr, ptr, ...) @sscanf(ptr noundef nonnull %2, ptr noundef nonnull @.str.24, ptr noundef nonnull %3)
  %12 = icmp eq i32 %11, 1
  br i1 %12, label %14, label %13

13:                                               ; preds = %10
  store double 0.000000e+00, ptr %3, align 8, !tbaa !26
  br label %14

14:                                               ; preds = %13, %10
  %15 = load i64, ptr %3, align 8, !tbaa !26
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #22
  br label %22

16:                                               ; preds = %8
  call void @llvm.lifetime.start.p0(ptr nonnull %4) #22
  store i64 0, ptr %4, align 8, !tbaa !13
  %17 = call i32 (ptr, ptr, ...) @sscanf(ptr noundef nonnull %2, ptr noundef nonnull @.str.25, ptr noundef nonnull %4)
  %18 = icmp eq i32 %17, 1
  br i1 %18, label %20, label %19

19:                                               ; preds = %16
  store i64 0, ptr %4, align 8, !tbaa !13
  br label %20

20:                                               ; preds = %19, %16
  %21 = load i64, ptr %4, align 8, !tbaa !13
  call void @llvm.lifetime.end.p0(ptr nonnull %4) #22
  br label %22

22:                                               ; preds = %1, %20, %14
  %23 = phi i64 [ %21, %20 ], [ %15, %14 ], [ 0, %1 ]
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #22
  ret i64 %23
}

; Function Attrs: nofree nounwind
declare noundef ptr @fgets(ptr noundef writeonly, i32 noundef signext, ptr noundef captures(none)) local_unnamed_addr #3

; Function Attrs: nofree nounwind
declare noundef i32 @sscanf(ptr noundef readonly captures(none), ptr noundef readonly captures(none), ...) local_unnamed_addr #3

; Function Attrs: nofree nounwind uwtable(sync)
define dso_local range(i64 0, 2) i64 @chali_stdin_eof() local_unnamed_addr #14 {
  %1 = load ptr, ptr @stdin, align 4, !tbaa !9
  %2 = tail call i32 @feof(ptr noundef %1)
  %3 = icmp ne i32 %2, 0
  %4 = zext i1 %3 to i64
  ret i64 %4
}

; Function Attrs: nofree nounwind
declare noundef i32 @feof(ptr noundef captures(none)) local_unnamed_addr #3

; Function Attrs: uwtable(sync)
define dso_local i64 @chali_string_of_int(i64 noundef signext %0) local_unnamed_addr #5 {
  %2 = alloca [32 x i8], align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #22
  %3 = call i32 (ptr, i32, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %2, i32 noundef signext 32, ptr noundef nonnull @.str.25, i64 noundef signext %0) #22
  %4 = call i32 @strlen(ptr noundef nonnull dereferenceable(1) %2) #22
  %5 = call i64 @chali_string_new(ptr noundef nonnull %2, i32 noundef signext %4)
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #22
  ret i64 %5
}

; Function Attrs: uwtable(sync)
define dso_local i64 @chali_string_of_double(double noundef %0) local_unnamed_addr #5 {
  %2 = alloca [64 x i8], align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #22
  %3 = call i32 (ptr, i32, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %2, i32 noundef signext 64, ptr noundef nonnull @.str.26, double noundef %0) #22
  %4 = call i32 @strlen(ptr noundef nonnull dereferenceable(1) %2) #22
  %5 = call i64 @chali_string_new(ptr noundef nonnull %2, i32 noundef signext %4)
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #22
  ret i64 %5
}

; Function Attrs: uwtable(sync)
define dso_local void @chali_check(i64 noundef signext %0, i64 noundef signext %1) local_unnamed_addr #5 {
  %3 = alloca [128 x i8], align 1
  %4 = icmp sgt i64 %0, -1
  %5 = icmp slt i64 %0, %1
  %6 = and i1 %4, %5
  br i1 %6, label %9, label %7

7:                                                ; preds = %2
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #22
  %8 = call i32 (ptr, i32, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i32 noundef signext 128, ptr noundef nonnull @.str.7, ptr noundef nonnull @.str.27, i64 noundef signext %0, i64 noundef signext %1) #22
  call void @chali_panic(ptr noundef nonnull %3)
  unreachable

9:                                                ; preds = %2
  ret void
}

; Function Attrs: uwtable(sync)
define dso_local void @chali_check_pointer(i64 noundef signext %0) local_unnamed_addr #5 {
  %2 = alloca [128 x i8], align 1
  %3 = icmp eq i64 %0, 0
  br i1 %3, label %4, label %6

4:                                                ; preds = %1
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #22
  %5 = call i32 (ptr, i32, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %2, i32 noundef signext 128, ptr noundef nonnull @.str.8, ptr noundef nonnull @.str.28) #22
  call void @chali_panic(ptr noundef nonnull %2)
  unreachable

6:                                                ; preds = %1
  ret void
}

; Function Attrs: uwtable(sync)
define dso_local range(i64 1, 0) i64 @chali_require_reference(i64 noundef returned signext %0) local_unnamed_addr #5 {
  %2 = icmp eq i64 %0, 0
  br i1 %2, label %3, label %4

3:                                                ; preds = %1
  tail call void @chali_panic(ptr noundef nonnull @.str.29)
  unreachable

4:                                                ; preds = %1
  ret i64 %0
}

; Function Attrs: nofree nounwind uwtable(sync)
define dso_local void @chali_put_char(i64 noundef signext %0) local_unnamed_addr #14 {
  %2 = trunc i64 %0 to i32
  %3 = and i32 %2, 255
  %4 = load ptr, ptr @stdout, align 4, !tbaa !9
  %5 = tail call i32 @fputc(i32 noundef signext %3, ptr noundef %4)
  ret void
}

; Function Attrs: noreturn uwtable(sync)
define dso_local void @chali_abort(i64 noundef signext %0) local_unnamed_addr #15 {
  %2 = icmp eq i64 %0, 0
  br i1 %2, label %4, label %3

3:                                                ; preds = %1
  store i64 %0, ptr @chali_fmt_at, align 8, !tbaa !13
  store i32 0, ptr @chali_fmt_pos, align 4, !tbaa !15
  tail call void @chali_fmt_finish()
  br label %4

4:                                                ; preds = %3, %1
  tail call void @chali_panic(ptr noundef nonnull @.str.30)
  unreachable
}

; Function Attrs: uwtable(sync)
define dso_local void @chali_assert(i64 noundef signext %0, i64 noundef signext %1) local_unnamed_addr #5 {
  %3 = icmp eq i64 %0, 0
  br i1 %3, label %4, label %8

4:                                                ; preds = %2
  %5 = icmp eq i64 %1, 0
  br i1 %5, label %7, label %6

6:                                                ; preds = %4
  store i64 %1, ptr @chali_fmt_at, align 8, !tbaa !13
  store i32 0, ptr @chali_fmt_pos, align 4, !tbaa !15
  tail call void @chali_fmt_finish()
  br label %7

7:                                                ; preds = %6, %4
  tail call void @chali_panic(ptr noundef nonnull @.str.30)
  unreachable

8:                                                ; preds = %2
  ret void
}

; Function Attrs: uwtable(sync)
define dso_local noundef i64 @chali_copy_bytes(i64 noundef returned signext %0, i64 noundef signext %1, i64 noundef signext %2) local_unnamed_addr #5 {
  %4 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext %2)
  %5 = tail call ptr @chali_host_address(i64 noundef signext %1, i64 noundef zeroext %2)
  %6 = trunc i64 %2 to i32
  tail call void @llvm.memmove.p0.p0.i32(ptr align 1 %4, ptr align 1 %5, i32 %6, i1 false)
  ret i64 %0
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memmove.p0.p0.i32(ptr writeonly captures(none), ptr readonly captures(none), i32, i1 immarg) #6

; Function Attrs: uwtable(sync)
define dso_local noundef i64 @chali_set_bytes(i64 noundef returned signext %0, i64 noundef signext %1, i64 noundef signext %2) local_unnamed_addr #5 {
  %4 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext %2)
  %5 = trunc i64 %1 to i8
  %6 = trunc i64 %2 to i32
  tail call void @llvm.memset.p0.i32(ptr align 1 %4, i8 %5, i32 %6, i1 false)
  ret i64 %0
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i32(ptr writeonly captures(none), i8, i32, i1 immarg) #16

; Function Attrs: uwtable(sync)
define dso_local range(i64 0, 256) i64 @chali_string_at(i64 noundef signext %0, i64 noundef signext %1) local_unnamed_addr #5 {
  %3 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %4 = icmp slt i64 %0, %3
  br i1 %4, label %15, label %5

5:                                                ; preds = %2
  %6 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 8)
  %7 = load i64, ptr %6, align 1
  %8 = icmp eq i64 %7, -1
  br i1 %8, label %9, label %13

9:                                                ; preds = %5
  %10 = add nsw i64 %0, 24
  %11 = tail call ptr @chali_host_address(i64 noundef signext %10, i64 noundef zeroext 8)
  %12 = load i64, ptr %11, align 1
  br label %15

13:                                               ; preds = %5
  %14 = add nsw i64 %0, 8
  br label %15

15:                                               ; preds = %2, %9, %13
  %16 = phi i64 [ %14, %13 ], [ %12, %9 ], [ %0, %2 ]
  %17 = tail call ptr @chali_host_address(i64 noundef signext %16, i64 noundef zeroext 0)
  %18 = trunc i64 %1 to i32
  %19 = getelementptr inbounds i8, ptr %17, i32 %18
  %20 = load i8, ptr %19, align 1, !tbaa !12
  %21 = zext i8 %20 to i64
  ret i64 %21
}

; Function Attrs: uwtable(sync)
define dso_local void @chali_string_set(i64 noundef signext %0, i64 noundef signext %1, i64 noundef signext %2) local_unnamed_addr #5 {
  %4 = add nsw i64 %0, 8
  %5 = add nsw i64 %4, %1
  %6 = trunc i64 %2 to i8
  %7 = tail call ptr @chali_host_address(i64 noundef signext %5, i64 noundef zeroext 1)
  store i8 %6, ptr %7, align 1, !tbaa !12
  ret void
}

; Function Attrs: uwtable(sync)
define dso_local noundef i64 @chali_string_concat(i64 noundef signext %0, i64 noundef signext %1) local_unnamed_addr #5 {
  %3 = icmp eq i64 %0, 0
  br i1 %3, label %19, label %4

4:                                                ; preds = %2
  %5 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %6 = icmp slt i64 %0, %5
  br i1 %6, label %7, label %11

7:                                                ; preds = %4
  %8 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 1)
  %9 = tail call i32 @strlen(ptr noundef nonnull dereferenceable(1) %8) #22
  %10 = zext i32 %9 to i64
  br label %19

11:                                               ; preds = %4
  %12 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 8)
  %13 = load i64, ptr %12, align 1
  %14 = icmp eq i64 %13, -1
  br i1 %14, label %15, label %19

15:                                               ; preds = %11
  %16 = add nsw i64 %0, 8
  %17 = tail call ptr @chali_host_address(i64 noundef signext %16, i64 noundef zeroext 8)
  %18 = load i64, ptr %17, align 1
  br label %19

19:                                               ; preds = %2, %7, %11, %15
  %20 = phi i64 [ %10, %7 ], [ 0, %2 ], [ %18, %15 ], [ %13, %11 ]
  %21 = trunc i64 %20 to i32
  %22 = icmp eq i64 %1, 0
  br i1 %22, label %38, label %23

23:                                               ; preds = %19
  %24 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %25 = icmp slt i64 %1, %24
  br i1 %25, label %26, label %30

26:                                               ; preds = %23
  %27 = tail call ptr @chali_host_address(i64 noundef signext %1, i64 noundef zeroext 1)
  %28 = tail call i32 @strlen(ptr noundef nonnull dereferenceable(1) %27) #22
  %29 = zext i32 %28 to i64
  br label %38

30:                                               ; preds = %23
  %31 = tail call ptr @chali_host_address(i64 noundef signext %1, i64 noundef zeroext 8)
  %32 = load i64, ptr %31, align 1
  %33 = icmp eq i64 %32, -1
  br i1 %33, label %34, label %38

34:                                               ; preds = %30
  %35 = add nsw i64 %1, 8
  %36 = tail call ptr @chali_host_address(i64 noundef signext %35, i64 noundef zeroext 8)
  %37 = load i64, ptr %36, align 1
  br label %38

38:                                               ; preds = %19, %26, %30, %34
  %39 = phi i64 [ %29, %26 ], [ 0, %19 ], [ %37, %34 ], [ %32, %30 ]
  %40 = trunc i64 %39 to i32
  %41 = add i32 %40, %21
  %42 = add i32 %41, 8
  %43 = zext i32 %42 to i64
  %44 = tail call i64 @chali_host_allocate(i64 noundef zeroext %43)
  %45 = zext i32 %41 to i64
  %46 = tail call ptr @chali_host_address(i64 noundef signext %44, i64 noundef zeroext 8)
  store i64 %45, ptr %46, align 1
  %47 = add nsw i64 %44, 8
  %48 = and i64 %20, 4294967295
  %49 = tail call ptr @chali_host_address(i64 noundef signext %47, i64 noundef zeroext %48)
  %50 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %51 = icmp slt i64 %0, %50
  br i1 %51, label %62, label %52

52:                                               ; preds = %38
  %53 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext 8)
  %54 = load i64, ptr %53, align 1
  %55 = icmp eq i64 %54, -1
  br i1 %55, label %56, label %60

56:                                               ; preds = %52
  %57 = add nsw i64 %0, 24
  %58 = tail call ptr @chali_host_address(i64 noundef signext %57, i64 noundef zeroext 8)
  %59 = load i64, ptr %58, align 1
  br label %62

60:                                               ; preds = %52
  %61 = add nsw i64 %0, 8
  br label %62

62:                                               ; preds = %38, %56, %60
  %63 = phi i64 [ %61, %60 ], [ %59, %56 ], [ %0, %38 ]
  %64 = tail call ptr @chali_host_address(i64 noundef signext %63, i64 noundef zeroext 0)
  tail call void @llvm.memcpy.p0.p0.i32(ptr align 1 %49, ptr align 1 %64, i32 %21, i1 false)
  %65 = add nsw i64 %47, %48
  %66 = and i64 %39, 4294967295
  %67 = tail call ptr @chali_host_address(i64 noundef signext %65, i64 noundef zeroext %66)
  %68 = icmp slt i64 %1, %50
  br i1 %68, label %79, label %69

69:                                               ; preds = %62
  %70 = tail call ptr @chali_host_address(i64 noundef signext %1, i64 noundef zeroext 8)
  %71 = load i64, ptr %70, align 1
  %72 = icmp eq i64 %71, -1
  br i1 %72, label %73, label %77

73:                                               ; preds = %69
  %74 = add nsw i64 %1, 24
  %75 = tail call ptr @chali_host_address(i64 noundef signext %74, i64 noundef zeroext 8)
  %76 = load i64, ptr %75, align 1
  br label %79

77:                                               ; preds = %69
  %78 = add nsw i64 %1, 8
  br label %79

79:                                               ; preds = %62, %73, %77
  %80 = phi i64 [ %78, %77 ], [ %76, %73 ], [ %1, %62 ]
  %81 = tail call ptr @chali_host_address(i64 noundef signext %80, i64 noundef zeroext 0)
  tail call void @llvm.memcpy.p0.p0.i32(ptr align 1 %67, ptr align 1 %81, i32 %40, i1 false)
  ret i64 %44
}

; Function Attrs: uwtable(sync)
define dso_local void @chali_owned_drop(i64 noundef signext %0) local_unnamed_addr #5 {
  tail call void @chali_host_drop(i64 noundef signext %0)
  ret void
}

; Function Attrs: uwtable(sync)
define dso_local i64 @chali_function_target(i64 noundef signext %0) local_unnamed_addr #5 {
  %2 = tail call i64 @chali_host_function(i64 noundef signext %0, i32 noundef signext 0)
  ret i64 %2
}

declare i64 @chali_host_function(i64 noundef signext, i32 noundef signext) local_unnamed_addr #2

; Function Attrs: uwtable(sync)
define dso_local i64 @chali_function_environment(i64 noundef signext %0) local_unnamed_addr #5 {
  %2 = tail call i64 @chali_host_function(i64 noundef signext %0, i32 noundef signext 1)
  ret i64 %2
}

; Function Attrs: uwtable(sync)
define dso_local range(i64 0, 2) i64 @chali_function_match(i64 noundef signext %0, ptr nofree noundef readonly captures(none) %1) local_unnamed_addr #5 {
  %3 = icmp slt i64 %0, 1
  br i1 %3, label %20, label %4

4:                                                ; preds = %2
  %5 = and i64 %0, 4294967295
  %6 = load i64, ptr @chali_data_size, align 8, !tbaa !13
  %7 = icmp ult i64 %5, %6
  br i1 %7, label %8, label %20

8:                                                ; preds = %4
  %9 = tail call i32 @strlen(ptr noundef nonnull dereferenceable(1) %1) #22
  %10 = zext i32 %9 to i64
  %11 = sub nuw i64 %6, %5
  %12 = icmp ugt i64 %11, %10
  br i1 %12, label %13, label %20

13:                                               ; preds = %8
  %14 = add i32 %9, 1
  %15 = zext i32 %14 to i64
  %16 = tail call ptr @chali_host_address(i64 noundef signext %0, i64 noundef zeroext %15)
  %17 = tail call i32 @bcmp(ptr %16, ptr nonnull %1, i32 %14)
  %18 = icmp eq i32 %17, 0
  %19 = zext i1 %18 to i64
  br label %20

20:                                               ; preds = %13, %8, %2, %4
  %21 = phi i64 [ 0, %2 ], [ 0, %4 ], [ %19, %13 ], [ 0, %8 ]
  ret i64 %21
}

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.umax.i32(i32, i32) #17

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.smax.i64(i64, i64) #17

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare ptr @memchr(ptr, i32 signext, i32) local_unnamed_addr #18

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.umin.i32(i32, i32) #17

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare i32 @bcmp(ptr captures(none), ptr captures(none), i32) local_unnamed_addr #18

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare range(i32 -1, 2) i32 @llvm.ucmp.i32.i32(i32, i32) #17

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.smin.i64(i64, i64) #17

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(read, argmem: none, inaccessiblemem: none, target_mem: none) uwtable(sync) "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="mips32r2" "target-features"="+fpxx,+mips32r2,+nooddspreg,-noabicalls" }
attributes #1 = { cold noreturn uwtable(sync) "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="mips32r2" "target-features"="+fpxx,+mips32r2,+nooddspreg,-noabicalls" }
attributes #2 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="mips32r2" "target-features"="+fpxx,+mips32r2,+nooddspreg,-noabicalls" }
attributes #3 = { nofree nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="mips32r2" "target-features"="+fpxx,+mips32r2,+nooddspreg,-noabicalls" }
attributes #4 = { nofree noreturn "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="mips32r2" "target-features"="+fpxx,+mips32r2,+nooddspreg,-noabicalls" }
attributes #5 = { uwtable(sync) "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="mips32r2" "target-features"="+fpxx,+mips32r2,+nooddspreg,-noabicalls" }
attributes #6 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #7 = { mustprogress nofree nounwind willreturn allockind("alloc,uninitialized") allocsize(0) memory(inaccessiblemem: readwrite, errnomem: write) "alloc-family"="malloc" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="mips32r2" "target-features"="+fpxx,+mips32r2,+nooddspreg,-noabicalls" }
attributes #8 = { mustprogress nounwind willreturn allockind("free") memory(argmem: readwrite, inaccessiblemem: readwrite) "alloc-family"="malloc" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="mips32r2" "target-features"="+fpxx,+mips32r2,+nooddspreg,-noabicalls" }
attributes #9 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable(sync) "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="mips32r2" "target-features"="+fpxx,+mips32r2,+nooddspreg,-noabicalls" }
attributes #10 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: read) "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="mips32r2" "target-features"="+fpxx,+mips32r2,+nooddspreg,-noabicalls" }
attributes #11 = { mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none, target_mem: none) uwtable(sync) "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="mips32r2" "target-features"="+fpxx,+mips32r2,+nooddspreg,-noabicalls" }
attributes #12 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="mips32r2" "target-features"="+fpxx,+mips32r2,+nooddspreg,-noabicalls" }
attributes #13 = { mustprogress nounwind willreturn allockind("realloc") allocsize(1) memory(argmem: readwrite, inaccessiblemem: readwrite, errnomem: write) "alloc-family"="malloc" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="mips32r2" "target-features"="+fpxx,+mips32r2,+nooddspreg,-noabicalls" }
attributes #14 = { nofree nounwind uwtable(sync) "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="mips32r2" "target-features"="+fpxx,+mips32r2,+nooddspreg,-noabicalls" }
attributes #15 = { noreturn uwtable(sync) "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="mips32r2" "target-features"="+fpxx,+mips32r2,+nooddspreg,-noabicalls" }
attributes #16 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: write) }
attributes #17 = { nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none) }
attributes #18 = { nocallback nofree nosync nounwind willreturn memory(argmem: read) }
attributes #19 = { cold nounwind }
attributes #20 = { cold noreturn }
attributes #21 = { allocsize(0) }
attributes #22 = { nounwind }
attributes #23 = { allocsize(1) }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}
!llvm.errno.tbaa = !{!4}

!0 = !{i32 8, !"PIC Level", i32 1}
!1 = !{i32 7, !"PIE Level", i32 1}
!2 = !{i32 7, !"uwtable", i32 1}
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
!14 = !{!"long long", !7, i64 0}
!15 = !{!6, !6, i64 0}
!16 = distinct !{!16, !17, !18}
!17 = !{!"llvm.loop.mustprogress"}
!18 = !{!"llvm.loop.unroll.disable"}
!19 = distinct !{!19, !17, !18}
!20 = distinct !{!20, !17, !18}
!21 = distinct !{!21, !17, !18}
!22 = distinct !{!22, !17, !18}
!23 = distinct !{!23, !17, !18}
!24 = distinct !{!24, !17, !18}
!25 = distinct !{!25, !17, !18}
!26 = !{!27, !27, i64 0}
!27 = !{!"double", !7, i64 0}
