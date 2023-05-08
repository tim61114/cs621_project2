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

        // return the next packet
        Ptr<Packet> DoPeek() {
            Ptr<Packet> p;
            for (TrafficClass *tc : q_class) {
                if (!tc->isEmpty()) {
                    return tc->DoPeek();
                }
            }
            return nullptr;
        }

        // comparater for TrafficClass by Priority
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

            for (uint32_t i = 0; i < q_class.size(); i++) {
                if (q_class[i]->match(p)) {
                    // NS_LOG_INFO("\tclassifier: queue " << i << " matches." );
                    classIndex = i;
                    
                    break;
                }
            }
            
            //printf("[SPQ] Classify: ClassIndex: %d\n", classIndex);
            return classIndex;
        }

        // assume vector of traffic class is sorted by priority from top to low
        Ptr<Packet> Schedule() override{
            for (TrafficClass *tc : q_class) {
                if (tc->getPacketCount() > 0) {
                    return tc->Dequeue();
                }
            }
            return nullptr;
        }

        void setQ_Class(std::vector<TrafficClass*> q_class) {
            this->q_class = q_class;
            // sort q_class from priority 0 (highest) to lowest
            std::sort(q_class.begin(), q_class.end(), CompareTCByPriority);
        }

};

NS_OBJECT_ENSURE_REGISTERED(StrictPriorityQueue);