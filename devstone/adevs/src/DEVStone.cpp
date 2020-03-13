//
// Created by rcardenas on 3/9/20.
//

#include "DEVStone.hpp"


int main(int argc, char* argv[]) {
    auto start = hclock::now();

    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("kind", po::value<string>()->required(), "set kind of devstone: LI, HI or HO")
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
    if (kind != "LI"  && kind != "HI" && kind != "HO") {
        cout << "The kind needs to be LI, HI or HO and received value was: " << kind;
        cout << endl;
        cout << "for mode information run: " << argv[0] << " --help" << endl;
        return 1;
    }

    int width = vm["width"].as<int>();
    int depth = vm["depth"].as<int>();
    int int_cycles = vm["int-cycles"].as<int>();
    int ext_cycles = vm["ext-cycles"].as<int>();
    int time_advance = vm["time-advance"].as<int>();


    LI c = LI(2, 1, 0, 0, 0.0);
 return 0;
}
