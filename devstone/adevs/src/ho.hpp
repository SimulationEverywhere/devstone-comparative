//
// Created by rcardenas on 3/10/20.
//

#ifndef DEVSTONE_ADEVS_HO_HPP
#define DEVSTONE_ADEVS_HO_HPP

#include <adevs.h>
#include "DEVStone.hpp"
#include "DEVSWrapper.hpp"

class HO: public DEVSWrapper {
public:
    HO(int depthIn, int widthIn, int intDelayIn, int extDelayIn, double procTimeIn);
};


#endif //DEVSTONE_ADEVS_HO_HPP
