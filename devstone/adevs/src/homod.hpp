#ifndef DEVSTONE_ADEVS_HOMOD_HPP
#define DEVSTONE_ADEVS_HOMOD_HPP

#include "utils.hpp"
#include "devstone_coupled.hpp"

class HOmod: public DEVStoneCoupled {
 public:
    /// Additional model input port
    static const int in2 = 2;
    HOmod(int width, int depth, int intDelay, int extDelay): DEVStoneCoupled(width, depth, intDelay, extDelay) {
        if (depth > 1) {
            childCoupled = new HOmod(width, depth - 1, intDelay, extDelay);
            add(childCoupled);
            couple(this, HOmod::in, childCoupled, HOmod::in);
            nEIC += 1;
            couple(childCoupled, HOmod::out, this, HOmod::out);
            nEOC += 1;

            std::vector<DEVStoneAtomic*> prevLayer, currentLayer;
            // First layer of atomic models:
            for (int i = 1; i < width; ++i) {
                auto atomic = addAtomic();
                couple(this, HOmod::in2, atomic, DEVStoneAtomic::in);
                nEIC += 1;
                couple(atomic, DEVStoneAtomic::out, childCoupled, HOmod::in2);
                nIC += 1;
                prevLayer.push_back(atomic);
            }
            // Second layer of atomic models:
            for (int i = 1; i < width; ++i) {
                auto atomic = addAtomic();
                if (i == 1) {
                    couple(this, HOmod::in2, atomic, DEVStoneAtomic::in);
                    nEIC += 1;
                }
                for (const auto& prevAtomic: prevLayer) {
                    couple(atomic, DEVStoneAtomic::out, prevAtomic, DEVStoneAtomic::in);
                    nIC += 1;
                }
                currentLayer.push_back(atomic);
            }
            // Rest of the tree
            prevLayer = currentLayer;
            currentLayer = std::vector<DEVStoneAtomic*>();
            for (int layer = 3; layer <= width; ++layer) {
                for (unsigned long i = 1; i < prevLayer.size(); ++i) {
                    auto atomic = addAtomic();
                    if (i == 1) {
                        couple(this, HOmod::in2, atomic, DEVStoneAtomic::in);
                        nEIC += 1;
                    }
                    couple(atomic, DEVStoneAtomic::out, prevLayer.at(i), DEVStoneAtomic::in);
                    nIC += 1;
                    currentLayer.push_back(atomic);
                }
                prevLayer = currentLayer;
                currentLayer = std::vector<DEVStoneAtomic*>();
            }
        }
    }
};

#endif //DEVSTONE_ADEVS_HOMOD_HPP
