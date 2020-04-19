#include "UdpClientSocket.h"

UdpClientSocket::UdpClientSocket()
{
   this->ipProtocol = IpProtocol::UNDEFINED;
   this->socketId = INVALID_SOCKET;
}

UdpClientSocket::~UdpClientSocket()
{
   this->reset();
}

void UdpClientSocket::reset(void)
{
   this->close();
   this->socketId = INVALID_SOCKET;
}

bool UdpClientSocket::init(IpProtocol ipProtocol)
{
   // locals
   int family = AF_INET;
   int type = SOCK_DGRAM;
   int proto = IPPROTO_UDP;
   bool rV = true;

   if (ipProtocol == IpProtocol::IPV6)
   {
      family = AF_INET6;
      this->ipProtocol = IpProtocol::IPV6;
   }

   this->socketId = ::socket(family, type, proto);
   if (this->socketId == INVALID_SOCKET)
   {
      rV = false;
   }
   return rV;
}

bool UdpClientSocket::sendTo(const char* address, const uint16_t port, IpProtocol ipProtocol,
   const std::string& sendBuff, int& bytesSend)
{
   // locals
   bool rV = true;

   sockaddr_in socketAddr;

   this->fillIpProtocolFamily(&socketAddr);
   this->fillPort(port, &socketAddr);
   rV = this->fillNetworkAddressStructure(address, &socketAddr);

   bytesSend = ::sendto(this->socketId, sendBuff.c_str(), sendBuff.length() + 1, 0,
      (sockaddr*)&socketAddr, sizeof(socketAddr));

   if (bytesSend == SOCKET_ERROR)
   {
      rV = false;
   }

   return rV;
}
int UdpClientSocket::recvFrom(const char* address, const uint16_t port, IpProtocol ipProtocol,
   char* recvBuff, int recvBuffSize)
{
   // locals
   int rV = 0;

   sockaddr_in socketAddr;
   int socketAddrSize = sizeof(socketAddr);

   this->fillIpProtocolFamily(&socketAddr);
   this->fillPort(port, &socketAddr);
   rV = this->fillNetworkAddressStructure(address, &socketAddr);

   memset(recvBuff, 0, recvBuffSize);
   rV = ::recvfrom(this->socketId, recvBuff, recvBuffSize, 0,
      (sockaddr*)&socketAddr, &socketAddrSize);

   return rV;
}

void UdpClientSocket::close()
{
   if (this->socketId != INVALID_SOCKET)
   {
      ::closesocket(this->socketId);
   }
}

void UdpClientSocket::fillAddrInfoCriteria(addrinfo* hints) const
{
   memset(hints, 0, sizeof(*hints));
   hints->ai_socktype = SOCK_DGRAM;
   hints->ai_family = this->ipProtocol == IpProtocol::IPV4 ? AF_INET : AF_INET6;
}

bool UdpClientSocket::fillNetworkAddressStructure(const char* address, sockaddr_in* socketAddr)
{
   // locals
   bool rV = true;
   std::unique_ptr<addrinfo> hints = std::make_unique<addrinfo>();
   addrinfo* res = nullptr;

   if (address == nullptr)
   {
      socketAddr->sin_addr.s_addr = INADDR_ANY;
   }
   else if (address[0] >= '0' && address[0] <= '9')
   {
      if (::inet_pton(AF_INET, address, &socketAddr->sin_addr) != 1)
      {
         rV = false;
      }
   }
   else
   {
      this->fillAddrInfoCriteria(hints.get());
      if (0 != ::getaddrinfo(address, nullptr, hints.get(), &res))
      {
         rV = false;
      }
      else
      {
         socketAddr->sin_addr.S_un = ((sockaddr_in*)(res->ai_addr))->sin_addr.S_un;
         ::freeaddrinfo(res);
      }
   }

   return rV;
}

void UdpClientSocket::fillPort(uint16_t port, sockaddr_in* socketAddr)
{
   socketAddr->sin_port = htons(port);
}

void UdpClientSocket::fillIpProtocolFamily(sockaddr_in* socketAddr)
{
   if (this->ipProtocol == IpProtocol::IPV4)
   {
      socketAddr->sin_family = AF_INET;
   }
   else
   {
      socketAddr->sin_family = AF_INET6;
   }
}