ulimit -n 12000
ulimit -u 12000
echo "RUNNING TEST 0:"
echo "- clients: 100"
echo "- messages: 1"
echo "- tries: 1"
echo ""
echo "Testing server_libevent_no_mutex..."
./compile.sh server_libevent_no_mutex 100
time ./run.sh 6666 100 1 1 server_libevent_no_mutex &> /dev/null
echo ""
echo "Testing server_fork..."
./compile.sh server_fork 100
time ./run.sh 6666 100 1 1 server_fork &> /dev/null
echo ""
echo "--------"
echo ""
echo "RUNNING TEST 1:"
echo "- clients: 100"
echo "- messages: 10"
echo "- tries: 1"
echo ""
echo "Testing server_libevent_no_mutex..."
./compile.sh server_libevent_no_mutex 100
time ./run.sh 6667 100 10 1 server_libevent_no_mutex &> /dev/null
echo ""
echo "Testing server_fork..."
./compile.sh server_fork 100
time ./run.sh 6667 100 10 1 server_fork &> /dev/null
echo ""
echo "--------"
echo ""
echo "RUNNING TEST 2:"
echo "- clients: 500"
echo "- messages: 10"
echo "- tries: 1"
echo ""
echo "Testing server_libevent_no_mutex..."
./compile.sh server_libevent_no_mutex 500
time ./run.sh 6668 500 10 1 server_libevent_no_mutex &> /dev/null
echo ""
echo "Testing server_fork..."
./compile.sh server_fork 500
time ./run.sh 6668 500 10 1 server_fork &> /dev/null
echo ""
echo "--------"
echo ""
echo "RUNNING TEST 3:"
echo "- clients: 500"
echo "- messages: 100"
echo "- tries: 1"
echo ""
echo "Testing server_libevent_no_mutex..."
./compile.sh server_libevent_no_mutex 500
time ./run.sh 6669 500 100 1 server_libevent_no_mutex &> /dev/null
echo ""
echo "Testing server_fork..."
./compile.sh server_fork 500
time ./run.sh 6669 500 100 1 server_fork &> /dev/null
echo ""
echo "--------"
echo ""
echo "RUNNING TEST 4:"
echo "- clients: 1000"
echo "- messages: 10"
echo "- tries: 1"
echo ""
echo "Testing server_libevent_no_mutex..."
./compile.sh server_libevent_no_mutex 1000
time ./run.sh 6670 1000 10 1 server_libevent_no_mutex &> /dev/null
echo ""
echo "Testing server_fork..."
./compile.sh server_fork 1000
time ./run.sh 6670 1000 10 1 server_fork &> /dev/null
echo ""
echo "--------"
echo ""
echo "RUNNING TEST 5:"
echo "- clients: 5000"
echo "- messages: 10"
echo "- tries: 1"
echo ""
echo "Testing server_libevent_no_mutex..."
./compile.sh server_libevent_no_mutex 5000
time ./run.sh 6671 5000 10 1 server_libevent_no_mutex &> /dev/null
echo ""
echo "Testing server_fork..."
./compile.sh server_fork 5000
time ./run.sh 6671 5000 10 1 server_fork &> /dev/null
echo ""
echo "--------"
echo ""
echo "RUNNING TEST 6:"
echo "- clients: 10000"
echo "- messages: 10"
echo "- tries: 1"
echo ""
echo "Testing server_libevent_no_mutex..."
./compile.sh server_libevent_no_mutex 10000
time ./run.sh 6672 10000 10 1 server_libevent_no_mutex &> /dev/null
echo ""
echo "Testing server_fork..."
./compile.sh server_fork 10000
time ./run.sh 6672 10000 10 1 server_fork &> /dev/null
echo ""
echo "--------"
echo ""
