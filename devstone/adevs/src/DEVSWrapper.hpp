//
// Created by rcardenas on 3/9/20.
//

#ifndef DEVSTONE_ADEVS_DEVSWRAPPER_HPP
#define DEVSTONE_ADEVS_DEVSWRAPPER_HPP

#include <adevs.h>
#include "DEVStone.hpp"
#include "DummyAtomic.hpp"


class DEVSWrapper: public adevs::Digraph<int*>
{
protected:
    int depth, width;
    int intDelay, extDelay;
    double procTime;
public:
    /// Model input port
    static const int in;
    /// Model output port
    static const int out;
    /// Constructor
    DEVSWrapper(int depthIn, int widthIn, int intDelayIn, int extDelayIn, double procTimeIn);
};

#endif //DEVSTONE_ADEVS_DEVSWRAPPER_HPP
