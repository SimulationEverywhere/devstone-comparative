//
// Created by rcardenas on 3/10/20.
//

#ifndef DEVSTONE_ADEVS_HI_HPP
#define DEVSTONE_ADEVS_HI_HPP

#include "utils.hpp"
#include "DEVSWrapper.hpp"

class HI: public DEVSWrapper {
public:
    HI(int depthIn, int widthIn, int intDelayIn, int extDelayIn, double procTimeIn);
};


#endif //DEVSTONE_ADEVS_HI_HPP
