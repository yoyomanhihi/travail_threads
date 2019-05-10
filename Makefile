build:
	gcc -Wall -o cracker test.c -lpthread -Werror -std=c99
	gcc -Wall -o count.o count.c -Werror -std=c99
	gcc -Wall -o check.o check.c -Werror -std=c99
tests: cracker
	./count.o cisted
	./count.o -c cisted
	./check.o
	./cracker -t 8 01_4c_1k.bin -o File.txt
	./cracker -t 6 02_6c_5.bin
	./cracker -t 3 01_4c_1k.bin 02_6c_5.bin
	./cracker -t 950 01_4c_1k.bin 02_6c_5.bin
all:
	gcc -Wall -o cracker test.c -lpthread -Werror -std=c99
	gcc -Wall -o count.o count.c -Werror -std=c99
	gcc -Wall -o check.o check.c -Werror -std=c99
	./count.o cisted
	./count.o -c cisted
	./check.o
	./cracker -t 8 01_4c_1k.bin -o File.txt
	./cracker -t 6 02_6c_5.bin
	./cracker -t 3 01_4c_1k.bin 02_6c_5.bin
	./cracker -t 950 01_4c_1k.bin 02_6c_5.bin
clean:
	rm -rf *.o
	rm -rf cracker
