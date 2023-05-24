
CC = g++
FLAGS = -g

serv: server.cpp InetAddress.cpp Socket.cpp Epoll.cpp Channel.cpp EventLoop.cpp Server.cpp Acceptor.cpp Connection.cpp
	$(CC) $^ -o $@ $(FLAGS)

clnt: client.cpp
	$(CC) $^ -o $@ $(FLAGS)
	
run: serv clnt
	./serv
	./clnt

.PHONY: clean
clean: 
	rm -rf serv clnt