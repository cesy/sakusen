#include "socket.h"
#include "libsakusen-global.h"
#include "stringutils.h"
#include <list>

#ifdef WIN32
#include <winsock2.h>

#else /* BSD sockets */

#include "unixdatagramsocket.h"

#endif //WIN32
#include "udpsocket.h"

using namespace std;

using namespace sakusen::comms;

void Socket::socketsInit(void)
{
#ifdef WIN32
  WSADATA wsaData;
  int iResult;
  iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
  if ( iResult != NO_ERROR )
    printf("Error at WSAStartup()\n");
#endif
}

Socket* Socket::newConnectionToAddress(const String& address)
{
  list<String> addressComponents = stringUtils_split(address, ADDR_DELIM);
  if (addressComponents.empty()) {
    return NULL;
  }
  String type = addressComponents.front();
  addressComponents.pop_front();
  if (type == "unix") {
#ifdef WIN32
    return NULL; /* Unix sockets not supported on Windows */
#else
    return UnixDatagramSocket::newConnectionToAddress(addressComponents);
#endif
  } else if (type == "udp") {
    return UDPSocket::newConnectionToAddress(addressComponents);
  } else {
    Debug("Unexpected type: " << type);
    return NULL;
  }
}

Socket* Socket::newBindingToAddress(const String& address)
{
  list<String> addressComponents = stringUtils_split(address, ADDR_DELIM);
  if (addressComponents.empty()) {
    return NULL;
  }
  String type = addressComponents.front();
  addressComponents.pop_front();
  if (type == "unix") {
#ifdef WIN32
    return NULL; /* Unix sockets not supported on Windows */
#else
    return UnixDatagramSocket::newBindingToAddress(addressComponents);
#endif
  } else if (type == "udp") {
    return UDPSocket::newBindingToAddress(addressComponents);
  } else {
    Debug("Unexpected type: " << type);
    return NULL;
  }
}

Socket::Socket()
{
}

Socket::Socket(const Socket&)
{
}

