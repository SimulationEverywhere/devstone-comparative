# DEVStone Comparative

Here you can find several DEVStone implementations for comparing the main
DEVS-compliant simulation engines.

So far, we support:
* [adevs](https://web.ornl.gov/~nutarojj/adevs/)
* [Cadmium](https://github.com/SimulationEverywhere/cadmium)
* [PyPDEVS](http://msdl.uantwerpen.be/git/yentl/PythonPDEVS.git) (regular and minimal versions).
* [xDEVS](https://github.com/iscar-ucm/xdevs) (C++, Java, and Python versions).

## Requirements

We rely on third-party software to run our tool. We enumerate these
dependencies:

* GNU Bash terminal
* GNU C Compiler (compatible with C++ 17), Git, Make, and CMake.
* The Boost library for C++ simulators.
* Java 11 for xDEVS (Java version).
* Python 3.7+ for PyPDEVS, xDEVS (Python version), and for running the main script.

## Getting started

1. Open a Bash terminal
2. Clone this repository and move to the main folder:
`git clone https://github.com/SimulationEverywhere/devstone-comparative.git; cd devstone-comparative`
3. Run the setup script: `./setup.sh`. All the different DEVStone applications will be compiled.
4. Use the `devstone_comparative.py` script for executing any DEVStone model you want.

```
usage: devstone_comparative.py [-h] [-m MODEL_TYPES] [-d DEPTH] [-w WIDTH]
                               [-i INT_CYCLES] [-e EXT_CYCLES] [-n NUM_REP]
                               [-a INCLUDE_ENGINES] [-r EXCLUDE_ENGINES]
                               [-o OUT_FILE] [-p PARAMS]

Script to compare DEVStone implementations with different engines

optional arguments:
  -h, --help            show this help message and exit
  -m MODEL_TYPES, --model-types MODEL_TYPES
                        DEVStone model type (LI, HI, HO, HOmod)
  -d DEPTH, --depth DEPTH
                        Number of recursive levels of the model.
  -w WIDTH, --width WIDTH
                        Width of each coupled model.
  -i INT_CYCLES, --int-cycles INT_CYCLES
                        Dhrystone cycles executed in internal transitions
  -e EXT_CYCLES, --ext-cycles EXT_CYCLES
                        Dhrystone cycles executed in external transitions
  -n NUM_REP, --num-rep NUM_REP
                        Number of repetitions per each engine and
                        configuration
  -a INCLUDE_ENGINES, --include_engines INCLUDE_ENGINES
                        Add specific engines to perform the comparative
  -r EXCLUDE_ENGINES, --exclude_engines EXCLUDE_ENGINES
                        Exclude specific engines from the comparative
  -o OUT_FILE, --out-file OUT_FILE
                        Output file path
  -p PARAMS, --params PARAMS
                        Specify params in a condensed form: d1-w1-ic1-ec1,
                        d2-w2-ic2-ec2...
```

For instance, if I want to execute HO models with width 10 and depth 5 for
10 times using xDEVS Python and Cadmium, I can use the following command:

```
python3 devstone_comparative.py -m HO -w 10 -d 5 -n 10 -a xdevs-python -a cadmium
```

So far, we support the following simulation engines:
* adevs (in the script, use the keyword `adevs`)
* Cadmium (in the script, use the keyword `cadmium`)
* PyPDEVS (in the script, use the keyword `pypdevs`)
* PyPDEVS minimal (in the script, use the keyword `pypdevs-min`)
* xDEVS C++ (in the script, use the keyword `xdevs-c++`)
* xDEVS Python (in the script, use the keyword `xdevs-python`)
* xDEVS Python with model flattening (in the script, use the keyword `xdevs-python-f`)
* xDEVS Python with chained algorithm (in the script, use the keyword `xdevs-python-c`)
* xDEVS Python with model flattening and chained algorithm (in the script, use the keyword `xdevs-python-fc`)
* xDEVS Java (in the script, use the keyword `xdevs-java`)
* xDEVS Java with parallel execution (in the script, use the keyword `xdevs-java-parallel`)

The results will be available in a new CSV file located at your current working directory.

# Contributing and reporting errors
If you want to contribute or you find any error, please contact us via this [GitHub repository](https://github.com/SimulationEverywhere/devstone-comparative).
