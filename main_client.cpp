#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#define MAXDATASIZE 10

using namespace std;

int main()
{
    fork();
    fork();
    fork();

    struct addrinfo hints, *res;
    struct addrinfo* servinfo;
    socklen_t addr_size;
    struct sockaddr their_addr;
    char msg[MAXDATASIZE];


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo("127.0.0.1", "23010", &hints, &res);
    int s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    connect(s, res->ai_addr, res->ai_addrlen);
    int bytes = send(s, "zader", 5, 0);
    cout << bytes << "\n";

    bytes = recv(s, msg, MAXDATASIZE-1, 0);
    cout << bytes << "\n";
    cout << string(msg).substr(0,bytes) << "\n";
    fflush(stdout);
    close(s);
}

