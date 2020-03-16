//
// Created by Román Cárdenas Rodríguez on 09/03/2020.
//

#ifndef DEVSTONE_ADEVS_DEVSTONE_HPP
#define DEVSTONE_ADEVS_DEVSTONE_HPP

#include <boost/program_options.hpp>
#include <iostream>
#include <chrono>
#include <string>

#include "utils.hpp"
#include "li.hpp"
#include "hi.hpp"
#include "ho.hpp"
#include "hoMod.hpp"
#include "seeder.hpp"

using namespace std;
namespace po=boost::program_options;
using hclock=chrono::high_resolution_clock;


class DEVStone: public adevs::Digraph<int*>
{
public:
    DEVStone(const string& typeIn, int depthIn, int widthIn, int intDelayIn, int extDelayIn, double propTimeIn);
};

#endif //DEVSTONE_ADEVS_DEVSTONE_HPP
