# campsaver.py


```py
#!/usr/bin/env python3
# Mail service should only be run from a system with postfix installed.
# e.g. sudo apt-get install postfix on Ubuntu Xenial
import logging
import requests
from bs4 import BeautifulSoup
# For mail notification service
import os
import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText

CAMPSAVER_URL = 'https://www.campsaver.com/'
ITEM_SOTO_OD_1RX = 'soto-od-1rx-windmaster-stove-with-4-flex-pot-support.html'
THRESHOLD_PRICE = 60.0
THRESHOLD_PERCENTAGE = 0.75
SENT_FROM = 'yaoshihyu@gmail.com'
SENT_TO = 'yaoshihyu@gmail.com'

SMTPSERVER = os.environ.get('SMTP_SERVER', 'localhost')

ERROR_MSG = """
ERROR!
The email for this job was supposed to be located in {} but that file
wasn't generated for some reason. This almost never happens, so
look for something farther up in the job related to this subject for
clues as to what might have happened
"""

format_str = "[ %(funcName)s() ] %(message)s"
logging.basicConfig(level=logging.INFO, format=format_str)


def mailit(subject, content_text, sent_from, sent_to):
    msg = MIMEMultipart()

    body = MIMEText(content_text)
    msg.attach(body)

    msg['Subject'] = subject
    msg['From'] = sent_from
    msg['To'] = sent_to

    s = smtplib.SMTP(SMTPSERVER)
    s.send_message(msg)
    s.quit()


def is_threshold_price(source, target):
    logging.debug("source is %f" % source)
    logging.debug("target is %f" % target)
    if source < target or source == target:
        return True
    else:
        return False


def is_threshold_percentage(original, sale, target):
    sale_percentage = float(sale) / original
    logging.debug(sale_percentage)
    logging.debug(target)
    return is_threshold_price(sale_percentage, target)


def is_threshold(commodity):
    price_original = commodity['price_original']
    price_sale = commodity['price_sale']
    flag_price = is_threshold_price(price_sale, THRESHOLD_PRICE)
    logging.debug("flag_price is %r" % flag_price)
    flag_percentage = is_threshold_percentage(price_original, price_sale, THRESHOLD_PERCENTAGE)
    logging.debug("flag_percentage is %r" % flag_percentage)
    return flag_price or flag_percentage


def get_web_page(url, item_entry):
    resp = requests.get(url + item_entry)
    if resp.status_code != 200:
        print('Invalid url:', resp.url)
        return None
    else:
        return resp.text


def get_commodity_info(dom):
    soup = BeautifulSoup(dom, 'html5lib')
    commodity = dict()
    commodity['name'] = soup.title.text.strip()
    prices_dict = soup.find(id='fancy-options').span.attrs
    commodity['price_original'] = float(prices_dict['data-variant-price'])
    commodity['price_sale'] = float(prices_dict['data-variant-sale'])
    # commodity['current_price'] = soup.find(id='fancy-options').span.contents[1].contents[0].split('$')[1]
    commodity['notification'] = is_threshold(commodity)
    return commodity


if __name__ == '__main__':
    page = get_web_page(CAMPSAVER_URL, ITEM_SOTO_OD_1RX)
    if page:
        commodity = get_commodity_info(page)
        # show summary
        logging.debug(commodity['name'])
        for k, v in commodity.items():
            if k != 'name':
                logging.debug("%s %s" % (k, v))

        # nofity the team if the price hits the targets
        if commodity['notification']:
            logging.info('Hit the target price. Mail notification.')
            subject = commodity['name'] + ' hits the target price'
            content_text = """
Your commodity meets your expectation {} or {} of the original price.
Click the link to go to the commodity page:
"""
            content_text += CAMPSAVER_URL + ITEM_SOTO_OD_1RX
            content_text.format(THRESHOLD_PRICE, THRESHOLD_PERCENTAGE)
            mailit(subject, content_text, SENT_FROM, SENT_TO)
        else:
            logging.info('Does not hit the target price. Do nothing.')
    else:
        # if there is no page, nofity maintainers
        logging.warning('No page. Mail notification.')
        mailit("No Campsaver watching page", CAMPSAVER_URL + ITEM_SOTO_OD_1RX, SENT_FROM, SENT_TO)

```