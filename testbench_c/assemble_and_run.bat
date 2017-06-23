@CLS

@tools\flasm prog.fc -o a.o

@..\bin\Debug\fvm -t FISC -d --dump reg.10 --dump mem.0..8.32 -c -b .\a.o
@rm *.o