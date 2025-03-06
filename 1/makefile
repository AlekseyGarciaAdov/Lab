CC=g++
CFLAGS=-c

myprog: converter.o
	$(CC) converter.cpp -o myprog
	./myprog
converter.o: converter.cpp
	$(CC) $(CFLAGS) converter.cpp

clean:
	rm -rf *.o myprog
