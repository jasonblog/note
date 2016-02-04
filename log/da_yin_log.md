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


dump_stack不準確的原因分析

kernel panic後打印的堆棧信息是調用dump_stack函數獲得的。而dump_stack的原理是遍歷堆棧，把所有可能是內核函數的內容找出來，並打印對應的函數。因為函數調用時會把下一條指令的地址放到堆棧中。所以只要找到這些return address，就可以找到這些return address所在函數，進而打印函數的調用關係。 
     但是dump_stack可能不準確，可能的原因有三： 
     1.所有這些可以找到的函數地址，存在/proc/kallsyms中。它並不包括內核中所有的函數，而只包括內核中stext~etext和sinittext~einittext範圍的函數，及模塊中的函數。詳細可參考scripts/kallsyms.c 
     2.一些函數在編譯時進行了優化，把call指令優化為jmp指令，這樣在調用時就不會把return address放到堆棧，導致dump_stack時在堆棧中找不到對應的信息。 
     3.堆棧中可能有一些數值，它們不是return address，但是在內核函數地址的範圍裡，這些數值會被誤認為return address從而打印出錯誤的調用關係。 
     
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