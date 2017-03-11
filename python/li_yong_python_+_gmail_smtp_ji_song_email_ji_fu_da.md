# 利用PYTHON + GMAIL SMTP 寄送EMAIL及附檔 CATEGORIES


```py
#!/usr/bin/env python

# USAGE: ./sendgmail.py path_to_filename [assigned_attachment_name]
# if not assign assigned_attachment_name, the attachment is named as original path_to_filename

from email.mime.text import MIMEText
from email.mime.application import MIMEApplication
from email.mime.multipart import MIMEMultipart
from smtplib import SMTP
import smtplib
import sys

sender = 'pttcrawle@gmail.com'
passwd = '' # 2l
receivers = ['yaoshihyu@gmail.com','shihyu.yao@gmail.com']

emails = [elem.strip().split(',') for elem in receivers]
msg = MIMEMultipart()
msg['Subject'] = "{YOUR_MAIL_SUBJECT}"
msg['From'] = sender
msg['To'] = ','.join(receivers)

msg.preamble = 'Multipart massage.\n'
part = MIMEText("{YOUR_MAIL_CONTENT}")
msg.attach(part)

part = MIMEApplication(open(str(sys.argv[1]),"rb").read())
if len(sys.argv) > 2:
    attachname = str(sys.argv[2])
else:
    attachname = str(sys.argv[1])

part.add_header('Content-Disposition', 'attachment', filename=attachname)
msg.attach(part)

smtp = smtplib.SMTP("smtp.gmail.com:587")
smtp.ehlo()
smtp.starttls()
smtp.login(sender, passwd)

smtp.sendmail(msg['From'], emails , msg.as_string())
print 'Send mails to',msg['To']
```

## Change account access for less secure apps

Go to the "[Less secure apps](https://www.google.com/settings/security/lesssecureapps)" section in My Account.

Next to "Access for less secure apps," select Turn on. (Note to G Suite users: This setting is hidden if your administrator has locked less secure app account access.)