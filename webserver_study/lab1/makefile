
byteorder: byteorder.cc
	g++ byteorder.cc -o bin/byteorder
	./bin/byteorder

webserver: webserver/webserver.h webserver/webserver.cc webserver/main.cc
	g++ webserver/webserver.cc webserver/main.cc -o bin/webserver
	./bin/webserver

client: webserver/client.cc
	g++ webserver/client.cc -o bin/client
	./bin/client