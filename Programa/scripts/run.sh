#!/usr/bin/env bash


TESTS_DIR=./tests
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
  /usr/bin/time -v -a -o ${RESULTS_DIR}/time-0$i.txt ./solver ${TESTS_DIR}/test_0$i.txt >> ${RESULTS_DIR}/stdout.txt

  mv output.txt ${RESULTS_DIR}/output-0$i.txt
done

echo "All done!"
