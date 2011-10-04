from execute import execute

MSG_SIZE = 19
next_port = 6666

def run_test(clients, msg_num, tries, programs, i):
    tries = 1
    print 'echo "RUNNING TEST %d:"' % i
    print 'echo "- clients: %d"' % clients
    print 'echo "- messages: %d"' % msg_num
    print 'echo "- tries: %d"' % tries
    print 'echo ""'

    port = globals()['next_port']
    globals()['next_port'] += 1
    def test_program(program):
        print 'echo "Testing %s..."' % program
        print './compile.sh %s %d' % (program, clients * tries)
        print 'time ./run.sh %d %d %d %d %s &> /dev/null' % (port, clients, msg_num, tries, program)
 #       print 'echo "readed `cat result` out of %d"' % (MSG_SIZE * msg_num * clients)
        print 'echo ""'

    for program in programs:
         test_program(program)
    print 'echo "--------"'
    print 'echo ""'


print "ulimit -n 12000"
print "ulimit -u 12000"

programs = ['server_libevent_no_mutex', 'server_fork']
tests = [ (100, 1, 10), (100, 10, 10), (500, 10, 5), (500, 100, 5), (1000, 10, 5), (5000, 10, 1), (10000, 10, 1)]

for i, (c, m, t) in enumerate(tests):
    run_test(c, m, t, programs, i)

