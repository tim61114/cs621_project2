#include <utility>
#include <cstdint>

#include "ns3/type-id.h"
#include "../../src/lib/filter.h"
#include "../../src/lib/filter-element/filter-element.h"
#include "../../src/lib/traffic-class.h"
#include "deficit-round-robin.h"

namespace ns3 {

    NS_OBJECT_ENSURE_REGISTERED(DeficitRoundRobin);

    Ptr<Packet> DeficitRoundRobin::DoPeek() {
        if (m_numPackets == 0) {
            return nullptr;
        }

        if (!q_class[m_currentTCIndex]->isEmpty()) {
            return q_class[m_currentTCIndex]->DoPeek();
        }
        
        //Handle curTC is empty?
    }

    void DeficitRoundRobin::InitializeTrafficClass() {

        SourcePortNumberFilter* sourcePortFilter1 = new SourcePortNumberFilter(m_firstPort);
        // Create Filter
        Filter* filter1 = new Filter();
        filter1->elements.push_back(sourcePortFilter1);  
        // Create TrafficClass
        TrafficClass* tc1 = new TrafficClass();
//        tc1->setWeight(m_firstDeficit);
        tc1->filters.push_back(filter1);

        // create second TrafficClass
        SourcePortNumberFilter* sourcePortFilter2 = new SourcePortNumberFilter(m_secondPort);
        Filter* filter2 = new Filter();
        filter2->elements.push_back(sourcePortFilter2);  
        TrafficClass* tc2 = new TrafficClass();
//       tc2->setWeight(m_secondDeficit);
        tc2->filters.push_back(filter2);

        SourcePortNumberFilter* sourcePortFilter3 = new SourcePortNumberFilter(m_thirdPort);
        Filter* filter3 = new Filter();
        filter3->elements.push_back(sourcePortFilter3);  
        TrafficClass* tc3 = new TrafficClass();
//        tc3->setWeight(m_thirdDeficit);
        tc3->filters.push_back(filter3);

        q_class.push_back(tc1);
        q_class.push_back(tc2);
        q_class.push_back(tc3);

    }

    TypeId DeficitRoundRobin::GetTypeId(void) {
        static TypeId tid = 
            TypeId("DeficitRoundRobin")
            .SetParent<DiffServ>()
            .AddConstructor<DeficitRoundRobin>()

            .AddAttribute("QueueNumber",
                    "Queue number.",
                    UintegerValue(3),
                    MakeUintegerAccessor(&DeficitRoundRobin::m_queueNumber),
                    MakeUintegerChecker<uint32_t>())

            .AddAttribute("Deficit",
                    "A vector of Deficits.",
                    VectorValue(Vector(300.0, 200.0, 100.0)),
                    MakeVectorAccessor(&DeficitRoundRobin::m_deficit),
                    MakeVectorChecker())

            .AddAttribute("FirstDeficit",
                    "First deficit value.",
                    UintegerValue(300),
                    MakeUintegerAccessor(&DeficitRoundRobin::m_firstDeficit),
                    MakeUintegerChecker<double_t>())

            .AddAttribute("FirstPort",
                    "First port value.",
                    UintegerValue(65535),
                    MakeUintegerAccessor(&DeficitRoundRobin::m_firstPort),
                    MakeUintegerChecker<uint16_t>())

            .AddAttribute("SecondDeficit",
                    "Second deficit value.",
                    UintegerValue(200),
                    MakeUintegerAccessor(&DeficitRoundRobin::m_secondDeficit),
                    MakeUintegerChecker<double_t>())

            .AddAttribute("SecondPort",
                    "Second port value.",
                    UintegerValue(65534),
                    MakeUintegerAccessor(&DeficitRoundRobin::m_secondPort),
                    MakeUintegerChecker<uint16_t>());

            .AddAttribute("ThirdDeficit",
                    "Third deficit value.",
                    UintegerValue(100),
                    MakeUintegerAccessor(&DeficitRoundRobin::m_thirdDeficit),
                    MakeUintegerChecker<double_t>())

            .AddAttribute("ThirdPort",
                    "Third port value.",
                    UintegerValue(65533),
                    MakeUintegerAccessor(&DeficitRoundRobin::m_thirdPort),
                    MakeUintegerChecker<uint16_t>());

        return tid;
    }

    DeficitRoundRobin::DeficitRoundRobin() {

    }

    uint32_t DeficitRoundRobin::Classify(Ptr<Packet> p) override {
        if (m_isInitialized == false) {
            InitializeTrafficClass();
            m_isInitialized = true;
        }

        for (uint32_t i = 0; i < q_class.size(); i++) {
            if (q_class[i]->match(p)) {
                classIndex = i;
                break;
            }
        }
        ++m_numPackets;

        return classIndex;
    }

    Ptr<Packet> DeficitRoundRobin::Schedule() override {
        Ptr<Packet> p;
        if (m_numPackets == 0) {
            return nullptr;
        }
        
        if (refresh) {
           RefreshDeficit();
           refresh = false;
        }

        //Serve current queue
        if (!q_class[m_currentTCIndex]->isEmpty() && 
             q_class[m_currentTCIndex].DoPeek().GetSize() <= m_curDeficit[m_currentTCIndex]) {
            
            m_curDeficit[m_currentTCIndex] -= q_class[m_currentTCIndex].DoPeek.GetSize();
            p = q_class[m_currentTCIndex].Dequeue();
            --m_numPackets;
            
        }

        //Current queue can no longer be served
        if (q_class[m_currentTCIndex]->isEmpty() ||
            q_class[m_currentTCIndex].DoPeek().GetSize() > m_curDeficit[m_currentTCIndex]) {
            m_currentTCIndex = (m_currentTCIndex + 1) % m_queueNumber;
            if (m_currentTCIndex == 0) {
                refresh = true;
            }
        }

        return p;
    }

    void DeficitRoundRobin::RefreshDeficit() {
        for (int i = 0; i < m_deficit.size(); ++i) {
            m_curDeficit[i] += m_deficit[i];
        }
    }

}

