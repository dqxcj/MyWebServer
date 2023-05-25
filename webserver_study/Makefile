
CC = g++
FLAGS = -g -pthread

serv: server.cpp InetAddress.cpp Socket.cpp Epoll.cpp Channel.cpp EventLoop.cpp Server.cpp Acceptor.cpp Connection.cpp Buffer.cpp
	$(CC) $^ -o $@ $(FLAGS)

clnt: client.cpp
	$(CC) $^ -o $@ $(FLAGS)

test: test.cpp InetAddress.cpp Socket.cpp Buffer.cpp 
	$(CC) $^ -o $@ $(FLAGS)

tp_test: tp_test.cpp
	$(CC) $^ -o $@ $(FLAGS)

.PHONY: clean
clean: 
	rm -rf serv clnt test