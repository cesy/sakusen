#include "udplisteningsocket.h"
#include "errorutils.h"

#include <sys/types.h>
#include <sys/socket.h>
//#include <sys/time.h>
#include <netinet/in.h>

#include <cerrno>
//#include <sstream>

using namespace sakusen::comms;

UDPListeningSocket::UDPListeningSocket(uint16 port) :
  UDPSocket(port)
{
  bzero(reinterpret_cast<char *>(&addr), sizeof(addr));
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  if (-1 == bind(sockfd, reinterpret_cast<const sockaddr *>(&addr), sizeof(addr)))
    Fatal("Error binding socket");
}