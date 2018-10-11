import os, sys, re, csv
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

plt.style.use('ggplot')
plt.rcParams['font.size'] = 15.0
plt.rcParams['axes.labelsize'] = 15.0
plt.rcParams['xtick.labelsize'] = 15.0
plt.rcParams['ytick.labelsize'] = 15.0
plt.rcParams['legend.fontsize'] = 15.0

# set the max column width
pd.options.display.max_colwidth = 1000

# avoid having warnings from chained assignments
pd.options.mode.chained_assignment = None



def is_starter_line(s):
    return 'o_test_' in s or 'test_' in s


get_board_dim = lambda s: re.match(r"\D+\d+ x \d+\D+(\d+)", s).group(1)
get_unassign = lambda s: re.match(r"\D+(\d+)", s).group(1)
get_timestamp = lambda s: re.match(r"\D+(\d+:\d+.\d+)", s).group(1)


def timestamp_to_secs(timestamp):
    a = [60, 1, 0]
    split = re.split('[:.]', timestamp)
    return sum(x * int(y) for x, y in zip(a, split))


namemap = {
    'no-no': 'vanilla',
    'no-most': 'most',
    'feasible-no': 'feasible',
    'feasible-most': 'feasible-most'
}


if __name__ == '__main__':

    df_l = []

    for f in os.listdir('.'):
        if not os.path.isfile(f) or not f.endswith('-10m.txt'):
            continue

        data_i = []
        exp, dim, time, unassign = None, None, None, None

        print('Processing {}'.format(f))

        with open(f, 'r', encoding = "ISO-8859-1") as file:
            for line in file:
                if 'test_' in line:
                    exp = line.split('/')[-1].replace('.txt', '').strip()
                    # reset variables
                    dim, time, unassign = -1, -1, -1
                elif 'Board dimension' in line:
                    dim = get_board_dim(line)
                elif 'unassignment' in line:
                    unassign = get_unassign(line)
                elif 'time' in line:
                    timestamp = get_timestamp(line)
                    time = timestamp_to_secs(timestamp)
                    # add to data_i
                    data_i.append((exp, dim, time, unassign))

        exp_l, dim_l, time_l, unassign_l = zip(*data_i)

        df_i = pd.DataFrame({
            'variant': [namemap[f.replace('-10m.txt', '')] for _ in range(len(data_i))],
            'data': exp_l,
            'size': dim_l,
            'time': time_l,
            'unassign': unassign_l
        }, columns=['variant', 'data', 'size', 'time', 'unassign'])

        df_l.append(df_i)

    df = pd.concat(df_l)

    # output the df
    out_fp = 'results.csv'
    df.to_csv(out_fp, index=False)
