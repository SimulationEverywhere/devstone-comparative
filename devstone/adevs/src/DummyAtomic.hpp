//
// Created by rcardenas on 3/9/20.
//

#ifndef DEVSTONE_ADEVS_DUMMYATOMIC_HPP
#define DEVSTONE_ADEVS_DUMMYATOMIC_HPP

#include <cstdlib>
#include <adevs.h>

typedef adevs::PortValue<int> Integer;


class DummyAtomic: public adevs::Atomic<Integer> {
    private:
        /// Model state variables
        int intDelay, extDelay, sigma;  // TODO include Dhrystone stuff
        double procTime;
        Integer *val;
        double t;
    public:
        /// Model input port
        static const int in;
        /// Model output port
        static const int out;

        /// Constructor.  The processing time is provided as an argument.
        DummyAtomic(int intDelayIn, int extDelayIn, double procTimeIn): adevs::Atomic<Integer>() {
            intDelay = intDelayIn;
            extDelay = extDelayIn;
            procTime = procTimeIn;
            sigma = DBL_MAX;
            val = nullptr;
            t = 0.0;
        }
        /// Internal transition function
        void delta_int() override {
            t += sigma;
            // Done with the job, so set time of next event to infinity
            sigma = DBL_MAX;
            val = nullptr;
        }
        /// External transition function
        void delta_ext(double e, const adevs::Bag<Integer>& x) override {
            t += e;
            // If we are waiting for a job
            if (sigma == DBL_MAX)
            {
                val = new Integer((*(x.begin())).value);
                sigma = procTime;
            }
            // Otherwise, model just continues with time of next event unchanged
            else
            {
                sigma -= e;
            }
        }
        /// Confluent transition function.
        void delta_conf(const adevs::Bag<Integer>& x) override {
            // Discard the old job
            delta_int();
            // Process the incoming job
            delta_ext(0.0,x);
        }
        /// Output function.
        void output_func(adevs::Bag<Integer>& y) override {
            y.insert(val);
        }
        /// Time advance function.
        double ta() override { return sigma; }
        /**
        Garbage collection. No heap allocation in output_func, so
        do nothing.
        */
        void gc_output(adevs::Bag<Integer>&) override {}
        /// Destructor
        ~DummyAtomic() override = default;
};


#endif //DEVSTONE_ADEVS_DUMMYATOMIC_HPP
