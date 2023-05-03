#ifndef STRICT_PRIORITY_QUEUE_H
#define STRICT_PRIORITY_QUEUE_H

#include <vector>
#include <utility>
#include <cstdint>
#include "ns3/log.h"
#include "../../src/lib/filter.h"
#include "../../src/lib/diff-serv.cc"
#include "../../src/lib/filter-element/dest-port-filter-element.cc"
#include "../../src/lib/filter-element/filter-element.h"
#include "../../src/lib/traffic-class.h"
#include "ns3/ppp-header.h"

using namespace ns3;


class StrictPriorityQueue : public DiffServ {
    private:
        // Ptr<Packet> DoPeek();

        // static bool compareByPriority(const std::pair<uint32_t, uint16_t>& pair1, 
        //                                 const std::pair<uint32_t, uint16_t>& pair2);
        
        // void InitializeTrafficClass(uint32_t queueNumber, 
        //                 std::vector<std::pair<uint32_t, uint16_t>>& priorityPortList);
        

    public:
        //uint32_t is priority, uint16_t is port number
        // StrictPriorityQueue(uint32_t queueNumber, 
        //                 std::vector<std::pair<uint32_t, uint16_t>>& priorityPortList);
        // StrictPriorityQueue(uint32_t a);

        // uint32_t Classify(Ptr<Packet> p);

        // Ptr<Packet> Schedule();
};


#endif