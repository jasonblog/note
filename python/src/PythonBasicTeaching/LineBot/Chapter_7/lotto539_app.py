# -*- coding:utf-8-*-

import requests
from datetime import datetime

from flask import Flask, request, abort
import json, time
import pandas as pd

from linebot import (
    LineBotApi, WebhookHandler
)
from linebot.exceptions import (
    InvalidSignatureError
)
from linebot.models import *

app = Flask(__name__)

# Channel Access Token
line_bot_api = LineBotApi('YOUR_CHANNEL_ACCESS_TOKEN')
# Channel Secret
handler = WebhookHandler('YOUR_CHANNEL_SECRET')
user_id = 'user_id'


# 監聽所有來自 /callback 的 Post Request
@app.route("/", methods = ['POST'])
def callback():
    # get X-Line-Signature header value
    signature = request.headers['X-Line-Signature']
    app.logger.info("signature : %s" % signature)
    # get request body as text
    body = request.get_data(as_text = True)
    app.logger.info("Mark body: %s" % body)
    # handle webhook body
    try:
        handler.handle(body, signature)
    except InvalidSignatureError:
        abort(400)
    return 'ok'


@handler.add(MessageEvent)
def handle_message_test(event):
    # get user id when reply
    MessageType, UserId = getEventsData(event)  # 取得使用者的發送訊息類型以及Id
    getSendMessage = getUserSendMessage(event, message_type = MessageType)  # 回傳使用者輸入的文字訊息，如果不是文字就回傳None
    line_bot_api.reply_message(event.reply_token, TextSendMessage(getSendMessage))


def get539Data():
    try:
        readData = pd.read_html('https://www.pilio.idv.tw/lto539/list.asp', header=0)[3]  # 取得網頁上的表格資訊
        isFormat = readData.loc[1][0][0:9]  # 判斷格式使用
    except:
        readData = pd.read_html('https://www.pilio.idv.tw/lto539/list.asp', header=0)[2]  # 取得網頁上的表格資訊
        isFormat = readData.loc[1][0][0:9]  # 判斷格式使用
    app.logger.info("get539Data: %s" % readData.loc[0])
    lotto539Value = readData.loc[0].values.tolist()
    get539 = str('台灣彩卷539\n最新開獎日期: %s\n中獎號碼: %s' %
                 (lotto539Value[0], lotto539Value[1]))
    return str(get539)


def getEventsData(event):
    """
    取得使用者輸入的訊息類型是什麼以及使用者的Id
    將這兩個參數值回傳
    """
    getMessageType = event.message.type
    getUserId = event.source.user_id
    app.logger.info("getMessageType: %s" % getMessageType)
    app.logger.info("getUserId: %s" % getUserId)
    return getMessageType, getUserId


def getUserSendMessage(event, message_type):
    # 使用者符合文字類型會將資訊回傳，如果不符合就會回傳None
    reply_message_str = '你要查最新539號碼嗎？請輸入\"539\"\n此機器人目前只能做以上功能，請見諒！'
    if message_type == 'text':
        app.logger.info("message content: %s" % event.message.text)
        if '539' in event.message.text:
            return get539Data()
    return reply_message_str

import os

if __name__ == "__main__":
    port = int(os.environ.get('PORT', 5000))
    app.run(host = '0.0.0.0', port = port, debug = True)
