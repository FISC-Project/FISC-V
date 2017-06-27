@CLS

@..\toolchain\Windows\Tools\flasm prog.fc -o a.o

@..\bin\Release\fvm -t FISC -d --dump reg.10 --dump mem.0..8.32 -c -b .\a.o
@rm *.o