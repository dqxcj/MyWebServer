#include <string.h>
#include <unistd.h>
#include <functional>
#include <iostream>
#include "Buffer.h"
#include "InetAddress.h"
#include "MyError.h"
#include "Socket.h"
#include "ThreadPool.h"


void oneClient(int msgs, int wait, int i) {
  Socket *sock = new Socket();
  InetAddress *addr = new InetAddress("127.0.0.1", 8889);
  sock->Connect(addr);

  int sockfd = sock->GetFd();

  Buffer *sendBuffer = new Buffer();
  Buffer *readBuffer = new Buffer();

  sleep(wait);
  int count = 0;
  while (count < msgs) {
    sendBuffer->SetBuf("I'm client!");
    ssize_t write_bytes = write(sockfd, sendBuffer->CStr(), sendBuffer->Size());
    if (write_bytes == -1) {
      printf("socket already disconnected, can't write any more!\n");
      break;
    }
    int already_read = 0;
    char buf[1024];  // 这个buf大小无所谓
    while (true) {
      bzero(&buf, sizeof(buf));
      ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
      if (read_bytes > 0) {
        readBuffer->Write(buf, read_bytes);
        already_read += read_bytes;
      } else if (read_bytes == 0) {  // EOF
        printf("server disconnected!\n");
        exit(EXIT_SUCCESS);
      }
      if (already_read >= sendBuffer->Size()) {
        printf("count: %d, message from server: %s\n", count++, readBuffer->CStr());
        break;
      }
    }
    readBuffer->Clear();
  }
  delete addr;
  delete sock;
}

int main(int argc, char *argv[]) {
  int threads = 100;
  int msgs = 100;
  int wait = 0;
  int o;
  const char *optstring = "t:m:w:";
  while ((o = getopt(argc, argv, optstring)) != -1) {
    switch (o) {
      case 't':
        threads = atoi(optarg);
        break;
      case 'm':
        msgs = atoi(optarg);
        break;
      case 'w':
        wait = atoi(optarg);
        break;
      case '?':
        printf("error optopt: %c\n", optopt);
        printf("error opterr: %d\n", opterr);
        break;
    }
  }

  ThreadPool *poll = new ThreadPool(threads);
  for (int i = 0; i < threads; ++i) {
    poll->AddTask(oneClient, msgs, wait, i);
  }
  sleep(10);
  delete poll;
  return 0;
}
