@CLS


@tools\clang -nostdlib -nostdinc -ffreestanding -fno-builtin -target fisc-unknown-unknown -S lib\entry.c -emit-llvm -o entry.ll

@tools\clang -nostdlib -nostdinc -ffreestanding -fno-builtin -target fisc-unknown-unknown -S prog.c -emit-llvm -o prog.ll


@tools\llvm-link -S -f entry.ll prog.ll -o a.ll


@tools\llc -march=fisc -filetype=obj -o a.o a.ll


@rm *.ll



@readelf -a -W a.o


@tools\llvm-objdump -s -d a.o