//
// Created by rcardenas on 3/13/20.
//

#include "seeder.hpp"

const int Seeder::out = 0;

Seeder::Seeder(int nMessagesIn): adevs::Atomic<IO_Type>() {
    nMessages = nMessagesIn;
    sigma = 0;
    t = 0.0;
}

/// External transition function
void Seeder::delta_ext(double e, const adevs::Bag<IO_Type>& x) { }

/// Internal transition function
void Seeder::delta_int() {
    t += sigma;
    sigma = DBL_MAX;
}

/// Confluent transition function.
void Seeder::delta_conf(const adevs::Bag<IO_Type>& x) {
    // Discard the old job
    delta_int();
    // Process the incoming job
    delta_ext(0.0, x);
}

/// Output function.
void Seeder::output_func(adevs::Bag<IO_Type >& y) {
    // Get the departing customer
    for (int i = 0; i < nMessages; i++) {
        y.insert(IO_Type(out, 0));
    }
}

/// Time advance function.
double Seeder::ta() {
    return sigma;
}

/// Garbage collection. No heap allocation in output_func, so do nothing.
void Seeder::gc_output(adevs::Bag<IO_Type>&) {}

/// Destructor
Seeder::~Seeder() = default;
