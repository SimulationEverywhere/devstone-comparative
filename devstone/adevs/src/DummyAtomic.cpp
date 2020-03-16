//
// Created by Román Cárdenas Rodríguez on 09/03/2020.
//

#include "DummyAtomic.hpp"
#include "../../dhry/dhry_1.c"

const int DummyAtomic::in = 0;
const int DummyAtomic::out = 1;

DummyAtomic::DummyAtomic(int intDelayIn, int extDelayIn, double procTimeIn): adevs::Atomic<IO_Type>() {
    intDelay = intDelayIn;
    extDelay = extDelayIn;
    procTime = procTimeIn;
    sigma = DBL_MAX;
    t = 0.0;
}

/// External transition function
void DummyAtomic::delta_ext(double e, const adevs::Bag<IO_Type>& x) {
    DhryStone().dhrystoneRun(extDelay);
    t += e;
    if (sigma != DBL_MAX) {
        sigma -= e;
    } else {
        sigma = procTime;
    }
}

/// Internal transition function
void DummyAtomic::delta_int() {
    DhryStone().dhrystoneRun(intDelay);
    t += sigma;
    sigma = DBL_MAX;
}

/// Confluent transition function.
void DummyAtomic::delta_conf(const adevs::Bag<IO_Type>& x) {
    delta_int();
    delta_ext(0.0, x);
}

/// Output function.
void DummyAtomic::output_func(adevs::Bag<IO_Type >& y) {
    y.insert(IO_Type(out, 0));
}

/// Time advance function.
double DummyAtomic::ta() {
    return sigma;
}

/// Garbage collection. No heap allocation in output_func, so do nothing.
void DummyAtomic::gc_output(adevs::Bag<IO_Type>&) {}

/// Destructor
DummyAtomic::~DummyAtomic() = default;
