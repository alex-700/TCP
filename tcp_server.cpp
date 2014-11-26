#include "tcp_server.h"
using namespace std;

tcp_server::~tcp_server() {

}


bool tcp_server::begin_listening(char* address, char* service)
{
    //char their_msg[MAX_LENGTH];

    epoll_fd = epoll_create(MAX_EVENTS);
    check_error(epoll_fd, "epoll_fd not created");

    create_event_fd();
    cout << event_fd << std::endl;
    fflush(stdout);

    socket_fd = create_and_bind(address, service);
    check_error(socket_fd, "socket_fd not created");
    check_error(listen(socket_fd, max_pending_connections), "listen");

    epoll_event listen_ev;
    listen_ev.events = EPOLLIN | EPOLLPRI | EPOLLET;
    listen_ev.data.fd = socket_fd;
    check_error(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &listen_ev), "epoll_ctl");

    socklen_t client;
    epoll_event events[MAX_EVENTS];
    epoll_event connev;
    sockaddr cliaddr;

    int events_cout = 2;
    running = true;
    std::map<int, tcp_socket*> sockets;

    while (running)
    { 
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1 && errno == EINTR)
        {
            cout << "stupid exit" << endl;
            running = false;
            break;
        }

        cout << nfds << " new events" << std::endl;
        fflush(stdout);

        for (int n = 0; n < nfds; n++)
        {
            if (events[n].data.fd == socket_fd)
            {

                client = sizeof cliaddr;
                int connfd = accept(socket_fd, (sockaddr*) &cliaddr, &client);
                cout << "new connection accepted connfd= " << connfd << std::endl;
                fflush(stdout);

                if (events_cout == max_pending_connections)
                {
                    std::cout << "full";
                    close(connfd);
                    continue;
                }

                tcp_socket* current_socket = new tcp_socket(connfd);
                current_socket->make_non_blocking();
                sockets[connfd] = current_socket;

                //make_socket_non_blocking(connfd);

                connev.data.fd = current_socket->get_descriptor();
                connev.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP;
                check_error(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, current_socket->get_descriptor(), &connev), "CTL");

                //connect function
                //check on close

                events_cout++;
            }
            else if (events[n].data.fd == event_fd)
            {
                cout << "YES";
                fflush(stdout);
                running = false;

                break;
            } else
            {
                tcp_socket* current_socket = sockets[events[n].data.fd];

                // function

                /*int bytes = recv(current_socket->get_descriptor(), their_msg, MAX_LENGTH, 0);
                std::cout << bytes << std::endl;
                std::cout << their_msg << std::endl;

                for (int i = 0, j = bytes - 1; i < j; i++, j--) {
                    char w = their_msg[i];
                    their_msg[i] = their_msg[j];
                    their_msg[j] = w;
                }

                bytes = send(current_socket->get_descriptor(), their_msg, bytes, 0);

                std::cout << bytes << std::endl;*/
                func(current_socket);
                //current_socket->close();

                // function

                if (!current_socket->is_open()) {
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, current_socket->get_descriptor(), &connev);
                    events_cout--;
                    sockets.erase(sockets.find(current_socket->get_descriptor()));
                }
            }
        }

    }
    //close epoll_fd, event_fd, sockets

    close(socket_fd);
    close(epoll_fd);
    close(event_fd);
    for (map<int, tcp_socket*>::iterator i = sockets.begin(); i != sockets.end(); i++)
    {
       delete i->second;
    }



}

void tcp_server::set_max_pending_connections(int max)
{
    max_pending_connections = max;
}

void tcp_server::set_new_connection(void (*f)(tcp_socket*))
{
    new_connection = f;
}

void tcp_server::set_func(void (*f)(tcp_socket*))
{
    func = f;
}

void tcp_server::check_error(int x, const char * msg)
{
    if (x == -1)
    {
        throw tcp_exception(msg);
    }
}

int tcp_server::create_and_bind(char* address, char* service)
{
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    check_error(getaddrinfo(address, service, &hints, &res), "getaddrinfo in create and bind");

    int s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    make_socket_non_blocking(s);

    check_error(bind(s, res->ai_addr, res->ai_addrlen), "bind in create and bind");
    return s;
    /*
     * struct addrinfo hints, *res;
    struct addrinfo* servinfo;
    socklen_t addr_size;
    struct sockaddr their_addr;
    char their_msg[MAXDATASIZE];
    struct tcp_server x;

    int efd = epoll_create(MAX_EPOLL_EVENTS);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo("127.0.0.1", "1509", &hints, &res);

    int s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    setnonblocking(s);

    bind(s, res->ai_addr, res->ai_addrlen);
    listen(s, 10);

    struct epoll_event listenev;
    listenev.events = EPOLLIN | EPOLLPRI | EPOLLET;
    listenev.data.fd = s;
    epoll_ctl(efd, EPOLL_CTL_ADD, s, &listenev);

    socklen_t client;
    struct epoll_event events[MAX_EPOLL_EVENTS];
    struct epoll_event connev;
    struct sockaddr cliaddr;
    int events_cout = 1;
    for (;;)
    {
        int nfds = epoll_wait(efd, events, MAX_EPOLL_EVENTS, -1);
        for (int n = 0; n < nfds; n++)
        {
            if (events[n].data.fd == s)
            {
                client = sizeof cliaddr;
                int connfd = accept(s, (struct sockaddr*) &cliaddr, &client);
                if (events_cout == -1)
                {
                    cout << "full";
                    close(connfd);
                    continue;
                }

                setnonblocking(connfd);
                connev.data.fd = connfd;
                connev.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP;
                epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &connev);
                events_cout++;
            } else {
                int fd = events[n].data.fd;
                int bytes = recv(fd, their_msg, MAXDATASIZE, 0);
                cout << bytes << std::endl;
                cout << their_msg << std::endl;
                for (int i = 0, j = bytes - 1; i < j; i++, j--) {
                    char w = their_msg[i];
                    their_msg[i] = their_msg[j];
                    their_msg[j] = w;
                }
                bytes = send(fd, their_msg, bytes, 0);
                cout << bytes << std::endl;
                epoll_ct
l(efd, EPOLL_CTL_DEL, fd, &connev);

                events_cout--;
                close(fd);
            }
        }
    }
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(address, service, &hints, &res);

    int s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    return bind(s, res->ai_addr, res->ai_addrlen);
*/
}

int tcp_server::make_socket_non_blocking(int socket_fd)
{
    int flags = fcntl(socket_fd, F_GETFL, 0);
    if (flags == -1)
    {
        //TODO: check error
        return -1;
    }
    flags |= O_NONBLOCK;
    int status = fcntl(socket_fd, F_SETFL, flags);
    if (status == -1)
        {
        //TODO: check error
        return -1;
    }
    return 0;
}

void tcp_server::create_event_fd()
{
    event_fd = eventfd(0, 0);

    make_socket_non_blocking(event_fd);

    struct epoll_event event_fd_ev;
    event_fd_ev.events = EPOLLIN | EPOLLET;
    event_fd_ev.data.fd = event_fd;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event_fd, &event_fd_ev);
}
