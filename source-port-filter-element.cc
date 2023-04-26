#include "filter-element.h"
#include "ns3/ipv4-header.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"

using namespace ns3;

// only filters port number in TCP and UDP
class SourcePortNumberFilter : FilterElement {
    private:
        uint32_t sourcePortNumber;

    public:
        SourcePortNumberFilter(uint32_t portNumber) {
            this->sourcePortNumber = portNumber;
        }

        bool match(Ptr<Packet> myPacket) override {
            Ptr<Packet> packetReplica = myPacket->Copy();
            
            // remove ipv4 header
            Ipv4Header ipHeader;
            packetReplica->RemoveHeader(ipHeader);

            uint16_t packetSourcePort = 0;

            if (ipHeader.GetProtocol() == TCP_Protocol_Number) {
                TcpHeader tcpHeader;
                packetReplica->RemoveHeader(tcpHeader);
                packetSourcePort = tcpHeader.GetSourcePort();
            } else if (ipHeader.GetProtocol() == UDP_Protocol_Number) {
                UdpHeader udpHeader;
                packetReplica->RemoveHeader(udpHeader);
                packetSourcePort = udpHeader.GetSourcePort();
            }

            return packetSourcePort == this->sourcePortNumber;
        }
};
