#/bin/bash

SIZES="2500,512mb,00:30:00; 5000,512mb,00:30:00; 10000,2gb,03:00:00; 15000,3gb,03:00:00; 20000,8gb,03:00:00; 25000,8gb,03:00:00;"

CONFIGURATIONS="1,1; 2,1; 4,1; 8,1; 16,1; 1,2; 1,4; 1,8; 1,16; 1,32; 1,64; 2,2; 2,4; 4,2; 8,4; 4,8; 16,8; 8,16;"


echo $SIZES | while IFS=',' read -d';' size mem time;
do
  echo $CONFIGURATIONS | while IFS=',' read -d';' nodes cores;
  do
    size=$(echo $size)
    nodes=$(echo $nodes)
    cores=$(echo $cores)
    ./gen_test.sh laplace-par.exe $size $nodes $cores "par" $mem $time > par_test/par-$size-$nodes-$cores.pbs
  done
done
#for I in $SIZES
#do
#  ./gen_test.sh laplace-seq.exe $I 1 1 > tests/test_seq_$I.pbs
#done
