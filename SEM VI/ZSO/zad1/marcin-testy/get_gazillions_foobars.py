#!/usr/bin/env python

FOOBARS=10000

print '// Generated with a Python script'
print

for i in xrange(FOOBARS):
    print 'void foobar_%d(){' % i
    print '}'
    print

 
print 'void gazillions_foobars(){'
for i in xrange(FOOBARS):
    print '    INT_CALL(foobar_%d());' % i
print '}'

