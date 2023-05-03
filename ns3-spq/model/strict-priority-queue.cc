#include <vector>
#include <utility>
#include <cstdint>
// #include "./strict-priority-queue.h"
#include "ns3/log.h"
#include "../../src/lib/filter.h"
#include "../../src/lib/diff-serv.cc"
#include "../../src/lib/filter-element/dest-port-filter-element.cc"
#include "../../src/lib/filter-element/filter-element.h"
#include "../../src/lib/traffic-class.h"
#include "ns3/ppp-header.h"


using namespace ns3;



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


class StrictPriorityQueue : public DiffServ {
    private:
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

        static bool compareByPriority(const std::pair<uint32_t, uint16_t>& pair1, 
                                        const std::pair<uint32_t, uint16_t>& pair2) {
            return pair1.first < pair2.first;
        }

        void InitializeTrafficClass(uint32_t queueNumber, 
                        std::vector<std::pair<uint32_t, uint16_t>>& priorityPortList) {
            // sort the List by priority in ascending order
            std::sort(priorityPortList.begin(), priorityPortList.end(), compareByPriority);

            // Create TrafficClasses and their filters
            for (uint32_t i = 0; i < queueNumber; i++) {
                uint32_t priorityLevel = priorityPortList[i].first;
                uint16_t destPort = priorityPortList[i].second;
                
                TrafficClass* tc = new TrafficClass();
                tc->setPriorityLevel(priorityLevel);
                DestPortNumberFilter* destPortFilter1 = new DestPortNumberFilter(destPort);
                Filter* filter = new Filter();
                DestPortNumberFilter* destPortFilter2 = new DestPortNumberFilter(destPort);
                filter->elements.push_back(destPortFilter1);
                filter->elements.push_back(destPortFilter2);
                tc->filters.push_back(filter);
                q_class.push_back(tc);
            } 
        }
    public: 
        // uint32_t is priority, uint16_t is port number
        StrictPriorityQueue(uint32_t queueNumber, 
                        std::vector<std::pair<uint32_t, uint16_t>>& priorityPortList) {
            InitializeTrafficClass(queueNumber, priorityPortList);
        }

        // return the index of traffic class
        uint32_t Classify(Ptr<Packet> p) {
            // NS_LOG_FUNCTION(this << p);
            PppHeader ppp;
            p->RemoveHeader(ppp);

            uint32_t classIndex;

            for (uint32_t i = 0; i < q_class.size(); i++) {
                if (q_class[i]->match(p)) {
                    // NS_LOG_INFO("\tclassifier: queue " << i << " matches." );
                    classIndex = i;
                    break;
                }
            }

            p->AddHeader(ppp);
            
            return classIndex;
        }

        // bool DoEnqueue(Ptr<Packet> p) override {
        //     NS_LOG_FUNCTION(this << p);

        //     uint32_t classIndex = Classify(p);

        //     // drop packet if enqueue fails
        //     if (!q_class[classIndex]->Enqueue(p)) {
        //         Drop(p);
        //         return false;
        //     }
            
        //     NS_LOG_LOGIC("Number packets " << q_class[classIndex]->getPacketCount());
        //     return true;
        // }

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


