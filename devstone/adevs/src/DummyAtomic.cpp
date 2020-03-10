//
// Created by Román Cárdenas Rodríguez on 09/03/2020.
//

#include "DummyAtomic.hpp"

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
    t += e;
    if (sigma != DBL_MAX) {
        sigma -= e;
    }
    // Add the new values to the back of the list.
    adevs::Bag<IO_Type>::const_iterator i = x.begin();
    values.push_back(new int(*((*i).value)));
    sigma = procTime;
}

/// Internal transition function
void DummyAtomic::delta_int() {
    t += sigma;
    // Done with the job, so set time of next event to infinity
    sigma = DBL_MAX;
    values.clear();
}

/// Confluent transition function.
void DummyAtomic::delta_conf(const adevs::Bag<IO_Type>& x) {
    // Discard the old job
    delta_int();
    // Process the incoming job
    delta_ext(0.0, x);
}

/// Output function.
void DummyAtomic::output_func(adevs::Bag<IO_Type >& y) {
    // Get the departing customer
    std::list<int*>::iterator i;
    for (i = values.begin(); i != values.end(); i++) {
        y.insert(IO_Type(out, *i));
    }
}

/// Time advance function.
double DummyAtomic::ta() {
    return sigma;
}

/// Garbage collection. No heap allocation in output_func, so do nothing.
void DummyAtomic::gc_output(adevs::Bag<IO_Type>&) {}

/// Destructor
DummyAtomic::~DummyAtomic() = default;
