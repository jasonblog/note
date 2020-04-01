from telegram.ext import Updater, CommandHandler
# use_context 主要是版本問題，暫時先不能改成True，否則下面的功能是無法顯示的
updater = Updater(token='YOUR_TOKEN', use_context=False)


def getYourInformation(bot, update):
    """
    :param bot: 機器人預設值一定要，如果沒有給的話，你的機器人不會回覆
    :param update:Telegram update資訊，主要我們只有用到 回覆訊息部分
    """
    update.message.reply_text('發送人 first name, {}'.format(update.message.from_user.first_name))
    update.message.reply_text('發送人 last name, {}'.format(update.message.from_user.last_name))
    update.message.reply_text('發送人 full name:, {}'.format(update.message.from_user.full_name))
    update.message.reply_text('發送人 username:, {}'.format(update.message.from_user.username))
    update.message.reply_text('發送人 id:, {}'.format(update.message.from_user.id))
    update.message.reply_text('message_id:, {}'.format(update.message.message_id))
    update.message.reply_text('所在的聊天室 id:, {}'.format(update.message.chat.id))
    update.message.reply_text('所在的聊天室 type:, {}'.format(update.message.chat.type))
    update.message.reply_text('訊息內容:, {}'.format(update.message.text))


def testReply(bot, update):
    """
    :param bot: 機器人預設值一定要，如果沒有給的話，你的機器人不會回覆
    :param update: Telegram update資訊，主要我們只有用到 回覆訊息部分
    :return:
    Hi Mark 最近好嗎？
    """
    update.message.reply_text('Hi {} 最近好嗎？'.format(update.message.from_user.first_name))


# 套用 getYourInformation()，當你對你的機器人說 '/meInfo'，就會執行這串
updater.dispatcher.add_handler(CommandHandler('meInfo', getYourInformation))

# 套用 testReply()，當你對你的機器人說'/hi'，就會執行這串
updater.dispatcher.add_handler(CommandHandler('hi', testReply))

# 這串是執行機器人算是一個運行server?很類似 我是這樣覺得
updater.start_polling()
updater.idle()
