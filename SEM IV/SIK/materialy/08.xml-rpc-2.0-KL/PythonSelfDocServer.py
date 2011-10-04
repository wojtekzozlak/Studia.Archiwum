#!/usr/bin/python

from DocXMLRPCServer import DocXMLRPCServer


def sumAndDifference(x,y):
    """This method returns sum and difference of arguments
       as struct with two fields: 'sum' and 'difference'"""
    
    return {'sum':x+y,'difference':x-y}

server = DocXMLRPCServer(("localhost", 5777))
server.register_function(sumAndDifference,"sample.sumAndDifference")
server.register_introspection_functions()
server.register_multicall_functions()
server.serve_forever()
