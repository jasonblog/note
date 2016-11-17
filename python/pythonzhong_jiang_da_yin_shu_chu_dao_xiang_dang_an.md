# Python中將打印輸出導向檔案


```py
#! /usr/bin/python
# -*- coding: utf-8 -*-
import sys
import codecs

# make a copy of original stdout route
stdout_backup = sys.stdout
# define the log file that receives your log info
log_file = codecs.open("message.log", "w", "utf-8")
# redirect print output to log file
sys.stdout = log_file

print u"你Now all print info will be written to message.log"
# any command line that you will execute

log_file.close()
# restore the output to initial pattern
sys.stdout = stdout_backup

print "Now this will be presented on screen"
```