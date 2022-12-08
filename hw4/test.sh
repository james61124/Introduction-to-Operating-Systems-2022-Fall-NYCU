gcc -c -fPIC bf.c -o bf.o
gcc bf.o -shared -o bf.so
gcc main.c -o main  
LD_PRELOAD=./bf.so ./main