#ifndef DEVSTONE_ADEVS_LI_HPP
#define DEVSTONE_ADEVS_LI_HPP

#include "utils.hpp"
#include "devstone_coupled.hpp"

class LI: public DEVStoneCoupled {
  public:
    LI(int width, int depth, int intDelay, int extDelay): DEVStoneCoupled(width, depth, intDelay, extDelay) {
        if (depth > 1) {
            childCoupled = new LI(width, depth - 1, intDelay, extDelay);
            add(childCoupled);
            couple(this, LI::in, childCoupled, LI::in);
            nEIC += 1;
            couple(childCoupled, LI::out, this, LI::out);
            nEOC += 1;
            for (auto i = 1; i < width; ++i) {
                auto atomic = addAtomic();
                couple(this, LI::in, atomic, DEVStoneAtomic::in);
                nEIC += 1;
            }
        }
    }
};

#endif //DEVSTONE_ADEVS_LI_HPP
