//
// Created by rcardenas on 3/10/20.
//

#include "hoMod.hpp"
#include <map>
#include <list>

const int HOMod::in1 = 0;
const int HOMod::in2 = 1;
const int HOMod::out = 2;

HOMod::HOMod(int depthIn, int widthIn, int intDelayIn, int extDelayIn, double procTimeIn):
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
        HOMod *c = new HOMod(depth - 1, width, intDelay, extDelay, procTime);
        couple(this, this->in1, c, c->in1);
        couple(c, c->out, this, this->out);
        if (width > 1) {
            std::map<int, std::list<DummyAtomic>> atomics;
            for (int i = 0; i < width; i++) {
                int minRowIdx = (i < 2)? 0 : (i - 1);
                for (int j = minRowIdx; j < width - 1; j++) {
                    DummyAtomic *a = new DummyAtomic(intDelay, extDelay, procTime);
                    add(a);
                    if(atomics.find("f") == atomics.end()
                        atomics.put(i, new LinkedList<>());
                    atomics.get(i).add(atomic);
                }
            }
        }
    }
}