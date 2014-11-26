#include "tcp_exception.h"

tcp_exception::tcp_exception(const char * ms)
{
    message = ms;
}

const char * tcp_exception::what() const throw()
{
    return message;
}

