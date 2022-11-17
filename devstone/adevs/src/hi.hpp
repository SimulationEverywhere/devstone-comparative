#ifndef DEVSTONE_ADEVS_HI_HPP
#define DEVSTONE_ADEVS_HI_HPP

#include "utils.hpp"
#include "devstone_coupled.hpp"

class HI: public DEVStoneCoupled {
public:
    HI(int width, int depth, int intDelay, int extDelay): DEVStoneCoupled(width, depth, intDelay, extDelay) {
        if (depth > 1) {
            childCoupled = new HI(width, depth - 1, intDelay, extDelay);
            add(childCoupled);
            couple(this, HI::in, childCoupled, HI::in);
            nEIC += 1;
            couple(childCoupled, HI::out, this, HI::out);
            nEOC += 1;
            DEVStoneAtomic* prevAtomic = nullptr;
            for (auto i = 1; i < width; ++i) {
                auto atomic = addAtomic();
                couple(this, HI::in, atomic, DEVStoneAtomic::in);
                nEIC += 1;
                if (prevAtomic != nullptr) {
                    couple(prevAtomic, DEVStoneAtomic::out, atomic, DEVStoneAtomic::in);
                    nIC += 1;
                }
                prevAtomic = atomic;
            }
        }
    }
};


#endif //DEVSTONE_ADEVS_HI_HPP
