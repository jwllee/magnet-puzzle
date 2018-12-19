#!/usr/bin/env bash

TIME_LIMIT=60s

for i in $(seq 1 6)
do
  echo "deprecated test_0$i.txt"
  timeout $TIME_LIMIT /usr/bin/time -f "%E" valgrind ./solver ./tests/deprecated/test_0$i.txt
done

for i in $(seq 1 5)
do
  echo "test_0$i.txt"
  timeout $TIME_LIMIT /usr/bin/time -f "%E" valgrind ./solver ./tests/test_0$i.txt
done

for i in $(seq 6 9)
do
  echo "test_0$i.txt"
  timeout $TIME_LIMIT /usr/bin/time -f "%E" valgrind ./solver ./tests/Lunatic/test_0$i.txt
done

