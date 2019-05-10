build:
	gcc -Wall -o cracker src/test.c -lpthread -Werror -std=c99
	gcc -Wall -o count.o tests/count.c -Werror -std=c99
	gcc -Wall -o check.o tests/check.c -Werror -std=c99
tests: cracker
	./count.o cisted
	./count.o -c cisted
	./check.o
	./cracker -t 8 tests/01_4c_1k.bin -o tests/File.txt
	./cracker -t 6 tests/02_6c_5.bin
	./cracker -t 3 tests/01_4c_1k.bin tests/02_6c_5.bin
	./cracker -t 950 tests/01_4c_1k.bin tests/02_6c_5.bin
all:
	gcc -Wall -o cracker src/test.c -lpthread -Werror -std=c99
	gcc -Wall -o count.o tests/count.c -Werror -std=c99
	gcc -Wall -o check.o tests/check.c -Werror -std=c99
	./count.o cisted
	./count.o -c cisted
	./check.o
	./cracker -t 8 tests/01_4c_1k.bin -o tests/File.txt
	./cracker -t 6 tests/02_6c_5.bin
	./cracker -t 3 tests/01_4c_1k.bin tests/02_6c_5.bin
	./cracker -t 950 tests/01_4c_1k.bin tests/02_6c_5.bin
clean:
	rm -rf *.o
	rm -rf cracker
