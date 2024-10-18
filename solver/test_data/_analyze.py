import re
import json
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

##############################
COUNT_LIM = 100000
TIME_LIM = None

NOT_INPUT = [
   
]
##############################

reg = re.compile(r'(.+)_(\d+|marge)\.csv')

def main():
    global reg, NOT_INPUT
    dir = Path(__file__).parent
    not_input = map(lambda x: dir/ x, NOT_INPUT)

    for file in filter(lambda x: x.suffix == '.csv' and x.name not in not_input, dir.iterdir()):
        try:
            fig, json_data = analyze(file)

            fig.savefig(file.with_suffix('.png'), format='png',dpi=300)
            with open(file.with_suffix('.json'), 'w') as f:
                json.dump(json_data, f, indent=4)

            print(f'{file.name}')
        except ValueError as e:
            print(f'Unable Read : {file.name}')
    
def analyze(file: Path):
    global COUNT_LIM, TIME_LIM, reg
    if hit := reg.fullmatch(file.name):
        solver, seed = hit.groups()
    else:
        raise ValueError('Input file name error.')
    
    df = pd.read_csv(file)  
    df['size']= np.sqrt(df['width'] * df['height'])
    df.drop(['width', 'height', 'seed'], axis=1, inplace=True)
    # countが-1のデータは欠損値としてレコードを削除
    df.loc[df['count'] == -1, 'count'] = np.nan
    nan_count = df['count'].isna().sum()
    df.dropna(inplace=True)
    count_max = df['count'].max()
    time_max = df['time'].max()
    # sizeが同じレコードでグループ化
    df = df.groupby(['size']).mean().reset_index()

    size = df['size']
    count = df['count']
    time = df['time']

    # 2次近似
    count_coefficients = np.polyfit(df['size'], df['count'], 2)
    time_coefficients = np.polyfit(df['size'], df['time'], 2)

    # グラフの作成
    fig = plt.figure(figsize = (10,6))
    ax_count = fig.add_subplot(1, 2, 1)
    ax_time = fig.add_subplot(1, 2, 2)

    ax_count.plot(size, np.poly1d(count_coefficients)(size), label='count_coef', color='green', alpha=0.5)
    ax_count.plot(size, count, label='count', color='blue', alpha=0.5)
    ax_time.plot(size, np.poly1d(time_coefficients)(size), label='time_coef', color='green', alpha=0.5)
    ax_time.plot(size, time, label='time', color='blue', alpha=0.5)

    fig.suptitle(f'{solver}-{seed}{"" if nan_count == 0 else f"(nan : {nan_count})"}')

    ax_count.set_xlabel('size')
    ax_count.set_ylabel('count')
    ax_time.set_xlabel('size')
    ax_time.set_ylabel('time[ms]')

    ax_count.set_xlim(0, 260)
    ax_count.set_ylim(0, COUNT_LIM if COUNT_LIM is not None else count.max()*1.05)
    ax_time.set_xlim(0, 260)
    ax_time.set_ylim(0, TIME_LIM if TIME_LIM is not None else time.max()*1.05)

    ax_count.grid(axis='both')
    ax_time.grid(axis='both')

    json_data = {
       'solver': solver,
       'seed': seed,
       'nan': int(nan_count),
       'count': {
           'plots': int(count.count()),
           'max': int(count_max),
           'coefficients': [float(i) for i in count_coefficients.tolist()],
       },
       'time': {
           'plots': int(time.count()),
           'max': float(time_max),
           'coefficients': [float(i) for i in time_coefficients.tolist()],
       }
    }

    return fig, json_data

if __name__ == '__main__':
    main()