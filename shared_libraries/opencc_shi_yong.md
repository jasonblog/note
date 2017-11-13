# Opencc 使用


```sh
https://github.com/BYVoid/OpenCC
```


```sh
cmake -DCMAKE_INSTALL_PREFIX="/home/shihyu/.mybin/opencc" \
-DCMAKE_INSTALL_RPATH="/home/shihyu/.mybin/opencc/lib" ../

make && make install
```


```py
# -*- coding: utf-8 -*-
# Author: bl_indie
# This script uses OpenCC as translate engine
# https://github.com/BYVoid/OpenCC
# License: MIT
# -----------------------------------
# s2t.json Simplified Chinese to Traditional Chinese 簡體到繁體
# t2s.json Traditional Chinese to Simplified Chinese 繁體到簡體
# s2tw.json Simplified Chinese to Traditional Chinese (Taiwan Standard) 簡體到臺灣正體
# tw2s.json Traditional Chinese (Taiwan Standard) to Simplified Chinese 臺灣正體到簡體
# s2hk.json Simplified Chinese to Traditional Chinese (Hong Kong Standard) 簡體到香港繁體（香港小學學習字詞表標準）
# hk2s.json Traditional Chinese (Hong Kong Standard) to Simplified Chinese 香港繁體（香港小學學習字詞表標準）到簡體
# s2twp.json Simplified Chinese to Traditional Chinese (Taiwan Standard) with Taiwanese idiom 簡體到繁體（臺灣正體標準）並轉換為臺灣常用詞彙
# tw2sp.json Traditional Chinese (Taiwan Standard) to Simplified Chinese with Mainland Chinese idiom 繁體（臺灣正體標準）到簡體並轉換為中國大陸常用詞彙
# t2tw.json Traditional Chinese (OpenCC Standard) to Taiwan Standard 繁體（OpenCC 標準）到臺灣正體
# t2hk.json Traditional Chinese (OpenCC Standard) to Hong Kong Standard 繁體（OpenCC 標準）到香港繁體（香港小學學習字詞表標準）
# -----------------------------------

import os
import re

# Translation config
config = "s2tw.json"
# Regular expression matching, ignore case
pattern = re.compile('.md$|.lua$|.php$|.json$', re.I)
# Directories where your text you want to translate exist
dirs = ["/home/shihyu/design-pattern-java"]

def transFile(fileName, config):
    cmdT = 'echo {I} | opencc -c {C}'
    cmd = cmdT.format(I = fileName, C = config)
    Out_fileName = os.popen(cmd).read() 
    #print Out_fileName

    cmdT = 'opencc -i {I} -o {O} -c {C}'
    cmd = cmdT.format(I = fileName, O = Out_fileName, C = config)
    os.popen(cmd)

    '''
    cmdT = 'rm {I}'
    cmd = cmdT.format(I = fileName)
    os.popen(cmd)
    '''

def judgeFile(fileName):
    if pattern.search(fileName):
        print "Match! " + fileName
        transFile(fileName, config)

def walkDir(rootDir):
    list_dirs = os.walk(rootDir)
    for root, dirs, files in list_dirs:
        for d in dirs:
            print os.path.join(root, d)
        for f in files:
            judgeFile(os.path.join(root, f))

for dir in dirs:
    walkDir(dir)

```