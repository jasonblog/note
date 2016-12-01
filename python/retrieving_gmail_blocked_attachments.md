# Retrieving Gmail blocked attachments

> Extract attachments from emails that Gmail doesn't allow you to download. This is dumb. (Only tested with Python 3.4)


- extract-attachments.py

```py
#!/usr/bin/env python3
# Get your files that Gmail block. Warning message:
# "Anti-virus warning - 1 attachment contains a virus or blocked file. Downloading this attachment is disabled."
# Based on: http://spapas.github.io/2014/10/23/retrieve-gmail-blocked-attachments/
# Go to your emails, click the arrow button in the top right, "Show original", save to the same directory as this script.

import email
import sys
import os

if __name__ == '__main__':
  if len(sys.argv) < 2:
    print("Press enter to process all files with .txt extension.")
    input()
    files = [ f for f in os.listdir('.') if os.path.isfile(f) and f.endswith('.txt') ]
  else:
    files = sys.argv[1:]

  print("Files: %s" % ', '.join(files))
  print()

  for f in files:
    msg = email.message_from_file(open(f))
    print("Processing %s" % f)
    print("Subject: %s" % msg['Subject'])
    for pl in msg.get_payload():
      fn_header = pl.get_filename()
      if fn_header:
        fn_data = email.header.decode_header(fn_header)
        (fn_str, fn_charset) = fn_data[0]
        if isinstance(fn_str, str):
          fn = fn_str
        else:
          fn = fn_str.decode(fn_charset)
        print("Found %s" % fn)
        if os.path.isfile(fn):
          print("The file %s already exists! Press enter to overwrite." % fn)
          input()
        open(fn, 'wb').write(pl.get_payload(decode=True))
    print()

```


```
./extract-attachments.py xxxx.gmail
```
