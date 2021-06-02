#ifndef __DevStoneAtomic_h_
#define __DevStoneAtomic_h_
#include <cstdlib>
#include <list>
#include <string>
#include "adevs.h"
#include "Dhrystone.h"

typedef adevs::PortValue<long> PortValue;

class DevStoneAtomic: public adevs::Atomic<PortValue> {
protected:	
	std::string name;
	double sigma;
    double preparationTime;
    double intDelayTime;
    double extDelayTime;
    std::list<long> outValues;
    Dhrystone dhrystone;
public:
	/// Model input port
	static const int in;
	/// Model output port
	static const int out;
	// STATS
	static long NUM_DELT_INTS;
	static long NUM_DELT_EXTS;
	static long NUM_EVENT_EXTS;

	/// Constructor.
	DevStoneAtomic(const std::string& name, double preparationTime, double intDelayTime, double extDelayTime) : adevs::Atomic<PortValue>(), outValues(), dhrystone() {
	this->name = name;
	this->sigma = DBL_MAX;
	this->preparationTime = preparationTime;
        this->intDelayTime = intDelayTime;
        this->extDelayTime = extDelayTime;
	}
		
	/// Internal transition function
	void delta_int() {
		NUM_DELT_INTS++;
		outValues.clear();
		if(intDelayTime > 0) dhrystone.execute(intDelayTime);
		sigma = DBL_MAX;
	}
	
	/// External transition function
	void delta_ext(double e, const adevs::Bag<PortValue>& x) {
		NUM_DELT_EXTS++;
		sigma -= e;
		if(extDelayTime > 0) dhrystone.execute(extDelayTime);
		outValues.push_back(0);
        	sigma = preparationTime;
	}
	
	/// Confluent transition function.
	void delta_conf(const adevs::Bag<PortValue>& x) {
		delta_int();
		delta_ext(0.0,x);
	}
		
	/// Output function.  
	void output_func(adevs::Bag<PortValue>& y) {
		std::list<long>::iterator itr = outValues.begin();
		for(; itr!=outValues.end(); ++itr) {
			PortValue pv(out, *itr);
			y.insert(pv);
		}
	}
		
	/// Time advance function.
	double ta() { 
		return sigma; 
	}
		
	/**
	Garbage collection. No heap allocation in output_func, so
	do nothing.
	*/
	void gc_output(adevs::Bag<PortValue>& g) {
	}	
};

/// Create unique 'names' for the model ports.
const int DevStoneAtomic::in(0);
const int DevStoneAtomic::out(1);

long DevStoneAtomic::NUM_DELT_INTS(0);
long DevStoneAtomic::NUM_DELT_EXTS(0);
long DevStoneAtomic::NUM_EVENT_EXTS(0);

#endif
