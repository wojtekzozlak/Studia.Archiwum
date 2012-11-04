import unittest
import functools
import subprocess
import time

def require_xordev(f):
	@functools.wraps(f)
	def wrapper(*args, **kwargs):
		subprocess.check_call(['insmod', '../xordev.ko'])
		try:
			f(*args, **kwargs)
		finally:
			subprocess.check_call(['rmmod', 'xordev'])
	return wrapper

def open_device(dev, w):
	return open('/dev/%s' % dev, '%sb' % w)

def device_write(dev, s):
	dev.write(s)
	dev.flush()

def device_read(dev, _bytes):
	return dev.read(_bytes)

class BaseTests(unittest.TestCase):
	@require_xordev
	def test_simple_read_write_bytes(self):
		input_strings = [('abcd', 'XYZX'), ('XYZ', 'abc'), ('aa', 'aa')]

		for inp in input_strings:
			with open_device('xor0s1', 'w') as fp:
				device_write(fp, inp[0])
			with open_device('xor0s2', 'w') as fp:
				device_write(fp, inp[1])
			with open_device('xor0d', 'r') as fp:
				result = map(ord, device_read(fp, len(inp[0])))
				expected_output = [ ord(a) ^ ord(b) for a, b in zip(*inp) ]
				self.assertEquals(expected_output, result)

	@require_xordev
	def test_overlapping_writes(self):
		input_strings = [('aaa', 'b'), ('aaa', 'b'), ('a', 'xxxxxx'), ('b', '')]
		b = ord('a') ^ ord('b')
		x = ord('a') ^ ord('x')
		bx = ord('b') ^ ord('x')
		output_strings = [(b,), (b,), (x,x,x,x,x), (bx,)]

		with open_device('xor1s1', 'w') as s1:
			with open_device('xor1s2', 'w') as s2:
				with open_device('xor1d', 'r') as dest:
					for inp, out in zip(input_strings, output_strings):
						device_write(s1, inp[0])
						device_write(s2, inp[1])
						result = tuple(map(ord, device_read(dest, len(out))))
						self.assertEquals(result, out)
#
#	def test_simple_read_write_bytes_in_loop(self):
#		for i in xrange(10):
#			self.test_simple_read_write_bytes()
#
#	def test_overlapping_writes_loop(self):
#		for i in xrange(10):
#			self.test_overlapping_writes()
#
#	@require_xordev
#	def test_non_blocking_writes(self):
#		self.non_blocking_writes_helper()
#
#	@require_xordev
#	def test_buffer_reset(self):
#		for i in xrange(10):
#			self.non_blocking_writes_helper()
#
#	def non_blocking_writes_helper(self):
#		with open_device('xor1s1', 'w') as s1:
#			device_write(s1, 'a' * 2048)
#			device_write(s1, 'a' * 2048)
#
#		with open_device('xor1s2', 'w') as s2:
#			device_write(s2, 'X' * 2048)
#			device_write(s2, 'X' * 2048)
#		with open_device('xor1d', 'r') as dest:
#			results = device_read(dest, 4096)
#			self.assertEquals('9' * 4096, results)
#
#	def test_non_blocking_writes_in_loop(self):
#		for i in xrange(10):
#			self.test_non_blocking_writes()
#
if __name__ == '__main__':
	unittest.main()

