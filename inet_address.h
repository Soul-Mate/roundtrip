#ifndef TTCP_INET_ADDRESS_H
#define TTCP_INET_ADDRESS_H

#include "common.h"
#include <netdb.h> // gethostbyname_r

#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <exception>
#include <string>
#include <vector>

class InetAddressException : public std::exception
{
public:
    InetAddressException(const char *s)
    {
        __what = new char[strlen(s) + 1];
        strcpy(__what, s);
    };

    ~InetAddressException()
    {
        delete[] __what;
    };

    virtual char const *what() const noexcept { return __what; };

private:
    char *__what;
};

class InetAddress : copyable
{
public:
    InetAddress() = default;

    ~InetAddress() = default;

    explicit InetAddress(const StringArg &ip, in_port_t port) throw();

    explicit InetAddress(in_port_t port, bool loopback);

    explicit InetAddress(const sockaddr_in &addr);

    explicit InetAddress(in_port_t port);

    std::string toIP() const;

    std::string toEndpoint() const;

    inline const sockaddr_in &getSockAddr() const { return __sockaddr; };

    inline sockaddr_in &getSockAddrNonConst() { return __sockaddr; };

    inline sockaddr_in *getSockAddrPtrNonConst() { return &__sockaddr; }

    inline void setSockAddr(const sockaddr &addr) { std::memcpy(&__sockaddr, &addr, sizeof addr); };

    bool reslove(const std::string &host);

private:
    bool resloveERANGE(const std::string &host);

private:
    sockaddr_in __sockaddr;
};

#endif /* TTCP_INET_ADDRESS_H */