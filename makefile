huffmanEncoder: huffmanEncoder.o list.o decode.o
	gcc -o huffmanEncoder huffmanEncoder.o list.o decode.o

huffmanEncoder.o: huffmanEncoder.c list.h decode.h
	gcc -c huffmanEncoder.c

list.o: list.c list.h
	gcc -c list.c

decode.o: decode.c decode.h
	gcc -c decode.c

clean:
	rm huffmanEncoder *.o
