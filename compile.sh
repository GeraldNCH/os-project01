mkdir -p bin
gcc -o bin/main.o -c src/main.c
gcc -o bin/copy-file.o -c src/copy-file.c
cd bin
gcc -o main.out main.o copy-file.o