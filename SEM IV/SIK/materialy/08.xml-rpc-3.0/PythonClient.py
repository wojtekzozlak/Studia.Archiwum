#!/usr/bin/python

import xmlrpclib

server = xmlrpclib.Server("http://localhost:5763/")

result = server.sample.sumAndDifference(5,3)
print "Sum: ", result['sum']
print "Difference: ", result['difference']
