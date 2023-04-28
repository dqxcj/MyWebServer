## 项目描述 
本项目是一个基于 Linux 平台的轻量级 HTTP 服务器，采用了 Reactor 高并发模型，使用 Epoll 达到高效的 I/O 复用，并使用自动增长缓冲区、定时器、异步日志等技术实现了稳定的性能和可靠的运行。

## 主要工作
1.  实现了 HTTP 协议的解析，支持 GET 和 POST 请求，长连接和断开连接。
2.  使用 epoll 多路复用机制高效处理 IO，监听和处理客户端连接事件和数据传输事件。
3.  使用 Reactor 高并发模型处理多个连接，使得服务器可以同时处理多个客户端连接。
4.  实现了自动增长缓冲区，支持动态调整缓冲区大小以应对不同大小的请求。
5.  实现了定时器，使用小根堆维护连接的超时时间，确保连接不会因为长时间未收到数据而被关闭。
6.  设计并实现了异步日志模块，使用单例模式和阻塞队列实现日志的异步写入，避免了同步写入的性能损失。
7.  使用 WebBench 等工具对服务器进行了性能测试和压力测试，确保服务器的稳定性和可靠性。

## 项目难点
1.  高并发场景下需要设计高效的事件驱动模型和 IO 处理机制。
2.  对于长连接和空闲连接，需要设计合理的定时器策略，避免占用过多服务器资源，同时保证连接的可靠性和稳定性。
3.  在多线程环境下，需要保证数据的同步和互斥，避免因为线程安全问题导致服务器崩溃或数据错误。
4.  HTTP 请求处理需要处理各种不同的请求类型和请求参数，需要进行充分的测试和验证，确保服务器能够正确地响应各种请求。

## 个人收获
1.  熟悉了 Linux 平台下的网络编程，加深了对 socket、Epoll、TCP/IP 等网络通信原理的理解。
2.  学习了 Reactor 高并发模型的实现方式，提高了对并发编程的理解。
3.  学会了如何使用自动增长缓冲区、定时器和异步日志等技术来提高服务器的性能和可靠性。
4.  掌握了如何使用 WebBench 等工具对服务器进行性能测试和压力测试，加深了对服务器稳定性和可靠性的认识。