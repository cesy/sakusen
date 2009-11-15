#include "tcpsocket.h"
#include "tcpconnectingsocket.h"
#include "tcplisteningsocket.h"
#include "stringutils.h"
#include "errorutils.h"
#include "socketexn.h"

#ifdef WIN32
#include "wsabsd.h"
#define NativeReceiveReturnType int
#else // BSD sockets
#define NativeReceiveReturnType ssize_t
#endif

namespace sakusen {
namespace comms {

namespace {

typedef uint32 BufferLenType;
inline BufferLenType hton_buffer(BufferLenType i) { return htonl(i); }
inline BufferLenType ntoh_buffer(BufferLenType i) { return ntohl(i); }

}

Socket::Ptr TCPSocket::newConnectionToAddress(std::list<String>& address)
{
  String hostname;
  uint16 port;

  interpretAddress(address, &hostname, &port);

  if (hostname.empty()) {
    return Socket::Ptr();
  }

  return Socket::Ptr(new TCPConnectingSocket(hostname, port));
}

Socket::Ptr TCPSocket::newBindingToAddress(std::list<String>& address)
{
  String hostname;
  uint16 port;

  interpretAddress(address, &hostname, &port);

  if (hostname.empty()) {
    return Socket::Ptr();
  }

  /* hostname is just ignored except for checking the error condition */
  return Socket::Ptr(new TCPListeningSocket(port));
}

TCPSocket::TCPSocket() :
  buffer(NULL),
  bufferLength(0),
  bufferCapacity(0)
{
  sockfd = ::socket(PF_INET, SOCK_STREAM, 0);
}

TCPSocket::TCPSocket(uint16 myPort):
  IPSocket(myPort),
  buffer(NULL),
  bufferLength(0),
  bufferCapacity(0)
{
  sockfd = ::socket(PF_INET, SOCK_STREAM, 0);
}

TCPSocket::TCPSocket(NativeSocket s, const sockaddr_in& peerAddress) :
  IPSocket(s, peerAddress),
  buffer(NULL),
  bufferLength(0),
  bufferCapacity(0)
{
}

void TCPSocket::send(const void* buf, size_t len)
{
  /* Prefix the message by its length */
  uint8* longerBuf = new uint8[len+sizeof(BufferLenType)];
  if (len > std::numeric_limits<BufferLenType>::max()) {
    Fatal("message too long for protocol ("<<len<<" bytes");
  }
  *reinterpret_cast<BufferLenType*>(longerBuf) = hton_buffer(len);
  memcpy(longerBuf+sizeof(BufferLenType), buf, len);
  
  int retVal = ::send(
      sockfd, reinterpret_cast<char*>(longerBuf), len+sizeof(BufferLenType),
      MSG_NOSIGNAL
    );
  if (retVal == -1) {
    switch (socket_errno) {
      case ENOTCONN:
      case ECONNREFUSED:
      case ECONNABORTED:
      case ECONNRESET:
        throw SocketClosedExn();
        break;
      case EPIPE:
        throw SocketExn("Socket has been closed locally");
        break;
      default:
        Fatal("error " << errorUtils_parseErrno(socket_errno) << " sending message");
        break;
    }
  }

  delete[] longerBuf;
}

void TCPSocket::sendTo(const void* /*buf*/, size_t /*len*/, const String& /*address*/)
{
  Fatal("not implemented");
}

size_t TCPSocket::receive(void* outBuf, size_t len)
{
  /* We fetch as much as possible and buffer it locally */
  do {
    if (bufferCapacity > bufferLength) {
      NativeReceiveReturnType received = recv(
          sockfd, reinterpret_cast<char*>(buffer+bufferLength),
          bufferCapacity - bufferLength, 0
        );
      if (received == -1) {
        if (socket_errno == EAGAIN || socket_errno == EWOULDBLOCK) {
          break;
        }
        if (socket_errno == ECONNABORTED) {
          throw SocketClosedExn();
        }
        if (socket_errno == ECONNRESET) {
           throw SocketClosedExn();
        }
        Fatal("error receiving message: " << errorUtils_parseErrno(socket_errno));
      }
      if (received != 0) {
        /*Debug("received = " << received);*/
      }
      bufferLength += received;
      if (bufferCapacity > bufferLength) {
        break;
      }
    }
    size_t newBufferCapacity = 2*bufferCapacity;
    if (newBufferCapacity == bufferCapacity) {
      ++newBufferCapacity;
    }
    uint8* newBuffer = new uint8[newBufferCapacity];
    memcpy(newBuffer, buffer, bufferLength);
    delete[] buffer;
    buffer = newBuffer;
    bufferCapacity = newBufferCapacity;
  } while (true);

  /* Now we check whether we've got a whole message */
  if (bufferLength < sizeof(BufferLenType)) {
    return 0;
  }
  BufferLenType nextMessageLen =
    ntoh_buffer(*reinterpret_cast<BufferLenType*>(buffer));
  assert(nextMessageLen > 0);
  Debug("nextMessageLen=" << nextMessageLen);
  if (bufferLength >= nextMessageLen + sizeof(BufferLenType)) {
    if (len < nextMessageLen) {
      Fatal("insufficient space in buffer for message (needed " <<
          nextMessageLen<<", given "<<len<<")");
    }
    memcpy(outBuf, buffer+sizeof(BufferLenType), nextMessageLen);
    memmove(
        buffer, buffer+nextMessageLen+sizeof(BufferLenType),
        bufferLength-nextMessageLen-sizeof(BufferLenType)
      );
    bufferLength -= (nextMessageLen+sizeof(BufferLenType));
    return nextMessageLen;
  }
  return 0;
}

size_t TCPSocket::receiveFrom(void* /*buf*/, size_t /*len*/, String& /*from*/)
{
  Fatal("not implemented");
  return 0; /* Return statement for the benefit of MSVC */
}

}}

