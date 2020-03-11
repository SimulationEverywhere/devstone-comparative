import argparse
import csv
import re
import subprocess
import sys
import time

sys.setrecursionlimit(10000)

XDEVS_PYTHON_CMD = ""
XDEVS_JAVA_CMD = "java -classpath simulators/xdevs-java/out/production/xdevs xdevs.core.devstone.DEVStone {model_type} {depth} {width} {int_cycles} {ext_cycles} chained false"
XDEVS_JAVA_CHAINED_CMD = "java -classpath simulators/xdevs-java/out/production/xdevs xdevs.core.devstone.DEVStone {model_type} {depth} {width} {int_cycles} {ext_cycles} coord false"
PYPDEVS_CMD = ""
PYPDEVS_MIN_CMD = ""
CADMIUM_CMD = ""
CADMIUM_CONC_CMD = ""
CDBOOST_CMD = ""
ADEVS_CMD = ""

DEFAULT_PARAMS = ((300, 10, 0, 0), (10, 300, 0, 0), (300, 300, 0, 0))
DEFAULT_MODEL_TYPES = ("LI", "HI", "HO", "HOmod")
DEFAULT_MAX_TIME = 1e10
DEFAULT_NUM_REPS = 10

RE_SIM_TIMES = r"Model creation time: ?([0-9.e-]+) ?.*Engine set ?up time: ?([0-9.e-]+) ?.*Simulation time: ?([0-9.e-]+)"


engines = {"xdevs-python": XDEVS_PYTHON_CMD,
           "xdevs-java": XDEVS_JAVA_CMD,
           "xdevs-java-chained": XDEVS_JAVA_CHAINED_CMD,
           "pypdevs": PYPDEVS_CMD,
           "pypdevs-min": PYPDEVS_MIN_CMD,
           "cadmium": CADMIUM_CMD,
           "cadmium-conc": CADMIUM_CONC_CMD,
           "cdboost": CDBOOST_CMD,
           "adevs": ADEVS_CMD
           }


def parse_args():
    parser = argparse.ArgumentParser(description='Script to compare DEVStone implementations with different engines')

    parser.add_argument('-m', '--model-types', help='DEVStone model type (LI, HI, HO, HOmod)')
    parser.add_argument('-d', '--depth', type=int, help='Number of recursive levels of the model.')
    parser.add_argument('-w', '--width', type=int, help='Width of each coupled model.')
    parser.add_argument('-i', '--int-cycles', type=int, help='Dhrystone cycles executed in internal transitions')
    parser.add_argument('-e', '--ext-cycles', type=int, help='Dhrystone cycles executed in external transitions')
    parser.add_argument('-n', '--num-rep', type=int, help='Number of repetitions per each engine and configuration')
    parser.add_argument('-a', '--include_engines', help='Add specific engines to perform the comparative')
    parser.add_argument('-r', '--exclude_engines', help='Exclude specific engines from the comparative')
    parser.add_argument('-o', '--out-file', help='Output file path')
    parser.add_argument('-p', '--params', help='Specify params in a condensed form: d1-w1-ic1-ec1, d2-w2-ic2-ec2...')

    args = parser.parse_args()

    if args.model_types:
        args.model_types = [x.strip() for x in args.model_types.split(",")]
    else:
        args.model_types = DEFAULT_MODEL_TYPES

    if args.params:
        args.params = [x.strip().split("-") for x in args.params.split(",")]
    elif args.depth and args.width:
        int_cycles = args.int_cycles or 0
        ext_cycles = args.ext_cycles or 0
        args.params = ((args.depth, args.width, int_cycles, ext_cycles),)

    else:
        args.params = DEFAULT_PARAMS

    if args.include_engines:
        args.include_engines = args.include_engines.split(",")
        for engine in args.include_engines:
            if engine not in engines:
                raise RuntimeError("%s is not in the engines list" % engine)
    else:
        args.include_engines = engines.keys()

    if args.exclude_engines:
        excluded = args.exclude_engines.split(",")
        args.include_engines = [x for x in args.include_engines if x not in excluded]

    if not args.num_rep:
        args.num_rep = DEFAULT_NUM_REPS

    if not args.out_file:
        args.out_file = "devstone_%de_%dp_%d.csv" % (len(engines), len(args.params), int(time.time()))

    return args


args = parse_args()

if not engines:
    raise RuntimeError("No engines were selected.")

with open(args.out_file, "w") as csv_file:
    csv_writer = csv.writer(csv_file, delimiter=';')
    csv_writer.writerow(("engine", "iter", "model", "depth", "width", "model_time", "runner_time", "sim_time", "total_time"))

    for engine in args.include_engines:
        engine_cmd = engines[engine]

        for model_type in args.model_types:
            for depth, width, int_cycles, ext_cycles in args.params:
                engine_cmd_f = engine_cmd.format(model_type=model_type, depth=depth, width=width, int_cycles=int_cycles, ext_cycles=ext_cycles)
                for i_exec in range(args.num_rep):
                    print(engine_cmd_f)

                    # Open subprocess
                    result = subprocess.run(engine_cmd_f.split(), stdout=subprocess.PIPE)

                    # Read data from output
                    found = re.search(RE_SIM_TIMES, str(result.stdout))

                    if not found:
                        print(result.stdout)
                        raise RuntimeError("Simulation execution times could not be extracted.")

                    model_time, engine_time, sim_time = tuple(map(float, found.groups()))
                    total_time = sum((model_time, engine_time, sim_time))

                    # Write results into output file
                    row = (engine, i_exec, model_type, depth, width, int_cycles, ext_cycles, model_time, engine_time, sim_time, total_time)
                    csv_writer.writerow(row)
