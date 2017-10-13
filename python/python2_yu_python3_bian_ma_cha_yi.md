# python2 與 python3 編碼差異


## python3 編碼 預設的  str  是 unicode

```py
#!/usr/bin/python
# -*- coding: UTF-8 -*-

print(type("中文"))
print(type("中文".encode("utf-8")))
print(type("中文"))

print(repr("中文").encode("utf-8"))
print(len("中文"))
print(repr("中文"))
```



##python2 編碼 預設 str 是 byte 

```py
#!/usr/bin/python
# -*- coding: UTF-8 -*-

print type("中文")
print type("中文".decode("utf-8"))
print type(u"中文")

print len("中文")
print repr("中文")
print len("中文".decode("utf-8"))
print repr("中文".decode("utf-8"))
print len(u"中文")
print repr(u"中文")
```
