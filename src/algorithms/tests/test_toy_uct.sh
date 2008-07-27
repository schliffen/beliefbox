#! /bin/bash

#rm -f results
for gamma in  0.99999 #0.1 0.5 0.9 0.99 0.999 0.9999
do
    for iter in  1 2 3 4 #64 #1 2 3 4 5 6 7 8 9 10 11 12 16 32 64
    do
        time ./bin/toy_uct_stopping_problem $gamma $iter 0 >>results
        tail -n 1 results
    done
done