@CLS


@rustc --emit=llvm-ir -o a.ll prog.rs

@tools\llc -march=fisc -filetype=obj -o a.o a.ll


@rm *.ll



@readelf -a -W a.o


@tools\llvm-objdump -s -d a.o

@rm *.o