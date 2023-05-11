#ifndef STRICT_PRIORITY_QUEUE_H
#define STRICT_PRIORITY_QUEUE_H

#include <utility>
#include <vector>
#include <cstdint>
#include "ns3/log.h"
#include "ns3/ppp-header.h"
#include "ns3/type-id.h"
#include "../../src/lib/filter.h"
#include "../../src/lib/diff-serv.cc"
#include "../../src/lib/filter-element/filter-element.h"
#include "../../src/lib/traffic-class.h"


using namespace ns3;

class StrictPriorityQueue : public DiffServ {
    private:
        uint32_t m_queueNumber;
        uint32_t m_firstPriority;
        uint16_t m_firstPort;

        uint32_t m_secondPriority;
        uint16_t m_secondPort;

        bool m_isInitialized = false;
        std::vector<TrafficClass*> m_tc_vector;

        static bool CompareTCByPriority(TrafficClass* tc1, TrafficClass* tc2) {
            return tc1->getPriorityLevel() < tc2->getPriorityLevel();
        }


    public: 
        static TypeId GetTypeId(void){
            TypeId tid = TypeId("StrictPriorityQueue").SetParent<DiffServ>().AddConstructor<StrictPriorityQueue>()

            .AddAttribute("QueueNumber",
                    "Queue number.",
                    UintegerValue(2),
                    MakeUintegerAccessor(&StrictPriorityQueue::m_queueNumber),
                    MakeUintegerChecker<uint32_t>());
            
            return tid;
        }

        StrictPriorityQueue() {
            
        }

        // return the index of traffic class
        uint32_t Classify(Ptr<Packet> p) override{
            uint32_t classIndex = 1;
            std::vector<TrafficClass*> q_class = DiffServ::getQClass();
            for (uint32_t i = 0; i < q_class.size(); i++) {
                if (q_class[i]->match(p)) {
                    classIndex = i;
                    
                    break;
                }
            }
            
            return classIndex;
        }

        // assume vector of traffic class is sorted by priority from top to low
        Ptr<Packet> Schedule() override{
            std::vector<TrafficClass*> q_class = DiffServ::getQClass();
            for (TrafficClass *tc : q_class) {
                if (tc->getPacketCount() > 0) {
                    return tc->Dequeue();
                }
            }
            return nullptr;
        }

        void setQ_Class(std::vector<TrafficClass*> q_class) {
             // sort q_class from priority 0 (highest) to lowest
            std::sort(q_class.begin(), q_class.end(), CompareTCByPriority);
            DiffServ::setQClass(q_class);
        }

};

NS_OBJECT_ENSURE_REGISTERED(StrictPriorityQueue);

#endif
