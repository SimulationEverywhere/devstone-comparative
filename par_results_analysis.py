import pandas as pd
from scipy import stats

THREADS = [1, 2, 4, 8, 16, 32, 64, 128]


if __name__ == '__main__':
    final = None
    for threads in THREADS:
        df = pd.read_csv(f'par_results/raw/par_{threads}_results.csv', sep=';')
        # remove useless columns
        del df['iter']
        del df['model_time']
        del df['runner_time']
        del df['sim_time']
        del df['int_delay']
        del df['ext_delay']
        # add threads and setup time column
        df.insert(2, 'threads', threads)
        # setup_time = df['model_time'] + df['runner_time']
        # df.insert(7, 'setup_time', setup_time)
        # re-label engines
        df['engine'] = df['engine'].replace('xdevs-rs-new-sequential', 'xdevs-new')
        df['engine'] = df['engine'].replace('xdevs-rs-new-parallel', 'xdevs-new-naive')
        df['engine'] = df['engine'].replace('xdevs-rs-new-fullparallel', 'xdevs-new-full')

        aux = df.groupby(['engine', 'threads', 'model', 'width', 'depth'])
        # Compute mean time values
        mean = aux.mean()
        mean.rename(columns={'total_time': 'mean_total_time'}, inplace=True)
        # mean.rename(columns={'model_time': 'mean_model_time', 'runner_time': 'mean_runner_time',
        #                      'setup_time': 'mean_setup_time', 'sim_time': 'mean_sim_time',
        #                      'total_time': 'mean_total_time'}, inplace=True)

        # We compute the standard error over the mean...
        sem = aux.sem()
        # The z-score for our confidence interval...
        alpha = 0.95  # Desired confidence level
        z = stats.norm.ppf((1 + alpha) / 2.)
        # and the normal confidence interval
        conf_z = sem * z
        # The t-score for our confidence interval...
        t = stats.t.ppf((1 + alpha) / 2., aux.count() - 1)
        # and the t-distribution confidence interval
        conf_t = sem * t
        # Compute the p-value
        grouped_total_time = aux['total_time'].mean()
        mean_total_time = mean['mean_total_time']

        p_values = list()
        for (engine, threads, model, width, depth), group in aux:
            mean_ = mean.loc[(engine, threads, model, width, depth)]
            t_stat, p_value = stats.ttest_1samp(group['total_time'], mean_)
            p_values.append(p_value)
        sem.rename(columns={'total_time': 'sem_total_time'}, inplace=True)
        # sem.rename(columns={'model_time': 'sem_model_time', 'runner_time': 'sem_runner_time',
        #                    'setup_time': 'sem_setup_time', 'sim_time': 'sem_sim_time',
        #                    'total_time': 'sem_total_time'}, inplace=True)

        conf_z.rename(columns={'total_time': f'conf_z_{alpha}_total_time'}, inplace=True)
        # conf_z.rename(columns={'model_time': f'conf_z_{alpha}_model_time', 'runner_time': f'conf_z_{alpha}_runner_time',
        #                        'setup_time': f'conf_z_{alpha}_setup_time', 'sim_time': f'conf_z_{alpha}_sim_time',
        #                        'total_time': f'conf_z_{alpha}_total_time'}, inplace=True)
        conf_t.rename(columns={'total_time': f'conf_t_{alpha}_total_time'}, inplace=True)
        # conf_t.rename(columns={'model_time': f'conf_t_{alpha}_model_time', 'runner_time': f'conf_t_{alpha}_runner_time',
        #                        'setup_time': f'conf_t_{alpha}_setup_time', 'sim_time': f'conf_t_{alpha}_sim_time',
        #                        'total_time': f'conf_t_{alpha}_total_time'}, inplace=True)
        mean = mean.join(sem)
        mean = mean.join(conf_z)
        mean = mean.join(conf_t)
        mean['p_value'] = p_values

        mean = mean.reset_index()
        mean.sort_values(['engine', 'model', 'width', 'depth'], inplace=True)

        mean.to_csv(f'par_results/par_{threads}_results.csv', index=False)
        final = mean if final is None else pd.concat([final, mean])

    ref_df = final[final['threads'] == 1].reset_index()

    res_df = ref_df
    res_df['speedup_total'] = 1.0
    res_df['efficiency_total'] = 1.0
    for engine in ['xdevs-new-naive', 'xdevs-new-full']:
        engine_df = final[final['engine'] == engine]
        for threads in THREADS[1:]:
            thread_df = engine_df[engine_df['threads'] == threads].reset_index().reindex(index=ref_df.index)
            thread_df['speedup_total'] = ref_df['mean_total_time'] / thread_df['mean_total_time']
            thread_df['efficiency_total'] = thread_df['speedup_total'] / min(threads, 8)
            res_df = thread_df if res_df is None else pd.concat([res_df, thread_df])

    res_df.to_csv('par_results/results.csv', index=False)
