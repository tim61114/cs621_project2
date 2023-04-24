#include <cstdio>
#include "FilterElement.cc"


class test : ns3::FilterElement {
    public:
        bool match(int a) override {
            printf("Hello test\n");
            return true;
        }
};

int main() {
    test t1;
    t1.match(1);  
}

