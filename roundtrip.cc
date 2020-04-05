#include "common.h"
#include "socket.h"
#include <cstring>
#include <cstdio>

void runRoundTripServer(const char *host, int port)
{
}

void runRoundTripClient(const char *host, int port)
{
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