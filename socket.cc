#include "socket.h"
#include <iostream>
#include <algorithm>
Socket::Socket(Socket &&that)
{
    std::cout << "move constructor for Socket" << std::endl;
    __sockfd = that.__sockfd;
    that.__sockfd = -1;
}

Socket &Socket::operator=(Socket &&that)
{
    std::cout << "move assign operator for Socket" << std::endl;
    if (this == &that)
    {
        return *this;
    }

    std::swap(__sockfd, that.__sockfd);
    return *this;
}

Socket::~Socket()
{
    std::cout << "Socket::~Socket closed sockfd" << std::endl;
    close(__sockfd);
}

Socket Socket::createTCP()
{
    std::cout << "Socket::createTCP" << std::endl;
    int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    assert(sockfd >= 0);
    return Socket(sockfd);
}

Socket Socket::createUDP()
{
    int sockfd = ::socket(AF_INET, SOCK_DGRAM, 0);
    assert(sockfd >= 0);
    return Socket(sockfd);
}

void Socket::bind(const InetAddress &addr)
{
    std::cout << "Socket::bind" << std::endl;
    const auto saddr = addr.getSockAddr();
    int ret = ::bind(__sockfd, (const sockaddr *)&saddr, sizeof saddr);
    if (ret)
    {
        perror("Socket::bind");
        abort();
    }
}

void Socket::listen(int backlog)
{
    std::cout << "Socket::listen" << std::endl;
    int ret = ::listen(__sockfd, backlog);
    if (ret)
    {
        perror("Socket::listen");
        abort();
    }
}

int Socket::connect(const InetAddress &serverAddr) throw()
{
    int code = ::connect(__sockfd, (const struct sockaddr *)&serverAddr.getSockAddr(), sizeof serverAddr.getSockAddr());
    if (code == -1)
    {
        // TODO 更详细的错误处理
        throw SocketConnectException();
    }
}

void Socket::setRefuse(bool refuse)
{
    int optval = refuse ? 1 : 0;
    int ret = ::setsockopt(__sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
    if (ret)
    {
        perror("Socket::setRefuse");
    }
}

int Socket::shutdownWrite()
{
    return shutdown(__sockfd, SHUT_WR);
}

int Socket::read(void *buf, size_t len)
{
    return ::recv(__sockfd, buf, len, 0);
}

int Socket::write(const void *buf, size_t len)
{
    return ::send(__sockfd, buf, len, 0);
}

int Socket::readUDP(void *buf, size_t len, InetAddress &addr)
{
    struct sockaddr_in *addrPtr = &addr.getSockAddrNonConst();
    socklen_t addrLen = sizeof *addrPtr;
    return recvfrom(__sockfd, buf, len, 0,
                    (struct sockaddr *)addrPtr, &addrLen);
}

int Socket::writeUDP(const void *buf, size_t len, const InetAddress &addr)
{
    return sendto(__sockfd, buf, len, 0,
                  (const struct sockaddr *)(&addr.getSockAddr()), sizeof addr.getSockAddr());
}
