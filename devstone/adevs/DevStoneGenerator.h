#ifndef _DevStoneGenerator_h_
#define _DevStoneGenerator_h_

#include <string>
#include "adevs.h"

typedef adevs::PortValue<long> PortValue;

class DevStoneGenerator: public adevs::Atomic<PortValue> {
protected:
	std::string name;
	double sigma;
	double preparationTime;
	double period;
	long maxEvents;
	long counter;
public:
	/// Model output port
	static const int out;

    DevStoneGenerator(const std::string name, double preparationTime, double period, long maxEvents) : adevs::Atomic<PortValue>() {
        this->name = name;
		this->sigma = preparationTime;
        this->preparationTime = preparationTime;
        this->period = period;
        this->maxEvents = maxEvents;
		this->counter = 1;
    }

    void delta_int() {
        counter++;
        if (counter > maxEvents) {
            sigma = DBL_MAX;
        } else {
            sigma = period;
        }
    }

    void delta_ext(double e, const adevs::Bag<PortValue>& x) {
        sigma = DBL_MAX;
    }

	void delta_conf(const adevs::Bag<PortValue>& x) {
		delta_int();
		delta_ext(0.0, x);
	}

    void output_func(adevs::Bag<PortValue>& y) {
        PortValue pv(out, counter);
		y.insert(pv);
    }

	double ta() { 
		return sigma; 
	}

	void gc_output(adevs::Bag<PortValue>& g) { }
};

/// Create the static ports and assign them unique 'names' (numbers)
const int DevStoneGenerator::out(0);
 
#endif
