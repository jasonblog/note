import random
import os
from telegram.ext import Updater, CommandHandler
import logging
import time

# 印出log的方法
logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger()

# 有Debug 以及 Info 模式，因為我不需要印太多資訊只需要Info就好
# logger.setLevel(logging.DEBUG)
logger.setLevel(logging.INFO)

# use_context 主要是版本問題，暫時先不能改成True，否則下面的功能是無法顯示的
updater = Updater(token='YOUR_TOKEN', use_context=False)


def getTotalCostList():
    """
    開啟totalCost.txt內的資料放入到recordMoney List 內
    在使用 /overall 會呼叫到這塊程式碼
    """
    recordMoney = []
    if os.path.exists('totalCost.txt'):
        with open('totalCost.txt', encoding='UTF-8') as FILE:
            for cost in FILE:
                recordMoney.append(cost.strip('\n'))
            logging.info('totalCost: %s' % recordMoney)
    return recordMoney


def inputCostRecord(bot, update):
    '''
    紀錄你的項目以及金額到 totalCost.txt
    一次只能一筆
    :param bot: 機器人預設值一定要，如果沒有給的話，你的機器人不會回覆
    :param update: Telegram update資訊，主要我們只有用到 回覆訊息部分
    '''
    timeStr = time.strftime('%Y/%m/%d', time.localtime(time.time()))  # 當天的時間
    if len(update.message.text) <= 6:  # 如果單純輸入 /cost 會跟你說叫你該如何輸入，以及因為/cost 的字元數量一定會小於6
        update.message.reply_text('你輸入方式有錯誤喔！\n請輸入：/addCost 項目 金錢 \n 例如: /addCost 吃飯 300')
    else:
        dailyCost = update.message.text[8:].replace('\n', ' ')  # 取得到 /cost 後面的參數值
        try:  # 用try的原因輸入參數太多，沒依照固定格式輸入，因此會請你重新用正確格式重新輸入
            # 判斷我們輸入的第二位參數一定要是Int以及list的數量一定要等於三個，如果沒有也會請你重新輸入正確格式
            if (isInputCostInt(Input_data = dailyCost.split(' ')) is True) and (len(dailyCost.split(' ')) == 3):
                f = open('totalCost.txt', 'a')
                f.write(timeStr + dailyCost + '\n')
                f.close()
                # 顯示出 你今天輸入的花費以及金額
                update.message.reply_text('[新增紀錄] [{}]  花費項目:{} ,金額:{}'.format(timeStr, dailyCost.split(' ')[1], dailyCost.split(' ')[2]))
                update.message.reply_text('查詢總共花費紀錄，請點擊 👉 /overall')  # 可以查看你這段時間所紀錄的花費
                logging.info(timeStr + ' 花費項目, 金額：{}'.format(dailyCost))  # 顯示 log
            else:
                # 輸入錯誤格式或有誤時，請重新輸入
                update.message.reply_text('你輸入方式有錯誤喔！\n請輸入：/addCost 項目 金錢 \n 例如: /addCost 吃飯 300')
        except Exception as e:
            # 輸入錯誤格式或有誤時，請重新輸入以及顯示問題點
            logging.info('[try inputCostRecord Fail]: %s' % e)
            update.message.reply_text('你輸入方式有錯誤喔！\n請輸入：/addCost 項目 金錢 \n 例如: /addCost 吃飯 300')


def isInputCostInt(Input_data):
    """
    :param Input_data: 給於 /inputCostRecord 的金額參數是否為int
    """
    try:
        getCost = Input_data[2]
        logging.info(int(getCost))  # 轉換為 int格式，如果是就回傳True
        return True
    except ValueError:
        logging.info('格式有錯誤')
        return False


def getOverallCostRecord(bot, update):
    """
    取得 totalCost.txt內的所有資料顯示到你的line bot上
    :param bot: 機器人預設值一定要，如果沒有給的話，你的機器人不會回覆
    :param update: Telegram update資訊，主要我們只有用到 回覆訊息部分
    """
    total_list = getTotalCostList()  # 取得 totalCost內的檔案並轉成的list
    logging.info(total_list)  # 印出 log
    if len(total_list) != 0:
        # 顯示 紀錄的全部花費list
        logging.info('recordMoney: %s' % total_list)
        # 將 list 轉成 str 顯示在 bot
        update.message.reply_text('所有的花費:\n%s' % '\n'.join(total_list))
    else:  # 如果list 沒有任何資料會是回傳0 會請你去紀錄，再查看
        update.message.reply_text('你暫時沒有紀錄喔！請透過底下指令做紀錄\n /addCost 項目 金錢 \n 例如: /addCost 吃飯 300')


def deleteCostRecord(bot, update):
    """
    刪除紀錄用
    :param bot: 機器人預設值一定要，如果沒有給的話，你的機器人不會回覆
    :param update: Telegram update資訊，主要我們只有用到 回覆訊息部分
    """
    total_list = getTotalCostList()  # 取得 totalCost內的檔案並轉成的list
    logging.info(total_list)  # 印出 log
    if len(total_list) != 0:
        # 顯示 紀錄的全部花費list
        isDeleteRecord = False
        deleteCost = update.message.text[12:].replace('\n', ' ')  # 取得到 /deleteCost 後面的參數值
        logging.info('deleteCost:%s' % deleteCost)

        # 判斷說是否刪除的參數值，是否在你的total_list，有的話就會進行刪除
        # 將 isDeleteRecord 改為True
        for deleteRecord in total_list:
            if deleteCost == deleteRecord:
                logging.info('[deleteRecord]:%s' % deleteCost)
                total_list.remove(deleteCost)
                isDeleteRecord = True

        # 如果有符合就會刪除list中的參數值，重新寫入到totalCost.txt
        if isDeleteRecord is True:
            logging.info('刪除後的List:%s' % total_list)
            f = open('totalCost.txt', 'w')
            f.write('\n'.join(total_list))
            f.write('\n')
            f.close()
            update_total_list = getTotalCostList()  # 取得 totalCost內的檔案並轉成的list
            # 將 list 轉成 str 顯示在 bot
            update.message.reply_text('刪除後的所有的紀錄:\n%s' % '\n'.join(update_total_list))
        else:
            update.message.reply_text('未找到你要刪除的: %s' % deleteCost)
            update.message.reply_text('可透過指令進行刪除: /deleteCost 2020/03/01 油錢 300')
            update.message.reply_text('查詢你所有紀錄的資料，請點擊 👉 /overall')  # 可以查看你這段時間所紀錄的花費

    else:  # 如果list 沒有任何資料會是回傳0 會請你去紀錄，再查看
        update.message.reply_text('你暫時沒有紀錄喔！'
                                  '請透過底下指令做紀錄\n '
                                  '/addCost 項目 金錢 \n 例如: /addCost 吃飯 300\n'
                                  '/在進行 /deleteCost 2020-03-01 吃飯 300')


updater.dispatcher.add_handler(CommandHandler('addCost', inputCostRecord))
updater.dispatcher.add_handler(CommandHandler('overall', getOverallCostRecord))
updater.dispatcher.add_handler(CommandHandler('deleteCost', deleteCostRecord))


updater.start_polling()
updater.idle()
