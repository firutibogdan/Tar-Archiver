
build: my_tar.o funct.o funct.h
	gcc my_tar.o funct.o -o my_tar

my_tar.o: my_tar.c
	gcc -c my_tar.c 

funct.o: funct.c funct.h
	gcc -c funct.c

clean:
	rm -rf *.o my_tar
