build:
	gcc -Wall -o cracker test.c -lpthread -Werror -std=c99
	gcc -Wall -o count count.c -std=c99 -Werror
tests: cracker
	./count 
	./cracker -t 8 01_4c_1k.bin -o File.txt
	./cracker -t 6 02_6c_5.bin
	./cracker -t 3 01_4c_1k.bin 02_6c_5.bin
	./cracker -t 950 01_4c_1k.bin 02_6c_5.bin
all:
	gcc -Wall -o cracker test.c -lpthread -Werror -std=c99
	./cracker -c -t 4 01_4c_1k.bin 02_6c_5.bin
	./cracker -t 700 01_4c_1k.bin -o File1.txt
	./cracker -t 7 02_6c_5.bin -o File2.txt
clean:
	rm -rf *.o
	rm -rf cracker
