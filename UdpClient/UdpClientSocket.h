#pragma once

#ifndef __UDP_CLIENT_SOCKET_H__
#define __UDP_CLIENT_SOCKET_H__

#include "IpProtocol.h"

#include <stdint.h>
#include <string>
#include <Winsock2.h>
#include <Ws2tcpip.h>
#include <memory>

class UdpClientSocket
{
public:
   UdpClientSocket();
   ~UdpClientSocket();
   void reset(void);
   bool init(IpProtocol ipProtocol);
   bool sendTo(const char* address, const uint16_t port, IpProtocol ipProtocol,
      const std::string& sendBuff, int& bytesSend);
   int recvFrom(const char* address, const uint16_t port, IpProtocol ipProtocol,
      char* recvBuff, int recvBuffSize);
   void close(void);

private:
   void fillAddrInfoCriteria(addrinfo* hints) const;
   bool fillNetworkAddressStructure(const char* address, sockaddr_in* socketAddr);
   void fillPort(uint16_t port, sockaddr_in* socketAddr);
   void fillIpProtocolFamily(sockaddr_in* socketAddr);

private:
   SOCKET socketId;
   IpProtocol ipProtocol;
};

#endif //__UDP_CLIENT_SOCKET_H__