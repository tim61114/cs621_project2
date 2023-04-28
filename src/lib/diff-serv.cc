#include <vector>
#include <cstdint>
#include "traffic-class.cc"
#include "filter.cc"
#include "ns3/queue.h"

using namespace ns3;

class DiffServ : Queue<Packet> {
    private:
        std::vector<TrafficClass> q_class;

    public:
        bool DoEnqueue(Ptr<Packet> p) override {
            for (auto& tc : q_class) {
                if (!tc.match(p)) {
                    continue;
                }
                return tc.Enqueue(p);
            }
            // Packet dropped.
            return false;
        }

        Ptr<Packet> DoDequeue() override {
            
        }

        Ptr<Packet> DoRemove() override {

        }

        const Ptr<Packet> DoPeek() override {

        }

        Ptr<Packet> Schedule() override {

        }

        uint32_t Classify(Ptr<Packet> p) override {
            
        }

};
