// #include "ns3/packet.h"
// #include "ns3/ptr.h"

namespace ns3{

class FilterElement {
    public: 
        //virtual bool match(Ptr<ns3::Packet>) = 0;
        virtual bool match(int a) = 0;
};

}

