#!/usr/bin/env bash

SRC_DIR=./Programa
TESTS_DIR=${SRC_DIR}/tests
RESULTS_DIR=./results

# remove directory if it exists
if [ -d ${RESULTS_DIR} ]; then
  rm -r ${RESULTS_DIR}
fi

mkdir ${RESULTS_DIR}

N=6

for i in $(seq 0 $N)
do
  echo "Testing test_0$i.txt"
  /usr/bin/time -a -o ${RESULTS_DIR}/time-0$i.txt -f "Command: %C\n[mm:ss.ms]: %E" ${SRC_DIR}/solver ${TESTS_DIR}/test_0$i.txt >> ${RESULTS_DIR}/stdout.txt

  mv ./output.txt ${RESULTS_DIR}/output-0$i.txt
done

echo "All done!"
