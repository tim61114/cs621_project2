#include <vector>
#include "src/lib/traffic_class.cc"
#include "src/lib/filter.cc"
#include "src/lib/filter-element/diff-serv.cc"
#include "src/lib/filter-element/dest-port-filter-element.cc"

using namespace ns3;

class StrictPriorityQueue : DiffServ {
    private:
        // return the next packet
        Ptr<Packet> DoPeek() {
            Ptr<Packet> p;
            for (TrafficClass tc : q_class) {
                if (!tc->isEmpty()) {
                    return tc->DoPeek();
                }
            }
            NS_LOG_LOGIC("all queues empty");
            return nullptr;
        }

        bool compareByPriority(const std::vector<uint32_t, uint16_t>& vec1, const std::vector<uint32_t, uint16_t> vec2) {
            return vev1[0] < vec2[0];
        }

        void InitializeTrafficClass(uint32_t queueNumber, std::vector<std::vector<uint32_t, uint16_t>>& priorityPortList) {
            // sort the List by priority in ascending order
            std::sort(priorityPortList.begin(), priorityPortList.end(), compareByPriority);

            // Create TrafficClasses and their filters
            for (uint32_t i = 0; i < queueNumber; i++) {
                uint32_t priorityLevel = priorityPortList[i][0];
                uint16_t destPort = priorityPortList[i][1];
                TrafficClass* tc = new TrafficClass(priorityLevel);
                tc->setPriorityLevel(priorityPortList[i][0]);
                DestPortNumberFilter destPortFilter(destPort);
                Filter filter;
                filter.elements.push_back(destPortFilter);
                tc->filters.push_back(filter);
                q_class.push_back(tc);
            } 
        }
    public: 
        // uint32_t is priority, uint16_t is port number
        StrictPriorityQueue(uint32_t queueNumber, std::vector<std::vector<uint32_t, uint16_t>>& priorityPortList) {
            InitializeTrafficClass(queueNumber, priorityPortList);
        }

        // return the index of traffic class
        uint32_t Classify(Ptr<Packet> p) override {
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

        bool DoEnqueue(Ptr<Packet> p) override {
            NS_LOG_FUNCTION(this << p);

            uint32_t classIndex = Classify(p);

            // drop packet if enqueue fails
            if (q_class[classIndex]->Enqueue(p)) {
                Drop(p);
                return false;
            }
            
            NS_LOG_LOGIC("Number packets " << q_class[classIndex]->getPacketCount());
            return true;
        }

        // assume vector of traffic class is sorted by priority from top to low
        Ptr<Packet> Schedule() override{
            for (TrafficClass tc : q_class) {
                if (tc->getPacketCount() > 0) {
                    return tc->Dequeue();
                }
            }
            return nullptr;
        }

};