# steepandcheap alerts


```py
import urllib2
import smtplib
from time import sleep, strftime, localtime

from bs4 import BeautifulSoup

GMAIL_PW = "wouldn't you like to know"
GMAIL_UN = "ajq5623@gmail.com"


class Member():
    def __init__(self, name, phone, carrier, keywords):
        self.name = name
        self.phone = phone
        self.carrier = carrier
        self.kws = keywords.strip().split(',')

    def get_sms_tag(self):
        handles = {'Verison': '@vtext.com', 'ATT': '@txt.att.net', 'TMOBILE': '@tmomail.net', 'Sprint': '@messaging.sprintpcs.com', 'VIRGIN': '@vmobl.com'}
        return handles[self.carrier]

    def __str__(self):
        return '%s - KW: %s, CELL: %s' % (self.name, self.kws, self.phone)


def inWord(keyword, title):
    words = title.split(' ')
    for w in words:
        if keyword.lower() == w.lower():
            return True
    return False

steapandcheap_url = 'http://www.steepandcheap.com/'

previous_title = ""

# Start polling
while True:
    # Get html form steapandcheap.com
    html = urllib2.urlopen(steapandcheap_url).read()
    sc_soup = BeautifulSoup(html)
    title = sc_soup.title.string

    # Make sure we are not still on the previous deal
    if (title != previous_title):
        # Open xml and get members
        members_soup = BeautifulSoup(open("members.xml", "r"))

        for m in members_soup.findAll('member'):
            member = Member(m.member_name.string, m.phone.string, m.carrier.string, m.keywords.string)

            match = False

            # Check if any keywords are in the title
            for kw in member.kws:
                if inWord(kw, title):
                    match = True

            # Do work based on if match was found
            if match:
                sms_email = member.phone + member.get_sms_tag()
                message = title + " " + steapandcheap_url

                log_name = "sc_log_" + member.name + ".txt"
                log_file = open(log_name, "a+")
                log_file.write("------------------------------------------------------------------------------\n")
                log_file.write(strftime("%a, %d %b %Y %H:%M:%S", localtime()) + "\n")
                log_file.write(title.encode('utf-8') + "\n")
                log_file.write('Found match! Sending sms to ' + sms_email + '...\n')

                server = smtplib.SMTP("smtp.gmail.com", 587)
                server.starttls()
                server.login(GMAIL_UN, GMAIL_PW)
                server.sendmail('SAC Alert', sms_email, message)
                server.quit()

                log_file.write('Done sending\n')
                log_file.write("------------------------------------------------------------------------------\n")
                log_file.close()

    previous_title = title

    sleep(300)
```

