//
// Created by rcardenas on 3/10/20.
//

#include "hoMod.hpp"
#include <map>
#include <list>

const int HOmod::in1 = 0;
const int HOmod::in2 = 1;
const int HOmod::out = 2;

HOmod::HOmod(int depthIn, int widthIn, int intDelayIn, int extDelayIn, double procTimeIn):
adevs::Digraph<int*>()
{
    depth = depthIn;
    width = widthIn;
    intDelay = intDelayIn;
    extDelay = extDelayIn;
    procTime = procTimeIn;

    if (depth == 1) {
        DummyAtomic *a = new DummyAtomic(intDelay, extDelay, procTime);
        add(a);
        couple(this, this->in1, a, a->in);
        couple(a, a->out, this, this->out);
    } else {
        HOmod *c = new HOmod(depth - 1, width, intDelay, extDelay, procTime);
        couple(this, this->in1, c, c->in1);
        couple(c, c->out, this, this->out);
        if (width > 1) {
            std::map<int, std::list<DummyAtomic*>> atomics;
            for (int i = 0; i < width; i++) {
                int minRowIdx = (i < 2)? 0 : (i - 1);
                for (int j = minRowIdx; j < width - 1; j++) {
                    DummyAtomic *a = new DummyAtomic(intDelay, extDelay, procTime);
                    add(a);

                    atomics.insert(std::pair<int, list<DummyAtomic*>>(i, std::list<DummyAtomic*>()));  // en teoria insert no machaca
                    atomics.find(i)->second.push_back(a);
                }
            }
            // Connect EIC
            for (auto a: atomics.find(0)->second) {
                couple(this, this->in2, a, a->in);
            }
            for (int i = 1; i < width; i++) {
                DummyAtomic *a = atomics.find(i)->second.front();
                couple(this, this->in2, a, a->in);
            }
            // Connect IC
            for (auto a: atomics.find(0)->second) {
                couple(a, a->out, c, c->in2);
            }
            for (int i = 0; i < atomics.find(0)->second.size(); i++) {
                DummyAtomic *aTop = *std::next(atomics.find(0)->second.begin(), i);
                for (int j = 0; j < atomics.find(1)->second.size(); j++) {
                    DummyAtomic *aDown = *std::next(atomics.find(1)->second.begin(), j);
                    couple(aDown, aDown->out, aTop, aTop->in);
                }
            }
            for (int i = 2; i < width; i++) {
                for (int j = 0; j < atomics.find(i)->second.size(); j++) {
                    DummyAtomic *aFrom = *std::next(atomics.find(i)->second.begin(), j);
                    DummyAtomic *aTo = *std::next(atomics.find(i - 1)->second.begin(), j + 1);
                    couple(aFrom, aFrom->out, aTo, aTo->in);
                }
            }
        }
    }
}