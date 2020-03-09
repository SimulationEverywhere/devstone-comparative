//
// Created by rcardenas on 3/9/20.
//

#ifndef DEVSTONE_ADEVS_DEVSWRAPPER_HPP
#define DEVSTONE_ADEVS_DEVSWRAPPER_HPP

#include "DummyAtomic.hpp"

class DEVSWrapper: public adevs::Digraph<int> {
    private:
        int depth;
        int width;
        int intDelay, extDelay;  // TODO include Dhrystone stuff
        double procTime;

    
    public:
        /// Model input port
        static const int in;
        /// Model output port
        static const int out;

        DEVSWrapper(int depthIn, int widthIn, int intDelayIn, int extDelayIn, double procTimeIn): adevs::Digraph<int>() {
            depth = depthIn;
            width = widthIn;
            intDelay = intDelayIn;
            extDelay = extDelayIn;
            procTime = procTimeIn;

            if (depth == 1) {
                DummyAtomic *a = new DummyAtomic(intDelay, extDelay, procTime);
                add(a);
                couple(this, this->in, a, a->in);
                couple(a, a->out, this, this->out);
            } else {
                DEVSWrapper c = generateCoupled();
                add(c);
                couple(this, this->in, c, c->in);
                couple(c, c->out, this, this->out);
            }
            for (int i = 0; i < width - 1; ++i) {
                DummyAtomic a = DummyAtomic(intDelay, extDelay, procTime);
                add(a);
            }
        }

        virtual DEVSWrapper generateCoupled();
};

#endif //DEVSTONE_ADEVS_DEVSWRAPPER_HPP
