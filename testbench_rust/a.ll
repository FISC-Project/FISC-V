; Function Attrs: uwtable
define internal void @_ZN4prog4main17hd6d4db6b7acfeb17E() unnamed_addr #0 {
entry-block:
  %_0 = alloca {}
  br label %start

start:                                            ; preds = %entry-block
  ret void
}

define i64 @main(i64, i8**) unnamed_addr {
top:
  %2 = call i64 @_ZN3std2rt10lang_start17h660a4a64b3f3f2ebE(i8* bitcast (void ()* @_ZN4prog4main17hd6d4db6b7acfeb17E to i8*), i64 %0, i8** %1)
  ret i64 %2
}

declare i64 @_ZN3std2rt10lang_start17h660a4a64b3f3f2ebE(i8*, i64, i8**) unnamed_addr

attributes #0 = { uwtable }

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"PIE Level", i32 2}
