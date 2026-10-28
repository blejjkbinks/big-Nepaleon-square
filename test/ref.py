# usage: python3 -I ref.py <map>   prints what bsq should print (stdout and stderr together)
import sys
def solve(data):
    nl = data.find(b'\n')
    if nl < 0: return None
    rules = data[:nl]
    if len(rules) < 4: return None
    num, chars = rules[:-3], rules[-3:]
    if not num or not num.isdigit(): return None
    y = int(num)
    if y == 0 or y > 2147483647: return None
    e, o, f = chars[0:1], chars[1:2], chars[2:3]
    if len({e, o, f}) != 3 or any(not (32 <= c <= 126) for c in chars): return None
    lines = []
    pos = nl + 1
    for _ in range(y):
        j = data.find(b'\n', pos)
        if j < 0: return None
        lines.append(data[pos:j]); pos = j + 1
    x = len(lines[0])
    if x == 0 or any(len(l) != x for l in lines): return None
    if any(c not in (e[0], o[0]) for l in lines for c in l): return None
    best = (0, 0, 0)
    prev = [0] * (x + 1)
    for r, l in enumerate(lines):
        cur = [0] * (x + 1)
        for c in range(x):
            if l[c] != o[0]:
                cur[c+1] = 1 + min(prev[c+1], prev[c], cur[c])
                if cur[c+1] > best[0]: best = (cur[c+1], r, c)
        prev = cur
    s, br, bc = best
    out = []
    for r, l in enumerate(lines):
        row = bytearray(l)
        if br - s < r <= br:
            for c in range(bc - s + 1, bc + 1): row[c] = f[0]
        out.append(bytes(row) + b'\n')
    return b''.join(out)
if __name__ == '__main__':
    r = solve(open(sys.argv[1], 'rb').read())
    sys.stdout.buffer.write(b'map error\n' if r is None else r)
