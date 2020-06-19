import argparse
import csv
import re
import subprocess
import sys
import time
import os

sys.setrecursionlimit(10000)

threads = os.cpu_count()

XDEVS_CPP_CMD = "simulators/xdevs-c++/src/xdevs/examples/DevStone/DevStone -w {width} -d {depth} -b {model_type} -m 1"
XDEVS_PYTHON_CMD = "python3 simulators/xdevs-python/perfdevs/examples/devstone/main.py -m {model_type} -d {depth} -w {width} -i {int_cycles} -e {ext_cycles}"
XDEVS_PYTHON_F_CMD = "python3 simulators/xdevs-python/perfdevs/examples/devstone/main.py -m {model_type} -d {depth} -w {width} -i {int_cycles} -e {ext_cycles} -f"
XDEVS_PYTHON_C_CMD = "python3 simulators/xdevs-python/perfdevs/examples/devstone/main.py -m {model_type} -d {depth} -w {width} -i {int_cycles} -e {ext_cycles} -c"
XDEVS_PYTHON_FC_CMD = "python3 simulators/xdevs-python/perfdevs/examples/devstone/main.py -m {model_type} -d {depth} -w {width} -i {int_cycles} -e {ext_cycles} -f -c"
XDEVS_JAVA_CMD = "java -classpath simulators/xdevs-java/out/ xdevs.core.devstone.DEVStone {model_type} {depth} {width} {int_cycles} {ext_cycles} coord false"
XDEVS_JAVA_CHAINED_CMD = "java -classpath simulators/xdevs-java/out/ xdevs.core.devstone.DEVStone {model_type} {depth} {width} {int_cycles} {ext_cycles} chained false"
XDEVS_JAVA_PARALLEL_CMD = "java -classpath simulators/xdevs-java/out/ xdevs.core.devstone.DEVStone {model_type} {depth} {width} {int_cycles} {ext_cycles} parallel false"
XDEVS_JAVA_PARALLEL_CHAINED_CMD = "java -classpath simulators/xdevs-java/out/ xdevs.core.devstone.DEVStone {model_type} {depth} {width} {int_cycles} {ext_cycles} chainedparallel false"
PYPDEVS_CMD = "python3 devstone/pythonpdevs/main.py -m {model_type} -d {depth} -w {width} -i {int_cycles} -e {ext_cycles}"
PYPDEVS_MIN_CMD = "python3 devstone/pythonpdevs-minimal/main.py -m {model_type} -d {depth} -w {width} -i {int_cycles} -e {ext_cycles}"
PYPDEVS_PYPY_CMD = "pypy3 devstone/pythonpdevs/main.py -m {model_type} -d {depth} -w {width} -i {int_cycles} -e {ext_cycles}"
PYPDEVS_PYPY_MIN_CMD = "pypy3 devstone/pythonpdevs-minimal/main.py -m {model_type} -d {depth} -w {width} -i {int_cycles} -e {ext_cycles}"
CADMIUM_CMD = "devstone/cadmium/build/cadmium-dynamic-devstone --kind={model_type} --depth={depth} --width={width} --int-cycles={int_cycles} --ext-cycles={ext_cycles}"
CADMIUM_CONC_CMD = "devstone/cadmium/build/cadmium-dynamic-conc-devstone --kind={model_type} --depth={depth} --width={width} --int-cycles={int_cycles} --ext-cycles={ext_cycles} --threads=" + str(threads)
CDBOOST_CMD = "devstone/cdboost/build/cdboost-devstone --kind={model_type} --depth={depth} --width={width} --int-cycles={int_cycles} --ext-cycles={ext_cycles} --event-list=events_devstone.txt"
ADEVS_CMD = "devstone/adevs/build/DEVStone --kind={model_type} --depth={depth} --width={width} --int-cycles={int_cycles} --ext-cycles={ext_cycles}"

DEFAULT_PARAMS = ((300, 10, 0, 0), (10, 300, 0, 0), (300, 300, 0, 0))
DEFAULT_MODEL_TYPES = ("LI", "HI", "HO", "HOmod")
DEFAULT_MAX_TIME = 1e10
DEFAULT_NUM_REPS = 10

RE_SIM_TIMES = r"Model creation time: ?([0-9.e-]+) ?.*Engine set ?up time: ?([0-9.e-]+) ?.*Simulation time: ?([0-9.e-]+)"


engines = {"xdevs-c++": XDEVS_CPP_CMD,
           "xdevs-python": XDEVS_PYTHON_CMD,
           "xdevs-python-f": XDEVS_PYTHON_F_CMD,
           "xdevs-python-c": XDEVS_PYTHON_C_CMD,
           "xdevs-python-fc": XDEVS_PYTHON_FC_CMD,
           "xdevs-java": XDEVS_JAVA_CMD,
           "xdevs-java-chained": XDEVS_JAVA_CHAINED_CMD,
           "xdevs-java-parallel": XDEVS_JAVA_PARALLEL_CMD,
           "xdevs-java-parallel-chained": XDEVS_JAVA_PARALLEL_CHAINED_CMD,
           "pypdevs": PYPDEVS_CMD,
           "pypdevs-min": PYPDEVS_MIN_CMD,
           "pypdevs-pypy": PYPDEVS_CMD,
           "pypdevs-pypy-min": PYPDEVS_MIN_CMD,
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


def execute_cmd(cmd, csv_writer):
    # Execute simulation
    try:
        result = subprocess.run(engine_cmd_f.split(), stdout=subprocess.PIPE)
    except Exception as e:
        print("%s: Error executing simulation." % engine)
        return

    # Read data from output
    found = re.search(RE_SIM_TIMES, str(result.stdout))

    if not found:
        print("%s: Simulation execution times could not be extracted." % engine)
        print(result.stdout)
        return

    model_time, engine_time, sim_time = tuple(map(float, found.groups()))
    total_time = sum((model_time, engine_time, sim_time))

    # Write results into output file
    row = (
    engine, i_exec, model_type, depth, width, int_cycles, ext_cycles, model_time, engine_time, sim_time, total_time)
    csv_writer.writerow(row)


with open(args.out_file, "w") as csv_file:
    csv_writer = csv.writer(csv_file, delimiter=';')
    csv_writer.writerow(("engine", "iter", "model", "depth", "width", "int_delay", "ext_delay", "model_time", "runner_time", "sim_time", "total_time"))

    for engine in args.include_engines:
        engine_cmd = engines[engine]

        if len(args.params[0]) == 4:
                for model_type in args.model_types:
                    for depth, width, int_cycles, ext_cycles in args.params:
                        engine_cmd_f = engine_cmd.format(model_type=model_type, depth=depth, width=width, int_cycles=int_cycles, ext_cycles=ext_cycles)
                        for i_exec in range(args.num_rep):

                            if not engine_cmd_f:
                                continue

                            print(engine_cmd_f)
                            execute_cmd(engine_cmd_f, csv_writer)

        elif len(args.params[0]) == 5:
            for model_type, depth, width, int_cycles, ext_cycles in args.params:
                engine_cmd_f = engine_cmd.format(model_type=model_type, depth=depth, width=width, int_cycles=int_cycles,
                                                 ext_cycles=ext_cycles)
                for i_exec in range(args.num_rep):

                    if not engine_cmd_f:
                        continue

                    print(engine_cmd_f)
                    execute_cmd(engine_cmd_f, csv_writer)
