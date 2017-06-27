@CLS


@rustc --emit=llvm-ir -o a.ll prog.rs

@..\toolchain\Windows\Tools\llc -march=fisc -filetype=obj -o a.o a.ll


@rm *.ll



@readelf -a -W a.o


@..\toolchain\Windows\Tools\llvm-objdump -s -d a.o

@rm *.o