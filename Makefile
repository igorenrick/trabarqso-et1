all: servidor.o cliente.o
	g++ -o servidor servidor.o -lpthread
	g++ -o cliente cliente.o

servidor.o: servidor.cpp
	g++ -c servidor.cpp -lpthread

cliente.o: cliente.cpp
	g++ -c cliente.cpp

clean:
	rm *.o
