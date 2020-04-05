#ifndef ROUNDTRIP_MESSAGE_H
#define ROUNDTRIP_MESSAGE_H
#include <memory>
#include "inet_address.h"
#include "socket.h"

struct RoundTripMessage
{
    uint64_t t1;
    uint64_t t2;
} __attribute__((__packed__));

void roundTripMessageRequest(std::shared_ptr<Socket> sock, std::shared_ptr<InetAddress> addr);

uint64_t now();

#endif /* ROUNDTRIP_MESSAGE_H */
