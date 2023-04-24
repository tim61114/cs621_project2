#include <cstdio>
// #include "ns3/core-module.h"
// #include "ns3/network-module.h"
#include "FilterElement.cc"

using namespace ns3;

class test : ns3::FilterElement {
    
    public:
        bool match(Ptr<ns3::Packet> myPackets) override {
            printf("%d\n", myPackets->GetSize());
            return true;
        }
};

int main() {
    test t1;
    Ptr<Packet> pkt = Create<Packet>(101);
    t1.match(pkt);  
}

