# usage: python3 -I fuzz.py <bsq binary> [count] [seed]
# random small maps, some corrupted, compared against ref.py's solve(),
# as a file argument and through stdin
import os, random, subprocess, sys, tempfile, importlib.util
here = os.path.dirname(os.path.abspath(__file__))
spec = importlib.util.spec_from_file_location('ref', os.path.join(here, 'ref.py'))
ref = importlib.util.module_from_spec(spec)
args = sys.argv[1:]
spec.loader.exec_module(ref)
binary, count, seed = args[0], int(args[1]) if len(args) > 1 else 2000, int(args[2]) if len(args) > 2 else 1
rng = random.Random(seed)
printable = [chr(c) for c in range(32, 127)]

def make_map():
    x, y = rng.randint(1, 30), rng.randint(1, 30)
    e, o, f = rng.sample(printable, 3) if rng.random() < 0.3 else ('.', 'o', 'x')
    d = rng.choice([0, 5, 20, 50, 90, 100])
    rows = [''.join(o if rng.random() * 100 < d else e for _ in range(x)) for _ in range(y)]
    return (f'{y}{e}{o}{f}\n' + ''.join(r + '\n' for r in rows)).encode()

def corrupt(data):
    k = rng.randrange(len(data))
    kind = rng.choice(['change', 'delete', 'insert', 'truncate', 'extra_row'])
    if kind == 'change':
        return data[:k] + bytes([rng.randrange(256)]) + data[k + 1:]
    if kind == 'delete':
        return data[:k] + data[k + 1:]
    if kind == 'insert':
        return data[:k] + bytes([rng.choice(b'.ox\n')]) + data[k:]
    if kind == 'truncate':
        return data[:k]
    return data + data.split(b'\n')[1] + b'\n'

fails = 0
with tempfile.TemporaryDirectory() as tmp:
    path = os.path.join(tmp, 'map')
    for n in range(count):
        data = make_map()
        if rng.random() < 0.4:
            data = corrupt(data)
        expected = ref.solve(data)
        expected = b'map error\n' if expected is None else expected
        open(path, 'wb').write(data)
        for mode in ('file', 'stdin'):
            if mode == 'file':
                p = subprocess.run([binary, path], capture_output=True)
            else:
                p = subprocess.run([binary], input=data, capture_output=True)
            got = p.stdout + p.stderr
            if got != expected or p.returncode != 0:
                fails += 1
                if fails <= 3:
                    print(f'FAIL #{n} ({mode}): input {data!r}\n  expected {expected!r}\n  got {got!r}')
print(f'{count} random maps, {fails} failures')
