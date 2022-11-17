#include <chrono>
#include <iostream>
#include <string>
#include "devstone.hpp"
using namespace std;
using hclock=chrono::high_resolution_clock;

int main(int argc, char* argv[]) {
    // First, we parse the arguments
    if (argc < 4) {
        std::cerr << "ERROR: not enough arguments" << std::endl;
        std::cerr << "    Usage:" << std::endl;
        std::cerr << "    > devstone MODEL_TYPE WIDTH DEPTH INTDELAY EXTDELAY" << std::endl;
        std::cerr << "        (MODEL_TYPE must be either LI, HI, HO, or HOmod)" << std::endl;
        std::cerr << "        (WIDTH and DEPTH must be greater than or equal to 1)" << std::endl;
        std::cerr << "        (INTDELAY and EXTDELAY must be greater than or equal to 0 ms)" << std::endl;
        std::cerr << "    Alternative usages:" << std::endl;
        std::cerr << "    > main_devstone MODEL_TYPE WIDTH DEPTH DELAY" << std::endl;
        std::cerr << "        (INTDELAY and EXTDELAY are set to DELAY ms)" << std::endl;
        std::cerr << "    > main_devstone MODEL_TYPE WIDTH DEPTH" << std::endl;
        std::cerr << "        (INTDELAY and EXTDELAY are set to 0 ms)" << std::endl;
        return -1;
    }
    std::string type = argv[1];
    int width = std::stoi(argv[2]);
    int depth = std::stoi(argv[3]);
    int intDelay = 0;
    int extDelay = 0;
    if (argc > 4) {
        intDelay = std::stoi(argv[4]);
        extDelay = (argc == 5) ? intDelay : std::stoi(argv[5]);
    }
    auto processed_parameters = hclock::now();

    auto *devstone = new DEVStone(type, width, depth, intDelay, extDelay);

    auto model_built = hclock::now();

    cout << endl;
    cout << "Model creation time: " << chrono::duration_cast<chrono::duration<double, ratio<1>>>( model_built - processed_parameters).count() << " seconds" << endl;
    adevs::Simulator<IO_Type> sim(devstone);

    auto model_init = hclock::now();

    cout << "Engine setup time: " << chrono::duration_cast<chrono::duration<double, ratio<1>>>( model_init - model_built).count() << " seconds" << endl;
    while (sim.nextEventTime() < DBL_MAX) {
        sim.execNextEvent();
    }
    /// Done!
    auto finished_simulation = hclock::now();
    cout << "Simulation time: " << chrono::duration_cast<chrono::duration<double, ratio<1>>>( finished_simulation - model_init).count() << " seconds" << endl;
    return 0;
}
