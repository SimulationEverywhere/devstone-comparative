#ifndef _DevStonecoupledHI_h_
#define _DevStonecoupledHI_h_

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "adevs.h"
#include "DevStoneAtomic.h"

class DevStoneCoupledHI : public adevs::Digraph<long> {
protected:
	std::string name;
public:
	/// Model input port
	static const int in;
	/// Model output port
	static const int out;
   
    DevStoneCoupledHI(const std::string& prefix, int width, int depth, double preparationTime, double intDelayTime, double extDelayTime) {
	char buffer[40];
	snprintf(buffer, sizeof(buffer), "%d", depth-1);
	this->name = prefix;		
	this->name.append(buffer);
        if (depth == 1) {
            DevStoneAtomic* atomic = new DevStoneAtomic(std::string("A1_") + name, preparationTime, intDelayTime, extDelayTime);
            add(atomic);
            couple(this, DevStoneCoupledHI::in, atomic, DevStoneAtomic::in);
            couple(atomic, DevStoneAtomic::out, this, DevStoneCoupledHI::out);
        } else {
            DevStoneCoupledHI* coupled = new DevStoneCoupledHI(prefix, width, depth - 1, preparationTime, intDelayTime, extDelayTime);
            add(coupled);
            couple(this, DevStoneCoupledHI::in, coupled, DevStoneCoupledHI::in);
            couple(coupled, DevStoneCoupledHI::out, this, DevStoneCoupledHI::out);
	    DevStoneAtomic* atomicPrev = 0;
            for (int i = 0; i < (width - 1); ++i) {
		snprintf(buffer, sizeof(buffer), "%d", i+1);
                DevStoneAtomic* atomic = new DevStoneAtomic(std::string("A") + buffer + "_" + name, preparationTime, intDelayTime, extDelayTime);
                add(atomic);
                couple(this, DevStoneCoupledHI::in, atomic, DevStoneAtomic::in);
                if(atomicPrev!=0) {
                    couple(atomicPrev, DevStoneAtomic::out, atomic, DevStoneAtomic::in);
                }
                atomicPrev = atomic;
            }
        }
    }    
};

const int DevStoneCoupledHI::in(0);
const int DevStoneCoupledHI::out(1);

#endif
