#include <vector>
#include "src/lib/filter-element/filter-element.h"

using namespace ns3;

class Filter {
    public:
        std::vector<FilterElement> elements;
        
        bool match(Ptr<Packet> p) {
            for (size_t i = 0; i < elements.size(); i++) {
                if (!elements[i].match(p)) {
                    return false;
                }
            }
        
            return true;
        }

};