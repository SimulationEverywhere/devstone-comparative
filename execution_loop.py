import csv
import sys
import time

sys.setrecursionlimit(10000)

XDEVS_PYTHON_CMD = ""
XDEVS_JAVA_CMD = ""
PYPDEVS_CMD = ""
PYPDEVS_MIN_CMD = ""
CADMIUM_CMD = ""
CADMIUM_CONC_CMD = ""
CDBOOST_CMD = ""
ADEVS_CMD = ""
MS4ME_CMD = ""

sim_max_time = 1e10
int_delay = 0
ext_delay = 0
flatten = False
num_execs = int(sys.argv[1]) if len(sys.argv) > 1 else 10

engines = {"xdevs-python": XDEVS_PYTHON_CMD,
           "xdevs-java" : XDEVS_JAVA_CMD,
           "pypdevs": PYPDEVS_CMD,
           "pypdevs-min": PYPDEVS_MIN_CMD,
           "cadmium": CADMIUM_CMD,
           "cadmium-conc": CADMIUM_CONC_CMD,
           "cdboost": CDBOOST_CMD,
           "adevs": ADEVS_CMD,
           "ms4me": MS4ME_CMD
           }

depths_widths = ((300, 10), (10, 300), (300, 300))
model_types = ("LI", "HI", "HO", "HOmod")

filename = "devstone_%dc_%di_%de_%d.csv" % (len(depths_widths), int_delay, ext_delay, int(time.time()))

with open(filename, "w") as csv_file:
    csv_writer = csv.writer(csv_file, delimiter=';')
    csv_writer.writerow(("engine", "iter", "model", "depth", "width", "model_time", "runner_time", "sim_time", "total_time"))

    for engine in engines:
        for model_type in model_types:
            for depth, width in depths_widths:
                for i_exec in range(num_execs):
                    print(engine, i_exec, model_type, depth, width)
                    # Open subprocess
                    # Read output
                    # Extract data
                    # Write results into output file
                    # csv_writer.writerow(curr_row)
