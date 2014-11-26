#ifndef TCP_EXCEPTION_H
#define TCP_EXCEPTION_H

#include <exception>

struct tcp_exception: public std::exception
{
public:
    tcp_exception(const char* message);
    virtual const char* what() const throw();
private:
    const char* message;
};

#endif // TCP_EXCEPTION_H
