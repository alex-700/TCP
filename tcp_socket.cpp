#include "tcp_socket.h"


tcp_socket::tcp_socket()
{
}

tcp_socket::tcp_socket(int fd)
{
    this->fd = fd;
    open = true;
}

int tcp_socket::get_descriptor() const
{
    return fd;
}

void tcp_socket::close()
{
    ::close(fd);
    open = false;
}

bool tcp_socket::is_open() const
{
    return open;
}

void tcp_socket::make_non_blocking()
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        throw tcp_exception("error in fcntl 1");
    }
    flags |= O_NONBLOCK;
    int status = fcntl(fd, F_SETFL, flags);
    if (status == -1)
    {
        throw tcp_exception("error in fcntl 2");
    }
}

int tcp_socket::read_data(const char *msg, int max_size)
{

}

std::string tcp_socket::read_all()
{

}

int tcp_socket::write_data(const char *msg, int max_size)
{

}

void tcp_socket::write_all(const char *msg, int size)
{

}


