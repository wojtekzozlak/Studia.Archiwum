LOOP=$2
PORT=$1
REPS=$3
TRIES=$4
PROGRAM=$5
echo "Running program on port $PORT"
./$PROGRAM $PORT &
PID=$!
echo "Time for initialization"
sleep 0.2

echo "Running $LOOP clients on port $PORT"
for i in `seq $LOOP`
do
	./client $PORT $REPS $TRIES &
done

wait $PID
echo "test done"
