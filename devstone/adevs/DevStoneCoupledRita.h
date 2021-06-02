#ifndef _DevStonecoupledRita_h_
#define _DevStonecoupledRita_h_

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "adevs.h"
#include "DevStoneAtomic.h"
#include "DevStoneSink.h"

class DevStoneCoupledRita : public adevs::Digraph<long> {
protected:
	std::string name;
public:
	/// Model input ports
	static const int in1, in2;
	/// Model output ports
	static const int out;
   
    DevStoneCoupledRita(const std::string& prefix, int width, int depth, double preparationTime, double intDelayTime, double extDelayTime) {
	char buffer[40];
	snprintf(buffer, sizeof(buffer), "%d", depth-0);
	this->name = prefix;		
	this->name.append(buffer);
		
        if (depth == 1) {
            DevStoneAtomic* atomic = new DevStoneAtomic(std::string("A1_") + name, preparationTime, intDelayTime, extDelayTime);
            add(atomic);
            couple(this, DevStoneCoupledRita::in1, atomic, DevStoneAtomic::in);
            couple(atomic, DevStoneAtomic::out, this, DevStoneCoupledRita::out);
	    // Connect in2 to sink::in, in order to prevent in2 queue overflowing.
	    /*DevStoneSink* sink = new DevStoneSink(std::string("Sink_") + name, preparationTime, intDelayTime, extDelayTime);
	    couple(this, this->in2, sink, sink->in);*/
        } else {
            DevStoneCoupledRita* coupled = new DevStoneCoupledRita(prefix, width, depth - 1, preparationTime, intDelayTime, extDelayTime);
            add(coupled);
            couple(this, DevStoneCoupledRita::in1, coupled, DevStoneCoupledRita::in1);
	    couple(coupled, DevStoneCoupledRita::out, this, DevStoneCoupledRita::out);
	    // First set of Atomics 
	    DevStoneAtomic** atomic_1;
	    atomic_1 = (DevStoneAtomic**) malloc((width - 0) * sizeof(DevStoneAtomic*));
	    for (int i = 0; i < (width - 1); i++) {
	      snprintf(buffer, sizeof(buffer), "[%d]", i);
	      atomic_1[i] = new DevStoneAtomic(std::string("A[0]") + buffer + "_" + name, preparationTime, intDelayTime, extDelayTime);
              add(atomic_1[i]);
	      //couple(this, this->in2, atomic_1[i], atomic_1[i]->in);
	      couple(atomic_1[i], atomic_1[i]->out, coupled, coupled->in2);
	      // cout << "(Depth=" << depth << ") Connected A[0][" << i << "]::out with C(" << depth - 1 << ")::in2" << endl;
	    }
	    // Second set of Atomics
            for (int i = 0; i < (width - 1); i++) {
		snprintf(buffer, sizeof(buffer), "[%d][0]", i+1);
                DevStoneAtomic* atomic = new DevStoneAtomic(std::string("A") + buffer + "_" + name, preparationTime, intDelayTime, extDelayTime);
                add(atomic);
                couple(this, this->in2, atomic, atomic->in);
		// cout << "(Depth=" << depth << ") Connected in2 with A[" << i+1 << "][0]::in" << endl; 
		// connect output to inputs in first set
		for (int j = 0; j < (width - 1) ; j++) {
		  couple(atomic, atomic->out, atomic_1[j], atomic_1[j]->in);
		  // cout << "(Depth=" << depth << ") Connected A[" << i+1 << "][0]::out with A[0][" << j << "]::in" << endl;
		}
            }
        }
    }    
};

const int DevStoneCoupledRita::in1(0);
const int DevStoneCoupledRita::in2(1);
const int DevStoneCoupledRita::out(2);

#endif
