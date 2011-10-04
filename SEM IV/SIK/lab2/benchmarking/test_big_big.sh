ulimit -n 12000
ulimit -u 12000
echo "RUNNING TEST 0:"
echo "- clients: 100"
echo "- messages: 1"
echo ""
echo "Testing server_libevent..."
./compile.sh server_libevent 100000
time ./run.sh 6666 100 1 1000 server_libevent &> /dev/null
echo ""
echo "Testing server_poll..."
./compile.sh server_poll 100000
time ./run.sh 6666 100 1 1000 server_poll &> /dev/null
echo ""
echo "Testing server_libevent_no_mutex..."
./compile.sh server_libevent_no_mutex 100000
time ./run.sh 6666 100 1 1000 server_libevent_no_mutex &> /dev/null
echo ""
echo "Testing server_fork..."
./compile.sh server_fork 100000
time ./run.sh 6666 100 1 1000 server_fork &> /dev/null
echo ""
echo "--------"
echo ""
echo "RUNNING TEST 1:"
echo "- clients: 100"
echo "- messages: 10"
echo ""
echo "Testing server_libevent..."
./compile.sh server_libevent 100000
time ./run.sh 6667 100 10 1000 server_libevent &> /dev/null
echo ""
echo "Testing server_poll..."
./compile.sh server_poll 100000
time ./run.sh 6667 100 10 1000 server_poll &> /dev/null
echo ""
echo "Testing server_libevent_no_mutex..."
./compile.sh server_libevent_no_mutex 100000
time ./run.sh 6667 100 10 1000 server_libevent_no_mutex &> /dev/null
echo ""
echo "Testing server_fork..."
./compile.sh server_fork 100000
time ./run.sh 6667 100 10 1000 server_fork &> /dev/null
echo ""
echo "--------"
echo ""
echo "RUNNING TEST 2:"
echo "- clients: 100"
echo "- messages: 1000"
echo ""
echo "Testing server_libevent..."
./compile.sh server_libevent 100000
time ./run.sh 6668 100 1000 1000 server_libevent &> /dev/null
echo ""
echo "Testing server_poll..."
./compile.sh server_poll 100000
time ./run.sh 6668 100 1000 1000 server_poll &> /dev/null
echo ""
echo "Testing server_libevent_no_mutex..."
./compile.sh server_libevent_no_mutex 100000
time ./run.sh 6668 100 1000 1000 server_libevent_no_mutex &> /dev/null
echo ""
echo "Testing server_fork..."
./compile.sh server_fork 100000
time ./run.sh 6668 100 1000 1000 server_fork &> /dev/null
echo ""
echo "--------"
echo ""
echo "RUNNING TEST 3:"
echo "- clients: 500"
echo "- messages: 10"
echo ""
echo "Testing server_libevent..."
./compile.sh server_libevent 250000
time ./run.sh 6669 500 10 500 server_libevent &> /dev/null
echo ""
echo "Testing server_poll..."
./compile.sh server_poll 250000
time ./run.sh 6669 500 10 500 server_poll &> /dev/null
echo ""
echo "Testing server_libevent_no_mutex..."
./compile.sh server_libevent_no_mutex 250000
time ./run.sh 6669 500 10 500 server_libevent_no_mutex &> /dev/null
echo ""
echo "Testing server_fork..."
./compile.sh server_fork 250000
time ./run.sh 6669 500 10 500 server_fork &> /dev/null
echo ""
echo "--------"
echo ""
echo "RUNNING TEST 4:"
echo "- clients: 500"
echo "- messages: 1000"
echo ""
echo "Testing server_libevent..."
./compile.sh server_libevent 250000
time ./run.sh 6670 500 1000 500 server_libevent &> /dev/null
echo ""
echo "Testing server_poll..."
./compile.sh server_poll 250000
time ./run.sh 6670 500 1000 500 server_poll &> /dev/null
echo ""
echo "Testing server_libevent_no_mutex..."
./compile.sh server_libevent_no_mutex 250000
time ./run.sh 6670 500 1000 500 server_libevent_no_mutex &> /dev/null
echo ""
echo "Testing server_fork..."
./compile.sh server_fork 250000
time ./run.sh 6670 500 1000 500 server_fork &> /dev/null
echo ""
echo "--------"
echo ""
echo "RUNNING TEST 5:"
echo "- clients: 2000"
echo "- messages: 10"
echo ""
echo "Testing server_libevent..."
./compile.sh server_libevent 1000000
time ./run.sh 6671 2000 10 500 server_libevent &> /dev/null
echo ""
echo "Testing server_poll..."
./compile.sh server_poll 1000000
time ./run.sh 6671 2000 10 500 server_poll &> /dev/null
echo ""
echo "Testing server_libevent_no_mutex..."
./compile.sh server_libevent_no_mutex 1000000
time ./run.sh 6671 2000 10 500 server_libevent_no_mutex &> /dev/null
echo ""
echo "Testing server_fork..."
./compile.sh server_fork 1000000
time ./run.sh 6671 2000 10 500 server_fork &> /dev/null
echo ""
echo "--------"
echo ""
echo "RUNNING TEST 6:"
echo "- clients: 2000"
echo "- messages: 1000"
echo ""
echo "Testing server_libevent..."
./compile.sh server_libevent 1000000
time ./run.sh 6672 2000 1000 500 server_libevent &> /dev/null
echo ""
echo "Testing server_poll..."
./compile.sh server_poll 1000000
time ./run.sh 6672 2000 1000 500 server_poll &> /dev/null
echo ""
echo "Testing server_libevent_no_mutex..."
./compile.sh server_libevent_no_mutex 1000000
time ./run.sh 6672 2000 1000 500 server_libevent_no_mutex &> /dev/null
echo ""
echo "Testing server_fork..."
./compile.sh server_fork 1000000
time ./run.sh 6672 2000 1000 500 server_fork &> /dev/null
echo ""
echo "--------"
echo ""
echo "RUNNING TEST 7:"
echo "- clients: 4000"
echo "- messages: 10"
echo ""
echo "Testing server_libevent..."
./compile.sh server_libevent 1600000
time ./run.sh 6673 4000 10 400 server_libevent &> /dev/null
echo ""
echo "Testing server_poll..."
./compile.sh server_poll 1600000
time ./run.sh 6673 4000 10 400 server_poll &> /dev/null
echo ""
echo "Testing server_libevent_no_mutex..."
./compile.sh server_libevent_no_mutex 1600000
time ./run.sh 6673 4000 10 400 server_libevent_no_mutex &> /dev/null
echo ""
echo "Testing server_fork..."
./compile.sh server_fork 1600000
time ./run.sh 6673 4000 10 400 server_fork &> /dev/null
echo ""
echo "--------"
echo ""
echo "RUNNING TEST 8:"
echo "- clients: 4000"
echo "- messages: 1000"
echo ""
echo "Testing server_libevent..."
./compile.sh server_libevent 1600000
time ./run.sh 6674 4000 1000 400 server_libevent &> /dev/null
echo ""
echo "Testing server_poll..."
./compile.sh server_poll 1600000
time ./run.sh 6674 4000 1000 400 server_poll &> /dev/null
echo ""
echo "Testing server_libevent_no_mutex..."
./compile.sh server_libevent_no_mutex 1600000
time ./run.sh 6674 4000 1000 400 server_libevent_no_mutex &> /dev/null
echo ""
echo "Testing server_fork..."
./compile.sh server_fork 1600000
time ./run.sh 6674 4000 1000 400 server_fork &> /dev/null
echo ""
echo "--------"
echo ""
echo "RUNNING TEST 9:"
echo "- clients: 10000"
echo "- messages: 10"
echo ""
echo "Testing server_libevent..."
./compile.sh server_libevent 2500000
time ./run.sh 6675 10000 10 250 server_libevent &> /dev/null
echo ""
echo "Testing server_poll..."
./compile.sh server_poll 2500000
time ./run.sh 6675 10000 10 250 server_poll &> /dev/null
echo ""
echo "Testing server_libevent_no_mutex..."
./compile.sh server_libevent_no_mutex 2500000
time ./run.sh 6675 10000 10 250 server_libevent_no_mutex &> /dev/null
echo ""
echo "Testing server_fork..."
./compile.sh server_fork 2500000
time ./run.sh 6675 10000 10 250 server_fork &> /dev/null
echo ""
echo "--------"
echo ""
