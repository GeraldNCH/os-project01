mkdir -p bin
gcc -o bin/main.o -c src/main.c
gcc -o bin/utils.o -c src/utils.c
gcc -o bin/files-list.o -c src/files-list.c

cd bin
gcc -o main.out main.o utils.o files-list.o