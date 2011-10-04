#!/usr/bin/python

import xmlrpclib

server = xmlrpclib.Server("http://localhost:5777/")

multicall = xmlrpclib.MultiCall(server)
multicall.sample.sumAndDifference(4,4)
multicall.sample.sumAndDifference(9, 7)
result1, result2 = multicall()


print "Sum1: ", result1['sum']
print "Difference1: ", result1['difference']

print "Sum2: ", result2['sum']
print "Difference2: ", result2['difference']

