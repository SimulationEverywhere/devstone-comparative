#ifndef DEVSTONE_ADEVS_DEVSTONE_HPP
#define DEVSTONE_ADEVS_DEVSTONE_HPP

#include "utils.hpp"
#include "li.hpp"
#include "hi.hpp"
#include "ho.hpp"
#include "homod.hpp"
#include "seeder.hpp"

class DEVStone: public adevs::Digraph<int*> {
public:
    [[maybe_unused]] DEVStoneCoupled *model;
    DEVStone(const string& type, int width, int depth, int intDelay, int extDelay): adevs::Digraph<int*>() {
        auto *seeder = new Seeder();
        add(seeder);

        //adevs::Digraph<int*> devstone;
        if (type == "LI") {
            auto *li = new LI(width, depth, intDelay, extDelay);
            add(li);
            couple(seeder, Seeder::out, li, LI::in);
            model = li;
        } else if (type == "HI") {
            auto *hi = new HI(width, depth, intDelay, extDelay);
            add(hi);
            couple(seeder, Seeder::out, hi, HI::in);
            model = hi;
        } else if (type == "HO") {
            auto *ho = new HO(width, depth, intDelay, extDelay);
            add(ho);
            couple(seeder, Seeder::out, ho, HO::in);
            couple(seeder, Seeder::out, ho, HOmod::in2);
            model = ho;
        } else if (type == "HOmod") {
            auto *homod = new HOmod(width, depth, intDelay, extDelay);
            add(homod);
            couple(seeder, Seeder::out, homod, HOmod::in);
            couple(seeder, Seeder::out, homod, HOmod::in2);
            model = homod;
        } else {
            throw runtime_error("Unable to determine DEVStone model type");
        }
    }
};

#endif //DEVSTONE_ADEVS_DEVSTONE_HPP
