#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

class WebServer {
public:
    WebServer(int);
private:
    bool InitSocket_();
};
#endif