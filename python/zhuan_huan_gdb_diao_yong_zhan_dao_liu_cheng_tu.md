# 轉換GDB調用棧到流程圖


如果你想在GDB調試時把調用堆棧保存下來歸檔，那下面這個腳本就方便你了。原理是將調用堆棧的函數抽取出來，再完成調用關係就可以了。

首先你要安裝dot (Mac OS下安裝Graphviz), 如果你想轉為文本格式，就可安裝Perl的Graph::Easy包(命令行:sudo perl -MCPAN -e 'install Graph::Easy', Ubuntu下直接安裝libgraph-easy-perl)。

然後按需要執行腳本就可以了， 假定如下的調用棧:

- stack.txt

```sh
#0  WebCore::FrameLoader::FunctionA (this=0x2a7d91f8) at /FrameLoader.cpp
#1  0x4efd2514 in WebCore::FrameLoader::FunctionB (this=0x2a7d91f8) at /FrameLoader.cpp:553
#2  0x4efd1918 in FunctionC ()at /mainFile.cpp:100
```



```sh
python convertStackToDot.py stack.txt|dot -Tpng>output.png
```

- convertStackToDot.py

```py
#!/usr/bin/python  
#coding=utf-8  
#python convertStackToDot.py stack.txt|dot -Tpng>output.png  
#To generate ascii flow chart, graph_easy should be installed:  
#  sudo apt-get install libgraph-easy-perl  
#The use below command line:  
#python convertStackToDot.py stack.txt|graph-easy -as_ascii>output.txt  
  
import sys  
import re  
import argparse  
import os  
  
function_name_str = r"[a-zA-Z][a-zA-Z0-9]*::[~a-zA-Z0-9\_\-<: >=]*\(|[a-zA-Z0-9_\-<>]* \("  
class_name_str = r"::[a-zA-Z][a-zA-Z0-9]*::"  
known_namespaces = []  
  
libCName="/system/lib/libc.so"  
  
Colors=["#000000","#ff0000","#00aa00","#0000ff","#800080","#daa520","#ff00b4","#d2691e","#00bfff",  
        "#D0A020","#006000","#305050","#101870"]  
  
Styles=["solid","dashed","dotted"]  
  
MAX_COLOR_COUNT = len(Colors)  
MAX_LINE_WIDTH_COUNT = 4  
MAX_STYLE_COUNT = len(Styles)  
  
FirstNodeAttr = ' style="rounded,filled" fillcolor=\"#00bb0050\"'  
HighlightAttr = ' style="rounded,filled" fillcolor=\"yellow\"'  
  
blockNum = 0      
nodeNo = 0  
  
nodeList={}  
nodeOrderList={}  
firstNodeList={}  
nodeAttr={}  
  
outputText = ''  
callingStack = ''  
newBlock=True  
  
willCommit=False #For filtering purpose  
blockBackTrace = ''  
blockNodeList={}  
  
def getTextOfBlockNodeList(lastNodeName,lastNodeLabel):  
    global firstNodeList  
    strBlockNodeText = ''  
  
    for key in blockNodeList.keys():  
        if not nodeList.has_key(key):  
            name = blockNodeList[key]  
            strBlockNodeText = strBlockNodeText + name + nodeAttr[name]+'\n'  
            nodeList[key] = name  
  
    #Replace the attribute of the last node  
    if len(lastNodeName)>0 and not firstNodeList.has_key(lastNodeName):  
        oldStr = lastNodeName+'[label="'+lastNodeLabel+'" shape=box ];';  
        newStr = lastNodeName+'[label="'+lastNodeLabel+'" shape=box '+FirstNodeAttr+' ];'  
        strBlockNodeText = strBlockNodeText.replace(oldStr,newStr,1)  
        firstNodeList[lastNodeName] = True  
  
    return strBlockNodeText  
  
  
def submitAndResetForNewBlock(args,lastNodeName,lastNodeLabel):  
    global blockBackTrace,newBlock,callingStack  
    global blockNodeList,willCommit,outputText  
  
    newBlock = True  
    if willCommit and len(blockBackTrace)>0:  
        callingStack = blockBackTrace + '\n' + callingStack  
        blockNodeText = getTextOfBlockNodeList(lastNodeName,lastNodeLabel)  
        outputText = outputText+blockNodeText  
  
    blockNodeList = {}  
    blockBackTrace = ''  
    willCommit = (len(args.filter)==0)  
  
def getClassName(text):  
    m = re.search(class_name_str,text)  
    if m:  
        className=text[0:m.end()-2]  
    elif not text[:text.find('::')] in known_namespaces:  
        className = text[:text.find('::')]  
    else:  
        className = text  
  
    return className  
  
def getNodeName(text,nodeNo,args):  
    global willCommit,blockNodeList,newBlock  
  
    processText = text  
  
  
    if len(args.ignore)>0 and re.search(args.ignore,text):  
        return ''   
  
    if args.onlyClass:  
        processText = getClassName(text)  
  
    if nodeList.has_key(processText):  
        nodeName = nodeList[processText]  
    elif blockNodeList.has_key(processText):  
        nodeName = blockNodeList[processText]  
    else:  
        nodeName = 'Node'+str(nodeNo)  
        blockNodeList[processText]=nodeName  
  
        extraAttr = ''  
        try:  
            if len(args.highlight)>0 and re.search(args.highlight,processText):  
                extraAttr = HighlightAttr  
        except:  
            extraAttr = ''  
  
        nodeAttr[nodeName] = '[label="'+processText+'" shape=box '+extraAttr+'];'  
  
      
    if len(args.filter)>0 and re.search(args.filter,text):  
        willCommit = True  
  
    return nodeName  
  
  
def createNewRelation(nodeName,lastNodeName,blockNum):  
    global blockBackTrace  
  
    tempKey = "%s_%s"%(nodeName,lastNodeName)  
  
    if args.duplicate or not nodeOrderList.has_key(tempKey):  
        lineColor = Colors[(blockNum-1)%MAX_COLOR_COUNT]  
        linePenWidth = str((int((blockNum-1)/MAX_COLOR_COUNT)%MAX_LINE_WIDTH_COUNT)+1)  
        lineStyle = Styles[((blockNum-1)/(MAX_COLOR_COUNT*MAX_LINE_WIDTH_COUNT))%MAX_STYLE_COUNT]  
  
        if nodeOrderList.has_key(tempKey):  
            linePenWidth = '1'  
            lineColor = lineColor+'50' #Set alpha value  
  
        blockBackTrace = nodeName+'->'+lastNodeName+'[label='+str(blockNum)+ \
                    ',color=\"'+lineColor+'\"'+ \
                    ',style=\"'+lineStyle+'\"'+ \
                    ',penwidth='+linePenWidth+']\n'+ \
                    blockBackTrace  
  
    nodeOrderList[tempKey] = True  
  
  
def combineOutputText():  
    global outputText,callingStack  
  
    if len(callingStack)>0:  
        outputText = outputText+callingStack+"\n}"  
        return outputText  
    else:  
        return ''  
  
def initialize(args):  
    global outputText,callingStack  
    outputText = "digraph backtrace{ \nnode [style=rounded  fontname=\"Helvetica Bold\"];\n" + args.extraDotOptions +"\n"  
  
def convertToDot(file,args):  
    global willCommit,outputText,newBlock,blockNum,nodeNo  
    global outputText,callingStack,blockBackTrace  
      
    lastNodeName = ''  
    lastNodeLabel = ''  
  
    willCommit = (len(args.filter)==0) #To specify the initial value according to the filter.  
  
    f = open(file, 'r')  
  
    for line in f:  
        line = line.strip()  
        if(len(line)==0) or line.startswith("#0  ") or line.startswith("#00 "):  
            if not newBlock:  
                #Start with new block here.  
                submitAndResetForNewBlock(args, lastNodeName, lastNodeLabel)  
  
            if(len(line.strip())==0):  
                continue  
  
        if not line.startswith("#"):  
            continue  
  
        text = ""  
  
        m = re.search(function_name_str, line)  
        if m:  
            nodeNo = nodeNo+1  
            text=m.group(0).strip()  
            text = text[:-1]  
            text = text.strip()  
        elif line.find(libCName)>0:  
            nodeNo = nodeNo+1  
            text='FunctionInLibC'  
  
        if(len(text)==0):  
            continue  
  
        #Get the existing node or create new one. Anyway, just ask for the name.  
        nodeName = getNodeName(text,nodeNo,args)  
  
        #To throw it away if no valid name was returned according to your arguments.  
        if(len(nodeName)==0):  
            continue  
  
        if newBlock:  
            newBlock = False  
            blockNum = blockNum + 1  
        else:  
            createNewRelation(nodeName,lastNodeName,blockNum)  
  
        lastNodeName = nodeName  
        lastNodeLabel = text  
        if args.onlyClass:  
            lastNodeLabel = getClassName(text)  
  
    if len(blockBackTrace)>0:  
        #Wow, one block was built successfully, sumit it.  
        submitAndResetForNewBlock(args, lastNodeName, lastNodeLabel)  
  
    f.close()  
  
if __name__=="__main__":  
    parser = argparse.ArgumentParser()  
    parser.add_argument('file', type=str, help='The text file which contains GDB call stacks.')  
    parser.add_argument('-e','--extraDotOptions', default='', help='Extra graph options. For example: rankdir=LR; That means to show functions in horizontal.')  
    parser.add_argument('-l','--highlight', default='', help='Regular Expression Pattern. Nodes are highlighted whose name match the pattern.')  
    parser.add_argument('-f','--filter', default='', help='Regular Expression Pattern. The calling stack are shown only if which include the matched nodes.')  
    parser.add_argument('-d','--duplicate', action='store_true', default=False, help='Leave duplicated callings.')  
    parser.add_argument('-i','--ignore', default='', help='To hide some nodes, try this.')  
    parser.add_argument('-c','--onlyClass', action='store_true', default=False, help='To simplify the output with less nodes, only Class node will be listed.')  
  
    if len(sys.argv)<=1:  
        parser.print_help()  
        print "  Any comment, please feel free to contact horky.chen@gmail.com."  
        quit()  
  
    args = parser.parse_args()  
      
    if args.file is None:  
        quit()  
  
    initialize(args)  
  
    if os.path.isfile(args.file):  
        convertToDot(args.file,args)  
    else:  
        filenames = os.listdir(args.file)  
        for filename in filenames:  
            convertToDot(os.path.join(args.file,filename),args)  
  
    resultDotString = combineOutputText()  
  
    if len(resultDotString)>0:  
        print(resultDotString)  
```