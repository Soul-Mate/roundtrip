#include "tcp_stream.h"

TcpStream::TcpStream(Socket &&socket)
    : __sock(std::move(socket))
{
}

void TcpStream::shutdownWrite()
{
    int code = __sock.shutdownWrite();
    if (code == -1)
    {
        perror("TcpStream::shutdownWrite");
        abort();
    }
}

int TcpStream::receiveSome(void *buf, size_t len)
{
    int readn = 0, nr = 0;
    while (readn < len)
    {
        nr = __sock.read(static_cast<char *>(buf) + readn, len - readn);
        std::cout << "nr = " << nr << std::endl;
        if (nr == 0)
        {
            return 0;
        }
        else if (nr > 0)
        {
            readn += nr;
        }
        else if (nr == -1 && errno != EINTR)
        {
            // TODO
            perror("TcpStream::receiveSome");
            abort();
        }
    }

    return readn;
}

int TcpStream::receiveAll(void *buf, size_t len)
{
    int read_n = recv(__sock.get_sockfd(), buf, len, MSG_WAITALL);
    if (read_n == 0)
    {
        // TODO对端关闭
    }
    if (read_n < 0)
    {
        perror("TcpStream::receiveAll");
        abort();
    }

    return read_n;
}

int TcpStream::sendAll(const void *buf, size_t len)
{
    int write_n = 0;
    int wn = 0;
    while (write_n < len)
    {
        wn = __sock.write(static_cast<const char *>(buf) + write_n, len - write_n);
        if (wn > 0)
        {
            write_n += wn;
        }
        else if (wn == 0)
        {
            return 0;
        }
        else if (errno != EINTR)
        {
            std::cout << strerror(errno) << std::endl;
            return write_n;
        }
    }

    return write_n;
}

std::unique_ptr<TcpStream> TcpStream::connect(const InetAddress &serverAddr)
{
    Socket s(std::move(Socket::createTCP()));
    s.connect(serverAddr);

    return std::unique_ptr<TcpStream>(new TcpStream(std::move(s)));
}