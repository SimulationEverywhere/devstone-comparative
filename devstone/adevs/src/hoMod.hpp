//
// Created by rcardenas on 3/10/20.
//

#ifndef DEVSTONE_ADEVS_HOMOD_HPP
#define DEVSTONE_ADEVS_HOMOD_HPP

#include <adevs.h>
#include "DEVStone.hpp"
#include "DummyAtomic.hpp"


class HOMod: public adevs::Digraph<int*> {
protected:
    int depth, width;
    int intDelay, extDelay;
    double procTime;
public:
    /// Model input ports
    static const int in1, in2;
    /// Model output port
    static const int out;
    // Constructor
    HOMod(int depthIn, int widthIn, int intDelayIn, int extDelayIn, double procTimeIn);
};


#endif //DEVSTONE_ADEVS_HOMOD_HPP
