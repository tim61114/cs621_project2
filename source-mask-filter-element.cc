#include <cstdint>
#include "filter-element.h"
#include "ns3/ipv4-header.h"

using namespace ns3;

class SourceMaskFilter : FilterElement {
    private:
        Ipv4Address subnetAddress;
        Ipv4Mask subnetMask;

    public: 
        SourceMaskFilter(Ipv4Address subnet, Ipv4Mask mask) {
            this->subnetAddress = subnet;
            this->subnetMask = mask;
        }

        bool match(Ptr<Packet> myPacket) override {
            Ipv4Header header;
            myPacket->PeekHeader(header);

            uint32_t ip = header.GetSource().Get();
            uint32_t subnet = this->subnetAddress.Get();

            uint32_t maskedSubnet = subnet & subnetMask.Get();

            return (ip & subnetMask.Get()) == maskedSubnet;
        }
};
