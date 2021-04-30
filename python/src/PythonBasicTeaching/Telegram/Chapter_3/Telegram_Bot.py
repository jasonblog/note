from telegram import InlineKeyboardButton, InlineKeyboardMarkup
from telegram.ext import Updater, CommandHandler, CallbackQueryHandler
import logging
import time
updater = Updater(token='YOUR_TOKEN', use_context=False)

# 印出log的方法
logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger()

# 有Debug 以及 Info 模式，因為我不需要印太多資訊只需要Info就好
# logger.setLevel(logging.DEBUG)
logger.setLevel(logging.INFO)


def getUrl(bot, update):
    """
    當執行此功能時，顯示標題為 網站連接
    三個按鈕 Mark 粉絲專頁, Mark 部落格教學文章, Google 首頁 點擊任一個按鈕，會開啟網頁到對應的url
    :param bot: 機器人預設值一定要，如果沒有給的話，你的機器人不會回覆
    :param update: Telegram update資訊，主要我們用到 send_message
    """
    # 一個按鈕需要包含需要的格式，當然你可以在後面加上更多按鈕搭配url，但我不確定最多幾個
    reply_markup = InlineKeyboardMarkup([[
        InlineKeyboardButton('Mark 粉絲專頁', url = 'https://github.com/mzshieh/pa19spring'),
        InlineKeyboardButton('Mark 部落格教學文章', url = 'https://python-telegram-bot.readthedocs.io/en/stable/index.html'),
        InlineKeyboardButton('Google 首頁', url = 'https://www.google.com.tw/')]])
    # 需要有4個參數值 1.聊天室的id 2.發送訊息的標題 3. 回覆訊息的id, 4 回覆的內容
    bot.send_message(update.message.chat.id, '網站連結', reply_to_message_id = update.message.message_id,
                     reply_markup = reply_markup)
    # 印出log
    logging.info('[getUrl][chat id]: %s' % update.message.chat.id)
    logging.info('[getUrl][reply_to_message_id]: %s' % update.message.message_id)
    logging.info('[getUrl][reply_markup]: %s' % reply_markup)


def clickButton(bot, update):
    """
    如何得知 使用者按了什麼？我們又要怎麼回覆使用者所案的按鈕呢？
    我們會針對每一個按鈕給一個 callback_data，可以依照個人喜好自己取
    :param bot: 機器人預設值一定要，如果沒有給的話，你的機器人不會回覆
    :param update: Telegram update資訊，主要我們用到 send_message
    """

    # 跟上面的方式依樣，只是將url 改成callback_data
    reply_markup = InlineKeyboardMarkup([[
        InlineKeyboardButton('吃飽了', callback_data='eat'),
        InlineKeyboardButton('還沒吃飽', callback_data='notEat'),
        InlineKeyboardButton('還在想要吃什麼？', callback_data='think')]])

    # 需要有4個參數值 1.聊天室的id 2.發送訊息的標題 3. 回覆訊息的id, 4 回覆的內容
    bot.send_message(update.message.chat.id, '你吃飽了嗎？', reply_to_message_id = update.message.message_id,
                     reply_markup = reply_markup)

    # 印出log
    logging.info('[clickButton][reply_markup]: %s' % reply_markup)
    logging.info('[clickButton][chat id]: %s' % update.message.chat.id)
    logging.info('[clickButton][reply_to_message_id]: %s' % update.message.message_id)


def getClickButtonData(bot, update):
    """
    我們透過上方的clickButton()取得了 callback_data，針對取得的參數值去判斷說要回覆給使用者什麼訊息
    :param bot: 機器人預設值一定要，如果沒有給的話，你的機器人不會回覆
    :param update: Telegram update資訊，主要我們用到 callback_query
    """
    # 取得到對應的callback_data後，去判斷說是否有符合，有符合就執行 update.callback_query.edit_message_text
    # 傳送你想傳送的訊息給使用者
    if update.callback_query.data == 'eat':
        update.callback_query.edit_message_text('真好我都還沒吃')
    if update.callback_query.data == 'notEat':
        update.callback_query.edit_message_text('還不去吃？')
    if update.callback_query.data == 'think':
        update.callback_query.edit_message_text('你慢慢想吧！我先去吃嚕！')
    # 印 log
    logging.info('[getClickButtonData][callback_query data]: %s' % update.callback_query.data)




# 當你對你的機器人說'/url'，就會執行這串
updater.dispatcher.add_handler(CommandHandler('url', getUrl))
# 當你對你的機器人說'/today'，就會執行這串
updater.dispatcher.add_handler(CommandHandler('today', clickButton))
# 當使用者點擊了 clickButton時，會獲取到 callback_data 這時會執行下方的指令，針對callback_data的參數值回應對應的訊息
updater.dispatcher.add_handler(CallbackQueryHandler(getClickButtonData))

# 這串是執行機器人算是一個運行server?很類似 我是這樣覺得
updater.start_polling()
updater.idle()
