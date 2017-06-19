@CLS

@tools\flasm prog.fc -o prog.o

@..\bin\Debug\fvm -t FISC -d --dump reg.10 --dump mem.0..8.32 -b .\prog.o