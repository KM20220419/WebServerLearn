all : server client

utils.o: ../utils.cpp
	g++ -c ../utils.cpp -o utils.o
epoll.o: epoll.cpp
	g++ -c epoll.cpp -o epoll.o

webserver.o: webserver.cpp
	g++ -c webserver.cpp -o webserver.o

client.o: client.cpp
	g++ -c client.cpp -o client.o

sockaddr.o: sockaddr.cpp
	g++ -c sockaddr.cpp -o sockaddr.o

server.o: server.cpp
	g++ -c server.cpp -o server.o

server: server.o epoll.o webserver.o sockaddr.o utils.o
	g++ server.o epoll.o webserver.o sockaddr.o utils.o -o server

client: client.o utils.o
	g++ client.o utils.o -o client

clean:
	rm -f *.o server client