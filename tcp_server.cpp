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
    memset(events, 0, sizeof events);
    epoll_event connev;
    memset(&connev, 0, sizeof(epoll_event));
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
                connev.events = EPOLLIN | EPOLLET;
                check_error(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, current_socket->get_descriptor(), &connev), "CTL");

                new_connection(current_socket);
                if (!current_socket->is_open()) {
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, current_socket->get_descriptor(), &connev);
                    events_cout--;
                    sockets.erase(sockets.find(current_socket->get_descriptor()));
                    delete current_socket;
                }

                events_cout++;
            }
            else if (events[n].data.fd == event_fd)
            {
                fflush(stdout);
                running = false;
                break;
            } else
            {
                cout << "bad socket " << events[n].data.fd << std::endl;
                fflush(stdout);
                tcp_socket* current_socket = sockets[events[n].data.fd];

                cout << current_socket->is_open() << std::endl;

                func(current_socket);

                if (!current_socket->is_open()) {
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, current_socket->get_descriptor(), &connev);
                    events_cout--;
                    sockets.erase(sockets.find(current_socket->get_descriptor()));
                    delete current_socket;
                }
            }
        }

    }

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
    memset(&event_fd_ev, 0, sizeof(epoll_event));
    event_fd_ev.events = EPOLLIN | EPOLLET;
    event_fd_ev.data.fd = event_fd;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event_fd, &event_fd_ev);
}
