all: server client

sock.o: sock.cpp
	g++ -c sock.cpp -o sock.o

client.o: client.cpp
	g++ -c client.cpp -o client.o

Inetaddr.o: Inetaddr.cpp
	g++ -c Inetaddr.cpp -o Inetaddr.o

epoll.o: epoll.cpp
	g++ -c epoll.cpp -o epoll.o

Channel.o: Channel.cpp
	g++ -c Channel.cpp -o Channel.o

util.o: ../utils.cpp
	g++ -c ../utils.cpp -o util.o

server.o: server.cpp
	g++ -c server.cpp -o server.o

server: Inetaddr.o epoll.o Channel.o util.o sock.o server.o
	g++ Inetaddr.o epoll.o Channel.o util.o sock.o server.o -o server

client: util.o client.o
	g++ util.o client.o -o client

clean:
	rm -f *.o client server