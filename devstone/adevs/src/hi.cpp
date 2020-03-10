//
// Created by rcardenas on 3/10/20.
//

#include "hi.hpp"

HI::HI(int depthIn, int widthIn, int intDelayIn, int extDelayIn, double procTimeIn):
        DEVSWrapper(depthIn, widthIn, intDelayIn, extDelayIn, procTimeIn)
{
    if (depth > 1) {
        HI *c =  new HI(depth - 1, width, intDelay, extDelay, procTime);
        add(c);
        couple(this, this->in, c, c->in);
        couple(c, c->out, this, this->out);

        for (auto i = atomics.begin(); i < atomics.end(); ++i) {
            DummyAtomic *atomic = *i;
            couple(this, this->in, atomic, atomic->in);

            if(i != atomics.end() - 1) {
                DummyAtomic *nextAtomic = *(i+1);
                couple(atomic, atomic->in, nextAtomic, nextAtomic->in);
            }
        }
    }
}