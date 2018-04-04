#!/bin/bash
gcc -O2 ls.c
sudo cp a.out /
cd /
./a.out -Rl
