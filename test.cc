#include <cstdio>
#include <cassert>
// #include "ns3/core-module.h"
// #include "ns3/network-module.h"
//#include "filter-element.cc"
#include "source-ip-filter-element.cc"
#include "source-mask-filter-element.cc"
#include "dest-ip-filter-element.cc"
#include "dest-mask-filter-element.cc"

using namespace ns3;

void TestSourceMaskFilter();
void TestSourceIPFilter();
void TestDestMaskFilter();
void TestDestIPFilter();
void TestProtocolNumberFilter();

int main() {

    TestSourceIPFilter();
    TestDestIPFilter();
    TestSourceMaskFilter();
    TestDestMaskFilter();   

}

void TestSourceIPFilter() {

    printf("Testing Source IP Filter.\n");
    Ptr<Packet> pkt = Create<Packet>(100);
    Ipv4Address address1("102.102.102.102");
    Ipv4Address address2("102.102.102.103");

    //Matching "102.102.102.102 with 102.102.102.102"
    SourceIPAddressFilter s1(address1);
    assert(s1.match(pkt));
    printf("#Test case 1 passed\n");

    //Matching "102.102.102.103 with 102.102.102.102"
    SourceIPAddressFilter s2(address2);
    assert(!s2.match(pkt));
    printf("#Test case 2 passed\n");

}

void TestDestIPFilter() {

    printf("Testing Dest IP Filter.\n");
    Ptr<Packet> pkt = Create<Packet>(100);
    Ipv4Address address1("102.102.102.102");
    Ipv4Address address2("102.102.102.103");

    //Matching "102.102.102.102 with 102.102.102.102"
    DestIPAddressFilter s1(address1);
    assert(s1.match(pkt));
    printf("#Test case 1 passed\n");

    //Matching "102.102.102.103 with 102.102.102.102"
    DestIPAddressFilter s2(address2);
    assert(!s2.match(pkt));
    printf("#Test case 2 passed\n");

}

void TestSourceMaskFilter() {

    printf("Testing Source Mask Filter.\n");
    Ptr<Packet> pkt1 = Create<Packet>(100);
    Ipv4Address address1("102.102.102.102");
    Ipv4Address address2("102.102.0.1");
    Ipv4Address address3("102.101.0.1");
    Ipv4Mask mask1("255.255.0.0");

    //Matching "102.102.102.102 with mask 255.255.0.0 with 102.102.102.102"
    SourceMaskFilter s1(address1, mask1);
    assert(s1.match(pkt1));
    printf("#Test case 1 passed\n");

    //Matching "102.102.0.1 with mask 255.255.0.0 with 102.102.102.102"
    SourceMaskFilter s2(address2, mask1);
    assert(s2.match(pkt1));
    printf("#Test case 2 passed\n");

    //Matching "102.101.0.1 with mask 255.255.0.0 with 102.102.102.102"
    SourceMaskFilter s3(address3, mask1);
    assert(!s3.match(pkt1));
    printf("#Test case 3 passed\n");

}

void TestDestMaskFilter() {

    printf("Testing Dest Mask Filter.\n");
    Ptr<Packet> pkt1 = Create<Packet>(100);
    Ipv4Address address1("102.102.102.102");
    Ipv4Address address2("102.102.0.1");
    Ipv4Address address3("102.101.0.1");
    Ipv4Mask mask1("255.255.0.0");

    //Matching "102.102.102.102 with mask 255.255.0.0 with 102.102.102.102"
    DestMaskFilter s1(address1, mask1);
    assert(s1.match(pkt1));
    printf("#Test case 1 passed\n");

    //Matching "102.102.0.1 with mask 255.255.0.0 with 102.102.102.102"
    DestMaskFilter s2(address2, mask1);
    assert(s2.match(pkt1));
    printf("#Test case 2 passed\n");

    //Matching "102.101.0.1 with mask 255.255.0.0 with 102.102.102.102"
    DestMaskFilter s3(address3, mask1);
    assert(!s3.match(pkt1));
    printf("#Test case 3 passed\n");

}
