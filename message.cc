#include "message.h"
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <memory>

void roundTripMessageRequest(std::shared_ptr<Socket> sock, std::shared_ptr<InetAddress> addr)
{
    struct RoundTripMessage msg;
    int nw = 0;
    while (true)
    {
        msg.t1 = now();
        nw = sock->writeUDP(&msg, sizeof msg, *addr);
        if (nw != sizeof msg)
        {
            std::printf("sendto failed\n");
            continue;
        }
#ifdef DEBUG
        std::printf("DEBUG: client sendto server request time: %jd\n", msg.t1);
#endif

        ::sleep(2);
    }
}

uint64_t now()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}