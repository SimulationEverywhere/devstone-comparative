//
// Created by rcardenas on 3/9/20.
//

#include "DEVStone.hpp"


DEVStone::DEVStone(const string& typeIn, int depthIn, int widthIn, int intDelayIn, int extDelayIn, double procTimeIn):
        adevs::Digraph<int*>()
{
    Seeder *seeder = new Seeder(1);
    add(seeder);

    adevs::Digraph<int*> devstone;
    if (typeIn == "LI") {
        LI *li = new LI(depthIn, widthIn, intDelayIn, extDelayIn, procTimeIn);
        add(li);
        couple(seeder, seeder->out, li, li->in);
    } else if (typeIn == "HI") {
        HI *hi = new HI(depthIn, widthIn, intDelayIn, extDelayIn, procTimeIn);
        add(hi);
        couple(seeder, seeder->out, hi, hi->in);
    } else if (typeIn == "HO") {
        HO *ho = new HO(depthIn, widthIn, intDelayIn, extDelayIn, procTimeIn);
        add(ho);
        couple(seeder, seeder->out, ho, ho->in);
    } else if (typeIn == "HOmod") {
        HOmod *homod = new HOmod(depthIn, widthIn, intDelayIn, extDelayIn, procTimeIn);
        add(homod);
        couple(seeder, seeder->out, homod, homod->in1);
        couple(seeder, seeder->out, homod, homod->in2);
    } else {
        throw runtime_error("Unable to determine DEVStone model type");
    }
}


int main(int argc, char* argv[]) {
    auto start = hclock::now();

    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("kind", po::value<string>()->required(), "set kind of devstone: LI, HI, HO, or HOmod")
            ("width", po::value<int>()->required(), "set width of the DEVStone: integer value")
            ("depth", po::value<int>()->required(), "set depth of the DEVStone: integer value")
            ("int-cycles", po::value<int>()->required(), "set the Dhrystone cycles to expend in internal transtions: integer value")
            ("ext-cycles", po::value<int>()->required(), "set the Dhrystone cycles to expend in external transtions: integer value")
            ("time-advance", po::value<int>()->default_value(1), "set the time expend in external transtions by the Dhrystone in miliseconds: integer value")
            ;

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch ( boost::program_options::required_option be ){
        if (vm.count("help")) {
            cout << desc << "\n";
            return 0;
        } else {
            cout << be.what() << endl;
            cout << endl;
            cout << "for mode information run: " << argv[0] << " --help" << endl;
            return 1;
        }
    }
    string kind = vm["kind"].as<string>();
    if (kind != "LI"  && kind != "HI" && kind != "HO" && kind != "HOmod") {
        cout << "The kind needs to be LI, HI, HO, or HOmod and received value was: " << kind;
        cout << endl;
        cout << "for mode information run: " << argv[0] << " --help" << endl;
        return 1;
    }

    int width = vm["width"].as<int>();
    int depth = vm["depth"].as<int>();
    int int_cycles = vm["int-cycles"].as<int>();
    int ext_cycles = vm["ext-cycles"].as<int>();
    int time_advance = vm["time-advance"].as<int>();

    auto processed_parameters = hclock::now();

    DEVStone *devstone = new DEVStone(kind, depth, width, int_cycles, ext_cycles, time_advance);

    auto model_built = hclock::now();

    cout << endl;
    cout << "Model creation time: " << chrono::duration_cast<chrono::duration<double, ratio<1>>>( model_built - processed_parameters).count() << endl;
    adevs::Simulator<IO_Type> sim(devstone);

    auto model_init = hclock::now();

    cout << "Engine setup time: " << chrono::duration_cast<chrono::duration<double, ratio<1>>>( model_init - model_built).count() << endl;
    while (sim.nextEventTime() < DBL_MAX)
    {
        sim.execNextEvent();
    }
    /// Done!
    auto finished_simulation = hclock::now();
    cout << "Simulation time: " << chrono::duration_cast<chrono::duration<double, ratio<1>>>( finished_simulation - model_init).count() << endl;
    return 0;
}
