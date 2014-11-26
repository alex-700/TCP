#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <signal.h>

#include <unistd.h>
#include "tcp_server.h"

#define MAXDATASIZE 10
#define MAX_EPOLL_EVENTS 10
using namespace std;
tcp_server* server;

static void sig_handler(int sig, siginfo_t *si, void* unused)
{
    cout << std::endl <<"signal" << std::endl;
    fflush(stdout);

    write(server->event_fd, "1", 2);

    cout << "signal ok" << std::endl;
    fflush(stdout);
}

void just(tcp_socket* x)
{

}

void reverse(tcp_socket* x)
{
    const char * msg = x->read_all();
    int bytes = strlen(msg);
    std::cout << bytes << std::endl;
    std::cout << msg << std::endl;
    char * msg2 = new char[bytes];
    for (int i = 0; i < bytes; i++) {
        msg2[i] = msg2[bytes - 1 - i];
    }

    x->write_data(msg2, bytes);
    x->close();
    std::cout << bytes << std::endl;
    std::cout << "/" << msg2 << "/" << std::endl;
    fflush(stdout);
}

int main()
{

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = sig_handler;

    sigaction(SIGINT, &sa, NULL);
    //sigaction(SIGINT, )


    server = new tcp_server();

    server->set_new_connection(just);
    server->set_func(reverse);

    server->begin_listening("127.0.0.1", "23009");

    delete server;

}

