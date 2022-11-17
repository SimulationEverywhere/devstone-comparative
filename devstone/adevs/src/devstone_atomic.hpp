#ifndef DEVSTONE_ADEVS_DEVSTONEATOMIC_HPP
#define DEVSTONE_ADEVS_DEVSTONEATOMIC_HPP

#include <adevs.h>
#include <list>
#include "utils.hpp"

class DEVStoneAtomic: public adevs::Atomic<IO_Type> {
    private:
        /// Model state variables
        __attribute__ ((unused)) int intDelay, extDelay;  // TODO include Dhrystone stuff
        double sigma;
    public:
        /// Model input port
        static const int in = 0;
        /// Model output port
        static const int out = 1;
        /// To count the number of events etc.
        unsigned int nInternals, nExternals, nEvents;

        /// Constructor
        DEVStoneAtomic(int intDelay, int extDelay): adevs::Atomic<IO_Type>(), intDelay(intDelay), extDelay(extDelay), sigma(DBL_MAX) {
            nInternals = 0;
            nExternals = 0;
            nEvents = 0;
        }

        /// External transition function
        void delta_ext(__attribute__ ((unused)) double e, const adevs::Bag<IO_Type>& x) override {
            // DhryStone().dhrystoneRun(extDelay); // TODO
            nExternals += 1;
            nEvents += x.size();
            sigma = 0;
        }

        /// Internal transition function
        void delta_int() override {
            // DhryStone().dhrystoneRun(intDelay); // TODO
            nInternals += 1;
            sigma = DBL_MAX;
        }

        /// Confluent transition function.
        void delta_conf(const adevs::Bag<IO_Type>& x) override {
            delta_int();
            delta_ext(0.0, x);
        }
        /// Output function.
        void output_func(adevs::Bag<IO_Type>& y) override {
            y.insert(IO_Type(out, 0));
        }

        /// Time advance function.
        double ta() override {
            return sigma;
        }
        /// Garbage collection.
        void gc_output(adevs::Bag<IO_Type>&) override {}
};

#endif //DEVSTONE_ADEVS_DEVSTONEATOMIC_HPP
