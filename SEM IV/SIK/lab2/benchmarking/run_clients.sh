LOOP=$2
PORT=$1
REPS=$3
echo "Running $LOOP clients on port $PORT"
for i in `seq $LOOP`
do
	./client $PORT $REPS &
done
