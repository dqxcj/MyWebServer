CXX = g++
# 如果报错记得修改为c++14
CFLAGS = -std=c++14 -O2 -g

TARGET = server
OBJS = ./src/server/*.cc ./src/test/*.cc

all: $(OBJS) ./src/client/client.cc
	$(CXX) $(CFLAGS) $(OBJS) -o ./bin/$(TARGET)
	$(CXX) $(CFLAGS) ./src/client/client.cc -o ./bin/client


runserver: ./bin/$(TARGET)
	./bin/$(TARGET)

runclient: ./bin/client
	./bin/client


clean: 
	rm -rf ./bin/$(TARGET)
	rm -rf ./bin/client

