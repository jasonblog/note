from flask import Flask, request, abort
import json
from datetime import datetime

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


# ======================================================================================================
# Line Bot TextSendMessage (發送文字訊息) 第一種方法
# @app.route("/TextSendMessage/<string:push_text_str>")
# def text_send_message(push_text_str):
#     line_bot_api.push_message(user_id, TextSendMessage(text=push_text_str))
#     return 'TextSendMessage: %s' % push_text_str

# Line Bot TextSendMessage (發送文字訊息) 第二種方法
# @app.route("/TextSendMessage/")
# def text_send_message():
#     send_message_str = 'hi 你好'
#     line_bot_api.push_message(user_id, TextSendMessage(text=send_message_str))
#     return 'TextSendMessage: %s' % send_message_str

# ======================================================================================================

# Line Bot ImageSendMessage (發送圖片訊息） 第一種方法
# @app.route("/ImageSendMessage/")
# def image_send_message():
#     image_message = ImageSendMessage(
#         original_content_url='https://shareboxnow.com/wp-content/uploads/2020/02/th.jpeg',
#         preview_image_url='https://shareboxnow.com/wp-content/uploads/2020/02/th.jpeg'
#     )
#     line_bot_api.push_message(user_id, image_message)
#     return 'ImageSendMessage Done!'

# Line Bot ImageSendMessage (發送圖片訊息）第二種方法
# def get_image_url_path():
#     image_url = 'https://shareboxnow.com/wp-content/uploads/2020/02/th.jpeg'
#     return image_url
#
# @app.route("/ImageSendMessage/")
# def image_send_message():
#     get_image_url = get_image_url_path()
#     image_message = ImageSendMessage(
#         original_content_url=get_image_url,
#         preview_image_url=get_image_url
#     )
#     line_bot_api.push_message(user_id, image_message)
#     return 'ImageSendMessage Done!'
# ======================================================================================================

# Line Bot VideoSendMessage (發送影片訊息) 第一種方法
# @app.route("/VideoSendMessage/")
# def video_send_message():
#     video_message = VideoSendMessage(
#         original_content_url='https://shareboxnow.com/wp-content/uploads/2020/02/IMG_0469.mp4',
#         preview_image_url='https://shareboxnow.com/wp-content/uploads/2020/02/th.jpeg'
#     )
#     line_bot_api.push_message(user_id, video_message)
#     return 'VideoSendMessage Done!'

# Line Bot VideoSendMessage (發送影片訊息) 第二種方法
# def get_video_and_preview_image_url():
#     video_url = 'https://shareboxnow.com/wp-content/uploads/2020/02/IMG_0469.mp4'
#     preview_image_url = 'https://shareboxnow.com/wp-content/uploads/2020/02/th.jpeg'
#     return video_url, preview_image_url
#
#
# @app.route("/VideoSendMessage/")
# def video_send_message():
#     video_url, preview_image_url = get_video_and_preview_image_url()
#     video_message = VideoSendMessage(
#         original_content_url=video_url,
#         preview_image_url=preview_image_url
#     )
#     line_bot_api.push_message(user_id, video_message)
#     return 'VideoSendMessage Done!'
# ======================================================================================================

# Line Bot AudioSendMessage (發送音訊訊息) 第一種方法
# @app.route("/AudioSendMessage/")
# def audio_send_message():
#     audio = AudioSendMessage(
#         original_content_url='https://shareboxnow.com/wp-content/uploads/2020/02/test.m4a',
#         duration=2400
#     )
#     line_bot_api.push_message(user_id, audio)
#     return 'AudioSendMessage Done!'

# Line Bot AudioSendMessage (發送音訊訊息) 第二種方法
# def get_audio_url_duration():
#     get_audio = {
#         'original_content_url': 'https://shareboxnow.com/wp-content/uploads/2020/02/test.m4a',
#         'duration': 2400
#     }
#     return get_audio
#
#
# @app.route("/AudioSendMessage/")
# def audio_send_message():
#     get_audio_dict = get_audio_url_duration()
#     audio = AudioSendMessage(get_audio_dict['original_content_url'], get_audio_dict['duration'])
#     line_bot_api.push_message(user_id, audio)
#     return 'AudioSendMessage Done!'

# ======================================================================================================

# Line Bot LocationSendMessage (發送位置訊息) 第一種方法
# @app.route("/LocationSendMessage/")
# def location_send_message():
#     location = LocationSendMessage(
#         title = '我未來要爬的山！',
#         address = '雪山山脈',
#         latitude = 24.533723,
#         longitude = 121.396090
#     )
#     line_bot_api.push_message(user_id, location)
#     return 'LocationSendMessage Done!'

# Line Bot LocationSendMessage (發送位置訊息) 第二種方法
# def get_location_dict():
#     get_location = {
#         'title': '我未來要爬的山！',
#         'address': '雪山山脈',
#         'latitude': 24.533723,
#         'longitude': 121.396090
#     }
#     return get_location
#
# @app.route("/LocationSendMessage/")
# def location_send_message():
#     get_location_data = get_location_dict()
#     location = LocationSendMessage(
#         get_location_data['title'],
#         get_location_data['address'],
#         get_location_data['latitude'],
#         get_location_data['longitude']
#     )
#     line_bot_api.push_message(user_id, location)
#     return 'LocationSendMessage Done!'

# ======================================================================================================

# Line Bot StickerSendMessage (發送貼紙訊息) 第一種方法
# @app.route("/StickerSendMessage/")
# def location_send_message():
#     sticker_message = StickerSendMessage(
#         package_id = '1',
#         sticker_id = '1'
#     )
#     line_bot_api.push_message(user_id, sticker_message)
#     return 'StickerSendMessage Done!'


# Line Bot StickerSendMessage (發送貼紙訊息) 第二種方法
# def get_sticker_dict():
#     sticker_message = {TextSendMessage
#         'package_id': '1',
#         'sticker_id': '1'
#     }
#     return sticker_message
#
#
# @app.route("/StickerSendMessage/")
# def location_send_message():
#     get_sticker_data = get_sticker_dict()
#     sticker_message = StickerSendMessage(
#         get_sticker_data['package_id'],
#         get_sticker_data['sticker_id']
#     )
#     line_bot_api.push_message(user_id, sticker_message)
#     return 'StickerSendMessage Done!'

# ======================================================================================================

# Line Bot TemplateSendMessage – ButtonsTemplate ( 發送按鈕模板訊息)
# @app.route("/ButtonsTemplate/")
# def ButtonsTemplate_send_message():
#     # 這是一個傳送按鈕的模板，架構解說
#     buttons_template_message = TemplateSendMessage(
#         alt_text = '我是按鈕模板',  # 當你發送到你的Line bot 群組的時候，通知的名稱
#         template = ButtonsTemplate(
#             thumbnail_image_url = 'https://shareboxnow.com/wp-content/uploads/2020/02/th.jpeg',  # 你的按鈕模板的圖片是什麼
#             title = 'M&M Share',  # 你的標題名稱
#             text = '請選擇你要的項目：',  # 應該算是一個副標題
#             # 下面主要就是你希望使用者點擊了按鈕會有哪些動作，最多只能有四個action！超過會報錯喔！
#             actions = [
#                 # 說真的這個我不知道要幹嘛用，可能後台可以收數據？我點了就回應我 postback text，至於data我就不熟了
#                 PostbackAction(
#                     label = 'postback',  # 在按鈕模板上顯示的名稱
#                     display_text = 'postback text',  # 點擊會顯示的文字
#                     data = 'action=buy&itemid=1'  # 這個...我真的就不知道了～
#                 ),
#                 # 跟上面差不多
#                 MessageAction(
#                     label = '現在幾點了？',   # 在按鈕模板上顯示的名稱
#                     text = datetime.now().strftime("%Y-%m-%d %H:%M:%S")  # 點擊後，顯示現在的時間，這些都可以隨意修改喔！
#                 ),
#                 # 跳轉到URL
#                 URIAction(
#                     label = '我的部落格',  # 在按鈕模板上顯示的名稱
#                     uri = 'https://shareboxnow.com/'  # 跳轉到的url，看你要改什麼都行，只要是url
#                 ),
#                 # 這裡可以跟上面一樣，可以重複不限定只有一個 URIAction
#                 URIAction(
#                     label = 'Google',  # 在按鈕模板上顯示的名稱
#                     uri = 'https://www.google.com.tw/'  # 跳轉到的url，看你要改什麼都行，只要是url
#                 )
#             ]
#         )
#     )
#     line_bot_api.push_message(user_id, buttons_template_message)
#     return 'ButtonsTemplate_send_message Done!'

# ======================================================================================================

# Line Bot TemplateSendMessage – ConfirmTemplate (發送確認模板）
# @app.route("/ConfirmTemplate/")
# def confirm_template_SendMessage_send_message():
#     # 這是一個傳送確認的模板，架構解說
#     confirm_template_message = TemplateSendMessage(
#         alt_text = '我是一個確認模板',  # 當你發送到你的Line bot 群組的時候，通知的名稱
#         template = ConfirmTemplate(
#             text = '你確定要買晚餐嗎?',  # 你要問的問題，或是文字敘述
#             # action 最多只能兩個喔！
#             actions = [
#                 PostbackAction(
#                     label = '確認',  # 顯示名稱
#                     display_text = '那還不快去買！？',  # 點擊後，回傳的文字
#                     data = 'action=buy&itemid=1'  # 取得資料！？
#                 ),
#                 MessageAction(
#                     label = '不買',  # 顯示名稱
#                     text = '那就不買吧！'  # 點擊後，回傳的文字
#                 )
#             ]
#         )
#     )
#     line_bot_api.push_message(user_id, confirm_template_message)
#     return 'ConfirmTemplate Done!'

# ======================================================================================================

# Line Bot TemplateSendMessage – CarouselTemplate(發送輪播訊息)
# @app.route("/CarouselTemplate/")
# def carousel_template_SendMessage_send_message():
#     # 這是一個傳送 輪播的模板，架構解說
#     carousel_template_message = TemplateSendMessage(
#         alt_text = '我是一個輪播模板',  # 通知訊息的名稱
#         template = CarouselTemplate(
#             # culumns 是一個父親
#             columns = [
#                 # 這是我第一個兒子
#                 CarouselColumn(
#                     thumbnail_image_url = 'https://shareboxnow.com/wp-content/uploads/2020/02/IMG_5601.jpg',  # 呈現圖片
#                     title = '這是一隻貓頭鷹',  # 你要顯示的標題
#                     text = '想養嗎？',  # 你想問的問題或是敘述
#                     actions = [
#                         PostbackAction(
#                             label = '養',  # 顯示的文字
#                             display_text = '對不起，這不是我的',  # 回覆的文字
#                             data = 'action=buy&itemid=1'  # 取得資料？
#                         ),
#                         MessageAction(
#                             label = '不養',  # 顯示的文字
#                             text = '好喔！沒問題'  # 回覆的文字
#                         ),
#                         URIAction(
#                             label = '這是我的網址',  # 顯示的文字
#                             uri = 'https://shareboxnow.com/'   # 跳轉的url
#                         )
#                     ]
#                 ),
#                 # 這是我第二個兒子，下面的都跟上面一樣，只是內容稍為不同，如果想嘗試可以多複製一個看看唷！
#                 # 記得要在父親裡面，不然你就沒有父親了，就會報錯還有 , 要特別注意
#                 CarouselColumn(
#                     thumbnail_image_url = 'https://shareboxnow.com/wp-content/uploads/2020/02/IMG_5599.jpg',
#                     title = '我還是貓頭鷹',
#                     text = '想喂我吃東西嗎？',
#                     actions = [
#                         PostbackAction(
#                             label = '想',
#                             display_text = '但我不想吃',
#                             data = 'action=buy&itemid=2'
#                         ),
#                         MessageAction(
#                             label = '不想',
#                             text = '我剛好也不餓，謝謝你'
#                         ),
#                         URIAction(
#                             label = '這還是我的網址 哈',
#                             uri = 'https://shareboxnow.com/'
#                         )
#                     ]
#                 )
#             ]
#         )
#     )
#     line_bot_api.push_message(user_id, carousel_template_message)
#     return 'CarouselTemplate Done!'

# ======================================================================================================

# Line Bot TemplateSendMessage – ImageCarouselTemplate ( 發送 照片輪播訊息)
# @app.route("/ImageCarouselTemplate/")
# def image_carousel_template_SendMessage_send_message():
#     # 這是一個傳送 圖片輪播的模板，架構解說
#     image_carousel_template_message = TemplateSendMessage(
#         alt_text = '我是一個圖片輪播 模板',  # 發送通知訊息名稱
#         template = ImageCarouselTemplate(
#             # 我改一下角色 我是媽媽
#             columns = [
#                 # 這是我第一個女兒
#                 ImageCarouselColumn(
#                     # 她的圖片在這裡
#                     image_url = 'https://shareboxnow.com/wp-content/uploads/2020/02/IMG_5599.jpg',
#                     # 她的動作：分享網址給你看
#                     action = URIAction(
#                             label = '這是我的網址',  # 顯示的文字
#                             uri = 'https://shareboxnow.com/'   # 跳轉的url
#                             )
#                 ),
#                 # 這是我第二個女兒
#                 ImageCarouselColumn(
#                     # 這是我二女兒的圖片在這裡
#                     image_url = 'https://shareboxnow.com/wp-content/uploads/2020/02/IMG_5601.jpg',
#                     # 她的動作是說 叫你不要點他，如果你點了 她會回復你訊息
#                     action = PostbackAction(
#                         label = '拜託不要選我',
#                         display_text = '就說叫你不要選我了！為什麼要選我？',
#                         data = 'action=buy&itemid=2'  # 取得資料？
#                     )
#                 )
#             ]
#         )
#     )
#     line_bot_api.push_message(user_id, image_carousel_template_message)
#     return 'ImageCarouselTemplate Done!'

# ======================================================================================================

# Line bot QuickReply( 發送 快速回覆訊息)
# @app.route("/QuickReplyText/")
# def QuickReply_text_send_message():
#     """
#     這裡很眼熟吧？就發送一個訊息
#     quick_reply 底下的動作，就是幫你建立自動回覆的按鈕
#     每一個 QuickReplyButton 都代表一個按鈕
#     label 就是顯示的文字，text就是當你點下去，他會回復你的訊息
#     最後一樣 push_message
#     """
#     QuickReply_text_message = TextSendMessage(
#         text = '你晚餐想吃什麼？',
#         quick_reply = QuickReply(
#             items = [
#                 QuickReplyButton(
#                     action = MessageAction(label = "泡麵", text = "自己煮！！"),
#                     image_url = 'https://shareboxnow.com/wp-content/uploads/2020/01/S__7938233.jpg'
#                 ),
#                 QuickReplyButton(
#                     action = MessageAction(label = "火鍋", text = "自己買！！"),
#                 ),
#                 QuickReplyButton(
#                     action = MessageAction(label = "牛排", text = "自己煎！！"),
#                 ),
#                 QuickReplyButton(
#                     action = MessageAction(label = "炒麵", text = "炒起來！！"),
#                 ),
#                 QuickReplyButton(
#                     action = MessageAction(label = "鐵板燒", text = "我不會！！"),
#                 ),
#                 QuickReplyButton(
#                     action = MessageAction(label = "生魚片", text = "該去漁港了！！"),
#                 )
#             ]
#         )
#     )
#     line_bot_api.push_message(user_id, QuickReply_text_message)
#     return 'QuickReplyText: %s' % QuickReply_text_message

# ======================================================================================================

# @app.route("/QuickReplyImage/")
# def QuickReply_text_send_message():
#     """
#     上面是文字，當然的圖片也行嚕！
#     """
#
#     QuickReply_image_message = ImageSendMessage(
#         original_content_url='https://shareboxnow.com/wp-content/uploads/2020/02/th.jpeg',
#         preview_image_url='https://shareboxnow.com/wp-content/uploads/2020/02/th.jpeg',
#         quick_reply = QuickReply(
#             items = [
#                 QuickReplyButton(
#                     action = MessageAction(label = "好看請按我", text = "我知道！！"),
#                     image_url = 'https://shareboxnow.com/wp-content/uploads/2020/01/S__7938233.jpg'
#                 ),
#                 QuickReplyButton(
#                     action = MessageAction(label = "不好看請按我", text = "你是不是點錯了？"),
#                 )
#             ]
#         )
#     )
#     line_bot_api.push_message(user_id, QuickReply_image_message)
#     return 'QuickReplyImage: %s' % QuickReply_image_message



import os

if __name__ == "__main__":
    port = int(os.environ.get('PORT', 5000))
    app.run(host = '0.0.0.0', port = port, debug = True)
