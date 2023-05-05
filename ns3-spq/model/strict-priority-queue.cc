#include <utility>
#include <cstdint>
#include "ns3/log.h"
#include "ns3/ppp-header.h"
#include "ns3/type-id.h"
#include "../../src/lib/filter.h"
#include "../../src/lib/diff-serv.cc"
//#include "../../src/lib/filter-element/source-port-filter-element.cc"
#include "../../src/lib/filter-element/filter-element.h"
#include "../../src/lib/traffic-class.h"




// // NS_LOG_COMPONENT_DEFINE("StrictPriorityQueue");

// // Ptr<Packet> StrictPriorityQueue::DoPeek() {
// //     Ptr<Packet> p;
// //             for (TrafficClass *tc : q_class) {
// //                 if (!tc->isEmpty()) {
// //                     return tc->DoPeek();
// //                 }
// //             }
// //             // NS_LOG_INFO("all queues empty");
// //             return 0;
// // }

// // bool StrictPriorityQueue::compareByPriority(const std::pair<uint32_t, uint16_t>& pair1, 
// //                                         const std::pair<uint32_t, uint16_t>& pair2) {
// //     static bool res = pair1.first < pair2.first;
// //     return res; 
// // }


// // void StrictPriorityQueue::InitializeTrafficClass(uint32_t queueNumber, 
// //                         std::vector<std::pair<uint32_t, uint16_t>>& priorityPortList) {
// //     // sort the List by priority in ascending order
// //     std::sort(priorityPortList.begin(), priorityPortList.end(), compareByPriority);

// //     // Create TrafficClasses and their filters
// //     for (uint32_t i = 0; i < queueNumber; i++) {
// //         uint32_t priorityLevel = priorityPortList[i].first;
// //         uint16_t destPort = priorityPortList[i].second;
                
// //         TrafficClass* tc = new TrafficClass();
// //         tc->setPriorityLevel(priorityLevel);
// //         // DestPortNumberFilter destPortFilter(destPort);
// //         // FilterElement *fe = &destPortFilter;
// //         DestPortNumberFilter* destPortFilter = new DestPortNumberFilter(destPort);
// //         Filter filter;
// //         filter.elements.push_back(destPortFilter);
// //         tc->filters.push_back(&filter);
// //         q_class.push_back(tc);
// //     } 

// // }

// // StrictPriorityQueue::StrictPriorityQueue(uint32_t queueNumber, 
// //             std::vector<std::pair<uint32_t, uint16_t>>& priorityPortList) {
// //             // InitializeTrafficClass(queueNumber, priorityPortList);
// // }


// StrictPriorityQueue::StrictPriorityQueue(uint32_t a) {
//     printf("constructor");
// }

// // // return the index of traffic class
// // uint32_t StrictPriorityQueue::Classify(Ptr<Packet> p) {
// //     // NS_LOG_FUNCTION(this << p);
// //     PppHeader ppp;
// //     p->RemoveHeader(ppp);

// //     uint32_t classIndex;
// //     for (uint32_t i = 0; i < q_class.size(); i++) {
// //         if (q_class[i]->match(p)) {
// //             // NS_LOG_INFO("\tclassifier: queue " << i << " matches." );
// //             classIndex = i;
// //             break;
// //         }
// //     }

// //     p->AddHeader(ppp);    
// //     return classIndex;
// // }


// // // assume vector of traffic class is sorted by priority from top to low
// // Ptr<Packet> StrictPriorityQueue::Schedule() {
// //     for (TrafficClass *tc : q_class) {
// //         if (tc->getPacketCount() > 0) {
// //             return tc->Dequeue();
// //         }
// //     }
// //     return nullptr;
// // }


using namespace ns3;



class StrictPriorityQueue : public DiffServ {
    private:
        uint32_t m_queueNumber;
        uint32_t m_firstPriority;
        uint16_t m_firstPort;

        uint32_t m_secondPriority;
        uint16_t m_secondPort;

        bool m_isInitialized = false;


        // return the next packet
        Ptr<Packet> DoPeek() {
            Ptr<Packet> p;
            for (TrafficClass *tc : q_class) {
                if (!tc->isEmpty()) {
                    return tc->DoPeek();
                }
            }
            // NS_LOG_INFO("all queues empty");
            return nullptr;
        }

        void putHighPriorityFirst() {
            if (m_firstPriority > m_secondPriority) {
                // switch priority
                uint32_t temp1 = m_firstPriority;
                m_firstPriority = m_secondPriority;
                m_secondPriority = temp1;
                
                // switch source port
                uint16_t temp2 = m_firstPort;
                m_firstPort = m_secondPort;
                m_secondPort = temp2;
            }
        }

        void InitializeTrafficClass() {
            // sort the List by priority in ascending order
            putHighPriorityFirst();

            // create first TrafficClass
            SourcePortNumberFilter* sourcePortFilter1 = new SourcePortNumberFilter(m_firstPort);
            // Create Filter
            Filter* filter1 = new Filter();
            filter1->elements.push_back(sourcePortFilter1);  
            // Create TrafficClass
            TrafficClass* tc1 = new TrafficClass();
            tc1->setPriorityLevel(m_firstPriority);
            tc1->filters.push_back(filter1);
            
            // create second TrafficClass
            SourcePortNumberFilter* sourcePortFilter2 = new SourcePortNumberFilter(m_secondPort);
            Filter* filter2 = new Filter();
            filter2->elements.push_back(sourcePortFilter2);  
            TrafficClass* tc2 = new TrafficClass();
            tc2->setPriorityLevel(m_firstPriority);
            tc2->filters.push_back(filter2);
            
            q_class.push_back(tc1);
            q_class.push_back(tc2);

            printf("[spq] init: q_class size : %ld\n", q_class.size());
        }

    public: 
        static TypeId GetTypeId(void){
            TypeId tid = TypeId("StrictPriorityQueue").SetParent<DiffServ>().AddConstructor<StrictPriorityQueue>()

            .AddAttribute("QueueNumber",
                    "Queue number.",
                    UintegerValue(2),
                    MakeUintegerAccessor(&StrictPriorityQueue::m_queueNumber),
                    MakeUintegerChecker<uint32_t>())

            .AddAttribute("FirstPriority",
                    "First priority value.",
                    UintegerValue(0),
                    MakeUintegerAccessor(&StrictPriorityQueue::m_firstPriority),
                    MakeUintegerChecker<uint32_t>())
            
            .AddAttribute("FirstPort",
                    "First port value.",
                    UintegerValue(65535),
                    MakeUintegerAccessor(&StrictPriorityQueue::m_firstPort),
                    MakeUintegerChecker<uint16_t>())
            
            .AddAttribute("SecondPriority",
                    "Second priority value.",
                    UintegerValue(1),
                    MakeUintegerAccessor(&StrictPriorityQueue::m_secondPriority),
                    MakeUintegerChecker<uint32_t>())
            
            .AddAttribute("SecondPort",
                    "Second port value.",
                    UintegerValue(65534),
                    MakeUintegerAccessor(&StrictPriorityQueue::m_secondPort),
                    MakeUintegerChecker<uint16_t>());
            
            return tid;
        }


        // uint32_t is priority, uint16_t is port number
        StrictPriorityQueue() {
            printf("Create Strict Priority Queue (Constructor)\n");
            if (m_isInitialized == false)
            {   
                // setParameters();
                InitializeTrafficClass();
                m_isInitialized = true;
            } 
        }

        // TODO: delete! 
        void setParameters() {
            this->m_firstPort = 8000;
            this->m_firstPriority = 0;
            this->m_secondPort = 9500;
            this->m_secondPriority = 1;
            this->m_queueNumber = 2;
        }

        // return the index of traffic class
        uint32_t Classify(Ptr<Packet> p) override{
            // NS_LOG_FUNCTION(this << p);
  

            uint32_t classIndex = 0;

            for (uint32_t i = 0; i < q_class.size(); i++) {
                if (q_class[i]->match(p)) {
                    // NS_LOG_INFO("\tclassifier: queue " << i << " matches." );
                    classIndex = i;
                    
                    break;
                }
            }
     
            printf("[SPQ] Classify: ClassIndex: %d\n", classIndex);
            return classIndex;
        }

        // assume vector of traffic class is sorted by priority from top to low
        Ptr<Packet> Schedule() {
            for (TrafficClass *tc : q_class) {
                if (tc->getPacketCount() > 0) {
                    return tc->Dequeue();
                }
            }
            return nullptr;
        }

};

NS_OBJECT_ENSURE_REGISTERED(StrictPriorityQueue);