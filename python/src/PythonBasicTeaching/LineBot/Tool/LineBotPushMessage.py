# -*- coding:utf-8 -*-
import requests
import sys


class LineBotFunction:

    def __init__(self, push_str, webhook_url):
        self.push_str = push_str
        self.webhook_url = webhook_url

    def push_message(self):
        """
        取得 self.push_str 參數值來做判斷
        如果是None 則不發送請求

        如果有字串的話，會針對Line Bot 的 Webhook URL發送請求
        並印出輸入的字串
        :return:
        """
        if self.push_str is None:
            print("未輸入字串，不發送請求")
            print("使用方式：請在 python3 ~/LineBotPushMessage.py 空一個輸入參數，記得要加上雙引號")
            print("範例： python3 LineBotPushMessage.py \"how are you\"")
        else:
            requests.get(self.webhook_url + self.push_str)
            print(self.webhook_url + self.push_str)



if __name__ == "__main__":

    """
    程式碼會從這裡開始跑
    在執行程式碼時，請執行 python3 LineBotPushMessage.py "how are you"
    "how are you" 代表的是Line Bot 會收到什麼字串
    """

    Webhook_URL = {你的WebHook_URL} # 請先修改你的Webhook URL，否則你無法發送請求

    try:
        push_str = sys.argv[1] # 這串代表的是 它會取得 你執行檔案後面 下一個字串
    except Exception:
        push_str = None  # 當如果沒有輸入會等於None 因此在執行時，不會去請求

    LineBot = LineBotFunction(push_str, Webhook_URL)  # 取得到的push_str與 Webhook_RUL 會給 LineBotFunction class
    LineBot.push_message()  # 執行 LineBotFunction內的 push_message function
