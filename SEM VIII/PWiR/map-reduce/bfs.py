import subprocess
import sys

def execute(command):
  print command
  process = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True)
  result = process.stdout.read()
  process.stdout.close()
  return result


def print_usage():
  print "here be usage"



if (len(sys.argv) < 4):
  print_usage()
  sys.exit()



node = long(sys.argv[1])
out_dir = sys.argv[2]
in_dir = sys.argv[3]

count = ""
if (len(sys.argv) > 4):
  count = sys.argv[4]



# create step0b
print "Creating initial step"
execute("echo \"%s\t0 false\" > .step0b" % ((node * -1) - 1))
execute("bin/hadoop dfs -copyFromLocal .step0b /user/mimuw/graph-tmp/step0b/step0b")
execute("rm .step0b")

# run job
print "Starting map-reduce procedure"
execute("bin/hadoop jar BFS.jar BFS %s %s %s" % (in_dir, out_dir, count))

# clean
print "Cleaning"
execute("bin/hadoop dfs -rmr /user/mimuw/graph-tmp")
