import argparse
import csv
import sys
import time

sys.setrecursionlimit(10000)

XDEVS_PYTHON_CMD = ""
XDEVS_JAVA_CMD = "java -classpath simulators/xdevs-java/out/production/xdevs xdevs.core.devstone.DEVStone {model_type} {depth} {width} {int_delay} {ext_delay} chained false"
XDEVS_JAVA_CHAINED_CMD = "java -classpath simulators/xdevs-java/out/production/xdevs xdevs.core.devstone.DEVStone {model_type} {depth} {width} {int_delay} {ext_delay} coord false"
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
        args.include_engines = [x.split() for x in args.include_engines.split(",")]
    else:
        args.include_engines = engines.keys()

    if args.exclude_engines:
        excluded = [x.split() for x in args.exclude_engines.split(",")]
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
        for model_type in args.model_types:
            for depth, width, int_delay, ext_delay in args.params:
                for i_exec in range(args.num_rep):
                    print(engine, i_exec, model_type, depth, width)
                    # Open subprocess
                    # Read output
                    # Extract data
                    # Write results into output file
                    # csv_writer.writerow(curr_row)
