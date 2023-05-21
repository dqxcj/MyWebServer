
CC = g++
FLAGS = -g

serv: server.cpp
	$(CC) $< -o $@ $(FLAGS)

clnt: client.cpp
	$(CC) $< -o $@ $(FLAGS)
	
run: serv clnt
	./serv
	./clnt

.PHONY: clean
clean: 
	rm -rf serv clnt