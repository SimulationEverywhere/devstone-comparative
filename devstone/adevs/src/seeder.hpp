//
// Created by rcardenas on 3/13/20.
//

#ifndef DEVSTONE_ADEVS_SEEDER_HPP
#define DEVSTONE_ADEVS_SEEDER_HPP

#include <adevs.h>
#include <list>
#include "utils.hpp"


class Seeder: public adevs::Atomic<IO_Type> {
private:
    /// Model state variables
    double sigma;
    int nMessages;
    double t;
public:
    /// Model output port
    static const int out;

    /// Constructor.  The processing time is provided as an argument.
    Seeder(int nMessagesIn);
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
    ~Seeder();
};

#endif //DEVSTONE_ADEVS_SEEDER_HPP
