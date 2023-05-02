#include <vector>
#include <cstdint>
#include "traffic-class.h"
#include "filter.h"
#include "ns3/queue.h"

using namespace ns3;


class DiffServ : Queue<Packet> {
    private:
        std::vector<TrafficClass*> q_class;

        bool DoEnqueue(Ptr<Packet> p) override {
            uint32_t index = Classify(p);
            return q_class[index]->Enqueue(p);
        }

        Ptr<Packet> DoDequeue() {
            return Schedule();
        }

        Ptr<Packet> DoRemove() {
            return Schedule();
        }


        Ptr<const Packet> DoPeek() const {
            return 0; // TODO:
        }

    public:
        std::vector<TrafficClass*> q_class;
        
        Ptr<Packet> Schedule() {
            return 0;
        }

        uint32_t Classify(Ptr<Packet> p) {
            return 0;
        }

        bool Enqueue(Ptr<Packet> p) override {
            return DoEnqueue(p);
        }

        Ptr<Packet> Dequeue() override {
            return DoDequeue();
        }

        Ptr<const Packet> Peek() const  {
            return DoPeek();
        }
        
        Ptr<Packet> Remove() override {
            return DoRemove();
        }
};  

