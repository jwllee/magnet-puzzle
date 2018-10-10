#!/usr/bin/env bash

for i in $(seq 1 6)
do
  echo "deprecated test_0$i.txt"
  /usr/bin/time -f "%E" ./solver ./tests/deprecated/test_0$i.txt
done

for i in $(seq 1 5)
do
  echo "test_0$i.txt"
  /usr/bin/time -f "%E" ./solver ./tests/test_0$i.txt
done

for i in $(seq 6 9)
do
  echo "test_0$i.txt"
  /usr/bin/time -f "%E" ./solver ./tests/Lunatic/test_0$i.txt
done

