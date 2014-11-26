#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <exception>
#include <sys/epoll.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/eventfd.h>
#include <errno.h>
#include <map>
#include <algorithm>
#include <functional>

#include "cstdio"
#include "cstring"
#include "cstdlib"
#include "tcp_exception.h"
#include "tcp_socket.h"



struct tcp_server
{
public:
    ~tcp_server();
    bool begin_listening(char * address, char * service);
    void set_max_pending_connections(int max);

    void set_new_connection(void (*f) (tcp_socket*));
    void set_func(void (*f) (tcp_socket*));

    int event_fd;
private:
    bool running;
    const int MAX_EVENTS = 20;
    const int MAX_LENGTH = 20;
    int max_pending_connections = 10;

    int epoll_fd;
    int socket_fd;

    std::function<void (tcp_socket*)> new_connection;
    std::function<void (tcp_socket*)> func;

    static void check_error(int, const char*);
    static int create_and_bind(char * address, char * service);
    static int make_socket_non_blocking(int socket_fd);
    void create_event_fd();
};


#endif // TCP_SERVER_H
