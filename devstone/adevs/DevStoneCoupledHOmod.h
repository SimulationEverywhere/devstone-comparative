#ifndef _DevStonecoupledHOmod_h_
#define _DevStonecoupledHOmod_h_

#include <string>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include "adevs.h"
#include "DevStoneAtomic.h"

class DevStoneCoupledHOmod : public adevs::Digraph<long> {
protected:
	std::string name;
public:
	/// Model input ports
	static const int in1, in2;
	/// Model output ports
	static const int out;
   
    DevStoneCoupledHOmod(const std::string& prefix, int width, int depth, double preparationTime, double intDelayTime, double extDelayTime) {
	char buffer[40];
	snprintf(buffer, sizeof(buffer), "%d", depth-1);
	this->name = prefix;		
	this->name.append(buffer);
		
        if (depth == 1) { // Deepest
        	//printf("Depth: 1\n");
            DevStoneAtomic* atomic = new DevStoneAtomic(std::string("A1_") + name, preparationTime, intDelayTime, extDelayTime);
            add(atomic);
            couple(this, DevStoneCoupledHOmod::in1, atomic, DevStoneAtomic::in);
            couple(atomic, DevStoneAtomic::out, this, DevStoneCoupledHOmod::out);
        } else {
        	//printf("Depth: %d\n", depth);
            DevStoneCoupledHOmod* c = new DevStoneCoupledHOmod(prefix, width, depth - 1, preparationTime, intDelayTime, extDelayTime);
            add(c);
            couple(this, DevStoneCoupledHOmod::in1, c, DevStoneCoupledHOmod::in1);
		    couple(c, DevStoneCoupledHOmod::out, this, DevStoneCoupledHOmod::out);
		    
		    if (width > 1) {
            std::map<int, std::list<DevStoneAtomic*>> atomics;
            for (int i = 0; i < width; i++) {
                int minRowIdx = (i < 2)? 0 : (i - 1);
                for (int j = minRowIdx; j < width - 1; j++) {
                	snprintf(buffer, sizeof(buffer), "%d_%d_%d", depth, i+1, j+1);
                    DevStoneAtomic *a = new DevStoneAtomic(std::string("A") + buffer + "_" + name, preparationTime, intDelayTime, extDelayTime);
                    add(a);

                    atomics.insert(std::pair<int, list<DevStoneAtomic*>>(i, std::list<DevStoneAtomic*>()));  // en teoria insert no machaca
                    atomics.find(i)->second.push_back(a);
                }
            }
            // Connect EIC
            for (auto a: atomics.find(0)->second) {
                couple(this, DevStoneCoupledHOmod::in2, a, DevStoneAtomic::in);
            }
            for (int i = 1; i < width; i++) {
                DevStoneAtomic *a = atomics.find(i)->second.front();
                couple(this, DevStoneCoupledHOmod::in2, a, DevStoneAtomic::in);
            }
            // Connect IC
            for (auto a: atomics.find(0)->second) {
                couple(a, DevStoneAtomic::out, c, DevStoneCoupledHOmod::in2);
            }
            for (int i = 0; i < atomics.find(0)->second.size(); i++) {
                DevStoneAtomic *aTop = *std::next(atomics.find(0)->second.begin(), i);
                for (int j = 0; j < atomics.find(1)->second.size(); j++) {
                    DevStoneAtomic *aDown = *std::next(atomics.find(1)->second.begin(), j);
                    couple(aDown, DevStoneAtomic::out, aTop, DevStoneAtomic::in);
                }
            }
            for (int i = 2; i < width; i++) {
                for (int j = 0; j < atomics.find(i)->second.size(); j++) {
                    DevStoneAtomic *aFrom = *std::next(atomics.find(i)->second.begin(), j);
                    DevStoneAtomic *aTo = *std::next(atomics.find(i - 1)->second.begin(), j + 1);
                    couple(aFrom, DevStoneAtomic::out, aTo, DevStoneAtomic::in);
                }
            }
        }
        }
    }    
};

const int DevStoneCoupledHOmod::in1(0);
const int DevStoneCoupledHOmod::in2(1);
const int DevStoneCoupledHOmod::out(2);

#endif
