#ifndef FILTER_ELEMENT_H
#define FILTER_ELEMENT_H

#include "ns3/packet.h"
#include "ns3/ptr.h"

namespace ns3{

class FilterElement {
    public: 
        virtual bool match(Ptr<ns3::Packet> myPackets) = 0;
};

}

#endif
