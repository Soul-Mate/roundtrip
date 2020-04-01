#include "accept.h"

Acceptor::Acceptor(const InetAddress& saddr) 
 : __listenSock(std::move(Socket::createTCP()))
{
    __listenSock.setRefuse(true);
    __listenSock.bind(saddr);
    __listenSock.listen(128);
}

std::unique_ptr<TcpStream> Acceptor::accept()
{
    socklen_t peerLen;
    int peerfd = ::accept(__listenSock.get_sockfd(), (sockaddr*)&__peer.getSockAddrNonConst(), &peerLen);
    if (peerfd == -1) {
        perror("Acceptor::accept");
        abort();
    }

    // char buf[1024];
    // memset(buf, '1', 1024);
    // int nw = ::send(peerfd, buf, 1024, 0);
    // if (nw == -1) {
    //     perror("fail");
    //     abort();
    // }

    return std::unique_ptr<TcpStream>(new TcpStream(Socket(peerfd)));
}