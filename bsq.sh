#!/bin/bash

make
./map.pl "$@" > map
./bsq map
make fclean
rm map

