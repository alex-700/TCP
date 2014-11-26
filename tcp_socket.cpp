#include "tcp_socket.h"


tcp_socket::tcp_socket()
{
}

tcp_socket::tcp_socket(int fd)
{
    this->fd = fd;
    open = true;
}

tcp_socket::~tcp_socket()
{
    if (is_open())
    {
        ::close(fd);
        open = false;
    }
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
    if (is_open())
    {
        int x = recv(fd, msg, max_size, 0);
        if (x == 0)
        {
            close(fd);
            open = false;
            return 0;
        }
        else
        {
            return x;
        }
    }
    else
    {
        throw tcp_exception("read from closed socket");
    }
}

std::string tcp_socket::read_all()
{
    if (is_open())
    {
        const char * msg;
        int size = 0;
        while (true)
        {
            int x = read_data(msg + size, CHUNK_SIZE);
            if (x != -1)
            {
                  size += x;
            }
            else
            {
                if (errno == EACCES)
                {
                    continue;
                }
                else
                {
                    break;
                }
            }
        }
    }
    else
    {
        throw tcp_exception("read from closed socket");
    }
}

int tcp_socket::write_data(const char *msg, int max_size)
{
    if (is_open())
    {
        return send(fd, msg, max_size);
    }
    else
    {
        throw tcp_exception("write to closed socket");
    }
}

void tcp_socket::write_all(const char *msg, int size)
{
    if (is_open())
    {
        int count = 0;
        while (count < size)
        {
            count += send(fd, msg + count, CHUNK_SIZE);
        }
    }
    else
    {
        throw tcp_exception("write to closed socket");
    }
}


