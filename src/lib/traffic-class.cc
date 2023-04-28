#include <vector>
#include <cstdint>
#include <cmath>
#include <queue>
//#include "filter.cc"
#include "ns3/packet.h"
#include "ns3/ptr.h"

using namespace ns3;

class TrafficClass {
    private:
        uint32_t packets;
        uint32_t maxPackets;
        double_t weight;
        uint32_t priorityLevel;
        bool isDefault;
        std::queue<Ptr<Packet>> m_queue;
//        std::vector<Filter> filters;// Commented this out to avoid errors

    public:
        bool Enqueue(Ptr<Packet> p);
        Ptr<Packet> Dequeue();
        bool match(Ptr<Packet>);
        // Consider maxPackets for match as well
        // i.e. if the Queue is full then don't match, minimize drop rate.
};
