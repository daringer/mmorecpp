#ifndef XSOCKET_H
#define XSOCKET_H

#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include <iostream>
#include <vector>
#include <string>

#include "general.h"
#include "xstring.h"


namespace TOOLS {
namespace NET {

DEFINE_EXCEPTION(SocketException, BaseException)

#define MAX_BUF (1024*100)

/**
 * @brief the SocketStream handles the actual data
 *        writing and reading from the socket.
 */
class SocketStream {
  public:
    int fd;

    SocketStream(int fd);
    SocketStream(const SocketStream& obj);
    virtual ~SocketStream();

    const std::string get(int len);
    const std::string operator>>(int len);

    void send(const std::string& data);
    void send(const std::string& data, int len);
    void operator<<(const std::string& data);
};
/**
 * @brief the Socket, is an abstract OO approach
 *        to handle (actually wrap the C functions) sockets
 */
class Socket {
  protected:
    int fd, port;
    struct sockaddr_in addr;
    SOCKET_TYPE type;

  public:
    SocketStream* stream;

    Socket(SOCKET_TYPE type, int port);
    virtual ~Socket();
};
/**
 * @brief ServerSocket does the obvious, it sets up a server
 *        socket, which then can be used to listen to and recive connections
 */
class ServerSocket : public Socket {
  public:
    ServerSocket(SOCKET_TYPE type, int port);
    virtual ~ServerSocket();

    SocketStream* listen();
};
/**
 * @brief ClientSocket does the obvious, it can connect to any socket
 */
class ClientSocket : public Socket {
  public:
    ClientSocket(SOCKET_TYPE type, int port, const std::string& target);
    virtual ~ClientSocket();
};
/**
 * @brief StreamSelecter wants an arbitrary number of (Socket)Streams as
 *        input - it wraps the select() mechanism.
 */
class StreamSelecter {
  private:
    fd_set socks;
    std::vector<SocketStream*> streams;

  public:
    void add_stream(SocketStream* stream);
    void remove_stream(SocketStream* stream);
    SocketStream* select();
    SocketStream* select(unsigned int usecs);
};

}
}

#endif
