mkdir -p bin
gcc -o bin/main.o -c src/main.c
gcc -o bin/utils.o -c src/utils.c

cd bin
gcc -o main.out main.o utils.o