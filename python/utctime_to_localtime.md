# UTC to Local Datetime



```python
import time
from datetime import datetime

import pytz # $ pip install pytz
from tzlocal import get_localzone # $ pip install tzlocal

# get local timezone    
local_tz = get_localzone()

# test it
# utc_now, now = datetime.utcnow(), datetime.now()
ts = time.time()
print(ts)
utc_now, now = datetime.utcfromtimestamp(ts), datetime.fromtimestamp(ts)

local_now = utc_now.replace(tzinfo=pytz.utc).astimezone(local_tz) # utc -> local
print(local_now)
```

