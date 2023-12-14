from __future__ import annotations
import argparse
import csv
import re
import subprocess
import sys
import time


DEFAULT_PARAMS = ((300, 10, 0, 0), (10, 300, 0, 0), (300, 300, 0, 0))
DEFAULT_MODEL_TYPES = ("LI", "HI", "HO", "HOmod")
DEFAULT_MAX_TIME = 1e10
DEFAULT_NUM_REPS = 30

PRELAUNCH_TASKS = {
    "adevs": [
        "cmake -S devstone/adevs/ -B devstone/adevs/build/ -D CMAKE_BUILD_TYPE=Release",
        "cmake --build devstone/adevs/build/ --target devstone",
    ],
    "cadmium": {
        "v1": [
            "cmake -S devstone/cadmium/ -B devstone/cadmium/build/ -D CMAKE_BUILD_TYPE=Release",
            "cmake --build devstone/cadmium/build/ --target devstone",
        ],
        "v2": {
            "sequential": [
                "cmake -S simulators/cadmium_v2/ -B simulators/cadmium_v2/build/ -D CMAKE_BUILD_TYPE=Release",
                "cmake --build simulators/cadmium_v2/build/ --target main_devstone",
            ],
            "parallel": [
                "cmake -S simulators/cadmium_v2/ -B simulators/cadmium_v2/build/ -D CMAKE_BUILD_TYPE=Release",
                "cmake --build simulators/cadmium_v2/build/ --target parallel_main_devstone",
            ],
        },
    },
    "xdevs": {
        "c": ["cd simulators/xdevs.c/ && make"],
        "cpp": ["cd simulators/xdevs.cpp/ && make"],
        "java": {
            "sequential": ["cd simulators/xdevs.java/ && mvn clean && mvn compile && mvn package"],
            "parallel": ["cd simulators/xdevs.java/ && mvn clean && mvn compile && mvn package"],
        },
        "rs": {
            "old": ["cargo build --manifest-path simulators/xdevs.rs/Cargo.toml --release"],
            "new": {
                "sequential": ["cargo build --manifest-path simulators/xdevs.rs-new/Cargo.toml --release  --example devstone"],
                "parallel": ["cargo build --manifest-path simulators/xdevs.rs-new/Cargo.toml --release --features par_all_no_couplings --example devstone"],
                "fullparallel": ["cargo build --manifest-path simulators/xdevs.rs-new/Cargo.toml --release --features par_all --example devstone"],
            }
        },
    },
}

COMMANDS = {
    "adevs": "devstone/adevs/bin/devstone {model_type} {width} {depth} {int_cycles} {ext_cycles}",
    "cadmium": {
        "v1": "devstone/cadmium/build/cadmium-dynamic-devstone --kind={model_type} --width={width} --depth={depth} --int-cycles={int_cycles} --ext-cycles={ext_cycles}",
        "v2": {
            "sequential": "simulators/cadmium_v2/bin/main_devstone {model_type} {width} {depth} {int_cycles} {ext_cycles}",
            "parallel": "simulators/cadmium_v2/bin/parallel_main_devstone {model_type} {width} {depth} {int_cycles} {ext_cycles}",
        },
    },
    "pypdevs": {
        "standard": {
            "python": "python3 devstone/pythonpdevs/main.py -m {model_type} -w {width} -d {depth} -i {int_cycles} -e {ext_cycles}",
            "pypy": "pypy3 devstone/pythonpdevs/main.py -m {model_type} -w {width} -d {depth} -i {int_cycles} -e {ext_cycles}",
        },
        "minimal": {
            "python": "python3 devstone/pythonpdevs-minimal/main.py -m {model_type} -d {depth} -w {width} -i {int_cycles} -e {ext_cycles}",
            "pypy": "pypy3 devstone/pythonpdevs-minimal/main.py -m {model_type} -d {depth} -w {width} -i {int_cycles} -e {ext_cycles}",
        },
    },
    "xdevs": {
        "c": "simulators/xdevs.c/examples/devstone/devstone -w {width} -d {depth} -b {model_type} -m 1",
        "cpp": "simulators/xdevs.cpp/src/xdevs/examples/DevStone/DevStone -w {width} -d {depth} -b {model_type} -m 1",
        # "go": ,  # TODO add this
        "java": {
            "sequential": "java -cp simulators/xdevs.java/target/xdevs-2.0.3-jar-with-dependencies.jar xdevs.lib.performance.DevStoneSimulation --model={model_type} --width={width} --depth={depth} --delay-distribution=Constant-{int_cycles} --coordinator=Coordinator",
            "parallel": "java -cp simulators/xdevs.java/target/xdevs-2.0.3-jar-with-dependencies.jar xdevs.lib.performance.DevStoneSimulation --model={model_type} --width={width} --depth={depth} --delay-distribution=Constant-{int_cycles} --coordinator=CoordinatorParallel",
        },
        "py": "python3 simulators/xdevs.py/xdevs/examples/devstone/devstone.py {model_type} {width} {depth} {int_cycles} {ext_cycles}",
        "rs": {
            "old": "simulators/xdevs.rs/target/release/xdevs {model_type} {width} {depth} {int_cycles} {ext_cycles}",
            "new": {
                "sequential": "simulators/xdevs.rs-new/target/release/examples/devstone {model_type} {width} {depth} {int_cycles} {ext_cycles}",
                "parallel": "simulators/xdevs.rs-new/target/release/examples/devstone {model_type} {width} {depth} {int_cycles} {ext_cycles}",
                "fullparallel": "simulators/xdevs.rs-new/target/release/examples/devstone {model_type} {width} {depth} {int_cycles} {ext_cycles}",
            }
        },
    },
}

DEFAULT_RE = r"model creation time(?:[ \(s\)]*): ?([0-9.e-]+)([ nuµms]*).*(?:engine|simulator) (?:set ?up|creation) time(?:[ \(s\)]*): ?([0-9.e-]+)([ nuµms]*).*simulation time(?:[ \(s\)]*): ?([0-9.e-]+)([ nuµms]*)"

CUSTOM_RE = {}


def serialize_simengines(res: list[str], prefix: str, flavors: str | dict):
    if isinstance(flavors, str):
        res.append(prefix)
    else:
        for key, val in flavors.items():
            new_prefix = f'{prefix}-{key}' if prefix else key
            serialize_simengines(res, new_prefix, val)


def parse_args():
    parser = argparse.ArgumentParser(description='Script to compare DEVStone implementations with different engines')
    parser.add_argument('-m', '--model-types', help='DEVStone model type (LI, HI, HO, HOmod)')
    parser.add_argument('-w', '--width', type=int, help='Width of each coupled model.')
    parser.add_argument('-d', '--depth', type=int, help='Number of recursive levels of the model.')
    parser.add_argument('-i', '--int-cycles', type=int, help='Dhrystone cycles executed in internal transitions')
    parser.add_argument('-e', '--ext-cycles', type=int, help='Dhrystone cycles executed in external transitions')
    parser.add_argument('-n', '--num-rep', type=int, help='Number of repetitions per each engine and configuration')
    parser.add_argument('-a', '--include_engines', help='Add specific engines to perform the comparative')
    parser.add_argument('-r', '--exclude_engines', help='Exclude specific engines from the comparative')
    parser.add_argument('-o', '--out-file', help='Output file path')
    parser.add_argument('-p', '--params', help='Specify params in a condensed form: w1-d1-ic1-ec1, w2-d2-ic2-ec2...')

    args = parser.parse_args()

    if args.model_types:
        args.model_types = [x.strip() for x in args.model_types.split(",")]
    else:
        args.model_types = list(DEFAULT_MODEL_TYPES)

    if args.params:
        params = [x.strip().split("-") for x in args.params.split(",")]
        # args.params = [x.strip().split("-") for x in args.params.split(",")]
    elif args.depth and args.width:
        int_cycles = args.int_cycles or 0
        ext_cycles = args.ext_cycles or 0
        params = [(args.width, args.depth, int_cycles, ext_cycles)]
        # args.params = ((args.depth, args.width, int_cycles, ext_cycles),)
    else:
        params = list(DEFAULT_PARAMS)
    args.params = []
    for param in params:
        if len(param) == 5:
            args.params.append(param)
        elif len(param) == 4:
            for model in args.model_types:
                args.params.append((model, *param))
        else:
            raise ValueError(f'invalid number of params ({param})')

    engines = args.include_engines.split(',') if args.include_engines else list(COMMANDS.keys())
    args.include_engines = []
    for eng in engines:
        commands = COMMANDS
        for s in eng.split('-'):
            commands = commands.get(s, dict())
        subengines = []
        serialize_simengines(subengines, '', commands)
        if not subengines:
            raise RuntimeError(f'unknown simulation engine {eng}')
        subengines = [f'{eng}-{x}' if x else f'{eng}' for x in subengines]
        args.include_engines.extend(subengines)

    if args.exclude_engines:
        args.include_engines = [x for x in args.include_engines if x not in args.exclude_engines.split(",")]

    args.prelaunch = {}
    args.commands = {}
    args.regex = {}
    for eng in args.include_engines:
        prelaunch = PRELAUNCH_TASKS
        cmd = COMMANDS
        regex = CUSTOM_RE
        for s in eng.split('-'):
            if isinstance(prelaunch, dict):
                prelaunch = prelaunch.get(s, dict())
            cmd = cmd.get(s, dict())
            if not isinstance(regex, str):
                regex = regex.get(s, DEFAULT_RE)
        if isinstance(prelaunch, dict):
            prelaunch = None
        if not cmd:
            raise RuntimeError(f'unknown simulation engine {eng}')
        if isinstance(cmd, dict):
            # TODO instead of this, serialize all the flavors of the engine
            raise RuntimeError(f'There are more than one flavor for the engine {eng}')
        else:
            args.prelaunch[eng] = prelaunch
            args.commands[eng] = cmd
            args.regex[eng] = regex

    if not args.num_rep:
        args.num_rep = DEFAULT_NUM_REPS

    if not args.out_file:
        args.out_file = "devstone_%de_%dp_%d.csv" % (len(args.include_engines), len(args.params), int(time.time()))

    return args


def execute_prelaunch(cmd):
    try:
        result = subprocess.run(cmd.split(), stdout=subprocess.PIPE)
    except Exception as e:
        print(f"Error executing prelaunch command. ({str(e)})")
        return
    print(result.stdout)


def execute_cmd(cmd, regex, csv_writer):
    # Execute simulation
    try:
        result = subprocess.run(cmd.split(), stdout=subprocess.PIPE)
    except Exception as e:
        print(f"{engine}: Error executing simulation. ({str(e)})")
        return

    # Read data from output
    res_out = result.stdout.decode('UTF-8').strip()
    if not res_out:
        res_out = str(result.stderr).strip()
    res_out.replace("\n", " ")
    found = re.search(regex, res_out, flags=re.IGNORECASE | re.DOTALL)
    if not found:
        print(f"{engine}: Simulation execution times could not be extracted.")
        print(result.stdout)
        return
    groups = found.groups()
    if len(groups) == 3:
        model_time, engine_time, sim_time = tuple(map(float, found.groups()))
    elif len(groups) == 6:
        times = list(map(float, (groups[0], groups[2], groups[4])))
        units = (groups[1], groups[3], groups[5])
        for i in range(3):
            if units[i]:
                unit = units[i].strip()
                if unit.startswith('s'):
                    pass
                elif unit.startswith('ms'):
                    times[i] *= 1e-3
                elif unit.startswith('us') or unit.startswith('µs'):
                    times[i] *= 1e-6
                elif unit.startswith('ns'):
                    times[i] *= 1e-9
                else:
                    raise Exception(f"unknown time units: {unit}")
        times = tuple(times)
        model_time, engine_time, sim_time = times
    total_time = sum((model_time, engine_time, sim_time))

    # Write results into output file
    row = (engine, i_exec, model_type, width, depth, int_cycles, ext_cycles, model_time, engine_time, sim_time, total_time)
    csv_writer.writerow(row)


if __name__ == "__main__":
    sys.setrecursionlimit(10000)

    args = parse_args()

    if not args.include_engines:
        raise RuntimeError("No engines were selected.")

    with open(args.out_file, "w") as csv_file:
        csv_writer = csv.writer(csv_file, delimiter=';')
        csv_writer.writerow(("engine", "iter", "model", "width", "depth", "int_delay", "ext_delay", "model_time", "runner_time", "sim_time", "total_time"))

        for engine, engine_cmd in args.commands.items():
            engine_prelaunch = args.prelaunch[engine]
            if engine_prelaunch is not None:
                print(f'({engine}) executing prelaunch tasks...')
                for prelaunch_cmd in engine_prelaunch:
                    execute_prelaunch(prelaunch_cmd)
            engine_regex = args.regex[engine]
            for params in args.params:
                model_type, width, depth, int_cycles, ext_cycles = params
                engine_cmd_f = engine_cmd.format(model_type=model_type, width=width, depth=depth,
                                                 int_cycles=int_cycles, ext_cycles=ext_cycles)
                if not engine_cmd_f:
                    continue
                for i_exec in range(args.num_rep):
                    print(f'({i_exec}) {engine_cmd_f}')
                    execute_cmd(engine_cmd_f, engine_regex, csv_writer)
                csv_file.flush()
