#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>

#include "tcp_exception.h"

class tcp_socket
{
public:
    tcp_socket();
    tcp_socket(int fd);

    ~tcp_socket();

    int get_descriptor() const;
    void close();
    bool is_open() const;
    void make_non_blocking();

    int read_data(char* msg, int max_size);
    int write_data(const char* msg, int max_size);
    const char * read_all();
    void write_all(const char* msg, int size);
private:
    static const int CHUNK_SIZE = 512;
    static const int MAX_SIZE = 100000;
    int fd;
    bool open;
};

#endif // TCP_SOCKET_H
