#%% IMPORT PACKAGES AND GLOBAL CONFIGURATIONS
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from collections import deque
sns.set(font_scale=1.1)
plt.rcParams["font.family"] = "Times New Roman"
plt.rc('xtick', labelsize=20)
plt.rc('ytick', labelsize=20)
plt.rc('legend', fontsize=16)

folder = 'img'
t_column = 'mean_total_time'
speedup_column = 'speedup_total'
efficiency_column = 'efficiency_total'

ENGINES = ['xdevs-new-naive', 'xdevs-new-full']
THREADS = [2, 4, 8, 16, 32, 64, 128]
MODELS = [('HOmod', 10, 10), ('HOmod', 25, 25), ('HOmod', 40, 40)]
COLUMNS = [f'{model}-{width}-{depth}' for model, width, depth in MODELS]

df = pd.read_csv('par_results/par_results.csv')

#%%
vals = dict()
percents = dict()

for engine in ENGINES:
    vals[engine] = dict()
    percents[engine] = dict()
    for threads in THREADS:
        vals[engine][threads] = list()
        percents[engine][threads] = list()
        for (model, width, depth) in MODELS:
            threads_df = df[(df['engine'] == engine) & (df['threads'] == threads) & (df['model'] == model) & (df['width'] == width) & (df['depth'] == depth)]
            vals[engine][threads].append(threads_df[speedup_column].values[0])
            percents[engine][threads].append(threads_df[efficiency_column].values[0] * 100)


#%% GRAPH
x = np.arange(len(MODELS))  # the label locations
width = 0.13  # the width of the bars

for engine, models in vals.items():
    fig, ax = plt.subplots(figsize=(12, 4))
    multiplier = 0
    for attribute, measurement in models.items():
        offset = width * multiplier
        rects = ax.bar(x + offset, measurement, width, label=attribute)
        # ax.bar_label(rects, padding=3)
        multiplier += 1
    # Add some text for labels, title and custom x-axis tick labels, etc.
    ax.set_ylabel('Speedup', fontsize=20)
    ax.set_xticks(x + 3.5 * width, COLUMNS)
    ax.legend(loc='upper center', ncols=7)
    ax.set_ylim(0, 9.6)

    plt.savefig(f'{folder}/par/{engine}_par_speedup.pdf', format='pdf')
    plt.close()


#%% GRAPH
x = np.arange(len(MODELS))  # the label locations
width = 0.13  # the width of the bars

for engine, models in percents.items():
    fig, ax = plt.subplots(figsize=(12, 4))
    multiplier = 0
    for attribute, measurement in models.items():
        offset = width * multiplier
        rects = ax.bar(x + offset, measurement, width, label=attribute)
        # ax.bar_label(rects, padding=3)
        multiplier += 1
    # Add some text for labels, title and custom x-axis tick labels, etc.
    ax.set_ylabel('Efficiency (%)', fontsize=20)
    ax.set_xticks(x + 3.5 * width, COLUMNS)
    ax.legend(loc='upper center', ncols=7)
    ax.set_ylim(0, 120)

    plt.savefig(f'{folder}/par/{engine}_par_efficiency.pdf', format='pdf')
    plt.close()


def interpolate_colors(r1, g1, b1, r2, g2, b2, n):
    return [(round(r1 + i * (r2 - r1) / n), round(g1 + i * (g2 - g1) / n), round(b1 + i * (b2 - b1) / n)) for i in range(n + 1)]

