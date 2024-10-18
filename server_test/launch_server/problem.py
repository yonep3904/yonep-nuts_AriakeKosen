import json
import random
import numpy as np
from pathlib import Path
from copy import deepcopy

PATTENS_MAX_SIZE = 70

def main():
    pass

class Problem:
    """問題を生成し、JSON形式で出力します
    """
    def __init__(self, width: int, height: int) -> None:
        # board:
        self.width = width
        self.height = height
        self.start = None
        self.goal = None
        # patterns:
        self.patterns = []

    def generate_original_board(self, start: list[list]=None, goal: list[list]=None):
        """任意のボードを作成します

        Args:
            start (list[list]): 初期状態のボード
            goal (list[list]): 最終状態のボード
        """
        if start is not None:
            self.start = start
        if goal is not None:
            self.goal = goal
    
    def generate_zero_filled_board(self):
        """ゼロ埋めされたボードを生成します
        """
        self.start = [[0 for _ in range(self.width)] for __ in range(self.height)]
        self.goal = deepcopy(self.start)

    def generate_vertical_stripe_board(self):
        """縦方向ストライプのボードを生成します
        """
        self.start = [[i%4 for i in range(self.width)] for _ in range(self.height)]
        self.goal = deepcopy(self.start)
        
    def generate_horizontal_stripe_board(self):
        """横方向ストライプのボードを生成します
        """
        self.start = [[i%4 for _ in range(self.width)] for i in range(self.height)]
        self.goal = deepcopy(self.start)
    
    def generate_diagonal_stripe_board(self):
        """斜め方向ストライプのボードを生成します"""
        self.start = [[(i+j)%4 for i in range(self.width)] for j in range(self.height)]
        self.goal = deepcopy(self.start)

    def generate_random_board(self, seed: int=None):
        """ランダムなボードを生成します.各種類のピース数が全体の10%以上あることは保証されています

        Args:
            seed (int, optional): シード値を指定.Noneでランダムなシード地. Defaults to None.
        """
        rng = np.random.default_rng(seed)
        least_n = int(self.width*self.height/10) + 1
        random_n = self.width*self.height - 4*least_n
        board = np.concatenate((np.array([0, 1, 2, 3]*least_n), rng.integers(0, 4, random_n)))

        self.start = rng.permutation(board).reshape((self.height, self.width)).tolist()
        self.goal = deepcopy(self.start)
    
    def rotate_x_board(self, n: int, goal: bool=True):
        """ボードを横(左)方向にローテートします

        Args:
            n (int): ローテート回数
            goal (bool, optional): 対象ボードを指定.Trueでgoal/Falseでstart. Defaults to True.
        """
        if goal:
            for _ in range(n):
                for line in self.goal:
                    tmp = line.pop(0)
                    line.append(tmp)
        else:
            for _ in range(n):
                for line in self.start:
                    tmp = line.pop(0)
                    line.append(tmp)
    
    def rotate_y_board(self, n: int, goal: bool=True):
        """ボードを縦(上)方向にローテートします

        Args:
            n (int): ローテート回数
            goal (bool, optional): 対象ボードを指定.Trueでgoal/Falseでstart. Defaults to True.
        """
        if goal:
            for _ in range(n):
                tmp = self.goal.pop(0)
                self.goal.append(tmp)
        else:
            for _ in range(n):
                tmp = self.start.pop(0)
                self.start.append(tmp)
    
    def shuffle_board(self, seed: int=None, goal: bool=True):
        """ボードをシャッフルします

        Args:
            seed (int, optional): シード値を指定.Noneでランダムなシード地. Defaults to None.
            goal (bool, optional): 対象ボードを指定.Trueでgoal/Falseでstart. Defaults to True.
        """
        rng = np.random.default_rng(seed)
        if goal:
            array_1d = np.array(self.goal).flatten()
            self.goal = rng.permutation(array_1d).reshape((self.height, self.width)).tolist()
        else:
            array_1d = np.array(self.start).flatten()
            self.start = rng.permutation(array_1d).reshape((self.height, self.width)).tolist()

    def hash_board(self, hash_intensity: int=30, seed: int=None, goal: bool=True):
        """ボードをハッシュ(ある一定の規則に従ってランダムより弱いシャッフル)します

        Args:
            seed (int, optional): シード値を指定.Noneでランダムなシード地. Defaults to None.
            hash_intensity (int, optional): ハッシュする度合いを指定(数値が小さいと弱いハッシュ). Defaults to 30.
            goal (bool, optional): 対象ボードを指定.Trueでgoal/Falseでstart. Defaults to True.
        """
        rng = np.random.default_rng(seed)
        if goal:
            for _ in range(max(0, hash_intensity-5), hash_intensity+5):
                for line in self.goal:
                    tmp = line.pop(rng.integers(0, self.width-1))
                    line.append(tmp)
                tmp = self.goal.pop(rng.integers(0, self.height-1))
                self.goal.append(tmp)
        else:
            for _ in range(max(0, hash_intensity-5), hash_intensity+5):
                for line in self.start:
                    tmp = line.pop(rng.integers(0, self.width-1))
                    line.append(tmp)
                tmp = self.start.pop(rng.integers(0, self.height-1))
                self.start.append(tmp)
            
    def add_original_pattern(self, pattern: list[list]):
        """任意の抜き型を追加します

        Args:
            pattern (list[list]): 抜き型
        """
        self.patterns.append(pattern)
        
    def add_diagonal_stripe_pattern(self, width: int=None, height:int = None):
        """斜め方向ストライプの抜き型を生成します

        Args:
            width (int, optional): 抜き型の横幅. Defaults to None.
            height (int, optional): 抜き型の縦幅. Defaults to None.
        """
        self.patterns.append([[(i+j)%2 for i in range(width)] for j in range(height)])

    def add_random_pattern(self, width: int=None, height:int = None, seed: int=None):
        """ランダムな抜き型を生成します

        Args:
            seed (int, optional): シード値を指定.Noneでランダムなシード地. Defaults to None.
        """
        rng = np.random.default_rng(seed)
        self.patterns.append(rng.integers(0, 2, width*height).reshape(height, width).tolist())
        
    def generate_identity_matrix_pattern(self, size: int):
        """単位行列型の抜き型を生成します

        Args:
            size (int): 抜き型の縦/横幅
        """
        self.patterns.append([[1 if i == j else 0 for i in range(size)] for j in range(size)])
    
    def rotate_x_pattern(self, p: int, n: int, goal: bool=True):
        """抜き型を横(左)方向にローテートします

        Args:
            p (int): 抜き型番号
            n (int): ローテート回数
            goal (bool, optional): 対象ボードを指定.Trueでgoal/Falseでstart. Defaults to True.
        """
        for _ in range(n):
            for line in self.patterns[p]:
                tmp = line.pop(0)
                line.append(tmp)
    
    def rotate_y_pattern(self, p :int, n: int, goal: bool=True):
        """抜き型を縦(上)方向にローテートします

        Args:
            p (int): 抜き型番号
            n (int): ローテート回数
            goal (bool, optional): 対象ボードを指定.Trueでgoal/Falseでstart. Defaults to True.
        """
        for _ in range(n):
            tmp = self.patterns[p].pop(0)
            self.patterns[p].append(tmp)
            
    def _to_string(self, list2D: list[list[int]]) -> list[str]:
        return [''.join(i) for i in map(lambda x: map(str, x),list2D)]
    
    def to_json_dict(self) -> dict:
        j_dict = {
            'board': {
                'width': self.width,
                'height': self.height,
                'start': self._to_string(self.start),
                'goal': self._to_string(self.goal),
            },
            'general': {
                'n': len(self.patterns),
                'patterns': [],
            }
        }

        for p, pattern in enumerate(self.patterns):
            tmp_dict = {
                'p': 25 + p,
                'width': len(pattern[0]),
                'height': len(pattern),
                'cells' : self._to_string(pattern)
            }
            j_dict['general']['patterns'].append(tmp_dict)
    
        return j_dict
    
    def get_goal(self) ->list[str]:
        return self._to_string(self.goal)
    
    def dump(self, path: Path) -> None:
        with open(path, 'w') as f:
            json.dump(self.to_json_dict(), f, indent=4)
    
    def dumps(self) -> str:
        return json.dumps(self.to_json_dict(), indent=4)

def make_problem(width: int, height: int, method: str, seed: int, swap: bool, general_off: bool) -> Problem:
    global PATTENS_MAX_SIZE
    random.seed(seed)
    problem = Problem(width, height)

    # 盤面の生成
    match method:
        case "fill":
            problem.generate_zero_filled_board()
        case "vertical":
            problem.generate_vertical_stripe_board()
        case "horizontal":
            problem.generate_horizontal_stripe_board()
        case "diagonal":
            problem.generate_diagonal_stripe_board()
        case "random":
            problem.generate_random_board(seed)
        case _:
            raise ValueError("Not Found Command.")

    # 初期盤面(または最終盤面)のシャッフル
    problem.hash_board(hash_intensity=10, seed=seed, goal=swap)

    # 一般抜き型の生成
    if general_off:
        problem.add_original_pattern([[1, 1], [0, 0]])
    else:
        for i in range(5, 30):
            problem.add_diagonal_stripe_pattern(i, i)
        for _ in range(10):
            problem.add_random_pattern(random.randint(2, PATTENS_MAX_SIZE), random.randint(2, PATTENS_MAX_SIZE), seed)
    return problem

if __name__ == '__main__':
    main()
    