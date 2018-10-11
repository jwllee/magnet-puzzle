import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mplib

plt.style.use('seaborn-whitegrid')
plt.rcParams['font.size'] = 15.0
plt.rcParams['axes.labelsize'] = 15.0
plt.rcParams['xtick.labelsize'] = 15.0
plt.rcParams['ytick.labelsize'] = 15.0
plt.rcParams['legend.fontsize'] = 15.0

# set the max column width
pd.options.display.max_colwidth = 1000

# avoid having warnings from chained assignments
pd.options.mode.chained_assignment = None

print(plt.style.available)


if __name__ == '__main__':
    fp = './results.csv'
    plot0_fp = './plot0.png'
    plot1_fp = './plot1.png'

    result_df = pd.read_csv(fp)

    sz = 30
    markers = ['o', 'v', 'P', 'x']
    colors = ["#4C72B0", "#DD8452", "#55A868", "#C44E52", "#8172B3",
              "#937860", "#DA8BC3", "#8C8C8C", "#CCB974", "#64B5CD"]

    # plot 1
    # size vs. time (s)
    df_0 = result_df[['variant', 'size', 'time']]

    fig, ax = plt.subplots(figsize=(10, 5))

    for i, (name, group) in enumerate(df_0.groupby('variant')):
        print('Plotting {}th plot {}'.format(i, name))
        print(group.head())

        ax_i = group.plot(kind='scatter', x='size', y='time', label=name,
                          marker=markers[i], color=colors[i], s=sz, ax=ax)

        ax.set_ylabel('time (s)')

    plt.tight_layout()
    fig.savefig(plot0_fp)

    # plot 2
    # size vs. unassignment
    df_1 = result_df[['variant', 'size', 'unassign']]

    fig, ax = plt.subplots(figsize=(10, 5))

    handles = []
    labels = []

    for i, (name, group) in enumerate(df_1.groupby('variant')):
        print('Plotting {}th plot {}'.format(i, name))
        print(group.head())

        ax_i = group.plot(kind='scatter', x='size', y='unassign', label=name,
                          marker=markers[i], color=colors[i], s=sz, ax=ax)

    plt.tight_layout()
    fig.savefig(plot1_fp)
