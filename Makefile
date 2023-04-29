webserver: src/webserver.h src/webserver.cc src/main.cc
	g++ src/webserver.cc src/main.cc -o bin/webserver
	./bin/webserver

client: src/client.cc
	g++ src/client.cc -o bin/client
	./bin/client