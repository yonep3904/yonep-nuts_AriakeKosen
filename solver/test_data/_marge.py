import re
import pandas as pd
from pathlib import Path

##############################
NOT_INPUT = [
   
]
##############################

reg = re.compile(r'(.+)_(\d+)\.csv')

def main():
    global reg, NOT_INPUT
    dir = Path(__file__).parent
    not_input = map(lambda x: dir/ x, NOT_INPUT)

    solvers = dict()
    for file in filter(lambda x: x.suffix == '.csv' and x.name not in not_input, dir.iterdir()):
        if hit := reg.fullmatch(file.name):
            solver, seed = hit.groups()
            if solver in solvers:
                solvers[solver].append(seed)
            else:
                solvers[solver] = [seed]

    for key, value in solvers.items():
        if len(value) > 1:
           source = [dir / f'{key}_{i}.csv' for i in value]
           marge_csv = dir / f'{key}_marge.csv'
           csv_data = marge(source, drop_duplicates=False)
           csv_data.to_csv(marge_csv, index=False)
           source_display = "\n".join(map(lambda x: x.name, source))
           print(f'{source_display}\n  -> {marge_csv.name}')

def marge(files: list[Path], drop_duplicates: bool = False):
    dfs = [pd.read_csv(file) for file in files]
    marged_df = pd.concat(dfs).reset_index().drop('index', axis=1)
    if drop_duplicates:
        marged_df.drop_duplicates(inplace=True)
    return marged_df

if __name__ == '__main__':
    main()