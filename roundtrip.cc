#include "common.h"
#include "socket.h"
#include "message.h"
#include <string>
#include <cstring>
#include <cstdio>
#include <thread>
#include <memory>

void runRoundTripServer(const char *host, int port)
{
    // server bind
    InetAddress addr(port);
    if (addr.reslove(std::string(host)) == false)
    {
        std::printf("server reslove error\n");
        return;
    }
    Socket sock(Socket::createUDP());
    sock.bind(addr);

    //
    InetAddress peerAddr;
    struct RoundTripMessage msg;
    int nr = 0, nw = 0;
    while (true)
    {
        nr = sock.readUDP(&msg, sizeof msg, peerAddr);
#ifdef DEBUG
        std::printf("DEBUG: server recvfrom client request time: %jd\n", msg.t1);
#endif
        if (nr != sizeof msg)
        {
            printf("%s\n", strerror(errno));
            continue;
        }

        msg.t2 = now();
        nw = sock.writeUDP(&msg, sizeof msg, peerAddr);
        if (nw != sizeof msg)
        {
            std::printf("writeUDP failed\n");
            continue;
        }
#ifdef DEBUG
        std::printf("DEBUG: server sendto client response time: %jd\n", msg.t2);
#endif
    }
}

void runRoundTripClient(const char *host, int port)
{
    std::shared_ptr<InetAddress> addrPtr = std::make_shared<InetAddress>(InetAddress(port));
    if (addrPtr->reslove(std::string(host)) == false)
    {
        std::printf("reslove error\n");
        return;
    }

    std::shared_ptr<Socket> sockPtr = std::make_shared<Socket>(Socket::createUDP());

    std::thread request(roundTripMessageRequest, sockPtr, addrPtr);

    request.detach();

    struct RoundTripMessage msg;
    int nr = 0;
    while (true)
    {
        nr = sockPtr->readUDP(&msg, sizeof msg, *addrPtr);
        if (nr != sizeof msg)
        {
            std::printf("readUDP failed\n");
            continue;
        }

#ifdef DEBUG
        std::printf("DEBUG: client recvfrom server response time: %jd\n", msg.t1);
#endif

        uint64_t t3 = now();
        uint64_t avg = (t3 + msg.t1) / 2;
        std::printf("now %jd round trip %jd clock error %jd\n",
                    t3, avg - msg.t1, msg.t2 - avg);
    }
}

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: -l host port to start server\nUsage: -c host port to start client\n");
        return 0;
    }

    // -l host port
    if (strcmp(argv[1], "-l") == 0)
    {

        if (argc != 4)
        {
            printf("Usage: -l host port\n");
            return 0;
        }

        runRoundTripServer(argv[2], std::atoi(argv[3]));
    }
    else if (strcmp(argv[1], "-c") == 0)
    {
        // host port
        if (argc != 4)
        {
            printf("Usage: host port\n");
            return 0;
        }

        runRoundTripClient(argv[2], std::atoi(argv[3]));
    }
    else
    {
        printf("invalid argument\n");
    }
}