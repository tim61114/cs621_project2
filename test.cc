#include "src/lib/filter-element/source-ip-filter-element.cc"
#include "src/lib/filter-element/source-mask-filter-element.cc"
#include "src/lib/filter-element/dest-ip-filter-element.cc"
#include "src/lib/filter-element/dest-mask-filter-element.cc"
#include "src/lib/filter-element/source-port-filter-element.cc"
#include "src/lib/filter-element/dest-port-filter-element.cc"

using namespace ns3;

void TestSourceMaskFilter();
void TestSourceIPFilter();
void TestDestMaskFilter();
void TestDestIPFilter();
void TestProtocolNumberFilter();
void TestSourcePortFilter();
void TestDestPortFilter();

int main() {

    TestSourceIPFilter();
    TestDestIPFilter();
    TestSourceMaskFilter();
    TestDestMaskFilter();   
    TestSourcePortFilter();
    TestDestPortFilter();

}

void TestSourceIPFilter() {

    printf("Testing Source IP Filter.\n");
    Ptr<Packet> pkt = Create<Packet>(100);
    Ipv4Address address1("102.102.102.102");
    Ipv4Address address2("102.102.102.103");

    //Matching "102.102.102.102 with 102.102.102.102"
    SourceIPAddressFilter s1(address1);
    NS_ASSERT(s1.match(pkt));
    printf("#Test case 1 passed\n");

    //Matching "102.102.102.103 with 102.102.102.102"
    SourceIPAddressFilter s2(address2);
    NS_ASSERT(!s2.match(pkt));
    printf("#Test case 2 passed\n");

}

void TestDestIPFilter() {

    printf("Testing Dest IP Filter.\n");
    Ptr<Packet> pkt = Create<Packet>(100);
    Ipv4Address address1("102.102.102.102");
    Ipv4Address address2("102.102.102.103");

    //Matching "102.102.102.102 with 102.102.102.102"
    DestIPAddressFilter s1(address1);
    NS_ASSERT(s1.match(pkt));
    printf("#Test case 1 passed\n");

    //Matching "102.102.102.103 with 102.102.102.102"
    DestIPAddressFilter s2(address2);
    NS_ASSERT(!s2.match(pkt));
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
    NS_ASSERT(s1.match(pkt1));
    printf("#Test case 1 passed\n");

    //Matching "102.102.0.1 with mask 255.255.0.0 with 102.102.102.102"
    SourceMaskFilter s2(address2, mask1);
    NS_ASSERT(s2.match(pkt1));
    printf("#Test case 2 passed\n");

    //Matching "102.101.0.1 with mask 255.255.0.0 with 102.102.102.102"
    SourceMaskFilter s3(address3, mask1);
    NS_ASSERT(!s3.match(pkt1));
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
    NS_ASSERT(s1.match(pkt1));
    printf("#Test case 1 passed\n");

    //Matching "102.102.0.1 with mask 255.255.0.0 with 102.102.102.102"
    DestMaskFilter s2(address2, mask1);
    NS_ASSERT(s2.match(pkt1));
    printf("#Test case 2 passed\n");

    //Matching "102.101.0.1 with mask 255.255.0.0 with 102.102.102.102"
    DestMaskFilter s3(address3, mask1);
    NS_ASSERT(!s3.match(pkt1));
    printf("#Test case 3 passed\n");

}

void TestSourcePortFilter() {
    printf("Testing Source Port Filter.\n");

    // create a TCP packet with source port 9999
    Ptr<Packet> tcpPacket = Create<Packet>(100);
    TcpHeader tcpHeader;
    tcpHeader.SetSourcePort(9999);
    tcpPacket->AddHeader(tcpHeader);

    // wrap it in an IP packet with protocol set to TCP
    Ptr<Packet> ipPacket = Create<Packet>();
    Ipv4Header ipHeader;
    ipHeader.SetProtocol(TCP_Protocol_Number);
    ipHeader.SetPayloadSize(tcpPacket->GetSize());
    ipPacket->AddHeader(ipHeader);
    ipPacket->AddAtEnd(tcpPacket);
    
    uint16_t port1 = 9999;
    uint16_t port2 = 9998;

    SourcePortNumberFilter filter1(port1);
    SourcePortNumberFilter filter2(port2);

    // target: TCP 9999, input: TCP 9999
    NS_ASSERT(filter1.match(ipPacket)); 
    printf("#Test TCP Source Port Filter 9999 passed\n");
    
    // target: TCP 9998, input: TCP 9999
    NS_ASSERT(!filter2.match(ipPacket));
    printf("#Test TCP Source Port Filter 9998 passed\n");
    

    // create a UDP packet with source port 9997
    Ptr<Packet> udpPacket = Create<Packet>(100);
    UdpHeader udpHeader;
    udpHeader.SetSourcePort(9997);
    udpPacket->AddHeader(udpHeader);

    // wrap it in an IP packet with protocol set to TCP
    ipPacket = Create<Packet>();
    ipHeader.SetProtocol(UDP_Protocol_Number);
    ipHeader.SetPayloadSize(udpPacket->GetSize());
    ipPacket->AddHeader(ipHeader);
    ipPacket->AddAtEnd(udpPacket);
    
    uint16_t port3 = 9997;
    uint16_t port4 = 8887;

    SourcePortNumberFilter filter3(port3);
    SourcePortNumberFilter filter4(port4);

     // target: UDP 9997, input: UDP 9997
    NS_ASSERT(filter3.match(ipPacket)); 
    printf("#Test UDP Source Port Filter 9997 passed\n");
    
    // target: TCP 9996, input: TCP 9997
    NS_ASSERT(!filter4.match(ipPacket));
    printf("#Test UDP Source Port Filter 9996 passed\n");
}

void TestDestPortFilter() {
    printf("Testing Destination Port Filter.\n");

    // create a TCP packet with dest port 8888
    Ptr<Packet> tcpPacket = Create<Packet>(100);
    TcpHeader tcpHeader;
    tcpHeader.SetDestinationPort(8888);
    tcpPacket->AddHeader(tcpHeader);

    // wrap it in an IP packet with protocol set to TCP
    Ptr<Packet> ipPacket = Create<Packet>();
    Ipv4Header ipHeader;
    ipHeader.SetProtocol(TCP_Protocol_Number);
    ipHeader.SetPayloadSize(tcpPacket->GetSize());
    ipPacket->AddHeader(ipHeader);
    ipPacket->AddAtEnd(tcpPacket);
    
    uint16_t port1 = 8888;
    uint16_t port2 = 8887;

    DestPortNumberFilter filter1(port1);
    DestPortNumberFilter filter2(port2);

    // target: TCP 8888, input: TCP 8888
    NS_ASSERT(filter1.match(ipPacket)); 
    printf("#Test TCP Dest Port Filter 8888 passed\n");
    
    // target: TCP 8887, input: TCP 8888
    NS_ASSERT(!filter2.match(ipPacket));
    printf("#Test TCP Dest Port Filter 8887 passed\n");
    

    // create a UDP packet with dest port 9997
    Ptr<Packet> udpPacket = Create<Packet>(100);
    UdpHeader udpHeader;
    udpHeader.SetDestinationPort(8886);
    udpPacket->AddHeader(udpHeader);

    // wrap it in an IP packet with protocol set to TCP
    ipPacket = Create<Packet>();
    ipHeader.SetProtocol(UDP_Protocol_Number);
    ipHeader.SetPayloadSize(udpPacket->GetSize());
    ipPacket->AddHeader(ipHeader);
    ipPacket->AddAtEnd(udpPacket);
    
    uint16_t port3 = 8886;
    uint16_t port4 = 8885;

    DestPortNumberFilter filter3(port3);
    DestPortNumberFilter filter4(port4);

     // target: UDP 8886, input: UDP 8886
    NS_ASSERT(filter3.match(ipPacket)); 
    printf("#Test UDP Dest Port Filter 8886 passed\n");
    
    // target: TCP 8885, input: TCP 8886
    NS_ASSERT(!filter4.match(ipPacket)); 
    printf("#Test UDP Dest Port Filter 8885 passed\n");
}

