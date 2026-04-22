import pandas as pd
from scipy import stats

TARGETS = ['ubuntu']
MODELS = ['LI', 'HI', 'HO', 'HOmod']
REFERENCE = 'adevs'


if __name__ == '__main__':
    final = None
    for target in TARGETS:
        df = None
        for model in MODELS:
            mod_df = pd.read_csv(f'results/raw_{target}/results_{model}.csv', sep=';')
            # remove useless columns
            del mod_df['iter']
            del mod_df['int_delay']
            del mod_df['ext_delay']
            # add setup column
            setup_time = mod_df['model_time'] + mod_df['runner_time']
            mod_df.insert(6, 'setup_time', setup_time)
            # concatenate results
            df = mod_df if df is None else pd.concat([df, mod_df])
        aux = df.groupby(['engine', 'model', 'width', 'depth'])
        # Compute mean time values
        mean = aux.mean()
        mean.rename(columns={'model_time': 'mean_model_time', 'runner_time': 'mean_runner_time',
                             'setup_time': 'mean_setup_time', 'sim_time': 'mean_sim_time',
                             'total_time': 'mean_total_time'}, inplace=True)

        # We compute the standard error over the mean...
        sem = aux.sem()
        # The z-score for our confidence interval...
        alpha = 0.95  # Desired confidence level
        z = stats.norm.ppf((1 + alpha) / 2.)
        # and the confidence interval
        conf = sem * z
        sem.rename(columns={'model_time': 'sem_model_time', 'runner_time': 'sem_runner_time',
                            'setup_time': 'sem_setup_time', 'sim_time': 'sem_sim_time',
                            'total_time': 'sem_total_time'}, inplace=True)

        conf.rename(columns={'model_time': f'conf_{alpha}_model_time', 'runner_time': f'conf{alpha}_runner_time',
                             'setup_time': f'conf_{alpha}_setup_time', 'sim_time': f'conf_{alpha}_sim_time',
                             'total_time': f'conf_{alpha}_total_time'}, inplace=True)
        mean = mean.join(sem)
        mean = mean.join(conf)

        # Compute speedups
        mean = mean.reset_index()
        mean.sort_values(['engine', 'model', 'width', 'depth'], inplace=True)
        mean_ref = mean[mean['engine'] == REFERENCE]
        for _ in range(len(mean['engine'].unique()) - 1):
            mean_ref = pd.concat([mean_ref, mean[mean['engine'] == REFERENCE]], ignore_index=True)
        for t in 'model', 'runner', 'setup', 'sim', 'total':
            mean[f'speedup_{t}'] = mean_ref[f'mean_{t}_time'] / mean[f'mean_{t}_time']

        mean.to_csv(f'results/results_{target}.csv', index=False)
        mean.insert(0, 'target', target)
        final = mean if final is None else pd.concat([final, mean])
    final.to_csv('results/results.csv', index=False)
