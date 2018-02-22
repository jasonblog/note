C/C++ Call Graph Generator

install `graphviz, graphviz-dev, pygraphviz`

link `instrument.c` with your C/C++ application  
with `-g -finstrument-functions` compiling options

then run your application to generate the trace file

`python CallGraph.py [tracefile exe]` to generate the call graph