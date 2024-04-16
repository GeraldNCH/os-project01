mkdir -p bin
gcc -o bin/main.o -c src/main.c
gcc -o bin/copy-file.o -c src/copy-file.c
gcc -o bin/utils.o -c src/utils.c

cd bin
gcc -o main.out main.o copy-file.o utils.o