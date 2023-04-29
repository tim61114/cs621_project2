#include <vector>
#include <cstdint>
#include <cmath>
#include <queue>
#include "src/lib/filter.cc"
#include "ns3/packet.h"
#include "ns3/ptr.h"

using namespace ns3;

class TrafficClass {
    private:
        uint32_t packets;   // packet count in the queue
        uint32_t maxPackets;    // maximum number of packets allowed in the queue
        double_t weight;    // used by some QoS mechanism
        uint32_t priorityLevel;
        bool isDefault;
        std::queue<Ptr<Packet>> m_queue;
        

    public:
        std::vector<Filter> filters;

        TrafficClass() {
            this->packets = 0;
            this->maxPackets = std::numeric_limits<uint32_t>::max();
            this->weight = 0.0;
            this->priorityLevel = 0;
        }

        // return true if queue is not full and matches all filters
        bool Enqueue(Ptr<Packet> p) {
            if ( !isFull() || !match(p)) {
                return false;
            }

            m_queue.push(p);
            packets++;
            return true;
        };

        Ptr<Packet> Dequeue() {
            if (packets > 0) {
                m_queue.pop();
                packets--;
            }
        }

        // return true if the packet matches all filters
        bool match(Ptr<Packet> p) {
            for (size_t i = 0; i < filters.size(); i++) {
                if (!filters[i].match(p)) {
                    return false;
                }
            }
            return true;
        };
        
        bool isFull() {
            return packets >= maxPackets;
        }

        uint32_t getMaxPackets() {
            return maxPackets;
        }

        bool setMaxPackets(uint32_t maxPackets) {
            if (maxPackets < 0) {
                return false;
            }
            this->maxPackets = maxPackets;
            return true;
        }

        uint32_t getPacketCount() {
            return packets;
        }

        double_t getWeight() {
            return weight;
        }

        bool setWeight(double_t weight) {
            if (weight < 0) {
                return false;
            }
            this->weight = weight;
            return true;
        }

        uint32_t getPriorityLevel() {
            return priorityLevel;
        }

        bool setPriorityLevel(uint32_t priorityLevel) {
            if (priorityLevel < 0) {
                return false;
            }
            this->priorityLevel = priorityLevel;
            return true;
        }
};