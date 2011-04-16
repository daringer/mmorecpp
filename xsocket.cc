#include "xsocket.h"

using namespace std;
using namespace TOOLS::NET;

SocketException::SocketException(const std::string& msg)
  : BaseException(msg, "SocketException") {}

SocketStream::SocketStream(int fd) : fd(fd) {}

SocketStream::SocketStream(const SocketStream& obj) : fd(obj.fd) {}

SocketStream::~SocketStream() {
  close(fd);
}

void SocketStream::operator<<(const std::string& data) {
  send(data);
}

void SocketStream::send(const std::string& data) {
  send(data, data.length());
}

void SocketStream::send(const std::string& data, int len) {
  int n = write(fd, data.c_str(), len);
  if(n < 0)
    throw SocketException("Could not write to socket");
  //flush(fd);
}

const std::string SocketStream::get(int len) {
  char buf[len+1];
  bzero(&buf, len+1);

  int n = read(fd, &buf, len);
  if(n < 0)
    throw SocketException("Could not read from socket");
  return buf;
}

const std::string SocketStream::operator>>(int len) {
  return get(len);
}

Socket::Socket(SOCKET_TYPE type, int port) : fd(0), port(port), type(type), stream(0) {
  bzero((char*) &addr, sizeof(addr));

  if(type == TCP)
    fd = socket(AF_INET, SOCK_STREAM, 0);
  else if(type == UDP)
    fd = socket(AF_INET, SOCK_DGRAM, 0);
  else if(type == UNIX)
    fd = socket(AF_UNIX, SOCK_DGRAM, 0);

  if(fd < 0)
    throw SocketException("Could not create socket");
}

Socket::~Socket() {
  // hmmm no fd because SocketStream does the job
}

ServerSocket::ServerSocket(SOCKET_TYPE type, int port) : Socket(type, port) {
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  if(bind(fd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
    throw SocketException("Could not bind to address/port");

  stream = new SocketStream(fd);
}

ServerSocket::~ServerSocket() {
  if(stream != NULL)
    delete stream;
}

SocketStream* ServerSocket::listen() {
  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);

  bzero((char*) &client_addr, client_addr_len);
  ::listen(fd, MAX_LISTEN_QUEUE);

  int client_fd = accept(fd, (struct sockaddr*) &client_addr, (socklen_t*) &client_addr_len);
  if(client_fd < 0)
    throw SocketException("Error during accaptance of remote client");
  return new SocketStream(client_fd);
}

ClientSocket::ClientSocket(SOCKET_TYPE type, int port, const std::string& target) : Socket(type, port) {
  addr.sin_port = htons((unsigned short int) port);
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(target.c_str());

  socklen_t addr_len = sizeof(addr);
  if(connect(fd, (struct sockaddr*) &addr, sizeof(addr))) {
    perror("connect:");
    throw SocketException("Could not connect to given target");

  }

  stream = new SocketStream(fd);
}

ClientSocket::~ClientSocket() { }


void StreamSelecter::add_stream(SocketStream* stream) {
  streams.push_back(stream);
}

SocketStream* StreamSelecter::select() {
  return this->select(250000);
}

SocketStream* StreamSelecter::select(unsigned int usecs) {
  FD_ZERO(&socks);
  for(vector<SocketStream*>::iterator i=streams.begin(); i!=streams.end(); ++i)
    FD_SET((*i)->fd, &socks);

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = usecs;

  int readsocks = ::select(sizeof(socks)*8, &socks, (fd_set*) 0,
                           (fd_set*) 0, &timeout);

  if(readsocks == 0)
    return NULL;

  for(vector<SocketStream*>::iterator i=streams.begin(); i!=streams.end(); ++i)
    if(FD_ISSET((*i)->fd, &socks))
      return (*i);

  return NULL;
}
