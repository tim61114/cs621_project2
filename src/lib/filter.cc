#include <vector>
#include "src/lib/filter-element/filter-element.h"

using namespace ns3;

class Filter {
    public:
        std::vector<FilterElement> elements;
        
        bool match(Ptr<Packet> p) {
            for (auto &fe : elements) {
                if (!fe.match(p)) {
                    return false;
                }
            }
        
            return true;
        }

};