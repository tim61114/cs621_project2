#include <vector>
#include <cstdint>
#include "traffic-class.cc"
#include "filter.cc"
#include "ns3/queue.h"

using namespace ns3;

class DiffServ : Queue<Packet> {
    protected:
        std::vector<TrafficClass> q_class;

    public:
        bool DoEnqueue(Ptr<Packet> p) override {
            for (auto& tc : q_class) {
                if (!tc.match(p) || tc.isFull()) {
                    continue;
                }
                return tc.Enqueue(p);
            }
            // Packet dropped.
            return false;
        }

        Ptr<Packet> Schedule() {
            return DoDequeue();
        }

        uint32_t Classify(Ptr<Packet> p) {
            for (auto& tc : q_class) {
                if (tc.match(p) && !tc.isFull()) {
                    return tc.getPriorityLevel();
                }
            }

            return -1;
        }

};
