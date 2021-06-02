#include <iostream>
#include <ctime>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>
#include "adevs.h"
#include "DevStoneGenerator.h"
#include "DevStoneCoupledLI.h"
#include "DevStoneCoupledHI.h"
#include "DevStoneCoupledHO.h"
#include "DevStoneCoupledHOmod.h"
#include "DevStoneCoupledRita.h"
#include "DevStoneCoupledTrivial.h"

#define BENCH_LI      0
#define BENCH_HI      1
#define BENCH_HO      2
#define BENCH_HOmod   3
#define BENCH_HOmem   4
#define BENCH_Trivial 5

const char* bench[]={"LI", "HI", "HO", "HOmod", "HOmem", "Trivial"};

typedef adevs::PortValue<long> PortValue;

void usage(char* name) {
  fprintf(stderr, "Usage: %s [-w width] [-d depth] [-m MaxEvents] [-b {HI | HO | HOmod | HOmem | Trivial}]\nwith w>=2, d>=2, m>=1\n",
	  name);
}

/* Number of external Events */

/* HOmod */
int W(int w, int i) {
  return ((w-i)>0?w-i:0);
}

int K(int w, int l) {
  if (l == 1) return 1;
  return K(w, l-1) + W(w, 1);
}

int P(int w, int l, int j) {
  if (j < 1 || j > K(w,l)) return 0;
  if (j == 1 && l == 1) return 1;
  int p=0;
  for (int i=1 ; i <= w; i++)
    p += P(w,  l-1, j);
  return (w-1)*p;
}
  
long int HOmod_lim(int w, int d) {
  //int i, j, l, c;
  
  long int n=1;
  for (int l=1; l <= d-1; l++)
    for (int c=1; c <= K(w,l)+w-1; c++)
      for (int i=1; i <= w; i++)
	n += (W(w,1) + W(w,i))*P(w, l, c-i+1);
  
  return n;
}

/* HOmem */

long int HOmem_lim(int w, int d) {
  int l;
  long int n=1;
  for (l=1; l <= d-1; l++)
    n+=w*pow(w-1,2*l-1);
  
  return n;
}


int main(int argc, char *argv[]) {
	// Common parameters
	double preparationTime = 0.0;
	double period = 1.0;
	//long maxEvents = 6000;
	long maxEvents = 1;
	double intDelayTime = 0;
	double extDelayTime = 0;
	struct rusage resources;
	char* strbenchmark = NULL;
	int benchmark = BENCH_HI;
	int width = 5, depth = 4;
	int opt;
	int n_events = 0;
	adevs::Digraph<long> framework;
	DevStoneGenerator* generator;
	DevStoneGenerator* trigger;   
	
	DevStoneCoupledLI* coupled_li;
	DevStoneCoupledHI* coupled_hi;
	DevStoneCoupledHO* coupled_ho;
	DevStoneCoupledHOmod* coupled_homod;
	DevStoneCoupledRita* coupled_rita;
	DevStoneCoupledTrivial* coupled_trivial;
	
	// TODO
	// Evaluate parameteres: benchmark, width and depth
	while ((opt = getopt(argc, argv, "w:d:b:m:")) != -1) {
	  switch (opt) {
	    case 'w':
	      width = atoi(optarg);
	      break;
	    case 'd':
	      depth = atoi(optarg);
	      break;
	    case 'b':
	      strbenchmark = optarg;
	      break;
	    case 'm':
	      maxEvents = atoi(optarg);
	      break;
	    default: /* '?' */
	      usage(argv[0]);
	      exit(EXIT_FAILURE);
	  }
	}
	
	if (strbenchmark == NULL) {
	  benchmark = BENCH_HI;
	} else if (strncmp(strbenchmark, "LI", 2) == 0) {
	  benchmark = BENCH_LI;
	} else if (strncmp(strbenchmark, "HI", 2) == 0) {
	  benchmark = BENCH_HI;
	} else if (strncmp(strbenchmark, "HOmod", 5) == 0) {
	  benchmark = BENCH_HOmod;
	} else if (strncmp(strbenchmark, "HOmem", 5) == 0) {
	  benchmark = BENCH_HOmem;
	} else if (strncmp(strbenchmark, "HO", 2) == 0) {
	  benchmark = BENCH_HO;
	} else if (strncmp(strbenchmark, "Trivial", 7) == 0) {
	  benchmark = BENCH_Trivial;
	}
// 	cout << "Width = " << width << endl;
// 	cout << "Depth = " << depth << endl;
// 	cout << "MaxEvents = " << maxEvents << endl;
// 	cout << "Benchmark: " << bench[benchmark] << endl; 
	if ((width <= 0) || (depth <= 0) || (maxEvents <= 0)) {
	  usage(argv[0]);
	  exit(EXIT_FAILURE);
	}

	struct timespec ts_start0, ts_start1, ts_start2, ts_end;
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts_start0);
	
	generator = new DevStoneGenerator("Generator", preparationTime, period, maxEvents);
	trigger = new DevStoneGenerator("Trigger", preparationTime, period, 1); /* Just one event */
	framework.add(generator);
	framework.add(trigger);
	
	switch (benchmark) {

	/*********************************************************************************************/
	// LI
	//int width = 1000;
	//int depth = 5;
	  case BENCH_LI:
	    /*DevStoneCoupledLI* */ coupled_li = new DevStoneCoupledLI("C", width, depth, preparationTime, intDelayTime, extDelayTime);
	    framework.add(coupled_li);
	    framework.couple(generator, DevStoneGenerator::out, coupled_li, DevStoneCoupledLI::in);
	    break;
	/*********************************************************************************************/
	// HI
	//int width = 100;
	//int depth = 5;
	  case BENCH_HI:
	    /*DevStoneCoupledHI* */ coupled_hi = new DevStoneCoupledHI("C", width, depth, preparationTime, intDelayTime, extDelayTime);
	    framework.add(coupled_hi);
	    framework.couple(generator, DevStoneGenerator::out, coupled_hi, DevStoneCoupledHI::in);
	    break;
	/*********************************************************************************************/
	// HO
	//int width = 100;
	//int depth = 5;
	  case BENCH_HO:
	    /* DevStoneCoupledHO* */ coupled_ho = new DevStoneCoupledHO("C", width, depth, preparationTime, intDelayTime, extDelayTime);
	    framework.add(coupled_ho);
	    framework.couple(trigger, DevStoneGenerator::out, coupled_ho, DevStoneCoupledHO::in1);
	    framework.couple(trigger, DevStoneGenerator::out, coupled_ho, DevStoneCoupledHO::in2);
	    break;
	/*********************************************************************************************/
	// HOmod
	//int width = 5;
	//int depth = 4;
	  case BENCH_HOmod:
	    /* DevStoneCoupledHOmod* */ coupled_homod = new DevStoneCoupledHOmod("C", width, depth, preparationTime, intDelayTime, extDelayTime);
	    framework.add(coupled_homod);
	    framework.couple(trigger, DevStoneGenerator::out, coupled_homod, DevStoneCoupledHOmod::in1);
	    framework.couple(trigger, DevStoneGenerator::out, coupled_homod, DevStoneCoupledHOmod::in2);
	    //n_events = HOmod_lim(width, depth);
	    break;
	/*********************************************************************************************/
	// HOmem (Rita)
	//int width = 5;
	//int depth = 4;
	  case BENCH_HOmem:
	    /* DevStoneCoupledRita* */ coupled_rita = new DevStoneCoupledRita("C", width, depth, preparationTime, intDelayTime, extDelayTime);
	    framework.add(coupled_rita);
	    framework.couple(trigger, DevStoneGenerator::out, coupled_rita, DevStoneCoupledRita::in1);
	    framework.couple(trigger, DevStoneGenerator::out, coupled_rita, DevStoneCoupledRita::in2);
	    n_events=1;
	    for (int l = 1; l <= depth-1 ; l++)
	      n_events += pow((width-1),(2*l)) + pow((width-1),(2*l-1));
	    n_events = HOmem_lim(width, depth);
	    break;
	/*********************************************************************************************/
	// Trivial
	//int width = 5;
	//int depth = 1;
	  case BENCH_Trivial:
	   /*  DevStoneCoupledTrivial* */ coupled_trivial = new DevStoneCoupledTrivial("C", width, depth, preparationTime, intDelayTime, extDelayTime);
	    framework.add(coupled_trivial);
	    framework.couple(trigger, trigger->out, coupled_trivial, coupled_trivial->in);
	    break;
	/*********************************************************************************************/
	  default:
	    cerr << "Unrecognised benchmark" << endl;
	    exit(EXIT_FAILURE);
	}
	
	
//	time_t start = clock();
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts_start1);
	adevs::Simulator<PortValue> sim(&framework);
//	time_t start = clock();
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts_start2);
	while (sim.nextEventTime() < DBL_MAX) {
		sim.execNextEvent();
	}
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts_end);
//	time_t end = clock();
//	double time = (double)(end-start)/CLOCKS_PER_SEC;
	long double time_model = (long double)ts_start1.tv_sec - (long double)ts_start0.tv_sec + ((long double)ts_start1.tv_nsec - (long double)ts_start0.tv_nsec)/(double)1e9;
	long double time_setup = (long double)ts_start2.tv_sec - (long double)ts_start1.tv_sec + ((long double)ts_start2.tv_nsec - (long double)ts_start1.tv_nsec)/(double)1e9;
	long double time_sim = (long double)ts_end.tv_sec - (long double)ts_start2.tv_sec + ((long double)ts_end.tv_nsec - (long double)ts_start2.tv_nsec)/(double)1e9;

	if (getrusage(RUSAGE_SELF, &resources)) {
		perror ("rusage");
	}
	

	//cout << "MEM: " << resources.ru_utime.tv_sec << "." << resources.ru_utime.tv_usec << ", " << resources.ru_maxrss << endl;

// 	std::cout << "Execution time (PreparationTime, Period, MaxEvents, Width, Depth, IntDelayTime, ExtDelatTime) = (" << preparationTime << ", " << period << ", " << maxEvents << ", " << width << ", " << depth << ", " << intDelayTime << ", " << extDelayTime << ") = " << time << std::endl;

//     cout << "STATS:";
// 	cout << "PreparationTime = " << preparationTime << std::endl;
// 	cout << "Period = " << period << std::endl;
// 	cout << "MaxEvents = " << maxEvents << std::endl;
// 	cout << "Width = " << width << std::endl;
// 	cout << "Depth = " << depth << std::endl;
// 	cout << "IntDelayTime = " << intDelayTime << std::endl;
// 	cout << "ExtDelatTime = " << extDelayTime << std::endl;
// 	cout << "Num delta_int = " << DevStoneAtomic::NUM_DELT_INTS << " [" << maxEvents*((width-1)*(depth-1)+1) << "]" << std::endl;
// 	cout << "Num delta_ext = " << DevStoneAtomic::NUM_DELT_EXTS << " [" << maxEvents*((width-1)*(depth-1)+1) << "]" << std::endl;
// 	cout << "Num event_ext = " << DevStoneAtomic::NUM_EVENT_EXTS << " [" << maxEvents*((width-1)*(depth-1)+1) << "]" << std::endl;
// 	cout << "SIMULATION TIME = " << time << std::endl;

	cout << "STATS" << endl;
	cout << "Benchmark: " << strbenchmark << endl;
	cout << "PreparationTime: " << preparationTime << endl;
	cout << "Period: " << period << endl;
	cout << "MaxEvents: " << maxEvents << endl;
	cout << "Width: " << width << endl;
	cout << "Depth: " << depth << endl;
	cout << "IntDelayTime: " << intDelayTime << endl;
	cout << "ExtDelatTime: " << extDelayTime << endl;
	cout << "Num delta_int: " << DevStoneAtomic::NUM_DELT_INTS << ", [" << maxEvents*((width-1)*(depth-1)+1) << "]" << endl;
	cout << "Num delta_ext: " << DevStoneAtomic::NUM_DELT_EXTS << ", [" << maxEvents*((width-1)*(depth-1)+1) << "]" << endl;
	//cout << "Num event_ext: " << DevStoneAtomic::NUM_EVENT_EXTS << ", [" << n_events << "]" << endl;
	cout << "Model creation time: " << time_model << endl;
	cout << "Engine setup time: " << time_setup << endl;
	cout << "Simulation time: " << time_sim << endl;//" , " << time2 << ", (" << 200*(time1-time2)/(time1+time2) << "%)" << endl;
	cout << "MEMORY: " << resources.ru_maxrss << endl;
	//cout << std::endl;
	
//	cout << "Tamaño: " << sizeof(adevs::Bag<PortValue>) << endl;
	return 0;
}

