#include <vector>
#include "ns3/log.h"
#include "./lib/filter.h"
#include "./lib/diff-serv.cc"
#include "./lib/filter-element/dest-port-filter-element.cc"
#include "./lib/filter-element/filter-element.h"
#include "./lib/traffic-class.h"
#include "ns3/ppp-header.h"
#include <utility>
#include <cstdint>


using namespace ns3;

NS_LOG_COMPONENT_DEFINE("StrictPriorityQueue");


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
            NS_LOG_INFO("all queues empty");
            return nullptr;
        }

        static bool compareByPriority(const std::pair<uint32_t, uint16_t>& pair1, const std::pair<uint32_t, uint16_t>& pair2) {
            return pair1.first < pair2.first;
        }

        void InitializeTrafficClass(uint32_t queueNumber, std::vector<std::pair<uint32_t, uint16_t>>& priorityPortList) {
            // sort the List by priority in ascending order
            std::sort(priorityPortList.begin(), priorityPortList.end(), compareByPriority);

            // Create TrafficClasses and their filters
            for (uint32_t i = 0; i < queueNumber; i++) {
                uint32_t priorityLevel = priorityPortList[i].first;
                uint16_t destPort = priorityPortList[i].second;
                TrafficClass* tc = new TrafficClass();
                tc->setPriorityLevel(priorityPortList[i].first);
                // DestPortNumberFilter destPortFilter(destPort);
                // FilterElement *fe = &destPortFilter;
                DestPortNumberFilter* destPortFilter = new DestPortNumberFilter(destPort);
                Filter filter;
                filter.elements.push_back(destPortFilter);
                tc->filters.push_back(&filter);
                q_class.push_back(tc);
            } 
        }
    public: 
        // uint32_t is priority, uint16_t is port number
        StrictPriorityQueue(uint32_t queueNumber, std::vector<std::pair<uint32_t, uint16_t>>& priorityPortList) {
            InitializeTrafficClass(queueNumber, priorityPortList);
        }

        // return the index of traffic class
        uint32_t Classify(Ptr<Packet> p) {
            NS_LOG_FUNCTION(this << p);
            PppHeader ppp;
            p->RemoveHeader(ppp);

            uint32_t classIndex;

            for (uint32_t i = 0; i < q_class.size(); i++) {
                if (q_class[i]->match(p)) {
                    NS_LOG_INFO("\tclassifier: queue " << i << " matches." );
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
