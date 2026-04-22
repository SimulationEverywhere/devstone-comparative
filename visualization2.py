#%% IMPORT PACKAGES AND GLOBAL CONFIGURATIONS
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from collections import deque
sns.set_theme(font_scale=1.1)
plt.rcParams["font.family"] = "Times New Roman"
plt.rc('xtick', labelsize=17)
plt.rc('ytick', labelsize=17)
plt.rc('legend', fontsize=15)

folder = 'img'
t_column = 'mean_total_time'
speedup_column = 'speedup_total'
engines = ['xdevs-new-full', 'xdevs-new-naive', 'xdevs-new', 'xdevs-orig', 'adevs']
references = [engines[i] for i in range(len(engines) - 1, 0, -1)]

df = pd.read_csv('results/results.csv')

#%% SPEEDUP HEATMAPS
for reference in references:
    reference_df = df[df['engine'] == reference]
    for engine in engines:
        engine_df = df[df['engine'] == engine]
        if engine == reference:
            break
        for model, model_df in engine_df.groupby('model'):
            ref_model_df = reference_df[reference_df['model'] == model]
            widths = list(model_df['width'].unique())
            depths = list(model_df['depth'].unique())
            widths.sort(reverse=True)
            depths.sort()
            heatmap = np.zeros((len(widths), len(depths)))
            for i, w in enumerate(widths):
                for j, d in enumerate(depths):
                    ref_speedup = ref_model_df[(ref_model_df['width'] == w) & (ref_model_df['depth'] == d)][speedup_column].values[0]
                    model_speedup = model_df[(model_df['width'] == w) & (model_df['depth'] == d)][speedup_column].values[0]
                    heatmap[i, j] = model_speedup / ref_speedup
            ax = sns.heatmap(heatmap, cmap="magma", xticklabels=depths, yticklabels=widths, cbar=True)
            ax.set(xlabel="depth", ylabel="width")
            plt.subplots_adjust(bottom=0.15)
            plt.xlabel("depth", fontsize=20)
            plt.ylabel("width", fontsize=20)

            plt.savefig(f'{folder}/speedup_heatmap/{model}_{engine}_vs_{reference}.pdf', format='pdf')
            plt.close()

#%% SPEEDUP CONTOUR GRAPHS
for reference in references:
    reference_df = df[df['engine'] == reference]
    for engine in engines:
        engine_df = df[df['engine'] == engine]
        if engine == reference:
            break
        for model, model_df in engine_df.groupby('model'):
            ref_model_df = reference_df[reference_df['model'] == model]
            widths = list(model_df['width'].unique())
            depths = list(model_df['depth'].unique())
            widths.sort(reverse=True)
            depths.sort()
            heatmap = np.zeros((len(widths), len(depths)))
            for i, w in enumerate(widths):
                for j, d in enumerate(depths):
                    ref_speedup = ref_model_df[(ref_model_df['width'] == w) & (ref_model_df['depth'] == d)][speedup_column].values[0]
                    model_speedup = model_df[(model_df['width'] == w) & (model_df['depth'] == d)][speedup_column].values[0]
                    heatmap[i, j] = model_speedup / ref_speedup
            
            fig, ax = plt.subplots()
            X, Y = np.meshgrid(depths, widths)
            CS_filled= ax.contourf(X, Y, heatmap, levels=[0, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2, 2.25, 2.5, 2.75], cmap='magma')
            
            CS_lines = ax.contour(X, Y, heatmap, levels=[0, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2, 2.25, 2.5, 2.75], colors='white')

            # Manually specify the positions of the labels
            manual_positions = [(x + 0.5, y) for x, y in zip(X.flatten(), Y.flatten())]

            # Add labels to the contour lines with manual positions
            # ax.clabel(CS_lines, fontsize=20, manual=manual_positions)

            ax.clabel(CS_lines, fontsize=20)

            ax.set(xlabel="depth", ylabel="width")
            plt.xlabel("depth", fontsize=20)
            plt.ylabel("width", fontsize=20)

            # Set the ticks for x and y axes
            ax.set_xticks(range(5, 51, 5))
            ax.set_yticks(range(5, 51, 5))

            # Set the aspect ratio to be equal
            ax.set_aspect('equal', 'box')
            plt.tight_layout()

            plt.savefig(f'{folder}/speedup_contour/{model}_{engine}_vs_{reference}.pdf', format='pdf')
            plt.close()

#%% TIME GRAPHS
for model, model_df in df.groupby('model'):
    for reference in references:
        ref_df = model_df[model_df['engine'] == reference].reset_index().sort_values(['mean_total_time'])
        ref_df['xlabel'] = ref_df['width'].astype(str) + "-" + ref_df["depth"].astype(str)
        x = list(range(len(ref_df.index)))

        fig, ax = plt.subplots()
        for engine in engines:
            engine_df = model_df[model_df['engine'] == engine].reset_index().reindex(index=ref_df.index)
            ax.plot(ref_df['xlabel'], engine_df[t_column], label=engine)
            if engine == reference:
                break
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
        plt.savefig(f'{folder}/time/{model}_vs_{reference}.pdf', format='pdf')
        # plt.show()
        plt.close()

#%% SPEEDUP GRAPHS
for model, model_df in df.groupby('model'):
    for reference in references:
        ref_df = model_df[model_df['engine'] == reference].reset_index().sort_values(['mean_total_time'])
        ref_df['xlabel'] = ref_df['width'].astype(str) + "-" + ref_df["depth"].astype(str)
        x = list(range(len(ref_df.index)))

        fig, ax = plt.subplots()
        for engine in engines:
            if engine == reference:
                break
            engine_df = model_df[model_df['engine'] == engine].reset_index().reindex(index=ref_df.index)
            speedup = ref_df[t_column] / engine_df[t_column]
            ax.plot(ref_df['xlabel'], speedup, label=engine)
        ax.set_ylabel('speedup', fontsize=20)
        ax.set_xlabel('structure [width-depth]', fontsize=20)
        N = 10 if model == 'HOmod' else 40  # 1 tick every 3
        xticks_pos = ax.get_xticks()
        xticks_labels = ax.get_xticklabels()
        myticks = [j for i, j in enumerate(xticks_pos) if not i % N]  # index of selected ticks
        newlabels = [label for i, label in enumerate(xticks_labels) if not i % N]
        ax.set_xticks(myticks, newlabels, rotation=30)
        ax.legend()
        # plt.legend(fontsize=15)
        plt.subplots_adjust(bottom=0.25)
        plt.margins(0.05, tight=True)
        plt.savefig(f'{folder}/speedup/{model}_vs_{reference}.pdf', format='pdf')
        # plt.show()
        plt.close()

#%% PERCENTAGE CHARS
percentages = dict()
res = deque()
for engine in engines:
    engine_df = df[df['engine'] == engine]
    res.appendleft(engine)
    t_sum = engine_df[t_column].sum()
    for model, model_df in engine_df.groupby('model'):
        print(model)
        if model not in percentages:
            percentages[model] = deque()
        percentages[model].appendleft(100 * model_df[t_column].sum() / t_sum)
fig, ax = plt.subplots(figsize=(7, 3))
left = None
for model in 'LI', 'HI', 'HO', 'HOmod':
    bar = percentages[model]
    ax.barh(res, percentages[model], align='center', height=.7, left=left, label=model)
    left = bar if left is None else [left[i] + bar[i] for i in range(len(left))]
ax.set_yticks(res)
ax.set_xlabel('execution time [%]', fontsize=15)
ax.legend()
plt.tight_layout()
plt.savefig(f'{folder}/percentage.pdf', format='pdf')
plt.close()
