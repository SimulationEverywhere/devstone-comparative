#ifndef _DevStonecoupledHO_h_
#define _DevStonecoupledHO_h_

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "adevs.h"
#include "DevStoneAtomic.h"

class DevStoneCoupledHO : public adevs::Digraph<long> {
protected:
	std::string name;
public:
	/// Model input ports
	static const int in1, in2;
	/// Model output ports
	static const int out1, out2;
   
    DevStoneCoupledHO(const std::string& prefix, int width, int depth, double preparationTime, double intDelayTime, double extDelayTime) {
		char buffer[40];
		snprintf(buffer, sizeof(buffer), "%d", depth-1);
		this->name = prefix;		
		this->name.append(buffer);
		//DevStoneCoupledHO::in1=0;
        if (depth == 1) {
            DevStoneAtomic* atomic = new DevStoneAtomic(std::string("A1_") + name, preparationTime, intDelayTime, extDelayTime);
            add(atomic);
            couple(this, DevStoneCoupledHO::in2, atomic, DevStoneAtomic::in);
            couple(atomic, DevStoneAtomic::out, this, DevStoneCoupledHO::out2);
        } else {
            DevStoneCoupledHO* coupled = new DevStoneCoupledHO(prefix, width, depth - 1, preparationTime, intDelayTime, extDelayTime);
            add(coupled);
            couple(this, DevStoneCoupledHO::in1, coupled, DevStoneCoupledHO::in1);
	    couple(this, DevStoneCoupledHO::in1, coupled, DevStoneCoupledHO::in2);
            couple(coupled, DevStoneCoupledHO::out1, this, DevStoneCoupledHO::out1);
	    DevStoneAtomic* atomicPrev = 0;
            for (int i = 0; i < (width - 1); ++i) {
		snprintf(buffer, sizeof(buffer), "%d", i+1);
                DevStoneAtomic* atomic = new DevStoneAtomic(std::string("A") + buffer + "_" + name, preparationTime, intDelayTime, extDelayTime);
                add(atomic);
                couple(this, DevStoneCoupledHO::in2, atomic, DevStoneAtomic::in);
		couple(atomic, DevStoneAtomic::out, this, DevStoneCoupledHO::out2);
                if(atomicPrev!=0) {
                    couple(atomicPrev, DevStoneAtomic::out, atomic, DevStoneAtomic::in);
                }
                atomicPrev = atomic;
            }
        }
    }    
};

const int DevStoneCoupledHO::in1(0);
const int DevStoneCoupledHO::in2(1);
const int DevStoneCoupledHO::out1(2);
const int DevStoneCoupledHO::out2(3);

#endif
