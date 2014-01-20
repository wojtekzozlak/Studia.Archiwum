import unittest
import functools
import subprocess
import time
import multiprocessing
import random
import io

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
	dev.write(s)
	dev.flush()

def device_read(dev, _bytes):
	data = []
	while len(data) < _bytes:
	  data.extend(dev.read(_bytes - len(data)))
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

def random_writes(devname, what):
	with open_device(devname, 'w') as fp:
		while what:
			idx = random.randint(1, 10000)
			to_send = str(''.join(map(chr, what[:idx])))
			device_write(fp, to_send)
			what = what[idx:]
			time.sleep(0.1)
	print 'write to %s done!' % devname

def random_reads(devname, what):
	with open_device(devname, 'r') as fp:
		while what:
			idx = min(len(what), random.randint(1, 15000))
			result = map(ord, device_read(fp, idx))
			gold_part = what[:idx]
			if result != gold_part:
				print 'FAIL', len(result), len(gold_part)
				print 'left ', len(what) - idx, 'idx: ' , idx
				common_prefix = 0
				common_suffix = 0
				for a, b in zip(result, gold_part):
					common_prefix += 1
					if a != b: break

				for a, b in zip(reversed(result), reversed(gold_part)):
					common_suffix += 1
					if a != b: break

				print len(result) - common_prefix - common_suffix, len(gold_part) - common_prefix - common_suffix
				print result[common_prefix:-common_suffix], gold_part[common_prefix:-common_suffix]

			time.sleep(0.13)
			what = what[idx:]

def read_xor_and_require(what, sleep=0):
	time.sleep(sleep)
	with open_device('xor1d', 'r', True) as fp:
		result = map(ord, device_read(fp, len(what)))
		if result != what:
			print 'FAIL!'
		else:
			print 'OK!'

class MultiTests(unittest.TestCase):
	@require_xordev
	def test_first_read_then_write(self):
		print 'test1'
		input_strings = ('abcde', 'XXXXX')

		multiprocessing.Process(target=read_xor_and_require,
				args=([ord(a) ^ ord(b) for a, b in zip(*input_strings)], ) ).start()
		time.sleep(0.3)

		with open_device('xor1s1', 'w') as fp:
			device_write(fp, input_strings[0])
		with open_device('xor1s2', 'w') as fp:
			device_write(fp, input_strings[1])

		with open_device('xor1s1', 'w') as fp:
			device_write(fp, input_strings[0])
		with open_device('xor1s2', 'w') as fp:
			device_write(fp, input_strings[1])

		p = multiprocessing.Process(target=read_xor_and_require,
				args=([ord(a) ^ ord(b) for a, b in zip(*input_strings)], ) )
		p.start()
		p.join()

	@require_xordev
	def test_first_write_a_lot_then_try_to_write_a_lot_then_read(self):
		input_strings = ('a' * 4096, 'X' * 4096)
		print 'test2'

		with open_device('xor1s1', 'w') as fp:
			device_write(fp, input_strings[0])
		with open_device('xor1s2', 'w') as fp:
			device_write(fp, input_strings[1])

		p = multiprocessing.Process(target=read_xor_and_require,
				args=([ord(a) ^ ord(b) for a, b in zip(*input_strings)], 1) )
		p.start()

		with open_device('xor1s1', 'w') as fp:
			device_write(fp, input_strings[0])
		with open_device('xor1s2', 'w') as fp:
			device_write(fp, input_strings[1])

		p2 = multiprocessing.Process(target=read_xor_and_require,
				args=([ord(a) ^ ord(b) for a, b in zip(*input_strings)], ) )
		p2.start()
		p.join()
		p2.join()
		print 'test2 done'

	@require_xordev
	def test_mixed_reads_and_writes(self):
		print 'test3'
		inp_set = range(ord('a'), ord('z'))
		input_1 = [random.choice(inp_set) for x in xrange(100000)]
		input_2 = [random.choice(inp_set) for x in xrange(100000)]
		out = [a ^ b for a, b in zip(input_1, input_2)]

		p1 = multiprocessing.Process(target=random_writes, args=('xor1s1', input_1))
		p2 = multiprocessing.Process(target=random_writes, args=('xor1s2', input_2))
		p3 = multiprocessing.Process(target=random_reads, args=('xor1d', out))

		p1.start()
		p2.start()
		p3.start()

		p1.join()
		p2.join()
		p3.join()


	@require_xordev
	def test_blocked_reads(self):
		inp_set = [ord('a')] * 130
		p1 = multiprocessing.Process(target=random_writes, args=('xor1s1', inp_set))
		p2 = multiprocessing.Process(target=random_writes, args=('xor1s2', inp_set))
		r_reads = [ [0] * i for i in [5, 10, 15, 20, 25, 4, 9, 16, 26] ]
		r_processes = [
			multiprocessing.Process(target=read_bytes, args=('xor1d', rr)) 
			for rr in r_reads ]

		for p in r_processes:
			p.start();
		time.sleep(0.3)
		p1.start()
		p2.start()

		p1.join()
		p2.join()

		for p in r_processes:
			p.join()

if __name__ == '__main__':
	unittest.main()
