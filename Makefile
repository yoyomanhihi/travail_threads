make:
	gcc -Wall -o cracker test.c -lpthread -Werror -std=c99
tests: cracker
	./cracker -t 9 01_4c_1k.bin 02_6c_5.bin
all:
	gcc -Wall -o cracker test.c -lpthread -Werror -std=c99
	./cracker -c -t 4 01_4c_1k.bin 02_6c_5.bin
clean:
	rm ./-testfile.o
