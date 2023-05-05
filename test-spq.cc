#include <iostream>
#include <utility>
#include <cstdint>
#include "ns3/log.h"
#include "ns3/ppp-header.h"
#include "ns3/type-id.h"
#include "./src/lib/filter.h"
#include "./src/lib/filter-element/filter-element.h"
#include "./src/lib/traffic-class.h"
#include "./ns3-spq/model/strict-priority-queue.cc"

using namespace ns3;

int main() {

    StrictPriorityQueue spq;


    Ptr<Packet> packet = Create<Packet> (100);
    TcpHeader tcpHeader;
    tcpHeader.SetSourcePort(7000);
    packet->AddHeader(tcpHeader);
    
    Ipv4Header ipHeader;
    ipHeader.SetProtocol(TCP_Protocol_Number);
    ipHeader.SetPayloadSize(packet->GetSize());
    packet->AddHeader(ipHeader);

    PppHeader pppHeader;
    packet->AddHeader(pppHeader);

                              


    uint32_t classIndex =  spq.Enqueue(packet);
    printf("class index %d\n", classIndex);
}