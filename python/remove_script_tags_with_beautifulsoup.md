# remove script tags with BeautifulSoup


```py
#! /usr/bin/python
# -*- coding: utf-8 -*-
from bs4 import BeautifulSoup
soup = BeautifulSoup('<script>a</script>baba<script>b</script>', 'lxml')
[s.extract() for s in soup('script')]
print soup
```

- result

```sh
<html><head></head><body><p>baba</p></body></html>
```