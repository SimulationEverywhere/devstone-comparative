//
// Created by rcardenas on 3/10/20.
//

#ifndef DEVSTONE_ADEVS_LI_HPP
#define DEVSTONE_ADEVS_LI_HPP

#include <adevs.h>
#include "DEVStone.hpp"
#include "DEVSWrapper.hpp"

class LI: public DEVSWrapper
{
public:
    LI(int depthIn, int widthIn, int intDelayIn, int extDelayIn, double procTimeIn);
};

#endif //DEVSTONE_ADEVS_LI_HPP
