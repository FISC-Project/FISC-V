@CLS

@rustc --emit=llvm-ir -o a.ll prog.rs

@..\toolchain\Windows\Tools\llc -march=fisc -filetype=obj -o a.o a.ll



@rm *.ll



@..\bin\Release\fvm -t FISC -d --nodbgexec --dump reg.10 --dump mem.16384..16404.32 -c -b .\a.o

@rm *.o