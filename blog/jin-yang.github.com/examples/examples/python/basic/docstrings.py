#!/usr/bin/env python
"""
This is an example for python docstring, including modules,
    functions, classes, methods, etc.
"""

import math
print math.__doc__         # standard modules' docstring.
print str.__doc__          # standard function docstring.

print '#' * 65

def foo():
    """It's just a foo() function."""
    pass
class foobar(object):
    """It's just a foobar class."""
    def bar(self):
        """It's just a foobar.bar() function."""
        pass
    def get_doc(self):
        return self.__doc__
f = foobar()
print f.__doc__
print f.get_doc()
print __doc__
print foo.__doc__
print foobar.__doc__
print foobar.bar.__doc__

print '#' * 65

#from pydoc import help
#def foobar():
#    """
#    It's just a foobar function.
#    """
#    return
#help(foobar)
