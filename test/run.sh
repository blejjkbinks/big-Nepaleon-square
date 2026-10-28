#!/bin/bash
# runs every map in cases/ twice: as a file argument and through stdin
# usage: run.sh <bsq binary>   compares against ref.py; stdout+stderr merged
B=$1; T=$(dirname $0); C=$T/cases; fail=0; n=0
for f in $C/*; do
  n=$((n+1))
  exp=$(python3 -I $T/ref.py $f; echo x)
  got=$($B $f 2>&1; echo x)
  if [ "$exp" != "$got" ]; then echo "FAIL $f"; fail=$((fail+1)); fi
  got=$($B < $f 2>&1; echo x)
  if [ "$exp" != "$got" ]; then echo "FAIL(stdin) $f"; fail=$((fail+1)); fi
done
echo "$n cases, $fail failures"
