#! /bin/sh

for i in $(seq 0.0 0.01 1); do
    ./a.out $i >> result.csv
done
