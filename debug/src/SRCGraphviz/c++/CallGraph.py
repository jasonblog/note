import os
import sys
import tempfile
import webbrowser
import pygraphviz as pgv
import pdb

def addr2sym(addr, exe):
    cmd = "addr2line " + addr + " -e " + exe + "  -f | c++filt"
    return os.popen(cmd).read().split()

def src_lineno_process(pos, isCallSite):
    src, lineno = pos.split(':')
    src = src.split( os.getcwd()+'/' )[-1]
    if(isCallSite):
        lineno = str( int(lineno)-1 )
    return [src, lineno]

def parseTrace(tracefile, exe):
    curdir = os.getcwd()
    callStack = []

    with open(tracefile , 'r') as trace:
        for record in trace.readlines():
            fnAddr, callSiteAddr = record.split()

            fn, fn_pos = addr2sym(fnAddr, exe)
            fn_pos = src_lineno_process(fn_pos, False)

            callsite, callsite_pos = addr2sym(callSiteAddr, exe)
            callsite_pos = src_lineno_process(callsite_pos, True)

            callStack.append([callsite, callsite_pos, fn, fn_pos])
    return callStack


def callGraphDraw(callStack):
    G = pgv.AGraph(strict=False, directed=True)

    node_set = set()
    subgraph_set = {}

    for index, \
        [ callsite, [callsite_src, callsite_lineno], \
          fn, [fn_src, fn_lineno] ] \
        in enumerate(callStack):

        node = '{0}:{1}'.format(fn_src, fn)
        if node not in node_set:
            node_set.add(node)
            if fn_src not in subgraph_set:
                subgraph_set[fn_src] = G.add_subgraph(
                    name = 'cluster' + fn_src,
                    label = fn_src
                )
            subgraph = subgraph_set[fn_src]
            subgraph.add_node(
                node,
                label='{0}:{1}'.format(fn, fn_lineno)
            )

        callsite_node = '{0}:{1}'.format(callsite_src, callsite)
        if callsite_node not in node_set:
            continue

        if index == 1:
            color = 'green'
        elif index == len(callStack) - 1:
            color = 'red'
        else:
            color = 'black'

        # import pdb;pdb.settrace()
        G.add_edge(
            '{0}:{1}'.format(callsite_src, callsite),
            '{0}:{1}'.format(fn_src, fn),
            color = color,
            ltail = subgraph_set[fn_src].name,
            lhead = subgraph_set[callsite_src].name,
            label = '#{0} at {1}'.format(index, callsite_lineno)
        )

    fd, name = tempfile.mkstemp('.png')

    G.draw(name, prog='dot')
    G.close()

    webbrowser.open('file://' + name)

if __name__ == "__main__":
    tracefile = sys.argv[1]
    exe = sys.argv[2]
    callGraphDraw( parseTrace(tracefile, exe) )