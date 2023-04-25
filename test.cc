#include <cstdio>
// #include "ns3/core-module.h"
// #include "ns3/network-module.h"
//#include "filter-element.cc"
#include "source-ip-filter-element.cc"
#include "source-mask-filter-element.cc"

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
    
    Ipv4Address address1("102.102.102.102");
    SourceIPAddressFilter s1(address1);
    Ptr<Packet> pkt2 = Create<Packet>(101);
    printf("%d\n", s1.match(pkt2));

    Ipv4Address address2("102.102.0.1");
    Ipv4Mask mask1("255.255.0.0");
    SourceMaskFilter s2(address1, mask1);
    printf("%d\n", s2.match(pkt2));
    

}

