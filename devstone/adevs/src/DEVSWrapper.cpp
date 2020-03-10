//
// Created by Román Cárdenas Rodríguez on 09/03/2020.
//

#include "DEVSWrapper.hpp"

const int DEVSWrapper::in = 0;
const int DEVSWrapper::out = 1;

DEVSWrapper::DEVSWrapper(int depthIn, int widthIn, int intDelayIn, int extDelayIn, double procTimeIn):
        adevs::Digraph<IO_Type>()
{
    depth = depthIn;
    width = widthIn;
    intDelay = intDelayIn;
    extDelay = extDelayIn;
    procTime = procTimeIn;

    if (depth == 1) {
        DummyAtomic *a = new DummyAtomic(intDelay, extDelay, procTime);
        add(a);
        couple(this, this->in, a, a->in);
        couple(a, a->out, this, this->out);
    } else {
        DEVSWrapper c = generateCoupled();
        add(c);
        couple(this, this->in, c, c->in);
        couple(c, c->out, this, this->out);
    }
    for (int i = 0; i < width - 1; ++i) {
        DummyAtomic a = DummyAtomic(intDelay, extDelay, procTime);
        add(a);
    }
}



