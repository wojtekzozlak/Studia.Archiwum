#!/usr/bin/python

import xmlrpclib
import pprint

server = xmlrpclib.Server("http://localhost:5777/")

result = server.system.listMethods()
pprint.pprint(result)
for method in result:
  print server.system.methodHelp(method)
  print server.system.methodSignature(method)
