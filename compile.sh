mkdir -p bin
gcc -o bin/main.o -c src/code/main.c
gcc -o bin/archives.o -c src/code/archives.c
gcc -o bin/msg-queue.o -c src/code/msg-queue.c

cd bin
gcc -o main.out main.o archives.o msg-queue.o