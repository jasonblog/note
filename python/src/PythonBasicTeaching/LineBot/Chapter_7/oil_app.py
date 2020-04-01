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


def getOilData():
    try:
        readData = pd.read_html('https://www2.moeaboe.gov.tw/oil102/oil2017/A01/A0108/tablesprices.asp', header=0)[1]  # 取得網頁上的表格資訊
    except:
        readData = pd.read_html('https://www2.moeaboe.gov.tw/oil102/oil2017/A01/A0108/tablesprices.asp', header=0)[0]  # 取得網頁上的表格資訊
    app.logger.info("getOilData: %s" % readData[0:2])
    oilValue = readData.loc[1].values.tolist()
    getOil = str('油品供應商: %s\n98無鉛汽油: %s\n95無鉛汽油: %s\n92無鉛汽油: %s\n超(高)級柴油: %s\n計價單位: %s\n施行日期: %s' %
                 (oilValue[0], oilValue[1], oilValue[2], oilValue[3], oilValue[4], oilValue[5], oilValue[6][0:10]))
    return getOil


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
    reply_message_str = '你要查最新油價嗎？請輸入\"油價\"\n此機器人目前只能做以上功能，請見諒！'
    if message_type == 'text':
        app.logger.info("message content: %s" % event.message.text)
        if '油價' in event.message.text:
            return getOilData()
    return reply_message_str

import os

if __name__ == "__main__":
    port = int(os.environ.get('PORT', 5000))
    app.run(host = '0.0.0.0', port = port, debug = True)
