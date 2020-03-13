//
// Created by rcardenas on 3/9/20.
//

#ifndef DEVSTONE_ADEVS_DUMMYATOMIC_HPP
#define DEVSTONE_ADEVS_DUMMYATOMIC_HPP

#include <adevs.h>
#include <list>
#include "utils.hpp"


class DummyAtomic: public adevs::Atomic<IO_Type> {
    private:
        /// Model state variables
        int intDelay;  // TODO include Dhrystone stuff
        int extDelay;
        double sigma;
        double procTime;
        std::list<int*> values;
        double t;
    public:
        /// Model input port
        static const int in;
        /// Model output port
        static const int out;

        /// Constructor.  The processing time is provided as an argument.
        DummyAtomic(int intDelayIn, int extDelayIn, double procTimeIn);
        /// External transition function
        void delta_ext(double e, const adevs::Bag<IO_Type>& x);
        /// Internal transition function
        void delta_int();
        /// Confluent transition function.
        void delta_conf(const adevs::Bag<IO_Type>& x);
        /// Output function.
        void output_func(adevs::Bag<IO_Type>& y);
        /// Time advance function.
        double ta();
        /// Garbage collection.
        void gc_output(adevs::Bag<IO_Type>&);
        /// Destructor
        ~DummyAtomic();
};

#endif //DEVSTONE_ADEVS_DUMMYATOMIC_HPP
