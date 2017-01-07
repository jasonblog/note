# Audio 利用ffmpeg 編碼轉換


```py
# -*- coding: utf-8 -*-
#!/usr/bin/python

import os
import shlex
from subprocess import check_call

for filename in os.listdir("./"):
    if filename.endswith(".wav"):
        #body, ext = os.path.splitext(filename)
        command = 'ffmpeg -i ' + filename + ' tmp_file.wav' 
        check_call(shlex.split(command))
        command = 'mv' + ' tmp_file.wav ' + filename
        check_call(shlex.split(command))
```