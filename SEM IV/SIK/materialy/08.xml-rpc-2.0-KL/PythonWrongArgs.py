#!/usr/bin/python

import xmlrpclib

server = xmlrpclib.Server("http://localhost:5777/")

result = server.sample.sumAndDifference("aaa",3)
print "Sum: ", result['sum']
print "Difference: ", result['difference']
