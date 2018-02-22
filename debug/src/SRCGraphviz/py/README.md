Python Call Graph Generator

`virtualenv venv`  
`sudo apt-get install graphviz graphviz-dev`  
`source ./venv/bin/activate`  
`pip3 freeze > requirements.txt`

Insert the following lines into the breakpoint
```py
from CallGraph import CallGraph
CallGraph()
```

Then run the python program to generate the call graph
