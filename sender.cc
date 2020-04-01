#include "common.h"
#include "tcp_stream.h"
#include "inet_address.h"
#include "accept.h"
#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdio>
#include <memory>
#include <fstream>
#include <chrono>
#include <thread>
#include <csignal>

class Command
{
public:
    Command() = default;

    ~Command() = default;

    void parse(int &argc, const char *argv[]) throw();

    const StringArg &getHost() const { return __host; };

    const int getPort() const { return static_cast<int>(__port); };

    const StringArg &getFilename() const { return __file; };

private:
    StringArg __host;
    in_port_t __port;
    StringArg __file;
};

void signal_handler(int signal_num)
{
    if (signal_num == SIGPIPE)
    {
        std::cout << "The interrupt signal is (SIGPIPE). \n";
    }
}

void Command::parse(int &argc, const char *argv[]) throw()
{
    if (argc < 6)
    {
        throw std::invalid_argument("Usage: -l host -p port -f filepath.");
    }

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-l") == 0 && i + 1 < argc)
        {
            __host = argv[++i];
        }

        if (strcmp(argv[i], "-p") == 0 && i + 1 < argc)
        {
            try
            {
                __port = std::atoi(argv[++i]);
            }
            catch (std::invalid_argument &e)
            {
                throw e;
            }
        }

        if (strcmp(argv[i], "-f") == 0 && i + 1 < argc)
        {
            __file = argv[++i];
        }
    }

    if (__host.empty() || __port == 0 || __file.empty())
    {
        throw std::invalid_argument("parameter not enough.");
    }
}

void sender(std::shared_ptr<Command> command, std::unique_ptr<TcpStream> stream)
{
    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::fstream fs;

    fs.open(command->getFilename().get_c_str(), std::ios::in | std::ios::binary);
    if (!fs.is_open())
    {
        return;
    }

    printf("Start sending file %s\n", command->getFilename().get_c_str());
    char buf[8192];
    int nr = 0;
    while (!fs.eof())
    {
        if (fs.read(buf, sizeof buf).fail() && !(fs.rdstate() & fs.eofbit))
        {
            std::cout << "fstream read fail" << std::endl;
            return;
        }

        nr = stream->sendAll(buf, fs.gcount());

        if (nr == 0)
        {
            std::cout << "client closed" << std::endl;
            return;
        }

        if (nr != fs.gcount())
        {
            return;
        }
    }
    printf("Finish sending file %s\n", command->getFilename().get_c_str());

    stream->shutdownWrite();
    // TODO the client may not be clossed for some reason, and
    // the server can add some timeout logic.
    while (stream->receiveSome(buf, sizeof buf) > 0)
    {
    }
    std::printf("All done.\n");
}

// sender -l host -p port -f filepath
int main(int argc, const char *argv[])
{
    signal(SIGPIPE, signal_handler);
    try
    {
        std::shared_ptr<Command> commandPtr = std::make_shared<Command>();
        commandPtr->parse(argc, argv);

        std::printf("listen %s:%d, will wait 10 second and send %s file\n",
                    commandPtr->getHost().get_c_str(),
                    commandPtr->getPort(),
                    commandPtr->getFilename().get_c_str());

        int n = 0;
        while (true)
        {
            Acceptor acceptor(InetAddress(commandPtr->getPort(), true));
            std::unique_ptr<TcpStream> stream = acceptor.accept();
            std::printf("accept %d client.\n", ++n);

            std::thread t(sender, commandPtr, std::move(stream));
            t.detach();

            std::printf("command shared count: %ld.\n", commandPtr.use_count());
        }

        // std::fstream fs;
        // fs.open(command.getFilename().get_c_str(), std::ios::in | std::ios::binary);
        // if (!fs.is_open())
        // {
        //     return 0;
        // }

        // std::this_thread::sleep_for(std::chrono::seconds(10));

        // printf("Start sending file %s\n", command.getFilename().get_c_str());
        // char buf[8192];
        // while (!fs.eof())
        // {
        //     if (fs.read(buf, sizeof buf).fail() && !(fs.rdstate() & fs.eofbit))
        //     {
        //         std::cout << "fstream read fail" << std::endl;
        //         return 0;
        //     }

        //     if (stream->sendAll(buf, fs.gcount()) != fs.gcount())
        //     {
        //         std::cout << "sendAll fail" << std::endl;
        //         return 0;
        //     }
        // }
        // printf("Finish sending file %s\n", command.getFilename().get_c_str());

        // stream->shutdownWrite();
        // // TODO the client may not be clossed for some reason, and
        // // the server can add some timeout logic.
        // while (stream->receiveSome(buf, sizeof buf) > 0)
        // {
        // }
        // std::printf("All done.\n");
    }
    catch (std::invalid_argument &e)
    {
        // std::cout << e.what() << std::endl;
    }
}