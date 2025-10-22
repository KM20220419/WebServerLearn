all: Server client
SRCS:= ./src/Channel.cpp ./src/Epoll.cpp ./src/Eventloop.cpp ./src/Interadd.cpp ./src/server.cpp ./src/Sock.cpp ./src/utils.cpp

OBJS:= $(SRCS:.cpp=.o)

CXX:= g++

./src/%.o: ./src/%.cpp
	$(CXX) -c $< -o $@

Server.o: Server.cpp
	$(CXX) -c Server.cpp -o Server.o

client.o: client.cpp
	$(CXX) -c client.cpp -o client.o

Server: $(OBJS) Server.o
	$(CXX) $(OBJS) Server.o -o Server

client: client.o ./src/utils.o
	$(CXX) client.o ./src/utils.o -o client

clean:
	rm -f $(OBJS) $(TARGET) client