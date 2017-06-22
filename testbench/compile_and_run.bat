@CLS


@tools\clang -nostdlib -nostdinc -ffreestanding -fno-builtin -target fisc-unknown-unknown -S lib\entry.c -emit-llvm -o entry.ll


@tools\clang -nostdlib -nostdinc -ffreestanding -fno-builtin -target fisc-unknown-unknown -S prog.c -emit-llvm -o prog.ll


@tools\llvm-link -S -f entry.ll prog.ll -o a.ll


@tools\llc -march=fisc -filetype=obj -o a.o a.ll



@rm *.ll



@..\bin\Debug\fvm -t FISC -d --dump reg.10 --dump mem.16384..16404.32 -c -b .\a.o