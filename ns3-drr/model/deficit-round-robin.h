#ifndef DEFICIT_ROUND_ROBIN_H
#define DEFICIT_ROUND_ROBIN_H

#include <cstdint>
#include <vector>
#include "../../src/lib/diff-serv.cc"

namespace ns3 {

    class DeficitRoundRobin : public DiffServ {
        public:
            static TypeId GetTypeId(void);
            DeficitRoundRobin();
            uint32_t Classify(Ptr<Packet> p);
            Ptr<Packet> Schedule();

        private:
            uint32_t m_currentTCIndex = 0;
            uint32_t m_numPackets = 0;

            uint32_t m_queueNumber;
            std::vector<double_t> m_deficit;
            std::vector<double_t> m_curDeficit = {0, 0, 0};
//           double_t m_firstDeficit;
//           double_t m_curFirstDeficit = 0;
            uint16_t m_firstPort;
//            double_t m_secondDeficit;
//            double_t m_curSecondDeficit = 0;
            uint16_t m_secondPort;
//            double_t m_thirdDeficit;
//            double_t m_curThirdDeficit = 0;
            uint16_t m_thirdPort;
            
            bool m_isInitialized = false;
            bool refresh = true;
            Ptr<Packet> DoPeek();
            void InitializeTrafficClass();
            void RefreshDeficit();

    };
}

#endif
