#!/usr/bin/python

from SimpleXMLRPCServer import SimpleXMLRPCServer

def sumAndDifference(x,y):
    return {'sum':x+y,'difference':x-y}

server = SimpleXMLRPCServer(("localhost", 5763))
server.register_function(sumAndDifference,"sample.sumAndDifference")
server.serve_forever()
