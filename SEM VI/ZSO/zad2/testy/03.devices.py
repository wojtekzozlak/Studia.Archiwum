import unittest
import functools
import subprocess
import time
import multiprocessing
import random
import io
import sys

def require_xordev(f):
  @functools.wraps(f)
  def wrapper(*args, **kwargs):
    subprocess.check_call(['insmod', '../xordev.ko'])
    try:
      f(*args, **kwargs)
    finally:
      subprocess.check_call(['rmmod', 'xordev'])
  return wrapper

def open_device(dev, w, use_io=False):
  if use_io:
    o = lambda f, m: io.open(f, m, buffering=0)
  else:
    o = open
  return o('/dev/%s' % dev, '%sb' % w)

def device_write(dev, s):
  i = dev.write(s)
  dev.flush()
  return i

debugs = []

def device_read(dev, _bytes):
  data = []
  global debugs
  debugs = []
  while len(data) < _bytes:
    p = dev.read(_bytes - len(data))
    data.extend(p)
    debugs.append(len(p))
    
  return data

def read_bytes(dev, what):
  with open_device(dev, 'r', True) as fp:
    print 'read: ', len(what)
    data = device_read(fp, len(what))
    print 'GOT :', len(data)
    data = map(ord, data)
    if data != what:
      print 'FAIL!'
    else:
      print 'OK!'

def random_writes(devname, what, id = 0):
  with open_device(devname, 'w', True) as fp:
    bytes = 0
    while what:
      idx = random.randint(1, 10000)
      to_send = str(''.join(map(chr, what[:idx])))
      idx = device_write(fp, to_send)
      what = what[idx:]
      #time.sleep(0.05)
      bytes += idx 

#
#def random_writes(devname, what):
#  with open_device(devname, 'w') as fp:
#    while what:
#      idx = random.randint(1, 10000)
#      to_send = str(''.join(map(chr, what[:idx])))
#      device_write(fp, to_send)
#      what = what[idx:]
#      time.sleep(0.1)
#

def random_reads(devname, what, id = 0):
  with open_device(devname, 'r', True) as fp:
    bytes = 0
    while what:
      idx = min(len(what), random.randint(1, 15000))
      result = map(ord, device_read(fp, idx))
      idx = len(result)
      gold_part = what[:idx]
      if result != gold_part:
        print 'FAIL', len(result), len(gold_part)
        print debugs
        print 'res', result, gold_part
      #time.sleep(0.07)
      what = what[idx:]

      bytes += len(result)

#
#def random_reads(devname, what):
#  with open_device(devname, 'r') as fp:
#    while what:
#      idx = min(len(what), random.randint(1, 15000))
#      result = map(ord, device_read(fp, idx))
#      gold_part = what[:idx]
#      if result != gold_part:
#        print 'FAIL', result, gold_part
#      time.sleep(0.13)
#      what = what[idx:]

def read_xor_and_require(what, sleep=0):
  time.sleep(sleep)
  with open_device('xor1d', 'r', True) as fp:
    result = map(ord, device_read(fp, len(what)))
    if result != what:
      print 'FAIL!'
    else:
      print 'OK!'

class MultiDevicesTests(unittest.TestCase):
  @require_xordev
  def test_random_reads_writes(self):
    print 'test3'
    inp_set = range(ord('a'), ord('z'))
    input_1 = [random.choice(inp_set) for x in xrange(100000)]
    input_2 = [random.choice(inp_set) for x in xrange(100000)]
    input_3 = [random.choice(inp_set) for x in xrange(100000)]
    out_1 = [a ^ b for a, b in zip(input_1, input_2)]
    out_2 = [a ^ b for a, b in zip(input_1, input_3)]
    out_3 = [a ^ b for a, b in zip(input_3, input_2)]

    p = [
     multiprocessing.Process(target=random_writes, args=('xor1s1', input_1)),
     multiprocessing.Process(target=random_writes, args=('xor1s2', input_2)),
     multiprocessing.Process(target=random_reads, args=('xor1d', out_1)),

     multiprocessing.Process(target=random_writes, args=('xor2s1', input_1)),
     multiprocessing.Process(target=random_writes, args=('xor2s2', input_3)),
     multiprocessing.Process(target=random_reads, args=('xor2d', out_2)),

     multiprocessing.Process(target=random_writes, args=('xor0s1', input_3)),
     multiprocessing.Process(target=random_writes, args=('xor0s2', input_2)),
     multiprocessing.Process(target=random_reads, args=('xor0d', out_3))
     ]

    for pp in p:
      pp.start()

    for pp in p:
      pp.join()

  @require_xordev
  def test_random_concurrent(self):
    print '4',
    size = 10000
    input_a = [ord('a')] * size
    input_b = [ord('b')] * size
    input_c = [ord('c')] * size
    input_d = [ord('d')] * size

    out_1 = [a ^ b for a, b in zip(input_a, input_b)]
    out_2 = [a ^ b for a, b in zip(input_b, input_c)]
    out_3 = [a ^ b for a, b in zip(input_c, input_d)]

    p = [
     multiprocessing.Process(target=random_writes, args=('xor1s1', input_a)),
     multiprocessing.Process(target=random_writes, args=('xor1s2', input_b)),
     multiprocessing.Process(target=random_reads, args=('xor1d', out_1, 1)),

     multiprocessing.Process(target=random_writes, args=('xor1s1', input_a)),
     multiprocessing.Process(target=random_writes, args=('xor1s2', input_b)),
     multiprocessing.Process(target=random_reads, args=('xor1d', out_1, 2)),

     multiprocessing.Process(target=random_writes, args=('xor1s1', input_a)),
     multiprocessing.Process(target=random_writes, args=('xor1s2', input_b)),
     multiprocessing.Process(target=random_reads, args=('xor1d', out_1, 3)),

     multiprocessing.Process(target=random_writes, args=('xor2s1', input_b)),
     multiprocessing.Process(target=random_writes, args=('xor2s2', input_c)),
     multiprocessing.Process(target=random_reads, args=('xor2d', out_2, 4)),

     multiprocessing.Process(target=random_writes, args=('xor2s1', input_b)),
     multiprocessing.Process(target=random_writes, args=('xor2s2', input_c)),
     multiprocessing.Process(target=random_reads, args=('xor2d', out_2, 5)),

     multiprocessing.Process(target=random_writes, args=('xor0s1', input_c, 6)),
     multiprocessing.Process(target=random_writes, args=('xor0s2', input_d, 6)),
     multiprocessing.Process(target=random_reads, args=('xor0d', out_3, 6)),

     multiprocessing.Process(target=random_writes, args=('xor0s1', input_c, 7)),
     multiprocessing.Process(target=random_writes, args=('xor0s2', input_d, 7)),
     multiprocessing.Process(target=random_reads, args=('xor0d', out_3, 7))
     ]

    for pp in p:
      pp.start()

    for pp in p:
      pp.join()



if __name__ == '__main__':
  unittest.main()
