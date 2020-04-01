from flask import Flask, request, abort

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


# @app.route("/push_function/<string:push_text_str>")
# def push_message(push_text_str):
#     line_bot_api.push_message(user_id, TextSendMessage(text=push_text_str))
#

# 監聽所有來自 /callback 的 Post Request
@app.route("/callback", methods=['POST'])
def callback():
    # get X-Line-Signature header value
    signature = request.headers['X-Line-Signature']
    # get request body as text
    body = request.get_data(as_text=True)
    app.logger.info("Request body: " + body)
    # handle webhook body
    try:
        handler.handle(body, signature)
    except InvalidSignatureError:
        abort(400)
    return 'OK'


# # 處理訊息
# @handler.add(MessageEvent, message=TextMessage)
# def handle_message(event):
#     message = TextSendMessage(text=event.message.text)
#     line_bot_api.reply_message(event.reply_token, message)


@app.route('/')
def index():
    return 'Hello World'


# 當機器人加入到一個群組，第一次顯示的訊息
@handler.add(JoinEvent)
def handle_join(event):
    welcome_message = "大家好！我是Line Bot，請多多指教"  # 可以修改bot進到群組時，出現的字串
    line_bot_api.reply_message(event.reply_token, TextMessage(text=welcome_message))
    print("加入的事件: %s" % JoinEvent)


@handler.add(LeaveEvent)
def handle_leave(event):
    print("離開 事件: %s" % event)
    print("離開事件的資訊: %s" % event.source)


import os

if __name__ == "__main__":
    port = int(os.environ.get('PORT', 5000))
    app.run(host='0.0.0.0', port=port)
