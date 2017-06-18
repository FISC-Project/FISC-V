@CLS

@tools\clang -target fisc-unknown-unknown -S prog.c -emit-llvm -o prog.ll

@rem tools\clang -target fisc-unknown-unknown -S prog2.c -emit-llvm -o prog2.ll
@rem tools\llvm-link -S -f prog.ll prog2.ll -o theprog.ll
@rem tools\llc -march=fisc -filetype=obj -o prog.o theprog.ll

@tools\llc -march=fisc -filetype=obj -o prog.o prog.ll

@rm *.ll

@..\bin\Debug\fvm -t FISC -d --dump reg.10 --dump mem.0..8.32 -b .\prog.o