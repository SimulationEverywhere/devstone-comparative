#ifndef _DevStonecoupledLI_h_
#define _DevStonecoupledLI_h_

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "adevs.h"
#include "DevStoneAtomic.h"

class DevStoneCoupledLI : public adevs::Digraph<long> {
protected:
	std::string name;
public:
	/// Model input port
	static const int in;
	/// Model output port
	static const int out;
   
    DevStoneCoupledLI(const std::string& prefix, int width, int depth, double preparationTime, double intDelayTime, double extDelayTime) {
		char buffer[40];
		snprintf(buffer, sizeof(buffer), "%d", depth-1);
		this->name = prefix;		
		this->name.append(buffer);
        if (depth == 1) {
            DevStoneAtomic* atomic = new DevStoneAtomic(std::string("A1_") + name, preparationTime, intDelayTime, extDelayTime);
            add(atomic);
            couple(this, DevStoneCoupledLI::in, atomic, DevStoneAtomic::in);
            couple(atomic, DevStoneAtomic::out, this, DevStoneCoupledLI::out);
        } else {
            DevStoneCoupledLI* coupled = new DevStoneCoupledLI(prefix, width, depth - 1, preparationTime, intDelayTime, extDelayTime);
            add(coupled);
            couple(this, DevStoneCoupledLI::in, coupled, DevStoneCoupledLI::in);
            couple(coupled, DevStoneCoupledLI::out, this, DevStoneCoupledLI::out);
            for (int i = 0; i < (width - 1); ++i) {
				snprintf(buffer, sizeof(buffer), "%d", i+1);
                DevStoneAtomic* atomic = new DevStoneAtomic(std::string("A") + buffer + "_" + name, preparationTime, intDelayTime, extDelayTime);
                add(atomic);
                couple(this, DevStoneCoupledLI::in, atomic, DevStoneAtomic::in);
            }
        }
    }    
};

const int DevStoneCoupledLI::in(0);
const int DevStoneCoupledLI::out(1);

#endif
