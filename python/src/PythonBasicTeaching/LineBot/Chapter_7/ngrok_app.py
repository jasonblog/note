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
    line_bot_api.reply_message(event.reply_token, TextSendMessage(text = "Test"))


import os

if __name__ == "__main__":
    port = int(os.environ.get('PORT', 5000))
    app.run(host = '0.0.0.0', port = port, debug = True)
