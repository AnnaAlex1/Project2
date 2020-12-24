CC=gcc
CFLAGS=-Wall -g

project1: readX.o readW.o hashTable.o ids_list.o result.o tfidf.o BOW.o words.o bowvector.o main.o
	$(CC) $(CFLAGS) -o project1 readX.o readW.o hashTable.o ids_list.o result.o tfidf.o BOW.o words.o bowvector.o main.o -lm

test_project1: readX.o readW.o hashTable.o ids_list.o result.o test.o 
	$(CC) $(CFLAGS) -o test_project1 readX.o readW.o hashTable.o ids_list.o result.o test.o
	
test_project2: readX.o readW.o hashTable.o ids_list.o result.o test2.o 
	$(CC) $(CFLAGS) -o test_project1 readX.o readW.o hashTable.o ids_list.o result.o test2.o	

bowvector.o: bowvector.c
	$(CC) $(CFLAGS) -c bowvector.c

words.o: words.c
	$(CC) $(CFLAGS) -c words.c	

BOW.o: BOW.c
	$(CC) $(CFLAGS) -c BOW.c


tfidf.o: tfidf.c
	$(CC) $(CFLAGS) -c tfidf.c

test2.o: test2.c
	$(CC) $(CFLAGS) -c test2.c

test.o: test.c
	$(CC) $(CFLAGS) -c test.c

readX.o: readX.c
	$(CC) $(CFLAGS) -c readX.c

readW.o: readW.c
	$(CC) $(CFLAGS) -c readW.c

hashTable.o: hashTable.c
	$(CC) $(CFLAGS) -c hashTable.c

ids_list.o: ids_list.c
	$(CC) $(CFLAGS) -c ids_list.c

result.o: result.c
	$(CC) $(CFLAGS) -c result.c

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

.PHONY: clean

clean:
	rm -f *.o project1 output* test_project1
