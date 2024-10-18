import re
from pathlib import Path

dir = Path(__file__).parent

reg1 = re.compile(r'#include <(.+)>')
reg2 = re.compile(r'#include "(.+)"')
headers = (set(), set())

for file in (dir / 'include').iterdir():
    with open(file, 'r', encoding='utf-8') as f:
        for line in f.readlines():
            if hit := reg1.match(line):
                headers[0].add(hit.group(1))
            elif hit := reg2.match(line):
                headers[1].add(hit.group(1))
            else:
                pass

for file in (dir / 'src').iterdir():
    with open(file, 'r', encoding='utf-8') as f:
        for line in f.readlines():
            if hit := reg1.match(line):
                headers[0].add(hit.group(1))
            elif hit := reg2.match(line):
                headers[1].add(hit.group(1))
            else:
                pass

# print('\n'.join(sorted(list(headers[0]))))

# print('\n'.join(sorted(list(headers[1]))))

with open(dir / 'headers.txt', 'w', encoding='utf-8') as f:
    f.write('\n'.join(sorted(list(headers[0]))))
    f.write('\n')
    f.write('\n'.join(sorted(list(headers[1]))))