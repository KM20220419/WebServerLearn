all: Server client
SRCS:= ./src/Channel.cpp ./src/Epoll.cpp ./src/Eventloop.cpp ./src/Interadd.cpp ./src/server.cpp ./src/Sock.cpp ./src/utils.cpp ./src/Acceptor.cpp ./src/Connection.cpp ./src/Buf.cpp

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
	rm -f *.o Server client

clean_2:
	find . -name "*.o" -type f -delete


test: ThreadPoolTest.cpp
	$(CXX) ThreadPoolTest.cpp -o test

# ./src/ThreadPool: ./src/ThreadPool.hpp
# 	$(CXX) -c ./src/ThreadPool.hpp -o ./src/ThreadPool

