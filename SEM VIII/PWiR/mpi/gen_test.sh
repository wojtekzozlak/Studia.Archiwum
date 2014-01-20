BINARY=$1
N=$2
NODES=$3
CORES=$4
NAME=$5
MEM=$6
TIME=$7

echo "#!/bin/tcsh"
echo "#PBS -N $NAME-$N-$NODES-$CORES"
echo "#PBS -q halo2"
echo "#PBS -A G52-5"
echo "#PBS -l nodes=$NODES:ppn=$CORES"
echo "#PBS -l mem=$MEM"
echo "#PBS -l walltime=$TIME"

# Ustaw srodowisko
echo "cd \$PBS_O_WORKDIR/"
echo "module load openmpi/1.6.3/gnu/4.4.7"

# Wykonaj zadanie
echo "echo \"Started at\" `date`"
echo "mpirun ./$BINARY $N"
echo "echo \"Finished at\" `date`"
