from telegram import InlineKeyboardButton, InlineKeyboardMarkup
import telegram
from flask import Flask, request
from telegram.ext import Dispatcher, CommandHandler, MessageHandler, Filters
import logging

# 你的機器人token
bot = telegram.Bot('YOUR_TOKEN')

# 印出log的方法
logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger()

# 有Debug 以及 Info 模式，因為我不需要印太多資訊只需要Info就好
# logger.setLevel(logging.DEBUG)
logger.setLevel(logging.INFO)

# Initial Flask app
app = Flask(__name__)


# 這塊程式碼，我無法說明，我只知道一定要這樣做才能確保你的機器人能夠接收到最新的資訊
# 也許官網有寫 哈 我沒特別查 當然的/hook是可以改的，無太大影響
@app.route('/hook', methods=['POST'])
def hook():
    """Set route /hook with POST method will trigger this method."""
    if request.method == "POST":
        update = telegram.Update.de_json(request.get_json(force=True), bot)
        dispatcher.process_update(update)
    return 'ok'


def start(bot, update):
    """
    對 Bot 說 /start
    他會回你 I'm a bot, please talk to me!

    接著跳出網站連結搭配四個按鈕，跟之前文章一樣，我只是複製過來使用
    """
    update.message.reply_text("I'm a bot, please talk to me!")

    reply_markup = InlineKeyboardMarkup([[
        InlineKeyboardButton('Mark 粉絲專頁', url = 'https://github.com/mzshieh/pa19spring'),
        InlineKeyboardButton('Mark 部落格教學文章', url = 'https://python-telegram-bot.readthedocs.io/en/stable/index.html'),
        InlineKeyboardButton('Google 首頁', url = 'https://www.google.com.tw/')]])
    # 需要有4個參數值 1.聊天室的id 2.發送訊息的標題 3. 回覆訊息的id, 4 回覆的內容
    bot.send_message(update.message.chat.id, '網站連結', reply_to_message_id = update.message.message_id,
                     reply_markup = reply_markup)


def echo(bot, update):
    """
     簡稱自動回話，也就是你打什麼，他就回你什麼
    """
    text = update.message.text  # 取得對話的內容
    update.message.reply_text(text)  # 回覆你輸入的內容


# 這是一個呼叫bot的方式，但跟之前寫的呼叫bot的有作用，至於這兩種呼叫bot有什麼差別，我還要再查查看
dispatcher = Dispatcher(bot, None)

start_handler = CommandHandler('start', start)  # 對bot輸入 /start 會執行這塊
echo_handler = MessageHandler(Filters.text, echo)  # 當你輸入 hi 機器人就會回你 hi

dispatcher.add_handler(start_handler)  # 將剛剛的 /start功能加入到你的 bot內
dispatcher.add_handler(echo_handler)  # 也將剛剛自動回覆的功能加到你的 bot內

if __name__ == "__main__":
    # Running server
    app.run(debug=True)
