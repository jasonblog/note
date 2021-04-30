from flask import Flask, request, abort
import json
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


@app.route("/broadcast_function/<string:broadcast_text_str>")
def broadcast_message(broadcast_text_str):
    line_bot_api.broadcast(TextSendMessage(text = broadcast_text_str))
    return 'OK'

import os

if __name__ == "__main__":
    port = int(os.environ.get('PORT', 5000))
    app.run(host = '0.0.0.0', port = port, debug = True)
