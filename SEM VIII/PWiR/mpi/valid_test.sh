for I in {2..20}
do
  echo "TESTING N=$I"
  ./laplace-seq.exe --verbose $I > out_seq
  for P in {1..8}
  do
    echo "  P=$P"
    mpirun -np $P ./laplace-par.exe --verbose $I > out
    diff out out_seq
  done
done
