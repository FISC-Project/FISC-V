@CLS


@..\toolchain\Windows\Tools\clang -Werror=implicit-function-declaration -fwritable-strings -nostdlib -nostdinc -ffreestanding -fno-builtin -target fisc-unknown-unknown -S lib\entry.c -emit-llvm -o entry.ll

@..\toolchain\Windows\Tools\clang -Werror=implicit-function-declaration -fwritable-strings -nostdlib -nostdinc -ffreestanding -fno-builtin -target fisc-unknown-unknown -S prog.c -emit-llvm -o prog.ll


@..\toolchain\Windows\Tools\llvm-link -S -f entry.ll prog.ll -o a.ll


@..\toolchain\Windows\Tools\llc -march=fisc -relocation-model=static -filetype=obj -o a.o a.ll


@rm *.ll



@readelf -a -W a.o


@..\toolchain\Windows\Tools\llvm-objdump -s -d a.o

@rm *.o