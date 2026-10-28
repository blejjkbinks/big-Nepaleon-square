# usage: python3 -I drip.py <map> <n> | ./bsq   feeds a map through a pipe n bytes at a time
import sys, os, time
d = open(sys.argv[1], 'rb').read(); n = int(sys.argv[2])
for k in range(0, len(d), n):
    os.write(1, d[k:k+n]); time.sleep(0.0005)
