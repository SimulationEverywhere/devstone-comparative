import pandas as pd


if __name__ == '__main__':
    df = None
    for model in 'LI', 'HI', 'HO', 'HOmod':
        mod_df = pd.read_csv(f'results_{model}.csv', sep=';', index_col=None)
        del mod_df['iter']
        del mod_df['int_delay']
        del mod_df['ext_delay']
        df = mod_df if df is None else pd.concat([df, mod_df])
    aux = df.groupby(['engine', 'model', 'width', 'depth'])
    mean = aux.mean()
    mean.rename(columns={'model_time': 'mean_model_time', 'runner_time': 'mean_runner_time', 'sim_time': 'mean_sim_time', 'total_time': 'mean_total_time'}, inplace=True)
    std = aux.std()
    std.rename(columns={'model_time': 'std_model_time', 'runner_time': 'std_runner_time', 'sim_time': 'std_sim_time', 'total_time': 'std_total_time'}, inplace=True)
    mean = mean.join(std)
    mean.to_csv('results_macos.csv')
    print('done')
