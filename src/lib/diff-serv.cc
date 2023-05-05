#include <vector>
#include <cstdint>
#include "traffic-class.h"
#include "filter.h"
#include "ns3/queue.h"
#include "./filter-element/source-port-filter-element.cc"

using namespace ns3;

class DiffServ : public Queue<Packet> {
    private:
        // std::vector<TrafficClass*> q_class;

        bool DoEnqueue(Ptr<Packet> p) {
            printf("[DiffServ] DoEnqueue queue len %ld\n", q_class.size());
            uint32_t index = Classify(p);
            printf("[DiffServ] DoEnqueue class index %d\n", index);    
            return q_class[index]->Enqueue(p);
        }

        Ptr<Packet> DoDequeue() {
            return Schedule();
        }

        Ptr<Packet> DoRemove() {
            return Schedule();
        }

        Ptr<const Packet> DoPeek() const {
            return 0; // TODO: ???
        }

    public:
        std::vector<TrafficClass*> q_class; // TODO: change to protected
        
        Ptr<Packet> Schedule() {
            return 0;
        }

        virtual uint32_t Classify(Ptr<Packet> p) = 0;
        // uint32_t Classify(Ptr<Packet> p){
        //     printf("[DiffServ] Classify\n");
        //     return -1;
        // };

        bool Enqueue(Ptr<Packet> p) override {
            printf("[DiffServ] Enqueue\n");
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