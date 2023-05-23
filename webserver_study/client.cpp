#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include "MyError.h"
using namespace std;

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    ErrIf(sockfd == -1, "client.cpp error: socket()");
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8889);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    ErrIf(connect(sockfd, (sockaddr *)&serv_addr, sizeof(serv_addr)) == -1, "client error: accept()");
    
    while(true) {
        char buf[1024];
        bzero(buf, sizeof(buf));
        cin >> buf;
        ssize_t write_len = write(sockfd, buf, sizeof(buf));
        ErrIf(write_len == -1, "client error: write to server");
        cout << "client send " << write_len << " bytes to server " << inet_ntoa(serv_addr.sin_addr) << endl;

        bzero(buf, sizeof(buf));
        ssize_t read_len = read(sockfd, buf, sizeof(buf));
        if(read_len > 0) {
            cout << read_len << " bytes message from server fd "<< sockfd << ": " << buf << endl;
        } else if(read_len == 0) {
            cout << "server fd " << sockfd << " disconnected" << endl;
            break;
        } else if(read_len == -1) {
            close(sockfd);
            ErrIf(true, "client error: read from server");
        }
    }

    close(sockfd);
    return 0;
}