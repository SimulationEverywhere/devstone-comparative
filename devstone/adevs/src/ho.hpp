#ifndef DEVSTONE_ADEVS_HO_HPP
#define DEVSTONE_ADEVS_HO_HPP

#include <adevs.h>
#include "devstone.hpp"
#include "devstone_coupled.hpp"

class HO: public DEVStoneCoupled {
public:
    /// Additional model input port
    static const int in2 = 2;
    /// Additional model output port
    static const int out2 = 3;
    HO(int width, int depth, int intDelay, int extDelay): DEVStoneCoupled(width, depth, intDelay, extDelay) {
        if (depth > 1) {
            childCoupled = new HO(width, depth - 1, intDelay, extDelay);
            add(childCoupled);
            couple(this, HO::in, childCoupled, HO::in);
            nEIC += 1;
            couple(this, HO::in2, childCoupled, HO::in2);
            nEIC += 1;
            couple(childCoupled, HO::out, this, HO::out);
            nEOC += 1;
            DEVStoneAtomic* prevAtomic = nullptr;
            for (auto i = 1; i < width; ++i) {
                auto atomic = addAtomic();
                couple(this, HO::in2, atomic, DEVStoneAtomic::in);
                nEIC += 1;
                if (prevAtomic != nullptr) {
                    couple(prevAtomic, DEVStoneAtomic::out, atomic, DEVStoneAtomic::in);
                    nIC += 1;
                }
                couple(atomic, DEVStoneAtomic::out, this, HO::out2);
                nEOC += 1;
                prevAtomic = atomic;
            }
        }
    }
};

#endif //DEVSTONE_ADEVS_HO_HPP
