//
// Created by rcardenas on 3/10/20.
//

#include "li.hpp"

LI::LI(int depthIn, int widthIn, int intDelayIn, int extDelayIn, double procTimeIn):
DEVSWrapper(depthIn, widthIn, intDelayIn, extDelayIn, procTimeIn)
{
    if (depth > 1) {
        LI *c =  new LI(depth - 1, width, intDelay, extDelay, procTime);
        add(c);
        couple(this, this->in, c, c->in);
        couple(c, c->out, this, this->out);
    }
}
