#ifndef DEVSTONE_ADEVS_SEEDER_HPP
#define DEVSTONE_ADEVS_SEEDER_HPP

#include <adevs.h>
#include <list>
#include "utils.hpp"

class Seeder: public adevs::Atomic<IO_Type> {
private:
    /// Model state variables
    double sigma;
public:
    /// Model output port
    static const int out = 0;

    /// Constructor.
    explicit Seeder(): adevs::Atomic<IO_Type>(), sigma() {}

    /// External transition function (does not apply)
    void delta_ext(__attribute__ ((unused)) double e, __attribute__ ((unused)) const adevs::Bag<IO_Type>& x) override {}

    /// Internal transition function (just passivates)
    void delta_int() override {
            sigma = std::numeric_limits<double>::infinity();
    }
    /// Confluent transition function (standard one)
    void delta_conf(const adevs::Bag<IO_Type>& x) override {
        delta_int();
        delta_ext(0.0, x);
    }
    /// Output function (sends a single message via the single port).
    void output_func(adevs::Bag<IO_Type>& y) override {
        y.insert(IO_Type(out, 0));
    }
    /// Time advance function (just returns sigma)
    double ta() override {
        return sigma;
    }
    /// Garbage collection.
    void gc_output(adevs::Bag<IO_Type>&) override {}
};

#endif //DEVSTONE_ADEVS_SEEDER_HPP
