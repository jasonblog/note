import requests
from flask import Flask, request, abort
import json, time
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

    getUserDisplayName, getUserId, getPictureUrl, getStatusMessage = getUserProfile(user_id = UserId)  # 取得使用者的個人資訊
    # 將以上取得的參數統整到 TextSendMessage內
    SendYouData = TextSendMessage(text = '---以下是你的個人資料---\n'
                                         '傳送的訊息類型：%s \n'
                                         '發送的訊息或是ID為: %s \n'
                                         'User ID： %s \n'
                                         '名稱是: %s \n'
                                         '大頭照URL: %s \n'
                                         '狀態: %s' % (MessageType, getSendMessage, getUserId,
                                                     getUserDisplayName, getPictureUrl, getStatusMessage))
    # 最後透過自動回覆的方法也就是reply_message，將SendYouData回傳到Line Bot訊息內
    line_bot_api.reply_message(event.reply_token, SendYouData)


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


def getUserProfile(user_id):
    """
    :param user_id: 再使用者輸入文字後，會取得到user_id用再這裏
    目的是可以透過 user_id 取得到其他資訊 名稱、大頭貼url、個人狀態訊息
    """
    profile = line_bot_api.get_profile(user_id)
    getUserDisplayName = profile.display_name
    getUserId = profile.user_id
    getPictureUrl = profile.picture_url
    getStatusMessage = profile.status_message
    app.logger.info("display_name: %s" % getUserDisplayName)
    app.logger.info("user_id: %s" % getUserId)
    app.logger.info("picture_url: %s" % getPictureUrl)
    app.logger.info("status_message: %s" % getStatusMessage)
    return getUserDisplayName, getUserId, getPictureUrl, getStatusMessage


def getUserSendMessage(event, message_type):
    # 使用者符合文字類型會將資訊回傳，如果不符合就會回傳None
    if message_type == 'text':
        app.logger.info("message content: %s" % event.message.text)
        return event.message.text
    return None


def PATH(p):
    return os.path.abspath(os.path.join(os.path.dirname(__file__), p))


def json_parser(raw_data):
    # json -> dict of python
    json_data = json.loads(raw_data)
    return json_data


def getJsonToDictData():
    # 取得 json
    read_package = open(str(PATH('./join_list.json')), "r")
    raw_data = read_package.read()
    read_package.close()
    # 將 json to dict
    package_dict = json_parser(raw_data)
    return package_dict

import os

if __name__ == "__main__":
    port = int(os.environ.get('PORT', 5000))
    app.run(host = '0.0.0.0', port = port, debug = True)
