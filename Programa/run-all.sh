#!/usr/bin/env bash

TIME_FILE=$1
TIME_LIMIT=60

for i in $(seq 1 6)
do
  if [ $i == 4 ]; then
    continue
  fi

  echo "o_test_0$i"
  timeout ${TIME_LIMIT}s /usr/bin/time -f "time: %E" -a -o $TIME_FILE ./solver ./tests/deprecated/test_0$i.txt
  echo ""
done

for i in $(seq 1 5)
do
  echo "test_0$i"
  timeout ${TIME_LIMIT}s /usr/bin/time -f "time: %E" -a -o $TIME_FILE ./solver ./tests/test_0$i.txt
  echo ""
done

for i in $(seq 6 9)
do
  echo "test_0$i"
  timeout ${TIME_LIMIT}s /usr/bin/time -f "time: %E" -a -o $TIME_FILE ./solver ./tests/Lunatic/test_0$i.txt
  echo ""
done

