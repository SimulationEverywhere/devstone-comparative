/**
 * Copyright (c) 2013-2014, Carleton University, Universite de Nice-Sophia Antipolis
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <iostream>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <boost/program_options.hpp>
#include <boost/simulation.hpp>
#include "cdboost-devstone-atomic.hpp"
#include "cdboost-devstone-atomic-pair.hpp"
#include "cdboost-devstone-filter.hpp"
#include "cdboost-devstone-input-stream-pair.hpp"

using namespace std;
using namespace cdpp;
namespace po=boost::program_options;
using hclock=chrono::high_resolution_clock;
using Time=double;
//const float infinity = std::numeric_limits<double>::infinity();
inline bool is_infinity(double& f ){ return isinf(f); }
using msg_type_int=int;
using msg_type_pair=std::pair<int, int>;

shared_ptr<boost::simulation::pdevs::coupled<Time, msg_type_int>> LI_coupling(int& counted_atomic_models, int& counted_coupled_models,
                           int width, int depth, string event_list, int ext_cycles, int int_cycles, int time_advance)
{
    shared_ptr<boost::simulation::pdevs::coupled<Time, msg_type_int>> cm;
    auto first_pdevstone = boost::simulation::make_atomic_ptr<PDEVStoneAtomic<Time, msg_type_int>, int, int, Time>(int_cycles, ext_cycles, Time(time_advance));
    counted_atomic_models++;
    cm.reset(new boost::simulation::pdevs::coupled<Time, msg_type_int>{{first_pdevstone}, {first_pdevstone}, {}, {first_pdevstone}});
    counted_coupled_models++;

    //connect higher level models

    for (int i=1; i < depth; i++){
        vector<std::shared_ptr<boost::simulation::model<Time>>> vpdt;
        vector<std::shared_ptr<boost::simulation::model<Time>>> eoc_cm;
        vector<std::shared_ptr<boost::simulation::model<Time>>> eic_cm;
        vpdt.clear();
        eoc_cm.clear();
        eic_cm.clear();
        eoc_cm.push_back(cm);
        for (int j=0; j < width-1; j++){
            auto current = (std::shared_ptr<boost::simulation::model<Time>>) std::make_shared<cdpp::PDEVStoneAtomic<Time, msg_type_int>>(int_cycles, ext_cycles, Time(time_advance));
            vpdt.push_back(current);
            counted_atomic_models++;
            eic_cm.push_back(current);
        }
        vpdt.push_back(cm);
        eic_cm.push_back(cm);

        shared_ptr<boost::simulation::pdevs::coupled<Time, msg_type_int>> cm_int ( new boost::simulation::pdevs::coupled<Time, msg_type_int>{vpdt, eic_cm, {}, eoc_cm});
        counted_coupled_models++;
        cm=cm_int;
    }

    //Plug the input events
    shared_ptr<istream> piss{ new ifstream{event_list} };
    auto pf = boost::simulation::make_atomic_ptr<boost::simulation::pdevs::basic_models::input_stream<Time, msg_type_int, int, int>, shared_ptr<istream>, Time>(piss, Time{0});
    counted_atomic_models++;

    auto root = std::make_shared<boost::simulation::pdevs::coupled<Time, msg_type_int>>(boost::simulation::pdevs::coupled<Time, msg_type_int>({pf, cm}, {}, {{pf, cm}}, {cm}));
    counted_coupled_models++;
    return root;
}

shared_ptr<boost::simulation::pdevs::coupled<Time, msg_type_int>> HI_coupling(int& counted_atomic_models, int& counted_coupled_models,
                           int width, int depth, string event_list, int ext_cycles, int int_cycles, int time_advance)
{
    shared_ptr<boost::simulation::pdevs::coupled<Time, msg_type_int>> cm;
    auto first_pdevstone = boost::simulation::make_atomic_ptr<PDEVStoneAtomic<Time, msg_type_int>, int, int, Time>(int_cycles, ext_cycles, Time(time_advance));
    counted_atomic_models++;
    cm.reset(new boost::simulation::pdevs::coupled<Time, msg_type_int>{{first_pdevstone}, {first_pdevstone}, {}, {first_pdevstone}});
    counted_coupled_models++;

    //connect higher level models

    for (int i=1; i < depth; i++){
        vector<std::shared_ptr<boost::simulation::model<Time>>> vpdt;
        vector<std::shared_ptr<boost::simulation::model<Time>>> eoc_cm;
        vector<std::shared_ptr<boost::simulation::model<Time>>> eic_cm;
        vector<pair<std::shared_ptr<boost::simulation::model<Time>>, std::shared_ptr<boost::simulation::model<Time>>>> ic_cm;
        vpdt.clear();
        eoc_cm.clear();
        eic_cm.clear();
        ic_cm.clear();
        eoc_cm.push_back(cm);
        for (int j=0; j < width-1; j++){
            std::shared_ptr<boost::simulation::model<Time>> current = std::shared_ptr<boost::simulation::model<Time>>(make_shared<PDEVStoneAtomic<Time, msg_type_int>>(int_cycles, ext_cycles, Time(time_advance)));
            if (j > 0) ic_cm.push_back({vpdt.back(), current});
            vpdt.push_back(current);
            counted_atomic_models++;
            eic_cm.push_back(current);
        }
        vpdt.push_back(cm);
        eic_cm.push_back(cm);

        shared_ptr<boost::simulation::pdevs::coupled<Time, msg_type_int>> cm_int ( new boost::simulation::pdevs::coupled<Time, msg_type_int>{vpdt, eic_cm, ic_cm, eoc_cm});
        counted_coupled_models++;
        cm=cm_int;
    }

    //Plug the input events
    shared_ptr<istream> piss{ new ifstream{event_list} };
    auto pf = boost::simulation::make_atomic_ptr<boost::simulation::pdevs::basic_models::input_stream<Time, msg_type_int, int, int>, shared_ptr<istream>, Time>(piss, Time{0});

    counted_atomic_models++;

    auto root = std::make_shared<boost::simulation::pdevs::coupled<Time, msg_type_int>>(boost::simulation::pdevs::coupled<Time, msg_type_int>({pf, cm}, {}, {{pf, cm}}, {cm}));
    counted_coupled_models++;
    return root;
}


shared_ptr<boost::simulation::pdevs::coupled<Time, msg_type_pair>> HO_coupling(int& counted_atomic_models, int& counted_coupled_models,
                                                                          int width, int depth, string event_list, int ext_cycles, int int_cycles, int time_advance)
{
    shared_ptr<boost::simulation::pdevs::coupled<Time, msg_type_pair>> cm;
    auto first_pdevstone = boost::simulation::make_atomic_ptr<PDEVStoneAtomicPair<Time, msg_type_pair>, int, int, Time>(int_cycles, ext_cycles, Time(time_advance));
    counted_atomic_models++;
    cm.reset(new boost::simulation::pdevs::coupled<Time, msg_type_pair>{{first_pdevstone}, {first_pdevstone}, {}, {first_pdevstone}});
    counted_coupled_models++;

    //connect higher level models

    for (int i=1; i < depth; i++){
        auto filter0 = boost::simulation::make_atomic_ptr<PDEVStoneFilter<Time, msg_type_pair>, int, Time>(0, Time(time_advance));   // in
        auto filter1 = boost::simulation::make_atomic_ptr<PDEVStoneFilter<Time, msg_type_pair>, int, Time>(1, Time(time_advance));   // in2
        counted_atomic_models+=2;
        vector<std::shared_ptr<boost::simulation::model<Time>>> vpdt;
        vector<std::shared_ptr<boost::simulation::model<Time>>> eoc_cm;
        vector<std::shared_ptr<boost::simulation::model<Time>>> eic_cm;
        vector<pair<std::shared_ptr<boost::simulation::model<Time>>, std::shared_ptr<boost::simulation::model<Time>>>> ic_cm;
        vpdt.clear();
        eoc_cm.clear();
        eic_cm.clear();
        ic_cm.clear();
        eoc_cm.push_back(cm);
        eic_cm.push_back(filter0);
        eic_cm.push_back(filter1);
        for (int j=0; j < width-1; j++){
            std::shared_ptr<boost::simulation::model<Time>> current = std::shared_ptr<boost::simulation::model<Time>>(make_shared<PDEVStoneAtomicPair<Time, msg_type_pair>>(int_cycles, ext_cycles, Time(time_advance)));
            if (j > 0) ic_cm.push_back({vpdt.back(), current});
            vpdt.push_back(current);
            counted_atomic_models++;
            //eic_cm.push_back(current);
            ic_cm.push_back({filter1, current});
            eoc_cm.push_back(current);
        }
        vpdt.push_back(cm);
        vpdt.push_back(filter0);
        vpdt.push_back(filter1);
        //eic_cm.push_back(cm);
        ic_cm.push_back({filter0, cm});

        shared_ptr<boost::simulation::pdevs::coupled<Time, msg_type_pair>> cm_int ( new boost::simulation::pdevs::coupled<Time, msg_type_pair>{vpdt, eic_cm, ic_cm, eoc_cm});
        counted_coupled_models++;
        cm=cm_int;
    }

    //Plug the input events
    auto gen = boost::simulation::make_atomic_ptr<devstone_input_stream_pair<Time, msg_type_pair>, Time>(Time(time_advance));
    counted_atomic_models++;

    auto root = std::make_shared<boost::simulation::pdevs::coupled<Time, msg_type_pair>>(boost::simulation::pdevs::coupled<Time, msg_type_pair>({gen, cm}, {}, {{gen, cm}}, {cm}));
    counted_coupled_models++;
    return root;
}




int main(int argc, char* argv[]){
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
            ("event-list", po::value<string>()->required(), "set the file to read the events. The format is 2 ints per line meaning time->msg")
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
    if (kind.compare("LI") != 0  && kind.compare("HI") != 0 && kind.compare("HO") != 0) {
        cout << "The kind needs to be LI, HI or HO and received value was: " << kind;
        cout << endl;
        cout << "for mode information run: " << argv[0] << " --help" << endl;
        return 1;
    }

    {
        std::ifstream f(vm["event-list"].as<string>().c_str());
        if(!f.is_open()){
            cout << "File for events: " << vm["event-list"].as<string>() << " is not accesible." << endl;
            cout << endl;
            cout << "for mode information run: " << argv[0] << " --help" << endl;
            return 1;
        }
    }

    int width = vm["width"].as<int>();
    int depth = vm["depth"].as<int>();
    int int_cycles = vm["int-cycles"].as<int>();
    int ext_cycles = vm["ext-cycles"].as<int>();
    int time_advance = vm["time-advance"].as<int>();
    string event_list = vm["event-list"].as<string>();
    //finished processing input

    //create models for LI kind
    int models_quantity = (width - 1) * (depth - 1) + 1;
    int counted_atomic_models=0;
    int counted_coupled_models=0;

    auto processed_parameters = hclock::now();

    shared_ptr<boost::simulation::pdevs::coupled<Time, msg_type_int>> root_int;
    shared_ptr<boost::simulation::pdevs::coupled<Time, msg_type_pair>> root_pair;
    if (kind.compare("LI") == 0){
        root_int = LI_coupling(counted_atomic_models, counted_coupled_models, width, depth, event_list, ext_cycles, int_cycles, time_advance);
    } else if (kind.compare("HI") == 0){
        root_int = HI_coupling(counted_atomic_models, counted_coupled_models, width, depth, event_list, ext_cycles, int_cycles, time_advance);
    } else if (kind.compare("HO") == 0){
        root_pair = HO_coupling(counted_atomic_models, counted_coupled_models, width, depth, event_list, ext_cycles, int_cycles, time_advance);
    } else {
        abort();
    }

    auto model_built = hclock::now();
    std::chrono::_V2::system_clock::time_point model_init;
    //run the model
    if (kind.compare("LI") == 0 || kind.compare("HI") == 0) {
        boost::simulation::pdevs::runner<Time, msg_type_int> r(root_int, Time{0});
        model_init = hclock::now();
        r.runUntilPassivate();
    } else {  // HO or HOmod
        boost::simulation::pdevs::runner<Time, msg_type_pair> r(root_pair, Time{0});
        model_init = hclock::now();
        r.runUntilPassivate();
    }

    auto finished_simulation = hclock::now();

    cout << "Simulation with params: ";

    for (const auto& it : vm) {
        cout << it.first.c_str() << ": ";
        auto& value = it.second.value();
        if (auto v = boost::any_cast<int>(&value))
            std::cout << *v;
        else if (auto v = boost::any_cast<std::string>(&value))
            std::cout << *v;
        else
            std::cout << "error";
        cout << " ";
    }


    cout << endl;
    cout << "Model creation time: " << chrono::duration_cast<chrono::duration<double, ratio<1>>>( model_built - processed_parameters).count() << endl;
    cout << "Engine setup time: " << chrono::duration_cast<chrono::duration<double, ratio<1>>>( model_init - model_built).count() << endl;
    cout << "Simulation time: " << chrono::duration_cast<chrono::duration<double, ratio<1>>>( finished_simulation - model_init).count() << endl;
}
