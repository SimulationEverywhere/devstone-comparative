#%% IMPORT PACKAGES AND GLOBAL CONFIGURATIONS
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from collections import deque
sns.set(font_scale=1.1)
plt.rcParams["font.family"] = "Times New Roman"

#%% LOAD DATA AND DEFINE CONSTANTS
folder = 'img'
t_columns = ['total']
engines = ['xdevs-new-full', 'xdevs-new-naive', 'xdevs-new', 'xdevs-old', 'adevs']
reference = 'adevs'

df = pd.read_csv('results/results.csv')

#%% SPEEDUP GRAPHS
for target, target_df in df.groupby('target'):
    for engine, engine_df in target_df.groupby('engine'):
        if engine == reference:
            continue
        for model, model_df in engine_df.groupby('model'):
            for t in t_columns:
                speedup = f'speedup_{t}'
                widths = list(model_df['width'].unique())
                depths = list(model_df['depth'].unique())
                widths.sort(reverse=True)
                depths.sort()
                heatmap = np.zeros((len(widths), len(depths)))
                for i, w in enumerate(widths):
                    for j, d in enumerate(depths):
                        heatmap[i, j] = model_df[(model_df['width'] == w) & (model_df['depth'] == d)][speedup].values[0]

                ax = sns.heatmap(heatmap, cmap="magma", xticklabels=depths, yticklabels=widths, cbar=True)
                ax.set(xlabel="depth", ylabel="width")
                plt.subplots_adjust(bottom=0.15)
                plt.xlabel("depth", fontsize=20)
                plt.ylabel("width", fontsize=20)
                plt.savefig(f'results/speedup_heatmap/{target}_{engine}_{model}_{t}.pdf', format='pdf')
                plt.close()
                # plt.show()

#%% TIME GRAPHS
for target, target_df in df.groupby('target'):
    for model, model_df in target_df.groupby('model'):
        ref_df = model_df[model_df['engine'] == reference].reset_index().sort_values(['mean_total_time'])
        ref_df['xlabel'] = ref_df['width'].astype(str) + "-" + ref_df["depth"].astype(str)
        x = list(range(len(ref_df.index)))

        for time in t_columns:
            fig, ax = plt.subplots()
            time_col = f'mean_{time}_time'
            ax.plot(ref_df['xlabel'], ref_df[time_col], label=reference)
            for engine, engine_df in model_df.groupby('engine'):
                if engine == reference:
                    continue
                engine_df = engine_df.reset_index().reindex(index=ref_df.index)
                ax.plot(ref_df['xlabel'], engine_df[time_col], label=engine)
            ax.set_ylabel('time [s]', fontsize=20)
            ax.set_xlabel('structure [width-depth]', fontsize=20)
            N = 10 if model == 'HOmod' else 40  # 1 tick every 3
            xticks_pos = ax.get_xticks()
            xticks_labels = ax.get_xticklabels()
            myticks = [j for i, j in enumerate(xticks_pos) if not i % N]  # index of selected ticks
            newlabels = [label for i, label in enumerate(xticks_labels) if not i % N]
            ax.set_xticks(myticks, newlabels, rotation=30)
            ax.legend()
            plt.subplots_adjust(bottom=0.2)
            plt.margins(0.05, tight=True)
            plt.savefig(f'results/time/{target}_{model}_{time}.pdf', format='pdf')
            # plt.show()
            plt.close()

            fig, ax = plt.subplots()
            # next(ax._get_lines.prop_cycler)
            for engine, engine_df in model_df.groupby('engine'):
                if engine == reference:
                    pass
                engine_df = engine_df.reset_index().reindex(index=ref_df.index)
                ax.plot(ref_df['xlabel'], engine_df[f'speedup_{time}'], label=engine)
            ax.set_ylabel('speedup', fontsize=20)
            ax.set_xlabel('structure [width-depth]', fontsize=20)
            N = 10 if model == 'HOmod' else 40  # 1 tick every 3
            xticks_pos = ax.get_xticks()
            xticks_labels = ax.get_xticklabels()
            myticks = [j for i, j in enumerate(xticks_pos) if not i % N]  # index of selected ticks
            newlabels = [label for i, label in enumerate(xticks_labels) if not i % N]
            ax.set_xticks(myticks, newlabels, rotation=30)
            ax.legend(loc='upper left')
            plt.subplots_adjust(bottom=0.2)
            plt.margins(0.05, tight=True)
            plt.savefig(f'results/speedup/{target}_{model}_{time}.pdf', format='pdf')
            # plt.show()
            plt.close()

#%% PERCENTAGE CHARS
for target, target_df in df.groupby('target'):
    for time in t_columns:
        time_col = f'mean_{time}_time'
        percentages = dict()
        engines = deque()
        for engine, engine_df in target_df.groupby('engine'):
            engines.appendleft(engine)
            t_sum = engine_df[time_col].sum()
            for model, model_df in engine_df.groupby('model'):
                if model not in percentages:
                    percentages[model] = deque()
                percentages[model].appendleft(100 * model_df[time_col].sum() / t_sum)
        fig, ax = plt.subplots(figsize=(7, 3))
        left = None
        for model in 'LI', 'HI', 'HO', 'HOmod':
            bar = percentages[model]
            ax.barh(engines, percentages[model], align='center', height=.7, left=left, label=model)
            left = bar if left is None else [left[i] + bar[i] for i in range(len(left))]
        ax.set_yticks(engines)
        ax.set_xlabel('execution time [%]', fontsize=15)
        ax.legend()
        plt.tight_layout()
        plt.savefig(f'results/percentage/{target}_{time}.pdf', format='pdf')
        plt.close()
