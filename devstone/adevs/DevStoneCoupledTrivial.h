#ifndef _DevStoneCoupledTrivial_h_
#define _DevStoneCoupledTrivial_h_

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "adevs.h"
#include "DevStoneAtomic.h"
  
class DevStoneCoupledTrivial : public adevs::Digraph<long> {
protected:
	std::string name;
public:
    /// Model input port
    static const int in;
    /// Model output port
    static const int out;
    
    DevStoneCoupledTrivial(const std::string& prefix, int width, int depth, double preparationTime, double intDelayTime, double extDelayTime) {
	char buffer[40];
	snprintf(buffer, sizeof(buffer), "%d", depth-1);
	this->name = prefix;		
	this->name.append(buffer);
	DevStoneAtomic* atomic1 = new DevStoneAtomic(std::string("A1") + "_" + name, preparationTime, intDelayTime, extDelayTime);
	add(atomic1);
	DevStoneAtomic* atomic2 = new DevStoneAtomic(std::string("A2") + "_" + name, preparationTime, intDelayTime, extDelayTime);
	add(atomic2);
	DevStoneAtomic* atomic3 = new DevStoneAtomic(std::string("A3") + "_" + name, preparationTime, intDelayTime, extDelayTime);
	add(atomic3);
	DevStoneAtomic* atomic4 = new DevStoneAtomic(std::string("A4") + "_" + name, preparationTime, intDelayTime, extDelayTime);
	add(atomic4);
	
	couple(this, this->in, atomic1, atomic1->in);
	couple(this, this->in, atomic2, atomic2->in);
	couple(this, this->in, atomic3, atomic3->in);
	couple(atomic4, atomic4->out, this, this->out);
	couple(atomic2, atomic2->out, atomic4, atomic4->in);
        couple(atomic1, atomic1->out, atomic4, atomic4->in);
	couple(atomic3, atomic3->out, atomic4, atomic4->in);
        
// 	if (depth == 1) {
//             DevStoneAtomic* atomic = new DevStoneAtomic(std::string("A1_") + name, preparationTime, intDelayTime, extDelayTime);
//             add(atomic);
//             couple(this, DevStoneCoupledTrivial::in, atomic, DevStoneAtomic::in);
//             couple(atomic, DevStoneAtomic::out, this, DevStoneCoupledTrivial::out);
//         } else {
//             DevStoneCoupledTrivial* coupled = new DevStoneCoupledTrivial(prefix, width, depth - 1, preparationTime, intDelayTime, extDelayTime);
//             add(coupled);
//             couple(this, DevStoneCoupledTrivial::in, coupled, DevStoneCoupledTrivial::in);
//             couple(coupled, DevStoneCoupledTrivial::out, this, DevStoneCoupledTrivial::out);
// 	    DevStoneAtomic* atomicPrev = 0;
//             for (int i = 0; i < (width - 1); ++i) {
// 		snprintf(buffer, sizeof(buffer), "%d", i+1);
//                 DevStoneAtomic* atomic = new DevStoneAtomic(std::string("A") + buffer + "_" + name, preparationTime, intDelayTime, extDelayTime);
//                 add(atomic);
//                 couple(this, DevStoneCoupledTrivial::in, atomic, DevStoneAtomic::in);
//                 if(atomicPrev!=0) {
//                     couple(atomicPrev, DevStoneAtomic::out, atomic, DevStoneAtomic::in);
//                 }
//                 atomicPrev = atomic;
//             }
//         }
    }    
};

const int DevStoneCoupledTrivial::in=0;
const int DevStoneCoupledTrivial::out=1;

#endif
