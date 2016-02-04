# 打印log

```c
fprintf (stderr, "YAO This is line %d of file %s (function %s)\n", __LINE__, __FILE__, __FUNCTION__);

```


- 減少log數次

```c
unsigned int count = 0;
if (count % 10 == 0) {
   fprintf (stderr, "YAO This is line %d of file %s (function %s)\n", __LINE__, __FILE__, __FUNCTION__);
    count++;
}
    
```

- kernel 

```c
dump_stack()


dump_stack不准确的原因分析

kernel panic后打印的堆栈信息是调用dump_stack函数获得的。而dump_stack的原理是遍历堆栈，把所有可能是内核函数的内容找出来，并打印对应的函数。因为函数调用时会把下一条指令的地址放到堆栈中。所以只要找到这些return address，就可以找到这些return address所在函数，进而打印函数的调用关系。 
     但是dump_stack可能不准确，可能的原因有三： 
     1.所有这些可以找到的函数地址，存在/proc/kallsyms中。它并不包括内核中所有的函数，而只包括内核中stext~etext和sinittext~einittext范围的函数，及模块中的函数。详细可参考scripts/kallsyms.c 
     2.一些函数在编译时进行了优化，把call指令优化为jmp指令，这样在调用时就不会把return address放到堆栈，导致dump_stack时在堆栈中找不到对应的信息。 
     3.堆栈中可能有一些数值，它们不是return address，但是在内核函数地址的范围里，这些数值会被误认为return address从而打印出错误的调用关系。 
     
```


```py
#! /usr/bin/python
# -*- coding: utf-8 -*-
import os
import sys

def InsertToFunc(lines, funcLines, FileName):
    #print lines
    #print funcLines

    for i in funcLines:
        for j in range(i-1, len(lines)):
            if str(lines[j]).find(';') != -1:
                break

            if str(lines[j]).find('{') != -1:
                if str(lines[j]).find('}') != -1:
                    break

                if str(FileName).find('.java') != -1:
                    lines.insert(j+1, '\tSlog.d(Thread.currentThread().getStackTrace()[2].getClassName(), "YAO ["+ Thread.currentThread().getStackTrace()[2].getMethodName() +" | "+Thread.currentThread().getStackTrace()[2].getFileName()+":"+Thread.currentThread().getStackTrace()[2].getLineNumber()+"]");\n');
                elif str(FileName).find('.cpp') != -1 or str(FileName).find('.c') != -1:
                    #lines.insert(j+1, '\t::printf ("This is line %d of file %s (function %s)\\n", __LINE__, __FILE__, __func__);\n')
                    lines.insert(j+1, '\tALOGW("YAO This is line %d of file %s (function %s)\\n", __LINE__, __FILE__, __func__);\n')

                break
    return lines

def main():
    FileName = sys.argv[1]
    print FileName
 
    if str(FileName).find('.java') != -1:
        os.system("ctags-exuberant -x " + FileName + " | ack -o -w 'method\s+.*' | ack -o '\d+\s+.*' | ack -o '^\d+\s+' | sort -k 1 -nr > /tmp/test.txt")
    elif str(FileName).find('.cpp') != -1 or str(FileName).find('.c') != -1:
        os.system("ctags-exuberant -x " + FileName + " | ack -o -w 'function\s+.*' | ack -o '\d+\s+.*' | ack -o '^\d+\s+' | sort -k 1 -nr > /tmp/test.txt")
    else:
        print 'noknow type'
        sys.exit(0)

    f = open('/tmp/test.txt', 'r+')
    #funcLines = map(int, funcLines)
    funcLines = [int(i)for i in f.read().splitlines()]
    f.close()

    f = open(str(FileName), 'r+')
    lines = [i for i in f.read().splitlines()]
    f.close()

    InsertListfinish = InsertToFunc(lines, funcLines, FileName)

    newfile = open(str(FileName), "w+")
    newfile.truncate()

    for l in InsertListfinish:
        newfile.write(l + '\n')

    newfile.close()

if __name__=='__main__':
    if len(sys.argv) > 1:
        main()
    else:
        pass
        print "please input python test.py filename"


```