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

class Command
{
public:
    Command() = default;
    
    ~Command() = default;
    
    void parse(int& argc, const char* argv[]) throw();
    
    const StringArg& getHost() const { return __host; };
    
    const int getPort() const { return static_cast<int>(__port); };
    
    const StringArg& getFilename() const { return __file; };

private:
    StringArg __host;
    in_port_t __port;
    StringArg __file;
};

void Command::parse(int& argc, const char* argv[]) throw()
{
    if (argc < 6) {
        throw std::invalid_argument("Usage: -l host -p port -f filepath.");
    }

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0 && i + 1 < argc) {
            __host = argv[++i];
        }

        if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            try 
            {
                __port = std::atoi(argv[++i]);
            } 
            catch(std::invalid_argument& e) 
            {
                throw e;
            }
        }

        if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            __file = argv[++i];
        }
    }

    if (__host.empty() || __port == 0 || __file.empty()) {
        throw  std::invalid_argument("parameter not enough.");
    }

}

class ThreadGuard: nocopyable
{
public:
    ThreadGuard(std::thread& t);

private:
    std::thread t;
};

// sender -l host -p port -f filepath
int main(int argc, const char* argv[]) {
    
    try
    {
        Command command;
        command.parse(argc, argv);
        std::printf("listen %s:%d, will wait 10 second and send %s file\n", 
            command.getHost().get_c_str(),command.getPort(), command.getFilename().get_c_str());


        Acceptor acceptor(InetAddress( command.getPort(), true));
        std::unique_ptr<TcpStream> stream = acceptor.accept();


        std::fstream fs;
        fs.open(command.getFilename().get_c_str(), std::ios::in | std::ios::binary);
        if (!fs.is_open()) {
            return 0;
        }

        std::this_thread::sleep_for(std::chrono::seconds(10));

        printf("Start sending file %s\n", command.getFilename().get_c_str());
        char buf[8192];
        while(!fs.eof()) {
            if (fs.read(buf, sizeof buf).fail() && !(fs.rdstate() & fs.eofbit)) {
                std::cout << "fstream read fail" << std::endl;
                return 0;
            }

            if (stream->sendAll(buf, fs.gcount()) != fs.gcount()) {
                std::cout << "sendAll fail" << std::endl;
                return 0;
            }
        }
        printf("Finish sending file %s\n", command.getFilename().get_c_str());

        stream->shutdownWrite();
        // TODO the client may not be clossed for some reason, and
        // the server can add some timeout logic.
        while(stream->receiveSome(buf, sizeof buf) > 0) {

        }
        std::printf("All done.\n");
    }
    catch(std::invalid_argument& e)
    {
        // std::cout << e.what() << std::endl;
    }
}