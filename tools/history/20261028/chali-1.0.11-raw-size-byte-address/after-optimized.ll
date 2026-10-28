; ModuleID = 'tmp/byte-address-new-ir.ll'
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
@chali_data_layout_size = local_unnamed_addr constant i64 498
@chali_data_layout = local_unnamed_addr constant [499 x i8] c"CHLY\01\1E\0Aast_data_0\08\03\1F\08\0Aast_data_1(\03\08\08\0Aast_data_20\03\10\08\0Aast_data_3@\03\10\08\0Aast_data_4P\03\10\08\0Aast_data_5h\03\08\08\0Aast_data_6p\03\01\08\0Aast_data_7x\03\0A\08\0Aast_data_8\88\01\03\05\08\0Aast_data_9\90\01\03\0B\08\0Bast_data_10\A0\01\03\07\08\0Bast_data_11\A8\01\03!\08\0Bast_data_12\D0\01\03\1B\08\0Bast_data_13\F0\01\03\02\08\0Bast_data_14\F8\01\03\02\08\0Bast_data_15\80\02\03\10\08\0Bast_data_16\90\02\03\10\08\0Bast_data_17\A0\02\03\1C\08\0Bast_data_18\C0\02\03\16\08\0Bast_data_19\D8\02\03#\08\0Bast_data_20\80\03\03#\08\0Bast_data_21\A8\03\03#\08\0Bast_data_22\D0\03\03\1F\08\0Bast_data_23\F0\03\03>\08\0Bast_data_24\B0\04\03\1C\08\0Bast_data_25\D0\04\03\22\08\0Bast_data_26\F8\04\03\17\08\0Bast_data_27\90\05\03\08\08\0Bast_data_28\98\05\03\08\08\0Bast_data_29\A0\05\03\08\08\00"
@chali_data_image = local_unnamed_addr constant [688 x i8] c"\00\00\00\00\00\00\00\00called Option.unwrap() on None\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\E5\AD\97\E7\AC\A6\E4\B8\B2\00\00\00\00\00\00\00\F0\9F\99\82\00\00\00\00\E4\B8\8D\E5\AD\98\E5\9C\A8!\00\00\00\00\00\00012345\00\00ChaLi-\E5\AD\97\E7\AC\A6\E4\B8\B2-\F0\9F\99\82-0123456789|\00\00\00\00\00\00\00\00ABCDEFGHIJKLMNOPQRSTUVWXYZ\00\00\00\00\00\00a\00\00\00\00\00\00\00B\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00String\EF\BC\9A\E8\BF\9E\E6\8E\A5\E7\BB\93\E6\9E\9C\E8\BF\87\E9\95\BF\00\00\00\00\00String\EF\BC\9A\E7\BB\93\E6\9E\9C\E8\BF\87\E9\95\BF\00\00\00String\EF\BC\9A\E9\9D\9E\E6\B3\95 UTF-8 \E8\B5\B7\E5\A7\8B\E5\AD\97\E8\8A\82\00\00\00\00\00\00String\EF\BC\9A\E4\B8\8D\E5\AE\8C\E6\95\B4\E7\9A\84 UTF-8 \E5\AD\97\E7\AC\A6\00\00\00\00\00\00String\EF\BC\9A\E9\9D\9E\E6\B3\95 UTF-8 \E5\BB\B6\E7\BB\AD\E5\AD\97\E8\8A\82\00\00\00\00\00\00String\EF\BC\9A\E9\9D\9E\E6\B3\95 Unicode \E7\A0\81\E7\82\B9\00\00\E5\88\87\E7\89\87\E7\AB\AF\E7\82\B9\E4\B8\8D\E8\83\BD\E4\B8\BA\E8\B4\9F\E6\95\B0\EF\BC\88String \E7\9A\84\E5\88\87\E7\89\87\E6\8C\89\E5\AD\97\E8\8A\82\E7\AE\97\EF\BC\89\00\00\00String\EF\BC\9A\E5\AD\97\E8\8A\82\E4\B8\8B\E6\A0\87\E8\B6\8A\E7\95\8C\00\00\00\00\00String\EF\BC\9A\E6\9F\A5\E8\AF\A2\E6\93\8D\E4\BD\9C\E7\BC\96\E5\8F\B7\E6\97\A0\E6\95\88\00\00\00\00\00\00\00assert(x) \E7\9A\84 x \E4\B8\BA\E5\81\87\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00"
@chali_data_segment_count = local_unnamed_addr constant i64 1
@chali_data_segments = local_unnamed_addr constant [1 x { i64, i64, i64 }] [{ i64, i64, i64 } { i64 0, i64 0, i64 688 }]
@chali_data_size = local_unnamed_addr constant i64 688
@chali_heap_size = local_unnamed_addr constant i64 536870912
@chali_allocator_kind = local_unnamed_addr constant i32 1
@source_file0 = private constant [19 x i8] c"<\E5\86\85\E5\BB\BA std::lang>\00"
@source_file1 = private constant [29 x i8] c"benchmarks/strings/cases.chl\00"
@trace_anchor_name = private constant [1 x i8] zeroinitializer
@trace_name0 = private constant [33 x i8] c"__chali_enum_Option__uint8_Value\00"
@trace_name1 = private constant [32 x i8] c"__chali_enum_Option__uint8_None\00"
@trace_name2 = private constant [28 x i8] c"std::lang::__bootStringByte\00"
@trace_name3 = private constant [29 x i8] c"std::lang::__bootStringSpace\00"
@trace_name4 = private constant [29 x i8] c"std::lang::__bootStringClamp\00"
@trace_name5 = private constant [34 x i8] c"std::lang::__bootStringEqualRange\00"
@trace_name6 = private constant [28 x i8] c"std::lang::__bootStringFind\00"
@trace_name7 = private constant [28 x i8] c"std::lang::__bootStringNext\00"
@trace_name8 = private constant [25 x i8] c"std::lang::__stringQuery\00"
@trace_name9 = private constant [30 x i8] c"std::lang::__bootStringLength\00"
@trace_name10 = private constant [30 x i8] c"std::lang::__bootStringBuffer\00"
@trace_name11 = private constant [33 x i8] c"std::lang::__bootStringCopyBytes\00"
@trace_name12 = private constant [28 x i8] c"std::lang::__bootStringCopy\00"
@trace_name13 = private constant [28 x i8] c"std::lang::__bootStringDrop\00"
@trace_name14 = private constant [29 x i8] c"std::lang::__bootStringSlice\00"
@trace_name15 = private constant [34 x i8] c"std::lang::__bootStringSliceBytes\00"
@trace_name16 = private constant [31 x i8] c"std::lang::__bootStringCompare\00"
@trace_name17 = private constant [33 x i8] c"std::lang::__bootStringAddLength\00"
@trace_name18 = private constant [25 x i8] c"std::lang::String.length\00"
@trace_name19 = private constant [29 x i8] c"std::lang::String.byteLength\00"
@trace_name20 = private constant [26 x i8] c"std::lang::String.indexOf\00"
@trace_name21 = private constant [25 x i8] c"std::lang::String.byteAt\00"
@trace_name22 = private constant [23 x i8] c"std::lang::assert$bool\00"
@trace_name23 = private constant [15 x i8] c"program::setup\00"
@trace_name24 = private constant [18 x i8] c"program::readWork\00"
@trace_name25 = private constant [20 x i8] c"program::concatWork\00"
@trace_name26 = private constant [19 x i8] c"program::sliceWork\00"
@trace_name27 = private constant [18 x i8] c"program::findWork\00"
@trace_name28 = private constant [30 x i8] c"std::lang::barePointer__int64\00"
@trace_name29 = private constant [37 x i8] c"std::lang::pointerFromAddress__int64\00"
@trace_name30 = private constant [37 x i8] c"std::lang::pointerFromAddress__uint8\00"
@trace_name31 = private constant [21 x i8] c"Option__uint8.unwrap\00"
@trace_name32 = private constant [25 x i8] c"std::lang::dropAt__int64\00"
@trace_name33 = private constant [25 x i8] c"std::lang::takeAt__int64\00"
@trace_name34 = private constant [27 x i8] c"RawStorage__int64.allocate\00"
@trace_name35 = private constant [26 x i8] c"RawStorage__int64.pointer\00"
@trace_name36 = private constant [24 x i8] c"RawStorage__int64.close\00"
@trace_name37 = private constant [27 x i8] c"RawStorage__int64.capacity\00"
@trace_name38 = private constant [21 x i8] c"__chali_library_init\00"
@trace_name39 = private constant [23 x i8] c"__chali_library_finish\00"
@trace_name40 = private constant [20 x i8] c"__chali_copy_struct\00"
@trace_name41 = private constant [29 x i8] c"std::lang::__bootStringJoin2\00"
@trace_name42 = private constant [29 x i8] c"std::lang::__bootStringJoin3\00"
@chali_trace_count = local_unnamed_addr constant i64 44
@chali_trace_functions = local_unnamed_addr constant [44 x ptr] [ptr @chali_code_anchor, ptr @f_v23___chali_enum_Option__uint8_Value, ptr @f_v24___chali_enum_Option__uint8_None, ptr @f_v49_std__lang____bootStringByte, ptr @f_v50_std__lang____bootStringSpace, ptr @f_v51_std__lang____bootStringClamp, ptr @f_v52_std__lang____bootStringEqualRange, ptr @f_v53_std__lang____bootStringFind, ptr @f_v54_std__lang____bootStringNext, ptr @f_v55_std__lang____stringQuery, ptr @f_v56_std__lang____bootStringLength, ptr @f_v57_std__lang____bootStringBuffer, ptr @f_v58_std__lang____bootStringCopyBytes, ptr @f_v60_std__lang____bootStringCopy, ptr @f_v61_std__lang____bootStringDrop, ptr @f_v62_std__lang____bootStringSlice, ptr @f_v63_std__lang____bootStringSliceBytes, ptr @f_v64_std__lang____bootStringCompare, ptr @f_v70_std__lang____bootStringAddLength, ptr @f_v75_std__lang__String_length, ptr @f_v77_std__lang__String_byteLength, ptr @f_v85_std__lang__String_indexOf, ptr @f_v92_std__lang__String_byteAt, ptr @f_v162_std__lang__assert_bool, ptr @__chali_impl_program__setup, ptr @__chali_impl_program__readWork, ptr @__chali_impl_program__concatWork, ptr @__chali_impl_program__sliceWork, ptr @__chali_impl_program__findWork, ptr @f_v271_std__lang__barePointer__int64, ptr @f_v273_std__lang__pointerFromAddress__int64, ptr @f_v275_std__lang__pointerFromAddress__uint8, ptr @f_v283_Option__uint8_unwrap, ptr @f_v461_std__lang__dropAt__int64, ptr @f_v462_std__lang__takeAt__int64, ptr @f_v463_RawStorage__int64_allocate, ptr @f_v464_RawStorage__int64_pointer, ptr @f_v465_RawStorage__int64_close, ptr @f_v466_RawStorage__int64_capacity, ptr @f___chali_library_init, ptr @f___chali_library_finish, ptr @f_v472___chali_copy_struct, ptr @f_v474_std__lang____bootStringJoin2, ptr @f_v475_std__lang____bootStringJoin3]
@chali_trace_names = local_unnamed_addr constant [44 x ptr] [ptr @trace_anchor_name, ptr @trace_name0, ptr @trace_name1, ptr @trace_name2, ptr @trace_name3, ptr @trace_name4, ptr @trace_name5, ptr @trace_name6, ptr @trace_name7, ptr @trace_name8, ptr @trace_name9, ptr @trace_name10, ptr @trace_name11, ptr @trace_name12, ptr @trace_name13, ptr @trace_name14, ptr @trace_name15, ptr @trace_name16, ptr @trace_name17, ptr @trace_name18, ptr @trace_name19, ptr @trace_name20, ptr @trace_name21, ptr @trace_name22, ptr @trace_name23, ptr @trace_name24, ptr @trace_name25, ptr @trace_name26, ptr @trace_name27, ptr @trace_name28, ptr @trace_name29, ptr @trace_name30, ptr @trace_name31, ptr @trace_name32, ptr @trace_name33, ptr @trace_name34, ptr @trace_name35, ptr @trace_name36, ptr @trace_name37, ptr @trace_name38, ptr @trace_name39, ptr @trace_name40, ptr @trace_name41, ptr @trace_name42]
@native_name0 = private constant [17 x i8] c"__std_move_begin\00"
@native_name1 = private constant [15 x i8] c"__std_move_end\00"
@native_name2 = private constant [16 x i8] c"__std_move_live\00"
@native_name4 = private constant [19 x i8] c"__std_move_require\00"
@native_name3 = private constant [12 x i8] c"__std_panic\00"
@s_g_1___chali_global_reference = internal thread_local unnamed_addr global i1 false
@s_g_2_program__firstChunk = internal thread_local unnamed_addr global i64 0
@s_g_3_program__secondChunk = internal thread_local unnamed_addr global i64 0
@s_g_4_program__unitWidth = internal thread_local unnamed_addr global i64 0

; Function Attrs: nounwind uwtable
define dso_local ptr @chali_standard_stream(i32 noundef %0) local_unnamed_addr #0 {
  %2 = icmp eq i32 %0, 1
  %3 = select i1 %2, i32 1, i32 2
  %4 = icmp eq i32 %0, 0
  %5 = select i1 %4, i32 0, i32 %3
  %6 = tail call ptr @__acrt_iob_func(i32 noundef %5) #27
  ret ptr %6
}

declare dllimport ptr @__acrt_iob_func(i32 noundef) local_unnamed_addr #1

; Function Attrs: cold noreturn nounwind uwtable
define dso_local void @chali_panic(ptr noundef %0) local_unnamed_addr #2 {
  %2 = tail call ptr @__acrt_iob_func(i32 noundef 1) #27
  %3 = tail call i32 @fflush(ptr noundef %2)
  %4 = tail call ptr @__acrt_iob_func(i32 noundef 2) #27
  %5 = tail call i32 (ptr, ptr, ...) @fprintf(ptr noundef %4, ptr noundef nonnull @.str, ptr noundef %0) #27
  tail call void @chali_host_trace() #27
  tail call void @exit(i32 noundef 1) #28
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
  tail call void @chali_host_init() #27
  ret void
}

declare dso_local void @chali_host_init() local_unnamed_addr #1

; Function Attrs: nounwind uwtable
define dso_local void @chali_runtime_finish() local_unnamed_addr #0 {
  %1 = tail call ptr @__acrt_iob_func(i32 noundef 1) #27
  %2 = tail call i32 @fflush(ptr noundef %1)
  tail call void @chali_host_finish() #27
  ret void
}

declare dso_local void @chali_host_finish() local_unnamed_addr #1

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_alloc(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call i64 @chali_host_allocate(i64 noundef %0) #27
  ret i64 %2
}

declare dso_local i64 @chali_host_allocate(i64 noundef) local_unnamed_addr #1

; Function Attrs: nounwind uwtable
define dso_local void @chali_drop(i64 noundef %0) local_unnamed_addr #0 {
  tail call void @chali_host_drop(i64 noundef %0) #27
  ret void
}

declare dso_local void @chali_host_drop(i64 noundef) local_unnamed_addr #1

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_load_i64(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #27
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
  %3 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #27
  store i64 %1, ptr %3, align 1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local double @chali_load_f64(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #27
  %3 = load double, ptr %2, align 1
  ret double %3
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_store_f64(i64 noundef %0, double noundef %1) local_unnamed_addr #0 {
  %3 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #27
  store double %1, ptr %3, align 1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 0, 256) i64 @chali_load_u8(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #27
  %3 = load i8, ptr %2, align 1, !tbaa !12
  %4 = zext i8 %3 to i64
  ret i64 %4
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_store_u8(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = trunc i64 %1 to i8
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #27
  store i8 %3, ptr %4, align 1, !tbaa !12
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 -2147483648, 2147483648) i64 @chali_load_i32(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 4) #27
  %3 = load i32, ptr %2, align 1
  %4 = sext i32 %3 to i64
  ret i64 %4
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_store_i32(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = trunc i64 %1 to i32
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 4) #27
  store i32 %3, ptr %4, align 1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 -32768, 32768) i64 @chali_load_i16(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 2) #27
  %3 = load i16, ptr %2, align 1
  %4 = sext i16 %3 to i64
  ret i64 %4
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_store_i16(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = trunc i64 %1 to i16
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 2) #27
  store i16 %3, ptr %4, align 1
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_new(ptr nofree noundef readonly captures(none) %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = icmp eq i64 %1, 0
  %4 = tail call i64 @llvm.umax.i64(i64 %1, i64 1)
  %5 = tail call ptr @malloc(i64 noundef %4) #29
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
  %12 = tail call i64 @chali_host_allocate(i64 noundef %11) #27
  %13 = tail call ptr @chali_host_address(i64 noundef %12, i64 noundef 8) #27
  store i64 %1, ptr %13, align 1
  br i1 %3, label %17, label %14

14:                                               ; preds = %10
  %15 = add nsw i64 %12, 8
  %16 = tail call ptr @chali_host_address(i64 noundef %15, i64 noundef %1) #27
  tail call void @llvm.memcpy.p0.p0.i64(ptr align 1 %16, ptr nonnull align 1 %5, i64 %1, i1 false)
  br label %17

17:                                               ; preds = %14, %10
  %18 = add i64 %1, 8
  %19 = add i64 %18, %12
  %20 = tail call ptr @chali_host_address(i64 noundef %19, i64 noundef 1) #27
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
  %2 = icmp sgt i64 %0, 687
  %3 = zext i1 %2 to i32
  ret i32 %3
}

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_string_length(i64 noundef %0) local_unnamed_addr #0 {
  %2 = icmp eq i64 %0, 0
  br i1 %2, label %11, label %3

3:                                                ; preds = %1
  %4 = icmp slt i64 %0, 688
  br i1 %4, label %8, label %5

5:                                                ; preds = %3
  %6 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #27
  %7 = load i64, ptr %6, align 1
  br label %11

8:                                                ; preds = %3
  %9 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #27
  %10 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %9) #27
  br label %11

11:                                               ; preds = %8, %5, %1
  %12 = phi i64 [ 0, %1 ], [ %7, %5 ], [ %10, %8 ]
  ret i64 %12
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare dso_local i64 @strlen(ptr noundef captures(none)) local_unnamed_addr #9

; Function Attrs: nounwind uwtable
define dso_local ptr @chali_string_bytes(i64 noundef %0) local_unnamed_addr #0 {
  %2 = icmp slt i64 %0, 688
  %3 = select i1 %2, i64 0, i64 8
  %4 = add nsw i64 %3, %0
  %5 = tail call ptr @chali_host_address(i64 noundef %4, i64 noundef 0) #27
  ret ptr %5
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_literal(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #27
  %3 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %2) #27
  %4 = tail call i64 @chali_string_new(ptr noundef nonnull %2, i64 noundef %3)
  ret i64 %4
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_copy(i64 noundef %0) local_unnamed_addr #0 {
  %2 = icmp eq i64 %0, 0
  br i1 %2, label %17, label %3

3:                                                ; preds = %1
  %4 = icmp slt i64 %0, 688
  %5 = select i1 %4, i64 0, i64 8
  %6 = add nsw i64 %5, %0
  %7 = tail call ptr @chali_host_address(i64 noundef %6, i64 noundef 0) #27
  br i1 %4, label %11, label %8

8:                                                ; preds = %3
  %9 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #27
  %10 = load i64, ptr %9, align 1
  br label %14

11:                                               ; preds = %3
  %12 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #27
  %13 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %12) #27
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
  tail call void @chali_host_drop(i64 noundef %0) #27
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 -9223372036854775800, -9223372036854775808) i64 @chali_new_array(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = tail call i64 @llvm.smax.i64(i64 %0, i64 0)
  %4 = mul i64 %3, %1
  %5 = add i64 %4, 8
  %6 = tail call i64 @chali_host_allocate(i64 noundef %5) #27
  %7 = tail call ptr @chali_host_address(i64 noundef %6, i64 noundef 8) #27
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
  call void @llvm.lifetime.start.p0(ptr nonnull %4) #27
  %9 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %4, i64 noundef 128, ptr noundef nonnull @.str.6, ptr noundef %2, i64 noundef %0, i64 noundef %1) #27
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
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #27
  %6 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i64 noundef 128, ptr noundef nonnull @.str.7, ptr noundef %1) #27
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
  %5 = icmp slt i64 %4, 688
  %6 = select i1 %5, i64 0, i64 8
  %7 = add nsw i64 %6, %4
  %8 = tail call ptr @chali_host_address(i64 noundef %7, i64 noundef 0) #27
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
  %26 = tail call ptr @__acrt_iob_func(i32 noundef 1) #27
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
  %102 = tail call ptr @__acrt_iob_func(i32 noundef 1) #27
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
  call void @llvm.lifetime.start.p0(ptr nonnull %5) #27
  call void @llvm.lifetime.start.p0(ptr nonnull %6) #27
  switch i32 %0, label %33 [
    i32 115, label %7
    i32 99, label %25
    i32 63, label %30
  ]

7:                                                ; preds = %4
  %8 = icmp eq i64 %2, 0
  br i1 %8, label %58, label %9

9:                                                ; preds = %7
  %10 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef 64, ptr noundef nonnull @.str.10, ptr noundef %1) #27
  %11 = icmp slt i64 %2, 688
  br i1 %11, label %15, label %12

12:                                               ; preds = %9
  %13 = tail call ptr @chali_host_address(i64 noundef %2, i64 noundef 8) #27
  %14 = load i64, ptr %13, align 1
  br label %18

15:                                               ; preds = %9
  %16 = tail call ptr @chali_host_address(i64 noundef %2, i64 noundef 1) #27
  %17 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %16) #27
  br label %18

18:                                               ; preds = %15, %12
  %19 = phi i64 [ 0, %15 ], [ 8, %12 ]
  %20 = phi i64 [ %17, %15 ], [ %14, %12 ]
  %21 = trunc i64 %20 to i32
  %22 = add nsw i64 %19, %2
  %23 = tail call ptr @chali_host_address(i64 noundef %22, i64 noundef 0) #27
  %24 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %5, i32 noundef %21, ptr noundef %23)
  br label %58

25:                                               ; preds = %4
  %26 = trunc i64 %2 to i32
  %27 = and i32 %26, 255
  %28 = tail call ptr @__acrt_iob_func(i32 noundef 1) #27
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
  %39 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %6, i64 noundef 40, ptr noundef nonnull @.str.12, ptr noundef %1) #27
  %40 = call ptr @strchr(ptr noundef nonnull dereferenceable(1) %6, i32 noundef 46) #27
  %41 = icmp eq ptr %40, null
  br i1 %41, label %42, label %46

42:                                               ; preds = %37
  %43 = call i64 @strlen(ptr noundef nonnull dereferenceable(1) %6) #27
  %44 = sub i64 39, %43
  %45 = call ptr @strncat(ptr noundef nonnull dereferenceable(1) %6, ptr noundef nonnull @.str.13, i64 noundef %44) #27
  br label %46

46:                                               ; preds = %42, %37
  %47 = shl i32 %34, 24
  %48 = ashr exact i32 %47, 24
  %49 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef 64, ptr noundef nonnull @.str.14, ptr noundef nonnull %6, i32 noundef %48) #27
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
  %53 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef 64, ptr noundef nonnull @.str.15, ptr noundef %1, i32 noundef %34) #27
  %54 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef %2)
  br label %58

55:                                               ; preds = %51
  %56 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef 64, ptr noundef nonnull @.str.16, ptr noundef %1) #27
  %57 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %5, i64 noundef %2)
  br label %58

58:                                               ; preds = %55, %52, %46, %25, %18, %7
  call void @llvm.lifetime.end.p0(ptr nonnull %6) #27
  call void @llvm.lifetime.end.p0(ptr nonnull %5) #27
  ret void
}

; Function Attrs: nofree nounwind
declare dso_local noundef i32 @printf(ptr noundef readonly captures(none), ...) local_unnamed_addr #3

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare dso_local ptr @strncat(ptr noalias noundef returned, ptr noalias noundef readonly captures(none), i64 noundef) local_unnamed_addr #11

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_int(i64 noundef %0) local_unnamed_addr #0 {
  %2 = alloca [32 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #27
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
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #27
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_uint(i64 noundef %0) local_unnamed_addr #0 {
  %2 = alloca [32 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #27
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
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #27
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_char(i64 noundef %0) local_unnamed_addr #0 {
  %2 = alloca [32 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #27
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
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #27
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_double(double noundef %0) local_unnamed_addr #0 {
  %2 = alloca [32 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #27
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
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #27
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_string(i64 noundef %0) local_unnamed_addr #0 {
  %2 = alloca [32 x i8], align 16
  %3 = alloca [48 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #27
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #27
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
  %9 = icmp slt i64 %0, 688
  %10 = select i1 %9, i64 0, i64 8
  %11 = add nsw i64 %10, %0
  %12 = tail call ptr @chali_host_address(i64 noundef %11, i64 noundef 0) #27
  br i1 %9, label %16, label %13

13:                                               ; preds = %8
  %14 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #27
  %15 = load i64, ptr %14, align 1
  br label %19

16:                                               ; preds = %8
  %17 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #27
  %18 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %17) #27
  br label %19

19:                                               ; preds = %16, %13
  %20 = phi i64 [ %18, %16 ], [ %15, %13 ]
  %21 = trunc i64 %20 to i32
  %22 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i64 noundef 48, ptr noundef nonnull @.str.10, ptr noundef nonnull %2) #27
  %23 = call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) %3, i32 noundef %21, ptr noundef %12)
  br label %24

24:                                               ; preds = %19, %6, %6, %1
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #27
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #27
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_fmt_finish() local_unnamed_addr #0 {
  %1 = load i64, ptr @chali_fmt_at, align 8, !tbaa !13
  %2 = icmp slt i64 %1, 688
  %3 = select i1 %2, i64 0, i64 8
  %4 = add nsw i64 %3, %1
  %5 = tail call ptr @chali_host_address(i64 noundef %4, i64 noundef 0) #27
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
  %18 = tail call ptr @__acrt_iob_func(i32 noundef 1) #27
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
  %5 = icmp slt i64 %0, 688
  br i1 %5, label %9, label %6

6:                                                ; preds = %4
  %7 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #27
  %8 = load i64, ptr %7, align 1
  br label %12

9:                                                ; preds = %4
  %10 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #27
  %11 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %10) #27
  br label %12

12:                                               ; preds = %9, %6, %2
  %13 = phi i64 [ 0, %2 ], [ %11, %9 ], [ %8, %6 ]
  %14 = icmp eq i64 %1, 0
  br i1 %14, label %23, label %15

15:                                               ; preds = %12
  %16 = icmp slt i64 %1, 688
  br i1 %16, label %20, label %17

17:                                               ; preds = %15
  %18 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 8) #27
  %19 = load i64, ptr %18, align 1
  br label %23

20:                                               ; preds = %15
  %21 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 1) #27
  %22 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %21) #27
  br label %23

23:                                               ; preds = %20, %17, %12
  %24 = phi i64 [ 0, %12 ], [ %22, %20 ], [ %19, %17 ]
  %25 = tail call i64 @llvm.umin.i64(i64 %13, i64 %24)
  %26 = icmp eq i64 %25, 0
  br i1 %26, label %.thread, label %27

27:                                               ; preds = %23
  %28 = icmp slt i64 %0, 688
  %29 = select i1 %28, i64 0, i64 8
  %30 = add nsw i64 %29, %0
  %31 = tail call ptr @chali_host_address(i64 noundef %30, i64 noundef 0) #27
  %32 = icmp slt i64 %1, 688
  %33 = select i1 %32, i64 0, i64 8
  %34 = add nsw i64 %33, %1
  %35 = tail call ptr @chali_host_address(i64 noundef %34, i64 noundef 0) #27
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
  %6 = tail call i64 @chali_host_allocate(i64 noundef 9) #27
  %7 = tail call ptr @chali_host_address(i64 noundef %6, i64 noundef 8) #27
  store i64 0, ptr %7, align 1
  %8 = add i64 %6, 8
  %9 = tail call ptr @chali_host_address(i64 noundef %8, i64 noundef 1) #27
  store i8 0, ptr %9, align 1, !tbaa !12
  br label %30

10:                                               ; preds = %3
  %11 = icmp slt i64 %0, 688
  br i1 %11, label %15, label %12

12:                                               ; preds = %10
  %13 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #27
  %14 = load i64, ptr %13, align 1
  br label %18

15:                                               ; preds = %10
  %16 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #27
  %17 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %16) #27
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
  %27 = tail call ptr @chali_host_address(i64 noundef %26, i64 noundef 0) #27
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
  %7 = icmp slt i64 %0, 688
  br i1 %7, label %11, label %8

8:                                                ; preds = %6
  %9 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #27
  %10 = load i64, ptr %9, align 1
  br label %14

11:                                               ; preds = %6
  %12 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #27
  %13 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %12) #27
  br label %14

14:                                               ; preds = %11, %8
  %15 = phi i64 [ %13, %11 ], [ %10, %8 ]
  %16 = icmp slt i64 %1, 688
  br i1 %16, label %20, label %17

17:                                               ; preds = %14
  %18 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 8) #27
  %19 = load i64, ptr %18, align 1
  br label %23

20:                                               ; preds = %14
  %21 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 1) #27
  %22 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %21) #27
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
  %35 = tail call ptr @chali_host_address(i64 noundef %30, i64 noundef 0) #27
  %36 = getelementptr inbounds nuw i8, ptr %35, i64 %34
  %37 = tail call ptr @chali_host_address(i64 noundef %32, i64 noundef 0) #27
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
  %1 = tail call dereferenceable_or_null(256) ptr @malloc(i64 noundef 256) #29
  %2 = icmp eq ptr %1, null
  br i1 %2, label %3, label %.preheader

3:                                                ; preds = %0
  tail call void @chali_panic(ptr noundef nonnull @.str.18)
  unreachable

.preheader:                                       ; preds = %0, %20
  %4 = phi ptr [ %21, %20 ], [ %1, %0 ]
  %5 = phi i64 [ %24, %20 ], [ 0, %0 ]
  %6 = phi i64 [ %22, %20 ], [ 256, %0 ]
  %7 = tail call ptr @__acrt_iob_func(i32 noundef 0) #27
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
  %17 = tail call ptr @realloc(ptr noundef %4, i64 noundef %16) #30
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
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #27
  %4 = icmp slt i64 %1, 0
  %5 = trunc i64 %1 to i32
  %6 = select i1 %4, i32 3, i32 %5
  %7 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i64 noundef 256, ptr noundef nonnull @.str.21, i32 noundef %6, double noundef %0) #27
  %8 = icmp slt i32 %7, 0
  br i1 %8, label %9, label %10

9:                                                ; preds = %2
  tail call void @chali_panic(ptr noundef nonnull @.str.22)
  unreachable

10:                                               ; preds = %2
  %11 = tail call i32 @llvm.umin.i32(i32 %7, i32 255)
  %12 = zext nneg i32 %11 to i64
  %13 = call i64 @chali_string_new(ptr noundef nonnull %3, i64 noundef %12)
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #27
  ret i64 %13
}

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_read_number(i32 noundef %0) local_unnamed_addr #0 {
  %2 = alloca [256 x i8], align 16
  %3 = alloca double, align 8
  %4 = alloca i64, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #27
  %5 = tail call ptr @__acrt_iob_func(i32 noundef 0) #27
  %6 = call ptr @fgets(ptr noundef nonnull %2, i32 noundef 256, ptr noundef %5)
  %7 = icmp eq ptr %6, null
  br i1 %7, label %18, label %8

8:                                                ; preds = %1
  %9 = icmp eq i32 %0, 0
  br i1 %9, label %14, label %10

10:                                               ; preds = %8
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #27
  store double 0.000000e+00, ptr %3, align 8, !tbaa !30
  %11 = call i32 (ptr, ptr, ...) @sscanf(ptr noundef nonnull %2, ptr noundef nonnull @.str.23, ptr noundef nonnull %3)
  %12 = icmp eq i32 %11, 1
  %.pre = load i64, ptr %3, align 8
  %13 = select i1 %12, i64 %.pre, i64 0
  call void @llvm.lifetime.end.p0(ptr nonnull %3) #27
  br label %18

14:                                               ; preds = %8
  call void @llvm.lifetime.start.p0(ptr nonnull %4) #27
  store i64 0, ptr %4, align 8, !tbaa !13
  %15 = call i32 (ptr, ptr, ...) @sscanf(ptr noundef nonnull %2, ptr noundef nonnull @.str.24, ptr noundef nonnull %4)
  %16 = icmp eq i32 %15, 1
  %.pre2 = load i64, ptr %4, align 8
  %17 = select i1 %16, i64 %.pre2, i64 0
  call void @llvm.lifetime.end.p0(ptr nonnull %4) #27
  br label %18

18:                                               ; preds = %14, %10, %1
  %19 = phi i64 [ %17, %14 ], [ %13, %10 ], [ 0, %1 ]
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #27
  ret i64 %19
}

; Function Attrs: nofree nounwind
declare dso_local noundef ptr @fgets(ptr noundef writeonly, i32 noundef, ptr noundef captures(none)) local_unnamed_addr #3

; Function Attrs: nofree nounwind
declare dso_local noundef i32 @sscanf(ptr noundef readonly captures(none), ptr noundef readonly captures(none), ...) local_unnamed_addr #3

; Function Attrs: nounwind uwtable
define dso_local range(i64 0, 2) i64 @chali_stdin_eof() local_unnamed_addr #0 {
  %1 = tail call ptr @__acrt_iob_func(i32 noundef 0) #27
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
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #27
  %3 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %2, i64 noundef 32, ptr noundef nonnull @.str.24, i64 noundef %0) #27
  %4 = call i64 @strlen(ptr noundef nonnull dereferenceable(1) %2) #27
  %5 = call i64 @chali_string_new(ptr noundef nonnull %2, i64 noundef %4)
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #27
  ret i64 %5
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_of_double(double noundef %0) local_unnamed_addr #0 {
  %2 = alloca [64 x i8], align 16
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #27
  %3 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %2, i64 noundef 64, ptr noundef nonnull @.str.25, double noundef %0) #27
  %4 = call i64 @strlen(ptr noundef nonnull dereferenceable(1) %2) #27
  %5 = call i64 @chali_string_new(ptr noundef nonnull %2, i64 noundef %4)
  call void @llvm.lifetime.end.p0(ptr nonnull %2) #27
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
  call void @llvm.lifetime.start.p0(ptr nonnull %3) #27
  %8 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %3, i64 noundef 128, ptr noundef nonnull @.str.6, ptr noundef nonnull @.str.26, i64 noundef %0, i64 noundef %1) #27
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
  call void @llvm.lifetime.start.p0(ptr nonnull %2) #27
  %5 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr noundef nonnull dereferenceable(1) %2, i64 noundef 128, ptr noundef nonnull @.str.7, ptr noundef nonnull @.str.27) #27
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
  %4 = tail call ptr @__acrt_iob_func(i32 noundef 1) #27
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
  %4 = icmp slt i64 %0, 688
  %5 = select i1 %4, i64 0, i64 8
  %6 = add nsw i64 %5, %0
  %7 = tail call ptr @chali_host_address(i64 noundef %6, i64 noundef 0) #27
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
  %20 = tail call ptr @__acrt_iob_func(i32 noundef 1) #27
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
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef %2) #27
  %5 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef %2) #27
  tail call void @llvm.memmove.p0.p0.i64(ptr align 1 %4, ptr align 1 %5, i64 %2, i1 false)
  ret i64 %0
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memmove.p0.p0.i64(ptr writeonly captures(none), ptr readonly captures(none), i64, i1 immarg) #5

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_set_bytes(i64 noundef returned %0, i64 noundef %1, i64 noundef %2) local_unnamed_addr #0 {
  %4 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef %2) #27
  %5 = trunc i64 %1 to i8
  tail call void @llvm.memset.p0.i64(ptr align 1 %4, i8 %5, i64 %2, i1 false)
  ret i64 %0
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #14

; Function Attrs: nounwind uwtable
define dso_local range(i64 0, 256) i64 @chali_string_at(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = icmp slt i64 %0, 688
  %4 = select i1 %3, i64 0, i64 8
  %5 = add nsw i64 %4, %0
  %6 = tail call ptr @chali_host_address(i64 noundef %5, i64 noundef 0) #27
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
  %7 = tail call ptr @chali_host_address(i64 noundef %5, i64 noundef 1) #27
  store i8 %6, ptr %7, align 1, !tbaa !12
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local noundef i64 @chali_string_concat(i64 noundef %0, i64 noundef %1) local_unnamed_addr #0 {
  %3 = icmp eq i64 %0, 0
  br i1 %3, label %12, label %4

4:                                                ; preds = %2
  %5 = icmp slt i64 %0, 688
  br i1 %5, label %9, label %6

6:                                                ; preds = %4
  %7 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 8) #27
  %8 = load i64, ptr %7, align 1
  br label %12

9:                                                ; preds = %4
  %10 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef 1) #27
  %11 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %10) #27
  br label %12

12:                                               ; preds = %9, %6, %2
  %13 = phi i64 [ 0, %2 ], [ %8, %6 ], [ %11, %9 ]
  %14 = icmp eq i64 %1, 0
  br i1 %14, label %23, label %15

15:                                               ; preds = %12
  %16 = icmp slt i64 %1, 688
  br i1 %16, label %20, label %17

17:                                               ; preds = %15
  %18 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 8) #27
  %19 = load i64, ptr %18, align 1
  br label %23

20:                                               ; preds = %15
  %21 = tail call ptr @chali_host_address(i64 noundef %1, i64 noundef 1) #27
  %22 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %21) #27
  br label %23

23:                                               ; preds = %20, %17, %12
  %24 = phi i64 [ 0, %12 ], [ %19, %17 ], [ %22, %20 ]
  %25 = add i64 %24, %13
  %26 = add i64 %25, 8
  %27 = tail call i64 @chali_host_allocate(i64 noundef %26) #27
  %28 = tail call ptr @chali_host_address(i64 noundef %27, i64 noundef 8) #27
  store i64 %25, ptr %28, align 1
  %29 = add nsw i64 %27, 8
  %30 = tail call ptr @chali_host_address(i64 noundef %29, i64 noundef %13) #27
  %31 = icmp slt i64 %0, 688
  %32 = select i1 %31, i64 0, i64 8
  %33 = add nsw i64 %32, %0
  %34 = tail call ptr @chali_host_address(i64 noundef %33, i64 noundef 0) #27
  tail call void @llvm.memcpy.p0.p0.i64(ptr align 1 %30, ptr align 1 %34, i64 %13, i1 false)
  %35 = add i64 %29, %13
  %36 = tail call ptr @chali_host_address(i64 noundef %35, i64 noundef %24) #27
  %37 = icmp slt i64 %1, 688
  %38 = select i1 %37, i64 0, i64 8
  %39 = add nsw i64 %38, %1
  %40 = tail call ptr @chali_host_address(i64 noundef %39, i64 noundef 0) #27
  tail call void @llvm.memcpy.p0.p0.i64(ptr align 1 %36, ptr align 1 %40, i64 %24, i1 false)
  ret i64 %27
}

; Function Attrs: nounwind uwtable
define dso_local void @chali_owned_drop(i64 noundef %0) local_unnamed_addr #0 {
  tail call void @chali_host_drop(i64 noundef %0) #27
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_function_target(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call i64 @chali_host_function(i64 noundef %0, i32 noundef 0) #27
  ret i64 %2
}

declare dso_local i64 @chali_host_function(i64 noundef, i32 noundef) local_unnamed_addr #1

; Function Attrs: nounwind uwtable
define dso_local i64 @chali_function_environment(i64 noundef %0) local_unnamed_addr #0 {
  %2 = tail call i64 @chali_host_function(i64 noundef %0, i32 noundef 1) #27
  ret i64 %2
}

; Function Attrs: nounwind uwtable
define dso_local range(i64 0, 2) i64 @chali_function_match(i64 noundef %0, ptr nofree noundef readonly captures(none) %1) local_unnamed_addr #0 {
  %3 = add i64 %0, -1
  %or.cond = icmp ult i64 %3, 687
  br i1 %or.cond, label %4, label %14

4:                                                ; preds = %2
  %5 = tail call i64 @strlen(ptr noundef nonnull dereferenceable(1) %1) #27
  %6 = sub nuw nsw i64 688, %0
  %7 = icmp ult i64 %5, %6
  br i1 %7, label %8, label %14

8:                                                ; preds = %4
  %9 = add nuw nsw i64 %5, 1
  %10 = tail call ptr @chali_host_address(i64 noundef %0, i64 noundef %9) #27
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

declare i64 @chali_host_raw_storage(i64, i64, i64, i64, i64, i64) local_unnamed_addr

declare i64 @chali_host_string_storage(i64, i64) local_unnamed_addr

declare void @chali_host_memory_prepare(i64, i64, i64, i64, ptr, ptr) local_unnamed_addr

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

declare i64 @chali_host_copy_words(i64, i64, i64) local_unnamed_addr

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
  store i1 false, ptr @s_g_1___chali_global_reference, align 1
  store i64 0, ptr @s_g_2_program__firstChunk, align 8
  store i64 0, ptr @s_g_3_program__secondChunk, align 8
  store i64 0, ptr @s_g_4_program__unitWidth, align 8
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define noundef i1 @chali_program_std_bind(ptr nofree readnone captures(none) %registry, ptr nofree readnone captures(none) %error) local_unnamed_addr #19 {
entry:
  ret i1 true
}

; Function Attrs: alwaysinline uwtable
define internal noundef i64 @f_v23___chali_enum_Option__uint8_Value(i64 %p0) #21 section "chali_code" {
entry:
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
  %offset.i = sub i64 88, %begin.i
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
  %resolved.i = tail call ptr @chali_host_address_view(i64 88, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit

chali_primitive_address.exit:                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  store i64 %p0, ptr %common.ret.op.i, align 1
  ret i64 80
}

; Function Attrs: alwaysinline mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable
define internal noundef i64 @f_v24___chali_enum_Option__uint8_None() #22 section "chali_code" {
entry:
  ret i64 64
}

; Function Attrs: alwaysinline uwtable
define internal range(i64 0, 256) i64 @f_v49_std__lang____bootStringByte(i64 %p0, i64 %p1) #21 section "chali_code" {
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
  ret i64 %r11
}

; Function Attrs: alwaysinline mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none, target_mem: none) uwtable
define internal range(i64 0, 2) i64 @f_v50_std__lang____bootStringSpace(i64 %p0) #23 section "chali_code" {
entry:
  store ptr @f_v50_std__lang____bootStringSpace, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 17, ptr @chali_source_line, align 8
  %r2 = icmp eq i64 %p0, 32
  br i1 %r2, label %ast_block_2, label %ast_block_1

ast_block_1:                                      ; preds = %entry
  store ptr @source_file0, ptr @chali_source_file, align 8
  %r7 = icmp sgt i64 %p0, 8
  br i1 %r7, label %ast_block_4, label %ast_block_2

ast_block_4:                                      ; preds = %ast_block_1
  store ptr @source_file0, ptr @chali_source_file, align 8
  %r12 = icmp samesign ult i64 %p0, 14
  %0 = zext i1 %r12 to i64
  br label %ast_block_2

ast_block_2:                                      ; preds = %ast_block_4, %ast_block_1, %entry
  %r18 = phi i64 [ 1, %entry ], [ %0, %ast_block_4 ], [ 0, %ast_block_1 ]
  ret i64 %r18
}

; Function Attrs: alwaysinline mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none, target_mem: none) uwtable
define internal i64 @f_v51_std__lang____bootStringClamp(i64 %p0, i64 %p1) #23 section "chali_code" {
entry:
  store ptr @f_v51_std__lang____bootStringClamp, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 20, ptr @chali_source_line, align 8
  %r2 = icmp slt i64 %p0, 0
  br i1 %r2, label %common.ret, label %ast_block_2

common.ret:                                       ; preds = %ast_block_2, %entry
  %common.ret.op = phi i64 [ 0, %entry ], [ %p1.p0, %ast_block_2 ]
  ret i64 %common.ret.op

ast_block_2:                                      ; preds = %entry
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 21, ptr @chali_source_line, align 8
  %p1.p0 = tail call i64 @llvm.smin.i64(i64 %p0, i64 %p1)
  br label %common.ret
}

; Function Attrs: alwaysinline uwtable
define internal range(i64 0, 2) i64 @f_v52_std__lang____bootStringEqualRange(i64 %p0, i64 %p1, i64 %p2, i64 %p3) #21 section "chali_code" {
entry:
  store ptr @f_v52_std__lang____bootStringEqualRange, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 27, ptr @chali_source_line, align 8
  %r438 = icmp sgt i64 %p3, 0
  br i1 %r438, label %ast_block_0.lr.ph, label %common.ret

ast_block_0.lr.ph:                                ; preds = %entry
  %r13 = add i64 %p1, %p0
  br label %ast_block_0

ast_block_0:                                      ; preds = %ast_block_0.lr.ph, %ast_block_5
  %s_v492___ast_52_i.039 = phi i64 [ 0, %ast_block_0.lr.ph ], [ %r29, %ast_block_5 ]
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i7 = add i64 %r13, %s_v492___ast_52_i.039
  %0 = icmp eq i64 %r4.i7, 0
  br i1 %0, label %1, label %chali_require_reference.exit35

1:                                                ; preds = %ast_block_0
  tail call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit35:                   ; preds = %ast_block_0
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i10 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i11 = icmp eq ptr %revision_pointer.i.i10, null
  br i1 %has_revision.not.i.i11, label %slow.i.i26, label %check.i.i12

check.i.i12:                                      ; preds = %chali_require_reference.exit35
  %revision.i.i13 = load atomic i64, ptr %revision_pointer.i.i10 acquire, align 8
  %stamp.i.i14 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i15 = icmp eq i64 %revision.i.i13, %stamp.i.i14
  %enabled.i.i16 = icmp ne i64 %revision.i.i13, 0
  %live.i.i17 = and i1 %enabled.i.i16, %same_revision.i.i15
  %begin.i.i18 = load i64, ptr @chali_address_view, align 16
  %size.i.i19 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i20 = sub i64 %r4.i7, %begin.i.i18
  %inside.i.i21 = icmp ult i64 %offset.i.i20, %size.i.i19
  %valid.i.i25 = and i1 %live.i.i17, %inside.i.i21
  br i1 %valid.i.i25, label %fast.i.i31, label %slow.i.i26

fast.i.i31:                                       ; preds = %check.i.i12
  %base.i.i32 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i33 = getelementptr i8, ptr %base.i.i32, i64 %offset.i.i20
  br label %f_v49_std__lang____bootStringByte.exit34

slow.i.i26:                                       ; preds = %check.i.i12, %chali_require_reference.exit35
  %resolved.i.i27 = tail call ptr @chali_host_address_view(i64 %r4.i7, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit34

f_v49_std__lang____bootStringByte.exit34:         ; preds = %fast.i.i31, %slow.i.i26
  %common.ret.op.i.i28 = phi ptr [ %physical.i.i33, %fast.i.i31 ], [ %resolved.i.i27, %slow.i.i26 ]
  %r10.i29 = load i8, ptr %common.ret.op.i.i28, align 1
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i = add i64 %s_v492___ast_52_i.039, %p2
  %2 = icmp eq i64 %r4.i, 0
  br i1 %2, label %3, label %chali_require_reference.exit37

3:                                                ; preds = %f_v49_std__lang____bootStringByte.exit34
  tail call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit37:                   ; preds = %f_v49_std__lang____bootStringByte.exit34
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i = icmp eq ptr %revision_pointer.i.i, null
  br i1 %has_revision.not.i.i, label %slow.i.i, label %check.i.i

check.i.i:                                        ; preds = %chali_require_reference.exit37
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

slow.i.i:                                         ; preds = %check.i.i, %chali_require_reference.exit37
  %resolved.i.i = tail call ptr @chali_host_address_view(i64 %r4.i, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit

f_v49_std__lang____bootStringByte.exit:           ; preds = %fast.i.i, %slow.i.i
  %common.ret.op.i.i = phi ptr [ %physical.i.i, %fast.i.i ], [ %resolved.i.i, %slow.i.i ]
  %r10.i = load i8, ptr %common.ret.op.i.i, align 1
  store ptr @f_v52_std__lang____bootStringEqualRange, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 28, ptr @chali_source_line, align 8
  %r22.not = icmp eq i8 %r10.i29, %r10.i
  br i1 %r22.not, label %ast_block_5, label %common.ret

common.ret:                                       ; preds = %f_v49_std__lang____bootStringByte.exit, %ast_block_5, %entry
  %common.ret.op = phi i64 [ 1, %entry ], [ 1, %ast_block_5 ], [ 0, %f_v49_std__lang____bootStringByte.exit ]
  ret i64 %common.ret.op

ast_block_5:                                      ; preds = %f_v49_std__lang____bootStringByte.exit
  %r29 = add nuw nsw i64 %s_v492___ast_52_i.039, 1
  store ptr @f_v52_std__lang____bootStringEqualRange, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 27, ptr @chali_source_line, align 8
  %exitcond.not = icmp eq i64 %r29, %p3
  br i1 %exitcond.not, label %common.ret, label %ast_block_0
}

; Function Attrs: uwtable
define internal i64 @f_v53_std__lang____bootStringFind(i64 %p0, i64 %p1, i64 %p2, i64 %p3, i64 %p4) #24 section "chali_code" {
entry:
  %indirect_arguments.i.i = alloca [2 x i64], align 8
  %native_argument_types.i.i = alloca [2 x i8], align 1
  %indirect_arguments.i466 = alloca [4 x i64], align 8
  %native_argument_types.i467 = alloca [4 x i8], align 1
  %raw_memory_output.i468 = alloca ptr, align 8
  %raw_memory_input.i469 = alloca ptr, align 8
  %indirect_arguments.i = alloca [7 x i64], align 8
  %native_argument_types.i = alloca [7 x i8], align 1
  %raw_memory_output.i = alloca ptr, align 8
  %raw_memory_input.i = alloca ptr, align 8
  %indirect_arguments = alloca [4 x i64], align 8
  %native_argument_types = alloca [4 x i8], align 1
  %raw_memory_output = alloca ptr, align 8
  %raw_memory_input = alloca ptr, align 8
  %0 = tail call i64 @chali_host_allocate(i64 noundef 8) #27
  %revision_pointer.i278 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i279 = icmp eq ptr %revision_pointer.i278, null
  br i1 %has_revision.not.i279, label %slow.i294, label %check.i280

check.i280:                                       ; preds = %entry
  %revision.i281 = load atomic i64, ptr %revision_pointer.i278 acquire, align 8
  %stamp.i282 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i283 = icmp eq i64 %revision.i281, %stamp.i282
  %enabled.i284 = icmp ne i64 %revision.i281, 0
  %live.i285 = and i1 %enabled.i284, %same_revision.i283
  %begin.i286 = load i64, ptr @chali_address_view, align 16
  %size.i287 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i288 = sub i64 %0, %begin.i286
  %inside.i289 = icmp ult i64 %offset.i288, %size.i287
  %remaining.i290 = sub i64 %size.i287, %offset.i288
  %fits.i291 = icmp ugt i64 %remaining.i290, 7
  %range.i292 = and i1 %inside.i289, %fits.i291
  %valid.i293 = and i1 %live.i285, %range.i292
  br i1 %valid.i293, label %fast.i297, label %slow.i294

fast.i297:                                        ; preds = %check.i280
  %base.i298 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i299 = getelementptr i8, ptr %base.i298, i64 %offset.i288
  br label %chali_primitive_address.exit300

slow.i294:                                        ; preds = %check.i280, %entry
  %resolved.i295 = tail call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit300

chali_primitive_address.exit300:                  ; preds = %fast.i297, %slow.i294
  %common.ret.op.i296 = phi ptr [ %physical.i299, %fast.i297 ], [ %resolved.i295, %slow.i294 ]
  store i64 %p0, ptr %common.ret.op.i296, align 1
  store ptr @f_v53_std__lang____bootStringFind, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 35, ptr @chali_source_line, align 8
  %r8 = icmp eq i64 %p3, 0
  br i1 %r8, label %ast_block_0, label %ast_block_2

ast_block_0:                                      ; preds = %chali_primitive_address.exit300
  %r15.not = icmp eq i64 %0, 0
  br i1 %r15.not, label %common.ret, label %ast_block_3

ast_block_3:                                      ; preds = %ast_block_0
  tail call void @chali_host_drop(i64 noundef %0) #27
  br label %common.ret

common.ret:                                       ; preds = %ast_block_77, %ast_block_76, %ast_block_65, %ast_block_64, %ast_block_9, %ast_block_6, %ast_block_3, %ast_block_0
  %common.ret.op = phi i64 [ %r366, %ast_block_65 ], [ %p4, %ast_block_3 ], [ -1, %ast_block_9 ], [ %p4, %ast_block_0 ], [ -1, %ast_block_6 ], [ %r366, %ast_block_64 ], [ -1, %ast_block_76 ], [ -1, %ast_block_77 ]
  ret i64 %common.ret.op

ast_block_2:                                      ; preds = %chali_primitive_address.exit300
  %r28 = sub i64 %p1, %p4
  store ptr @f_v53_std__lang____bootStringFind, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 36, ptr @chali_source_line, align 8
  %r29 = icmp sgt i64 %p3, %r28
  br i1 %r29, label %ast_block_6, label %ast_block_8

ast_block_6:                                      ; preds = %ast_block_2
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 36, ptr @chali_source_line, align 8
  %r35.not = icmp eq i64 %0, 0
  br i1 %r35.not, label %common.ret, label %ast_block_9

ast_block_9:                                      ; preds = %ast_block_6
  tail call void @chali_host_drop(i64 noundef %0) #27
  br label %common.ret

ast_block_8:                                      ; preds = %ast_block_2
  %1 = tail call i64 @chali_host_allocate(i64 noundef 8) #27
  call void @chali_host_memory_prepare(i64 8, i64 %1, i64 0, i64 8, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r45 = load ptr, ptr %raw_memory_output, align 8
  store i64 0, ptr %r45, align 1
  store i64 %1, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r51 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  store i64 8, ptr %r51, align 8
  %r52 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r52, align 1
  %r53 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r54 = call i8 @chali_host_result_float()
  store ptr @f_v53_std__lang____bootStringFind, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 37, ptr @chali_source_line, align 8
  %r57 = call i64 @f_v463_RawStorage__int64_allocate(i64 %p3)
  %r63 = call i64 @chali_host_copy_words(i64 %1, i64 40, i64 1)
  store i64 %1, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r51, align 8
  store i8 0, ptr %r52, align 1
  %r70 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r71 = call i8 @chali_host_result_float()
  %r76.not = icmp eq i64 %1, 40
  br i1 %r76.not, label %ast_block_14, label %ast_block_12

ast_block_12:                                     ; preds = %ast_block_8
  store i64 40, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r51, align 8
  store i8 0, ptr %r52, align 1
  %r85 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 16
  store i64 44, ptr %r85, align 8
  %r86 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r86, align 1
  %r87 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r88 = call i8 @chali_host_result_float()
  br label %ast_block_14

ast_block_14:                                     ; preds = %ast_block_8, %ast_block_12
  %2 = call i64 @chali_host_allocate(i64 noundef 16) #27
  call void @chali_host_memory_prepare(i64 8, i64 %2, i64 0, i64 16, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r92 = load ptr, ptr %raw_memory_output, align 8
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 1 dereferenceable(16) %r92, i8 0, i64 16, i1 false)
  store i64 %2, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 16, ptr %r51, align 8
  store i8 0, ptr %r52, align 1
  %r100 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r101 = call i8 @chali_host_result_float()
  %3 = call i64 @chali_host_allocate(i64 noundef 8) #27
  call void @chali_host_memory_prepare(i64 8, i64 %3, i64 0, i64 8, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r105 = load ptr, ptr %raw_memory_output, align 8
  store i64 0, ptr %r105, align 1
  store i64 %3, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r51, align 8
  store i8 0, ptr %r52, align 1
  %r113 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r114 = call i8 @chali_host_result_float()
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i466)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i467)
  call void @llvm.lifetime.start.p0(ptr nonnull %raw_memory_output.i468)
  call void @llvm.lifetime.start.p0(ptr nonnull %raw_memory_input.i469)
  store ptr @f_v464_RawStorage__int64_pointer, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %revision_pointer.i73.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i74.i = icmp eq ptr %revision_pointer.i73.i, null
  br i1 %has_revision.not.i74.i, label %slow.i89.i, label %check.i75.i

check.i75.i:                                      ; preds = %ast_block_14
  %revision.i76.i = load atomic i64, ptr %revision_pointer.i73.i acquire, align 8
  %stamp.i77.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i78.i = icmp eq i64 %revision.i76.i, %stamp.i77.i
  %enabled.i79.i = icmp ne i64 %revision.i76.i, 0
  %live.i80.i = and i1 %enabled.i79.i, %same_revision.i78.i
  %begin.i81.i = load i64, ptr @chali_address_view, align 16
  %size.i82.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i83.i = sub i64 %1, %begin.i81.i
  %inside.i84.i = icmp ult i64 %offset.i83.i, %size.i82.i
  %remaining.i85.i = sub i64 %size.i82.i, %offset.i83.i
  %fits.i86.i = icmp ugt i64 %remaining.i85.i, 7
  %range.i87.i = and i1 %inside.i84.i, %fits.i86.i
  %valid.i88.i = and i1 %live.i80.i, %range.i87.i
  br i1 %valid.i88.i, label %fast.i92.i, label %slow.i89.i

fast.i92.i:                                       ; preds = %check.i75.i
  %base.i93.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i94.i = getelementptr i8, ptr %base.i93.i, i64 %offset.i83.i
  br label %chali_primitive_address.exit95.i

slow.i89.i:                                       ; preds = %check.i75.i, %ast_block_14
  %resolved.i90.i = call ptr @chali_host_address_view(i64 %1, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit95.i

chali_primitive_address.exit95.i:                 ; preds = %slow.i89.i, %fast.i92.i
  %common.ret.op.i91.i = phi ptr [ %physical.i94.i, %fast.i92.i ], [ %resolved.i90.i, %slow.i89.i ]
  %r5.i470 = load i64, ptr %common.ret.op.i91.i, align 1
  %r6.not.i = icmp eq i64 %r5.i470, 0
  br i1 %r6.not.i, label %ast_block_6.i.thread, label %ast_block_1.i

ast_block_6.i.thread:                             ; preds = %chali_primitive_address.exit95.i
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i.i)
  br label %ast_block_0.i.i

ast_block_1.i:                                    ; preds = %chali_primitive_address.exit95.i
  store ptr @f_v466_RawStorage__int64_capacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %revision_pointer.i3.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i4.i = icmp eq ptr %revision_pointer.i3.i, null
  br i1 %has_revision.not.i4.i, label %slow.i19.i, label %check.i5.i

check.i5.i:                                       ; preds = %ast_block_1.i
  %revision.i6.i = load atomic i64, ptr %revision_pointer.i3.i acquire, align 8
  %stamp.i7.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i8.i = icmp eq i64 %revision.i6.i, %stamp.i7.i
  %enabled.i9.i = icmp ne i64 %revision.i6.i, 0
  %live.i10.i = and i1 %enabled.i9.i, %same_revision.i8.i
  %begin.i11.i = load i64, ptr @chali_address_view, align 16
  %size.i12.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i13.i = sub i64 %1, %begin.i11.i
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

slow.i19.i:                                       ; preds = %check.i5.i, %ast_block_1.i
  %resolved.i20.i = call ptr @chali_host_address_view(i64 %1, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit25.i

chali_primitive_address.exit25.i:                 ; preds = %slow.i19.i, %fast.i22.i
  %common.ret.op.i21.i = phi ptr [ %physical.i24.i, %fast.i22.i ], [ %resolved.i20.i, %slow.i19.i ]
  %r5.i542 = load i64, ptr %common.ret.op.i21.i, align 1
  %r6.i = icmp eq i64 %r5.i542, 0
  br i1 %r6.i, label %f_v162_std__lang__assert_bool.exit.i.critedge, label %ast_block_2.i543

ast_block_2.i543:                                 ; preds = %chali_primitive_address.exit25.i
  store ptr @f_v466_RawStorage__int64_capacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %revision_pointer.i.i544 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i545 = icmp eq ptr %revision_pointer.i.i544, null
  br i1 %has_revision.not.i.i545, label %slow.i.i560, label %check.i.i546

check.i.i546:                                     ; preds = %ast_block_2.i543
  %revision.i.i547 = load atomic i64, ptr %revision_pointer.i.i544 acquire, align 8
  %stamp.i.i548 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i549 = icmp eq i64 %revision.i.i547, %stamp.i.i548
  %enabled.i.i550 = icmp ne i64 %revision.i.i547, 0
  %live.i.i551 = and i1 %enabled.i.i550, %same_revision.i.i549
  %begin.i.i552 = load i64, ptr @chali_address_view, align 16
  %size.i.i553 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i554 = sub i64 %1, %begin.i.i552
  %inside.i.i555 = icmp ult i64 %offset.i.i554, %size.i.i553
  %remaining.i.i556 = sub i64 %size.i.i553, %offset.i.i554
  %fits.i.i557 = icmp ugt i64 %remaining.i.i556, 7
  %range.i.i558 = and i1 %inside.i.i555, %fits.i.i557
  %valid.i.i559 = and i1 %live.i.i551, %range.i.i558
  br i1 %valid.i.i559, label %fast.i.i566, label %slow.i.i560

fast.i.i566:                                      ; preds = %check.i.i546
  %base.i.i567 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i568 = getelementptr i8, ptr %base.i.i567, i64 %offset.i.i554
  br label %chali_primitive_address.exit.i562

slow.i.i560:                                      ; preds = %check.i.i546, %ast_block_2.i543
  %resolved.i.i561 = call ptr @chali_host_address_view(i64 %1, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit.i562

chali_primitive_address.exit.i562:                ; preds = %slow.i.i560, %fast.i.i566
  %common.ret.op.i.i563 = phi ptr [ %physical.i.i568, %fast.i.i566 ], [ %resolved.i.i561, %slow.i.i560 ]
  %r16.i = load i64, ptr %common.ret.op.i.i563, align 1
  store ptr @f_v466_RawStorage__int64_capacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %r17.i564 = call i64 @chali_host_raw_storage(i64 4, i64 %r16.i, i64 0, i64 0, i64 0, i64 1)
  %4 = icmp slt i64 %r17.i564, 0
  store ptr @f_v464_RawStorage__int64_pointer, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i.i)
  br i1 %4, label %ast_block_0.i.i, label %f_v162_std__lang__assert_bool.exit.i

ast_block_0.i.i:                                  ; preds = %ast_block_6.i.thread, %chali_primitive_address.exit.i562
  store ptr @f_v162_std__lang__assert_bool, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 782, ptr @chali_source_line, align 8
  store i64 632, ptr %indirect_arguments.i.i, align 8
  store i8 0, ptr %native_argument_types.i.i, align 1
  %r8.i.i = call i64 @chali_host_call(ptr nonnull @native_name3, i64 1, ptr nonnull %indirect_arguments.i.i, ptr nonnull %native_argument_types.i.i)
  %r9.i.i = call i8 @chali_host_result_float()
  br label %f_v162_std__lang__assert_bool.exit.i

f_v162_std__lang__assert_bool.exit.i.critedge:    ; preds = %chali_primitive_address.exit25.i
  store ptr @f_v464_RawStorage__int64_pointer, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i.i)
  br label %f_v162_std__lang__assert_bool.exit.i

f_v162_std__lang__assert_bool.exit.i:             ; preds = %f_v162_std__lang__assert_bool.exit.i.critedge, %ast_block_0.i.i, %chali_primitive_address.exit.i562
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i.i)
  %5 = call i64 @chali_host_allocate(i64 noundef 16) #27
  call void @chali_host_memory_prepare(i64 8, i64 %5, i64 0, i64 16, ptr nonnull %raw_memory_output.i468, ptr nonnull %raw_memory_input.i469)
  %r33.i = load ptr, ptr %raw_memory_output.i468, align 8
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 1 dereferenceable(16) %r33.i, i8 0, i64 16, i1 false)
  store i64 %5, ptr %indirect_arguments.i466, align 8
  store i8 0, ptr %native_argument_types.i467, align 1
  %r39.i472 = getelementptr inbounds nuw i8, ptr %indirect_arguments.i466, i64 8
  store i64 16, ptr %r39.i472, align 8
  %r40.i = getelementptr inbounds nuw i8, ptr %native_argument_types.i467, i64 1
  store i8 0, ptr %r40.i, align 1
  %r41.i = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments.i466, ptr nonnull %native_argument_types.i467)
  %r42.i = call i8 @chali_host_result_float()
  store ptr @f_v464_RawStorage__int64_pointer, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %revision_pointer.i50.i473 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i51.i474 = icmp eq ptr %revision_pointer.i50.i473, null
  br i1 %has_revision.not.i51.i474, label %slow.i66.i489, label %check.i52.i475

check.i52.i475:                                   ; preds = %f_v162_std__lang__assert_bool.exit.i
  %revision.i53.i476 = load atomic i64, ptr %revision_pointer.i50.i473 acquire, align 8
  %stamp.i54.i477 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i55.i478 = icmp eq i64 %revision.i53.i476, %stamp.i54.i477
  %enabled.i56.i479 = icmp ne i64 %revision.i53.i476, 0
  %live.i57.i480 = and i1 %enabled.i56.i479, %same_revision.i55.i478
  %begin.i58.i481 = load i64, ptr @chali_address_view, align 16
  %size.i59.i482 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i60.i483 = sub i64 %1, %begin.i58.i481
  %inside.i61.i484 = icmp ult i64 %offset.i60.i483, %size.i59.i482
  %remaining.i62.i485 = sub i64 %size.i59.i482, %offset.i60.i483
  %fits.i63.i486 = icmp ugt i64 %remaining.i62.i485, 7
  %range.i64.i487 = and i1 %inside.i61.i484, %fits.i63.i486
  %valid.i65.i488 = and i1 %live.i57.i480, %range.i64.i487
  br i1 %valid.i65.i488, label %fast.i69.i539, label %slow.i66.i489

fast.i69.i539:                                    ; preds = %check.i52.i475
  %base.i70.i540 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i71.i541 = getelementptr i8, ptr %base.i70.i540, i64 %offset.i60.i483
  br label %chali_primitive_address.exit72.i491

slow.i66.i489:                                    ; preds = %check.i52.i475, %f_v162_std__lang__assert_bool.exit.i
  %resolved.i67.i490 = call ptr @chali_host_address_view(i64 %1, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit72.i491

chali_primitive_address.exit72.i491:              ; preds = %slow.i66.i489, %fast.i69.i539
  %common.ret.op.i68.i492 = phi ptr [ %physical.i71.i541, %fast.i69.i539 ], [ %resolved.i67.i490, %slow.i66.i489 ]
  %r48.i = load i64, ptr %common.ret.op.i68.i492, align 1
  %revision_pointer.i27.i493 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i28.i494 = icmp eq ptr %revision_pointer.i27.i493, null
  br i1 %has_revision.not.i28.i494, label %slow.i43.i509, label %check.i29.i495

check.i29.i495:                                   ; preds = %chali_primitive_address.exit72.i491
  %revision.i30.i496 = load atomic i64, ptr %revision_pointer.i27.i493 acquire, align 8
  %stamp.i31.i497 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i32.i498 = icmp eq i64 %revision.i30.i496, %stamp.i31.i497
  %enabled.i33.i499 = icmp ne i64 %revision.i30.i496, 0
  %live.i34.i500 = and i1 %enabled.i33.i499, %same_revision.i32.i498
  %begin.i35.i501 = load i64, ptr @chali_address_view, align 16
  %size.i36.i502 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i37.i503 = sub i64 %5, %begin.i35.i501
  %inside.i38.i504 = icmp ult i64 %offset.i37.i503, %size.i36.i502
  %remaining.i39.i505 = sub i64 %size.i36.i502, %offset.i37.i503
  %fits.i40.i506 = icmp ugt i64 %remaining.i39.i505, 7
  %range.i41.i507 = and i1 %inside.i38.i504, %fits.i40.i506
  %valid.i42.i508 = and i1 %live.i34.i500, %range.i41.i507
  br i1 %valid.i42.i508, label %fast.i46.i536, label %slow.i43.i509

fast.i46.i536:                                    ; preds = %check.i29.i495
  %base.i47.i537 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i48.i538 = getelementptr i8, ptr %base.i47.i537, i64 %offset.i37.i503
  br label %chali_primitive_address.exit49.i511

slow.i43.i509:                                    ; preds = %check.i29.i495, %chali_primitive_address.exit72.i491
  %resolved.i44.i510 = call ptr @chali_host_address_view(i64 %5, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit49.i511

chali_primitive_address.exit49.i511:              ; preds = %slow.i43.i509, %fast.i46.i536
  %common.ret.op.i45.i512 = phi ptr [ %physical.i48.i538, %fast.i46.i536 ], [ %resolved.i44.i510, %slow.i43.i509 ]
  store i64 %r48.i, ptr %common.ret.op.i45.i512, align 1
  %r59.i = add i64 %5, 8
  %revision_pointer.i.i513 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i514 = icmp eq ptr %revision_pointer.i.i513, null
  br i1 %has_revision.not.i.i514, label %slow.i.i529, label %check.i.i515

check.i.i515:                                     ; preds = %chali_primitive_address.exit49.i511
  %revision.i.i516 = load atomic i64, ptr %revision_pointer.i.i513 acquire, align 8
  %stamp.i.i517 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i518 = icmp eq i64 %revision.i.i516, %stamp.i.i517
  %enabled.i.i519 = icmp ne i64 %revision.i.i516, 0
  %live.i.i520 = and i1 %enabled.i.i519, %same_revision.i.i518
  %begin.i.i521 = load i64, ptr @chali_address_view, align 16
  %size.i.i522 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i523 = sub i64 %r59.i, %begin.i.i521
  %inside.i.i524 = icmp ult i64 %offset.i.i523, %size.i.i522
  %remaining.i.i525 = sub i64 %size.i.i522, %offset.i.i523
  %fits.i.i526 = icmp ugt i64 %remaining.i.i525, 7
  %range.i.i527 = and i1 %inside.i.i524, %fits.i.i526
  %valid.i.i528 = and i1 %live.i.i520, %range.i.i527
  br i1 %valid.i.i528, label %fast.i.i533, label %slow.i.i529

fast.i.i533:                                      ; preds = %check.i.i515
  %base.i.i534 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i535 = getelementptr i8, ptr %base.i.i534, i64 %offset.i.i523
  br label %chali_primitive_address.exit.i531

slow.i.i529:                                      ; preds = %check.i.i515, %chali_primitive_address.exit49.i511
  %resolved.i.i530 = call ptr @chali_host_address_view(i64 %r59.i, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit.i531

chali_primitive_address.exit.i531:                ; preds = %slow.i.i529, %fast.i.i533
  %common.ret.op.i.i532 = phi ptr [ %physical.i.i535, %fast.i.i533 ], [ %resolved.i.i530, %slow.i.i529 ]
  store i64 0, ptr %common.ret.op.i.i532, align 1
  store i64 48, ptr %indirect_arguments.i466, align 8
  store i8 0, ptr %native_argument_types.i467, align 1
  store i64 16, ptr %r39.i472, align 8
  store i8 0, ptr %r40.i, align 1
  %r67.i = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments.i466, ptr nonnull %native_argument_types.i467)
  %r68.i = call i8 @chali_host_result_float()
  %r73.i = call i64 @chali_host_copy_words(i64 48, i64 %5, i64 2)
  store i64 48, ptr %indirect_arguments.i466, align 8
  store i8 0, ptr %native_argument_types.i467, align 1
  store i64 16, ptr %r39.i472, align 8
  store i8 0, ptr %r40.i, align 1
  %r80.i = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments.i466, ptr nonnull %native_argument_types.i467)
  %r81.i = call i8 @chali_host_result_float()
  %r84.not.i = icmp eq i64 %5, 0
  br i1 %r84.not.i, label %f_v464_RawStorage__int64_pointer.exit, label %ast_block_8.i

ast_block_8.i:                                    ; preds = %chali_primitive_address.exit.i531
  call void @chali_host_drop(i64 noundef %5) #27
  br label %f_v464_RawStorage__int64_pointer.exit

f_v464_RawStorage__int64_pointer.exit:            ; preds = %chali_primitive_address.exit.i531, %ast_block_8.i
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i466)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i467)
  call void @llvm.lifetime.end.p0(ptr nonnull %raw_memory_output.i468)
  call void @llvm.lifetime.end.p0(ptr nonnull %raw_memory_input.i469)
  %r123 = call i64 @chali_host_copy_words(i64 %2, i64 48, i64 2)
  store i64 %2, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 16, ptr %r51, align 8
  store i8 0, ptr %r52, align 1
  %r130 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r131 = call i8 @chali_host_result_float()
  %r136.not = icmp eq i64 %2, 48
  br i1 %r136.not, label %ast_block_17, label %ast_block_15

ast_block_15:                                     ; preds = %f_v464_RawStorage__int64_pointer.exit
  store i64 48, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 16, ptr %r51, align 8
  store i8 0, ptr %r52, align 1
  %r145 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 16
  store i64 48, ptr %r145, align 8
  %r146 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r146, align 1
  %r147 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r148 = call i8 @chali_host_result_float()
  br label %ast_block_17

ast_block_17:                                     ; preds = %f_v464_RawStorage__int64_pointer.exit, %ast_block_15
  store ptr @f_v53_std__lang____bootStringFind, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 38, ptr @chali_source_line, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %raw_memory_output.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %raw_memory_input.i)
  %6 = call i64 @chali_host_allocate(i64 noundef 16) #27
  %r5.i441 = call i64 @chali_host_copy_words(i64 %6, i64 %2, i64 2)
  store i64 %6, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  %r10.i442 = getelementptr inbounds nuw i8, ptr %indirect_arguments.i, i64 8
  store i64 16, ptr %r10.i442, align 8
  %r11.i443 = getelementptr inbounds nuw i8, ptr %native_argument_types.i, i64 1
  store i8 0, ptr %r11.i443, align 1
  %r12.i = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r13.i = call i8 @chali_host_result_float()
  %7 = call i64 @chali_host_allocate(i64 noundef 8) #27
  call void @chali_host_memory_prepare(i64 8, i64 %7, i64 0, i64 8, ptr nonnull %raw_memory_output.i, ptr nonnull %raw_memory_input.i)
  %r17.i = load ptr, ptr %raw_memory_output.i, align 8
  store i64 0, ptr %r17.i, align 1
  store i64 %7, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  store i64 8, ptr %r10.i442, align 8
  store i8 0, ptr %r11.i443, align 1
  %r25.i = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r26.i = call i8 @chali_host_result_float()
  store ptr @f_v271_std__lang__barePointer__int64, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1225, ptr @chali_source_line, align 8
  %revision_pointer.i50.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i51.i = icmp eq ptr %revision_pointer.i50.i, null
  br i1 %has_revision.not.i51.i, label %slow.i66.i, label %check.i52.i

check.i52.i:                                      ; preds = %ast_block_17
  %revision.i53.i = load atomic i64, ptr %revision_pointer.i50.i acquire, align 8
  %stamp.i54.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i55.i = icmp eq i64 %revision.i53.i, %stamp.i54.i
  %enabled.i56.i = icmp ne i64 %revision.i53.i, 0
  %live.i57.i = and i1 %enabled.i56.i, %same_revision.i55.i
  %begin.i58.i = load i64, ptr @chali_address_view, align 16
  %size.i59.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i60.i = sub i64 %6, %begin.i58.i
  %inside.i61.i = icmp ult i64 %offset.i60.i, %size.i59.i
  %remaining.i62.i = sub i64 %size.i59.i, %offset.i60.i
  %fits.i63.i = icmp ugt i64 %remaining.i62.i, 7
  %range.i64.i = and i1 %inside.i61.i, %fits.i63.i
  %valid.i65.i = and i1 %live.i57.i, %range.i64.i
  br i1 %valid.i65.i, label %fast.i69.i, label %slow.i66.i

fast.i69.i:                                       ; preds = %check.i52.i
  %base.i70.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i71.i = getelementptr i8, ptr %base.i70.i, i64 %offset.i60.i
  br label %chali_primitive_address.exit72.i

slow.i66.i:                                       ; preds = %check.i52.i, %ast_block_17
  %resolved.i67.i = call ptr @chali_host_address_view(i64 %6, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit72.i

chali_primitive_address.exit72.i:                 ; preds = %slow.i66.i, %fast.i69.i
  %common.ret.op.i68.i = phi ptr [ %physical.i71.i, %fast.i69.i ], [ %resolved.i67.i, %slow.i66.i ]
  %r32.i = load i64, ptr %common.ret.op.i68.i, align 1
  %r36.i = add i64 %6, 8
  store ptr @f_v271_std__lang__barePointer__int64, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1225, ptr @chali_source_line, align 8
  %revision_pointer.i27.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i28.i = icmp eq ptr %revision_pointer.i27.i, null
  br i1 %has_revision.not.i28.i, label %slow.i43.i, label %check.i29.i

check.i29.i:                                      ; preds = %chali_primitive_address.exit72.i
  %revision.i30.i = load atomic i64, ptr %revision_pointer.i27.i acquire, align 8
  %stamp.i31.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i32.i = icmp eq i64 %revision.i30.i, %stamp.i31.i
  %enabled.i33.i = icmp ne i64 %revision.i30.i, 0
  %live.i34.i = and i1 %enabled.i33.i, %same_revision.i32.i
  %begin.i35.i = load i64, ptr @chali_address_view, align 16
  %size.i36.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i37.i = sub i64 %r36.i, %begin.i35.i
  %inside.i38.i = icmp ult i64 %offset.i37.i, %size.i36.i
  %remaining.i39.i = sub i64 %size.i36.i, %offset.i37.i
  %fits.i40.i = icmp ugt i64 %remaining.i39.i, 7
  %range.i41.i = and i1 %inside.i38.i, %fits.i40.i
  %valid.i42.i = and i1 %live.i34.i, %range.i41.i
  br i1 %valid.i42.i, label %fast.i46.i, label %slow.i43.i

fast.i46.i:                                       ; preds = %check.i29.i
  %base.i47.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i48.i = getelementptr i8, ptr %base.i47.i, i64 %offset.i37.i
  br label %chali_primitive_address.exit49.i

slow.i43.i:                                       ; preds = %check.i29.i, %chali_primitive_address.exit72.i
  %resolved.i44.i = call ptr @chali_host_address_view(i64 %r36.i, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit49.i

chali_primitive_address.exit49.i:                 ; preds = %slow.i43.i, %fast.i46.i
  %common.ret.op.i45.i = phi ptr [ %physical.i48.i, %fast.i46.i ], [ %resolved.i44.i, %slow.i43.i ]
  %r38.i = load i64, ptr %common.ret.op.i45.i, align 1
  store ptr @f_v271_std__lang__barePointer__int64, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1225, ptr @chali_source_line, align 8
  %r39.i = call i64 @chali_host_raw_storage(i64 6, i64 %r32.i, i64 %r38.i, i64 0, i64 0, i64 0)
  %revision_pointer.i.i444 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i445 = icmp eq ptr %revision_pointer.i.i444, null
  br i1 %has_revision.not.i.i445, label %slow.i.i460, label %check.i.i446

check.i.i446:                                     ; preds = %chali_primitive_address.exit49.i
  %revision.i.i447 = load atomic i64, ptr %revision_pointer.i.i444 acquire, align 8
  %stamp.i.i448 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i449 = icmp eq i64 %revision.i.i447, %stamp.i.i448
  %enabled.i.i450 = icmp ne i64 %revision.i.i447, 0
  %live.i.i451 = and i1 %enabled.i.i450, %same_revision.i.i449
  %begin.i.i452 = load i64, ptr @chali_address_view, align 16
  %size.i.i453 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i454 = sub i64 %7, %begin.i.i452
  %inside.i.i455 = icmp ult i64 %offset.i.i454, %size.i.i453
  %remaining.i.i456 = sub i64 %size.i.i453, %offset.i.i454
  %fits.i.i457 = icmp ugt i64 %remaining.i.i456, 7
  %range.i.i458 = and i1 %inside.i.i455, %fits.i.i457
  %valid.i.i459 = and i1 %live.i.i451, %range.i.i458
  br i1 %valid.i.i459, label %fast.i.i463, label %slow.i.i460

fast.i.i463:                                      ; preds = %check.i.i446
  %base.i.i464 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i465 = getelementptr i8, ptr %base.i.i464, i64 %offset.i.i454
  br label %chali_primitive_address.exit.i

slow.i.i460:                                      ; preds = %check.i.i446, %chali_primitive_address.exit49.i
  %resolved.i.i461 = call ptr @chali_host_address_view(i64 %7, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit.i

chali_primitive_address.exit.i:                   ; preds = %slow.i.i460, %fast.i.i463
  %common.ret.op.i.i462 = phi ptr [ %physical.i.i465, %fast.i.i463 ], [ %resolved.i.i461, %slow.i.i460 ]
  store i64 %r39.i, ptr %common.ret.op.i.i462, align 1
  store i64 656, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  store i64 8, ptr %r10.i442, align 8
  store i8 0, ptr %r11.i443, align 1
  %r51.i = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r52.i = call i8 @chali_host_result_float()
  %r57.i = call i64 @chali_host_copy_words(i64 656, i64 %7, i64 1)
  store i64 656, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  store i64 8, ptr %r10.i442, align 8
  store i8 0, ptr %r11.i443, align 1
  %r64.i = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r65.i = call i8 @chali_host_result_float()
  %r68.not.i = icmp eq i64 %6, 0
  br i1 %r68.not.i, label %ast_block_2.i, label %ast_block_0.i

ast_block_0.i:                                    ; preds = %chali_primitive_address.exit.i
  call void @chali_host_drop(i64 noundef %6) #27
  br label %ast_block_2.i

ast_block_2.i:                                    ; preds = %ast_block_0.i, %chali_primitive_address.exit.i
  %r75.not.i = icmp eq i64 %7, 0
  br i1 %r75.not.i, label %f_v271_std__lang__barePointer__int64.exit, label %ast_block_3.i

ast_block_3.i:                                    ; preds = %ast_block_2.i
  call void @chali_host_drop(i64 noundef %7) #27
  br label %f_v271_std__lang__barePointer__int64.exit

f_v271_std__lang__barePointer__int64.exit:        ; preds = %ast_block_2.i, %ast_block_3.i
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %raw_memory_output.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %raw_memory_input.i)
  %r157 = call i64 @chali_host_copy_words(i64 %3, i64 656, i64 1)
  store i64 %3, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r51, align 8
  store i8 0, ptr %r52, align 1
  %r164 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r165 = call i8 @chali_host_result_float()
  %r170.not = icmp eq i64 %3, 656
  br i1 %r170.not, label %ast_block_20, label %ast_block_18

ast_block_18:                                     ; preds = %f_v271_std__lang__barePointer__int64.exit
  store i64 656, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r51, align 8
  store i8 0, ptr %r52, align 1
  %r179 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 16
  store i64 49, ptr %r179, align 8
  %r180 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r180, align 1
  %r181 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r182 = call i8 @chali_host_result_float()
  br label %ast_block_20

ast_block_20:                                     ; preds = %f_v271_std__lang__barePointer__int64.exit, %ast_block_18
  store ptr @f_v53_std__lang____bootStringFind, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 38, ptr @chali_source_line, align 8
  %revision_pointer.i255 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i256 = icmp eq ptr %revision_pointer.i255, null
  br i1 %has_revision.not.i256, label %slow.i271, label %check.i257

check.i257:                                       ; preds = %ast_block_20
  %revision.i258 = load atomic i64, ptr %revision_pointer.i255 acquire, align 8
  %stamp.i259 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i260 = icmp eq i64 %revision.i258, %stamp.i259
  %enabled.i261 = icmp ne i64 %revision.i258, 0
  %live.i262 = and i1 %enabled.i261, %same_revision.i260
  %begin.i263 = load i64, ptr @chali_address_view, align 16
  %size.i264 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i265 = sub i64 %3, %begin.i263
  %inside.i266 = icmp ult i64 %offset.i265, %size.i264
  %remaining.i267 = sub i64 %size.i264, %offset.i265
  %fits.i268 = icmp ugt i64 %remaining.i267, 7
  %range.i269 = and i1 %inside.i266, %fits.i268
  %valid.i270 = and i1 %live.i262, %range.i269
  br i1 %valid.i270, label %fast.i274, label %slow.i271

fast.i274:                                        ; preds = %check.i257
  %base.i275 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i276 = getelementptr i8, ptr %base.i275, i64 %offset.i265
  br label %chali_primitive_address.exit277

slow.i271:                                        ; preds = %check.i257, %ast_block_20
  %resolved.i272 = call ptr @chali_host_address_view(i64 %3, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit277

chali_primitive_address.exit277:                  ; preds = %fast.i274, %slow.i271
  %common.ret.op.i273 = phi ptr [ %physical.i276, %fast.i274 ], [ %resolved.i272, %slow.i271 ]
  %r188 = load i64, ptr %common.ret.op.i273, align 1
  %r191.not = icmp eq i64 %3, 0
  br i1 %r191.not, label %ast_block_23, label %ast_block_21

ast_block_21:                                     ; preds = %chali_primitive_address.exit277
  call void @chali_host_drop(i64 noundef %3) #27
  br label %ast_block_23

ast_block_23:                                     ; preds = %chali_primitive_address.exit277, %ast_block_21
  %r198.not = icmp eq i64 %2, 0
  br i1 %r198.not, label %ast_block_26, label %ast_block_24

ast_block_24:                                     ; preds = %ast_block_23
  call void @chali_host_drop(i64 noundef %2) #27
  br label %ast_block_26

ast_block_26:                                     ; preds = %ast_block_23, %ast_block_24
  %8 = icmp eq i64 %r188, 0
  br i1 %8, label %9, label %chali_require_reference.exit569

9:                                                ; preds = %ast_block_26
  call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit569:                  ; preds = %ast_block_26
  %revision_pointer.i232 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i233 = icmp eq ptr %revision_pointer.i232, null
  br i1 %has_revision.not.i233, label %slow.i248, label %check.i234

check.i234:                                       ; preds = %chali_require_reference.exit569
  %revision.i235 = load atomic i64, ptr %revision_pointer.i232 acquire, align 8
  %stamp.i236 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i237 = icmp eq i64 %revision.i235, %stamp.i236
  %enabled.i238 = icmp ne i64 %revision.i235, 0
  %live.i239 = and i1 %enabled.i238, %same_revision.i237
  %begin.i240 = load i64, ptr @chali_address_view, align 16
  %size.i241 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i242 = sub i64 %r188, %begin.i240
  %inside.i243 = icmp ult i64 %offset.i242, %size.i241
  %remaining.i244 = sub i64 %size.i241, %offset.i242
  %fits.i245 = icmp ugt i64 %remaining.i244, 7
  %range.i246 = and i1 %inside.i243, %fits.i245
  %valid.i247 = and i1 %live.i239, %range.i246
  br i1 %valid.i247, label %fast.i251, label %slow.i248

fast.i251:                                        ; preds = %check.i234
  %base.i252 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i253 = getelementptr i8, ptr %base.i252, i64 %offset.i242
  br label %chali_primitive_address.exit254

slow.i248:                                        ; preds = %check.i234, %chali_require_reference.exit569
  %resolved.i249 = call ptr @chali_host_address_view(i64 %r188, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit254

chali_primitive_address.exit254:                  ; preds = %fast.i251, %slow.i248
  %common.ret.op.i250 = phi ptr [ %physical.i253, %fast.i251 ], [ %resolved.i249, %slow.i248 ]
  store i64 0, ptr %common.ret.op.i250, align 1
  store ptr @f_v53_std__lang____bootStringFind, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 42, ptr @chali_source_line, align 8
  %r214602 = icmp sgt i64 %p3, 1
  br i1 %r214602, label %ast_block_27.lr.ph, label %ast_label_v529___chali_loop_7.preheader

ast_block_27.lr.ph:                               ; preds = %chali_primitive_address.exit254
  %r249 = add i64 %r188, -8
  br label %ast_block_27

ast_label_v529___chali_loop_7.preheader:          ; preds = %chali_primitive_address.exit208, %chali_primitive_address.exit254
  store ptr @f_v53_std__lang____bootStringFind, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 51, ptr @chali_source_line, align 8
  %r295609 = icmp slt i64 %p4, %p1
  br i1 %r295609, label %ast_block_40.lr.ph, label %ast_label_v532___chali_brk_8

ast_block_40.lr.ph:                               ; preds = %ast_label_v529___chali_loop_7.preheader
  %r332 = add i64 %r188, -8
  br label %ast_block_40

ast_block_27:                                     ; preds = %ast_block_27.lr.ph, %chali_primitive_address.exit208
  %s_v512___ast_53_matched.0604 = phi i64 [ 0, %ast_block_27.lr.ph ], [ %spec.select, %chali_primitive_address.exit208 ]
  %s_v510___ast_53_i_664.0603 = phi i64 [ 1, %ast_block_27.lr.ph ], [ %r288, %chali_primitive_address.exit208 ]
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i413 = add i64 %s_v510___ast_53_i_664.0603, %p2
  %10 = icmp eq i64 %r4.i413, 0
  br i1 %10, label %11, label %chali_require_reference.exit571

11:                                               ; preds = %ast_block_27
  call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit571:                  ; preds = %ast_block_27
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i416 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i417 = icmp eq ptr %revision_pointer.i.i416, null
  br i1 %has_revision.not.i.i417, label %slow.i.i432, label %check.i.i418

check.i.i418:                                     ; preds = %chali_require_reference.exit571
  %revision.i.i419 = load atomic i64, ptr %revision_pointer.i.i416 acquire, align 8
  %stamp.i.i420 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i421 = icmp eq i64 %revision.i.i419, %stamp.i.i420
  %enabled.i.i422 = icmp ne i64 %revision.i.i419, 0
  %live.i.i423 = and i1 %enabled.i.i422, %same_revision.i.i421
  %begin.i.i424 = load i64, ptr @chali_address_view, align 16
  %size.i.i425 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i426 = sub i64 %r4.i413, %begin.i.i424
  %inside.i.i427 = icmp ult i64 %offset.i.i426, %size.i.i425
  %valid.i.i431 = and i1 %live.i.i423, %inside.i.i427
  br i1 %valid.i.i431, label %fast.i.i437, label %slow.i.i432

fast.i.i437:                                      ; preds = %check.i.i418
  %base.i.i438 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i439 = getelementptr i8, ptr %base.i.i438, i64 %offset.i.i426
  br label %f_v49_std__lang____bootStringByte.exit440

slow.i.i432:                                      ; preds = %check.i.i418, %chali_require_reference.exit571
  %resolved.i.i433 = call ptr @chali_host_address_view(i64 %r4.i413, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit440

f_v49_std__lang____bootStringByte.exit440:        ; preds = %fast.i.i437, %slow.i.i432
  %common.ret.op.i.i434 = phi ptr [ %physical.i.i439, %fast.i.i437 ], [ %resolved.i.i433, %slow.i.i432 ]
  %r10.i435 = load i8, ptr %common.ret.op.i.i434, align 1
  store ptr @f_v53_std__lang____bootStringFind, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 44, ptr @chali_source_line, align 8
  %r225599 = icmp sgt i64 %s_v512___ast_53_matched.0604, 0
  br i1 %r225599, label %ast_block_31, label %ast_label_v527___chali_brk_6

ast_block_31:                                     ; preds = %f_v49_std__lang____bootStringByte.exit440, %chali_primitive_address.exit231
  %s_v512___ast_53_matched.1600 = phi i64 [ %r256, %chali_primitive_address.exit231 ], [ %s_v512___ast_53_matched.0604, %f_v49_std__lang____bootStringByte.exit440 ]
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i385 = add i64 %s_v512___ast_53_matched.1600, %p2
  %12 = icmp eq i64 %r4.i385, 0
  br i1 %12, label %13, label %chali_require_reference.exit573

13:                                               ; preds = %ast_block_31
  call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit573:                  ; preds = %ast_block_31
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i388 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i389 = icmp eq ptr %revision_pointer.i.i388, null
  br i1 %has_revision.not.i.i389, label %slow.i.i404, label %check.i.i390

check.i.i390:                                     ; preds = %chali_require_reference.exit573
  %revision.i.i391 = load atomic i64, ptr %revision_pointer.i.i388 acquire, align 8
  %stamp.i.i392 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i393 = icmp eq i64 %revision.i.i391, %stamp.i.i392
  %enabled.i.i394 = icmp ne i64 %revision.i.i391, 0
  %live.i.i395 = and i1 %enabled.i.i394, %same_revision.i.i393
  %begin.i.i396 = load i64, ptr @chali_address_view, align 16
  %size.i.i397 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i398 = sub i64 %r4.i385, %begin.i.i396
  %inside.i.i399 = icmp ult i64 %offset.i.i398, %size.i.i397
  %valid.i.i403 = and i1 %live.i.i395, %inside.i.i399
  br i1 %valid.i.i403, label %fast.i.i409, label %slow.i.i404

fast.i.i409:                                      ; preds = %check.i.i390
  %base.i.i410 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i411 = getelementptr i8, ptr %base.i.i410, i64 %offset.i.i398
  br label %f_v49_std__lang____bootStringByte.exit412

slow.i.i404:                                      ; preds = %check.i.i390, %chali_require_reference.exit573
  %resolved.i.i405 = call ptr @chali_host_address_view(i64 %r4.i385, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit412

f_v49_std__lang____bootStringByte.exit412:        ; preds = %fast.i.i409, %slow.i.i404
  %common.ret.op.i.i406 = phi ptr [ %physical.i.i411, %fast.i.i409 ], [ %resolved.i.i405, %slow.i.i404 ]
  %r10.i407 = load i8, ptr %common.ret.op.i.i406, align 1
  store ptr @f_v53_std__lang____bootStringFind, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 44, ptr @chali_source_line, align 8
  %r236.not = icmp eq i8 %r10.i435, %r10.i407
  br i1 %r236.not, label %ast_label_v527___chali_brk_6, label %ast_block_34

ast_block_34:                                     ; preds = %f_v49_std__lang____bootStringByte.exit412
  %r246 = shl i64 %s_v512___ast_53_matched.1600, 3
  store ptr @f_v53_std__lang____bootStringFind, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 45, ptr @chali_source_line, align 8
  %r250 = add i64 %r249, %r246
  %14 = icmp eq i64 %r250, 0
  br i1 %14, label %15, label %chali_require_reference.exit575

15:                                               ; preds = %ast_block_34
  call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit575:                  ; preds = %ast_block_34
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 45, ptr @chali_source_line, align 8
  %revision_pointer.i209 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i210 = icmp eq ptr %revision_pointer.i209, null
  br i1 %has_revision.not.i210, label %slow.i225, label %check.i211

check.i211:                                       ; preds = %chali_require_reference.exit575
  %revision.i212 = load atomic i64, ptr %revision_pointer.i209 acquire, align 8
  %stamp.i213 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i214 = icmp eq i64 %revision.i212, %stamp.i213
  %enabled.i215 = icmp ne i64 %revision.i212, 0
  %live.i216 = and i1 %enabled.i215, %same_revision.i214
  %begin.i217 = load i64, ptr @chali_address_view, align 16
  %size.i218 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i219 = sub i64 %r250, %begin.i217
  %inside.i220 = icmp ult i64 %offset.i219, %size.i218
  %remaining.i221 = sub i64 %size.i218, %offset.i219
  %fits.i222 = icmp ugt i64 %remaining.i221, 7
  %range.i223 = and i1 %inside.i220, %fits.i222
  %valid.i224 = and i1 %live.i216, %range.i223
  br i1 %valid.i224, label %fast.i228, label %slow.i225

fast.i228:                                        ; preds = %check.i211
  %base.i229 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i230 = getelementptr i8, ptr %base.i229, i64 %offset.i219
  br label %chali_primitive_address.exit231

slow.i225:                                        ; preds = %check.i211, %chali_require_reference.exit575
  %resolved.i226 = call ptr @chali_host_address_view(i64 %r250, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit231

chali_primitive_address.exit231:                  ; preds = %fast.i228, %slow.i225
  %common.ret.op.i227 = phi ptr [ %physical.i230, %fast.i228 ], [ %resolved.i226, %slow.i225 ]
  %r256 = load i64, ptr %common.ret.op.i227, align 1
  store ptr @f_v53_std__lang____bootStringFind, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 44, ptr @chali_source_line, align 8
  %r225 = icmp sgt i64 %r256, 0
  br i1 %r225, label %ast_block_31, label %ast_label_v527___chali_brk_6

ast_label_v527___chali_brk_6:                     ; preds = %f_v49_std__lang____bootStringByte.exit412, %chali_primitive_address.exit231, %f_v49_std__lang____bootStringByte.exit440
  %s_v512___ast_53_matched.1.lcssa = phi i64 [ %s_v512___ast_53_matched.0604, %f_v49_std__lang____bootStringByte.exit440 ], [ %r256, %chali_primitive_address.exit231 ], [ %s_v512___ast_53_matched.1600, %f_v49_std__lang____bootStringByte.exit412 ]
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i357 = add i64 %s_v512___ast_53_matched.1.lcssa, %p2
  %16 = icmp eq i64 %r4.i357, 0
  br i1 %16, label %17, label %chali_require_reference.exit577

17:                                               ; preds = %ast_label_v527___chali_brk_6
  call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit577:                  ; preds = %ast_label_v527___chali_brk_6
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i360 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i361 = icmp eq ptr %revision_pointer.i.i360, null
  br i1 %has_revision.not.i.i361, label %slow.i.i376, label %check.i.i362

check.i.i362:                                     ; preds = %chali_require_reference.exit577
  %revision.i.i363 = load atomic i64, ptr %revision_pointer.i.i360 acquire, align 8
  %stamp.i.i364 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i365 = icmp eq i64 %revision.i.i363, %stamp.i.i364
  %enabled.i.i366 = icmp ne i64 %revision.i.i363, 0
  %live.i.i367 = and i1 %enabled.i.i366, %same_revision.i.i365
  %begin.i.i368 = load i64, ptr @chali_address_view, align 16
  %size.i.i369 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i370 = sub i64 %r4.i357, %begin.i.i368
  %inside.i.i371 = icmp ult i64 %offset.i.i370, %size.i.i369
  %valid.i.i375 = and i1 %live.i.i367, %inside.i.i371
  br i1 %valid.i.i375, label %fast.i.i381, label %slow.i.i376

fast.i.i381:                                      ; preds = %check.i.i362
  %base.i.i382 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i383 = getelementptr i8, ptr %base.i.i382, i64 %offset.i.i370
  br label %f_v49_std__lang____bootStringByte.exit384

slow.i.i376:                                      ; preds = %check.i.i362, %chali_require_reference.exit577
  %resolved.i.i377 = call ptr @chali_host_address_view(i64 %r4.i357, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit384

f_v49_std__lang____bootStringByte.exit384:        ; preds = %fast.i.i381, %slow.i.i376
  %common.ret.op.i.i378 = phi ptr [ %physical.i.i383, %fast.i.i381 ], [ %resolved.i.i377, %slow.i.i376 ]
  %r10.i379 = load i8, ptr %common.ret.op.i.i378, align 1
  %r265 = icmp eq i8 %r10.i435, %r10.i379
  %r270 = zext i1 %r265 to i64
  %spec.select = add i64 %s_v512___ast_53_matched.1.lcssa, %r270
  %r277 = shl i64 %s_v510___ast_53_i_664.0603, 3
  store ptr @f_v53_std__lang____bootStringFind, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 48, ptr @chali_source_line, align 8
  %r278 = add i64 %r277, %r188
  %18 = icmp eq i64 %r278, 0
  br i1 %18, label %19, label %chali_require_reference.exit579

19:                                               ; preds = %f_v49_std__lang____bootStringByte.exit384
  call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit579:                  ; preds = %f_v49_std__lang____bootStringByte.exit384
  %revision_pointer.i186 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i187 = icmp eq ptr %revision_pointer.i186, null
  br i1 %has_revision.not.i187, label %slow.i202, label %check.i188

check.i188:                                       ; preds = %chali_require_reference.exit579
  %revision.i189 = load atomic i64, ptr %revision_pointer.i186 acquire, align 8
  %stamp.i190 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i191 = icmp eq i64 %revision.i189, %stamp.i190
  %enabled.i192 = icmp ne i64 %revision.i189, 0
  %live.i193 = and i1 %enabled.i192, %same_revision.i191
  %begin.i194 = load i64, ptr @chali_address_view, align 16
  %size.i195 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i196 = sub i64 %r278, %begin.i194
  %inside.i197 = icmp ult i64 %offset.i196, %size.i195
  %remaining.i198 = sub i64 %size.i195, %offset.i196
  %fits.i199 = icmp ugt i64 %remaining.i198, 7
  %range.i200 = and i1 %inside.i197, %fits.i199
  %valid.i201 = and i1 %live.i193, %range.i200
  br i1 %valid.i201, label %fast.i205, label %slow.i202

fast.i205:                                        ; preds = %check.i188
  %base.i206 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i207 = getelementptr i8, ptr %base.i206, i64 %offset.i196
  br label %chali_primitive_address.exit208

slow.i202:                                        ; preds = %check.i188, %chali_require_reference.exit579
  %resolved.i203 = call ptr @chali_host_address_view(i64 %r278, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit208

chali_primitive_address.exit208:                  ; preds = %fast.i205, %slow.i202
  %common.ret.op.i204 = phi ptr [ %physical.i207, %fast.i205 ], [ %resolved.i203, %slow.i202 ]
  store i64 %spec.select, ptr %common.ret.op.i204, align 1
  %r288 = add nuw nsw i64 %s_v510___ast_53_i_664.0603, 1
  store ptr @f_v53_std__lang____bootStringFind, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 42, ptr @chali_source_line, align 8
  %exitcond.not = icmp eq i64 %r288, %p3
  br i1 %exitcond.not, label %ast_label_v529___chali_loop_7.preheader, label %ast_block_27

ast_block_40:                                     ; preds = %ast_block_40.lr.ph, %ast_block_55
  %s_v512___ast_53_matched.3611 = phi i64 [ 0, %ast_block_40.lr.ph ], [ %spec.select630, %ast_block_55 ]
  %s_v510___ast_53_i_664.1610 = phi i64 [ %p4, %ast_block_40.lr.ph ], [ %r411, %ast_block_55 ]
  store ptr @f_v53_std__lang____bootStringFind, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 52, ptr @chali_source_line, align 8
  %revision_pointer.i163 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i164 = icmp eq ptr %revision_pointer.i163, null
  br i1 %has_revision.not.i164, label %slow.i179, label %check.i165

check.i165:                                       ; preds = %ast_block_40
  %revision.i166 = load atomic i64, ptr %revision_pointer.i163 acquire, align 8
  %stamp.i167 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i168 = icmp eq i64 %revision.i166, %stamp.i167
  %enabled.i169 = icmp ne i64 %revision.i166, 0
  %live.i170 = and i1 %enabled.i169, %same_revision.i168
  %begin.i171 = load i64, ptr @chali_address_view, align 16
  %size.i172 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i173 = sub i64 %0, %begin.i171
  %inside.i174 = icmp ult i64 %offset.i173, %size.i172
  %remaining.i175 = sub i64 %size.i172, %offset.i173
  %fits.i176 = icmp ugt i64 %remaining.i175, 7
  %range.i177 = and i1 %inside.i174, %fits.i176
  %valid.i178 = and i1 %live.i170, %range.i177
  br i1 %valid.i178, label %fast.i182, label %slow.i179

fast.i182:                                        ; preds = %check.i165
  %base.i183 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i184 = getelementptr i8, ptr %base.i183, i64 %offset.i173
  br label %chali_primitive_address.exit185

slow.i179:                                        ; preds = %check.i165, %ast_block_40
  %resolved.i180 = call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit185

chali_primitive_address.exit185:                  ; preds = %fast.i182, %slow.i179
  %common.ret.op.i181 = phi ptr [ %physical.i184, %fast.i182 ], [ %resolved.i180, %slow.i179 ]
  %r301 = load i64, ptr %common.ret.op.i181, align 1
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i329 = add i64 %r301, %s_v510___ast_53_i_664.1610
  %20 = icmp eq i64 %r4.i329, 0
  br i1 %20, label %21, label %chali_require_reference.exit581

21:                                               ; preds = %chali_primitive_address.exit185
  call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit581:                  ; preds = %chali_primitive_address.exit185
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i332 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i333 = icmp eq ptr %revision_pointer.i.i332, null
  br i1 %has_revision.not.i.i333, label %slow.i.i348, label %check.i.i334

check.i.i334:                                     ; preds = %chali_require_reference.exit581
  %revision.i.i335 = load atomic i64, ptr %revision_pointer.i.i332 acquire, align 8
  %stamp.i.i336 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i337 = icmp eq i64 %revision.i.i335, %stamp.i.i336
  %enabled.i.i338 = icmp ne i64 %revision.i.i335, 0
  %live.i.i339 = and i1 %enabled.i.i338, %same_revision.i.i337
  %begin.i.i340 = load i64, ptr @chali_address_view, align 16
  %size.i.i341 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i342 = sub i64 %r4.i329, %begin.i.i340
  %inside.i.i343 = icmp ult i64 %offset.i.i342, %size.i.i341
  %valid.i.i347 = and i1 %live.i.i339, %inside.i.i343
  br i1 %valid.i.i347, label %fast.i.i353, label %slow.i.i348

fast.i.i353:                                      ; preds = %check.i.i334
  %base.i.i354 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i355 = getelementptr i8, ptr %base.i.i354, i64 %offset.i.i342
  br label %f_v49_std__lang____bootStringByte.exit356

slow.i.i348:                                      ; preds = %check.i.i334, %chali_require_reference.exit581
  %resolved.i.i349 = call ptr @chali_host_address_view(i64 %r4.i329, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit356

f_v49_std__lang____bootStringByte.exit356:        ; preds = %fast.i.i353, %slow.i.i348
  %common.ret.op.i.i350 = phi ptr [ %physical.i.i355, %fast.i.i353 ], [ %resolved.i.i349, %slow.i.i348 ]
  %r10.i351 = load i8, ptr %common.ret.op.i.i350, align 1
  store ptr @f_v53_std__lang____bootStringFind, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 53, ptr @chali_source_line, align 8
  %r308605 = icmp sgt i64 %s_v512___ast_53_matched.3611, 0
  br i1 %r308605, label %ast_block_44, label %ast_label_v531___chali_brk_10

ast_block_44:                                     ; preds = %f_v49_std__lang____bootStringByte.exit356, %chali_primitive_address.exit
  %s_v512___ast_53_matched.4606 = phi i64 [ %r339, %chali_primitive_address.exit ], [ %s_v512___ast_53_matched.3611, %f_v49_std__lang____bootStringByte.exit356 ]
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i301 = add i64 %s_v512___ast_53_matched.4606, %p2
  %22 = icmp eq i64 %r4.i301, 0
  br i1 %22, label %23, label %chali_require_reference.exit583

23:                                               ; preds = %ast_block_44
  call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit583:                  ; preds = %ast_block_44
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i304 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i305 = icmp eq ptr %revision_pointer.i.i304, null
  br i1 %has_revision.not.i.i305, label %slow.i.i320, label %check.i.i306

check.i.i306:                                     ; preds = %chali_require_reference.exit583
  %revision.i.i307 = load atomic i64, ptr %revision_pointer.i.i304 acquire, align 8
  %stamp.i.i308 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i309 = icmp eq i64 %revision.i.i307, %stamp.i.i308
  %enabled.i.i310 = icmp ne i64 %revision.i.i307, 0
  %live.i.i311 = and i1 %enabled.i.i310, %same_revision.i.i309
  %begin.i.i312 = load i64, ptr @chali_address_view, align 16
  %size.i.i313 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i314 = sub i64 %r4.i301, %begin.i.i312
  %inside.i.i315 = icmp ult i64 %offset.i.i314, %size.i.i313
  %valid.i.i319 = and i1 %live.i.i311, %inside.i.i315
  br i1 %valid.i.i319, label %fast.i.i325, label %slow.i.i320

fast.i.i325:                                      ; preds = %check.i.i306
  %base.i.i326 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i327 = getelementptr i8, ptr %base.i.i326, i64 %offset.i.i314
  br label %f_v49_std__lang____bootStringByte.exit328

slow.i.i320:                                      ; preds = %check.i.i306, %chali_require_reference.exit583
  %resolved.i.i321 = call ptr @chali_host_address_view(i64 %r4.i301, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit328

f_v49_std__lang____bootStringByte.exit328:        ; preds = %fast.i.i325, %slow.i.i320
  %common.ret.op.i.i322 = phi ptr [ %physical.i.i327, %fast.i.i325 ], [ %resolved.i.i321, %slow.i.i320 ]
  %r10.i323 = load i8, ptr %common.ret.op.i.i322, align 1
  store ptr @f_v53_std__lang____bootStringFind, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 53, ptr @chali_source_line, align 8
  %r319.not = icmp eq i8 %r10.i351, %r10.i323
  br i1 %r319.not, label %ast_label_v531___chali_brk_10, label %ast_block_47

ast_block_47:                                     ; preds = %f_v49_std__lang____bootStringByte.exit328
  %r329 = shl i64 %s_v512___ast_53_matched.4606, 3
  store ptr @f_v53_std__lang____bootStringFind, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 54, ptr @chali_source_line, align 8
  %r333 = add i64 %r332, %r329
  %24 = icmp eq i64 %r333, 0
  br i1 %24, label %25, label %chali_require_reference.exit585

25:                                               ; preds = %ast_block_47
  call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit585:                  ; preds = %ast_block_47
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 54, ptr @chali_source_line, align 8
  %revision_pointer.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i = icmp eq ptr %revision_pointer.i, null
  br i1 %has_revision.not.i, label %slow.i, label %check.i

check.i:                                          ; preds = %chali_require_reference.exit585
  %revision.i = load atomic i64, ptr %revision_pointer.i acquire, align 8
  %stamp.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i = icmp eq i64 %revision.i, %stamp.i
  %enabled.i = icmp ne i64 %revision.i, 0
  %live.i = and i1 %enabled.i, %same_revision.i
  %begin.i = load i64, ptr @chali_address_view, align 16
  %size.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i = sub i64 %r333, %begin.i
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

slow.i:                                           ; preds = %check.i, %chali_require_reference.exit585
  %resolved.i = call ptr @chali_host_address_view(i64 %r333, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit

chali_primitive_address.exit:                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  %r339 = load i64, ptr %common.ret.op.i, align 1
  store ptr @f_v53_std__lang____bootStringFind, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 53, ptr @chali_source_line, align 8
  %r308 = icmp sgt i64 %r339, 0
  br i1 %r308, label %ast_block_44, label %ast_label_v531___chali_brk_10

ast_label_v531___chali_brk_10:                    ; preds = %f_v49_std__lang____bootStringByte.exit328, %chali_primitive_address.exit, %f_v49_std__lang____bootStringByte.exit356
  %s_v512___ast_53_matched.4.lcssa = phi i64 [ %s_v512___ast_53_matched.3611, %f_v49_std__lang____bootStringByte.exit356 ], [ %r339, %chali_primitive_address.exit ], [ %s_v512___ast_53_matched.4606, %f_v49_std__lang____bootStringByte.exit328 ]
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i = add i64 %s_v512___ast_53_matched.4.lcssa, %p2
  %26 = icmp eq i64 %r4.i, 0
  br i1 %26, label %27, label %chali_require_reference.exit587

27:                                               ; preds = %ast_label_v531___chali_brk_10
  call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit587:                  ; preds = %ast_label_v531___chali_brk_10
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i = icmp eq ptr %revision_pointer.i.i, null
  br i1 %has_revision.not.i.i, label %slow.i.i, label %check.i.i

check.i.i:                                        ; preds = %chali_require_reference.exit587
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

slow.i.i:                                         ; preds = %check.i.i, %chali_require_reference.exit587
  %resolved.i.i = call ptr @chali_host_address_view(i64 %r4.i, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit

f_v49_std__lang____bootStringByte.exit:           ; preds = %fast.i.i, %slow.i.i
  %common.ret.op.i.i = phi ptr [ %physical.i.i, %fast.i.i ], [ %resolved.i.i, %slow.i.i ]
  %r10.i = load i8, ptr %common.ret.op.i.i, align 1
  %r348 = icmp eq i8 %r10.i351, %r10.i
  %r353 = zext i1 %r348 to i64
  %spec.select630 = add i64 %s_v512___ast_53_matched.4.lcssa, %r353
  store ptr @f_v53_std__lang____bootStringFind, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 57, ptr @chali_source_line, align 8
  %r358 = icmp eq i64 %spec.select630, %p3
  store ptr @f_v53_std__lang____bootStringFind, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  br i1 %r358, label %ast_block_53, label %ast_block_55

ast_block_53:                                     ; preds = %f_v49_std__lang____bootStringByte.exit
  %r365 = sub i64 %s_v510___ast_53_i_664.1610, %p3
  store i64 57, ptr @chali_source_line, align 8
  %r366 = add i64 %r365, 1
  store i64 %1, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r51, align 8
  store i8 0, ptr %r52, align 1
  %r373 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 16
  store i64 44, ptr %r373, align 8
  %r374 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r374, align 1
  %r375 = call i64 @chali_host_call(ptr nonnull @native_name2, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r376 = call i8 @chali_host_result_float()
  %r377.not = icmp eq i64 %r375, 0
  br i1 %r377.not, label %ast_block_58, label %ast_block_56

ast_block_56:                                     ; preds = %ast_block_53
  store i64 %1, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r51, align 8
  store i8 0, ptr %r52, align 1
  store i64 44, ptr %r373, align 8
  store i8 0, ptr %r374, align 1
  %r386 = call i64 @chali_host_call(ptr nonnull @native_name2, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r387 = call i8 @chali_host_result_float()
  %r388.not = icmp eq i64 %r386, 0
  br i1 %r388.not, label %ast_block_58, label %ast_block_59

ast_block_59:                                     ; preds = %ast_block_56
  %r391 = call i64 @f_v465_RawStorage__int64_close(i64 %1)
  br label %ast_block_58

ast_block_58:                                     ; preds = %ast_block_53, %ast_block_59, %ast_block_56
  %r395.not = icmp eq i64 %1, 0
  br i1 %r395.not, label %ast_block_64, label %ast_block_62

ast_block_62:                                     ; preds = %ast_block_58
  call void @chali_host_drop(i64 noundef %1) #27
  br label %ast_block_64

ast_block_64:                                     ; preds = %ast_block_58, %ast_block_62
  %r402.not = icmp eq i64 %0, 0
  br i1 %r402.not, label %common.ret, label %ast_block_65

ast_block_65:                                     ; preds = %ast_block_64
  call void @chali_host_drop(i64 noundef %0) #27
  br label %common.ret

ast_block_55:                                     ; preds = %f_v49_std__lang____bootStringByte.exit
  %r411 = add nsw i64 %s_v510___ast_53_i_664.1610, 1
  store i64 51, ptr @chali_source_line, align 8
  %exitcond617.not = icmp eq i64 %r411, %p1
  br i1 %exitcond617.not, label %ast_label_v532___chali_brk_8, label %ast_block_40

ast_label_v532___chali_brk_8:                     ; preds = %ast_block_55, %ast_label_v529___chali_loop_7.preheader
  store ptr @f_v53_std__lang____bootStringFind, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 60, ptr @chali_source_line, align 8
  store i64 %1, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r51, align 8
  store i8 0, ptr %r52, align 1
  %r419 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 16
  store i64 44, ptr %r419, align 8
  %r420 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r420, align 1
  %r421 = call i64 @chali_host_call(ptr nonnull @native_name2, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r422 = call i8 @chali_host_result_float()
  %r423.not = icmp eq i64 %r421, 0
  br i1 %r423.not, label %ast_block_70, label %ast_block_68

ast_block_68:                                     ; preds = %ast_label_v532___chali_brk_8
  store i64 %1, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r51, align 8
  store i8 0, ptr %r52, align 1
  store i64 44, ptr %r419, align 8
  store i8 0, ptr %r420, align 1
  %r432 = call i64 @chali_host_call(ptr nonnull @native_name2, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r433 = call i8 @chali_host_result_float()
  %r434.not = icmp eq i64 %r432, 0
  br i1 %r434.not, label %ast_block_70, label %ast_block_71

ast_block_71:                                     ; preds = %ast_block_68
  %r437 = call i64 @f_v465_RawStorage__int64_close(i64 %1)
  br label %ast_block_70

ast_block_70:                                     ; preds = %ast_label_v532___chali_brk_8, %ast_block_71, %ast_block_68
  %r441.not = icmp eq i64 %1, 0
  br i1 %r441.not, label %ast_block_76, label %ast_block_74

ast_block_74:                                     ; preds = %ast_block_70
  call void @chali_host_drop(i64 noundef %1) #27
  br label %ast_block_76

ast_block_76:                                     ; preds = %ast_block_70, %ast_block_74
  %r448.not = icmp eq i64 %0, 0
  br i1 %r448.not, label %common.ret, label %ast_block_77

ast_block_77:                                     ; preds = %ast_block_76
  call void @chali_host_drop(i64 noundef %0) #27
  br label %common.ret
}

; Function Attrs: uwtable
define internal i64 @f_v54_std__lang____bootStringNext(i64 %p0, i64 %p1, i64 %p2) #24 section "chali_code" {
entry:
  %indirect_arguments = alloca [3 x i64], align 8
  %native_argument_types = alloca [3 x i8], align 1
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i47 = add i64 %p2, %p0
  %0 = icmp eq i64 %r4.i47, 0
  br i1 %0, label %1, label %chali_require_reference.exit75

1:                                                ; preds = %entry
  tail call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit75:                   ; preds = %entry
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i50 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i51 = icmp eq ptr %revision_pointer.i.i50, null
  br i1 %has_revision.not.i.i51, label %slow.i.i66, label %check.i.i52

check.i.i52:                                      ; preds = %chali_require_reference.exit75
  %revision.i.i53 = load atomic i64, ptr %revision_pointer.i.i50 acquire, align 8
  %stamp.i.i54 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i55 = icmp eq i64 %revision.i.i53, %stamp.i.i54
  %enabled.i.i56 = icmp ne i64 %revision.i.i53, 0
  %live.i.i57 = and i1 %enabled.i.i56, %same_revision.i.i55
  %begin.i.i58 = load i64, ptr @chali_address_view, align 16
  %size.i.i59 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i60 = sub i64 %r4.i47, %begin.i.i58
  %inside.i.i61 = icmp ult i64 %offset.i.i60, %size.i.i59
  %valid.i.i65 = and i1 %live.i.i57, %inside.i.i61
  br i1 %valid.i.i65, label %fast.i.i71, label %slow.i.i66

fast.i.i71:                                       ; preds = %check.i.i52
  %base.i.i72 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i73 = getelementptr i8, ptr %base.i.i72, i64 %offset.i.i60
  br label %f_v49_std__lang____bootStringByte.exit74

slow.i.i66:                                       ; preds = %check.i.i52, %chali_require_reference.exit75
  %resolved.i.i67 = tail call ptr @chali_host_address_view(i64 %r4.i47, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit74

f_v49_std__lang____bootStringByte.exit74:         ; preds = %fast.i.i71, %slow.i.i66
  %common.ret.op.i.i68 = phi ptr [ %physical.i.i73, %fast.i.i71 ], [ %resolved.i.i67, %slow.i.i66 ]
  %r10.i69 = load i8, ptr %common.ret.op.i.i68, align 1
  %r11.i70 = zext i8 %r10.i69 to i64
  %r8 = add i64 %p2, 1
  store ptr @f_v54_std__lang____bootStringNext, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 65, ptr @chali_source_line, align 8
  %r11 = icmp sgt i8 %r10.i69, -1
  br i1 %r11, label %common.ret, label %ast_block_2

common.ret:                                       ; preds = %ast_block_48, %ast_block_44, %ast_block_41, %f_v49_std__lang____bootStringByte.exit74
  %common.ret.op = phi i64 [ %r8, %f_v49_std__lang____bootStringByte.exit74 ], [ %s_v536___ast_54_at.0.lcssa, %ast_block_41 ], [ %s_v536___ast_54_at.0.lcssa, %ast_block_44 ], [ %s_v536___ast_54_at.0.lcssa, %ast_block_48 ]
  ret i64 %common.ret.op

ast_block_2:                                      ; preds = %f_v49_std__lang____bootStringByte.exit74
  %r20 = icmp samesign ugt i8 %r10.i69, -63
  br i1 %r20, label %ast_block_4, label %ast_block_24

ast_block_4:                                      ; preds = %ast_block_2
  %r25 = icmp samesign ult i8 %r10.i69, -32
  br i1 %r25, label %ast_block_7, label %ast_block_11

ast_block_7:                                      ; preds = %ast_block_4
  %r33 = and i64 %r11.i70, 31
  br label %ast_block_9

ast_block_11:                                     ; preds = %ast_block_4
  %r41 = icmp samesign ult i8 %r10.i69, -16
  br i1 %r41, label %ast_block_14, label %ast_block_18

ast_block_14:                                     ; preds = %ast_block_11
  %r49 = and i64 %r11.i70, 15
  br label %ast_block_9

ast_block_18:                                     ; preds = %ast_block_11
  %r57 = icmp samesign ult i8 %r10.i69, -11
  br i1 %r57, label %ast_block_21, label %ast_block_24

ast_block_21:                                     ; preds = %ast_block_18
  %r65 = and i64 %r11.i70, 7
  br label %ast_block_9

ast_block_24:                                     ; preds = %ast_block_2, %ast_block_18
  store ptr @f_v54_std__lang____bootStringNext, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 70, ptr @chali_source_line, align 8
  store i64 344, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r72 = call i64 @chali_host_call(ptr nonnull @native_name3, i64 1, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r73 = call i8 @chali_host_result_float()
  br label %ast_block_9

ast_block_9:                                      ; preds = %ast_block_14, %ast_block_24, %ast_block_21, %ast_block_7
  %s_v537___ast_54_code.0 = phi i64 [ %r33, %ast_block_7 ], [ %r49, %ast_block_14 ], [ %r65, %ast_block_21 ], [ 0, %ast_block_24 ]
  %s_v540___ast_54_minimum.0 = phi i64 [ 128, %ast_block_7 ], [ 2048, %ast_block_14 ], [ 65536, %ast_block_21 ], [ 0, %ast_block_24 ]
  %r9382 = phi i1 [ true, %ast_block_7 ], [ true, %ast_block_14 ], [ true, %ast_block_21 ], [ false, %ast_block_24 ]
  %s_v542___ast_54_remaining.0 = phi i64 [ 1, %ast_block_7 ], [ 2, %ast_block_14 ], [ 3, %ast_block_21 ], [ 0, %ast_block_24 ]
  %r80 = sub i64 %p1, %r8
  %r81.not = icmp sgt i64 %s_v542___ast_54_remaining.0, %r80
  br i1 %r81.not, label %ast_block_27, label %ast_block_29

ast_block_27:                                     ; preds = %ast_block_9
  store ptr @f_v54_std__lang____bootStringNext, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 71, ptr @chali_source_line, align 8
  store i64 384, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r89 = call i64 @chali_host_call(ptr nonnull @native_name3, i64 1, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r90 = call i8 @chali_host_result_float()
  br label %ast_block_29

ast_block_29:                                     ; preds = %ast_block_9, %ast_block_27
  br i1 %r9382, label %ast_block_30, label %ast_label_v545___chali_brk_12

ast_block_30:                                     ; preds = %ast_block_29, %ast_block_35
  %s_v542___ast_54_remaining.185 = phi i64 [ %r127, %ast_block_35 ], [ %s_v542___ast_54_remaining.0, %ast_block_29 ]
  %s_v537___ast_54_code.184 = phi i64 [ %r124, %ast_block_35 ], [ %s_v537___ast_54_code.0, %ast_block_29 ]
  %s_v536___ast_54_at.083 = phi i64 [ %r104, %ast_block_35 ], [ %r8, %ast_block_29 ]
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i = add i64 %s_v536___ast_54_at.083, %p0
  %2 = icmp eq i64 %r4.i, 0
  br i1 %2, label %3, label %chali_require_reference.exit77

3:                                                ; preds = %ast_block_30
  call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit77:                   ; preds = %ast_block_30
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i = icmp eq ptr %revision_pointer.i.i, null
  br i1 %has_revision.not.i.i, label %slow.i.i, label %check.i.i

check.i.i:                                        ; preds = %chali_require_reference.exit77
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

slow.i.i:                                         ; preds = %check.i.i, %chali_require_reference.exit77
  %resolved.i.i = call ptr @chali_host_address_view(i64 %r4.i, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit

f_v49_std__lang____bootStringByte.exit:           ; preds = %fast.i.i, %slow.i.i
  %common.ret.op.i.i = phi ptr [ %physical.i.i, %fast.i.i ], [ %resolved.i.i, %slow.i.i ]
  %r10.i = load i8, ptr %common.ret.op.i.i, align 1
  %r11.i = zext i8 %r10.i to i64
  %r104 = add i64 %s_v536___ast_54_at.083, 1
  %r107 = and i64 %r11.i, 192
  %r108.not = icmp eq i64 %r107, 128
  br i1 %r108.not, label %ast_block_35, label %ast_block_33

ast_block_33:                                     ; preds = %f_v49_std__lang____bootStringByte.exit
  store ptr @f_v54_std__lang____bootStringNext, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 74, ptr @chali_source_line, align 8
  store i64 424, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r116 = call i64 @chali_host_call(ptr nonnull @native_name3, i64 1, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r117 = call i8 @chali_host_result_float()
  br label %ast_block_35

ast_block_35:                                     ; preds = %f_v49_std__lang____bootStringByte.exit, %ast_block_33
  %r120 = shl i64 %s_v537___ast_54_code.184, 6
  %r123 = and i64 %r11.i, 63
  %r124 = or disjoint i64 %r123, %r120
  %r127 = add nsw i64 %s_v542___ast_54_remaining.185, -1
  store ptr @f_v54_std__lang____bootStringNext, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 72, ptr @chali_source_line, align 8
  %r93 = icmp sgt i64 %s_v542___ast_54_remaining.185, 1
  br i1 %r93, label %ast_block_30, label %ast_label_v545___chali_brk_12

ast_label_v545___chali_brk_12:                    ; preds = %ast_block_35, %ast_block_29
  %s_v536___ast_54_at.0.lcssa = phi i64 [ %r8, %ast_block_29 ], [ %r104, %ast_block_35 ]
  %s_v537___ast_54_code.1.lcssa = phi i64 [ %s_v537___ast_54_code.0, %ast_block_29 ], [ %r124, %ast_block_35 ]
  %r132.not = icmp sge i64 %s_v537___ast_54_code.1.lcssa, %s_v540___ast_54_minimum.0
  %r137 = icmp slt i64 %s_v537___ast_54_code.1.lcssa, 1114112
  %or.cond = and i1 %r132.not, %r137
  br i1 %or.cond, label %ast_block_41, label %ast_block_48

ast_block_41:                                     ; preds = %ast_label_v545___chali_brk_12
  store ptr @f_v54_std__lang____bootStringNext, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 77, ptr @chali_source_line, align 8
  %r145 = icmp slt i64 %s_v537___ast_54_code.1.lcssa, 55296
  br i1 %r145, label %common.ret, label %ast_block_44

ast_block_44:                                     ; preds = %ast_block_41
  store ptr @source_file0, ptr @chali_source_file, align 8
  %r150 = icmp samesign ult i64 %s_v537___ast_54_code.1.lcssa, 57344
  br i1 %r150, label %ast_block_48, label %common.ret

ast_block_48:                                     ; preds = %ast_label_v545___chali_brk_12, %ast_block_44
  store ptr @f_v54_std__lang____bootStringNext, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 77, ptr @chali_source_line, align 8
  store i64 464, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r164 = call i64 @chali_host_call(ptr nonnull @native_name3, i64 1, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r165 = call i8 @chali_host_result_float()
  br label %common.ret
}

; Function Attrs: uwtable
define internal i64 @f_v55_std__lang____stringQuery(i64 %p0, i64 %p1, i64 %p2, i64 %p3) #24 section "chali_code" {
entry:
  %indirect_arguments = alloca [6 x i64], align 8
  %native_argument_types = alloca [6 x i8], align 1
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i359 = tail call i64 @chali_host_string_storage(i64 0, i64 %p0)
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i360 = tail call i64 @chali_host_string_storage(i64 0, i64 %p1)
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 84, ptr @chali_source_line, align 8
  %r10 = tail call i64 @chali_host_string_storage(i64 1, i64 %p0)
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 84, ptr @chali_source_line, align 8
  %r13 = tail call i64 @chali_host_string_storage(i64 1, i64 %p1)
  %r2.i289 = icmp slt i64 %p3, 0
  %p1.p0.i = tail call i64 @llvm.smin.i64(i64 %p3, i64 %r2.i359)
  %spec.select199 = select i1 %r2.i289, i64 %r2.i359, i64 %p1.p0.i
  %common.ret.op.i = select i1 %r2.i289, i64 0, i64 %p1.p0.i
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 87, ptr @chali_source_line, align 8
  %r22 = icmp eq i64 %p2, 0
  br i1 %r22, label %ast_block_4, label %ast_block_1

ast_block_1:                                      ; preds = %entry
  store ptr @source_file0, ptr @chali_source_file, align 8
  %r27 = icmp eq i64 %p2, 2
  br i1 %r27, label %ast_block_4, label %ast_block_6

ast_block_4:                                      ; preds = %entry, %ast_block_1
  %r35 = icmp eq i64 %p2, 2
  %spec.select = select i1 %r35, i64 0, i64 %common.ret.op.i
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 89, ptr @chali_source_line, align 8
  %r48 = tail call i64 @f_v53_std__lang____bootStringFind(i64 %r10, i64 %r2.i359, i64 %r13, i64 %r2.i360, i64 %spec.select)
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 90, ptr @chali_source_line, align 8
  br i1 %r35, label %ast_block_10, label %common.ret

ast_block_10:                                     ; preds = %ast_block_4
  store ptr @source_file0, ptr @chali_source_file, align 8
  %r57 = icmp sgt i64 %r48, -1
  %. = zext i1 %r57 to i64
  br label %common.ret

common.ret:                                       ; preds = %ast_block_173, %f_v50_std__lang____bootStringSpace.exit, %ast_block_127, %ast_block_134, %ast_block_141, %ast_block_154, %ast_block_159, %ast_block_1.i, %f_v49_std__lang____bootStringByte.exit260, %ast_block_74, %ast_block_61, %ast_block_56, %f_v49_std__lang____bootStringByte.exit.i, %ast_block_5.i, %ast_block_5.i350.us, %ast_label_v598___chali_brk_18, %ast_block_101.thread, %ast_label_v601___chali_loop_21.preheader, %ast_block_101, %ast_label_v595___chali_loop_15.preheader, %ast_block_43, %ast_block_28, %ast_block_96, %ast_block_80, %ast_block_38, %ast_block_4, %ast_block_10, %ast_block_176, %f_v49_std__lang____bootStringByte.exit, %ast_block_53, %ast_label_v594___chali_brk_14, %ast_block_22
  %common.ret.op = phi i64 [ %., %ast_block_10 ], [ 0, %ast_block_176 ], [ 0, %ast_block_96 ], [ -1, %ast_block_22 ], [ %r48, %ast_block_4 ], [ -1, %ast_label_v594___chali_brk_14 ], [ 1, %ast_block_159 ], [ 1, %ast_block_101 ], [ 0, %ast_block_38 ], [ %r190, %ast_block_53 ], [ %r11.i, %f_v49_std__lang____bootStringByte.exit ], [ %s_v589___ast_55_start_677.3438.us, %ast_block_5.i350.us ], [ -1, %ast_label_v598___chali_brk_18 ], [ 1, %ast_block_5.i ], [ %.201, %ast_block_80 ], [ 0, %ast_label_v601___chali_loop_21.preheader ], [ %spec.select466, %ast_block_28 ], [ %s_v575___ast_55_count_688.0425, %ast_block_56 ], [ 1, %ast_block_43 ], [ 1, %ast_block_74 ], [ 0, %ast_label_v595___chali_loop_15.preheader ], [ 1, %ast_block_101.thread ], [ 0, %f_v49_std__lang____bootStringByte.exit.i ], [ %r222, %ast_block_61 ], [ -1, %f_v49_std__lang____bootStringByte.exit260 ], [ 0, %ast_block_141 ], [ 0, %ast_block_134 ], [ 0, %ast_block_127 ], [ 0, %f_v50_std__lang____bootStringSpace.exit ], [ 0, %ast_block_154 ], [ 0, %ast_block_1.i ], [ %r550, %ast_block_173 ]
  ret i64 %common.ret.op

ast_block_6:                                      ; preds = %ast_block_1
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 93, ptr @chali_source_line, align 8
  %r70 = icmp eq i64 %p2, 1
  br i1 %r70, label %ast_block_16, label %ast_block_18

ast_block_16:                                     ; preds = %ast_block_6
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 95, ptr @chali_source_line, align 8
  %r84 = icmp sgt i64 %r2.i360, %r2.i359
  br i1 %r84, label %ast_block_22, label %ast_block_24

ast_block_22:                                     ; preds = %ast_block_16
  store ptr @source_file0, ptr @chali_source_file, align 8
  br label %common.ret

ast_block_24:                                     ; preds = %ast_block_16
  %r96 = sub i64 %r2.i359, %r2.i360
  %spec.select466 = tail call i64 @llvm.smin.i64(i64 %spec.select199, i64 %r96)
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 97, ptr @chali_source_line, align 8
  %r107437 = icmp sgt i64 %spec.select466, -1
  br i1 %r107437, label %ast_block_28.lr.ph, label %ast_label_v594___chali_brk_14

ast_block_28.lr.ph:                               ; preds = %ast_block_24
  %r4.i295433 = icmp sgt i64 %r2.i360, 0
  br i1 %r4.i295433, label %ast_block_28.us, label %ast_block_28

ast_block_28.us:                                  ; preds = %ast_block_28.lr.ph, %ast_block_33.us
  %s_v589___ast_55_start_677.3438.us = phi i64 [ %r127.us, %ast_block_33.us ], [ %spec.select466, %ast_block_28.lr.ph ]
  %r13.i298.us = add i64 %s_v589___ast_55_start_677.3438.us, %r10
  br label %ast_block_0.i297.us

ast_block_0.i297.us:                              ; preds = %ast_block_28.us, %ast_block_5.i350.us
  %s_v492___ast_52_i.0.i294434.us = phi i64 [ 0, %ast_block_28.us ], [ %r29.i351.us, %ast_block_5.i350.us ]
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i7.i299.us = add i64 %r13.i298.us, %s_v492___ast_52_i.0.i294434.us
  %0 = icmp eq i64 %r4.i7.i299.us, 0
  br i1 %0, label %.split.us, label %chali_require_reference.exit361.us

chali_require_reference.exit361.us:               ; preds = %ast_block_0.i297.us
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i10.i302.us = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i11.i303.us = icmp eq ptr %revision_pointer.i.i10.i302.us, null
  br i1 %has_revision.not.i.i11.i303.us, label %slow.i.i26.i318.us, label %check.i.i12.i304.us

check.i.i12.i304.us:                              ; preds = %chali_require_reference.exit361.us
  %revision.i.i13.i305.us = load atomic i64, ptr %revision_pointer.i.i10.i302.us acquire, align 8
  %stamp.i.i14.i306.us = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i15.i307.us = icmp eq i64 %revision.i.i13.i305.us, %stamp.i.i14.i306.us
  %enabled.i.i16.i308.us = icmp ne i64 %revision.i.i13.i305.us, 0
  %live.i.i17.i309.us = and i1 %enabled.i.i16.i308.us, %same_revision.i.i15.i307.us
  %begin.i.i18.i310.us = load i64, ptr @chali_address_view, align 16
  %size.i.i19.i311.us = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i20.i312.us = sub i64 %r4.i7.i299.us, %begin.i.i18.i310.us
  %inside.i.i21.i313.us = icmp ult i64 %offset.i.i20.i312.us, %size.i.i19.i311.us
  %valid.i.i25.i317.us = and i1 %live.i.i17.i309.us, %inside.i.i21.i313.us
  br i1 %valid.i.i25.i317.us, label %fast.i.i31.i355.us, label %slow.i.i26.i318.us

fast.i.i31.i355.us:                               ; preds = %check.i.i12.i304.us
  %base.i.i32.i356.us = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i33.i357.us = getelementptr i8, ptr %base.i.i32.i356.us, i64 %offset.i.i20.i312.us
  br label %f_v49_std__lang____bootStringByte.exit34.i320.us

slow.i.i26.i318.us:                               ; preds = %check.i.i12.i304.us, %chali_require_reference.exit361.us
  %resolved.i.i27.i319.us = tail call ptr @chali_host_address_view(i64 %r4.i7.i299.us, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit34.i320.us

f_v49_std__lang____bootStringByte.exit34.i320.us: ; preds = %slow.i.i26.i318.us, %fast.i.i31.i355.us
  %common.ret.op.i.i28.i321.us = phi ptr [ %physical.i.i33.i357.us, %fast.i.i31.i355.us ], [ %resolved.i.i27.i319.us, %slow.i.i26.i318.us ]
  %r10.i29.i322.us = load i8, ptr %common.ret.op.i.i28.i321.us, align 1
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i.i324.us = add i64 %s_v492___ast_52_i.0.i294434.us, %r13
  %1 = icmp eq i64 %r4.i.i324.us, 0
  br i1 %1, label %.split440.us, label %chali_require_reference.exit363.us

chali_require_reference.exit363.us:               ; preds = %f_v49_std__lang____bootStringByte.exit34.i320.us
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i.i327.us = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i.i328.us = icmp eq ptr %revision_pointer.i.i.i327.us, null
  br i1 %has_revision.not.i.i.i328.us, label %slow.i.i.i343.us, label %check.i.i.i329.us

check.i.i.i329.us:                                ; preds = %chali_require_reference.exit363.us
  %revision.i.i.i330.us = load atomic i64, ptr %revision_pointer.i.i.i327.us acquire, align 8
  %stamp.i.i.i331.us = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i.i332.us = icmp eq i64 %revision.i.i.i330.us, %stamp.i.i.i331.us
  %enabled.i.i.i333.us = icmp ne i64 %revision.i.i.i330.us, 0
  %live.i.i.i334.us = and i1 %enabled.i.i.i333.us, %same_revision.i.i.i332.us
  %begin.i.i.i335.us = load i64, ptr @chali_address_view, align 16
  %size.i.i.i336.us = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i.i337.us = sub i64 %r4.i.i324.us, %begin.i.i.i335.us
  %inside.i.i.i338.us = icmp ult i64 %offset.i.i.i337.us, %size.i.i.i336.us
  %valid.i.i.i342.us = and i1 %live.i.i.i334.us, %inside.i.i.i338.us
  br i1 %valid.i.i.i342.us, label %fast.i.i.i352.us, label %slow.i.i.i343.us

fast.i.i.i352.us:                                 ; preds = %check.i.i.i329.us
  %base.i.i.i353.us = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i.i354.us = getelementptr i8, ptr %base.i.i.i353.us, i64 %offset.i.i.i337.us
  br label %f_v49_std__lang____bootStringByte.exit.i345.us

slow.i.i.i343.us:                                 ; preds = %check.i.i.i329.us, %chali_require_reference.exit363.us
  %resolved.i.i.i344.us = tail call ptr @chali_host_address_view(i64 %r4.i.i324.us, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit.i345.us

f_v49_std__lang____bootStringByte.exit.i345.us:   ; preds = %slow.i.i.i343.us, %fast.i.i.i352.us
  %common.ret.op.i.i.i346.us = phi ptr [ %physical.i.i.i354.us, %fast.i.i.i352.us ], [ %resolved.i.i.i344.us, %slow.i.i.i343.us ]
  %r10.i.i347.us = load i8, ptr %common.ret.op.i.i.i346.us, align 1
  store ptr @f_v52_std__lang____bootStringEqualRange, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 28, ptr @chali_source_line, align 8
  %r22.not.i349.us = icmp eq i8 %r10.i29.i322.us, %r10.i.i347.us
  br i1 %r22.not.i349.us, label %ast_block_5.i350.us, label %ast_block_33.us

ast_block_33.us:                                  ; preds = %f_v49_std__lang____bootStringByte.exit.i345.us
  %r127.us = add nsw i64 %s_v589___ast_55_start_677.3438.us, -1
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 97, ptr @chali_source_line, align 8
  %r107.us = icmp sgt i64 %s_v589___ast_55_start_677.3438.us, 0
  br i1 %r107.us, label %ast_block_28.us, label %ast_label_v594___chali_brk_14

ast_block_5.i350.us:                              ; preds = %f_v49_std__lang____bootStringByte.exit.i345.us
  %r29.i351.us = add nuw nsw i64 %s_v492___ast_52_i.0.i294434.us, 1
  store ptr @f_v52_std__lang____bootStringEqualRange, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 27, ptr @chali_source_line, align 8
  %exitcond454.not = icmp eq i64 %r29.i351.us, %r2.i360
  br i1 %exitcond454.not, label %common.ret, label %ast_block_0.i297.us

ast_block_28:                                     ; preds = %ast_block_28.lr.ph
  store ptr @f_v52_std__lang____bootStringEqualRange, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 27, ptr @chali_source_line, align 8
  br label %common.ret

.split.us:                                        ; preds = %ast_block_0.i297.us
  tail call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

.split440.us:                                     ; preds = %f_v49_std__lang____bootStringByte.exit34.i320.us
  tail call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

ast_label_v594___chali_brk_14:                    ; preds = %ast_block_33.us, %ast_block_24
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 101, ptr @chali_source_line, align 8
  br label %common.ret

ast_block_18:                                     ; preds = %ast_block_6
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 103, ptr @chali_source_line, align 8
  %r133 = icmp eq i64 %p2, 3
  br i1 %r133, label %ast_block_38, label %ast_block_35

ast_block_35:                                     ; preds = %ast_block_18
  store ptr @source_file0, ptr @chali_source_file, align 8
  %r138 = icmp eq i64 %p2, 4
  br i1 %r138, label %ast_block_38, label %ast_block_40

ast_block_38:                                     ; preds = %ast_block_18, %ast_block_35
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 104, ptr @chali_source_line, align 8
  %r148 = icmp sgt i64 %r2.i360, %r2.i359
  br i1 %r148, label %common.ret, label %ast_block_43

ast_block_43:                                     ; preds = %ast_block_38
  store ptr @f_v52_std__lang____bootStringEqualRange, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 27, ptr @chali_source_line, align 8
  %r4.i291429 = icmp sgt i64 %r2.i360, 0
  br i1 %r4.i291429, label %ast_block_0.i.lr.ph, label %common.ret

ast_block_0.i.lr.ph:                              ; preds = %ast_block_43
  %r155 = icmp eq i64 %p2, 4
  %r162 = sub i64 %r2.i359, %r2.i360
  %spec.select467 = select i1 %r155, i64 %r162, i64 0
  %r13.i = add i64 %spec.select467, %r10
  br label %ast_block_0.i

ast_block_0.i:                                    ; preds = %ast_block_0.i.lr.ph, %ast_block_5.i
  %s_v492___ast_52_i.0.i430 = phi i64 [ 0, %ast_block_0.i.lr.ph ], [ %r29.i, %ast_block_5.i ]
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i7.i = add i64 %r13.i, %s_v492___ast_52_i.0.i430
  %2 = icmp eq i64 %r4.i7.i, 0
  br i1 %2, label %3, label %chali_require_reference.exit365

3:                                                ; preds = %ast_block_0.i
  tail call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit365:                  ; preds = %ast_block_0.i
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i10.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i11.i = icmp eq ptr %revision_pointer.i.i10.i, null
  br i1 %has_revision.not.i.i11.i, label %slow.i.i26.i, label %check.i.i12.i

check.i.i12.i:                                    ; preds = %chali_require_reference.exit365
  %revision.i.i13.i = load atomic i64, ptr %revision_pointer.i.i10.i acquire, align 8
  %stamp.i.i14.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i15.i = icmp eq i64 %revision.i.i13.i, %stamp.i.i14.i
  %enabled.i.i16.i = icmp ne i64 %revision.i.i13.i, 0
  %live.i.i17.i = and i1 %enabled.i.i16.i, %same_revision.i.i15.i
  %begin.i.i18.i = load i64, ptr @chali_address_view, align 16
  %size.i.i19.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i20.i = sub i64 %r4.i7.i, %begin.i.i18.i
  %inside.i.i21.i = icmp ult i64 %offset.i.i20.i, %size.i.i19.i
  %valid.i.i25.i = and i1 %live.i.i17.i, %inside.i.i21.i
  br i1 %valid.i.i25.i, label %fast.i.i31.i, label %slow.i.i26.i

fast.i.i31.i:                                     ; preds = %check.i.i12.i
  %base.i.i32.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i33.i = getelementptr i8, ptr %base.i.i32.i, i64 %offset.i.i20.i
  br label %f_v49_std__lang____bootStringByte.exit34.i

slow.i.i26.i:                                     ; preds = %check.i.i12.i, %chali_require_reference.exit365
  %resolved.i.i27.i = tail call ptr @chali_host_address_view(i64 %r4.i7.i, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit34.i

f_v49_std__lang____bootStringByte.exit34.i:       ; preds = %slow.i.i26.i, %fast.i.i31.i
  %common.ret.op.i.i28.i = phi ptr [ %physical.i.i33.i, %fast.i.i31.i ], [ %resolved.i.i27.i, %slow.i.i26.i ]
  %r10.i29.i = load i8, ptr %common.ret.op.i.i28.i, align 1
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i.i = add i64 %s_v492___ast_52_i.0.i430, %r13
  %4 = icmp eq i64 %r4.i.i, 0
  br i1 %4, label %5, label %chali_require_reference.exit367

5:                                                ; preds = %f_v49_std__lang____bootStringByte.exit34.i
  tail call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit367:                  ; preds = %f_v49_std__lang____bootStringByte.exit34.i
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i.i = icmp eq ptr %revision_pointer.i.i.i, null
  br i1 %has_revision.not.i.i.i, label %slow.i.i.i, label %check.i.i.i

check.i.i.i:                                      ; preds = %chali_require_reference.exit367
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

slow.i.i.i:                                       ; preds = %check.i.i.i, %chali_require_reference.exit367
  %resolved.i.i.i = tail call ptr @chali_host_address_view(i64 %r4.i.i, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit.i

f_v49_std__lang____bootStringByte.exit.i:         ; preds = %slow.i.i.i, %fast.i.i.i
  %common.ret.op.i.i.i = phi ptr [ %physical.i.i.i, %fast.i.i.i ], [ %resolved.i.i.i, %slow.i.i.i ]
  %r10.i.i = load i8, ptr %common.ret.op.i.i.i, align 1
  store ptr @f_v52_std__lang____bootStringEqualRange, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 28, ptr @chali_source_line, align 8
  %r22.not.i = icmp eq i8 %r10.i29.i, %r10.i.i
  br i1 %r22.not.i, label %ast_block_5.i, label %common.ret

ast_block_5.i:                                    ; preds = %f_v49_std__lang____bootStringByte.exit.i
  %r29.i = add nuw nsw i64 %s_v492___ast_52_i.0.i430, 1
  store ptr @f_v52_std__lang____bootStringEqualRange, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 27, ptr @chali_source_line, align 8
  %exitcond453.not = icmp eq i64 %r29.i, %r2.i360
  br i1 %exitcond453.not, label %common.ret, label %ast_block_0.i

ast_block_40:                                     ; preds = %ast_block_35
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 108, ptr @chali_source_line, align 8
  %r180 = icmp eq i64 %p2, 5
  store ptr @source_file0, ptr @chali_source_file, align 8
  br i1 %r180, label %ast_block_50, label %ast_block_52

ast_block_50:                                     ; preds = %ast_block_40
  store i64 109, ptr @chali_source_line, align 8
  %r185 = icmp eq i64 %r2.i360, 0
  br i1 %r185, label %ast_block_53, label %ast_label_v595___chali_loop_15.preheader

ast_label_v595___chali_loop_15.preheader:         ; preds = %ast_block_50
  %r199 = sub i64 %r2.i359, %r2.i360
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 111, ptr @chali_source_line, align 8
  %r200.not424 = icmp sgt i64 %common.ret.op.i, %r199
  br i1 %r200.not424, label %common.ret, label %ast_block_56

ast_block_53:                                     ; preds = %ast_block_50
  store ptr @source_file0, ptr @chali_source_file, align 8
  %r190 = add i64 %r2.i359, 1
  br label %common.ret

ast_block_56:                                     ; preds = %ast_label_v595___chali_loop_15.preheader, %ast_block_61
  %s_v589___ast_55_start_677.5426 = phi i64 [ %r227, %ast_block_61 ], [ %common.ret.op.i, %ast_label_v595___chali_loop_15.preheader ]
  %s_v575___ast_55_count_688.0425 = phi i64 [ %r222, %ast_block_61 ], [ 0, %ast_label_v595___chali_loop_15.preheader ]
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 112, ptr @chali_source_line, align 8
  %r213 = tail call i64 @f_v53_std__lang____bootStringFind(i64 %r10, i64 %r2.i359, i64 %r13, i64 %r2.i360, i64 %s_v589___ast_55_start_677.5426)
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 113, ptr @chali_source_line, align 8
  %r217 = icmp slt i64 %r213, 0
  br i1 %r217, label %common.ret, label %ast_block_61

ast_block_61:                                     ; preds = %ast_block_56
  %r222 = add i64 %s_v575___ast_55_count_688.0425, 1
  %r227 = add i64 %r213, %r2.i360
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 111, ptr @chali_source_line, align 8
  %r200.not = icmp sgt i64 %r227, %r199
  br i1 %r200.not, label %common.ret, label %ast_block_56

ast_block_52:                                     ; preds = %ast_block_40
  store i64 117, ptr @chali_source_line, align 8
  %r234 = icmp eq i64 %p2, 6
  store ptr @source_file0, ptr @chali_source_file, align 8
  br i1 %r234, label %ast_label_v597___chali_loop_17.preheader, label %ast_block_64

ast_label_v597___chali_loop_17.preheader:         ; preds = %ast_block_52
  store i64 119, ptr @chali_source_line, align 8
  %r241422 = icmp sgt i64 %r2.i359, 0
  br i1 %r241422, label %ast_block_66.preheader, label %ast_label_v598___chali_brk_18

ast_block_66.preheader:                           ; preds = %ast_label_v597___chali_loop_17.preheader
  %smax = tail call i64 @llvm.smax.i64(i64 %r2.i360, i64 0)
  br label %ast_block_66

ast_block_66:                                     ; preds = %ast_block_66.preheader, %ast_block_77
  %s_v581___ast_55_i_691.0423 = phi i64 [ %r287, %ast_block_77 ], [ 0, %ast_block_66.preheader ]
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 119, ptr @chali_source_line, align 8
  %exitcond451.not = icmp eq i64 %s_v581___ast_55_i_691.0423, %smax
  br i1 %exitcond451.not, label %ast_label_v598___chali_brk_18, label %ast_block_69

ast_block_69:                                     ; preds = %ast_block_66
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i261 = add i64 %s_v581___ast_55_i_691.0423, %r10
  %6 = icmp eq i64 %r4.i261, 0
  br i1 %6, label %7, label %chali_require_reference.exit369

7:                                                ; preds = %ast_block_69
  tail call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit369:                  ; preds = %ast_block_69
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i264 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i265 = icmp eq ptr %revision_pointer.i.i264, null
  br i1 %has_revision.not.i.i265, label %slow.i.i280, label %check.i.i266

check.i.i266:                                     ; preds = %chali_require_reference.exit369
  %revision.i.i267 = load atomic i64, ptr %revision_pointer.i.i264 acquire, align 8
  %stamp.i.i268 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i269 = icmp eq i64 %revision.i.i267, %stamp.i.i268
  %enabled.i.i270 = icmp ne i64 %revision.i.i267, 0
  %live.i.i271 = and i1 %enabled.i.i270, %same_revision.i.i269
  %begin.i.i272 = load i64, ptr @chali_address_view, align 16
  %size.i.i273 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i274 = sub i64 %r4.i261, %begin.i.i272
  %inside.i.i275 = icmp ult i64 %offset.i.i274, %size.i.i273
  %valid.i.i279 = and i1 %live.i.i271, %inside.i.i275
  br i1 %valid.i.i279, label %fast.i.i285, label %slow.i.i280

fast.i.i285:                                      ; preds = %check.i.i266
  %base.i.i286 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i287 = getelementptr i8, ptr %base.i.i286, i64 %offset.i.i274
  br label %f_v49_std__lang____bootStringByte.exit288

slow.i.i280:                                      ; preds = %check.i.i266, %chali_require_reference.exit369
  %resolved.i.i281 = tail call ptr @chali_host_address_view(i64 %r4.i261, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit288

f_v49_std__lang____bootStringByte.exit288:        ; preds = %fast.i.i285, %slow.i.i280
  %common.ret.op.i.i282 = phi ptr [ %physical.i.i287, %fast.i.i285 ], [ %resolved.i.i281, %slow.i.i280 ]
  %r10.i283 = load i8, ptr %common.ret.op.i.i282, align 1
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i233 = add i64 %s_v581___ast_55_i_691.0423, %r13
  %8 = icmp eq i64 %r4.i233, 0
  br i1 %8, label %9, label %chali_require_reference.exit371

9:                                                ; preds = %f_v49_std__lang____bootStringByte.exit288
  tail call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit371:                  ; preds = %f_v49_std__lang____bootStringByte.exit288
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i236 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i237 = icmp eq ptr %revision_pointer.i.i236, null
  br i1 %has_revision.not.i.i237, label %slow.i.i252, label %check.i.i238

check.i.i238:                                     ; preds = %chali_require_reference.exit371
  %revision.i.i239 = load atomic i64, ptr %revision_pointer.i.i236 acquire, align 8
  %stamp.i.i240 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i241 = icmp eq i64 %revision.i.i239, %stamp.i.i240
  %enabled.i.i242 = icmp ne i64 %revision.i.i239, 0
  %live.i.i243 = and i1 %enabled.i.i242, %same_revision.i.i241
  %begin.i.i244 = load i64, ptr @chali_address_view, align 16
  %size.i.i245 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i246 = sub i64 %r4.i233, %begin.i.i244
  %inside.i.i247 = icmp ult i64 %offset.i.i246, %size.i.i245
  %valid.i.i251 = and i1 %live.i.i243, %inside.i.i247
  br i1 %valid.i.i251, label %fast.i.i257, label %slow.i.i252

fast.i.i257:                                      ; preds = %check.i.i238
  %base.i.i258 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i259 = getelementptr i8, ptr %base.i.i258, i64 %offset.i.i246
  br label %f_v49_std__lang____bootStringByte.exit260

slow.i.i252:                                      ; preds = %check.i.i238, %chali_require_reference.exit371
  %resolved.i.i253 = tail call ptr @chali_host_address_view(i64 %r4.i233, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit260

f_v49_std__lang____bootStringByte.exit260:        ; preds = %fast.i.i257, %slow.i.i252
  %common.ret.op.i.i254 = phi ptr [ %physical.i.i259, %fast.i.i257 ], [ %resolved.i.i253, %slow.i.i252 ]
  %r10.i255 = load i8, ptr %common.ret.op.i.i254, align 1
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 121, ptr @chali_source_line, align 8
  %r270 = icmp ult i8 %r10.i283, %r10.i255
  store ptr @source_file0, ptr @chali_source_file, align 8
  br i1 %r270, label %common.ret, label %ast_block_74

ast_block_74:                                     ; preds = %f_v49_std__lang____bootStringByte.exit260
  %r280 = icmp ugt i8 %r10.i283, %r10.i255
  br i1 %r280, label %common.ret, label %ast_block_77

ast_block_77:                                     ; preds = %ast_block_74
  %r287 = add nuw nsw i64 %s_v581___ast_55_i_691.0423, 1
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 119, ptr @chali_source_line, align 8
  %exitcond452.not = icmp eq i64 %r287, %r2.i359
  br i1 %exitcond452.not, label %ast_label_v598___chali_brk_18, label %ast_block_66

ast_label_v598___chali_brk_18:                    ; preds = %ast_block_66, %ast_block_77, %ast_label_v597___chali_loop_17.preheader
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 123, ptr @chali_source_line, align 8
  %r292 = icmp slt i64 %r2.i359, %r2.i360
  store ptr @source_file0, ptr @chali_source_file, align 8
  br i1 %r292, label %common.ret, label %ast_block_80

ast_block_80:                                     ; preds = %ast_label_v598___chali_brk_18
  %r302 = icmp sgt i64 %r2.i359, %r2.i360
  %.201 = zext i1 %r302 to i64
  br label %common.ret

ast_block_64:                                     ; preds = %ast_block_52
  store i64 125, ptr @chali_source_line, align 8
  %r311 = icmp sgt i64 %p2, 6
  br i1 %r311, label %ast_block_85, label %ast_block_176

ast_block_85:                                     ; preds = %ast_block_64
  %r316 = icmp samesign ult i64 %p2, 12
  store ptr @source_file0, ptr @chali_source_file, align 8
  br i1 %r316, label %ast_block_88, label %ast_block_90

ast_block_88:                                     ; preds = %ast_block_85
  store i64 126, ptr @chali_source_line, align 8
  %r324 = icmp eq i64 %r2.i359, 0
  store ptr @source_file0, ptr @chali_source_file, align 8
  br i1 %r324, label %ast_block_92, label %ast_block_101

ast_block_92:                                     ; preds = %ast_block_88
  %r329.not = icmp eq i64 %p2, 7
  br i1 %r329.not, label %ast_block_101.thread, label %ast_block_96

ast_block_96:                                     ; preds = %ast_block_92
  store ptr @source_file0, ptr @chali_source_file, align 8
  %r337.not = icmp eq i64 %p2, 11
  br i1 %r337.not, label %ast_block_101.thread, label %common.ret

ast_block_101.thread:                             ; preds = %ast_block_92, %ast_block_96
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 128, ptr @chali_source_line, align 8
  br label %common.ret

ast_block_101:                                    ; preds = %ast_block_88
  store i64 128, ptr @chali_source_line, align 8
  %r349413 = icmp sgt i64 %r2.i359, 0
  br i1 %r349413, label %ast_block_102.lr.ph, label %common.ret

ast_block_102.lr.ph:                              ; preds = %ast_block_101
  %r400 = icmp eq i64 %p2, 7
  %r418 = icmp eq i64 %p2, 8
  %r434 = icmp eq i64 %p2, 9
  %r450 = icmp eq i64 %p2, 10
  %r475 = icmp eq i64 %p2, 11
  br label %ast_block_102

ast_block_102:                                    ; preds = %ast_block_102.lr.ph, %ast_block_159
  %s_v582___ast_55_i_700.0414 = phi i64 [ 0, %ast_block_102.lr.ph ], [ %r490, %ast_block_159 ]
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i205 = add i64 %s_v582___ast_55_i_700.0414, %r10
  %10 = icmp eq i64 %r4.i205, 0
  br i1 %10, label %11, label %chali_require_reference.exit373

11:                                               ; preds = %ast_block_102
  tail call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit373:                  ; preds = %ast_block_102
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i208 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i209 = icmp eq ptr %revision_pointer.i.i208, null
  br i1 %has_revision.not.i.i209, label %slow.i.i224, label %check.i.i210

check.i.i210:                                     ; preds = %chali_require_reference.exit373
  %revision.i.i211 = load atomic i64, ptr %revision_pointer.i.i208 acquire, align 8
  %stamp.i.i212 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i213 = icmp eq i64 %revision.i.i211, %stamp.i.i212
  %enabled.i.i214 = icmp ne i64 %revision.i.i211, 0
  %live.i.i215 = and i1 %enabled.i.i214, %same_revision.i.i213
  %begin.i.i216 = load i64, ptr @chali_address_view, align 16
  %size.i.i217 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i218 = sub i64 %r4.i205, %begin.i.i216
  %inside.i.i219 = icmp ult i64 %offset.i.i218, %size.i.i217
  %valid.i.i223 = and i1 %live.i.i215, %inside.i.i219
  br i1 %valid.i.i223, label %fast.i.i229, label %slow.i.i224

fast.i.i229:                                      ; preds = %check.i.i210
  %base.i.i230 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i231 = getelementptr i8, ptr %base.i.i230, i64 %offset.i.i218
  br label %f_v49_std__lang____bootStringByte.exit232

slow.i.i224:                                      ; preds = %check.i.i210, %chali_require_reference.exit373
  %resolved.i.i225 = tail call ptr @chali_host_address_view(i64 %r4.i205, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit232

f_v49_std__lang____bootStringByte.exit232:        ; preds = %fast.i.i229, %slow.i.i224
  %common.ret.op.i.i226 = phi ptr [ %physical.i.i231, %fast.i.i229 ], [ %resolved.i.i225, %slow.i.i224 ]
  %r10.i227 = load i8, ptr %common.ret.op.i.i226, align 1
  store ptr @source_file0, ptr @chali_source_file, align 8
  %r360 = icmp ugt i8 %r10.i227, 47
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  br i1 %r360, label %ast_block_107, label %ast_block_115

ast_block_107:                                    ; preds = %f_v49_std__lang____bootStringByte.exit232
  %r365 = icmp samesign ugt i8 %r10.i227, 57
  %r372 = icmp ugt i8 %r10.i227, 64
  br i1 %r372, label %ast_block_110, label %ast_block_115

ast_block_110:                                    ; preds = %ast_block_107
  %r377 = icmp ult i8 %r10.i227, 91
  br i1 %r377, label %ast_block_115, label %ast_block_114

ast_block_114:                                    ; preds = %ast_block_110
  %12 = add i8 %r10.i227, -123
  %spec.select468 = icmp ult i8 %12, -26
  br label %ast_block_115

ast_block_115:                                    ; preds = %ast_block_114, %ast_block_107, %f_v49_std__lang____bootStringByte.exit232, %ast_block_110
  %r368382 = phi i1 [ true, %ast_block_110 ], [ true, %f_v49_std__lang____bootStringByte.exit232 ], [ true, %ast_block_114 ], [ %r365, %ast_block_107 ]
  %r396 = phi i1 [ false, %ast_block_110 ], [ true, %f_v49_std__lang____bootStringByte.exit232 ], [ %spec.select468, %ast_block_114 ], [ true, %ast_block_107 ]
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 132, ptr @chali_source_line, align 8
  br i1 %r400, label %ast_block_122, label %ast_block_127

ast_block_122:                                    ; preds = %ast_block_115
  store ptr @f_v50_std__lang____bootStringSpace, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 17, ptr @chali_source_line, align 8
  %r2.i = icmp eq i8 %r10.i227, 32
  br i1 %r2.i, label %ast_block_159, label %ast_block_1.i

ast_block_1.i:                                    ; preds = %ast_block_122
  store ptr @source_file0, ptr @chali_source_file, align 8
  %r7.i = icmp ugt i8 %r10.i227, 8
  br i1 %r7.i, label %f_v50_std__lang____bootStringSpace.exit, label %common.ret

f_v50_std__lang____bootStringSpace.exit:          ; preds = %ast_block_1.i
  store ptr @source_file0, ptr @chali_source_file, align 8
  %r12.i = icmp ugt i8 %r10.i227, 13
  br i1 %r12.i, label %common.ret, label %ast_block_159

ast_block_127:                                    ; preds = %ast_block_115
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 133, ptr @chali_source_line, align 8
  %spec.select202 = and i1 %r418, %r368382
  br i1 %spec.select202, label %common.ret, label %ast_block_134

ast_block_134:                                    ; preds = %ast_block_127
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 134, ptr @chali_source_line, align 8
  %spec.select203 = and i1 %r434, %r396
  br i1 %spec.select203, label %common.ret, label %ast_block_141

ast_block_141:                                    ; preds = %ast_block_134
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 135, ptr @chali_source_line, align 8
  %spec.select204 = and i1 %r450, %r368382
  %r468 = and i1 %spec.select204, %r396
  br i1 %r468, label %common.ret, label %ast_block_152

ast_block_152:                                    ; preds = %ast_block_141
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 136, ptr @chali_source_line, align 8
  br i1 %r475, label %ast_block_154, label %ast_block_159

ast_block_154:                                    ; preds = %ast_block_152
  store ptr @source_file0, ptr @chali_source_file, align 8
  %r480 = icmp slt i8 %r10.i227, 0
  br i1 %r480, label %common.ret, label %ast_block_159

ast_block_159:                                    ; preds = %f_v50_std__lang____bootStringSpace.exit, %ast_block_122, %ast_block_152, %ast_block_154
  %r490 = add nuw nsw i64 %s_v582___ast_55_i_700.0414, 1
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 128, ptr @chali_source_line, align 8
  %exitcond.not = icmp eq i64 %r490, %r2.i359
  br i1 %exitcond.not, label %common.ret, label %ast_block_102

ast_block_90:                                     ; preds = %ast_block_85
  store i64 140, ptr @chali_source_line, align 8
  %r495 = icmp eq i64 %p2, 12
  br i1 %r495, label %ast_block_160, label %ast_block_162

ast_block_160:                                    ; preds = %ast_block_90
  %r500 = icmp sgt i64 %p3, -1
  %r507.not = icmp slt i64 %p3, %r2.i359
  %or.cond = select i1 %r500, i1 %r507.not, i1 false
  br i1 %or.cond, label %ast_block_169, label %ast_block_167

ast_block_167:                                    ; preds = %ast_block_160
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 141, ptr @chali_source_line, align 8
  store i64 560, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r518 = call i64 @chali_host_call(ptr nonnull @native_name3, i64 1, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r519 = call i8 @chali_host_result_float()
  br label %ast_block_169

ast_block_169:                                    ; preds = %ast_block_160, %ast_block_167
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i = add i64 %r10, %p3
  %13 = icmp eq i64 %r4.i, 0
  br i1 %13, label %14, label %chali_require_reference.exit375

14:                                               ; preds = %ast_block_169
  call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit375:                  ; preds = %ast_block_169
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i = icmp eq ptr %revision_pointer.i.i, null
  br i1 %has_revision.not.i.i, label %slow.i.i, label %check.i.i

check.i.i:                                        ; preds = %chali_require_reference.exit375
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

slow.i.i:                                         ; preds = %check.i.i, %chali_require_reference.exit375
  %resolved.i.i = call ptr @chali_host_address_view(i64 %r4.i, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit

f_v49_std__lang____bootStringByte.exit:           ; preds = %fast.i.i, %slow.i.i
  %common.ret.op.i.i = phi ptr [ %physical.i.i, %fast.i.i ], [ %resolved.i.i, %slow.i.i ]
  %r10.i = load i8, ptr %common.ret.op.i.i, align 1
  %r11.i = zext i8 %r10.i to i64
  br label %common.ret

ast_block_162:                                    ; preds = %ast_block_90
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 144, ptr @chali_source_line, align 8
  %r530 = icmp eq i64 %p2, 13
  br i1 %r530, label %ast_label_v601___chali_loop_21.preheader, label %ast_block_176

ast_label_v601___chali_loop_21.preheader:         ; preds = %ast_block_162
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 146, ptr @chali_source_line, align 8
  %r537410 = icmp sgt i64 %r2.i359, 0
  br i1 %r537410, label %ast_block_173, label %common.ret

ast_block_173:                                    ; preds = %ast_label_v601___chali_loop_21.preheader, %ast_block_173
  %s_v576___ast_55_count_710.0412 = phi i64 [ %r550, %ast_block_173 ], [ 0, %ast_label_v601___chali_loop_21.preheader ]
  %s_v574___ast_55_at_709.0411 = phi i64 [ %r546, %ast_block_173 ], [ 0, %ast_label_v601___chali_loop_21.preheader ]
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 146, ptr @chali_source_line, align 8
  %r546 = tail call i64 @f_v54_std__lang____bootStringNext(i64 %r10, i64 %r2.i359, i64 %s_v574___ast_55_at_709.0411)
  %r550 = add i64 %s_v576___ast_55_count_710.0412, 1
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 146, ptr @chali_source_line, align 8
  %r537 = icmp slt i64 %r546, %r2.i359
  br i1 %r537, label %ast_block_173, label %common.ret

ast_block_176:                                    ; preds = %ast_block_64, %ast_block_162
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 150, ptr @chali_source_line, align 8
  store i64 592, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r561 = call i64 @chali_host_call(ptr nonnull @native_name3, i64 1, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r562 = call i8 @chali_host_result_float()
  br label %common.ret
}

; Function Attrs: alwaysinline uwtable
define internal i64 @f_v56_std__lang____bootStringLength(i64 %p0) #21 section "chali_code" {
entry:
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2 = tail call i64 @chali_host_string_storage(i64 0, i64 %p0)
  ret i64 %r2
}

; Function Attrs: uwtable
define internal i64 @f_v57_std__lang____bootStringBuffer(i64 %p0) #24 section "chali_code" {
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
  store i64 320, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r18 = call i64 @chali_host_call(ptr nonnull @native_name3, i64 1, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r19 = call i8 @chali_host_result_float()
  br label %ast_block_6

ast_block_6:                                      ; preds = %entry, %ast_block_4
  store ptr @f_v57_std__lang____bootStringBuffer, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 166, ptr @chali_source_line, align 8
  %r22 = add i64 %p0, 8
  %0 = call i64 @chali_host_allocate(i64 noundef %r22) #27
  %1 = call i64 @chali_host_allocate(i64 noundef 8) #27
  call void @chali_host_memory_prepare(i64 8, i64 %1, i64 0, i64 8, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r27 = load ptr, ptr %raw_memory_output, align 8
  store i64 0, ptr %r27, align 1
  store i64 %1, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r33 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  store i64 8, ptr %r33, align 8
  %r34 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r34, align 1
  %r35 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r36 = call i8 @chali_host_result_float()
  store ptr @f_v57_std__lang____bootStringBuffer, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 167, ptr @chali_source_line, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %raw_memory_output.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %raw_memory_input.i)
  %2 = call i64 @chali_host_allocate(i64 noundef 8) #27
  call void @chali_host_memory_prepare(i64 8, i64 %2, i64 0, i64 8, ptr nonnull %raw_memory_output.i, ptr nonnull %raw_memory_input.i)
  %r3.i = load ptr, ptr %raw_memory_output.i, align 8
  store i64 0, ptr %r3.i, align 1
  store i64 %2, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  %r9.i = getelementptr inbounds nuw i8, ptr %indirect_arguments.i, i64 8
  store i64 8, ptr %r9.i, align 8
  %r10.i = getelementptr inbounds nuw i8, ptr %native_argument_types.i, i64 1
  store i8 0, ptr %r10.i, align 1
  %r11.i = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
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
  store i64 664, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  store i64 8, ptr %r9.i, align 8
  store i8 0, ptr %r10.i, align 1
  %r26.i = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r27.i = call i8 @chali_host_result_float()
  %r32.i = call i64 @chali_host_copy_words(i64 664, i64 %2, i64 1)
  store i64 664, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  store i64 8, ptr %r9.i, align 8
  store i8 0, ptr %r10.i, align 1
  %r39.i = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r40.i = call i8 @chali_host_result_float()
  %r43.not.i = icmp eq i64 %2, 0
  br i1 %r43.not.i, label %f_v273_std__lang__pointerFromAddress__int64.exit, label %ast_block_0.i

ast_block_0.i:                                    ; preds = %chali_primitive_address.exit.i
  call void @chali_host_drop(i64 noundef %2) #27
  br label %f_v273_std__lang__pointerFromAddress__int64.exit

f_v273_std__lang__pointerFromAddress__int64.exit: ; preds = %chali_primitive_address.exit.i, %ast_block_0.i
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %raw_memory_output.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %raw_memory_input.i)
  %r45 = call i64 @chali_host_copy_words(i64 %1, i64 664, i64 1)
  store i64 %1, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r33, align 8
  store i8 0, ptr %r34, align 1
  %r52 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r53 = call i8 @chali_host_result_float()
  %r58.not = icmp eq i64 %1, 664
  br i1 %r58.not, label %ast_block_9, label %ast_block_7

ast_block_7:                                      ; preds = %f_v273_std__lang__pointerFromAddress__int64.exit
  store i64 664, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r33, align 8
  store i8 0, ptr %r34, align 1
  %r67 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 16
  store i64 49, ptr %r67, align 8
  %r68 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r68, align 1
  %r69 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r70 = call i8 @chali_host_result_float()
  br label %ast_block_9

ast_block_9:                                      ; preds = %f_v273_std__lang__pointerFromAddress__int64.exit, %ast_block_7
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
  call void @chali_host_drop(i64 noundef %1) #27
  br label %ast_block_12

ast_block_12:                                     ; preds = %chali_primitive_address.exit, %ast_block_10
  ret i64 %0
}

; Function Attrs: uwtable
define internal noundef i64 @f_v58_std__lang____bootStringCopyBytes(i64 %p0, i64 %p1, i64 %p2) #24 section "chali_code" {
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
  %0 = tail call i64 @chali_host_allocate(i64 noundef 8) #27
  call void @chali_host_memory_prepare(i64 8, i64 %0, i64 0, i64 8, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r8 = load ptr, ptr %raw_memory_output, align 8
  store i64 0, ptr %r8, align 1
  store i64 %0, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r14 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  store i64 8, ptr %r14, align 8
  %r15 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r15, align 1
  %r16 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r17 = call i8 @chali_host_result_float()
  %1 = call i64 @chali_host_allocate(i64 noundef 8) #27
  call void @chali_host_memory_prepare(i64 8, i64 %1, i64 0, i64 8, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r21 = load ptr, ptr %raw_memory_output, align 8
  store i64 0, ptr %r21, align 1
  store i64 %1, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r14, align 8
  store i8 0, ptr %r15, align 1
  %r29 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r30 = call i8 @chali_host_result_float()
  store ptr @f_v58_std__lang____bootStringCopyBytes, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 172, ptr @chali_source_line, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %raw_memory_output.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %raw_memory_input.i)
  %2 = call i64 @chali_host_allocate(i64 noundef 8) #27
  call void @chali_host_memory_prepare(i64 8, i64 %2, i64 0, i64 8, ptr nonnull %raw_memory_output.i, ptr nonnull %raw_memory_input.i)
  %r3.i = load ptr, ptr %raw_memory_output.i, align 8
  store i64 0, ptr %r3.i, align 1
  store i64 %2, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  %r9.i = getelementptr inbounds nuw i8, ptr %indirect_arguments.i, i64 8
  store i64 8, ptr %r9.i, align 8
  %r10.i = getelementptr inbounds nuw i8, ptr %native_argument_types.i, i64 1
  store i8 0, ptr %r10.i, align 1
  %r11.i = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
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
  store i64 672, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  store i64 8, ptr %r9.i, align 8
  store i8 0, ptr %r10.i, align 1
  %r26.i = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r27.i = call i8 @chali_host_result_float()
  %r32.i = call i64 @chali_host_copy_words(i64 672, i64 %2, i64 1)
  store i64 672, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  store i64 8, ptr %r9.i, align 8
  store i8 0, ptr %r10.i, align 1
  %r39.i = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r40.i = call i8 @chali_host_result_float()
  %r43.not.i = icmp eq i64 %2, 0
  br i1 %r43.not.i, label %f_v275_std__lang__pointerFromAddress__uint8.exit, label %ast_block_0.i

ast_block_0.i:                                    ; preds = %chali_primitive_address.exit.i
  call void @chali_host_drop(i64 noundef %2) #27
  br label %f_v275_std__lang__pointerFromAddress__uint8.exit

f_v275_std__lang__pointerFromAddress__uint8.exit: ; preds = %chali_primitive_address.exit.i, %ast_block_0.i
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %raw_memory_output.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %raw_memory_input.i)
  %r39 = call i64 @chali_host_copy_words(i64 %0, i64 672, i64 1)
  store i64 %0, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r14, align 8
  store i8 0, ptr %r15, align 1
  %r46 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r47 = call i8 @chali_host_result_float()
  %r52.not = icmp eq i64 %0, 672
  br i1 %r52.not, label %ast_block_5, label %ast_block_3

ast_block_3:                                      ; preds = %f_v275_std__lang__pointerFromAddress__uint8.exit
  store i64 672, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r14, align 8
  store i8 0, ptr %r15, align 1
  %r61 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 16
  store i64 47, ptr %r61, align 8
  %r62 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r62, align 1
  %r63 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r64 = call i8 @chali_host_result_float()
  br label %ast_block_5

ast_block_5:                                      ; preds = %f_v275_std__lang__pointerFromAddress__uint8.exit, %ast_block_3
  store ptr @f_v58_std__lang____bootStringCopyBytes, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 172, ptr @chali_source_line, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i62)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i63)
  call void @llvm.lifetime.start.p0(ptr nonnull %raw_memory_output.i64)
  call void @llvm.lifetime.start.p0(ptr nonnull %raw_memory_input.i65)
  %3 = call i64 @chali_host_allocate(i64 noundef 8) #27
  call void @chali_host_memory_prepare(i64 8, i64 %3, i64 0, i64 8, ptr nonnull %raw_memory_output.i64, ptr nonnull %raw_memory_input.i65)
  %r3.i67 = load ptr, ptr %raw_memory_output.i64, align 8
  store i64 0, ptr %r3.i67, align 1
  store i64 %3, ptr %indirect_arguments.i62, align 8
  store i8 0, ptr %native_argument_types.i63, align 1
  %r9.i68 = getelementptr inbounds nuw i8, ptr %indirect_arguments.i62, i64 8
  store i64 8, ptr %r9.i68, align 8
  %r10.i69 = getelementptr inbounds nuw i8, ptr %native_argument_types.i63, i64 1
  store i8 0, ptr %r10.i69, align 1
  %r11.i70 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments.i62, ptr nonnull %native_argument_types.i63)
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
  store i64 672, ptr %indirect_arguments.i62, align 8
  store i8 0, ptr %native_argument_types.i63, align 1
  store i64 8, ptr %r9.i68, align 8
  store i8 0, ptr %r10.i69, align 1
  %r26.i92 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments.i62, ptr nonnull %native_argument_types.i63)
  %r27.i93 = call i8 @chali_host_result_float()
  %r32.i94 = call i64 @chali_host_copy_words(i64 672, i64 %3, i64 1)
  store i64 672, ptr %indirect_arguments.i62, align 8
  store i8 0, ptr %native_argument_types.i63, align 1
  store i64 8, ptr %r9.i68, align 8
  store i8 0, ptr %r10.i69, align 1
  %r39.i95 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments.i62, ptr nonnull %native_argument_types.i63)
  %r40.i96 = call i8 @chali_host_result_float()
  %r43.not.i97 = icmp eq i64 %3, 0
  br i1 %r43.not.i97, label %f_v275_std__lang__pointerFromAddress__uint8.exit102, label %ast_block_0.i98

ast_block_0.i98:                                  ; preds = %chali_primitive_address.exit.i90
  call void @chali_host_drop(i64 noundef %3) #27
  br label %f_v275_std__lang__pointerFromAddress__uint8.exit102

f_v275_std__lang__pointerFromAddress__uint8.exit102: ; preds = %chali_primitive_address.exit.i90, %ast_block_0.i98
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i62)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i63)
  call void @llvm.lifetime.end.p0(ptr nonnull %raw_memory_output.i64)
  call void @llvm.lifetime.end.p0(ptr nonnull %raw_memory_input.i65)
  %r75 = call i64 @chali_host_copy_words(i64 %1, i64 672, i64 1)
  store i64 %1, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r14, align 8
  store i8 0, ptr %r15, align 1
  %r82 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r83 = call i8 @chali_host_result_float()
  %r88.not = icmp eq i64 %1, 672
  br i1 %r88.not, label %ast_block_8, label %ast_block_6

ast_block_6:                                      ; preds = %f_v275_std__lang__pointerFromAddress__uint8.exit102
  store i64 672, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r14, align 8
  store i8 0, ptr %r15, align 1
  %r97 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 16
  store i64 47, ptr %r97, align 8
  %r98 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r98, align 1
  %r99 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r100 = call i8 @chali_host_result_float()
  br label %ast_block_8

ast_block_8:                                      ; preds = %f_v275_std__lang__pointerFromAddress__uint8.exit102, %ast_block_6
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
  call void @chali_host_drop(i64 noundef %1) #27
  br label %ast_block_11

ast_block_11:                                     ; preds = %chali_primitive_address.exit, %ast_block_9
  %r126.not = icmp eq i64 %0, 0
  br i1 %r126.not, label %ast_block_2, label %ast_block_12

ast_block_12:                                     ; preds = %ast_block_11
  call void @chali_host_drop(i64 noundef %0) #27
  br label %ast_block_2

ast_block_2:                                      ; preds = %ast_block_12, %ast_block_11, %entry
  ret i64 0
}

; Function Attrs: uwtable
define internal i64 @f_v60_std__lang____bootStringCopy(i64 %p0) #24 section "chali_code" {
entry:
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i = tail call i64 @chali_host_string_storage(i64 0, i64 %p0)
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 178, ptr @chali_source_line, align 8
  %r6 = tail call i64 @f_v57_std__lang____bootStringBuffer(i64 %r2.i)
  %r10 = add i64 %r6, 8
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 179, ptr @chali_source_line, align 8
  %r13 = tail call i64 @chali_host_string_storage(i64 1, i64 %p0)
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 179, ptr @chali_source_line, align 8
  %r16 = tail call i64 @f_v58_std__lang____bootStringCopyBytes(i64 %r10, i64 %r13, i64 %r2.i)
  ret i64 %r6
}

; Function Attrs: alwaysinline nounwind uwtable
define internal noundef i64 @f_v61_std__lang____bootStringDrop(i64 %p0) #25 section "chali_code" {
entry:
  tail call void @chali_host_drop(i64 noundef %p0) #27
  ret i64 0
}

; Function Attrs: uwtable
define internal i64 @f_v62_std__lang____bootStringSlice(i64 %p0, i64 %p1, i64 %p2) #24 section "chali_code" {
entry:
  %indirect_arguments = alloca [4 x i64], align 8
  %native_argument_types = alloca [4 x i8], align 1
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i30 = tail call i64 @chali_host_string_storage(i64 0, i64 %p0)
  %r7 = icmp eq i64 %p2, -1
  %spec.select = select i1 %r7, i64 %r2.i30, i64 %p2
  %r14 = icmp slt i64 %p1, 0
  %r19 = icmp slt i64 %spec.select, 0
  %or.cond = select i1 %r14, i1 true, i1 %r19
  br i1 %or.cond, label %ast_block_7, label %ast_block_9

ast_block_7:                                      ; preds = %entry
  store ptr @f_v62_std__lang____bootStringSlice, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 187, ptr @chali_source_line, align 8
  store i64 496, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r30 = call i64 @chali_host_call(ptr nonnull @native_name3, i64 1, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r31 = call i8 @chali_host_result_float()
  br label %ast_block_9

ast_block_9:                                      ; preds = %entry, %ast_block_7
  %r2.i24 = icmp slt i64 %p1, 0
  %p1.p0.i27 = call i64 @llvm.smin.i64(i64 %p1, i64 %r2.i30)
  %common.ret.op.i28 = select i1 %r2.i24, i64 0, i64 %p1.p0.i27
  %r2.i = icmp slt i64 %spec.select, 0
  %p1.p0.i = call i64 @llvm.smin.i64(i64 %spec.select, i64 %r2.i30)
  %common.ret.op.i = select i1 %r2.i, i64 0, i64 %p1.p0.i
  %spec.select23 = call i64 @llvm.smax.i64(i64 %common.ret.op.i, i64 %common.ret.op.i28)
  store ptr @f_v62_std__lang____bootStringSlice, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 190, ptr @chali_source_line, align 8
  %r55 = call i64 @chali_host_string_storage(i64 1, i64 %p0)
  %r4.i = sub i64 %spec.select23, %common.ret.op.i28
  store ptr @f_v63_std__lang____bootStringSliceBytes, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 194, ptr @chali_source_line, align 8
  %r5.i = call i64 @f_v57_std__lang____bootStringBuffer(i64 %r4.i), !inline_history !32
  %r9.i = add i64 %r5.i, 8
  %r14.i = add i64 %r55, %common.ret.op.i28
  store ptr @f_v63_std__lang____bootStringSliceBytes, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 194, ptr @chali_source_line, align 8
  %r20.i = call i64 @f_v58_std__lang____bootStringCopyBytes(i64 %r9.i, i64 %r14.i, i64 %r4.i), !inline_history !32
  ret i64 %r5.i
}

; Function Attrs: uwtable
define internal i64 @f_v63_std__lang____bootStringSliceBytes(i64 %p0, i64 %p1, i64 %p2) #24 section "chali_code" {
entry:
  %r4 = sub i64 %p2, %p1
  store ptr @f_v63_std__lang____bootStringSliceBytes, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 194, ptr @chali_source_line, align 8
  %r5 = tail call i64 @f_v57_std__lang____bootStringBuffer(i64 %r4)
  %r9 = add i64 %r5, 8
  %r14 = add i64 %p1, %p0
  store ptr @f_v63_std__lang____bootStringSliceBytes, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 194, ptr @chali_source_line, align 8
  %r20 = tail call i64 @f_v58_std__lang____bootStringCopyBytes(i64 %r9, i64 %r14, i64 %r4)
  ret i64 %r5
}

; Function Attrs: alwaysinline uwtable
define internal range(i64 -1, 2) i64 @f_v64_std__lang____bootStringCompare(i64 %p0, i64 %p1) #21 section "chali_code" {
entry:
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i57 = tail call i64 @chali_host_string_storage(i64 0, i64 %p0)
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i = tail call i64 @chali_host_string_storage(i64 0, i64 %p1)
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 199, ptr @chali_source_line, align 8
  %r10 = tail call i64 @chali_host_string_storage(i64 1, i64 %p0)
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 199, ptr @chali_source_line, align 8
  %r13 = tail call i64 @chali_host_string_storage(i64 1, i64 %p1)
  %r1861 = icmp sgt i64 %r2.i57, 0
  br i1 %r1861, label %ast_block_1.preheader, label %ast_label_v651___chali_brk_24

ast_block_1.preheader:                            ; preds = %entry
  %smax = tail call i64 @llvm.smax.i64(i64 %r2.i, i64 0)
  br label %ast_block_1

ast_block_1:                                      ; preds = %ast_block_1.preheader, %ast_block_12
  %s_v645___ast_64_i_734.062 = phi i64 [ %r64, %ast_block_12 ], [ 0, %ast_block_1.preheader ]
  %exitcond.not = icmp eq i64 %s_v645___ast_64_i_734.062, %smax
  br i1 %exitcond.not, label %ast_label_v651___chali_brk_24, label %ast_block_4

ast_block_4:                                      ; preds = %ast_block_1
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i29 = add i64 %s_v645___ast_64_i_734.062, %r10
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
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i = add i64 %s_v645___ast_64_i_734.062, %r13
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
  %r47 = icmp ult i8 %r10.i51, %r10.i
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 202, ptr @chali_source_line, align 8
  br i1 %r47, label %common.ret, label %ast_block_9

common.ret:                                       ; preds = %f_v49_std__lang____bootStringByte.exit, %ast_block_9, %ast_label_v651___chali_brk_24, %ast_block_15
  %common.ret.op = phi i64 [ -1, %ast_label_v651___chali_brk_24 ], [ %., %ast_block_15 ], [ 1, %ast_block_9 ], [ -1, %f_v49_std__lang____bootStringByte.exit ]
  ret i64 %common.ret.op

ast_block_9:                                      ; preds = %f_v49_std__lang____bootStringByte.exit
  %r57 = icmp ugt i8 %r10.i51, %r10.i
  br i1 %r57, label %common.ret, label %ast_block_12

ast_block_12:                                     ; preds = %ast_block_9
  %r64 = add nuw nsw i64 %s_v645___ast_64_i_734.062, 1
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 200, ptr @chali_source_line, align 8
  %exitcond63.not = icmp eq i64 %r64, %r2.i57
  br i1 %exitcond63.not, label %ast_label_v651___chali_brk_24, label %ast_block_1

ast_label_v651___chali_brk_24:                    ; preds = %ast_block_1, %ast_block_12, %entry
  %r69 = icmp slt i64 %r2.i57, %r2.i
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 204, ptr @chali_source_line, align 8
  br i1 %r69, label %common.ret, label %ast_block_15

ast_block_15:                                     ; preds = %ast_label_v651___chali_brk_24
  %r79 = icmp sgt i64 %r2.i57, %r2.i
  %. = zext i1 %r79 to i64
  br label %common.ret
}

; Function Attrs: alwaysinline uwtable
define internal i64 @f_v70_std__lang____bootStringAddLength(i64 %p0, i64 %p1) #21 section "chali_code" {
entry:
  %indirect_arguments = alloca [2 x i64], align 8
  %native_argument_types = alloca [2 x i8], align 1
  %r2 = icmp slt i64 %p0, 0
  %r7 = icmp slt i64 %p1, 0
  %or.cond.not10 = select i1 %r2, i1 true, i1 %r7
  %r17 = sub nsw i64 9223372036854775806, %p0
  %r18 = icmp sgt i64 %p1, %r17
  %or.cond9 = select i1 %or.cond.not10, i1 true, i1 %r18
  br i1 %or.cond9, label %ast_block_8, label %ast_block_10

ast_block_8:                                      ; preds = %entry
  store ptr @f_v70_std__lang____bootStringAddLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 250, ptr @chali_source_line, align 8
  store i64 288, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r29 = call i64 @chali_host_call(ptr nonnull @native_name3, i64 1, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r30 = call i8 @chali_host_result_float()
  br label %ast_block_10

ast_block_10:                                     ; preds = %entry, %ast_block_8
  store ptr @f_v70_std__lang____bootStringAddLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 250, ptr @chali_source_line, align 8
  %r35 = add i64 %p1, %p0
  ret i64 %r35
}

; Function Attrs: alwaysinline uwtable
define internal i64 @f_v75_std__lang__String_length(i64 %p0) #21 section "chali_code" {
entry:
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i = tail call i64 @chali_host_string_storage(i64 0, i64 %p0)
  ret i64 %r2.i
}

; Function Attrs: alwaysinline uwtable
define internal i64 @f_v77_std__lang__String_byteLength(i64 %p0) #21 section "chali_code" {
entry:
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i.i = tail call i64 @chali_host_string_storage(i64 0, i64 %p0)
  ret i64 %r2.i.i
}

; Function Attrs: uwtable
define internal i64 @f_v85_std__lang__String_indexOf(i64 %p0, i64 %p1, i64 %p2) #24 section "chali_code" {
entry:
  %0 = tail call i64 @chali_host_allocate(i64 noundef 8) #27
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i.i = tail call i64 @chali_host_string_storage(i64 0, i64 %p1), !inline_history !33
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 178, ptr @chali_source_line, align 8
  %r6.i = tail call i64 @f_v57_std__lang____bootStringBuffer(i64 %r2.i.i), !inline_history !33
  %r10.i = add i64 %r6.i, 8
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 179, ptr @chali_source_line, align 8
  %r13.i = tail call i64 @chali_host_string_storage(i64 1, i64 %p1), !inline_history !33
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 179, ptr @chali_source_line, align 8
  %r16.i = tail call i64 @f_v58_std__lang____bootStringCopyBytes(i64 %r10.i, i64 %r13.i, i64 %r2.i.i), !inline_history !33
  %revision_pointer.i82 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i83 = icmp eq ptr %revision_pointer.i82, null
  br i1 %has_revision.not.i83, label %slow.i98, label %check.i84

check.i84:                                        ; preds = %entry
  %revision.i85 = load atomic i64, ptr %revision_pointer.i82 acquire, align 8
  %stamp.i86 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i87 = icmp eq i64 %revision.i85, %stamp.i86
  %enabled.i88 = icmp ne i64 %revision.i85, 0
  %live.i89 = and i1 %enabled.i88, %same_revision.i87
  %begin.i90 = load i64, ptr @chali_address_view, align 16
  %size.i91 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i92 = sub i64 %0, %begin.i90
  %inside.i93 = icmp ult i64 %offset.i92, %size.i91
  %remaining.i94 = sub i64 %size.i91, %offset.i92
  %fits.i95 = icmp ugt i64 %remaining.i94, 7
  %range.i96 = and i1 %inside.i93, %fits.i95
  %valid.i97 = and i1 %live.i89, %range.i96
  br i1 %valid.i97, label %fast.i101, label %slow.i98

fast.i101:                                        ; preds = %check.i84
  %base.i102 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i103 = getelementptr i8, ptr %base.i102, i64 %offset.i92
  br label %chali_primitive_address.exit104

slow.i98:                                         ; preds = %check.i84, %entry
  %resolved.i99 = tail call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit104

chali_primitive_address.exit104:                  ; preds = %fast.i101, %slow.i98
  %common.ret.op.i100 = phi ptr [ %physical.i103, %fast.i101 ], [ %resolved.i99, %slow.i98 ]
  store i64 %r6.i, ptr %common.ret.op.i100, align 1
  store ptr @f_v85_std__lang__String_indexOf, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 446, ptr @chali_source_line, align 8
  %revision_pointer.i59 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i60 = icmp eq ptr %revision_pointer.i59, null
  br i1 %has_revision.not.i60, label %slow.i75, label %check.i61

check.i61:                                        ; preds = %chali_primitive_address.exit104
  %revision.i62 = load atomic i64, ptr %revision_pointer.i59 acquire, align 8
  %stamp.i63 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i64 = icmp eq i64 %revision.i62, %stamp.i63
  %enabled.i65 = icmp ne i64 %revision.i62, 0
  %live.i66 = and i1 %enabled.i65, %same_revision.i64
  %begin.i67 = load i64, ptr @chali_address_view, align 16
  %size.i68 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i69 = sub i64 %0, %begin.i67
  %inside.i70 = icmp ult i64 %offset.i69, %size.i68
  %remaining.i71 = sub i64 %size.i68, %offset.i69
  %fits.i72 = icmp ugt i64 %remaining.i71, 7
  %range.i73 = and i1 %inside.i70, %fits.i72
  %valid.i74 = and i1 %live.i66, %range.i73
  br i1 %valid.i74, label %fast.i78, label %slow.i75

fast.i78:                                         ; preds = %check.i61
  %base.i79 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i80 = getelementptr i8, ptr %base.i79, i64 %offset.i69
  br label %chali_primitive_address.exit81

slow.i75:                                         ; preds = %check.i61, %chali_primitive_address.exit104
  %resolved.i76 = tail call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit81

chali_primitive_address.exit81:                   ; preds = %fast.i78, %slow.i75
  %common.ret.op.i77 = phi ptr [ %physical.i80, %fast.i78 ], [ %resolved.i76, %slow.i75 ]
  %r13 = load i64, ptr %common.ret.op.i77, align 1
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i359.i = tail call i64 @chali_host_string_storage(i64 0, i64 %p0), !inline_history !34
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i360.i = tail call i64 @chali_host_string_storage(i64 0, i64 %r13), !inline_history !34
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 84, ptr @chali_source_line, align 8
  %r10.i105 = tail call i64 @chali_host_string_storage(i64 1, i64 %p0), !inline_history !34
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 84, ptr @chali_source_line, align 8
  %r13.i106 = tail call i64 @chali_host_string_storage(i64 1, i64 %r13), !inline_history !34
  %r2.i289.i = icmp slt i64 %p2, 0
  %p1.p0.i.i = tail call i64 @llvm.smin.i64(i64 %p2, i64 %r2.i359.i)
  %common.ret.op.i.i = select i1 %r2.i289.i, i64 0, i64 %p1.p0.i.i
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 89, ptr @chali_source_line, align 8
  %r48.i = tail call i64 @f_v53_std__lang____bootStringFind(i64 %r10.i105, i64 %r2.i359.i, i64 %r13.i106, i64 %r2.i360.i, i64 %common.ret.op.i.i), !inline_history !34
  store ptr @f_v55_std__lang____stringQuery, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 90, ptr @chali_source_line, align 8
  %revision_pointer.i36 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i37 = icmp eq ptr %revision_pointer.i36, null
  br i1 %has_revision.not.i37, label %slow.i52, label %check.i38

check.i38:                                        ; preds = %chali_primitive_address.exit81
  %revision.i39 = load atomic i64, ptr %revision_pointer.i36 acquire, align 8
  %stamp.i40 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i41 = icmp eq i64 %revision.i39, %stamp.i40
  %enabled.i42 = icmp ne i64 %revision.i39, 0
  %live.i43 = and i1 %enabled.i42, %same_revision.i41
  %begin.i44 = load i64, ptr @chali_address_view, align 16
  %size.i45 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i46 = sub i64 %0, %begin.i44
  %inside.i47 = icmp ult i64 %offset.i46, %size.i45
  %remaining.i48 = sub i64 %size.i45, %offset.i46
  %fits.i49 = icmp ugt i64 %remaining.i48, 7
  %range.i50 = and i1 %inside.i47, %fits.i49
  %valid.i51 = and i1 %live.i43, %range.i50
  br i1 %valid.i51, label %fast.i55, label %slow.i52

fast.i55:                                         ; preds = %check.i38
  %base.i56 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i57 = getelementptr i8, ptr %base.i56, i64 %offset.i46
  br label %chali_primitive_address.exit58

slow.i52:                                         ; preds = %check.i38, %chali_primitive_address.exit81
  %resolved.i53 = tail call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit58

chali_primitive_address.exit58:                   ; preds = %fast.i55, %slow.i52
  %common.ret.op.i54 = phi ptr [ %physical.i57, %fast.i55 ], [ %resolved.i53, %slow.i52 ]
  %r21 = load i64, ptr %common.ret.op.i54, align 1
  %r22.not = icmp eq i64 %r21, 0
  br i1 %r22.not, label %ast_block_2, label %ast_block_0

ast_block_0:                                      ; preds = %chali_primitive_address.exit58
  %revision_pointer.i13 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i14 = icmp eq ptr %revision_pointer.i13, null
  br i1 %has_revision.not.i14, label %slow.i29, label %check.i15

check.i15:                                        ; preds = %ast_block_0
  %revision.i16 = load atomic i64, ptr %revision_pointer.i13 acquire, align 8
  %stamp.i17 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i18 = icmp eq i64 %revision.i16, %stamp.i17
  %enabled.i19 = icmp ne i64 %revision.i16, 0
  %live.i20 = and i1 %enabled.i19, %same_revision.i18
  %begin.i21 = load i64, ptr @chali_address_view, align 16
  %size.i22 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i23 = sub i64 %0, %begin.i21
  %inside.i24 = icmp ult i64 %offset.i23, %size.i22
  %remaining.i25 = sub i64 %size.i22, %offset.i23
  %fits.i26 = icmp ugt i64 %remaining.i25, 7
  %range.i27 = and i1 %inside.i24, %fits.i26
  %valid.i28 = and i1 %live.i20, %range.i27
  br i1 %valid.i28, label %fast.i32, label %slow.i29

fast.i32:                                         ; preds = %check.i15
  %base.i33 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i34 = getelementptr i8, ptr %base.i33, i64 %offset.i23
  br label %chali_primitive_address.exit35

slow.i29:                                         ; preds = %check.i15, %ast_block_0
  %resolved.i30 = tail call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit35

chali_primitive_address.exit35:                   ; preds = %fast.i32, %slow.i29
  %common.ret.op.i31 = phi ptr [ %physical.i34, %fast.i32 ], [ %resolved.i30, %slow.i29 ]
  %r28 = load i64, ptr %common.ret.op.i31, align 1
  tail call void @chali_host_drop(i64 noundef %r28) #27
  br label %ast_block_2

ast_block_2:                                      ; preds = %chali_primitive_address.exit58, %chali_primitive_address.exit35
  %revision_pointer.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i = icmp eq ptr %revision_pointer.i, null
  br i1 %has_revision.not.i, label %slow.i, label %check.i

check.i:                                          ; preds = %ast_block_2
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

slow.i:                                           ; preds = %check.i, %ast_block_2
  %resolved.i = tail call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit

chali_primitive_address.exit:                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  store i64 0, ptr %common.ret.op.i, align 1
  %r36.not = icmp eq i64 %0, 0
  br i1 %r36.not, label %ast_block_5, label %ast_block_3

ast_block_3:                                      ; preds = %chali_primitive_address.exit
  tail call void @chali_host_drop(i64 noundef %0) #27
  br label %ast_block_5

ast_block_5:                                      ; preds = %chali_primitive_address.exit, %ast_block_3
  ret i64 %r48.i
}

; Function Attrs: uwtable
define internal range(i64 256, 273) i64 @f_v92_std__lang__String_byteAt(i64 %p0, i64 %p1) #24 section "chali_code" {
entry:
  %indirect_arguments = alloca [5 x i64], align 8
  %native_argument_types = alloca [5 x i8], align 1
  store ptr @f_v92_std__lang__String_byteAt, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 467, ptr @chali_source_line, align 8
  %r2 = icmp slt i64 %p1, 0
  br i1 %r2, label %ast_block_4, label %ast_block_1

ast_block_1:                                      ; preds = %entry
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i.i = tail call i64 @chali_host_string_storage(i64 0, i64 %p0)
  store ptr @f_v92_std__lang__String_byteAt, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 467, ptr @chali_source_line, align 8
  %r11.not = icmp slt i64 %p1, %r2.i.i
  br i1 %r11.not, label %ast_block_6, label %ast_block_4

common.ret:                                       ; preds = %f_v23___chali_enum_Option__uint8_Value.exit, %ast_block_4
  %common.ret.op = phi i64 [ 256, %ast_block_4 ], [ 272, %f_v23___chali_enum_Option__uint8_Value.exit ]
  %r87 = call i8 @chali_host_result_float()
  ret i64 %common.ret.op

ast_block_4:                                      ; preds = %entry, %ast_block_1
  store i64 256, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r21 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  store i64 16, ptr %r21, align 8
  %r22 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r22, align 1
  %r23 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r24 = call i8 @chali_host_result_float()
  store ptr @f_v92_std__lang__String_byteAt, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 467, ptr @chali_source_line, align 8
  %r29 = call i64 @chali_host_copy_words(i64 256, i64 64, i64 2)
  store i64 256, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 16, ptr %r21, align 8
  store i8 0, ptr %r22, align 1
  %r36 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  br label %common.ret

ast_block_6:                                      ; preds = %ast_block_1
  store i64 272, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r44 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  store i64 16, ptr %r44, align 8
  %r45 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r45, align 1
  %r46 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r47 = call i8 @chali_host_result_float()
  store ptr @f_v92_std__lang__String_byteAt, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 467, ptr @chali_source_line, align 8
  %r52 = call i64 @f_v55_std__lang____stringQuery(i64 %p0, i64 0, i64 12, i64 %p1)
  %r55 = and i64 %r52, 255
  store ptr @f_v92_std__lang__String_byteAt, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 467, ptr @chali_source_line, align 8
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
  %offset.i.i = sub i64 88, %begin.i.i
  %inside.i.i = icmp ult i64 %offset.i.i, %size.i.i
  %remaining.i.i = sub i64 %size.i.i, %offset.i.i
  %fits.i.i = icmp ugt i64 %remaining.i.i, 7
  %range.i.i = and i1 %inside.i.i, %fits.i.i
  %valid.i.i = and i1 %live.i.i, %range.i.i
  br i1 %valid.i.i, label %fast.i.i, label %slow.i.i

fast.i.i:                                         ; preds = %check.i.i
  %base.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i = getelementptr i8, ptr %base.i.i, i64 %offset.i.i
  br label %f_v23___chali_enum_Option__uint8_Value.exit

slow.i.i:                                         ; preds = %check.i.i, %ast_block_6
  %resolved.i.i = call ptr @chali_host_address_view(i64 88, i64 8, ptr nonnull @chali_address_view)
  br label %f_v23___chali_enum_Option__uint8_Value.exit

f_v23___chali_enum_Option__uint8_Value.exit:      ; preds = %fast.i.i, %slow.i.i
  %common.ret.op.i.i = phi ptr [ %physical.i.i, %fast.i.i ], [ %resolved.i.i, %slow.i.i ]
  store i64 %r55, ptr %common.ret.op.i.i, align 1
  %r62 = call i64 @chali_host_copy_words(i64 272, i64 80, i64 2)
  store i64 272, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 16, ptr %r44, align 8
  store i8 0, ptr %r45, align 1
  %r69 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r70 = call i8 @chali_host_result_float()
  store i64 80, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 16, ptr %r44, align 8
  store i8 0, ptr %r45, align 1
  %r84 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 16
  store i64 282, ptr %r84, align 8
  %r85 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r85, align 1
  %r86 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  br label %common.ret
}

; Function Attrs: alwaysinline uwtable
define internal noundef i64 @f_v162_std__lang__assert_bool(i64 %p0) #21 section "chali_code" {
entry:
  %indirect_arguments = alloca [2 x i64], align 8
  %native_argument_types = alloca [2 x i8], align 1
  %r2.not = icmp eq i64 %p0, 0
  br i1 %r2.not, label %ast_block_0, label %ast_block_2

ast_block_0:                                      ; preds = %entry
  store ptr @f_v162_std__lang__assert_bool, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 782, ptr @chali_source_line, align 8
  store i64 632, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r8 = call i64 @chali_host_call(ptr nonnull @native_name3, i64 1, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r9 = call i8 @chali_host_result_float()
  br label %ast_block_2

ast_block_2:                                      ; preds = %entry, %ast_block_0
  ret i64 0
}

; Function Attrs: uwtable
define noundef i64 @__chali_impl_program__setup() #24 section "chali_code" {
entry:
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i.i = tail call i64 @chali_host_string_storage(i64 0, i64 168), !inline_history !33
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 178, ptr @chali_source_line, align 8
  %r6.i = tail call i64 @f_v57_std__lang____bootStringBuffer(i64 %r2.i.i), !inline_history !33
  %r10.i = add i64 %r6.i, 8
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 179, ptr @chali_source_line, align 8
  %r13.i = tail call i64 @chali_host_string_storage(i64 1, i64 168), !inline_history !33
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 179, ptr @chali_source_line, align 8
  %r16.i = tail call i64 @f_v58_std__lang____bootStringCopyBytes(i64 %r10.i, i64 %r13.i, i64 %r2.i.i), !inline_history !33
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i.i173 = tail call i64 @chali_host_string_storage(i64 0, i64 208), !inline_history !33
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 178, ptr @chali_source_line, align 8
  %r6.i174 = tail call i64 @f_v57_std__lang____bootStringBuffer(i64 %r2.i.i173), !inline_history !33
  %r10.i175 = add i64 %r6.i174, 8
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 179, ptr @chali_source_line, align 8
  %r13.i176 = tail call i64 @chali_host_string_storage(i64 1, i64 208), !inline_history !33
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 179, ptr @chali_source_line, align 8
  %r16.i177 = tail call i64 @f_v58_std__lang____bootStringCopyBytes(i64 %r10.i175, i64 %r13.i176, i64 %r2.i.i173), !inline_history !33
  store ptr @__chali_impl_program__setup, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 11, ptr @chali_source_line, align 8
  %r6 = tail call i64 @f_v474_std__lang____bootStringJoin2(i64 %r6.i, i64 240)
  %r8 = load i64, ptr @s_g_2_program__firstChunk, align 8
  %r10.not = icmp eq i64 %r8, 0
  br i1 %r10.not, label %ast_block_2, label %ast_block_0

ast_block_0:                                      ; preds = %entry
  tail call void @chali_host_drop(i64 noundef %r8) #27
  br label %ast_block_2

ast_block_2:                                      ; preds = %entry, %ast_block_0
  store i64 %r6, ptr @s_g_2_program__firstChunk, align 8
  store ptr @__chali_impl_program__setup, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 12, ptr @chali_source_line, align 8
  %r21 = tail call i64 @f_v474_std__lang____bootStringJoin2(i64 %r6.i, i64 248)
  %r23 = load i64, ptr @s_g_3_program__secondChunk, align 8
  %r25.not = icmp eq i64 %r23, 0
  br i1 %r25.not, label %ast_block_5, label %ast_block_3

ast_block_3:                                      ; preds = %ast_block_2
  tail call void @chali_host_drop(i64 noundef %r23) #27
  br label %ast_block_5

ast_block_5:                                      ; preds = %ast_block_2, %ast_block_3
  store i64 %r21, ptr @s_g_3_program__secondChunk, align 8
  %r34 = load i64, ptr @s_g_2_program__firstChunk, align 8
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i.i.i = tail call i64 @chali_host_string_storage(i64 0, i64 %r34)
  store i64 %r2.i.i.i, ptr @s_g_4_program__unitWidth, align 8
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i.i178 = tail call i64 @chali_host_string_storage(i64 0, i64 112), !inline_history !33
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 178, ptr @chali_source_line, align 8
  %r6.i179 = tail call i64 @f_v57_std__lang____bootStringBuffer(i64 %r2.i.i178), !inline_history !33
  %r10.i180 = add i64 %r6.i179, 8
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 179, ptr @chali_source_line, align 8
  %r13.i181 = tail call i64 @chali_host_string_storage(i64 1, i64 112), !inline_history !33
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 179, ptr @chali_source_line, align 8
  %r16.i182 = tail call i64 @f_v58_std__lang____bootStringCopyBytes(i64 %r10.i180, i64 %r13.i181, i64 %r2.i.i178), !inline_history !33
  %r40.b = load i1, ptr @s_g_1___chali_global_reference, align 1
  %r40 = select i1 %r40.b, i64 104, i64 0
  %revision_pointer.i150 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i151 = icmp eq ptr %revision_pointer.i150, null
  br i1 %has_revision.not.i151, label %slow.i166, label %check.i152

check.i152:                                       ; preds = %ast_block_5
  %revision.i153 = load atomic i64, ptr %revision_pointer.i150 acquire, align 8
  %stamp.i154 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i155 = icmp eq i64 %revision.i153, %stamp.i154
  %enabled.i156 = icmp ne i64 %revision.i153, 0
  %live.i157 = and i1 %enabled.i156, %same_revision.i155
  %begin.i158 = load i64, ptr @chali_address_view, align 16
  %size.i159 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i160 = sub i64 %r40, %begin.i158
  %inside.i161 = icmp ult i64 %offset.i160, %size.i159
  %remaining.i162 = sub i64 %size.i159, %offset.i160
  %fits.i163 = icmp ugt i64 %remaining.i162, 7
  %range.i164 = and i1 %inside.i161, %fits.i163
  %valid.i165 = and i1 %live.i157, %range.i164
  br i1 %valid.i165, label %fast.i169, label %slow.i166

fast.i169:                                        ; preds = %check.i152
  %base.i170 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i171 = getelementptr i8, ptr %base.i170, i64 %offset.i160
  br label %chali_primitive_address.exit172

slow.i166:                                        ; preds = %check.i152, %ast_block_5
  %resolved.i167 = tail call ptr @chali_host_address_view(i64 %r40, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit172

chali_primitive_address.exit172:                  ; preds = %fast.i169, %slow.i166
  %common.ret.op.i168 = phi ptr [ %physical.i171, %fast.i169 ], [ %resolved.i167, %slow.i166 ]
  %r45 = load i64, ptr %common.ret.op.i168, align 1
  %r46.not = icmp eq i64 %r45, 0
  br i1 %r46.not, label %ast_block_8, label %ast_block_6

ast_block_6:                                      ; preds = %chali_primitive_address.exit172
  %revision_pointer.i127 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i128 = icmp eq ptr %revision_pointer.i127, null
  br i1 %has_revision.not.i128, label %slow.i143, label %check.i129

check.i129:                                       ; preds = %ast_block_6
  %revision.i130 = load atomic i64, ptr %revision_pointer.i127 acquire, align 8
  %stamp.i131 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i132 = icmp eq i64 %revision.i130, %stamp.i131
  %enabled.i133 = icmp ne i64 %revision.i130, 0
  %live.i134 = and i1 %enabled.i133, %same_revision.i132
  %begin.i135 = load i64, ptr @chali_address_view, align 16
  %size.i136 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i137 = sub i64 %r40, %begin.i135
  %inside.i138 = icmp ult i64 %offset.i137, %size.i136
  %remaining.i139 = sub i64 %size.i136, %offset.i137
  %fits.i140 = icmp ugt i64 %remaining.i139, 7
  %range.i141 = and i1 %inside.i138, %fits.i140
  %valid.i142 = and i1 %live.i134, %range.i141
  br i1 %valid.i142, label %fast.i146, label %slow.i143

fast.i146:                                        ; preds = %check.i129
  %base.i147 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i148 = getelementptr i8, ptr %base.i147, i64 %offset.i137
  br label %chali_primitive_address.exit149

slow.i143:                                        ; preds = %check.i129, %ast_block_6
  %resolved.i144 = tail call ptr @chali_host_address_view(i64 %r40, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit149

chali_primitive_address.exit149:                  ; preds = %fast.i146, %slow.i143
  %common.ret.op.i145 = phi ptr [ %physical.i148, %fast.i146 ], [ %resolved.i144, %slow.i143 ]
  %r52 = load i64, ptr %common.ret.op.i145, align 1
  tail call void @chali_host_drop(i64 noundef %r52) #27
  br label %ast_block_8

ast_block_8:                                      ; preds = %chali_primitive_address.exit172, %chali_primitive_address.exit149
  %revision_pointer.i104 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i105 = icmp eq ptr %revision_pointer.i104, null
  br i1 %has_revision.not.i105, label %slow.i120, label %check.i106

check.i106:                                       ; preds = %ast_block_8
  %revision.i107 = load atomic i64, ptr %revision_pointer.i104 acquire, align 8
  %stamp.i108 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i109 = icmp eq i64 %revision.i107, %stamp.i108
  %enabled.i110 = icmp ne i64 %revision.i107, 0
  %live.i111 = and i1 %enabled.i110, %same_revision.i109
  %begin.i112 = load i64, ptr @chali_address_view, align 16
  %size.i113 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i114 = sub i64 %r40, %begin.i112
  %inside.i115 = icmp ult i64 %offset.i114, %size.i113
  %remaining.i116 = sub i64 %size.i113, %offset.i114
  %fits.i117 = icmp ugt i64 %remaining.i116, 7
  %range.i118 = and i1 %inside.i115, %fits.i117
  %valid.i119 = and i1 %live.i111, %range.i118
  br i1 %valid.i119, label %fast.i123, label %slow.i120

fast.i123:                                        ; preds = %check.i106
  %base.i124 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i125 = getelementptr i8, ptr %base.i124, i64 %offset.i114
  br label %chali_primitive_address.exit126

slow.i120:                                        ; preds = %check.i106, %ast_block_8
  %resolved.i121 = tail call ptr @chali_host_address_view(i64 %r40, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit126

chali_primitive_address.exit126:                  ; preds = %fast.i123, %slow.i120
  %common.ret.op.i122 = phi ptr [ %physical.i125, %fast.i123 ], [ %resolved.i121, %slow.i120 ]
  store i64 %r6.i179, ptr %common.ret.op.i122, align 1
  br label %ast_block_9

ast_block_9:                                      ; preds = %chali_primitive_address.exit126, %ast_block_20
  %s_v678___ast_234_index_1323.0183 = phi i64 [ 0, %chali_primitive_address.exit126 ], [ %r127, %ast_block_20 ]
  %.lhs.trunc = trunc nuw i64 %s_v678___ast_234_index_1323.0183 to i8
  %0 = urem i8 %.lhs.trunc, 26
  %.zext = zext nneg i8 %0 to i64
  %r83 = add nuw nsw i64 %.zext, 1
  store ptr @__chali_impl_program__setup, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 18, ptr @chali_source_line, align 8
  %r84 = tail call i64 @f_v62_std__lang____bootStringSlice(i64 %r6.i174, i64 %.zext, i64 %r83)
  %r86.b = load i1, ptr @s_g_1___chali_global_reference, align 1
  %r86 = select i1 %r86.b, i64 104, i64 0
  store ptr @__chali_impl_program__setup, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 18, ptr @chali_source_line, align 8
  %revision_pointer.i81 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i82 = icmp eq ptr %revision_pointer.i81, null
  br i1 %has_revision.not.i82, label %slow.i97, label %check.i83

check.i83:                                        ; preds = %ast_block_9
  %revision.i84 = load atomic i64, ptr %revision_pointer.i81 acquire, align 8
  %stamp.i85 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i86 = icmp eq i64 %revision.i84, %stamp.i85
  %enabled.i87 = icmp ne i64 %revision.i84, 0
  %live.i88 = and i1 %enabled.i87, %same_revision.i86
  %begin.i89 = load i64, ptr @chali_address_view, align 16
  %size.i90 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i91 = sub i64 %r86, %begin.i89
  %inside.i92 = icmp ult i64 %offset.i91, %size.i90
  %remaining.i93 = sub i64 %size.i90, %offset.i91
  %fits.i94 = icmp ugt i64 %remaining.i93, 7
  %range.i95 = and i1 %inside.i92, %fits.i94
  %valid.i96 = and i1 %live.i88, %range.i95
  br i1 %valid.i96, label %fast.i100, label %slow.i97

fast.i100:                                        ; preds = %check.i83
  %base.i101 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i102 = getelementptr i8, ptr %base.i101, i64 %offset.i91
  br label %chali_primitive_address.exit103

slow.i97:                                         ; preds = %check.i83, %ast_block_9
  %resolved.i98 = tail call ptr @chali_host_address_view(i64 %r86, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit103

chali_primitive_address.exit103:                  ; preds = %fast.i100, %slow.i97
  %common.ret.op.i99 = phi ptr [ %physical.i102, %fast.i100 ], [ %resolved.i98, %slow.i97 ]
  %r89 = load i64, ptr %common.ret.op.i99, align 1
  store ptr @__chali_impl_program__setup, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 18, ptr @chali_source_line, align 8
  %r94 = tail call i64 @f_v475_std__lang____bootStringJoin3(i64 %r89, i64 %r6.i, i64 %r84)
  %r96.b = load i1, ptr @s_g_1___chali_global_reference, align 1
  %r96 = select i1 %r96.b, i64 104, i64 0
  %revision_pointer.i58 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i59 = icmp eq ptr %revision_pointer.i58, null
  br i1 %has_revision.not.i59, label %slow.i74, label %check.i60

check.i60:                                        ; preds = %chali_primitive_address.exit103
  %revision.i61 = load atomic i64, ptr %revision_pointer.i58 acquire, align 8
  %stamp.i62 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i63 = icmp eq i64 %revision.i61, %stamp.i62
  %enabled.i64 = icmp ne i64 %revision.i61, 0
  %live.i65 = and i1 %enabled.i64, %same_revision.i63
  %begin.i66 = load i64, ptr @chali_address_view, align 16
  %size.i67 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i68 = sub i64 %r96, %begin.i66
  %inside.i69 = icmp ult i64 %offset.i68, %size.i67
  %remaining.i70 = sub i64 %size.i67, %offset.i68
  %fits.i71 = icmp ugt i64 %remaining.i70, 7
  %range.i72 = and i1 %inside.i69, %fits.i71
  %valid.i73 = and i1 %live.i65, %range.i72
  br i1 %valid.i73, label %fast.i77, label %slow.i74

fast.i77:                                         ; preds = %check.i60
  %base.i78 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i79 = getelementptr i8, ptr %base.i78, i64 %offset.i68
  br label %chali_primitive_address.exit80

slow.i74:                                         ; preds = %check.i60, %chali_primitive_address.exit103
  %resolved.i75 = tail call ptr @chali_host_address_view(i64 %r96, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit80

chali_primitive_address.exit80:                   ; preds = %fast.i77, %slow.i74
  %common.ret.op.i76 = phi ptr [ %physical.i79, %fast.i77 ], [ %resolved.i75, %slow.i74 ]
  %r101 = load i64, ptr %common.ret.op.i76, align 1
  %r102.not = icmp eq i64 %r101, 0
  br i1 %r102.not, label %ast_block_17, label %ast_block_15

ast_block_15:                                     ; preds = %chali_primitive_address.exit80
  %revision_pointer.i35 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i36 = icmp eq ptr %revision_pointer.i35, null
  br i1 %has_revision.not.i36, label %slow.i51, label %check.i37

check.i37:                                        ; preds = %ast_block_15
  %revision.i38 = load atomic i64, ptr %revision_pointer.i35 acquire, align 8
  %stamp.i39 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i40 = icmp eq i64 %revision.i38, %stamp.i39
  %enabled.i41 = icmp ne i64 %revision.i38, 0
  %live.i42 = and i1 %enabled.i41, %same_revision.i40
  %begin.i43 = load i64, ptr @chali_address_view, align 16
  %size.i44 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i45 = sub i64 %r96, %begin.i43
  %inside.i46 = icmp ult i64 %offset.i45, %size.i44
  %remaining.i47 = sub i64 %size.i44, %offset.i45
  %fits.i48 = icmp ugt i64 %remaining.i47, 7
  %range.i49 = and i1 %inside.i46, %fits.i48
  %valid.i50 = and i1 %live.i42, %range.i49
  br i1 %valid.i50, label %fast.i54, label %slow.i51

fast.i54:                                         ; preds = %check.i37
  %base.i55 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i56 = getelementptr i8, ptr %base.i55, i64 %offset.i45
  br label %chali_primitive_address.exit57

slow.i51:                                         ; preds = %check.i37, %ast_block_15
  %resolved.i52 = tail call ptr @chali_host_address_view(i64 %r96, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit57

chali_primitive_address.exit57:                   ; preds = %fast.i54, %slow.i51
  %common.ret.op.i53 = phi ptr [ %physical.i56, %fast.i54 ], [ %resolved.i52, %slow.i51 ]
  %r108 = load i64, ptr %common.ret.op.i53, align 1
  tail call void @chali_host_drop(i64 noundef %r108) #27
  br label %ast_block_17

ast_block_17:                                     ; preds = %chali_primitive_address.exit80, %chali_primitive_address.exit57
  %revision_pointer.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i = icmp eq ptr %revision_pointer.i, null
  br i1 %has_revision.not.i, label %slow.i, label %check.i

check.i:                                          ; preds = %ast_block_17
  %revision.i = load atomic i64, ptr %revision_pointer.i acquire, align 8
  %stamp.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i = icmp eq i64 %revision.i, %stamp.i
  %enabled.i = icmp ne i64 %revision.i, 0
  %live.i = and i1 %enabled.i, %same_revision.i
  %begin.i = load i64, ptr @chali_address_view, align 16
  %size.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i = sub i64 %r96, %begin.i
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

slow.i:                                           ; preds = %check.i, %ast_block_17
  %resolved.i = tail call ptr @chali_host_address_view(i64 %r96, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit

chali_primitive_address.exit:                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  store i64 %r94, ptr %common.ret.op.i, align 1
  %r118.not = icmp eq i64 %r84, 0
  br i1 %r118.not, label %ast_block_20, label %ast_block_18

ast_block_18:                                     ; preds = %chali_primitive_address.exit
  tail call void @chali_host_drop(i64 noundef %r84) #27
  br label %ast_block_20

ast_block_20:                                     ; preds = %chali_primitive_address.exit, %ast_block_18
  %r127 = add nuw nsw i64 %s_v678___ast_234_index_1323.0183, 1
  store ptr @__chali_impl_program__setup, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 16, ptr @chali_source_line, align 8
  %exitcond.not = icmp eq i64 %r127, 256
  br i1 %exitcond.not, label %ast_label_v683___chali_brk_140, label %ast_block_9

ast_label_v683___chali_brk_140:                   ; preds = %ast_block_20
  %r130.not = icmp eq i64 %r6.i174, 0
  br i1 %r130.not, label %ast_block_23, label %ast_block_21

ast_block_21:                                     ; preds = %ast_label_v683___chali_brk_140
  tail call void @chali_host_drop(i64 noundef %r6.i174) #27
  br label %ast_block_23

ast_block_23:                                     ; preds = %ast_label_v683___chali_brk_140, %ast_block_21
  %r139.not = icmp eq i64 %r6.i, 0
  br i1 %r139.not, label %ast_block_26, label %ast_block_24

ast_block_24:                                     ; preds = %ast_block_23
  tail call void @chali_host_drop(i64 noundef %r6.i) #27
  br label %ast_block_26

ast_block_26:                                     ; preds = %ast_block_23, %ast_block_24
  ret i64 0
}

; Function Attrs: uwtable
define range(i64 0, 2147483648) i64 @__chali_impl_program__readWork(i64 %p0) #24 section "chali_code" {
entry:
  %indirect_arguments.i = alloca [2 x i64], align 8
  %native_argument_types.i = alloca [2 x i8], align 1
  %indirect_arguments = alloca [4 x i64], align 8
  %native_argument_types = alloca [4 x i8], align 1
  %raw_memory_output = alloca ptr, align 8
  %raw_memory_input = alloca ptr, align 8
  %r0.b = load i1, ptr @s_g_1___chali_global_reference, align 1
  %r0 = select i1 %r0.b, i64 104, i64 0
  br i1 %r0.b, label %chali_require_reference.exit52, label %0

0:                                                ; preds = %entry
  tail call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit52:                   ; preds = %entry
  %revision_pointer.i29 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i30 = icmp eq ptr %revision_pointer.i29, null
  br i1 %has_revision.not.i30, label %slow.i45, label %check.i31

check.i31:                                        ; preds = %chali_require_reference.exit52
  %revision.i32 = load atomic i64, ptr %revision_pointer.i29 acquire, align 8
  %stamp.i33 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i34 = icmp eq i64 %revision.i32, %stamp.i33
  %enabled.i35 = icmp ne i64 %revision.i32, 0
  %live.i36 = and i1 %enabled.i35, %same_revision.i34
  %begin.i37 = load i64, ptr @chali_address_view, align 16
  %size.i38 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i39 = sub i64 104, %begin.i37
  %inside.i40 = icmp ult i64 %offset.i39, %size.i38
  %remaining.i41 = sub i64 %size.i38, %offset.i39
  %fits.i42 = icmp ugt i64 %remaining.i41, 7
  %range.i43 = and i1 %inside.i40, %fits.i42
  %valid.i44 = and i1 %live.i36, %range.i43
  br i1 %valid.i44, label %fast.i48, label %slow.i45

fast.i48:                                         ; preds = %check.i31
  %base.i49 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i50 = getelementptr i8, ptr %base.i49, i64 %offset.i39
  br label %chali_primitive_address.exit51

slow.i45:                                         ; preds = %check.i31, %chali_require_reference.exit52
  %resolved.i46 = tail call ptr @chali_host_address_view(i64 104, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit51

chali_primitive_address.exit51:                   ; preds = %fast.i48, %slow.i45
  %common.ret.op.i47 = phi ptr [ %physical.i50, %fast.i48 ], [ %resolved.i46, %slow.i45 ]
  %r7 = load i64, ptr %common.ret.op.i47, align 1
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i.i.i = tail call i64 @chali_host_string_storage(i64 0, i64 %r7)
  %1 = icmp eq i64 %r2.i.i.i, 0
  %r37 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  %r38 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  %r79 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 16
  %r80 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 2
  br i1 %1, label %ast_block_0.us, label %ast_block_0

ast_block_0.us:                                   ; preds = %chali_primitive_address.exit51
  store ptr @__chali_impl_program__readWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 29, ptr @chali_source_line, align 8
  tail call void @chali_panic(ptr noundef nonnull @.str.3)
  unreachable

ast_block_0:                                      ; preds = %chali_primitive_address.exit51, %ast_block_8
  %s_v690___ast_235_index_1329.056 = phi i64 [ %r98, %ast_block_8 ], [ 0, %chali_primitive_address.exit51 ]
  %s_v688___ast_235_acc.055 = phi i64 [ %r88, %ast_block_8 ], [ %p0, %chali_primitive_address.exit51 ]
  store ptr @__chali_impl_program__readWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 29, ptr @chali_source_line, align 8
  %r19 = mul nuw nsw i64 %s_v690___ast_235_index_1329.056, 13
  %r22 = lshr i64 %s_v688___ast_235_acc.055, 7
  %r23 = and i64 %r22, 65535
  %r24 = add nuw nsw i64 %r19, %r23
  %2 = srem i64 %r24, %r2.i.i.i
  %3 = call i64 @chali_host_allocate(i64 noundef 16) #27
  call void @chali_host_memory_prepare(i64 8, i64 %3, i64 0, i64 16, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r31 = load ptr, ptr %raw_memory_output, align 8
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 1 dereferenceable(16) %r31, i8 0, i64 16, i1 false)
  store i64 %3, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 16, ptr %r37, align 8
  store i8 0, ptr %r38, align 1
  %r39 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r40 = call i8 @chali_host_result_float()
  store ptr @__chali_impl_program__readWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 30, ptr @chali_source_line, align 8
  %r43 = mul i64 %s_v688___ast_235_acc.055, 33
  %revision_pointer.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i = icmp eq ptr %revision_pointer.i, null
  br i1 %has_revision.not.i, label %slow.i, label %check.i

check.i:                                          ; preds = %ast_block_0
  %revision.i = load atomic i64, ptr %revision_pointer.i acquire, align 8
  %stamp.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i = icmp eq i64 %revision.i, %stamp.i
  %enabled.i = icmp ne i64 %revision.i, 0
  %live.i = and i1 %enabled.i, %same_revision.i
  %begin.i = load i64, ptr @chali_address_view, align 16
  %size.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i = sub i64 %r0, %begin.i
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

slow.i:                                           ; preds = %check.i, %ast_block_0
  %resolved.i = call ptr @chali_host_address_view(i64 %r0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit

chali_primitive_address.exit:                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  %r48 = load i64, ptr %common.ret.op.i, align 1
  store ptr @__chali_impl_program__readWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 30, ptr @chali_source_line, align 8
  %r51 = call i64 @f_v92_std__lang__String_byteAt(i64 %r48, i64 %2)
  %r57 = call i64 @chali_host_copy_words(i64 %3, i64 %r51, i64 2)
  store i64 %3, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 16, ptr %r37, align 8
  store i8 0, ptr %r38, align 1
  %r64 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r65 = call i8 @chali_host_result_float()
  %r70.not = icmp eq i64 %3, %r51
  br i1 %r70.not, label %ast_block_5, label %ast_block_3

ast_block_3:                                      ; preds = %chali_primitive_address.exit
  store i64 %r51, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 16, ptr %r37, align 8
  store i8 0, ptr %r38, align 1
  store i64 282, ptr %r79, align 8
  store i8 0, ptr %r80, align 1
  %r81 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r82 = call i8 @chali_host_result_float()
  br label %ast_block_5

ast_block_5:                                      ; preds = %chali_primitive_address.exit, %ast_block_3
  store ptr @__chali_impl_program__readWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 30, ptr @chali_source_line, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i)
  %revision_pointer.i5.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i6.i = icmp eq ptr %revision_pointer.i5.i, null
  br i1 %has_revision.not.i6.i, label %slow.i21.i, label %check.i7.i

check.i7.i:                                       ; preds = %ast_block_5
  %revision.i8.i = load atomic i64, ptr %revision_pointer.i5.i acquire, align 8
  %stamp.i9.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i10.i = icmp eq i64 %revision.i8.i, %stamp.i9.i
  %enabled.i11.i = icmp ne i64 %revision.i8.i, 0
  %live.i12.i = and i1 %enabled.i11.i, %same_revision.i10.i
  %begin.i13.i = load i64, ptr @chali_address_view, align 16
  %size.i14.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i15.i = sub i64 %3, %begin.i13.i
  %inside.i16.i = icmp ult i64 %offset.i15.i, %size.i14.i
  %remaining.i17.i = sub i64 %size.i14.i, %offset.i15.i
  %fits.i18.i = icmp ugt i64 %remaining.i17.i, 7
  %range.i19.i = and i1 %inside.i16.i, %fits.i18.i
  %valid.i20.i = and i1 %live.i12.i, %range.i19.i
  br i1 %valid.i20.i, label %fast.i24.i, label %slow.i21.i

fast.i24.i:                                       ; preds = %check.i7.i
  %base.i25.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i26.i = getelementptr i8, ptr %base.i25.i, i64 %offset.i15.i
  br label %chali_primitive_address.exit27.i

slow.i21.i:                                       ; preds = %check.i7.i, %ast_block_5
  %resolved.i22.i = call ptr @chali_host_address_view(i64 %3, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit27.i

chali_primitive_address.exit27.i:                 ; preds = %slow.i21.i, %fast.i24.i
  %common.ret.op.i23.i = phi ptr [ %physical.i26.i, %fast.i24.i ], [ %resolved.i22.i, %slow.i21.i ]
  %r7.i = load i64, ptr %common.ret.op.i23.i, align 1
  switch i64 %r7.i, label %f_v283_Option__uint8_unwrap.exit [
    i64 0, label %ast_block_0.i
    i64 1, label %ast_block_3.i
  ]

ast_block_0.i:                                    ; preds = %chali_primitive_address.exit27.i
  %r16.i = add i64 %3, 8
  store ptr @f_v283_Option__uint8_unwrap, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 810, ptr @chali_source_line, align 8
  %revision_pointer.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i = icmp eq ptr %revision_pointer.i.i, null
  br i1 %has_revision.not.i.i, label %slow.i.i, label %check.i.i

check.i.i:                                        ; preds = %ast_block_0.i
  %revision.i.i = load atomic i64, ptr %revision_pointer.i.i acquire, align 8
  %stamp.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i = icmp eq i64 %revision.i.i, %stamp.i.i
  %enabled.i.i = icmp ne i64 %revision.i.i, 0
  %live.i.i = and i1 %enabled.i.i, %same_revision.i.i
  %begin.i.i = load i64, ptr @chali_address_view, align 16
  %size.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i = sub i64 %r16.i, %begin.i.i
  %inside.i.i = icmp ult i64 %offset.i.i, %size.i.i
  %valid.i.i = and i1 %live.i.i, %inside.i.i
  br i1 %valid.i.i, label %fast.i.i, label %slow.i.i

fast.i.i:                                         ; preds = %check.i.i
  %base.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i = getelementptr i8, ptr %base.i.i, i64 %offset.i.i
  br label %chali_primitive_address.exit.i

slow.i.i:                                         ; preds = %check.i.i, %ast_block_0.i
  %resolved.i.i = call ptr @chali_host_address_view(i64 %r16.i, i64 1, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit.i

chali_primitive_address.exit.i:                   ; preds = %slow.i.i, %fast.i.i
  %common.ret.op.i.i = phi ptr [ %physical.i.i, %fast.i.i ], [ %resolved.i.i, %slow.i.i ]
  %r20.i = load i8, ptr %common.ret.op.i.i, align 1
  %r21.i = zext i8 %r20.i to i64
  br label %f_v283_Option__uint8_unwrap.exit

ast_block_3.i:                                    ; preds = %chali_primitive_address.exit27.i
  store ptr @f_v283_Option__uint8_unwrap, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 810, ptr @chali_source_line, align 8
  store i64 8, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  %r29.i = call i64 @chali_host_call(ptr nonnull @native_name3, i64 1, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r30.i = call i8 @chali_host_result_float()
  br label %f_v283_Option__uint8_unwrap.exit

f_v283_Option__uint8_unwrap.exit:                 ; preds = %chali_primitive_address.exit27.i, %chali_primitive_address.exit.i, %ast_block_3.i
  %s_v757___ast_283_result_1481.0.i = phi i64 [ %r21.i, %chali_primitive_address.exit.i ], [ 0, %ast_block_3.i ], [ 0, %chali_primitive_address.exit27.i ]
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i)
  %r87 = add i64 %s_v757___ast_283_result_1481.0.i, %r43
  store ptr @__chali_impl_program__readWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 30, ptr @chali_source_line, align 8
  %r88 = and i64 %r87, 2147483647
  %r91.not = icmp eq i64 %3, 0
  br i1 %r91.not, label %ast_block_8, label %ast_block_6

ast_block_6:                                      ; preds = %f_v283_Option__uint8_unwrap.exit
  call void @chali_host_drop(i64 noundef %3) #27
  br label %ast_block_8

ast_block_8:                                      ; preds = %f_v283_Option__uint8_unwrap.exit, %ast_block_6
  %r98 = add nuw nsw i64 %s_v690___ast_235_index_1329.056, 1
  store ptr @__chali_impl_program__readWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 28, ptr @chali_source_line, align 8
  %exitcond.not = icmp eq i64 %r98, 131072
  br i1 %exitcond.not, label %ast_label_v695___chali_brk_142, label %ast_block_0

ast_label_v695___chali_brk_142:                   ; preds = %ast_block_8
  ret i64 %r88
}

; Function Attrs: uwtable
define i64 @__chali_impl_program__concatWork(i64 %p0) #24 section "chali_code" {
entry:
  %indirect_arguments.i = alloca [2 x i64], align 8
  %native_argument_types.i = alloca [2 x i8], align 1
  %indirect_arguments = alloca [4 x i64], align 8
  %native_argument_types = alloca [4 x i8], align 1
  %raw_memory_output = alloca ptr, align 8
  %raw_memory_input = alloca ptr, align 8
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i.i = tail call i64 @chali_host_string_storage(i64 0, i64 112), !inline_history !33
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 178, ptr @chali_source_line, align 8
  %r6.i = tail call i64 @f_v57_std__lang____bootStringBuffer(i64 %r2.i.i), !inline_history !33
  %r10.i = add i64 %r6.i, 8
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 179, ptr @chali_source_line, align 8
  %r13.i = tail call i64 @chali_host_string_storage(i64 1, i64 112), !inline_history !33
  store ptr @f_v60_std__lang____bootStringCopy, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 179, ptr @chali_source_line, align 8
  %r16.i362 = tail call i64 @f_v58_std__lang____bootStringCopyBytes(i64 %r10.i, i64 %r13.i, i64 %r2.i.i), !inline_history !33
  %0 = tail call i64 @chali_host_allocate(i64 noundef 8) #27
  %revision_pointer.i339 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i340 = icmp eq ptr %revision_pointer.i339, null
  br i1 %has_revision.not.i340, label %slow.i355, label %check.i341

check.i341:                                       ; preds = %entry
  %revision.i342 = load atomic i64, ptr %revision_pointer.i339 acquire, align 8
  %stamp.i343 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i344 = icmp eq i64 %revision.i342, %stamp.i343
  %enabled.i345 = icmp ne i64 %revision.i342, 0
  %live.i346 = and i1 %enabled.i345, %same_revision.i344
  %begin.i347 = load i64, ptr @chali_address_view, align 16
  %size.i348 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i349 = sub i64 %0, %begin.i347
  %inside.i350 = icmp ult i64 %offset.i349, %size.i348
  %remaining.i351 = sub i64 %size.i348, %offset.i349
  %fits.i352 = icmp ugt i64 %remaining.i351, 7
  %range.i353 = and i1 %inside.i350, %fits.i352
  %valid.i354 = and i1 %live.i346, %range.i353
  br i1 %valid.i354, label %fast.i358, label %slow.i355

fast.i358:                                        ; preds = %check.i341
  %base.i359 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i360 = getelementptr i8, ptr %base.i359, i64 %offset.i349
  br label %chali_primitive_address.exit361

slow.i355:                                        ; preds = %check.i341, %entry
  %resolved.i356 = tail call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit361

chali_primitive_address.exit361:                  ; preds = %fast.i358, %slow.i355
  %common.ret.op.i357 = phi ptr [ %physical.i360, %fast.i358 ], [ %resolved.i356, %slow.i355 ]
  store i64 %r6.i, ptr %common.ret.op.i357, align 1
  br label %ast_block_0

ast_label_v710___chali_loop_145.preheader:        ; preds = %ast_block_5
  %r103 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  %r104 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  %r144 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 16
  %r145 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 2
  br label %ast_label_v710___chali_loop_145

ast_block_0:                                      ; preds = %chali_primitive_address.exit361, %ast_block_5
  %s_v705___ast_236_index_1336.0363 = phi i64 [ 0, %chali_primitive_address.exit361 ], [ %r80, %ast_block_5 ]
  %r17 = add i64 %s_v705___ast_236_index_1336.0363, %p0
  %r18 = and i64 %r17, 1
  %r19 = icmp eq i64 %r18, 0
  store ptr @__chali_impl_program__concatWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  br i1 %r19, label %ast_block_3, label %ast_block_4

ast_block_3:                                      ; preds = %ast_block_0
  store i64 41, ptr @chali_source_line, align 8
  %revision_pointer.i316 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i317 = icmp eq ptr %revision_pointer.i316, null
  br i1 %has_revision.not.i317, label %slow.i332, label %check.i318

check.i318:                                       ; preds = %ast_block_3
  %revision.i319 = load atomic i64, ptr %revision_pointer.i316 acquire, align 8
  %stamp.i320 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i321 = icmp eq i64 %revision.i319, %stamp.i320
  %enabled.i322 = icmp ne i64 %revision.i319, 0
  %live.i323 = and i1 %enabled.i322, %same_revision.i321
  %begin.i324 = load i64, ptr @chali_address_view, align 16
  %size.i325 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i326 = sub i64 %0, %begin.i324
  %inside.i327 = icmp ult i64 %offset.i326, %size.i325
  %remaining.i328 = sub i64 %size.i325, %offset.i326
  %fits.i329 = icmp ugt i64 %remaining.i328, 7
  %range.i330 = and i1 %inside.i327, %fits.i329
  %valid.i331 = and i1 %live.i323, %range.i330
  br i1 %valid.i331, label %fast.i335, label %slow.i332

fast.i335:                                        ; preds = %check.i318
  %base.i336 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i337 = getelementptr i8, ptr %base.i336, i64 %offset.i326
  br label %chali_primitive_address.exit338

slow.i332:                                        ; preds = %check.i318, %ast_block_3
  %resolved.i333 = tail call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit338

chali_primitive_address.exit338:                  ; preds = %fast.i335, %slow.i332
  %common.ret.op.i334 = phi ptr [ %physical.i337, %fast.i335 ], [ %resolved.i333, %slow.i332 ]
  %r25 = load i64, ptr %common.ret.op.i334, align 1
  %r26 = load i64, ptr @s_g_2_program__firstChunk, align 8
  store ptr @__chali_impl_program__concatWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 41, ptr @chali_source_line, align 8
  %r28 = tail call i64 @f_v474_std__lang____bootStringJoin2(i64 %r25, i64 %r26)
  %revision_pointer.i293 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i294 = icmp eq ptr %revision_pointer.i293, null
  br i1 %has_revision.not.i294, label %slow.i309, label %check.i295

check.i295:                                       ; preds = %chali_primitive_address.exit338
  %revision.i296 = load atomic i64, ptr %revision_pointer.i293 acquire, align 8
  %stamp.i297 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i298 = icmp eq i64 %revision.i296, %stamp.i297
  %enabled.i299 = icmp ne i64 %revision.i296, 0
  %live.i300 = and i1 %enabled.i299, %same_revision.i298
  %begin.i301 = load i64, ptr @chali_address_view, align 16
  %size.i302 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i303 = sub i64 %0, %begin.i301
  %inside.i304 = icmp ult i64 %offset.i303, %size.i302
  %remaining.i305 = sub i64 %size.i302, %offset.i303
  %fits.i306 = icmp ugt i64 %remaining.i305, 7
  %range.i307 = and i1 %inside.i304, %fits.i306
  %valid.i308 = and i1 %live.i300, %range.i307
  br i1 %valid.i308, label %fast.i312, label %slow.i309

fast.i312:                                        ; preds = %check.i295
  %base.i313 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i314 = getelementptr i8, ptr %base.i313, i64 %offset.i303
  br label %chali_primitive_address.exit315

slow.i309:                                        ; preds = %check.i295, %chali_primitive_address.exit338
  %resolved.i310 = tail call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit315

chali_primitive_address.exit315:                  ; preds = %fast.i312, %slow.i309
  %common.ret.op.i311 = phi ptr [ %physical.i314, %fast.i312 ], [ %resolved.i310, %slow.i309 ]
  %r35 = load i64, ptr %common.ret.op.i311, align 1
  %r36.not = icmp eq i64 %r35, 0
  br i1 %r36.not, label %ast_block_8, label %ast_block_6

ast_block_6:                                      ; preds = %chali_primitive_address.exit315
  %revision_pointer.i270 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i271 = icmp eq ptr %revision_pointer.i270, null
  br i1 %has_revision.not.i271, label %slow.i286, label %check.i272

check.i272:                                       ; preds = %ast_block_6
  %revision.i273 = load atomic i64, ptr %revision_pointer.i270 acquire, align 8
  %stamp.i274 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i275 = icmp eq i64 %revision.i273, %stamp.i274
  %enabled.i276 = icmp ne i64 %revision.i273, 0
  %live.i277 = and i1 %enabled.i276, %same_revision.i275
  %begin.i278 = load i64, ptr @chali_address_view, align 16
  %size.i279 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i280 = sub i64 %0, %begin.i278
  %inside.i281 = icmp ult i64 %offset.i280, %size.i279
  %remaining.i282 = sub i64 %size.i279, %offset.i280
  %fits.i283 = icmp ugt i64 %remaining.i282, 7
  %range.i284 = and i1 %inside.i281, %fits.i283
  %valid.i285 = and i1 %live.i277, %range.i284
  br i1 %valid.i285, label %fast.i289, label %slow.i286

fast.i289:                                        ; preds = %check.i272
  %base.i290 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i291 = getelementptr i8, ptr %base.i290, i64 %offset.i280
  br label %chali_primitive_address.exit292

slow.i286:                                        ; preds = %check.i272, %ast_block_6
  %resolved.i287 = tail call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit292

chali_primitive_address.exit292:                  ; preds = %fast.i289, %slow.i286
  %common.ret.op.i288 = phi ptr [ %physical.i291, %fast.i289 ], [ %resolved.i287, %slow.i286 ]
  %r42 = load i64, ptr %common.ret.op.i288, align 1
  tail call void @chali_host_drop(i64 noundef %r42) #27
  br label %ast_block_8

ast_block_8:                                      ; preds = %chali_primitive_address.exit315, %chali_primitive_address.exit292
  %revision_pointer.i247 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i248 = icmp eq ptr %revision_pointer.i247, null
  br i1 %has_revision.not.i248, label %slow.i263, label %check.i249

check.i249:                                       ; preds = %ast_block_8
  %revision.i250 = load atomic i64, ptr %revision_pointer.i247 acquire, align 8
  %stamp.i251 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i252 = icmp eq i64 %revision.i250, %stamp.i251
  %enabled.i253 = icmp ne i64 %revision.i250, 0
  %live.i254 = and i1 %enabled.i253, %same_revision.i252
  %begin.i255 = load i64, ptr @chali_address_view, align 16
  %size.i256 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i257 = sub i64 %0, %begin.i255
  %inside.i258 = icmp ult i64 %offset.i257, %size.i256
  %remaining.i259 = sub i64 %size.i256, %offset.i257
  %fits.i260 = icmp ugt i64 %remaining.i259, 7
  %range.i261 = and i1 %inside.i258, %fits.i260
  %valid.i262 = and i1 %live.i254, %range.i261
  br i1 %valid.i262, label %fast.i266, label %slow.i263

fast.i266:                                        ; preds = %check.i249
  %base.i267 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i268 = getelementptr i8, ptr %base.i267, i64 %offset.i257
  br label %chali_primitive_address.exit269

slow.i263:                                        ; preds = %check.i249, %ast_block_8
  %resolved.i264 = tail call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit269

chali_primitive_address.exit269:                  ; preds = %fast.i266, %slow.i263
  %common.ret.op.i265 = phi ptr [ %physical.i268, %fast.i266 ], [ %resolved.i264, %slow.i263 ]
  store i64 %r28, ptr %common.ret.op.i265, align 1
  br label %ast_block_5

ast_block_4:                                      ; preds = %ast_block_0
  store i64 42, ptr @chali_source_line, align 8
  %revision_pointer.i224 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i225 = icmp eq ptr %revision_pointer.i224, null
  br i1 %has_revision.not.i225, label %slow.i240, label %check.i226

check.i226:                                       ; preds = %ast_block_4
  %revision.i227 = load atomic i64, ptr %revision_pointer.i224 acquire, align 8
  %stamp.i228 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i229 = icmp eq i64 %revision.i227, %stamp.i228
  %enabled.i230 = icmp ne i64 %revision.i227, 0
  %live.i231 = and i1 %enabled.i230, %same_revision.i229
  %begin.i232 = load i64, ptr @chali_address_view, align 16
  %size.i233 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i234 = sub i64 %0, %begin.i232
  %inside.i235 = icmp ult i64 %offset.i234, %size.i233
  %remaining.i236 = sub i64 %size.i233, %offset.i234
  %fits.i237 = icmp ugt i64 %remaining.i236, 7
  %range.i238 = and i1 %inside.i235, %fits.i237
  %valid.i239 = and i1 %live.i231, %range.i238
  br i1 %valid.i239, label %fast.i243, label %slow.i240

fast.i243:                                        ; preds = %check.i226
  %base.i244 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i245 = getelementptr i8, ptr %base.i244, i64 %offset.i234
  br label %chali_primitive_address.exit246

slow.i240:                                        ; preds = %check.i226, %ast_block_4
  %resolved.i241 = tail call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit246

chali_primitive_address.exit246:                  ; preds = %fast.i243, %slow.i240
  %common.ret.op.i242 = phi ptr [ %physical.i245, %fast.i243 ], [ %resolved.i241, %slow.i240 ]
  %r53 = load i64, ptr %common.ret.op.i242, align 1
  %r54 = load i64, ptr @s_g_3_program__secondChunk, align 8
  store ptr @__chali_impl_program__concatWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 42, ptr @chali_source_line, align 8
  %r56 = tail call i64 @f_v474_std__lang____bootStringJoin2(i64 %r53, i64 %r54)
  %revision_pointer.i201 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i202 = icmp eq ptr %revision_pointer.i201, null
  br i1 %has_revision.not.i202, label %slow.i217, label %check.i203

check.i203:                                       ; preds = %chali_primitive_address.exit246
  %revision.i204 = load atomic i64, ptr %revision_pointer.i201 acquire, align 8
  %stamp.i205 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i206 = icmp eq i64 %revision.i204, %stamp.i205
  %enabled.i207 = icmp ne i64 %revision.i204, 0
  %live.i208 = and i1 %enabled.i207, %same_revision.i206
  %begin.i209 = load i64, ptr @chali_address_view, align 16
  %size.i210 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i211 = sub i64 %0, %begin.i209
  %inside.i212 = icmp ult i64 %offset.i211, %size.i210
  %remaining.i213 = sub i64 %size.i210, %offset.i211
  %fits.i214 = icmp ugt i64 %remaining.i213, 7
  %range.i215 = and i1 %inside.i212, %fits.i214
  %valid.i216 = and i1 %live.i208, %range.i215
  br i1 %valid.i216, label %fast.i220, label %slow.i217

fast.i220:                                        ; preds = %check.i203
  %base.i221 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i222 = getelementptr i8, ptr %base.i221, i64 %offset.i211
  br label %chali_primitive_address.exit223

slow.i217:                                        ; preds = %check.i203, %chali_primitive_address.exit246
  %resolved.i218 = tail call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit223

chali_primitive_address.exit223:                  ; preds = %fast.i220, %slow.i217
  %common.ret.op.i219 = phi ptr [ %physical.i222, %fast.i220 ], [ %resolved.i218, %slow.i217 ]
  %r63 = load i64, ptr %common.ret.op.i219, align 1
  %r64.not = icmp eq i64 %r63, 0
  br i1 %r64.not, label %ast_block_11, label %ast_block_9

ast_block_9:                                      ; preds = %chali_primitive_address.exit223
  %revision_pointer.i178 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i179 = icmp eq ptr %revision_pointer.i178, null
  br i1 %has_revision.not.i179, label %slow.i194, label %check.i180

check.i180:                                       ; preds = %ast_block_9
  %revision.i181 = load atomic i64, ptr %revision_pointer.i178 acquire, align 8
  %stamp.i182 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i183 = icmp eq i64 %revision.i181, %stamp.i182
  %enabled.i184 = icmp ne i64 %revision.i181, 0
  %live.i185 = and i1 %enabled.i184, %same_revision.i183
  %begin.i186 = load i64, ptr @chali_address_view, align 16
  %size.i187 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i188 = sub i64 %0, %begin.i186
  %inside.i189 = icmp ult i64 %offset.i188, %size.i187
  %remaining.i190 = sub i64 %size.i187, %offset.i188
  %fits.i191 = icmp ugt i64 %remaining.i190, 7
  %range.i192 = and i1 %inside.i189, %fits.i191
  %valid.i193 = and i1 %live.i185, %range.i192
  br i1 %valid.i193, label %fast.i197, label %slow.i194

fast.i197:                                        ; preds = %check.i180
  %base.i198 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i199 = getelementptr i8, ptr %base.i198, i64 %offset.i188
  br label %chali_primitive_address.exit200

slow.i194:                                        ; preds = %check.i180, %ast_block_9
  %resolved.i195 = tail call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit200

chali_primitive_address.exit200:                  ; preds = %fast.i197, %slow.i194
  %common.ret.op.i196 = phi ptr [ %physical.i199, %fast.i197 ], [ %resolved.i195, %slow.i194 ]
  %r70 = load i64, ptr %common.ret.op.i196, align 1
  tail call void @chali_host_drop(i64 noundef %r70) #27
  br label %ast_block_11

ast_block_11:                                     ; preds = %chali_primitive_address.exit223, %chali_primitive_address.exit200
  %revision_pointer.i155 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i156 = icmp eq ptr %revision_pointer.i155, null
  br i1 %has_revision.not.i156, label %slow.i171, label %check.i157

check.i157:                                       ; preds = %ast_block_11
  %revision.i158 = load atomic i64, ptr %revision_pointer.i155 acquire, align 8
  %stamp.i159 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i160 = icmp eq i64 %revision.i158, %stamp.i159
  %enabled.i161 = icmp ne i64 %revision.i158, 0
  %live.i162 = and i1 %enabled.i161, %same_revision.i160
  %begin.i163 = load i64, ptr @chali_address_view, align 16
  %size.i164 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i165 = sub i64 %0, %begin.i163
  %inside.i166 = icmp ult i64 %offset.i165, %size.i164
  %remaining.i167 = sub i64 %size.i164, %offset.i165
  %fits.i168 = icmp ugt i64 %remaining.i167, 7
  %range.i169 = and i1 %inside.i166, %fits.i168
  %valid.i170 = and i1 %live.i162, %range.i169
  br i1 %valid.i170, label %fast.i174, label %slow.i171

fast.i174:                                        ; preds = %check.i157
  %base.i175 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i176 = getelementptr i8, ptr %base.i175, i64 %offset.i165
  br label %chali_primitive_address.exit177

slow.i171:                                        ; preds = %check.i157, %ast_block_11
  %resolved.i172 = tail call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit177

chali_primitive_address.exit177:                  ; preds = %fast.i174, %slow.i171
  %common.ret.op.i173 = phi ptr [ %physical.i176, %fast.i174 ], [ %resolved.i172, %slow.i171 ]
  store i64 %r56, ptr %common.ret.op.i173, align 1
  br label %ast_block_5

ast_block_5:                                      ; preds = %chali_primitive_address.exit177, %chali_primitive_address.exit269
  %r80 = add nuw nsw i64 %s_v705___ast_236_index_1336.0363, 1
  store ptr @__chali_impl_program__concatWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 40, ptr @chali_source_line, align 8
  %exitcond.not = icmp eq i64 %r80, 2048
  br i1 %exitcond.not, label %ast_label_v710___chali_loop_145.preheader, label %ast_block_0

ast_label_v710___chali_loop_145:                  ; preds = %ast_label_v710___chali_loop_145.preheader, %ast_block_20
  %s_v704___ast_236_acc_1341.0 = phi i64 [ %r153, %ast_block_20 ], [ %p0, %ast_label_v710___chali_loop_145.preheader ]
  %s_v705___ast_236_index_1336.1 = phi i64 [ %r163, %ast_block_20 ], [ 0, %ast_label_v710___chali_loop_145.preheader ]
  store ptr @__chali_impl_program__concatWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 46, ptr @chali_source_line, align 8
  %revision_pointer.i132 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i133 = icmp eq ptr %revision_pointer.i132, null
  br i1 %has_revision.not.i133, label %slow.i148, label %check.i134

check.i134:                                       ; preds = %ast_label_v710___chali_loop_145
  %revision.i135 = load atomic i64, ptr %revision_pointer.i132 acquire, align 8
  %stamp.i136 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i137 = icmp eq i64 %revision.i135, %stamp.i136
  %enabled.i138 = icmp ne i64 %revision.i135, 0
  %live.i139 = and i1 %enabled.i138, %same_revision.i137
  %begin.i140 = load i64, ptr @chali_address_view, align 16
  %size.i141 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i142 = sub i64 %0, %begin.i140
  %inside.i143 = icmp ult i64 %offset.i142, %size.i141
  %remaining.i144 = sub i64 %size.i141, %offset.i142
  %fits.i145 = icmp ugt i64 %remaining.i144, 7
  %range.i146 = and i1 %inside.i143, %fits.i145
  %valid.i147 = and i1 %live.i139, %range.i146
  br i1 %valid.i147, label %fast.i151, label %slow.i148

fast.i151:                                        ; preds = %check.i134
  %base.i152 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i153 = getelementptr i8, ptr %base.i152, i64 %offset.i142
  br label %chali_primitive_address.exit154

slow.i148:                                        ; preds = %check.i134, %ast_label_v710___chali_loop_145
  %resolved.i149 = call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit154

chali_primitive_address.exit154:                  ; preds = %fast.i151, %slow.i148
  %common.ret.op.i150 = phi ptr [ %physical.i153, %fast.i151 ], [ %resolved.i149, %slow.i148 ]
  %r88 = load i64, ptr %common.ret.op.i150, align 1
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i.i.i = call i64 @chali_host_string_storage(i64 0, i64 %r88)
  store ptr @__chali_impl_program__concatWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 46, ptr @chali_source_line, align 8
  %r91 = icmp slt i64 %s_v705___ast_236_index_1336.1, %r2.i.i.i
  br i1 %r91, label %ast_block_12, label %ast_label_v711___chali_brk_146

ast_block_12:                                     ; preds = %chali_primitive_address.exit154
  %1 = call i64 @chali_host_allocate(i64 noundef 16) #27
  call void @chali_host_memory_prepare(i64 8, i64 %1, i64 0, i64 16, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r97 = load ptr, ptr %raw_memory_output, align 8
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 1 dereferenceable(16) %r97, i8 0, i64 16, i1 false)
  store i64 %1, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 16, ptr %r103, align 8
  store i8 0, ptr %r104, align 1
  %r105 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r106 = call i8 @chali_host_result_float()
  %r109 = mul i64 %s_v704___ast_236_acc_1341.0, 33
  store ptr @__chali_impl_program__concatWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 47, ptr @chali_source_line, align 8
  %revision_pointer.i109 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i110 = icmp eq ptr %revision_pointer.i109, null
  br i1 %has_revision.not.i110, label %slow.i125, label %check.i111

check.i111:                                       ; preds = %ast_block_12
  %revision.i112 = load atomic i64, ptr %revision_pointer.i109 acquire, align 8
  %stamp.i113 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i114 = icmp eq i64 %revision.i112, %stamp.i113
  %enabled.i115 = icmp ne i64 %revision.i112, 0
  %live.i116 = and i1 %enabled.i115, %same_revision.i114
  %begin.i117 = load i64, ptr @chali_address_view, align 16
  %size.i118 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i119 = sub i64 %0, %begin.i117
  %inside.i120 = icmp ult i64 %offset.i119, %size.i118
  %remaining.i121 = sub i64 %size.i118, %offset.i119
  %fits.i122 = icmp ugt i64 %remaining.i121, 7
  %range.i123 = and i1 %inside.i120, %fits.i122
  %valid.i124 = and i1 %live.i116, %range.i123
  br i1 %valid.i124, label %fast.i128, label %slow.i125

fast.i128:                                        ; preds = %check.i111
  %base.i129 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i130 = getelementptr i8, ptr %base.i129, i64 %offset.i119
  br label %chali_primitive_address.exit131

slow.i125:                                        ; preds = %check.i111, %ast_block_12
  %resolved.i126 = call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit131

chali_primitive_address.exit131:                  ; preds = %fast.i128, %slow.i125
  %common.ret.op.i127 = phi ptr [ %physical.i130, %fast.i128 ], [ %resolved.i126, %slow.i125 ]
  %r113 = load i64, ptr %common.ret.op.i127, align 1
  store ptr @__chali_impl_program__concatWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 47, ptr @chali_source_line, align 8
  %r116 = call i64 @f_v92_std__lang__String_byteAt(i64 %r113, i64 %s_v705___ast_236_index_1336.1)
  %r122 = call i64 @chali_host_copy_words(i64 %1, i64 %r116, i64 2)
  store i64 %1, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 16, ptr %r103, align 8
  store i8 0, ptr %r104, align 1
  %r129 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r130 = call i8 @chali_host_result_float()
  %r135.not = icmp eq i64 %1, %r116
  br i1 %r135.not, label %ast_block_17, label %ast_block_15

ast_block_15:                                     ; preds = %chali_primitive_address.exit131
  store i64 %r116, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 16, ptr %r103, align 8
  store i8 0, ptr %r104, align 1
  store i64 282, ptr %r144, align 8
  store i8 0, ptr %r145, align 1
  %r146 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r147 = call i8 @chali_host_result_float()
  br label %ast_block_17

ast_block_17:                                     ; preds = %chali_primitive_address.exit131, %ast_block_15
  store ptr @__chali_impl_program__concatWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 47, ptr @chali_source_line, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i)
  %revision_pointer.i5.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i6.i = icmp eq ptr %revision_pointer.i5.i, null
  br i1 %has_revision.not.i6.i, label %slow.i21.i, label %check.i7.i

check.i7.i:                                       ; preds = %ast_block_17
  %revision.i8.i = load atomic i64, ptr %revision_pointer.i5.i acquire, align 8
  %stamp.i9.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i10.i = icmp eq i64 %revision.i8.i, %stamp.i9.i
  %enabled.i11.i = icmp ne i64 %revision.i8.i, 0
  %live.i12.i = and i1 %enabled.i11.i, %same_revision.i10.i
  %begin.i13.i = load i64, ptr @chali_address_view, align 16
  %size.i14.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i15.i = sub i64 %1, %begin.i13.i
  %inside.i16.i = icmp ult i64 %offset.i15.i, %size.i14.i
  %remaining.i17.i = sub i64 %size.i14.i, %offset.i15.i
  %fits.i18.i = icmp ugt i64 %remaining.i17.i, 7
  %range.i19.i = and i1 %inside.i16.i, %fits.i18.i
  %valid.i20.i = and i1 %live.i12.i, %range.i19.i
  br i1 %valid.i20.i, label %fast.i24.i, label %slow.i21.i

fast.i24.i:                                       ; preds = %check.i7.i
  %base.i25.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i26.i = getelementptr i8, ptr %base.i25.i, i64 %offset.i15.i
  br label %chali_primitive_address.exit27.i

slow.i21.i:                                       ; preds = %check.i7.i, %ast_block_17
  %resolved.i22.i = call ptr @chali_host_address_view(i64 %1, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit27.i

chali_primitive_address.exit27.i:                 ; preds = %slow.i21.i, %fast.i24.i
  %common.ret.op.i23.i = phi ptr [ %physical.i26.i, %fast.i24.i ], [ %resolved.i22.i, %slow.i21.i ]
  %r7.i = load i64, ptr %common.ret.op.i23.i, align 1
  switch i64 %r7.i, label %f_v283_Option__uint8_unwrap.exit [
    i64 0, label %ast_block_0.i
    i64 1, label %ast_block_3.i
  ]

ast_block_0.i:                                    ; preds = %chali_primitive_address.exit27.i
  %r16.i = add i64 %1, 8
  store ptr @f_v283_Option__uint8_unwrap, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 810, ptr @chali_source_line, align 8
  %revision_pointer.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i = icmp eq ptr %revision_pointer.i.i, null
  br i1 %has_revision.not.i.i, label %slow.i.i, label %check.i.i

check.i.i:                                        ; preds = %ast_block_0.i
  %revision.i.i = load atomic i64, ptr %revision_pointer.i.i acquire, align 8
  %stamp.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i = icmp eq i64 %revision.i.i, %stamp.i.i
  %enabled.i.i = icmp ne i64 %revision.i.i, 0
  %live.i.i = and i1 %enabled.i.i, %same_revision.i.i
  %begin.i.i = load i64, ptr @chali_address_view, align 16
  %size.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i = sub i64 %r16.i, %begin.i.i
  %inside.i.i = icmp ult i64 %offset.i.i, %size.i.i
  %valid.i.i = and i1 %live.i.i, %inside.i.i
  br i1 %valid.i.i, label %fast.i.i, label %slow.i.i

fast.i.i:                                         ; preds = %check.i.i
  %base.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i = getelementptr i8, ptr %base.i.i, i64 %offset.i.i
  br label %chali_primitive_address.exit.i

slow.i.i:                                         ; preds = %check.i.i, %ast_block_0.i
  %resolved.i.i = call ptr @chali_host_address_view(i64 %r16.i, i64 1, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit.i

chali_primitive_address.exit.i:                   ; preds = %slow.i.i, %fast.i.i
  %common.ret.op.i.i = phi ptr [ %physical.i.i, %fast.i.i ], [ %resolved.i.i, %slow.i.i ]
  %r20.i = load i8, ptr %common.ret.op.i.i, align 1
  %r21.i = zext i8 %r20.i to i64
  br label %f_v283_Option__uint8_unwrap.exit

ast_block_3.i:                                    ; preds = %chali_primitive_address.exit27.i
  store ptr @f_v283_Option__uint8_unwrap, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 810, ptr @chali_source_line, align 8
  store i64 8, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  %r29.i = call i64 @chali_host_call(ptr nonnull @native_name3, i64 1, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r30.i = call i8 @chali_host_result_float()
  br label %f_v283_Option__uint8_unwrap.exit

f_v283_Option__uint8_unwrap.exit:                 ; preds = %chali_primitive_address.exit27.i, %chali_primitive_address.exit.i, %ast_block_3.i
  %s_v757___ast_283_result_1481.0.i = phi i64 [ %r21.i, %chali_primitive_address.exit.i ], [ 0, %ast_block_3.i ], [ 0, %chali_primitive_address.exit27.i ]
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i)
  %r152 = add i64 %s_v757___ast_283_result_1481.0.i, %r109
  store ptr @__chali_impl_program__concatWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 47, ptr @chali_source_line, align 8
  %r153 = and i64 %r152, 2147483647
  %r156.not = icmp eq i64 %1, 0
  br i1 %r156.not, label %ast_block_20, label %ast_block_18

ast_block_18:                                     ; preds = %f_v283_Option__uint8_unwrap.exit
  call void @chali_host_drop(i64 noundef %1) #27
  br label %ast_block_20

ast_block_20:                                     ; preds = %f_v283_Option__uint8_unwrap.exit, %ast_block_18
  store ptr @__chali_impl_program__concatWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 48, ptr @chali_source_line, align 8
  %r163 = add nuw nsw i64 %s_v705___ast_236_index_1336.1, 1
  br label %ast_label_v710___chali_loop_145

ast_label_v711___chali_brk_146:                   ; preds = %chali_primitive_address.exit154
  %revision_pointer.i86 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i87 = icmp eq ptr %revision_pointer.i86, null
  br i1 %has_revision.not.i87, label %slow.i102, label %check.i88

check.i88:                                        ; preds = %ast_label_v711___chali_brk_146
  %revision.i89 = load atomic i64, ptr %revision_pointer.i86 acquire, align 8
  %stamp.i90 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i91 = icmp eq i64 %revision.i89, %stamp.i90
  %enabled.i92 = icmp ne i64 %revision.i89, 0
  %live.i93 = and i1 %enabled.i92, %same_revision.i91
  %begin.i94 = load i64, ptr @chali_address_view, align 16
  %size.i95 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i96 = sub i64 %0, %begin.i94
  %inside.i97 = icmp ult i64 %offset.i96, %size.i95
  %remaining.i98 = sub i64 %size.i95, %offset.i96
  %fits.i99 = icmp ugt i64 %remaining.i98, 7
  %range.i100 = and i1 %inside.i97, %fits.i99
  %valid.i101 = and i1 %live.i93, %range.i100
  br i1 %valid.i101, label %fast.i105, label %slow.i102

fast.i105:                                        ; preds = %check.i88
  %base.i106 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i107 = getelementptr i8, ptr %base.i106, i64 %offset.i96
  br label %chali_primitive_address.exit108

slow.i102:                                        ; preds = %check.i88, %ast_label_v711___chali_brk_146
  %resolved.i103 = call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit108

chali_primitive_address.exit108:                  ; preds = %fast.i105, %slow.i102
  %common.ret.op.i104 = phi ptr [ %physical.i107, %fast.i105 ], [ %resolved.i103, %slow.i102 ]
  %r169 = load i64, ptr %common.ret.op.i104, align 1
  %r170.not = icmp eq i64 %r169, 0
  br i1 %r170.not, label %ast_block_23, label %ast_block_21

ast_block_21:                                     ; preds = %chali_primitive_address.exit108
  %revision_pointer.i63 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i64 = icmp eq ptr %revision_pointer.i63, null
  br i1 %has_revision.not.i64, label %slow.i79, label %check.i65

check.i65:                                        ; preds = %ast_block_21
  %revision.i66 = load atomic i64, ptr %revision_pointer.i63 acquire, align 8
  %stamp.i67 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i68 = icmp eq i64 %revision.i66, %stamp.i67
  %enabled.i69 = icmp ne i64 %revision.i66, 0
  %live.i70 = and i1 %enabled.i69, %same_revision.i68
  %begin.i71 = load i64, ptr @chali_address_view, align 16
  %size.i72 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i73 = sub i64 %0, %begin.i71
  %inside.i74 = icmp ult i64 %offset.i73, %size.i72
  %remaining.i75 = sub i64 %size.i72, %offset.i73
  %fits.i76 = icmp ugt i64 %remaining.i75, 7
  %range.i77 = and i1 %inside.i74, %fits.i76
  %valid.i78 = and i1 %live.i70, %range.i77
  br i1 %valid.i78, label %fast.i82, label %slow.i79

fast.i82:                                         ; preds = %check.i65
  %base.i83 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i84 = getelementptr i8, ptr %base.i83, i64 %offset.i73
  br label %chali_primitive_address.exit85

slow.i79:                                         ; preds = %check.i65, %ast_block_21
  %resolved.i80 = call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit85

chali_primitive_address.exit85:                   ; preds = %fast.i82, %slow.i79
  %common.ret.op.i81 = phi ptr [ %physical.i84, %fast.i82 ], [ %resolved.i80, %slow.i79 ]
  %r176 = load i64, ptr %common.ret.op.i81, align 1
  call void @chali_host_drop(i64 noundef %r176) #27
  br label %ast_block_23

ast_block_23:                                     ; preds = %chali_primitive_address.exit108, %chali_primitive_address.exit85
  %revision_pointer.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i = icmp eq ptr %revision_pointer.i, null
  br i1 %has_revision.not.i, label %slow.i, label %check.i

check.i:                                          ; preds = %ast_block_23
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

slow.i:                                           ; preds = %check.i, %ast_block_23
  %resolved.i = call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit

chali_primitive_address.exit:                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  store i64 0, ptr %common.ret.op.i, align 1
  %r184.not = icmp eq i64 %0, 0
  br i1 %r184.not, label %ast_block_26, label %ast_block_24

ast_block_24:                                     ; preds = %chali_primitive_address.exit
  call void @chali_host_drop(i64 noundef %0) #27
  br label %ast_block_26

ast_block_26:                                     ; preds = %chali_primitive_address.exit, %ast_block_24
  ret i64 %s_v704___ast_236_acc_1341.0
}

; Function Attrs: uwtable
define range(i64 0, 2147483648) i64 @__chali_impl_program__sliceWork(i64 %p0) #24 section "chali_code" {
entry:
  %indirect_arguments.i101 = alloca [2 x i64], align 8
  %native_argument_types.i102 = alloca [2 x i8], align 1
  %indirect_arguments.i = alloca [2 x i64], align 8
  %native_argument_types.i = alloca [2 x i8], align 1
  %indirect_arguments = alloca [4 x i64], align 8
  %native_argument_types = alloca [4 x i8], align 1
  %raw_memory_output = alloca ptr, align 8
  %raw_memory_input = alloca ptr, align 8
  %r0.b = load i1, ptr @s_g_1___chali_global_reference, align 1
  %r0 = select i1 %r0.b, i64 104, i64 0
  br i1 %r0.b, label %ast_label_v729___chali_loop_147.preheader, label %0

ast_label_v729___chali_loop_147.preheader:        ; preds = %entry
  %r94 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  %r95 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  %r144 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 16
  %r145 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 2
  br label %ast_block_0

0:                                                ; preds = %entry
  tail call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

ast_block_0:                                      ; preds = %ast_label_v729___chali_loop_147.preheader, %ast_block_26
  %s_v722___ast_237_index_1347.0173 = phi i64 [ 0, %ast_label_v729___chali_loop_147.preheader ], [ %r217, %ast_block_26 ]
  %s_v718___ast_237_acc_1346.0172 = phi i64 [ %p0, %ast_label_v729___chali_loop_147.preheader ], [ %r200, %ast_block_26 ]
  %r12 = mul nuw nsw i64 %s_v722___ast_237_index_1347.0173, 13
  %r16 = add i64 %r12, %s_v718___ast_237_acc_1346.0172
  %r17 = and i64 %r16, 127
  %r20 = load i64, ptr @s_g_4_program__unitWidth, align 8
  %r22 = mul i64 %r20, %r17
  %r27 = add i64 %s_v722___ast_237_index_1347.0173, %p0
  %r28 = and i64 %r27, 3
  %r29 = add nuw nsw i64 %r28, 1
  %r32 = mul i64 %r20, %r29
  %r37 = and i64 %s_v722___ast_237_index_1347.0173, 1
  store ptr @__chali_impl_program__sliceWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 62, ptr @chali_source_line, align 8
  %r38.not = icmp eq i64 %r37, 0
  br i1 %r38.not, label %chali_require_reference.exit159, label %ast_block_3

ast_block_3:                                      ; preds = %ast_block_0
  %r43 = add i64 %r16, 13
  %r44 = and i64 %r43, 127
  store ptr @__chali_impl_program__sliceWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 62, ptr @chali_source_line, align 8
  %r47 = mul i64 %r20, %r44
  br label %chali_require_reference.exit159

chali_require_reference.exit159:                  ; preds = %ast_block_3, %ast_block_0
  %s_v723___ast_237_other_1350.0 = phi i64 [ %r47, %ast_block_3 ], [ %r22, %ast_block_0 ]
  %revision_pointer.i77 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i78 = icmp eq ptr %revision_pointer.i77, null
  br i1 %has_revision.not.i78, label %slow.i93, label %check.i79

check.i79:                                        ; preds = %chali_require_reference.exit159
  %revision.i80 = load atomic i64, ptr %revision_pointer.i77 acquire, align 8
  %stamp.i81 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i82 = icmp eq i64 %revision.i80, %stamp.i81
  %enabled.i83 = icmp ne i64 %revision.i80, 0
  %live.i84 = and i1 %enabled.i83, %same_revision.i82
  %begin.i85 = load i64, ptr @chali_address_view, align 16
  %size.i86 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i87 = sub i64 %r0, %begin.i85
  %inside.i88 = icmp ult i64 %offset.i87, %size.i86
  %remaining.i89 = sub i64 %size.i86, %offset.i87
  %fits.i90 = icmp ugt i64 %remaining.i89, 7
  %range.i91 = and i1 %inside.i88, %fits.i90
  %valid.i92 = and i1 %live.i84, %range.i91
  br i1 %valid.i92, label %fast.i96, label %slow.i93

fast.i96:                                         ; preds = %check.i79
  %base.i97 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i98 = getelementptr i8, ptr %base.i97, i64 %offset.i87
  br label %chali_require_reference.exit160

slow.i93:                                         ; preds = %check.i79, %chali_require_reference.exit159
  %resolved.i94 = call ptr @chali_host_address_view(i64 %r0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_require_reference.exit160

chali_require_reference.exit160:                  ; preds = %slow.i93, %fast.i96
  %common.ret.op.i95 = phi ptr [ %physical.i98, %fast.i96 ], [ %resolved.i94, %slow.i93 ]
  %r52 = load i64, ptr %common.ret.op.i95, align 1
  %r59 = add i64 %r22, %r32
  store ptr @__chali_impl_program__sliceWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 63, ptr @chali_source_line, align 8
  %r60 = call i64 @f_v62_std__lang____bootStringSlice(i64 %r52, i64 %r22, i64 %r59)
  %revision_pointer.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i = icmp eq ptr %revision_pointer.i, null
  br i1 %has_revision.not.i, label %slow.i, label %check.i

check.i:                                          ; preds = %chali_require_reference.exit160
  %revision.i = load atomic i64, ptr %revision_pointer.i acquire, align 8
  %stamp.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i = icmp eq i64 %revision.i, %stamp.i
  %enabled.i = icmp ne i64 %revision.i, 0
  %live.i = and i1 %enabled.i, %same_revision.i
  %begin.i = load i64, ptr @chali_address_view, align 16
  %size.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i = sub i64 %r0, %begin.i
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

slow.i:                                           ; preds = %check.i, %chali_require_reference.exit160
  %resolved.i = call ptr @chali_host_address_view(i64 %r0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit

chali_primitive_address.exit:                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  %r66 = load i64, ptr %common.ret.op.i, align 1
  %r73 = add i64 %s_v723___ast_237_other_1350.0, %r32
  store ptr @__chali_impl_program__sliceWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 64, ptr @chali_source_line, align 8
  %r74 = call i64 @f_v62_std__lang____bootStringSlice(i64 %r66, i64 %s_v723___ast_237_other_1350.0, i64 %r73)
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i57.i = call i64 @chali_host_string_storage(i64 0, i64 %r60)
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i.i = call i64 @chali_host_string_storage(i64 0, i64 %r74)
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 199, ptr @chali_source_line, align 8
  %r10.i = call i64 @chali_host_string_storage(i64 1, i64 %r60)
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 199, ptr @chali_source_line, align 8
  %r13.i = call i64 @chali_host_string_storage(i64 1, i64 %r74)
  %r18.i170 = icmp sgt i64 %r2.i57.i, 0
  br i1 %r18.i170, label %ast_block_1.i.preheader, label %ast_label_v651___chali_brk_24.i

ast_block_1.i.preheader:                          ; preds = %chali_primitive_address.exit
  %smax = call i64 @llvm.smax.i64(i64 %r2.i.i, i64 0)
  br label %ast_block_1.i

ast_block_1.i:                                    ; preds = %ast_block_1.i.preheader, %ast_block_12.i
  %s_v645___ast_64_i_734.0.i171 = phi i64 [ %r64.i, %ast_block_12.i ], [ 0, %ast_block_1.i.preheader ]
  %exitcond.not = icmp eq i64 %s_v645___ast_64_i_734.0.i171, %smax
  br i1 %exitcond.not, label %ast_label_v651___chali_brk_24.i, label %ast_block_4.i

ast_block_4.i:                                    ; preds = %ast_block_1.i
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i29.i = add i64 %s_v645___ast_64_i_734.0.i171, %r10.i
  %1 = icmp eq i64 %r4.i29.i, 0
  br i1 %1, label %2, label %chali_require_reference.exit162

2:                                                ; preds = %ast_block_4.i
  call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit162:                  ; preds = %ast_block_4.i
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i32.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i33.i = icmp eq ptr %revision_pointer.i.i32.i, null
  br i1 %has_revision.not.i.i33.i, label %slow.i.i48.i, label %check.i.i34.i

check.i.i34.i:                                    ; preds = %chali_require_reference.exit162
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

slow.i.i48.i:                                     ; preds = %check.i.i34.i, %chali_require_reference.exit162
  %resolved.i.i49.i = call ptr @chali_host_address_view(i64 %r4.i29.i, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit56.i

f_v49_std__lang____bootStringByte.exit56.i:       ; preds = %slow.i.i48.i, %fast.i.i53.i
  %common.ret.op.i.i50.i = phi ptr [ %physical.i.i55.i, %fast.i.i53.i ], [ %resolved.i.i49.i, %slow.i.i48.i ]
  %r10.i51.i = load i8, ptr %common.ret.op.i.i50.i, align 1
  store ptr @f_v49_std__lang____bootStringByte, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 13, ptr @chali_source_line, align 8
  %r4.i.i = add i64 %s_v645___ast_64_i_734.0.i171, %r13.i
  %3 = icmp eq i64 %r4.i.i, 0
  br i1 %3, label %4, label %chali_require_reference.exit164

4:                                                ; preds = %f_v49_std__lang____bootStringByte.exit56.i
  call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit164:                  ; preds = %f_v49_std__lang____bootStringByte.exit56.i
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 14, ptr @chali_source_line, align 8
  %revision_pointer.i.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i.i = icmp eq ptr %revision_pointer.i.i.i, null
  br i1 %has_revision.not.i.i.i, label %slow.i.i.i, label %check.i.i.i

check.i.i.i:                                      ; preds = %chali_require_reference.exit164
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

slow.i.i.i:                                       ; preds = %check.i.i.i, %chali_require_reference.exit164
  %resolved.i.i.i = call ptr @chali_host_address_view(i64 %r4.i.i, i64 1, ptr nonnull @chali_address_view)
  br label %f_v49_std__lang____bootStringByte.exit.i

f_v49_std__lang____bootStringByte.exit.i:         ; preds = %slow.i.i.i, %fast.i.i.i
  %common.ret.op.i.i.i = phi ptr [ %physical.i.i.i, %fast.i.i.i ], [ %resolved.i.i.i, %slow.i.i.i ]
  %r10.i.i = load i8, ptr %common.ret.op.i.i.i, align 1
  %or.cond.not = icmp eq i8 %r10.i51.i, %r10.i.i
  br i1 %or.cond.not, label %ast_block_12.i, label %f_v64_std__lang____bootStringCompare.exit.thread

ast_block_12.i:                                   ; preds = %f_v49_std__lang____bootStringByte.exit.i
  %r64.i = add nuw nsw i64 %s_v645___ast_64_i_734.0.i171, 1
  store ptr @f_v64_std__lang____bootStringCompare, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 200, ptr @chali_source_line, align 8
  %exitcond174.not = icmp eq i64 %r64.i, %r2.i57.i
  br i1 %exitcond174.not, label %ast_label_v651___chali_brk_24.i, label %ast_block_1.i

ast_label_v651___chali_brk_24.i:                  ; preds = %ast_block_12.i, %ast_block_1.i, %chali_primitive_address.exit
  %r69.i = icmp slt i64 %r2.i57.i, %r2.i.i
  br i1 %r69.i, label %f_v64_std__lang____bootStringCompare.exit.thread, label %ast_block_15.i

ast_block_15.i:                                   ; preds = %ast_label_v651___chali_brk_24.i
  %r79.i = icmp sle i64 %r2.i57.i, %r2.i.i
  %spec.select = zext i1 %r79.i to i64
  br label %f_v64_std__lang____bootStringCompare.exit.thread

f_v64_std__lang____bootStringCompare.exit.thread: ; preds = %f_v49_std__lang____bootStringByte.exit.i, %ast_label_v651___chali_brk_24.i, %ast_block_15.i
  %5 = phi i64 [ %spec.select, %ast_block_15.i ], [ 0, %ast_label_v651___chali_brk_24.i ], [ 0, %f_v49_std__lang____bootStringByte.exit.i ]
  store ptr @__chali_impl_program__sliceWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 66, ptr @chali_source_line, align 8
  %6 = call i64 @chali_host_allocate(i64 noundef 16) #27
  call void @chali_host_memory_prepare(i64 8, i64 %6, i64 0, i64 16, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r88 = load ptr, ptr %raw_memory_output, align 8
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 1 dereferenceable(16) %r88, i8 0, i64 16, i1 false)
  store i64 %6, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 16, ptr %r94, align 8
  store i8 0, ptr %r95, align 1
  %r96 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r97 = call i8 @chali_host_result_float()
  %7 = call i64 @chali_host_allocate(i64 noundef 16) #27
  call void @chali_host_memory_prepare(i64 8, i64 %7, i64 0, i64 16, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r101 = load ptr, ptr %raw_memory_output, align 8
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 1 dereferenceable(16) %r101, i8 0, i64 16, i1 false)
  store i64 %7, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 16, ptr %r94, align 8
  store i8 0, ptr %r95, align 1
  %r109 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r110 = call i8 @chali_host_result_float()
  %r113 = mul i64 %s_v718___ast_237_acc_1346.0172, 33
  store ptr @__chali_impl_program__sliceWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 67, ptr @chali_source_line, align 8
  %r116 = call i64 @f_v92_std__lang__String_byteAt(i64 %r60, i64 0)
  %r122 = call i64 @chali_host_copy_words(i64 %7, i64 %r116, i64 2)
  store i64 %7, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 16, ptr %r94, align 8
  store i8 0, ptr %r95, align 1
  %r129 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r130 = call i8 @chali_host_result_float()
  %r135.not = icmp eq i64 %7, %r116
  br i1 %r135.not, label %ast_block_11, label %ast_block_9

ast_block_9:                                      ; preds = %f_v64_std__lang____bootStringCompare.exit.thread
  store i64 %r116, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 16, ptr %r94, align 8
  store i8 0, ptr %r95, align 1
  store i64 282, ptr %r144, align 8
  store i8 0, ptr %r145, align 1
  %r146 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r147 = call i8 @chali_host_result_float()
  br label %ast_block_11

ast_block_11:                                     ; preds = %f_v64_std__lang____bootStringCompare.exit.thread, %ast_block_9
  store ptr @__chali_impl_program__sliceWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 67, ptr @chali_source_line, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i)
  %revision_pointer.i5.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i6.i = icmp eq ptr %revision_pointer.i5.i, null
  br i1 %has_revision.not.i6.i, label %slow.i21.i, label %check.i7.i

check.i7.i:                                       ; preds = %ast_block_11
  %revision.i8.i = load atomic i64, ptr %revision_pointer.i5.i acquire, align 8
  %stamp.i9.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i10.i = icmp eq i64 %revision.i8.i, %stamp.i9.i
  %enabled.i11.i = icmp ne i64 %revision.i8.i, 0
  %live.i12.i = and i1 %enabled.i11.i, %same_revision.i10.i
  %begin.i13.i = load i64, ptr @chali_address_view, align 16
  %size.i14.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i15.i = sub i64 %7, %begin.i13.i
  %inside.i16.i = icmp ult i64 %offset.i15.i, %size.i14.i
  %remaining.i17.i = sub i64 %size.i14.i, %offset.i15.i
  %fits.i18.i = icmp ugt i64 %remaining.i17.i, 7
  %range.i19.i = and i1 %inside.i16.i, %fits.i18.i
  %valid.i20.i = and i1 %live.i12.i, %range.i19.i
  br i1 %valid.i20.i, label %fast.i24.i, label %slow.i21.i

fast.i24.i:                                       ; preds = %check.i7.i
  %base.i25.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i26.i = getelementptr i8, ptr %base.i25.i, i64 %offset.i15.i
  br label %chali_primitive_address.exit27.i

slow.i21.i:                                       ; preds = %check.i7.i, %ast_block_11
  %resolved.i22.i = call ptr @chali_host_address_view(i64 %7, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit27.i

chali_primitive_address.exit27.i:                 ; preds = %slow.i21.i, %fast.i24.i
  %common.ret.op.i23.i = phi ptr [ %physical.i26.i, %fast.i24.i ], [ %resolved.i22.i, %slow.i21.i ]
  %r7.i = load i64, ptr %common.ret.op.i23.i, align 1
  switch i64 %r7.i, label %f_v283_Option__uint8_unwrap.exit [
    i64 0, label %ast_block_0.i
    i64 1, label %ast_block_3.i
  ]

ast_block_0.i:                                    ; preds = %chali_primitive_address.exit27.i
  %r16.i = add i64 %7, 8
  store ptr @f_v283_Option__uint8_unwrap, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 810, ptr @chali_source_line, align 8
  %revision_pointer.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i = icmp eq ptr %revision_pointer.i.i, null
  br i1 %has_revision.not.i.i, label %slow.i.i, label %check.i.i

check.i.i:                                        ; preds = %ast_block_0.i
  %revision.i.i = load atomic i64, ptr %revision_pointer.i.i acquire, align 8
  %stamp.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i = icmp eq i64 %revision.i.i, %stamp.i.i
  %enabled.i.i = icmp ne i64 %revision.i.i, 0
  %live.i.i = and i1 %enabled.i.i, %same_revision.i.i
  %begin.i.i = load i64, ptr @chali_address_view, align 16
  %size.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i = sub i64 %r16.i, %begin.i.i
  %inside.i.i = icmp ult i64 %offset.i.i, %size.i.i
  %valid.i.i = and i1 %live.i.i, %inside.i.i
  br i1 %valid.i.i, label %fast.i.i, label %slow.i.i

fast.i.i:                                         ; preds = %check.i.i
  %base.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i = getelementptr i8, ptr %base.i.i, i64 %offset.i.i
  br label %chali_primitive_address.exit.i

slow.i.i:                                         ; preds = %check.i.i, %ast_block_0.i
  %resolved.i.i = call ptr @chali_host_address_view(i64 %r16.i, i64 1, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit.i

chali_primitive_address.exit.i:                   ; preds = %slow.i.i, %fast.i.i
  %common.ret.op.i.i = phi ptr [ %physical.i.i, %fast.i.i ], [ %resolved.i.i, %slow.i.i ]
  %r20.i = load i8, ptr %common.ret.op.i.i, align 1
  %r21.i = zext i8 %r20.i to i64
  br label %f_v283_Option__uint8_unwrap.exit

ast_block_3.i:                                    ; preds = %chali_primitive_address.exit27.i
  store ptr @f_v283_Option__uint8_unwrap, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 810, ptr @chali_source_line, align 8
  store i64 8, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  %r29.i = call i64 @chali_host_call(ptr nonnull @native_name3, i64 1, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r30.i = call i8 @chali_host_result_float()
  br label %f_v283_Option__uint8_unwrap.exit

f_v283_Option__uint8_unwrap.exit:                 ; preds = %chali_primitive_address.exit27.i, %chali_primitive_address.exit.i, %ast_block_3.i
  %s_v757___ast_283_result_1481.0.i = phi i64 [ %r21.i, %chali_primitive_address.exit.i ], [ 0, %ast_block_3.i ], [ 0, %chali_primitive_address.exit27.i ]
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i)
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i.i.i = call i64 @chali_host_string_storage(i64 0, i64 %r60)
  %r159 = add i64 %r2.i.i.i, -1
  store ptr @__chali_impl_program__sliceWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 67, ptr @chali_source_line, align 8
  %r160 = call i64 @f_v92_std__lang__String_byteAt(i64 %r60, i64 %r159)
  %r166 = call i64 @chali_host_copy_words(i64 %6, i64 %r160, i64 2)
  store i64 %6, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 16, ptr %r94, align 8
  store i8 0, ptr %r95, align 1
  %r173 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r174 = call i8 @chali_host_result_float()
  %r179.not = icmp eq i64 %6, %r160
  br i1 %r179.not, label %ast_block_14, label %ast_block_12

ast_block_12:                                     ; preds = %f_v283_Option__uint8_unwrap.exit
  store i64 %r160, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 16, ptr %r94, align 8
  store i8 0, ptr %r95, align 1
  store i64 282, ptr %r144, align 8
  store i8 0, ptr %r145, align 1
  %r190 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r191 = call i8 @chali_host_result_float()
  br label %ast_block_14

ast_block_14:                                     ; preds = %f_v283_Option__uint8_unwrap.exit, %ast_block_12
  store ptr @__chali_impl_program__sliceWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 67, ptr @chali_source_line, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i101)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i102)
  %revision_pointer.i5.i103 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i6.i104 = icmp eq ptr %revision_pointer.i5.i103, null
  br i1 %has_revision.not.i6.i104, label %slow.i21.i119, label %check.i7.i105

check.i7.i105:                                    ; preds = %ast_block_14
  %revision.i8.i106 = load atomic i64, ptr %revision_pointer.i5.i103 acquire, align 8
  %stamp.i9.i107 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i10.i108 = icmp eq i64 %revision.i8.i106, %stamp.i9.i107
  %enabled.i11.i109 = icmp ne i64 %revision.i8.i106, 0
  %live.i12.i110 = and i1 %enabled.i11.i109, %same_revision.i10.i108
  %begin.i13.i111 = load i64, ptr @chali_address_view, align 16
  %size.i14.i112 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i15.i113 = sub i64 %6, %begin.i13.i111
  %inside.i16.i114 = icmp ult i64 %offset.i15.i113, %size.i14.i112
  %remaining.i17.i115 = sub i64 %size.i14.i112, %offset.i15.i113
  %fits.i18.i116 = icmp ugt i64 %remaining.i17.i115, 7
  %range.i19.i117 = and i1 %inside.i16.i114, %fits.i18.i116
  %valid.i20.i118 = and i1 %live.i12.i110, %range.i19.i117
  br i1 %valid.i20.i118, label %fast.i24.i155, label %slow.i21.i119

fast.i24.i155:                                    ; preds = %check.i7.i105
  %base.i25.i156 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i26.i157 = getelementptr i8, ptr %base.i25.i156, i64 %offset.i15.i113
  br label %chali_primitive_address.exit27.i121

slow.i21.i119:                                    ; preds = %check.i7.i105, %ast_block_14
  %resolved.i22.i120 = call ptr @chali_host_address_view(i64 %6, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit27.i121

chali_primitive_address.exit27.i121:              ; preds = %slow.i21.i119, %fast.i24.i155
  %common.ret.op.i23.i122 = phi ptr [ %physical.i26.i157, %fast.i24.i155 ], [ %resolved.i22.i120, %slow.i21.i119 ]
  %r7.i123 = load i64, ptr %common.ret.op.i23.i122, align 1
  switch i64 %r7.i123, label %f_v283_Option__uint8_unwrap.exit158 [
    i64 0, label %ast_block_0.i128
    i64 1, label %ast_block_3.i124
  ]

ast_block_0.i128:                                 ; preds = %chali_primitive_address.exit27.i121
  %r16.i129 = add i64 %6, 8
  store ptr @f_v283_Option__uint8_unwrap, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 810, ptr @chali_source_line, align 8
  %revision_pointer.i.i130 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i131 = icmp eq ptr %revision_pointer.i.i130, null
  br i1 %has_revision.not.i.i131, label %slow.i.i146, label %check.i.i132

check.i.i132:                                     ; preds = %ast_block_0.i128
  %revision.i.i133 = load atomic i64, ptr %revision_pointer.i.i130 acquire, align 8
  %stamp.i.i134 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i135 = icmp eq i64 %revision.i.i133, %stamp.i.i134
  %enabled.i.i136 = icmp ne i64 %revision.i.i133, 0
  %live.i.i137 = and i1 %enabled.i.i136, %same_revision.i.i135
  %begin.i.i138 = load i64, ptr @chali_address_view, align 16
  %size.i.i139 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i140 = sub i64 %r16.i129, %begin.i.i138
  %inside.i.i141 = icmp ult i64 %offset.i.i140, %size.i.i139
  %valid.i.i145 = and i1 %live.i.i137, %inside.i.i141
  br i1 %valid.i.i145, label %fast.i.i152, label %slow.i.i146

fast.i.i152:                                      ; preds = %check.i.i132
  %base.i.i153 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i154 = getelementptr i8, ptr %base.i.i153, i64 %offset.i.i140
  br label %chali_primitive_address.exit.i148

slow.i.i146:                                      ; preds = %check.i.i132, %ast_block_0.i128
  %resolved.i.i147 = call ptr @chali_host_address_view(i64 %r16.i129, i64 1, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit.i148

chali_primitive_address.exit.i148:                ; preds = %slow.i.i146, %fast.i.i152
  %common.ret.op.i.i149 = phi ptr [ %physical.i.i154, %fast.i.i152 ], [ %resolved.i.i147, %slow.i.i146 ]
  %r20.i150 = load i8, ptr %common.ret.op.i.i149, align 1
  %r21.i151 = zext i8 %r20.i150 to i64
  br label %f_v283_Option__uint8_unwrap.exit158

ast_block_3.i124:                                 ; preds = %chali_primitive_address.exit27.i121
  store ptr @f_v283_Option__uint8_unwrap, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 810, ptr @chali_source_line, align 8
  store i64 8, ptr %indirect_arguments.i101, align 8
  store i8 0, ptr %native_argument_types.i102, align 1
  %r29.i125 = call i64 @chali_host_call(ptr nonnull @native_name3, i64 1, ptr nonnull %indirect_arguments.i101, ptr nonnull %native_argument_types.i102)
  %r30.i126 = call i8 @chali_host_result_float()
  br label %f_v283_Option__uint8_unwrap.exit158

f_v283_Option__uint8_unwrap.exit158:              ; preds = %chali_primitive_address.exit27.i121, %chali_primitive_address.exit.i148, %ast_block_3.i124
  %s_v757___ast_283_result_1481.0.i127 = phi i64 [ %r21.i151, %chali_primitive_address.exit.i148 ], [ 0, %ast_block_3.i124 ], [ 0, %chali_primitive_address.exit27.i121 ]
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i101)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i102)
  %r152 = add i64 %5, %r113
  %r196 = add i64 %r152, %s_v757___ast_283_result_1481.0.i
  %r199 = add i64 %r196, %s_v757___ast_283_result_1481.0.i127
  store ptr @__chali_impl_program__sliceWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 67, ptr @chali_source_line, align 8
  %r200 = and i64 %r199, 2147483647
  %r203.not = icmp eq i64 %7, 0
  br i1 %r203.not, label %ast_block_17, label %ast_block_15

ast_block_15:                                     ; preds = %f_v283_Option__uint8_unwrap.exit158
  call void @chali_host_drop(i64 noundef %7) #27
  br label %ast_block_17

ast_block_17:                                     ; preds = %f_v283_Option__uint8_unwrap.exit158, %ast_block_15
  %r210.not = icmp eq i64 %6, 0
  br i1 %r210.not, label %ast_block_20, label %ast_block_18

ast_block_18:                                     ; preds = %ast_block_17
  call void @chali_host_drop(i64 noundef %6) #27
  br label %ast_block_20

ast_block_20:                                     ; preds = %ast_block_17, %ast_block_18
  store ptr @__chali_impl_program__sliceWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 68, ptr @chali_source_line, align 8
  %r217 = add nuw nsw i64 %s_v722___ast_237_index_1347.0173, 1
  %r220.not = icmp eq i64 %r74, 0
  br i1 %r220.not, label %ast_block_23, label %ast_block_21

ast_block_21:                                     ; preds = %ast_block_20
  call void @chali_host_drop(i64 noundef %r74) #27
  br label %ast_block_23

ast_block_23:                                     ; preds = %ast_block_20, %ast_block_21
  %r229.not = icmp eq i64 %r60, 0
  br i1 %r229.not, label %ast_block_26, label %ast_block_24

ast_block_24:                                     ; preds = %ast_block_23
  call void @chali_host_drop(i64 noundef %r60) #27
  br label %ast_block_26

ast_block_26:                                     ; preds = %ast_block_23, %ast_block_24
  store ptr @__chali_impl_program__sliceWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 57, ptr @chali_source_line, align 8
  %exitcond175.not = icmp eq i64 %r217, 1024
  br i1 %exitcond175.not, label %ast_label_v730___chali_brk_148, label %ast_block_0

ast_label_v730___chali_brk_148:                   ; preds = %ast_block_26
  ret i64 %r200
}

; Function Attrs: uwtable
define range(i64 0, 2147483648) i64 @__chali_impl_program__findWork(i64 %p0) #24 section "chali_code" {
entry:
  %r0.b = load i1, ptr @s_g_1___chali_global_reference, align 1
  %r0 = select i1 %r0.b, i64 104, i64 0
  br i1 %r0.b, label %ast_block_0, label %0

0:                                                ; preds = %entry
  tail call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

ast_block_0:                                      ; preds = %entry, %ast_block_5
  %s_v734___ast_238_index_1358.0103 = phi i64 [ %r88, %ast_block_5 ], [ 0, %entry ]
  %s_v732___ast_238_acc_1357.0102 = phi i64 [ %r85, %ast_block_5 ], [ %p0, %entry ]
  %r12 = lshr i64 %s_v732___ast_238_acc_1357.0102, 7
  %r15 = add nuw nsw i64 %r12, %s_v734___ast_238_index_1358.0103
  %r16 = and i64 %r15, 255
  %r17 = load i64, ptr @s_g_4_program__unitWidth, align 8
  %r19 = mul i64 %r17, %r16
  %r24 = add i64 %s_v734___ast_238_index_1358.0103, %p0
  %r25 = and i64 %r24, 3
  store ptr @__chali_impl_program__findWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 81, ptr @chali_source_line, align 8
  %r29 = icmp eq i64 %r25, 0
  br i1 %r29, label %chali_require_reference.exit98, label %ast_block_4

chali_require_reference.exit98:                   ; preds = %ast_block_0
  %revision_pointer.i75 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i76 = icmp eq ptr %revision_pointer.i75, null
  br i1 %has_revision.not.i76, label %slow.i91, label %check.i77

check.i77:                                        ; preds = %chali_require_reference.exit98
  %revision.i78 = load atomic i64, ptr %revision_pointer.i75 acquire, align 8
  %stamp.i79 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i80 = icmp eq i64 %revision.i78, %stamp.i79
  %enabled.i81 = icmp ne i64 %revision.i78, 0
  %live.i82 = and i1 %enabled.i81, %same_revision.i80
  %begin.i83 = load i64, ptr @chali_address_view, align 16
  %size.i84 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i85 = sub i64 %r0, %begin.i83
  %inside.i86 = icmp ult i64 %offset.i85, %size.i84
  %remaining.i87 = sub i64 %size.i84, %offset.i85
  %fits.i88 = icmp ugt i64 %remaining.i87, 7
  %range.i89 = and i1 %inside.i86, %fits.i88
  %valid.i90 = and i1 %live.i82, %range.i89
  br i1 %valid.i90, label %fast.i94, label %slow.i91

fast.i94:                                         ; preds = %check.i77
  %base.i95 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i96 = getelementptr i8, ptr %base.i95, i64 %offset.i85
  br label %ast_block_5

slow.i91:                                         ; preds = %check.i77, %chali_require_reference.exit98
  %resolved.i92 = tail call ptr @chali_host_address_view(i64 %r0, i64 8, ptr nonnull @chali_address_view)
  br label %ast_block_5

ast_block_4:                                      ; preds = %ast_block_0
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 82, ptr @chali_source_line, align 8
  %r43 = icmp eq i64 %r25, 1
  br i1 %r43, label %chali_require_reference.exit99, label %ast_block_7

chali_require_reference.exit99:                   ; preds = %ast_block_4
  %revision_pointer.i52 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i53 = icmp eq ptr %revision_pointer.i52, null
  br i1 %has_revision.not.i53, label %slow.i68, label %check.i54

check.i54:                                        ; preds = %chali_require_reference.exit99
  %revision.i55 = load atomic i64, ptr %revision_pointer.i52 acquire, align 8
  %stamp.i56 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i57 = icmp eq i64 %revision.i55, %stamp.i56
  %enabled.i58 = icmp ne i64 %revision.i55, 0
  %live.i59 = and i1 %enabled.i58, %same_revision.i57
  %begin.i60 = load i64, ptr @chali_address_view, align 16
  %size.i61 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i62 = sub i64 %r0, %begin.i60
  %inside.i63 = icmp ult i64 %offset.i62, %size.i61
  %remaining.i64 = sub i64 %size.i61, %offset.i62
  %fits.i65 = icmp ugt i64 %remaining.i64, 7
  %range.i66 = and i1 %inside.i63, %fits.i65
  %valid.i67 = and i1 %live.i59, %range.i66
  br i1 %valid.i67, label %fast.i71, label %slow.i68

fast.i71:                                         ; preds = %check.i54
  %base.i72 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i73 = getelementptr i8, ptr %base.i72, i64 %offset.i62
  br label %ast_block_5

slow.i68:                                         ; preds = %check.i54, %chali_require_reference.exit99
  %resolved.i69 = tail call ptr @chali_host_address_view(i64 %r0, i64 8, ptr nonnull @chali_address_view)
  br label %ast_block_5

ast_block_7:                                      ; preds = %ast_block_4
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 83, ptr @chali_source_line, align 8
  %r57 = icmp eq i64 %r25, 2
  %revision_pointer.i29 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i30 = icmp eq ptr %revision_pointer.i29, null
  br i1 %r57, label %chali_require_reference.exit100, label %chali_require_reference.exit101

chali_require_reference.exit100:                  ; preds = %ast_block_7
  br i1 %has_revision.not.i30, label %slow.i45, label %check.i31

check.i31:                                        ; preds = %chali_require_reference.exit100
  %revision.i32 = load atomic i64, ptr %revision_pointer.i29 acquire, align 8
  %stamp.i33 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i34 = icmp eq i64 %revision.i32, %stamp.i33
  %enabled.i35 = icmp ne i64 %revision.i32, 0
  %live.i36 = and i1 %enabled.i35, %same_revision.i34
  %begin.i37 = load i64, ptr @chali_address_view, align 16
  %size.i38 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i39 = sub i64 %r0, %begin.i37
  %inside.i40 = icmp ult i64 %offset.i39, %size.i38
  %remaining.i41 = sub i64 %size.i38, %offset.i39
  %fits.i42 = icmp ugt i64 %remaining.i41, 7
  %range.i43 = and i1 %inside.i40, %fits.i42
  %valid.i44 = and i1 %live.i36, %range.i43
  br i1 %valid.i44, label %fast.i48, label %slow.i45

fast.i48:                                         ; preds = %check.i31
  %base.i49 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i50 = getelementptr i8, ptr %base.i49, i64 %offset.i39
  br label %ast_block_5

slow.i45:                                         ; preds = %check.i31, %chali_require_reference.exit100
  %resolved.i46 = tail call ptr @chali_host_address_view(i64 %r0, i64 8, ptr nonnull @chali_address_view)
  br label %ast_block_5

chali_require_reference.exit101:                  ; preds = %ast_block_7
  br i1 %has_revision.not.i30, label %slow.i, label %check.i

check.i:                                          ; preds = %chali_require_reference.exit101
  %revision.i = load atomic i64, ptr %revision_pointer.i29 acquire, align 8
  %stamp.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i = icmp eq i64 %revision.i, %stamp.i
  %enabled.i = icmp ne i64 %revision.i, 0
  %live.i = and i1 %enabled.i, %same_revision.i
  %begin.i = load i64, ptr @chali_address_view, align 16
  %size.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i = sub i64 %r0, %begin.i
  %inside.i = icmp ult i64 %offset.i, %size.i
  %remaining.i = sub i64 %size.i, %offset.i
  %fits.i = icmp ugt i64 %remaining.i, 7
  %range.i = and i1 %inside.i, %fits.i
  %valid.i = and i1 %live.i, %range.i
  br i1 %valid.i, label %fast.i, label %slow.i

fast.i:                                           ; preds = %check.i
  %base.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i = getelementptr i8, ptr %base.i, i64 %offset.i
  br label %ast_block_5

slow.i:                                           ; preds = %check.i, %chali_require_reference.exit101
  %resolved.i = tail call ptr @chali_host_address_view(i64 %r0, i64 8, ptr nonnull @chali_address_view)
  br label %ast_block_5

ast_block_5:                                      ; preds = %slow.i, %fast.i, %slow.i45, %fast.i48, %slow.i68, %fast.i71, %slow.i91, %fast.i94
  %common.ret.op.i70.sink = phi ptr [ %resolved.i92, %slow.i91 ], [ %resolved.i46, %slow.i45 ], [ %resolved.i69, %slow.i68 ], [ %physical.i96, %fast.i94 ], [ %physical.i73, %fast.i71 ], [ %physical.i50, %fast.i48 ], [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  %.sink104 = phi i64 [ 81, %slow.i91 ], [ 83, %slow.i45 ], [ 82, %slow.i68 ], [ 81, %fast.i94 ], [ 82, %fast.i71 ], [ 83, %fast.i48 ], [ 84, %fast.i ], [ 84, %slow.i ]
  %.sink = phi i64 [ 120, %slow.i91 ], [ 144, %slow.i45 ], [ 136, %slow.i68 ], [ 120, %fast.i94 ], [ 136, %fast.i71 ], [ 144, %fast.i48 ], [ 160, %fast.i ], [ 160, %slow.i ]
  %r50 = load i64, ptr %common.ret.op.i70.sink, align 1
  store ptr @__chali_impl_program__findWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 %.sink104, ptr @chali_source_line, align 8
  %r53 = tail call i64 @f_v85_std__lang__String_indexOf(i64 %r50, i64 %.sink, i64 %r19)
  %r80 = mul i64 %s_v732___ast_238_acc_1357.0102, 33
  %r83 = add i64 %r80, 1
  %r84 = add i64 %r83, %r53
  %r85 = and i64 %r84, 2147483647
  %r88 = add nuw nsw i64 %s_v734___ast_238_index_1358.0103, 1
  store ptr @__chali_impl_program__findWork, ptr @chali_source_function, align 8
  store ptr @source_file1, ptr @chali_source_file, align 8
  store i64 77, ptr @chali_source_line, align 8
  %exitcond.not = icmp eq i64 %r88, 256
  br i1 %exitcond.not, label %ast_label_v740___chali_brk_150, label %ast_block_0

ast_label_v740___chali_brk_150:                   ; preds = %ast_block_5
  ret i64 %r85
}

; Function Attrs: alwaysinline uwtable
define internal noundef i64 @f_v271_std__lang__barePointer__int64(i64 %p0) #21 section "chali_code" {
entry:
  %indirect_arguments = alloca [7 x i64], align 8
  %native_argument_types = alloca [7 x i8], align 1
  %raw_memory_output = alloca ptr, align 8
  %raw_memory_input = alloca ptr, align 8
  %0 = tail call i64 @chali_host_allocate(i64 noundef 16) #27
  %r5 = tail call i64 @chali_host_copy_words(i64 %0, i64 %p0, i64 2)
  store i64 %0, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r10 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  store i64 16, ptr %r10, align 8
  %r11 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r11, align 1
  %r12 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r13 = call i8 @chali_host_result_float()
  %1 = call i64 @chali_host_allocate(i64 noundef 8) #27
  call void @chali_host_memory_prepare(i64 8, i64 %1, i64 0, i64 8, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r17 = load ptr, ptr %raw_memory_output, align 8
  store i64 0, ptr %r17, align 1
  store i64 %1, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r10, align 8
  store i8 0, ptr %r11, align 1
  %r25 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r26 = call i8 @chali_host_result_float()
  store ptr @f_v271_std__lang__barePointer__int64, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1225, ptr @chali_source_line, align 8
  %revision_pointer.i50 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i51 = icmp eq ptr %revision_pointer.i50, null
  br i1 %has_revision.not.i51, label %slow.i66, label %check.i52

check.i52:                                        ; preds = %entry
  %revision.i53 = load atomic i64, ptr %revision_pointer.i50 acquire, align 8
  %stamp.i54 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i55 = icmp eq i64 %revision.i53, %stamp.i54
  %enabled.i56 = icmp ne i64 %revision.i53, 0
  %live.i57 = and i1 %enabled.i56, %same_revision.i55
  %begin.i58 = load i64, ptr @chali_address_view, align 16
  %size.i59 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i60 = sub i64 %0, %begin.i58
  %inside.i61 = icmp ult i64 %offset.i60, %size.i59
  %remaining.i62 = sub i64 %size.i59, %offset.i60
  %fits.i63 = icmp ugt i64 %remaining.i62, 7
  %range.i64 = and i1 %inside.i61, %fits.i63
  %valid.i65 = and i1 %live.i57, %range.i64
  br i1 %valid.i65, label %fast.i69, label %slow.i66

fast.i69:                                         ; preds = %check.i52
  %base.i70 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i71 = getelementptr i8, ptr %base.i70, i64 %offset.i60
  br label %chali_primitive_address.exit72

slow.i66:                                         ; preds = %check.i52, %entry
  %resolved.i67 = call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit72

chali_primitive_address.exit72:                   ; preds = %fast.i69, %slow.i66
  %common.ret.op.i68 = phi ptr [ %physical.i71, %fast.i69 ], [ %resolved.i67, %slow.i66 ]
  %r32 = load i64, ptr %common.ret.op.i68, align 1
  %r36 = add i64 %0, 8
  store ptr @f_v271_std__lang__barePointer__int64, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1225, ptr @chali_source_line, align 8
  %revision_pointer.i27 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i28 = icmp eq ptr %revision_pointer.i27, null
  br i1 %has_revision.not.i28, label %slow.i43, label %check.i29

check.i29:                                        ; preds = %chali_primitive_address.exit72
  %revision.i30 = load atomic i64, ptr %revision_pointer.i27 acquire, align 8
  %stamp.i31 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i32 = icmp eq i64 %revision.i30, %stamp.i31
  %enabled.i33 = icmp ne i64 %revision.i30, 0
  %live.i34 = and i1 %enabled.i33, %same_revision.i32
  %begin.i35 = load i64, ptr @chali_address_view, align 16
  %size.i36 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i37 = sub i64 %r36, %begin.i35
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

slow.i43:                                         ; preds = %check.i29, %chali_primitive_address.exit72
  %resolved.i44 = call ptr @chali_host_address_view(i64 %r36, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit49

chali_primitive_address.exit49:                   ; preds = %fast.i46, %slow.i43
  %common.ret.op.i45 = phi ptr [ %physical.i48, %fast.i46 ], [ %resolved.i44, %slow.i43 ]
  %r38 = load i64, ptr %common.ret.op.i45, align 1
  store ptr @f_v271_std__lang__barePointer__int64, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1225, ptr @chali_source_line, align 8
  %r39 = call i64 @chali_host_raw_storage(i64 6, i64 %r32, i64 %r38, i64 0, i64 0, i64 0)
  %revision_pointer.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i = icmp eq ptr %revision_pointer.i, null
  br i1 %has_revision.not.i, label %slow.i, label %check.i

check.i:                                          ; preds = %chali_primitive_address.exit49
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

slow.i:                                           ; preds = %check.i, %chali_primitive_address.exit49
  %resolved.i = call ptr @chali_host_address_view(i64 %1, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit

chali_primitive_address.exit:                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  store i64 %r39, ptr %common.ret.op.i, align 1
  store i64 656, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r10, align 8
  store i8 0, ptr %r11, align 1
  %r51 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r52 = call i8 @chali_host_result_float()
  %r57 = call i64 @chali_host_copy_words(i64 656, i64 %1, i64 1)
  store i64 656, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r10, align 8
  store i8 0, ptr %r11, align 1
  %r64 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r65 = call i8 @chali_host_result_float()
  %r68.not = icmp eq i64 %0, 0
  br i1 %r68.not, label %ast_block_2, label %ast_block_0

ast_block_0:                                      ; preds = %chali_primitive_address.exit
  call void @chali_host_drop(i64 noundef %0) #27
  br label %ast_block_2

ast_block_2:                                      ; preds = %chali_primitive_address.exit, %ast_block_0
  %r75.not = icmp eq i64 %1, 0
  br i1 %r75.not, label %ast_block_5, label %ast_block_3

ast_block_3:                                      ; preds = %ast_block_2
  call void @chali_host_drop(i64 noundef %1) #27
  br label %ast_block_5

ast_block_5:                                      ; preds = %ast_block_2, %ast_block_3
  ret i64 656
}

; Function Attrs: alwaysinline uwtable
define internal noundef i64 @f_v273_std__lang__pointerFromAddress__int64(i64 %p0) #21 section "chali_code" {
entry:
  %indirect_arguments = alloca [4 x i64], align 8
  %native_argument_types = alloca [4 x i8], align 1
  %raw_memory_output = alloca ptr, align 8
  %raw_memory_input = alloca ptr, align 8
  %0 = tail call i64 @chali_host_allocate(i64 noundef 8) #27
  call void @chali_host_memory_prepare(i64 8, i64 %0, i64 0, i64 8, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r3 = load ptr, ptr %raw_memory_output, align 8
  store i64 0, ptr %r3, align 1
  store i64 %0, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r9 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  store i64 8, ptr %r9, align 8
  %r10 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r10, align 1
  %r11 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
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
  store i64 664, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r9, align 8
  store i8 0, ptr %r10, align 1
  %r26 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r27 = call i8 @chali_host_result_float()
  %r32 = call i64 @chali_host_copy_words(i64 664, i64 %0, i64 1)
  store i64 664, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r9, align 8
  store i8 0, ptr %r10, align 1
  %r39 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r40 = call i8 @chali_host_result_float()
  %r43.not = icmp eq i64 %0, 0
  br i1 %r43.not, label %ast_block_2, label %ast_block_0

ast_block_0:                                      ; preds = %chali_primitive_address.exit
  call void @chali_host_drop(i64 noundef %0) #27
  br label %ast_block_2

ast_block_2:                                      ; preds = %chali_primitive_address.exit, %ast_block_0
  ret i64 664
}

; Function Attrs: alwaysinline uwtable
define internal noundef i64 @f_v275_std__lang__pointerFromAddress__uint8(i64 %p0) #21 section "chali_code" {
entry:
  %indirect_arguments = alloca [4 x i64], align 8
  %native_argument_types = alloca [4 x i8], align 1
  %raw_memory_output = alloca ptr, align 8
  %raw_memory_input = alloca ptr, align 8
  %0 = tail call i64 @chali_host_allocate(i64 noundef 8) #27
  call void @chali_host_memory_prepare(i64 8, i64 %0, i64 0, i64 8, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r3 = load ptr, ptr %raw_memory_output, align 8
  store i64 0, ptr %r3, align 1
  store i64 %0, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r9 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  store i64 8, ptr %r9, align 8
  %r10 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r10, align 1
  %r11 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
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
  store i64 672, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r9, align 8
  store i8 0, ptr %r10, align 1
  %r26 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r27 = call i8 @chali_host_result_float()
  %r32 = call i64 @chali_host_copy_words(i64 672, i64 %0, i64 1)
  store i64 672, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r9, align 8
  store i8 0, ptr %r10, align 1
  %r39 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r40 = call i8 @chali_host_result_float()
  %r43.not = icmp eq i64 %0, 0
  br i1 %r43.not, label %ast_block_2, label %ast_block_0

ast_block_0:                                      ; preds = %chali_primitive_address.exit
  call void @chali_host_drop(i64 noundef %0) #27
  br label %ast_block_2

ast_block_2:                                      ; preds = %chali_primitive_address.exit, %ast_block_0
  ret i64 672
}

; Function Attrs: alwaysinline uwtable
define internal range(i64 0, 256) i64 @f_v283_Option__uint8_unwrap(i64 %p0) #21 section "chali_code" {
entry:
  %indirect_arguments = alloca [2 x i64], align 8
  %native_argument_types = alloca [2 x i8], align 1
  %revision_pointer.i5 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i6 = icmp eq ptr %revision_pointer.i5, null
  br i1 %has_revision.not.i6, label %slow.i21, label %check.i7

check.i7:                                         ; preds = %entry
  %revision.i8 = load atomic i64, ptr %revision_pointer.i5 acquire, align 8
  %stamp.i9 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i10 = icmp eq i64 %revision.i8, %stamp.i9
  %enabled.i11 = icmp ne i64 %revision.i8, 0
  %live.i12 = and i1 %enabled.i11, %same_revision.i10
  %begin.i13 = load i64, ptr @chali_address_view, align 16
  %size.i14 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i15 = sub i64 %p0, %begin.i13
  %inside.i16 = icmp ult i64 %offset.i15, %size.i14
  %remaining.i17 = sub i64 %size.i14, %offset.i15
  %fits.i18 = icmp ugt i64 %remaining.i17, 7
  %range.i19 = and i1 %inside.i16, %fits.i18
  %valid.i20 = and i1 %live.i12, %range.i19
  br i1 %valid.i20, label %fast.i24, label %slow.i21

fast.i24:                                         ; preds = %check.i7
  %base.i25 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i26 = getelementptr i8, ptr %base.i25, i64 %offset.i15
  br label %chali_primitive_address.exit27

slow.i21:                                         ; preds = %check.i7, %entry
  %resolved.i22 = tail call ptr @chali_host_address_view(i64 %p0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit27

chali_primitive_address.exit27:                   ; preds = %fast.i24, %slow.i21
  %common.ret.op.i23 = phi ptr [ %physical.i26, %fast.i24 ], [ %resolved.i22, %slow.i21 ]
  %r7 = load i64, ptr %common.ret.op.i23, align 1
  switch i64 %r7, label %ast_label_v760___chali_brk_164 [
    i64 0, label %ast_block_0
    i64 1, label %ast_block_3
  ]

ast_block_0:                                      ; preds = %chali_primitive_address.exit27
  %r16 = add i64 %p0, 8
  store ptr @f_v283_Option__uint8_unwrap, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 810, ptr @chali_source_line, align 8
  %revision_pointer.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i = icmp eq ptr %revision_pointer.i, null
  br i1 %has_revision.not.i, label %slow.i, label %check.i

check.i:                                          ; preds = %ast_block_0
  %revision.i = load atomic i64, ptr %revision_pointer.i acquire, align 8
  %stamp.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i = icmp eq i64 %revision.i, %stamp.i
  %enabled.i = icmp ne i64 %revision.i, 0
  %live.i = and i1 %enabled.i, %same_revision.i
  %begin.i = load i64, ptr @chali_address_view, align 16
  %size.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i = sub i64 %r16, %begin.i
  %inside.i = icmp ult i64 %offset.i, %size.i
  %valid.i = and i1 %live.i, %inside.i
  br i1 %valid.i, label %fast.i, label %slow.i

fast.i:                                           ; preds = %check.i
  %base.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i = getelementptr i8, ptr %base.i, i64 %offset.i
  br label %chali_primitive_address.exit

slow.i:                                           ; preds = %check.i, %ast_block_0
  %resolved.i = tail call ptr @chali_host_address_view(i64 %r16, i64 1, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit

chali_primitive_address.exit:                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  %r20 = load i8, ptr %common.ret.op.i, align 1
  %r21 = zext i8 %r20 to i64
  br label %ast_label_v760___chali_brk_164

ast_block_3:                                      ; preds = %chali_primitive_address.exit27
  store ptr @f_v283_Option__uint8_unwrap, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 810, ptr @chali_source_line, align 8
  store i64 8, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r29 = call i64 @chali_host_call(ptr nonnull @native_name3, i64 1, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r30 = call i8 @chali_host_result_float()
  br label %ast_label_v760___chali_brk_164

ast_label_v760___chali_brk_164:                   ; preds = %chali_primitive_address.exit27, %ast_block_3, %chali_primitive_address.exit
  %s_v757___ast_283_result_1481.0 = phi i64 [ %r21, %chali_primitive_address.exit ], [ 0, %ast_block_3 ], [ 0, %chali_primitive_address.exit27 ]
  ret i64 %s_v757___ast_283_result_1481.0
}

; Function Attrs: uwtable
define internal noundef i64 @f_v461_std__lang__dropAt__int64(i64 %p0) #24 section "chali_code" {
entry:
  %indirect_arguments.i = alloca [7 x i64], align 8
  %native_argument_types.i = alloca [7 x i8], align 1
  %raw_memory_output.i = alloca ptr, align 8
  %raw_memory_input.i = alloca ptr, align 8
  %indirect_arguments = alloca [4 x i64], align 8
  %native_argument_types = alloca [4 x i8], align 1
  %0 = tail call i64 @chali_host_allocate(i64 noundef 16) #27
  %r5 = tail call i64 @chali_host_copy_words(i64 %0, i64 %p0, i64 2)
  store i64 %0, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r10 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  store i64 16, ptr %r10, align 8
  %r11 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r11, align 1
  %r12 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r13 = call i8 @chali_host_result_float()
  store ptr @f_v461_std__lang__dropAt__int64, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 5, ptr @chali_source_line, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %raw_memory_output.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %raw_memory_input.i)
  %1 = call i64 @chali_host_allocate(i64 noundef 16) #27
  %r5.i = call i64 @chali_host_copy_words(i64 %1, i64 %0, i64 2)
  store i64 %1, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  %r10.i = getelementptr inbounds nuw i8, ptr %indirect_arguments.i, i64 8
  store i64 16, ptr %r10.i, align 8
  %r11.i = getelementptr inbounds nuw i8, ptr %native_argument_types.i, i64 1
  store i8 0, ptr %r11.i, align 1
  %r12.i = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r13.i = call i8 @chali_host_result_float()
  store ptr @f_v462_std__lang__takeAt__int64, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 5, ptr @chali_source_line, align 8
  %revision_pointer.i144.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i145.i = icmp eq ptr %revision_pointer.i144.i, null
  br i1 %has_revision.not.i145.i, label %slow.i160.i, label %check.i146.i

check.i146.i:                                     ; preds = %entry
  %revision.i147.i = load atomic i64, ptr %revision_pointer.i144.i acquire, align 8
  %stamp.i148.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i149.i = icmp eq i64 %revision.i147.i, %stamp.i148.i
  %enabled.i150.i = icmp ne i64 %revision.i147.i, 0
  %live.i151.i = and i1 %enabled.i150.i, %same_revision.i149.i
  %begin.i152.i = load i64, ptr @chali_address_view, align 16
  %size.i153.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i154.i = sub i64 %1, %begin.i152.i
  %inside.i155.i = icmp ult i64 %offset.i154.i, %size.i153.i
  %remaining.i156.i = sub i64 %size.i153.i, %offset.i154.i
  %fits.i157.i = icmp ugt i64 %remaining.i156.i, 7
  %range.i158.i = and i1 %inside.i155.i, %fits.i157.i
  %valid.i159.i = and i1 %live.i151.i, %range.i158.i
  br i1 %valid.i159.i, label %fast.i163.i, label %slow.i160.i

fast.i163.i:                                      ; preds = %check.i146.i
  %base.i164.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i165.i = getelementptr i8, ptr %base.i164.i, i64 %offset.i154.i
  br label %chali_primitive_address.exit166.i

slow.i160.i:                                      ; preds = %check.i146.i, %entry
  %resolved.i161.i = call ptr @chali_host_address_view(i64 %1, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit166.i

chali_primitive_address.exit166.i:                ; preds = %slow.i160.i, %fast.i163.i
  %common.ret.op.i162.i = phi ptr [ %physical.i165.i, %fast.i163.i ], [ %resolved.i161.i, %slow.i160.i ]
  %r19.i = load i64, ptr %common.ret.op.i162.i, align 1
  %r23.i = add i64 %1, 8
  store ptr @f_v462_std__lang__takeAt__int64, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 5, ptr @chali_source_line, align 8
  %revision_pointer.i121.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i122.i = icmp eq ptr %revision_pointer.i121.i, null
  br i1 %has_revision.not.i122.i, label %slow.i137.i, label %check.i123.i

check.i123.i:                                     ; preds = %chali_primitive_address.exit166.i
  %revision.i124.i = load atomic i64, ptr %revision_pointer.i121.i acquire, align 8
  %stamp.i125.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i126.i = icmp eq i64 %revision.i124.i, %stamp.i125.i
  %enabled.i127.i = icmp ne i64 %revision.i124.i, 0
  %live.i128.i = and i1 %enabled.i127.i, %same_revision.i126.i
  %begin.i129.i = load i64, ptr @chali_address_view, align 16
  %size.i130.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i131.i = sub i64 %r23.i, %begin.i129.i
  %inside.i132.i = icmp ult i64 %offset.i131.i, %size.i130.i
  %remaining.i133.i = sub i64 %size.i130.i, %offset.i131.i
  %fits.i134.i = icmp ugt i64 %remaining.i133.i, 7
  %range.i135.i = and i1 %inside.i132.i, %fits.i134.i
  %valid.i136.i = and i1 %live.i128.i, %range.i135.i
  br i1 %valid.i136.i, label %fast.i140.i, label %slow.i137.i

fast.i140.i:                                      ; preds = %check.i123.i
  %base.i141.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i142.i = getelementptr i8, ptr %base.i141.i, i64 %offset.i131.i
  br label %chali_primitive_address.exit143.i

slow.i137.i:                                      ; preds = %check.i123.i, %chali_primitive_address.exit166.i
  %resolved.i138.i = call ptr @chali_host_address_view(i64 %r23.i, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit143.i

chali_primitive_address.exit143.i:                ; preds = %slow.i137.i, %fast.i140.i
  %common.ret.op.i139.i = phi ptr [ %physical.i142.i, %fast.i140.i ], [ %resolved.i138.i, %slow.i137.i ]
  %r25.i = load i64, ptr %common.ret.op.i139.i, align 1
  store ptr @f_v462_std__lang__takeAt__int64, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 5, ptr @chali_source_line, align 8
  %r26.i = call i64 @chali_host_raw_storage(i64 1, i64 %r19.i, i64 %r25.i, i64 1, i64 0, i64 1)
  %2 = icmp eq i64 %r26.i, 0
  br i1 %2, label %3, label %chali_require_reference.exit9

3:                                                ; preds = %chali_primitive_address.exit143.i
  call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit9:                    ; preds = %chali_primitive_address.exit143.i
  store i64 %r26.i, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  store i64 0, ptr %r10.i, align 8
  store i8 0, ptr %r11.i, align 1
  %r39.i = getelementptr inbounds nuw i8, ptr %indirect_arguments.i, i64 16
  store i64 1, ptr %r39.i, align 8
  %r40.i = getelementptr inbounds nuw i8, ptr %native_argument_types.i, i64 2
  store i8 0, ptr %r40.i, align 1
  %r41.i = call i64 @chali_host_call(ptr nonnull @native_name4, i64 3, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r42.i = call i8 @chali_host_result_float()
  %revision_pointer.i98.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i99.i = icmp eq ptr %revision_pointer.i98.i, null
  br i1 %has_revision.not.i99.i, label %slow.i114.i, label %check.i100.i

check.i100.i:                                     ; preds = %chali_require_reference.exit9
  %revision.i101.i = load atomic i64, ptr %revision_pointer.i98.i acquire, align 8
  %stamp.i102.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i103.i = icmp eq i64 %revision.i101.i, %stamp.i102.i
  %enabled.i104.i = icmp ne i64 %revision.i101.i, 0
  %live.i105.i = and i1 %enabled.i104.i, %same_revision.i103.i
  %begin.i106.i = load i64, ptr @chali_address_view, align 16
  %size.i107.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i108.i = sub i64 %r26.i, %begin.i106.i
  %inside.i109.i = icmp ult i64 %offset.i108.i, %size.i107.i
  %remaining.i110.i = sub i64 %size.i107.i, %offset.i108.i
  %fits.i111.i = icmp ugt i64 %remaining.i110.i, 7
  %range.i112.i = and i1 %inside.i109.i, %fits.i111.i
  %valid.i113.i = and i1 %live.i105.i, %range.i112.i
  br i1 %valid.i113.i, label %fast.i117.i, label %slow.i114.i

fast.i117.i:                                      ; preds = %check.i100.i
  %base.i118.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i119.i = getelementptr i8, ptr %base.i118.i, i64 %offset.i108.i
  br label %chali_primitive_address.exit120.i

slow.i114.i:                                      ; preds = %check.i100.i, %chali_require_reference.exit9
  %resolved.i115.i = call ptr @chali_host_address_view(i64 %r26.i, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit120.i

chali_primitive_address.exit120.i:                ; preds = %slow.i114.i, %fast.i117.i
  %common.ret.op.i116.i = phi ptr [ %physical.i119.i, %fast.i117.i ], [ %resolved.i115.i, %slow.i114.i ]
  %r46.i = load i64, ptr %common.ret.op.i116.i, align 1
  call void @chali_host_memory_prepare(i64 8, i64 %r26.i, i64 0, i64 8, ptr nonnull %raw_memory_output.i, ptr nonnull %raw_memory_input.i)
  %r49.i = load ptr, ptr %raw_memory_output.i, align 8
  store i64 0, ptr %r49.i, align 1
  %4 = call i64 @chali_host_allocate(i64 noundef 8) #27
  %revision_pointer.i75.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i76.i = icmp eq ptr %revision_pointer.i75.i, null
  br i1 %has_revision.not.i76.i, label %slow.i91.i, label %check.i77.i

check.i77.i:                                      ; preds = %chali_primitive_address.exit120.i
  %revision.i78.i = load atomic i64, ptr %revision_pointer.i75.i acquire, align 8
  %stamp.i79.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i80.i = icmp eq i64 %revision.i78.i, %stamp.i79.i
  %enabled.i81.i = icmp ne i64 %revision.i78.i, 0
  %live.i82.i = and i1 %enabled.i81.i, %same_revision.i80.i
  %begin.i83.i = load i64, ptr @chali_address_view, align 16
  %size.i84.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i85.i = sub i64 %4, %begin.i83.i
  %inside.i86.i = icmp ult i64 %offset.i85.i, %size.i84.i
  %remaining.i87.i = sub i64 %size.i84.i, %offset.i85.i
  %fits.i88.i = icmp ugt i64 %remaining.i87.i, 7
  %range.i89.i = and i1 %inside.i86.i, %fits.i88.i
  %valid.i90.i = and i1 %live.i82.i, %range.i89.i
  br i1 %valid.i90.i, label %fast.i94.i, label %slow.i91.i

fast.i94.i:                                       ; preds = %check.i77.i
  %base.i95.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i96.i = getelementptr i8, ptr %base.i95.i, i64 %offset.i85.i
  br label %chali_primitive_address.exit97.i

slow.i91.i:                                       ; preds = %check.i77.i, %chali_primitive_address.exit120.i
  %resolved.i92.i = call ptr @chali_host_address_view(i64 %4, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit97.i

chali_primitive_address.exit97.i:                 ; preds = %slow.i91.i, %fast.i94.i
  %common.ret.op.i93.i = phi ptr [ %physical.i96.i, %fast.i94.i ], [ %resolved.i92.i, %slow.i91.i ]
  store i64 %r46.i, ptr %common.ret.op.i93.i, align 1
  store ptr @f_v462_std__lang__takeAt__int64, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 8, ptr @chali_source_line, align 8
  %revision_pointer.i52.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i53.i = icmp eq ptr %revision_pointer.i52.i, null
  br i1 %has_revision.not.i53.i, label %slow.i68.i, label %check.i54.i

check.i54.i:                                      ; preds = %chali_primitive_address.exit97.i
  %revision.i55.i = load atomic i64, ptr %revision_pointer.i52.i acquire, align 8
  %stamp.i56.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i57.i = icmp eq i64 %revision.i55.i, %stamp.i56.i
  %enabled.i58.i = icmp ne i64 %revision.i55.i, 0
  %live.i59.i = and i1 %enabled.i58.i, %same_revision.i57.i
  %begin.i60.i = load i64, ptr @chali_address_view, align 16
  %size.i61.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i62.i = sub i64 %1, %begin.i60.i
  %inside.i63.i = icmp ult i64 %offset.i62.i, %size.i61.i
  %remaining.i64.i = sub i64 %size.i61.i, %offset.i62.i
  %fits.i65.i = icmp ugt i64 %remaining.i64.i, 7
  %range.i66.i = and i1 %inside.i63.i, %fits.i65.i
  %valid.i67.i = and i1 %live.i59.i, %range.i66.i
  br i1 %valid.i67.i, label %fast.i71.i, label %slow.i68.i

fast.i71.i:                                       ; preds = %check.i54.i
  %base.i72.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i73.i = getelementptr i8, ptr %base.i72.i, i64 %offset.i62.i
  br label %chali_primitive_address.exit74.i

slow.i68.i:                                       ; preds = %check.i54.i, %chali_primitive_address.exit97.i
  %resolved.i69.i = call ptr @chali_host_address_view(i64 %1, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit74.i

chali_primitive_address.exit74.i:                 ; preds = %slow.i68.i, %fast.i71.i
  %common.ret.op.i70.i = phi ptr [ %physical.i73.i, %fast.i71.i ], [ %resolved.i69.i, %slow.i68.i ]
  %r64.i = load i64, ptr %common.ret.op.i70.i, align 1
  store ptr @f_v462_std__lang__takeAt__int64, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 8, ptr @chali_source_line, align 8
  %revision_pointer.i29.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i30.i = icmp eq ptr %revision_pointer.i29.i, null
  br i1 %has_revision.not.i30.i, label %slow.i45.i, label %check.i31.i

check.i31.i:                                      ; preds = %chali_primitive_address.exit74.i
  %revision.i32.i = load atomic i64, ptr %revision_pointer.i29.i acquire, align 8
  %stamp.i33.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i34.i = icmp eq i64 %revision.i32.i, %stamp.i33.i
  %enabled.i35.i = icmp ne i64 %revision.i32.i, 0
  %live.i36.i = and i1 %enabled.i35.i, %same_revision.i34.i
  %begin.i37.i = load i64, ptr @chali_address_view, align 16
  %size.i38.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i39.i = sub i64 %r23.i, %begin.i37.i
  %inside.i40.i = icmp ult i64 %offset.i39.i, %size.i38.i
  %remaining.i41.i = sub i64 %size.i38.i, %offset.i39.i
  %fits.i42.i = icmp ugt i64 %remaining.i41.i, 7
  %range.i43.i = and i1 %inside.i40.i, %fits.i42.i
  %valid.i44.i = and i1 %live.i36.i, %range.i43.i
  br i1 %valid.i44.i, label %fast.i48.i, label %slow.i45.i

fast.i48.i:                                       ; preds = %check.i31.i
  %base.i49.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i50.i = getelementptr i8, ptr %base.i49.i, i64 %offset.i39.i
  br label %chali_primitive_address.exit51.i

slow.i45.i:                                       ; preds = %check.i31.i, %chali_primitive_address.exit74.i
  %resolved.i46.i = call ptr @chali_host_address_view(i64 %r23.i, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit51.i

chali_primitive_address.exit51.i:                 ; preds = %slow.i45.i, %fast.i48.i
  %common.ret.op.i47.i = phi ptr [ %physical.i50.i, %fast.i48.i ], [ %resolved.i46.i, %slow.i45.i ]
  %r70.i = load i64, ptr %common.ret.op.i47.i, align 1
  store ptr @f_v462_std__lang__takeAt__int64, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 8, ptr @chali_source_line, align 8
  %r71.i = call i64 @chali_host_raw_storage(i64 2, i64 %r64.i, i64 %r70.i, i64 0, i64 0, i64 1)
  store i64 %4, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  store i64 0, ptr %r10.i, align 8
  store i8 0, ptr %r11.i, align 1
  store i64 1, ptr %r39.i, align 8
  store i8 0, ptr %r40.i, align 1
  %r82.i = call i64 @chali_host_call(ptr nonnull @native_name4, i64 3, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r83.i = call i8 @chali_host_result_float()
  %revision_pointer.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i = icmp eq ptr %revision_pointer.i.i, null
  br i1 %has_revision.not.i.i, label %slow.i.i, label %check.i.i

check.i.i:                                        ; preds = %chali_primitive_address.exit51.i
  %revision.i.i = load atomic i64, ptr %revision_pointer.i.i acquire, align 8
  %stamp.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i = icmp eq i64 %revision.i.i, %stamp.i.i
  %enabled.i.i = icmp ne i64 %revision.i.i, 0
  %live.i.i = and i1 %enabled.i.i, %same_revision.i.i
  %begin.i.i = load i64, ptr @chali_address_view, align 16
  %size.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i = sub i64 %4, %begin.i.i
  %inside.i.i = icmp ult i64 %offset.i.i, %size.i.i
  %remaining.i.i = sub i64 %size.i.i, %offset.i.i
  %fits.i.i = icmp ugt i64 %remaining.i.i, 7
  %range.i.i = and i1 %inside.i.i, %fits.i.i
  %valid.i.i = and i1 %live.i.i, %range.i.i
  br i1 %valid.i.i, label %chali_primitive_address.exit.i, label %slow.i.i

slow.i.i:                                         ; preds = %check.i.i, %chali_primitive_address.exit51.i
  %resolved.i.i = call ptr @chali_host_address_view(i64 %4, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit.i

chali_primitive_address.exit.i:                   ; preds = %check.i.i, %slow.i.i
  call void @chali_host_memory_prepare(i64 8, i64 %4, i64 0, i64 8, ptr nonnull %raw_memory_output.i, ptr nonnull %raw_memory_input.i)
  %r90.i = load ptr, ptr %raw_memory_output.i, align 8
  store i64 0, ptr %r90.i, align 1
  %r96.not.i = icmp eq i64 %1, 0
  br i1 %r96.not.i, label %ast_block_2.i, label %ast_block_0.i

ast_block_0.i:                                    ; preds = %chali_primitive_address.exit.i
  call void @chali_host_drop(i64 noundef %1) #27
  br label %ast_block_2.i

ast_block_2.i:                                    ; preds = %ast_block_0.i, %chali_primitive_address.exit.i
  %r103.not.i = icmp eq i64 %4, 0
  br i1 %r103.not.i, label %f_v462_std__lang__takeAt__int64.exit, label %ast_block_3.i

ast_block_3.i:                                    ; preds = %ast_block_2.i
  call void @chali_host_drop(i64 noundef %4) #27
  br label %f_v462_std__lang__takeAt__int64.exit

f_v462_std__lang__takeAt__int64.exit:             ; preds = %ast_block_2.i, %ast_block_3.i
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %raw_memory_output.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %raw_memory_input.i)
  %r20.not = icmp eq i64 %0, 0
  br i1 %r20.not, label %ast_block_2, label %ast_block_0

ast_block_0:                                      ; preds = %f_v462_std__lang__takeAt__int64.exit
  call void @chali_host_drop(i64 noundef %0) #27
  br label %ast_block_2

ast_block_2:                                      ; preds = %f_v462_std__lang__takeAt__int64.exit, %ast_block_0
  ret i64 0
}

; Function Attrs: alwaysinline uwtable
define internal i64 @f_v462_std__lang__takeAt__int64(i64 %p0) #21 section "chali_code" {
entry:
  %indirect_arguments = alloca [7 x i64], align 8
  %native_argument_types = alloca [7 x i8], align 1
  %raw_memory_output = alloca ptr, align 8
  %raw_memory_input = alloca ptr, align 8
  %0 = tail call i64 @chali_host_allocate(i64 noundef 16) #27
  %r5 = tail call i64 @chali_host_copy_words(i64 %0, i64 %p0, i64 2)
  store i64 %0, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r10 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  store i64 16, ptr %r10, align 8
  %r11 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r11, align 1
  %r12 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r13 = call i8 @chali_host_result_float()
  store ptr @f_v462_std__lang__takeAt__int64, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 5, ptr @chali_source_line, align 8
  %revision_pointer.i144 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i145 = icmp eq ptr %revision_pointer.i144, null
  br i1 %has_revision.not.i145, label %slow.i160, label %check.i146

check.i146:                                       ; preds = %entry
  %revision.i147 = load atomic i64, ptr %revision_pointer.i144 acquire, align 8
  %stamp.i148 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i149 = icmp eq i64 %revision.i147, %stamp.i148
  %enabled.i150 = icmp ne i64 %revision.i147, 0
  %live.i151 = and i1 %enabled.i150, %same_revision.i149
  %begin.i152 = load i64, ptr @chali_address_view, align 16
  %size.i153 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i154 = sub i64 %0, %begin.i152
  %inside.i155 = icmp ult i64 %offset.i154, %size.i153
  %remaining.i156 = sub i64 %size.i153, %offset.i154
  %fits.i157 = icmp ugt i64 %remaining.i156, 7
  %range.i158 = and i1 %inside.i155, %fits.i157
  %valid.i159 = and i1 %live.i151, %range.i158
  br i1 %valid.i159, label %fast.i163, label %slow.i160

fast.i163:                                        ; preds = %check.i146
  %base.i164 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i165 = getelementptr i8, ptr %base.i164, i64 %offset.i154
  br label %chali_primitive_address.exit166

slow.i160:                                        ; preds = %check.i146, %entry
  %resolved.i161 = call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit166

chali_primitive_address.exit166:                  ; preds = %fast.i163, %slow.i160
  %common.ret.op.i162 = phi ptr [ %physical.i165, %fast.i163 ], [ %resolved.i161, %slow.i160 ]
  %r19 = load i64, ptr %common.ret.op.i162, align 1
  %r23 = add i64 %0, 8
  store ptr @f_v462_std__lang__takeAt__int64, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 5, ptr @chali_source_line, align 8
  %revision_pointer.i121 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i122 = icmp eq ptr %revision_pointer.i121, null
  br i1 %has_revision.not.i122, label %slow.i137, label %check.i123

check.i123:                                       ; preds = %chali_primitive_address.exit166
  %revision.i124 = load atomic i64, ptr %revision_pointer.i121 acquire, align 8
  %stamp.i125 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i126 = icmp eq i64 %revision.i124, %stamp.i125
  %enabled.i127 = icmp ne i64 %revision.i124, 0
  %live.i128 = and i1 %enabled.i127, %same_revision.i126
  %begin.i129 = load i64, ptr @chali_address_view, align 16
  %size.i130 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i131 = sub i64 %r23, %begin.i129
  %inside.i132 = icmp ult i64 %offset.i131, %size.i130
  %remaining.i133 = sub i64 %size.i130, %offset.i131
  %fits.i134 = icmp ugt i64 %remaining.i133, 7
  %range.i135 = and i1 %inside.i132, %fits.i134
  %valid.i136 = and i1 %live.i128, %range.i135
  br i1 %valid.i136, label %fast.i140, label %slow.i137

fast.i140:                                        ; preds = %check.i123
  %base.i141 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i142 = getelementptr i8, ptr %base.i141, i64 %offset.i131
  br label %chali_primitive_address.exit143

slow.i137:                                        ; preds = %check.i123, %chali_primitive_address.exit166
  %resolved.i138 = call ptr @chali_host_address_view(i64 %r23, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit143

chali_primitive_address.exit143:                  ; preds = %fast.i140, %slow.i137
  %common.ret.op.i139 = phi ptr [ %physical.i142, %fast.i140 ], [ %resolved.i138, %slow.i137 ]
  %r25 = load i64, ptr %common.ret.op.i139, align 1
  store ptr @f_v462_std__lang__takeAt__int64, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 5, ptr @chali_source_line, align 8
  %r26 = call i64 @chali_host_raw_storage(i64 1, i64 %r19, i64 %r25, i64 1, i64 0, i64 1)
  %1 = icmp eq i64 %r26, 0
  br i1 %1, label %2, label %chali_require_reference.exit167

2:                                                ; preds = %chali_primitive_address.exit143
  call void @chali_panic(ptr noundef nonnull @.str.28)
  unreachable

chali_require_reference.exit167:                  ; preds = %chali_primitive_address.exit143
  store i64 %r26, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 0, ptr %r10, align 8
  store i8 0, ptr %r11, align 1
  %r39 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 16
  store i64 1, ptr %r39, align 8
  %r40 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r40, align 1
  %r41 = call i64 @chali_host_call(ptr nonnull @native_name4, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r42 = call i8 @chali_host_result_float()
  %revision_pointer.i98 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i99 = icmp eq ptr %revision_pointer.i98, null
  br i1 %has_revision.not.i99, label %slow.i114, label %check.i100

check.i100:                                       ; preds = %chali_require_reference.exit167
  %revision.i101 = load atomic i64, ptr %revision_pointer.i98 acquire, align 8
  %stamp.i102 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i103 = icmp eq i64 %revision.i101, %stamp.i102
  %enabled.i104 = icmp ne i64 %revision.i101, 0
  %live.i105 = and i1 %enabled.i104, %same_revision.i103
  %begin.i106 = load i64, ptr @chali_address_view, align 16
  %size.i107 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i108 = sub i64 %r26, %begin.i106
  %inside.i109 = icmp ult i64 %offset.i108, %size.i107
  %remaining.i110 = sub i64 %size.i107, %offset.i108
  %fits.i111 = icmp ugt i64 %remaining.i110, 7
  %range.i112 = and i1 %inside.i109, %fits.i111
  %valid.i113 = and i1 %live.i105, %range.i112
  br i1 %valid.i113, label %fast.i117, label %slow.i114

fast.i117:                                        ; preds = %check.i100
  %base.i118 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i119 = getelementptr i8, ptr %base.i118, i64 %offset.i108
  br label %chali_primitive_address.exit120

slow.i114:                                        ; preds = %check.i100, %chali_require_reference.exit167
  %resolved.i115 = call ptr @chali_host_address_view(i64 %r26, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit120

chali_primitive_address.exit120:                  ; preds = %fast.i117, %slow.i114
  %common.ret.op.i116 = phi ptr [ %physical.i119, %fast.i117 ], [ %resolved.i115, %slow.i114 ]
  %r46 = load i64, ptr %common.ret.op.i116, align 1
  call void @chali_host_memory_prepare(i64 8, i64 %r26, i64 0, i64 8, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r49 = load ptr, ptr %raw_memory_output, align 8
  store i64 0, ptr %r49, align 1
  %3 = call i64 @chali_host_allocate(i64 noundef 8) #27
  %revision_pointer.i75 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i76 = icmp eq ptr %revision_pointer.i75, null
  br i1 %has_revision.not.i76, label %slow.i91, label %check.i77

check.i77:                                        ; preds = %chali_primitive_address.exit120
  %revision.i78 = load atomic i64, ptr %revision_pointer.i75 acquire, align 8
  %stamp.i79 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i80 = icmp eq i64 %revision.i78, %stamp.i79
  %enabled.i81 = icmp ne i64 %revision.i78, 0
  %live.i82 = and i1 %enabled.i81, %same_revision.i80
  %begin.i83 = load i64, ptr @chali_address_view, align 16
  %size.i84 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i85 = sub i64 %3, %begin.i83
  %inside.i86 = icmp ult i64 %offset.i85, %size.i84
  %remaining.i87 = sub i64 %size.i84, %offset.i85
  %fits.i88 = icmp ugt i64 %remaining.i87, 7
  %range.i89 = and i1 %inside.i86, %fits.i88
  %valid.i90 = and i1 %live.i82, %range.i89
  br i1 %valid.i90, label %fast.i94, label %slow.i91

fast.i94:                                         ; preds = %check.i77
  %base.i95 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i96 = getelementptr i8, ptr %base.i95, i64 %offset.i85
  br label %chali_primitive_address.exit97

slow.i91:                                         ; preds = %check.i77, %chali_primitive_address.exit120
  %resolved.i92 = call ptr @chali_host_address_view(i64 %3, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit97

chali_primitive_address.exit97:                   ; preds = %fast.i94, %slow.i91
  %common.ret.op.i93 = phi ptr [ %physical.i96, %fast.i94 ], [ %resolved.i92, %slow.i91 ]
  store i64 %r46, ptr %common.ret.op.i93, align 1
  store ptr @f_v462_std__lang__takeAt__int64, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 8, ptr @chali_source_line, align 8
  %revision_pointer.i52 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i53 = icmp eq ptr %revision_pointer.i52, null
  br i1 %has_revision.not.i53, label %slow.i68, label %check.i54

check.i54:                                        ; preds = %chali_primitive_address.exit97
  %revision.i55 = load atomic i64, ptr %revision_pointer.i52 acquire, align 8
  %stamp.i56 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i57 = icmp eq i64 %revision.i55, %stamp.i56
  %enabled.i58 = icmp ne i64 %revision.i55, 0
  %live.i59 = and i1 %enabled.i58, %same_revision.i57
  %begin.i60 = load i64, ptr @chali_address_view, align 16
  %size.i61 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i62 = sub i64 %0, %begin.i60
  %inside.i63 = icmp ult i64 %offset.i62, %size.i61
  %remaining.i64 = sub i64 %size.i61, %offset.i62
  %fits.i65 = icmp ugt i64 %remaining.i64, 7
  %range.i66 = and i1 %inside.i63, %fits.i65
  %valid.i67 = and i1 %live.i59, %range.i66
  br i1 %valid.i67, label %fast.i71, label %slow.i68

fast.i71:                                         ; preds = %check.i54
  %base.i72 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i73 = getelementptr i8, ptr %base.i72, i64 %offset.i62
  br label %chali_primitive_address.exit74

slow.i68:                                         ; preds = %check.i54, %chali_primitive_address.exit97
  %resolved.i69 = call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit74

chali_primitive_address.exit74:                   ; preds = %fast.i71, %slow.i68
  %common.ret.op.i70 = phi ptr [ %physical.i73, %fast.i71 ], [ %resolved.i69, %slow.i68 ]
  %r64 = load i64, ptr %common.ret.op.i70, align 1
  store ptr @f_v462_std__lang__takeAt__int64, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 8, ptr @chali_source_line, align 8
  %revision_pointer.i29 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i30 = icmp eq ptr %revision_pointer.i29, null
  br i1 %has_revision.not.i30, label %slow.i45, label %check.i31

check.i31:                                        ; preds = %chali_primitive_address.exit74
  %revision.i32 = load atomic i64, ptr %revision_pointer.i29 acquire, align 8
  %stamp.i33 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i34 = icmp eq i64 %revision.i32, %stamp.i33
  %enabled.i35 = icmp ne i64 %revision.i32, 0
  %live.i36 = and i1 %enabled.i35, %same_revision.i34
  %begin.i37 = load i64, ptr @chali_address_view, align 16
  %size.i38 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i39 = sub i64 %r23, %begin.i37
  %inside.i40 = icmp ult i64 %offset.i39, %size.i38
  %remaining.i41 = sub i64 %size.i38, %offset.i39
  %fits.i42 = icmp ugt i64 %remaining.i41, 7
  %range.i43 = and i1 %inside.i40, %fits.i42
  %valid.i44 = and i1 %live.i36, %range.i43
  br i1 %valid.i44, label %fast.i48, label %slow.i45

fast.i48:                                         ; preds = %check.i31
  %base.i49 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i50 = getelementptr i8, ptr %base.i49, i64 %offset.i39
  br label %chali_primitive_address.exit51

slow.i45:                                         ; preds = %check.i31, %chali_primitive_address.exit74
  %resolved.i46 = call ptr @chali_host_address_view(i64 %r23, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit51

chali_primitive_address.exit51:                   ; preds = %fast.i48, %slow.i45
  %common.ret.op.i47 = phi ptr [ %physical.i50, %fast.i48 ], [ %resolved.i46, %slow.i45 ]
  %r70 = load i64, ptr %common.ret.op.i47, align 1
  store ptr @f_v462_std__lang__takeAt__int64, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 8, ptr @chali_source_line, align 8
  %r71 = call i64 @chali_host_raw_storage(i64 2, i64 %r64, i64 %r70, i64 0, i64 0, i64 1)
  store i64 %3, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 0, ptr %r10, align 8
  store i8 0, ptr %r11, align 1
  store i64 1, ptr %r39, align 8
  store i8 0, ptr %r40, align 1
  %r82 = call i64 @chali_host_call(ptr nonnull @native_name4, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r83 = call i8 @chali_host_result_float()
  %revision_pointer.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i = icmp eq ptr %revision_pointer.i, null
  br i1 %has_revision.not.i, label %slow.i, label %check.i

check.i:                                          ; preds = %chali_primitive_address.exit51
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
  br label %chali_primitive_address.exit

slow.i:                                           ; preds = %check.i, %chali_primitive_address.exit51
  %resolved.i = call ptr @chali_host_address_view(i64 %3, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit

chali_primitive_address.exit:                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  %r87 = load i64, ptr %common.ret.op.i, align 1
  call void @chali_host_memory_prepare(i64 8, i64 %3, i64 0, i64 8, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r90 = load ptr, ptr %raw_memory_output, align 8
  store i64 0, ptr %r90, align 1
  %r96.not = icmp eq i64 %0, 0
  br i1 %r96.not, label %ast_block_2, label %ast_block_0

ast_block_0:                                      ; preds = %chali_primitive_address.exit
  call void @chali_host_drop(i64 noundef %0) #27
  br label %ast_block_2

ast_block_2:                                      ; preds = %chali_primitive_address.exit, %ast_block_0
  %r103.not = icmp eq i64 %3, 0
  br i1 %r103.not, label %ast_block_5, label %ast_block_3

ast_block_3:                                      ; preds = %ast_block_2
  call void @chali_host_drop(i64 noundef %3) #27
  br label %ast_block_5

ast_block_5:                                      ; preds = %ast_block_2, %ast_block_3
  ret i64 %r87
}

; Function Attrs: uwtable
define internal noundef i64 @f_v463_RawStorage__int64_allocate(i64 %p0) #24 section "chali_code" {
entry:
  %indirect_arguments = alloca [7 x i64], align 8
  %native_argument_types = alloca [7 x i8], align 1
  %raw_memory_output = alloca ptr, align 8
  %raw_memory_input = alloca ptr, align 8
  %0 = tail call i64 @chali_host_allocate(i64 noundef 8) #27
  call void @chali_host_memory_prepare(i64 8, i64 %0, i64 0, i64 8, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r3 = load ptr, ptr %raw_memory_output, align 8
  store i64 0, ptr %r3, align 1
  store i64 %0, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r9 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  store i64 8, ptr %r9, align 8
  %r10 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r10, align 1
  %r11 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r12 = call i8 @chali_host_result_float()
  store ptr @f_v463_RawStorage__int64_allocate, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %r15 = call i64 @chali_host_raw_storage(i64 0, i64 0, i64 %p0, i64 8, i64 8, i64 1)
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
  store i64 %r15, ptr %common.ret.op.i, align 1
  store i64 40, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r9, align 8
  store i8 0, ptr %r10, align 1
  %r27 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r28 = call i8 @chali_host_result_float()
  store i64 %0, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r9, align 8
  store i8 0, ptr %r10, align 1
  %r37 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 16
  store i64 44, ptr %r37, align 8
  %r38 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 2
  store i8 0, ptr %r38, align 1
  %r39 = call i64 @chali_host_call(ptr nonnull @native_name4, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r40 = call i8 @chali_host_result_float()
  %1 = call i64 @chali_host_allocate(i64 noundef 8) #27
  call void @chali_host_memory_prepare(i64 8, i64 %1, i64 0, i64 8, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r44 = load ptr, ptr %raw_memory_output, align 8
  store i64 0, ptr %r44, align 1
  store i64 %1, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r9, align 8
  store i8 0, ptr %r10, align 1
  %r52 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r53 = call i8 @chali_host_result_float()
  %r58 = call i64 @chali_host_copy_words(i64 %1, i64 %0, i64 1)
  store i64 %1, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r9, align 8
  store i8 0, ptr %r10, align 1
  %r65 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r66 = call i8 @chali_host_result_float()
  call void @chali_host_memory_prepare(i64 8, i64 %0, i64 0, i64 8, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r69 = load ptr, ptr %raw_memory_output, align 8
  store i64 0, ptr %r69, align 1
  store i64 %0, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r9, align 8
  store i8 0, ptr %r10, align 1
  store i64 44, ptr %r37, align 8
  store i8 0, ptr %r38, align 1
  %r79 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r80 = call i8 @chali_host_result_float()
  %r87 = call i64 @chali_host_copy_words(i64 40, i64 %1, i64 1)
  store i64 40, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r9, align 8
  store i8 0, ptr %r10, align 1
  %r94 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r95 = call i8 @chali_host_result_float()
  %r100.not = icmp eq i64 %1, 40
  br i1 %r100.not, label %ast_block_2, label %ast_block_0

ast_block_0:                                      ; preds = %chali_primitive_address.exit
  store i64 %1, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r9, align 8
  store i8 0, ptr %r10, align 1
  store i64 44, ptr %r37, align 8
  store i8 0, ptr %r38, align 1
  %r111 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r112 = call i8 @chali_host_result_float()
  br label %ast_block_2

ast_block_2:                                      ; preds = %chali_primitive_address.exit, %ast_block_0
  store i64 %0, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r9, align 8
  store i8 0, ptr %r10, align 1
  store i64 44, ptr %r37, align 8
  store i8 0, ptr %r38, align 1
  %r121 = call i64 @chali_host_call(ptr nonnull @native_name2, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r122 = call i8 @chali_host_result_float()
  %r123.not = icmp eq i64 %r121, 0
  br i1 %r123.not, label %ast_block_5, label %ast_block_3

ast_block_3:                                      ; preds = %ast_block_2
  store i64 %0, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 8, ptr %r9, align 8
  store i8 0, ptr %r10, align 1
  store i64 44, ptr %r37, align 8
  store i8 0, ptr %r38, align 1
  %r132 = call i64 @chali_host_call(ptr nonnull @native_name2, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r133 = call i8 @chali_host_result_float()
  %r134.not = icmp eq i64 %r132, 0
  br i1 %r134.not, label %ast_block_5, label %ast_block_6

ast_block_6:                                      ; preds = %ast_block_3
  %r137 = call i64 @f_v465_RawStorage__int64_close(i64 %0)
  br label %ast_block_5

ast_block_5:                                      ; preds = %ast_block_2, %ast_block_6, %ast_block_3
  %r141.not = icmp eq i64 %1, 0
  br i1 %r141.not, label %ast_block_11, label %ast_block_9

ast_block_9:                                      ; preds = %ast_block_5
  call void @chali_host_drop(i64 noundef %1) #27
  br label %ast_block_11

ast_block_11:                                     ; preds = %ast_block_5, %ast_block_9
  %r148.not = icmp eq i64 %0, 0
  br i1 %r148.not, label %ast_block_14, label %ast_block_12

ast_block_12:                                     ; preds = %ast_block_11
  call void @chali_host_drop(i64 noundef %0) #27
  br label %ast_block_14

ast_block_14:                                     ; preds = %ast_block_11, %ast_block_12
  ret i64 40
}

; Function Attrs: alwaysinline uwtable
define internal noundef i64 @f_v464_RawStorage__int64_pointer(i64 %p0, i64 %p1) #21 section "chali_code" {
entry:
  %indirect_arguments.i = alloca [2 x i64], align 8
  %native_argument_types.i = alloca [2 x i8], align 1
  %indirect_arguments = alloca [4 x i64], align 8
  %native_argument_types = alloca [4 x i8], align 1
  %raw_memory_output = alloca ptr, align 8
  %raw_memory_input = alloca ptr, align 8
  store ptr @f_v464_RawStorage__int64_pointer, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %revision_pointer.i73 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i74 = icmp eq ptr %revision_pointer.i73, null
  br i1 %has_revision.not.i74, label %slow.i89, label %check.i75

check.i75:                                        ; preds = %entry
  %revision.i76 = load atomic i64, ptr %revision_pointer.i73 acquire, align 8
  %stamp.i77 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i78 = icmp eq i64 %revision.i76, %stamp.i77
  %enabled.i79 = icmp ne i64 %revision.i76, 0
  %live.i80 = and i1 %enabled.i79, %same_revision.i78
  %begin.i81 = load i64, ptr @chali_address_view, align 16
  %size.i82 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i83 = sub i64 %p0, %begin.i81
  %inside.i84 = icmp ult i64 %offset.i83, %size.i82
  %remaining.i85 = sub i64 %size.i82, %offset.i83
  %fits.i86 = icmp ugt i64 %remaining.i85, 7
  %range.i87 = and i1 %inside.i84, %fits.i86
  %valid.i88 = and i1 %live.i80, %range.i87
  br i1 %valid.i88, label %fast.i92, label %slow.i89

fast.i92:                                         ; preds = %check.i75
  %base.i93 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i94 = getelementptr i8, ptr %base.i93, i64 %offset.i83
  br label %chali_primitive_address.exit95

slow.i89:                                         ; preds = %check.i75, %entry
  %resolved.i90 = tail call ptr @chali_host_address_view(i64 %p0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit95

chali_primitive_address.exit95:                   ; preds = %fast.i92, %slow.i89
  %common.ret.op.i91 = phi ptr [ %physical.i94, %fast.i92 ], [ %resolved.i90, %slow.i89 ]
  %r5 = load i64, ptr %common.ret.op.i91, align 1
  %r6.not = icmp ne i64 %r5, 0
  %r11 = icmp sgt i64 %p1, -1
  %or.cond = select i1 %r6.not, i1 %r11, i1 false
  br i1 %or.cond, label %ast_block_5, label %ast_block_6.thread

ast_block_5:                                      ; preds = %chali_primitive_address.exit95
  store ptr @f_v466_RawStorage__int64_capacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %revision_pointer.i3.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i4.i = icmp eq ptr %revision_pointer.i3.i, null
  br i1 %has_revision.not.i4.i, label %slow.i19.i, label %check.i5.i

check.i5.i:                                       ; preds = %ast_block_5
  %revision.i6.i = load atomic i64, ptr %revision_pointer.i3.i acquire, align 8
  %stamp.i7.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i8.i = icmp eq i64 %revision.i6.i, %stamp.i7.i
  %enabled.i9.i = icmp ne i64 %revision.i6.i, 0
  %live.i10.i = and i1 %enabled.i9.i, %same_revision.i8.i
  %begin.i11.i = load i64, ptr @chali_address_view, align 16
  %size.i12.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i13.i = sub i64 %p0, %begin.i11.i
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

slow.i19.i:                                       ; preds = %check.i5.i, %ast_block_5
  %resolved.i20.i = tail call ptr @chali_host_address_view(i64 %p0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit25.i

chali_primitive_address.exit25.i:                 ; preds = %slow.i19.i, %fast.i22.i
  %common.ret.op.i21.i = phi ptr [ %physical.i24.i, %fast.i22.i ], [ %resolved.i20.i, %slow.i19.i ]
  %r5.i = load i64, ptr %common.ret.op.i21.i, align 1
  %r6.i = icmp eq i64 %r5.i, 0
  br i1 %r6.i, label %ast_block_6, label %ast_block_2.i

ast_block_2.i:                                    ; preds = %chali_primitive_address.exit25.i
  store ptr @f_v466_RawStorage__int64_capacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %revision_pointer.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i = icmp eq ptr %revision_pointer.i.i, null
  br i1 %has_revision.not.i.i, label %slow.i.i, label %check.i.i

check.i.i:                                        ; preds = %ast_block_2.i
  %revision.i.i = load atomic i64, ptr %revision_pointer.i.i acquire, align 8
  %stamp.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i = icmp eq i64 %revision.i.i, %stamp.i.i
  %enabled.i.i = icmp ne i64 %revision.i.i, 0
  %live.i.i = and i1 %enabled.i.i, %same_revision.i.i
  %begin.i.i = load i64, ptr @chali_address_view, align 16
  %size.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i = sub i64 %p0, %begin.i.i
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

slow.i.i:                                         ; preds = %check.i.i, %ast_block_2.i
  %resolved.i.i = tail call ptr @chali_host_address_view(i64 %p0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit.i

chali_primitive_address.exit.i:                   ; preds = %slow.i.i, %fast.i.i
  %common.ret.op.i.i = phi ptr [ %physical.i.i, %fast.i.i ], [ %resolved.i.i, %slow.i.i ]
  %r16.i = load i64, ptr %common.ret.op.i.i, align 1
  store ptr @f_v466_RawStorage__int64_capacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %r17.i = tail call i64 @chali_host_raw_storage(i64 4, i64 %r16.i, i64 0, i64 0, i64 0, i64 1)
  br label %ast_block_6

ast_block_6.thread:                               ; preds = %chali_primitive_address.exit95
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i)
  br label %ast_block_0.i

ast_block_6:                                      ; preds = %chali_primitive_address.exit.i, %chali_primitive_address.exit25.i
  %common.ret.op.i96 = phi i64 [ %r17.i, %chali_primitive_address.exit.i ], [ 0, %chali_primitive_address.exit25.i ]
  %r23.not = icmp sgt i64 %p1, %common.ret.op.i96
  store ptr @f_v464_RawStorage__int64_pointer, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i)
  br i1 %r23.not, label %ast_block_0.i, label %f_v162_std__lang__assert_bool.exit

ast_block_0.i:                                    ; preds = %ast_block_6.thread, %ast_block_6
  store ptr @f_v162_std__lang__assert_bool, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 782, ptr @chali_source_line, align 8
  store i64 632, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  %r8.i = call i64 @chali_host_call(ptr nonnull @native_name3, i64 1, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r9.i = call i8 @chali_host_result_float()
  br label %f_v162_std__lang__assert_bool.exit

f_v162_std__lang__assert_bool.exit:               ; preds = %ast_block_6, %ast_block_0.i
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i)
  %0 = call i64 @chali_host_allocate(i64 noundef 16) #27
  call void @chali_host_memory_prepare(i64 8, i64 %0, i64 0, i64 16, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r33 = load ptr, ptr %raw_memory_output, align 8
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 1 dereferenceable(16) %r33, i8 0, i64 16, i1 false)
  store i64 %0, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  %r39 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  store i64 16, ptr %r39, align 8
  %r40 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  store i8 0, ptr %r40, align 1
  %r41 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r42 = call i8 @chali_host_result_float()
  store ptr @f_v464_RawStorage__int64_pointer, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %revision_pointer.i50 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i51 = icmp eq ptr %revision_pointer.i50, null
  br i1 %has_revision.not.i51, label %slow.i66, label %check.i52

check.i52:                                        ; preds = %f_v162_std__lang__assert_bool.exit
  %revision.i53 = load atomic i64, ptr %revision_pointer.i50 acquire, align 8
  %stamp.i54 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i55 = icmp eq i64 %revision.i53, %stamp.i54
  %enabled.i56 = icmp ne i64 %revision.i53, 0
  %live.i57 = and i1 %enabled.i56, %same_revision.i55
  %begin.i58 = load i64, ptr @chali_address_view, align 16
  %size.i59 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i60 = sub i64 %p0, %begin.i58
  %inside.i61 = icmp ult i64 %offset.i60, %size.i59
  %remaining.i62 = sub i64 %size.i59, %offset.i60
  %fits.i63 = icmp ugt i64 %remaining.i62, 7
  %range.i64 = and i1 %inside.i61, %fits.i63
  %valid.i65 = and i1 %live.i57, %range.i64
  br i1 %valid.i65, label %fast.i69, label %slow.i66

fast.i69:                                         ; preds = %check.i52
  %base.i70 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i71 = getelementptr i8, ptr %base.i70, i64 %offset.i60
  br label %chali_primitive_address.exit72

slow.i66:                                         ; preds = %check.i52, %f_v162_std__lang__assert_bool.exit
  %resolved.i67 = call ptr @chali_host_address_view(i64 %p0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit72

chali_primitive_address.exit72:                   ; preds = %fast.i69, %slow.i66
  %common.ret.op.i68 = phi ptr [ %physical.i71, %fast.i69 ], [ %resolved.i67, %slow.i66 ]
  %r48 = load i64, ptr %common.ret.op.i68, align 1
  %revision_pointer.i27 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i28 = icmp eq ptr %revision_pointer.i27, null
  br i1 %has_revision.not.i28, label %slow.i43, label %check.i29

check.i29:                                        ; preds = %chali_primitive_address.exit72
  %revision.i30 = load atomic i64, ptr %revision_pointer.i27 acquire, align 8
  %stamp.i31 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i32 = icmp eq i64 %revision.i30, %stamp.i31
  %enabled.i33 = icmp ne i64 %revision.i30, 0
  %live.i34 = and i1 %enabled.i33, %same_revision.i32
  %begin.i35 = load i64, ptr @chali_address_view, align 16
  %size.i36 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i37 = sub i64 %0, %begin.i35
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

slow.i43:                                         ; preds = %check.i29, %chali_primitive_address.exit72
  %resolved.i44 = call ptr @chali_host_address_view(i64 %0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit49

chali_primitive_address.exit49:                   ; preds = %fast.i46, %slow.i43
  %common.ret.op.i45 = phi ptr [ %physical.i48, %fast.i46 ], [ %resolved.i44, %slow.i43 ]
  store i64 %r48, ptr %common.ret.op.i45, align 1
  %r59 = add i64 %0, 8
  %revision_pointer.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i = icmp eq ptr %revision_pointer.i, null
  br i1 %has_revision.not.i, label %slow.i, label %check.i

check.i:                                          ; preds = %chali_primitive_address.exit49
  %revision.i = load atomic i64, ptr %revision_pointer.i acquire, align 8
  %stamp.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i = icmp eq i64 %revision.i, %stamp.i
  %enabled.i = icmp ne i64 %revision.i, 0
  %live.i = and i1 %enabled.i, %same_revision.i
  %begin.i = load i64, ptr @chali_address_view, align 16
  %size.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i = sub i64 %r59, %begin.i
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

slow.i:                                           ; preds = %check.i, %chali_primitive_address.exit49
  %resolved.i = call ptr @chali_host_address_view(i64 %r59, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit

chali_primitive_address.exit:                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  store i64 %p1, ptr %common.ret.op.i, align 1
  store i64 48, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 16, ptr %r39, align 8
  store i8 0, ptr %r40, align 1
  %r67 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r68 = call i8 @chali_host_result_float()
  %r73 = call i64 @chali_host_copy_words(i64 48, i64 %0, i64 2)
  store i64 48, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 16, ptr %r39, align 8
  store i8 0, ptr %r40, align 1
  %r80 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r81 = call i8 @chali_host_result_float()
  %r84.not = icmp eq i64 %0, 0
  br i1 %r84.not, label %ast_block_10, label %ast_block_8

ast_block_8:                                      ; preds = %chali_primitive_address.exit
  call void @chali_host_drop(i64 noundef %0) #27
  br label %ast_block_10

ast_block_10:                                     ; preds = %chali_primitive_address.exit, %ast_block_8
  ret i64 48
}

; Function Attrs: uwtable
define internal noundef i64 @f_v465_RawStorage__int64_close(i64 %p0) #24 section "chali_code" {
entry:
  %indirect_arguments.i.i = alloca [2 x i64], align 8
  %native_argument_types.i.i = alloca [2 x i8], align 1
  %indirect_arguments.i = alloca [4 x i64], align 8
  %native_argument_types.i = alloca [4 x i8], align 1
  %raw_memory_output.i = alloca ptr, align 8
  %raw_memory_input.i = alloca ptr, align 8
  %indirect_arguments = alloca [7 x i64], align 8
  %native_argument_types = alloca [7 x i8], align 1
  %raw_memory_output = alloca ptr, align 8
  %raw_memory_input = alloca ptr, align 8
  store ptr @f_v465_RawStorage__int64_close, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %revision_pointer.i129 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i130 = icmp eq ptr %revision_pointer.i129, null
  br i1 %has_revision.not.i130, label %slow.i145, label %check.i131

check.i131:                                       ; preds = %entry
  %revision.i132 = load atomic i64, ptr %revision_pointer.i129 acquire, align 8
  %stamp.i133 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i134 = icmp eq i64 %revision.i132, %stamp.i133
  %enabled.i135 = icmp ne i64 %revision.i132, 0
  %live.i136 = and i1 %enabled.i135, %same_revision.i134
  %begin.i137 = load i64, ptr @chali_address_view, align 16
  %size.i138 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i139 = sub i64 %p0, %begin.i137
  %inside.i140 = icmp ult i64 %offset.i139, %size.i138
  %remaining.i141 = sub i64 %size.i138, %offset.i139
  %fits.i142 = icmp ugt i64 %remaining.i141, 7
  %range.i143 = and i1 %inside.i140, %fits.i142
  %valid.i144 = and i1 %live.i136, %range.i143
  br i1 %valid.i144, label %fast.i148, label %slow.i145

fast.i148:                                        ; preds = %check.i131
  %base.i149 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i150 = getelementptr i8, ptr %base.i149, i64 %offset.i139
  br label %chali_primitive_address.exit151

slow.i145:                                        ; preds = %check.i131, %entry
  %resolved.i146 = tail call ptr @chali_host_address_view(i64 %p0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit151

chali_primitive_address.exit151:                  ; preds = %fast.i148, %slow.i145
  %common.ret.op.i147 = phi ptr [ %physical.i150, %fast.i148 ], [ %resolved.i146, %slow.i145 ]
  %r5 = load i64, ptr %common.ret.op.i147, align 1
  store ptr @f_v465_RawStorage__int64_close, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %r6 = icmp eq i64 %r5, 0
  br i1 %r6, label %common.ret, label %ast_block_2

common.ret:                                       ; preds = %chali_primitive_address.exit151, %chali_primitive_address.exit
  ret i64 0

ast_block_2:                                      ; preds = %chali_primitive_address.exit151
  store ptr @source_file0, ptr @chali_source_file, align 8
  %revision_pointer.i106 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i107 = icmp eq ptr %revision_pointer.i106, null
  br i1 %has_revision.not.i107, label %slow.i122, label %check.i108

check.i108:                                       ; preds = %ast_block_2
  %revision.i109 = load atomic i64, ptr %revision_pointer.i106 acquire, align 8
  %stamp.i110 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i111 = icmp eq i64 %revision.i109, %stamp.i110
  %enabled.i112 = icmp ne i64 %revision.i109, 0
  %live.i113 = and i1 %enabled.i112, %same_revision.i111
  %begin.i114 = load i64, ptr @chali_address_view, align 16
  %size.i115 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i116 = sub i64 %p0, %begin.i114
  %inside.i117 = icmp ult i64 %offset.i116, %size.i115
  %remaining.i118 = sub i64 %size.i115, %offset.i116
  %fits.i119 = icmp ugt i64 %remaining.i118, 7
  %range.i120 = and i1 %inside.i117, %fits.i119
  %valid.i121 = and i1 %live.i113, %range.i120
  br i1 %valid.i121, label %fast.i125, label %slow.i122

fast.i125:                                        ; preds = %check.i108
  %base.i126 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i127 = getelementptr i8, ptr %base.i126, i64 %offset.i116
  br label %chali_primitive_address.exit128

slow.i122:                                        ; preds = %check.i108, %ast_block_2
  %resolved.i123 = tail call ptr @chali_host_address_view(i64 %p0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit128

chali_primitive_address.exit128:                  ; preds = %fast.i125, %slow.i122
  %common.ret.op.i124 = phi ptr [ %physical.i127, %fast.i125 ], [ %resolved.i123, %slow.i122 ]
  %r16 = load i64, ptr %common.ret.op.i124, align 1
  store ptr @f_v465_RawStorage__int64_close, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %r17 = tail call i64 @chali_host_raw_storage(i64 4, i64 %r16, i64 0, i64 0, i64 0, i64 1)
  store ptr @f_v465_RawStorage__int64_close, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %revision_pointer.i83 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i84 = icmp eq ptr %revision_pointer.i83, null
  br i1 %has_revision.not.i84, label %slow.i99, label %check.i85

check.i85:                                        ; preds = %chali_primitive_address.exit128
  %revision.i86 = load atomic i64, ptr %revision_pointer.i83 acquire, align 8
  %stamp.i87 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i88 = icmp eq i64 %revision.i86, %stamp.i87
  %enabled.i89 = icmp ne i64 %revision.i86, 0
  %live.i90 = and i1 %enabled.i89, %same_revision.i88
  %begin.i91 = load i64, ptr @chali_address_view, align 16
  %size.i92 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i93 = sub i64 %p0, %begin.i91
  %inside.i94 = icmp ult i64 %offset.i93, %size.i92
  %remaining.i95 = sub i64 %size.i92, %offset.i93
  %fits.i96 = icmp ugt i64 %remaining.i95, 7
  %range.i97 = and i1 %inside.i94, %fits.i96
  %valid.i98 = and i1 %live.i90, %range.i97
  br i1 %valid.i98, label %fast.i102, label %slow.i99

fast.i102:                                        ; preds = %check.i85
  %base.i103 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i104 = getelementptr i8, ptr %base.i103, i64 %offset.i93
  br label %chali_primitive_address.exit105

slow.i99:                                         ; preds = %check.i85, %chali_primitive_address.exit128
  %resolved.i100 = tail call ptr @chali_host_address_view(i64 %p0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit105

chali_primitive_address.exit105:                  ; preds = %fast.i102, %slow.i99
  %common.ret.op.i101 = phi ptr [ %physical.i104, %fast.i102 ], [ %resolved.i100, %slow.i99 ]
  %r23 = load i64, ptr %common.ret.op.i101, align 1
  store ptr @f_v465_RawStorage__int64_close, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %r24 = tail call i64 @chali_host_raw_storage(i64 9, i64 %r23, i64 0, i64 0, i64 0, i64 1)
  %r25.not = icmp ne i64 %r24, 0
  %r32179 = icmp sgt i64 %r17, 0
  %or.cond = select i1 %r25.not, i1 %r32179, i1 false
  br i1 %or.cond, label %ast_block_6.lr.ph, label %ast_block_5

ast_block_6.lr.ph:                                ; preds = %chali_primitive_address.exit105
  %r56 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 8
  %r57 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 1
  %r39.i = getelementptr inbounds nuw i8, ptr %indirect_arguments.i, i64 8
  %r40.i = getelementptr inbounds nuw i8, ptr %native_argument_types.i, i64 1
  %r92 = getelementptr inbounds nuw i8, ptr %indirect_arguments, i64 16
  %r93 = getelementptr inbounds nuw i8, ptr %native_argument_types, i64 2
  br label %ast_block_6

ast_block_6:                                      ; preds = %ast_block_6.lr.ph, %ast_block_11
  %s_v794___ast_465_i_2242.0180 = phi i64 [ 0, %ast_block_6.lr.ph ], [ %r109, %ast_block_11 ]
  store ptr @f_v465_RawStorage__int64_close, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %revision_pointer.i60 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i61 = icmp eq ptr %revision_pointer.i60, null
  br i1 %has_revision.not.i61, label %slow.i76, label %check.i62

check.i62:                                        ; preds = %ast_block_6
  %revision.i63 = load atomic i64, ptr %revision_pointer.i60 acquire, align 8
  %stamp.i64 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i65 = icmp eq i64 %revision.i63, %stamp.i64
  %enabled.i66 = icmp ne i64 %revision.i63, 0
  %live.i67 = and i1 %enabled.i66, %same_revision.i65
  %begin.i68 = load i64, ptr @chali_address_view, align 16
  %size.i69 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i70 = sub i64 %p0, %begin.i68
  %inside.i71 = icmp ult i64 %offset.i70, %size.i69
  %remaining.i72 = sub i64 %size.i69, %offset.i70
  %fits.i73 = icmp ugt i64 %remaining.i72, 7
  %range.i74 = and i1 %inside.i71, %fits.i73
  %valid.i75 = and i1 %live.i67, %range.i74
  br i1 %valid.i75, label %fast.i79, label %slow.i76

fast.i79:                                         ; preds = %check.i62
  %base.i80 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i81 = getelementptr i8, ptr %base.i80, i64 %offset.i70
  br label %chali_primitive_address.exit82

slow.i76:                                         ; preds = %check.i62, %ast_block_6
  %resolved.i77 = call ptr @chali_host_address_view(i64 %p0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit82

chali_primitive_address.exit82:                   ; preds = %fast.i79, %slow.i76
  %common.ret.op.i78 = phi ptr [ %physical.i81, %fast.i79 ], [ %resolved.i77, %slow.i76 ]
  %r40 = load i64, ptr %common.ret.op.i78, align 1
  store ptr @f_v465_RawStorage__int64_close, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %r43 = call i64 @chali_host_raw_storage(i64 3, i64 %r40, i64 %s_v794___ast_465_i_2242.0180, i64 0, i64 0, i64 1)
  store ptr @f_v465_RawStorage__int64_close, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %r44.not = icmp eq i64 %r43, 0
  br i1 %r44.not, label %ast_block_11, label %ast_block_9

ast_block_9:                                      ; preds = %chali_primitive_address.exit82
  %0 = call i64 @chali_host_allocate(i64 noundef 16) #27
  call void @chali_host_memory_prepare(i64 8, i64 %0, i64 0, i64 16, ptr nonnull %raw_memory_output, ptr nonnull %raw_memory_input)
  %r50 = load ptr, ptr %raw_memory_output, align 8
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 1 dereferenceable(16) %r50, i8 0, i64 16, i1 false)
  store i64 %0, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 16, ptr %r56, align 8
  store i8 0, ptr %r57, align 1
  %r58 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r59 = call i8 @chali_host_result_float()
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %raw_memory_output.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %raw_memory_input.i)
  store ptr @f_v464_RawStorage__int64_pointer, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %revision_pointer.i73.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i74.i = icmp eq ptr %revision_pointer.i73.i, null
  br i1 %has_revision.not.i74.i, label %slow.i89.i, label %check.i75.i

check.i75.i:                                      ; preds = %ast_block_9
  %revision.i76.i = load atomic i64, ptr %revision_pointer.i73.i acquire, align 8
  %stamp.i77.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i78.i = icmp eq i64 %revision.i76.i, %stamp.i77.i
  %enabled.i79.i = icmp ne i64 %revision.i76.i, 0
  %live.i80.i = and i1 %enabled.i79.i, %same_revision.i78.i
  %begin.i81.i = load i64, ptr @chali_address_view, align 16
  %size.i82.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i83.i = sub i64 %p0, %begin.i81.i
  %inside.i84.i = icmp ult i64 %offset.i83.i, %size.i82.i
  %remaining.i85.i = sub i64 %size.i82.i, %offset.i83.i
  %fits.i86.i = icmp ugt i64 %remaining.i85.i, 7
  %range.i87.i = and i1 %inside.i84.i, %fits.i86.i
  %valid.i88.i = and i1 %live.i80.i, %range.i87.i
  br i1 %valid.i88.i, label %fast.i92.i, label %slow.i89.i

fast.i92.i:                                       ; preds = %check.i75.i
  %base.i93.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i94.i = getelementptr i8, ptr %base.i93.i, i64 %offset.i83.i
  br label %chali_primitive_address.exit95.i

slow.i89.i:                                       ; preds = %check.i75.i, %ast_block_9
  %resolved.i90.i = call ptr @chali_host_address_view(i64 %p0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit95.i

chali_primitive_address.exit95.i:                 ; preds = %slow.i89.i, %fast.i92.i
  %common.ret.op.i91.i = phi ptr [ %physical.i94.i, %fast.i92.i ], [ %resolved.i90.i, %slow.i89.i ]
  %r5.i = load i64, ptr %common.ret.op.i91.i, align 1
  %r6.not.i = icmp eq i64 %r5.i, 0
  br i1 %r6.not.i, label %ast_block_6.i.thread, label %ast_block_5.i

ast_block_5.i:                                    ; preds = %chali_primitive_address.exit95.i
  store ptr @f_v466_RawStorage__int64_capacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %revision_pointer.i3.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i4.i = icmp eq ptr %revision_pointer.i3.i, null
  br i1 %has_revision.not.i4.i, label %slow.i19.i, label %check.i5.i

check.i5.i:                                       ; preds = %ast_block_5.i
  %revision.i6.i = load atomic i64, ptr %revision_pointer.i3.i acquire, align 8
  %stamp.i7.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i8.i = icmp eq i64 %revision.i6.i, %stamp.i7.i
  %enabled.i9.i = icmp ne i64 %revision.i6.i, 0
  %live.i10.i = and i1 %enabled.i9.i, %same_revision.i8.i
  %begin.i11.i = load i64, ptr @chali_address_view, align 16
  %size.i12.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i13.i = sub i64 %p0, %begin.i11.i
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

slow.i19.i:                                       ; preds = %check.i5.i, %ast_block_5.i
  %resolved.i20.i = call ptr @chali_host_address_view(i64 %p0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit25.i

chali_primitive_address.exit25.i:                 ; preds = %slow.i19.i, %fast.i22.i
  %common.ret.op.i21.i = phi ptr [ %physical.i24.i, %fast.i22.i ], [ %resolved.i20.i, %slow.i19.i ]
  %r5.i152 = load i64, ptr %common.ret.op.i21.i, align 1
  %r6.i = icmp eq i64 %r5.i152, 0
  br i1 %r6.i, label %ast_block_6.i, label %ast_block_2.i

ast_block_2.i:                                    ; preds = %chali_primitive_address.exit25.i
  store ptr @f_v466_RawStorage__int64_capacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %revision_pointer.i.i153 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i154 = icmp eq ptr %revision_pointer.i.i153, null
  br i1 %has_revision.not.i.i154, label %slow.i.i169, label %check.i.i155

check.i.i155:                                     ; preds = %ast_block_2.i
  %revision.i.i156 = load atomic i64, ptr %revision_pointer.i.i153 acquire, align 8
  %stamp.i.i157 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i158 = icmp eq i64 %revision.i.i156, %stamp.i.i157
  %enabled.i.i159 = icmp ne i64 %revision.i.i156, 0
  %live.i.i160 = and i1 %enabled.i.i159, %same_revision.i.i158
  %begin.i.i161 = load i64, ptr @chali_address_view, align 16
  %size.i.i162 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i163 = sub i64 %p0, %begin.i.i161
  %inside.i.i164 = icmp ult i64 %offset.i.i163, %size.i.i162
  %remaining.i.i165 = sub i64 %size.i.i162, %offset.i.i163
  %fits.i.i166 = icmp ugt i64 %remaining.i.i165, 7
  %range.i.i167 = and i1 %inside.i.i164, %fits.i.i166
  %valid.i.i168 = and i1 %live.i.i160, %range.i.i167
  br i1 %valid.i.i168, label %fast.i.i174, label %slow.i.i169

fast.i.i174:                                      ; preds = %check.i.i155
  %base.i.i175 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i.i176 = getelementptr i8, ptr %base.i.i175, i64 %offset.i.i163
  br label %chali_primitive_address.exit.i171

slow.i.i169:                                      ; preds = %check.i.i155, %ast_block_2.i
  %resolved.i.i170 = call ptr @chali_host_address_view(i64 %p0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit.i171

chali_primitive_address.exit.i171:                ; preds = %slow.i.i169, %fast.i.i174
  %common.ret.op.i.i172 = phi ptr [ %physical.i.i176, %fast.i.i174 ], [ %resolved.i.i170, %slow.i.i169 ]
  %r16.i = load i64, ptr %common.ret.op.i.i172, align 1
  store ptr @f_v466_RawStorage__int64_capacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %r17.i = call i64 @chali_host_raw_storage(i64 4, i64 %r16.i, i64 0, i64 0, i64 0, i64 1)
  br label %ast_block_6.i

ast_block_6.i.thread:                             ; preds = %chali_primitive_address.exit95.i
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i.i)
  br label %ast_block_0.i.i

ast_block_6.i:                                    ; preds = %chali_primitive_address.exit.i171, %chali_primitive_address.exit25.i
  %common.ret.op.i173 = phi i64 [ %r17.i, %chali_primitive_address.exit.i171 ], [ 0, %chali_primitive_address.exit25.i ]
  %r23.i.not = icmp sgt i64 %s_v794___ast_465_i_2242.0180, %common.ret.op.i173
  store ptr @f_v464_RawStorage__int64_pointer, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i.i)
  br i1 %r23.i.not, label %ast_block_0.i.i, label %f_v162_std__lang__assert_bool.exit.i

ast_block_0.i.i:                                  ; preds = %ast_block_6.i.thread, %ast_block_6.i
  store ptr @f_v162_std__lang__assert_bool, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 782, ptr @chali_source_line, align 8
  store i64 632, ptr %indirect_arguments.i.i, align 8
  store i8 0, ptr %native_argument_types.i.i, align 1
  %r8.i.i = call i64 @chali_host_call(ptr nonnull @native_name3, i64 1, ptr nonnull %indirect_arguments.i.i, ptr nonnull %native_argument_types.i.i)
  %r9.i.i = call i8 @chali_host_result_float()
  br label %f_v162_std__lang__assert_bool.exit.i

f_v162_std__lang__assert_bool.exit.i:             ; preds = %ast_block_0.i.i, %ast_block_6.i
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i.i)
  %1 = call i64 @chali_host_allocate(i64 noundef 16) #27
  call void @chali_host_memory_prepare(i64 8, i64 %1, i64 0, i64 16, ptr nonnull %raw_memory_output.i, ptr nonnull %raw_memory_input.i)
  %r33.i = load ptr, ptr %raw_memory_output.i, align 8
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 1 dereferenceable(16) %r33.i, i8 0, i64 16, i1 false)
  store i64 %1, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  store i64 16, ptr %r39.i, align 8
  store i8 0, ptr %r40.i, align 1
  %r41.i = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r42.i = call i8 @chali_host_result_float()
  store ptr @f_v464_RawStorage__int64_pointer, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %revision_pointer.i50.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i51.i = icmp eq ptr %revision_pointer.i50.i, null
  br i1 %has_revision.not.i51.i, label %slow.i66.i, label %check.i52.i

check.i52.i:                                      ; preds = %f_v162_std__lang__assert_bool.exit.i
  %revision.i53.i = load atomic i64, ptr %revision_pointer.i50.i acquire, align 8
  %stamp.i54.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i55.i = icmp eq i64 %revision.i53.i, %stamp.i54.i
  %enabled.i56.i = icmp ne i64 %revision.i53.i, 0
  %live.i57.i = and i1 %enabled.i56.i, %same_revision.i55.i
  %begin.i58.i = load i64, ptr @chali_address_view, align 16
  %size.i59.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i60.i = sub i64 %p0, %begin.i58.i
  %inside.i61.i = icmp ult i64 %offset.i60.i, %size.i59.i
  %remaining.i62.i = sub i64 %size.i59.i, %offset.i60.i
  %fits.i63.i = icmp ugt i64 %remaining.i62.i, 7
  %range.i64.i = and i1 %inside.i61.i, %fits.i63.i
  %valid.i65.i = and i1 %live.i57.i, %range.i64.i
  br i1 %valid.i65.i, label %fast.i69.i, label %slow.i66.i

fast.i69.i:                                       ; preds = %check.i52.i
  %base.i70.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i71.i = getelementptr i8, ptr %base.i70.i, i64 %offset.i60.i
  br label %chali_primitive_address.exit72.i

slow.i66.i:                                       ; preds = %check.i52.i, %f_v162_std__lang__assert_bool.exit.i
  %resolved.i67.i = call ptr @chali_host_address_view(i64 %p0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit72.i

chali_primitive_address.exit72.i:                 ; preds = %slow.i66.i, %fast.i69.i
  %common.ret.op.i68.i = phi ptr [ %physical.i71.i, %fast.i69.i ], [ %resolved.i67.i, %slow.i66.i ]
  %r48.i = load i64, ptr %common.ret.op.i68.i, align 1
  %revision_pointer.i27.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i28.i = icmp eq ptr %revision_pointer.i27.i, null
  br i1 %has_revision.not.i28.i, label %slow.i43.i, label %check.i29.i

check.i29.i:                                      ; preds = %chali_primitive_address.exit72.i
  %revision.i30.i = load atomic i64, ptr %revision_pointer.i27.i acquire, align 8
  %stamp.i31.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i32.i = icmp eq i64 %revision.i30.i, %stamp.i31.i
  %enabled.i33.i = icmp ne i64 %revision.i30.i, 0
  %live.i34.i = and i1 %enabled.i33.i, %same_revision.i32.i
  %begin.i35.i = load i64, ptr @chali_address_view, align 16
  %size.i36.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i37.i = sub i64 %1, %begin.i35.i
  %inside.i38.i = icmp ult i64 %offset.i37.i, %size.i36.i
  %remaining.i39.i = sub i64 %size.i36.i, %offset.i37.i
  %fits.i40.i = icmp ugt i64 %remaining.i39.i, 7
  %range.i41.i = and i1 %inside.i38.i, %fits.i40.i
  %valid.i42.i = and i1 %live.i34.i, %range.i41.i
  br i1 %valid.i42.i, label %fast.i46.i, label %slow.i43.i

fast.i46.i:                                       ; preds = %check.i29.i
  %base.i47.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i48.i = getelementptr i8, ptr %base.i47.i, i64 %offset.i37.i
  br label %chali_primitive_address.exit49.i

slow.i43.i:                                       ; preds = %check.i29.i, %chali_primitive_address.exit72.i
  %resolved.i44.i = call ptr @chali_host_address_view(i64 %1, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit49.i

chali_primitive_address.exit49.i:                 ; preds = %slow.i43.i, %fast.i46.i
  %common.ret.op.i45.i = phi ptr [ %physical.i48.i, %fast.i46.i ], [ %resolved.i44.i, %slow.i43.i ]
  store i64 %r48.i, ptr %common.ret.op.i45.i, align 1
  %r59.i = add i64 %1, 8
  %revision_pointer.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i = icmp eq ptr %revision_pointer.i.i, null
  br i1 %has_revision.not.i.i, label %slow.i.i, label %check.i.i

check.i.i:                                        ; preds = %chali_primitive_address.exit49.i
  %revision.i.i = load atomic i64, ptr %revision_pointer.i.i acquire, align 8
  %stamp.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i = icmp eq i64 %revision.i.i, %stamp.i.i
  %enabled.i.i = icmp ne i64 %revision.i.i, 0
  %live.i.i = and i1 %enabled.i.i, %same_revision.i.i
  %begin.i.i = load i64, ptr @chali_address_view, align 16
  %size.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i = sub i64 %r59.i, %begin.i.i
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

slow.i.i:                                         ; preds = %check.i.i, %chali_primitive_address.exit49.i
  %resolved.i.i = call ptr @chali_host_address_view(i64 %r59.i, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit.i

chali_primitive_address.exit.i:                   ; preds = %slow.i.i, %fast.i.i
  %common.ret.op.i.i = phi ptr [ %physical.i.i, %fast.i.i ], [ %resolved.i.i, %slow.i.i ]
  store i64 %s_v794___ast_465_i_2242.0180, ptr %common.ret.op.i.i, align 1
  store i64 48, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  store i64 16, ptr %r39.i, align 8
  store i8 0, ptr %r40.i, align 1
  %r67.i = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r68.i = call i8 @chali_host_result_float()
  %r73.i = call i64 @chali_host_copy_words(i64 48, i64 %1, i64 2)
  store i64 48, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  store i64 16, ptr %r39.i, align 8
  store i8 0, ptr %r40.i, align 1
  %r80.i = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r81.i = call i8 @chali_host_result_float()
  %r84.not.i = icmp eq i64 %1, 0
  br i1 %r84.not.i, label %f_v464_RawStorage__int64_pointer.exit, label %ast_block_8.i

ast_block_8.i:                                    ; preds = %chali_primitive_address.exit.i
  call void @chali_host_drop(i64 noundef %1) #27
  br label %f_v464_RawStorage__int64_pointer.exit

f_v464_RawStorage__int64_pointer.exit:            ; preds = %chali_primitive_address.exit.i, %ast_block_8.i
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %raw_memory_output.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %raw_memory_input.i)
  %r70 = call i64 @chali_host_copy_words(i64 %0, i64 48, i64 2)
  store i64 %0, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 16, ptr %r56, align 8
  store i8 0, ptr %r57, align 1
  %r77 = call i64 @chali_host_call(ptr nonnull @native_name0, i64 2, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r78 = call i8 @chali_host_result_float()
  %r83.not = icmp eq i64 %0, 48
  br i1 %r83.not, label %ast_block_14, label %ast_block_12

ast_block_12:                                     ; preds = %f_v464_RawStorage__int64_pointer.exit
  store i64 48, ptr %indirect_arguments, align 8
  store i8 0, ptr %native_argument_types, align 1
  store i64 16, ptr %r56, align 8
  store i8 0, ptr %r57, align 1
  store i64 48, ptr %r92, align 8
  store i8 0, ptr %r93, align 1
  %r94 = call i64 @chali_host_call(ptr nonnull @native_name1, i64 3, ptr nonnull %indirect_arguments, ptr nonnull %native_argument_types)
  %r95 = call i8 @chali_host_result_float()
  br label %ast_block_14

ast_block_14:                                     ; preds = %f_v464_RawStorage__int64_pointer.exit, %ast_block_12
  store ptr @f_v465_RawStorage__int64_close, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %r98 = call i64 @f_v461_std__lang__dropAt__int64(i64 %0)
  %r102.not = icmp eq i64 %0, 0
  br i1 %r102.not, label %ast_block_11, label %ast_block_15

ast_block_15:                                     ; preds = %ast_block_14
  call void @chali_host_drop(i64 noundef %0) #27
  br label %ast_block_11

ast_block_11:                                     ; preds = %ast_block_15, %ast_block_14, %chali_primitive_address.exit82
  %r109 = add nuw nsw i64 %s_v794___ast_465_i_2242.0180, 1
  store ptr @f_v465_RawStorage__int64_close, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %exitcond.not = icmp eq i64 %r109, %r17
  br i1 %exitcond.not, label %ast_block_5, label %ast_block_6

ast_block_5:                                      ; preds = %ast_block_11, %chali_primitive_address.exit105
  store ptr @f_v465_RawStorage__int64_close, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %revision_pointer.i37 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i38 = icmp eq ptr %revision_pointer.i37, null
  br i1 %has_revision.not.i38, label %slow.i53, label %check.i39

check.i39:                                        ; preds = %ast_block_5
  %revision.i40 = load atomic i64, ptr %revision_pointer.i37 acquire, align 8
  %stamp.i41 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i42 = icmp eq i64 %revision.i40, %stamp.i41
  %enabled.i43 = icmp ne i64 %revision.i40, 0
  %live.i44 = and i1 %enabled.i43, %same_revision.i42
  %begin.i45 = load i64, ptr @chali_address_view, align 16
  %size.i46 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i47 = sub i64 %p0, %begin.i45
  %inside.i48 = icmp ult i64 %offset.i47, %size.i46
  %remaining.i49 = sub i64 %size.i46, %offset.i47
  %fits.i50 = icmp ugt i64 %remaining.i49, 7
  %range.i51 = and i1 %inside.i48, %fits.i50
  %valid.i52 = and i1 %live.i44, %range.i51
  br i1 %valid.i52, label %fast.i56, label %slow.i53

fast.i56:                                         ; preds = %check.i39
  %base.i57 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i58 = getelementptr i8, ptr %base.i57, i64 %offset.i47
  br label %chali_primitive_address.exit59

slow.i53:                                         ; preds = %check.i39, %ast_block_5
  %resolved.i54 = call ptr @chali_host_address_view(i64 %p0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit59

chali_primitive_address.exit59:                   ; preds = %fast.i56, %slow.i53
  %common.ret.op.i55 = phi ptr [ %physical.i58, %fast.i56 ], [ %resolved.i54, %slow.i53 ]
  %r115 = load i64, ptr %common.ret.op.i55, align 1
  store ptr @f_v465_RawStorage__int64_close, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %r116 = call i64 @chali_host_raw_storage(i64 5, i64 %r115, i64 0, i64 0, i64 0, i64 1)
  %revision_pointer.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i = icmp eq ptr %revision_pointer.i, null
  br i1 %has_revision.not.i, label %slow.i, label %check.i

check.i:                                          ; preds = %chali_primitive_address.exit59
  %revision.i = load atomic i64, ptr %revision_pointer.i acquire, align 8
  %stamp.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i = icmp eq i64 %revision.i, %stamp.i
  %enabled.i = icmp ne i64 %revision.i, 0
  %live.i = and i1 %enabled.i, %same_revision.i
  %begin.i = load i64, ptr @chali_address_view, align 16
  %size.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i = sub i64 %p0, %begin.i
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

slow.i:                                           ; preds = %check.i, %chali_primitive_address.exit59
  %resolved.i = call ptr @chali_host_address_view(i64 %p0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit

chali_primitive_address.exit:                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  store i64 0, ptr %common.ret.op.i, align 1
  br label %common.ret
}

; Function Attrs: alwaysinline uwtable
define internal i64 @f_v466_RawStorage__int64_capacity(i64 %p0) #21 section "chali_code" {
entry:
  store ptr @f_v466_RawStorage__int64_capacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
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
  %r5 = load i64, ptr %common.ret.op.i21, align 1
  store ptr @f_v466_RawStorage__int64_capacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %r6 = icmp eq i64 %r5, 0
  br i1 %r6, label %common.ret, label %ast_block_2

common.ret:                                       ; preds = %chali_primitive_address.exit25, %chali_primitive_address.exit
  %common.ret.op = phi i64 [ %r17, %chali_primitive_address.exit ], [ 0, %chali_primitive_address.exit25 ]
  ret i64 %common.ret.op

ast_block_2:                                      ; preds = %chali_primitive_address.exit25
  store ptr @source_file0, ptr @chali_source_file, align 8
  %revision_pointer.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i = icmp eq ptr %revision_pointer.i, null
  br i1 %has_revision.not.i, label %slow.i, label %check.i

check.i:                                          ; preds = %ast_block_2
  %revision.i = load atomic i64, ptr %revision_pointer.i acquire, align 8
  %stamp.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i = icmp eq i64 %revision.i, %stamp.i
  %enabled.i = icmp ne i64 %revision.i, 0
  %live.i = and i1 %enabled.i, %same_revision.i
  %begin.i = load i64, ptr @chali_address_view, align 16
  %size.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i = sub i64 %p0, %begin.i
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

slow.i:                                           ; preds = %check.i, %ast_block_2
  %resolved.i = tail call ptr @chali_host_address_view(i64 %p0, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit

chali_primitive_address.exit:                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  %r16 = load i64, ptr %common.ret.op.i, align 1
  store ptr @f_v466_RawStorage__int64_capacity, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 1330, ptr @chali_source_line, align 8
  %r17 = tail call i64 @chali_host_raw_storage(i64 4, i64 %r16, i64 0, i64 0, i64 0, i64 1)
  br label %common.ret
}

; Function Attrs: alwaysinline uwtable
define internal noundef i64 @f___chali_library_init() #21 section "chali_code" {
entry:
  store i1 true, ptr @s_g_1___chali_global_reference, align 1
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
  %offset.i = sub i64 104, %begin.i
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
  %resolved.i = tail call ptr @chali_host_address_view(i64 104, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit

chali_primitive_address.exit:                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  store i64 0, ptr %common.ret.op.i, align 1
  store i64 0, ptr @s_g_2_program__firstChunk, align 8
  store i64 0, ptr @s_g_3_program__secondChunk, align 8
  store i64 0, ptr @s_g_4_program__unitWidth, align 8
  ret i64 0
}

; Function Attrs: uwtable
define internal noundef i64 @f___chali_library_finish() #24 section "chali_code" {
entry:
  %r0 = load i64, ptr @s_g_3_program__secondChunk, align 8
  %r2.not = icmp eq i64 %r0, 0
  br i1 %r2.not, label %ast_block_2, label %ast_block_0

ast_block_0:                                      ; preds = %entry
  tail call void @chali_host_drop(i64 noundef %r0) #27
  br label %ast_block_2

ast_block_2:                                      ; preds = %entry, %ast_block_0
  store i64 0, ptr @s_g_3_program__secondChunk, align 8
  %r9 = load i64, ptr @s_g_2_program__firstChunk, align 8
  %r11.not = icmp eq i64 %r9, 0
  br i1 %r11.not, label %ast_block_5, label %ast_block_3

ast_block_3:                                      ; preds = %ast_block_2
  tail call void @chali_host_drop(i64 noundef %r9) #27
  br label %ast_block_5

ast_block_5:                                      ; preds = %ast_block_2, %ast_block_3
  store i64 0, ptr @s_g_2_program__firstChunk, align 8
  %r18.b = load i1, ptr @s_g_1___chali_global_reference, align 1
  %r18 = select i1 %r18.b, i64 104, i64 0
  %revision_pointer.i24 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i25 = icmp eq ptr %revision_pointer.i24, null
  br i1 %has_revision.not.i25, label %slow.i40, label %check.i26

check.i26:                                        ; preds = %ast_block_5
  %revision.i27 = load atomic i64, ptr %revision_pointer.i24 acquire, align 8
  %stamp.i28 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i29 = icmp eq i64 %revision.i27, %stamp.i28
  %enabled.i30 = icmp ne i64 %revision.i27, 0
  %live.i31 = and i1 %enabled.i30, %same_revision.i29
  %begin.i32 = load i64, ptr @chali_address_view, align 16
  %size.i33 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i34 = sub i64 %r18, %begin.i32
  %inside.i35 = icmp ult i64 %offset.i34, %size.i33
  %remaining.i36 = sub i64 %size.i33, %offset.i34
  %fits.i37 = icmp ugt i64 %remaining.i36, 7
  %range.i38 = and i1 %inside.i35, %fits.i37
  %valid.i39 = and i1 %live.i31, %range.i38
  br i1 %valid.i39, label %fast.i43, label %slow.i40

fast.i43:                                         ; preds = %check.i26
  %base.i44 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i45 = getelementptr i8, ptr %base.i44, i64 %offset.i34
  br label %chali_primitive_address.exit46

slow.i40:                                         ; preds = %check.i26, %ast_block_5
  %resolved.i41 = tail call ptr @chali_host_address_view(i64 %r18, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit46

chali_primitive_address.exit46:                   ; preds = %fast.i43, %slow.i40
  %common.ret.op.i42 = phi ptr [ %physical.i45, %fast.i43 ], [ %resolved.i41, %slow.i40 ]
  %r21 = load i64, ptr %common.ret.op.i42, align 1
  %r22.not = icmp eq i64 %r21, 0
  br i1 %r22.not, label %ast_block_8, label %ast_block_6

ast_block_6:                                      ; preds = %chali_primitive_address.exit46
  %r25.b = load i1, ptr @s_g_1___chali_global_reference, align 1
  %r25 = select i1 %r25.b, i64 104, i64 0
  %revision_pointer.i1 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i2 = icmp eq ptr %revision_pointer.i1, null
  br i1 %has_revision.not.i2, label %slow.i17, label %check.i3

check.i3:                                         ; preds = %ast_block_6
  %revision.i4 = load atomic i64, ptr %revision_pointer.i1 acquire, align 8
  %stamp.i5 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i6 = icmp eq i64 %revision.i4, %stamp.i5
  %enabled.i7 = icmp ne i64 %revision.i4, 0
  %live.i8 = and i1 %enabled.i7, %same_revision.i6
  %begin.i9 = load i64, ptr @chali_address_view, align 16
  %size.i10 = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i11 = sub i64 %r25, %begin.i9
  %inside.i12 = icmp ult i64 %offset.i11, %size.i10
  %remaining.i13 = sub i64 %size.i10, %offset.i11
  %fits.i14 = icmp ugt i64 %remaining.i13, 7
  %range.i15 = and i1 %inside.i12, %fits.i14
  %valid.i16 = and i1 %live.i8, %range.i15
  br i1 %valid.i16, label %fast.i20, label %slow.i17

fast.i20:                                         ; preds = %check.i3
  %base.i21 = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 16), align 16
  %physical.i22 = getelementptr i8, ptr %base.i21, i64 %offset.i11
  br label %chali_primitive_address.exit23

slow.i17:                                         ; preds = %check.i3, %ast_block_6
  %resolved.i18 = tail call ptr @chali_host_address_view(i64 %r25, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit23

chali_primitive_address.exit23:                   ; preds = %fast.i20, %slow.i17
  %common.ret.op.i19 = phi ptr [ %physical.i22, %fast.i20 ], [ %resolved.i18, %slow.i17 ]
  %r28 = load i64, ptr %common.ret.op.i19, align 1
  tail call void @chali_host_drop(i64 noundef %r28) #27
  br label %ast_block_8

ast_block_8:                                      ; preds = %chali_primitive_address.exit46, %chali_primitive_address.exit23
  %r31.b = load i1, ptr @s_g_1___chali_global_reference, align 1
  %r31 = select i1 %r31.b, i64 104, i64 0
  %revision_pointer.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i = icmp eq ptr %revision_pointer.i, null
  br i1 %has_revision.not.i, label %slow.i, label %check.i

check.i:                                          ; preds = %ast_block_8
  %revision.i = load atomic i64, ptr %revision_pointer.i acquire, align 8
  %stamp.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i = icmp eq i64 %revision.i, %stamp.i
  %enabled.i = icmp ne i64 %revision.i, 0
  %live.i = and i1 %enabled.i, %same_revision.i
  %begin.i = load i64, ptr @chali_address_view, align 16
  %size.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i = sub i64 %r31, %begin.i
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

slow.i:                                           ; preds = %check.i, %ast_block_8
  %resolved.i = tail call ptr @chali_host_address_view(i64 %r31, i64 8, ptr nonnull @chali_address_view)
  br label %chali_primitive_address.exit

chali_primitive_address.exit:                     ; preds = %fast.i, %slow.i
  %common.ret.op.i = phi ptr [ %physical.i, %fast.i ], [ %resolved.i, %slow.i ]
  store i64 0, ptr %common.ret.op.i, align 1
  ret i64 0
}

; Function Attrs: alwaysinline uwtable
define internal noundef i64 @f_v472___chali_copy_struct(i64 %p0, i64 %p1, i64 %p2) #21 section "chali_code" {
entry:
  %r430 = icmp sgt i64 %p2, 0
  br i1 %r430, label %ast_block_1, label %ast_block_2

ast_block_1:                                      ; preds = %entry, %chali_primitive_address.exit
  %s_v802___ast_472___chali_k.031 = phi i64 [ %r26, %chali_primitive_address.exit ], [ 0, %entry ]
  %r11 = shl nsw i64 %s_v802___ast_472___chali_k.031, 3
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
  %r25 = shl nsw i64 %s_v802___ast_472___chali_k.031, 32
  %sext = add i64 %r25, 4294967296
  %r26 = ashr exact i64 %sext, 32
  %r4 = icmp slt i64 %r26, %p2
  br i1 %r4, label %ast_block_1, label %ast_block_2

ast_block_2:                                      ; preds = %chali_primitive_address.exit, %entry
  ret i64 0
}

; Function Attrs: uwtable
define internal i64 @f_v474_std__lang____bootStringJoin2(i64 %p0, i64 %p1) #24 section "chali_code" {
entry:
  %indirect_arguments.i29 = alloca [2 x i64], align 8
  %native_argument_types.i30 = alloca [2 x i8], align 1
  %indirect_arguments.i = alloca [2 x i64], align 8
  %native_argument_types.i = alloca [2 x i8], align 1
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i27 = tail call i64 @chali_host_string_storage(i64 0, i64 %p0)
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i29)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i30)
  %r7.i32 = icmp slt i64 %r2.i27, 0
  %or.cond.not = icmp ult i64 %r2.i27, 9223372036854775807
  br i1 %or.cond.not, label %f_v70_std__lang____bootStringAddLength.exit39, label %ast_block_8.i33

ast_block_8.i33:                                  ; preds = %entry
  store ptr @f_v70_std__lang____bootStringAddLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 250, ptr @chali_source_line, align 8
  store i64 288, ptr %indirect_arguments.i29, align 8
  store i8 0, ptr %native_argument_types.i30, align 1
  %r29.i34 = call i64 @chali_host_call(ptr nonnull @native_name3, i64 1, ptr nonnull %indirect_arguments.i29, ptr nonnull %native_argument_types.i30)
  %r30.i35 = call i8 @chali_host_result_float()
  br label %f_v70_std__lang____bootStringAddLength.exit39

f_v70_std__lang____bootStringAddLength.exit39:    ; preds = %entry, %ast_block_8.i33
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i29)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i30)
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i = call i64 @chali_host_string_storage(i64 0, i64 %p1)
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i)
  %r7.i = icmp slt i64 %r2.i, 0
  %or.cond40.not42 = select i1 %r7.i32, i1 true, i1 %r7.i
  %r17.i = sub nsw i64 9223372036854775806, %r2.i27
  %r18.i = icmp sgt i64 %r2.i, %r17.i
  %or.cond41 = select i1 %or.cond40.not42, i1 true, i1 %r18.i
  br i1 %or.cond41, label %ast_block_8.i, label %f_v70_std__lang____bootStringAddLength.exit

ast_block_8.i:                                    ; preds = %f_v70_std__lang____bootStringAddLength.exit39
  store ptr @f_v70_std__lang____bootStringAddLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 250, ptr @chali_source_line, align 8
  store i64 288, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  %r29.i = call i64 @chali_host_call(ptr nonnull @native_name3, i64 1, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r30.i = call i8 @chali_host_result_float()
  br label %f_v70_std__lang____bootStringAddLength.exit

f_v70_std__lang____bootStringAddLength.exit:      ; preds = %f_v70_std__lang____bootStringAddLength.exit39, %ast_block_8.i
  store ptr @f_v70_std__lang____bootStringAddLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 250, ptr @chali_source_line, align 8
  %r35.i = add i64 %r2.i, %r2.i27
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i)
  %r22 = call i64 @f_v57_std__lang____bootStringBuffer(i64 %r35.i)
  %r28 = add i64 %r22, 8
  %r31 = call i64 @chali_host_string_storage(i64 1, i64 %p0)
  %r34 = call i64 @f_v58_std__lang____bootStringCopyBytes(i64 %r28, i64 %r31, i64 %r2.i27)
  %r40 = add i64 %r2.i27, 8
  %r45 = add i64 %r40, %r22
  %r48 = call i64 @chali_host_string_storage(i64 1, i64 %p1)
  %r51 = call i64 @f_v58_std__lang____bootStringCopyBytes(i64 %r45, i64 %r48, i64 %r2.i)
  ret i64 %r22
}

; Function Attrs: uwtable
define internal i64 @f_v475_std__lang____bootStringJoin3(i64 %p0, i64 %p1, i64 %p2) #24 section "chali_code" {
entry:
  %indirect_arguments.i57 = alloca [2 x i64], align 8
  %native_argument_types.i58 = alloca [2 x i8], align 1
  %indirect_arguments.i44 = alloca [2 x i64], align 8
  %native_argument_types.i45 = alloca [2 x i8], align 1
  %indirect_arguments.i = alloca [2 x i64], align 8
  %native_argument_types.i = alloca [2 x i8], align 1
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i42 = tail call i64 @chali_host_string_storage(i64 0, i64 %p0)
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i57)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i58)
  %r7.i60 = icmp slt i64 %r2.i42, 0
  %or.cond.not = icmp ult i64 %r2.i42, 9223372036854775807
  br i1 %or.cond.not, label %f_v70_std__lang____bootStringAddLength.exit67, label %ast_block_8.i61

ast_block_8.i61:                                  ; preds = %entry
  store ptr @f_v70_std__lang____bootStringAddLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 250, ptr @chali_source_line, align 8
  store i64 288, ptr %indirect_arguments.i57, align 8
  store i8 0, ptr %native_argument_types.i58, align 1
  %r29.i62 = call i64 @chali_host_call(ptr nonnull @native_name3, i64 1, ptr nonnull %indirect_arguments.i57, ptr nonnull %native_argument_types.i58)
  %r30.i63 = call i8 @chali_host_result_float()
  br label %f_v70_std__lang____bootStringAddLength.exit67

f_v70_std__lang____bootStringAddLength.exit67:    ; preds = %entry, %ast_block_8.i61
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i57)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i58)
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i41 = call i64 @chali_host_string_storage(i64 0, i64 %p1)
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i44)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i45)
  %r7.i52 = icmp slt i64 %r2.i41, 0
  %or.cond68.not72 = select i1 %r7.i60, i1 true, i1 %r7.i52
  %r17.i54 = sub nsw i64 9223372036854775806, %r2.i42
  %r18.i55 = icmp sgt i64 %r2.i41, %r17.i54
  %or.cond69 = select i1 %or.cond68.not72, i1 true, i1 %r18.i55
  br i1 %or.cond69, label %ast_block_8.i47, label %f_v70_std__lang____bootStringAddLength.exit56

ast_block_8.i47:                                  ; preds = %f_v70_std__lang____bootStringAddLength.exit67
  store ptr @f_v70_std__lang____bootStringAddLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 250, ptr @chali_source_line, align 8
  store i64 288, ptr %indirect_arguments.i44, align 8
  store i8 0, ptr %native_argument_types.i45, align 1
  %r29.i48 = call i64 @chali_host_call(ptr nonnull @native_name3, i64 1, ptr nonnull %indirect_arguments.i44, ptr nonnull %native_argument_types.i45)
  %r30.i49 = call i8 @chali_host_result_float()
  br label %f_v70_std__lang____bootStringAddLength.exit56

f_v70_std__lang____bootStringAddLength.exit56:    ; preds = %f_v70_std__lang____bootStringAddLength.exit67, %ast_block_8.i47
  %r35.i50 = add i64 %r2.i41, %r2.i42
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i44)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i45)
  store ptr @f_v56_std__lang____bootStringLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 161, ptr @chali_source_line, align 8
  %r2.i = call i64 @chali_host_string_storage(i64 0, i64 %p2)
  call void @llvm.lifetime.start.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.start.p0(ptr nonnull %native_argument_types.i)
  %r2.i43 = icmp slt i64 %r35.i50, 0
  %r7.i = icmp slt i64 %r2.i, 0
  %or.cond70.not73 = select i1 %r2.i43, i1 true, i1 %r7.i
  %r17.i = sub nsw i64 9223372036854775806, %r35.i50
  %r18.i = icmp sgt i64 %r2.i, %r17.i
  %or.cond71 = select i1 %or.cond70.not73, i1 true, i1 %r18.i
  br i1 %or.cond71, label %ast_block_8.i, label %f_v70_std__lang____bootStringAddLength.exit

ast_block_8.i:                                    ; preds = %f_v70_std__lang____bootStringAddLength.exit56
  store ptr @f_v70_std__lang____bootStringAddLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 250, ptr @chali_source_line, align 8
  store i64 288, ptr %indirect_arguments.i, align 8
  store i8 0, ptr %native_argument_types.i, align 1
  %r29.i = call i64 @chali_host_call(ptr nonnull @native_name3, i64 1, ptr nonnull %indirect_arguments.i, ptr nonnull %native_argument_types.i)
  %r30.i = call i8 @chali_host_result_float()
  br label %f_v70_std__lang____bootStringAddLength.exit

f_v70_std__lang____bootStringAddLength.exit:      ; preds = %f_v70_std__lang____bootStringAddLength.exit56, %ast_block_8.i
  store ptr @f_v70_std__lang____bootStringAddLength, ptr @chali_source_function, align 8
  store ptr @source_file0, ptr @chali_source_file, align 8
  store i64 250, ptr @chali_source_line, align 8
  %r35.i = add i64 %r2.i, %r35.i50
  call void @llvm.lifetime.end.p0(ptr nonnull %indirect_arguments.i)
  call void @llvm.lifetime.end.p0(ptr nonnull %native_argument_types.i)
  %r32 = call i64 @f_v57_std__lang____bootStringBuffer(i64 %r35.i)
  %r38 = add i64 %r32, 8
  %r41 = call i64 @chali_host_string_storage(i64 1, i64 %p0)
  %r44 = call i64 @f_v58_std__lang____bootStringCopyBytes(i64 %r38, i64 %r41, i64 %r2.i42)
  %r50 = add i64 %r2.i42, 8
  %r55 = add i64 %r32, %r50
  %r58 = call i64 @chali_host_string_storage(i64 1, i64 %p1)
  %r61 = call i64 @f_v58_std__lang____bootStringCopyBytes(i64 %r55, i64 %r58, i64 %r2.i41)
  %r67 = add i64 %r2.i41, %r50
  %r72 = add i64 %r67, %r32
  %r75 = call i64 @chali_host_string_storage(i64 1, i64 %p2)
  %r78 = call i64 @f_v58_std__lang____bootStringCopyBytes(i64 %r72, i64 %r75, i64 %r2.i)
  ret i64 %r32
}

; Function Attrs: noreturn nounwind
define noundef i64 @chali_invoke(i64 %function, i64 %count, ptr nofree readnone captures(none) %arguments) local_unnamed_addr #26 {
entry:
  %target = tail call i64 @chali_function_target(i64 %function)
  %environment = tail call i64 @chali_function_environment(i64 %function)
  tail call void @chali_abort(i64 0)
  unreachable
}

; Function Attrs: noreturn nounwind
define noundef i64 @chali_function_returns_real(i64 %function) local_unnamed_addr #26 {
entry:
  %target = tail call i64 @chali_function_target(i64 %function)
  tail call void @chali_abort(i64 0)
  unreachable
}

define i64 @chali_export_dispatch(i64 %index, ptr nofree readonly captures(none) %arguments) local_unnamed_addr {
entry:
  switch i64 %index, label %bad [
    i64 0, label %case0
    i64 1, label %case1
    i64 2, label %case2
    i64 3, label %case3
    i64 4, label %case4
  ]

common.ret:                                       ; preds = %case4, %case3, %case2, %case1, %case0
  %common.ret.op = phi i64 [ 0, %case0 ], [ %result1, %case1 ], [ %result2, %case2 ], [ %result3, %case3 ], [ %result4, %case4 ]
  ret i64 %common.ret.op

case0:                                            ; preds = %entry
  %result0 = tail call i64 @__chali_impl_program__setup()
  br label %common.ret

case1:                                            ; preds = %entry
  %v1_0 = load i64, ptr %arguments, align 8
  %result1 = tail call i64 @__chali_impl_program__readWork(i64 %v1_0)
  br label %common.ret

case2:                                            ; preds = %entry
  %v2_0 = load i64, ptr %arguments, align 8
  %result2 = tail call i64 @__chali_impl_program__concatWork(i64 %v2_0)
  br label %common.ret

case3:                                            ; preds = %entry
  %v3_0 = load i64, ptr %arguments, align 8
  %result3 = tail call i64 @__chali_impl_program__sliceWork(i64 %v3_0)
  br label %common.ret

case4:                                            ; preds = %entry
  %v4_0 = load i64, ptr %arguments, align 8
  %result4 = tail call i64 @__chali_impl_program__findWork(i64 %v4_0)
  br label %common.ret

bad:                                              ; preds = %entry
  tail call void @chali_abort(i64 0)
  unreachable
}

define dllexport void @chali_init() local_unnamed_addr {
entry:
  tail call void @chali_host_init() #27
  store i1 true, ptr @s_g_1___chali_global_reference, align 1
  %revision_pointer.i.i = load ptr, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 24), align 8
  %has_revision.not.i.i = icmp eq ptr %revision_pointer.i.i, null
  br i1 %has_revision.not.i.i, label %slow.i.i, label %check.i.i

check.i.i:                                        ; preds = %entry
  %revision.i.i = load atomic i64, ptr %revision_pointer.i.i acquire, align 8
  %stamp.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 32), align 16
  %same_revision.i.i = icmp eq i64 %revision.i.i, %stamp.i.i
  %enabled.i.i = icmp ne i64 %revision.i.i, 0
  %live.i.i = and i1 %enabled.i.i, %same_revision.i.i
  %begin.i.i = load i64, ptr @chali_address_view, align 16
  %size.i.i = load i64, ptr getelementptr inbounds nuw (i8, ptr @chali_address_view, i64 8), align 8
  %offset.i.i = sub i64 104, %begin.i.i
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

slow.i.i:                                         ; preds = %check.i.i, %entry
  %resolved.i.i = tail call ptr @chali_host_address_view(i64 104, i64 8, ptr nonnull @chali_address_view)
  br label %f___chali_library_init.exit

f___chali_library_init.exit:                      ; preds = %fast.i.i, %slow.i.i
  %common.ret.op.i.i = phi ptr [ %physical.i.i, %fast.i.i ], [ %resolved.i.i, %slow.i.i ]
  store i64 0, ptr %common.ret.op.i.i, align 1
  store i64 0, ptr @s_g_2_program__firstChunk, align 8
  store i64 0, ptr @s_g_3_program__secondChunk, align 8
  store i64 0, ptr @s_g_4_program__unitWidth, align 8
  ret void
}

define dllexport void @chali_finish() local_unnamed_addr {
entry:
  %finished = tail call i64 @f___chali_library_finish()
  %0 = tail call ptr @__acrt_iob_func(i32 noundef 1) #27
  %1 = tail call i32 @fflush(ptr noundef %0)
  tail call void @chali_host_finish() #27
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
attributes #22 = { alwaysinline mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable }
attributes #23 = { alwaysinline mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none, target_mem: none) uwtable }
attributes #24 = { uwtable }
attributes #25 = { alwaysinline nounwind uwtable }
attributes #26 = { noreturn nounwind }
attributes #27 = { nounwind }
attributes #28 = { cold noreturn nounwind }
attributes #29 = { allocsize(0) }
attributes #30 = { allocsize(1) }

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
!32 = !{ptr @f_v63_std__lang____bootStringSliceBytes}
!33 = !{ptr @f_v60_std__lang____bootStringCopy}
!34 = !{ptr @f_v55_std__lang____stringQuery}
