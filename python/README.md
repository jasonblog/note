# python


```py
# -*- coding: utf-8 -*-

# author: yinxuanh@gmail.com
# version: 1.0
# github: https://github.com/yin-xuanHuang/web_crawler_ptt_gamesale
# license: MIT license


import requests
import sqlite3
import re
from bs4 import BeautifulSoup
#from time import sleep

# 文章內容處理
def catch_content_return_list(content_list, sellORwanted):
    # 找出目標內容，並擷取
    find_all_body_list = content_list
    content_wanted_index = 0
    for body_string in find_all_body_list:
        if u'【' in body_string: break
        content_wanted_index = content_wanted_index + 1
    try:
        content_wanted = find_all_body_list[content_wanted_index]
    except:
        return []

    while True:
        try:
            if find_all_body_list[content_wanted_index+1][0] == u"※": break
        except:
            break
        content_wanted = content_wanted + find_all_body_list[content_wanted_index + 1]
        content_wanted_index = content_wanted_index +1

    # 當標題非售非徵
    if sellORwanted != u'售' and sellORwanted != u'徵':
        if u'徵' in content_wanted:
            sellORwanted = u'徵'
        else:
            sellORwanted = u'售'

    # 消除特定字元 TODO: 多遊戲同一文章之間隔區隔問題
    body_content = content_wanted
    body_content = body_content.replace("\n", "")
    body_content = body_content.replace(" ", "")
    body_content = body_content.replace(u'★', "")
    body_content = body_content.replace(u'　', "")

    # 消除可能的"範例文字" 
    # e.x.←可直接使用，無須編輯
    if u"←可直接使用，無須編輯" in body_content:
        body_content = body_content.replace(u"←可直接使用，無須編輯", "")
    # e.x.（限制級、輔15級、輔12級、保護級、普遍級）
    if u"（限制級、輔15級、輔12級、保護級、普遍級）" in body_content:
        body_content = body_content.replace(u"（限制級、輔15級、輔12級、保護級、普遍級）", "")
    # e.x.←分級提示
    if u"←分級提示" in body_content:
        body_content = body_content.replace(u"←分級提示", "")
    # e.x.（未照左方分級者違規）
    if u"（未照左方分級者違規）" in body_content:
        body_content = body_content.replace(u"（未照左方分級者違規）", "")
    # e.x.（徵求者、配件等無分級者可免填，販售國內未代理之遊戲，請直接填限制級即可。）
    if u"（徵求者、配件等無分級者可免填，販售國內未代理之遊戲，請直接填限制級即可。）" in body_content:
        body_content = body_content.replace(u"（徵求者、配件等無分級者可免填，販售國內未代理之遊戲，請直接填限制級即可。）", "")
    # e.x.（遊戲有多語系版本必填，海外版請註明是哪個國家。）
    if u"（遊戲有多語系版本必填，海外版請註明是哪個國家。）" in body_content:
        body_content = body_content.replace(u"（遊戲有多語系版本必填，海外版請註明是哪個國家。）", "")

    body_content = body_content.replace(u'。', "")
    body_content = body_content[:-2] + u'【'

    # 處理文章
    try:
        the_colon = body_content.index(u'：')
        the_brackets = body_content.index(u'【')
        next_brackets = body_content.index(u'【', the_brackets + 1)
        content_dic = dict()
        while True:
            dic_name = body_content[the_brackets + 1:the_colon - 1]
            content_dic[dic_name] = body_content[the_colon + 1:next_brackets]

            if u'：' not in body_content[the_colon + 1:] or u'【' not in body_content[next_brackets + 1:]: break

            the_brackets = next_brackets
            next_brackets = body_content.index(u'【', next_brackets + 1)
            the_colon = body_content.index(u'：', the_brackets)
    except:
        return []

    # 排序內容，回傳有排序的序列
    # TODO : "交換" 選項
    content_list = list()
    i = 0
    while i < 9:
        if i == 0:                                # 售or徵求 0
            content_list.append(sellORwanted)
            i = i + 1

        check = False
        for item in content_dic:
            if u"名" in item and i == 1:          # 物品名稱 1
                content_list.append(content_dic.get(item," "))
                check = True
                break
            elif u"分" in item and i == 2:        # 遊戲分級 2
                content_list.append(content_dic.get(item," "))
                check = True
                break
            elif u"語" in item and i == 3:        # 語系版本 3
                content_list.append(content_dic.get(item," "))
                check = True
                break
            elif u"價" in item and i == 4:        # 徵求價 or 售價 4
                content_list.append(content_dic.get(item," "))
                check = True
                break
            elif u"易" in item and i == 5:        # 交易方式 5
                content_list.append(content_dic.get(item," "))
                check = True
                break
            elif u"保" in item and i == 6:        # 保存狀況 6
                content_list.append(content_dic.get(item," "))
                check = True
                break
            elif u"地" in item and i == 7:        # 地區 7
                content_list.append(content_dic.get(item," "))
                check = True
                break
            elif u"附" in item and i == 8:        # 附註 8
                content_list.append(content_dic.get(item," "))
                check = True
                break
        if not check:
            content_list.append(" ")
        i = i + 1

    return content_list


# 主程式開始

# connect database
conn = sqlite3.connect('ptt_gamesale.sqlite')
cur = conn.cursor()

# TODO 適當資料型態
# set database
# author, ok
# date, ok
# gamestation, ok
# title_id, titleID ok
# sellORwanted 0 ok

# 物品名稱 1 item_name ok
# 遊戲分級 2 law_limited ok
# 語系版本 3 language ok
# 徵求價 or 售價 4 price ok
# 交易方式 5 howtotrade ok
# 保存狀況 6 status ok
# 地區 7 address ok
# 附註 8 message ok 

cur.executescript('''
CREATE TABLE IF NOT EXISTS Content(
    id     INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
    titleID     TEXT UNIQUE,
    author_id   INTEGER,
    sellORwanted_id     INTEGER,
    gamestation_id INTEGER,
    date     TEXT,
    item_name     TEXT,
    law_limited_id     INTEGER,
    language_id     INTEGER,
    price     TEXT,
    howtotrade     TEXT,
    status     TEXT,
    address     TEXT,
    message     TEXT    
);

CREATE TABLE IF NOT EXISTS Author (
    id     INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
    name  TEXT UNIQUE
);

CREATE TABLE IF NOT EXISTS SellORwanted (
    id     INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
    name  TEXT UNIQUE
);

CREATE TABLE IF NOT EXISTS Gamestation (
    id     INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
    name  TEXT UNIQUE
);

CREATE TABLE IF NOT EXISTS Law_limited (
    id     INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
    name  TEXT UNIQUE
);

CREATE TABLE IF NOT EXISTS Language (
    id     INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
    name  TEXT UNIQUE
);

CREATE TABLE IF NOT EXISTS Fail_title (
    title_id     TEXT NOT NULL PRIMARY KEY UNIQUE
);

CREATE TABLE IF NOT EXISTS End_page (
    pre_page     TEXT NOT NULL PRIMARY KEY UNIQUE
);

''')


ptt_url = "https://www.ptt.cc/bbs/Gamesale/index.html"

#from_page = raw_input("輸入開始頁數（index3311.html），不需要就直接按Enter:")

#if from_page == "":
#    res = requests.get(ptt_url)
#else:
#    try:
#        res = requests.get(ptt_url[:32] + from_page, "html.parser")
#    except:
#        print "輸入錯誤：", from_page
#        exit()

#print res.encoding

res = requests.get(ptt_url)
soup = BeautifulSoup(res.text, "html.parser")

#print soup.prettify().encode('utf-8')

#title_list_per_page = list()


# 處理的文章條數
count = 0
# 處理的頁數
page = 0
# 問題文章數
fail_count = 0
# 發現重複處理的文章
#find_title_id = False
# 是否使用database 抓取上次寫入的頁數
is_read_database = False
# 遊戲主機白名單
gamestation_list = ["PC","XONE","X360",u"其他","WII","NDS","GBA","XBOX","PS","NGC","DC","DS"]

while True: # page loop

    try:

        page = page + 1

        # page上的每一條文章 list row_list
        row_list = soup.select("body div.title a")
        row_list.reverse()

        # 抓page上一頁的網址
        pre_page = str(soup.find_all("a", text="‹ 上頁")[0].get('href'))

        # 處理每條文章開始
        for row in row_list: # title loop

            # 文章的連結與其識別碼 str title_url title_id
            title_id = row.get('href').encode('utf-8')
            title_url = ptt_url[0:18] + title_id
            title_id = title_url[32:-5]

            # TODO 設定第二判斷（有爬完一次為第二次判斷，以免被覆蓋）
            # 判斷文章識別碼是否已經在資料庫中
            cur.execute('SELECT titleID FROM Content WHERE titleID =?',(title_id,))
            r = cur.fetchone()
            if r is not None :
                #find_title_id = True
                if not is_read_database:
                    cur.execute('SELECT pre_page FROM End_page')
                    try:
                        pre_page_database = cur.fetchone()[0]
                        pre_page = pre_page_database
                        is_read_database = True
                    except:
                        pass
                    #print title_id, "is already in database."
                    #print "title_url=", title_url
                continue

            # 擷取文章名
            title_content = row.get_text()
            # 徵求文或販賣文
            try:
                sellORwanted = re.search("\]\s*([\S+])\s*", title_content).group(1)
            except:
                cur.execute('''INSERT OR IGNORE INTO Fail_title (title_id) 
                    VALUES ( ? )''', ( title_id, ) )
                print title_id, "skiped.(Fail)"
                fail_count = fail_count + 1
                continue
            # page上的遊戲主機 str gamestation
            if '[' not in title_content or ']' not in title_content: continue
            pre_bracket = title_content.index('[')
            back_bracket = title_content.index(']')
            gamestation = title_content[pre_bracket + 1: back_bracket].upper()
            gamestation = gamestation.rstrip().lstrip()

            # 文章的遊戲主機 是否在白名單中
            in_gamestation_list = False
            for station in gamestation_list:
                if station in gamestation:
                    in_gamestation_list = True
            if not in_gamestation_list:
                print title_id, "skiped.(Not in white list)"
                continue

            # 爬進文章中 TODO: 
            # 網址會壞掉嗎？例如剛(被)砍的文章.ans:Yes(try and except?) 利用回應時間來解決？

            res = requests.get(title_url)
            soup = BeautifulSoup(res.text, "html.parser")

            # 擷取文章資訊
            title_head = soup.select(".article-meta-value")
            try:
                author = title_head[0].text
                date = title_head[3].text
                date = date[4:16] + date[19:]
            except:
                cur.execute('''INSERT OR IGNORE INTO Fail_title (title_id) 
                    VALUES ( ? )''', ( title_id, ) )
                print title_id, "skiped.(Fail)"
                fail_count = fail_count + 1
                continue
            # 找出目標內容，並擷取
            content_list = catch_content_return_list(soup.body.find_all(string=True), sellORwanted)
            if content_list == []:
                cur.execute('''INSERT OR IGNORE INTO Fail_title (title_id) 
                    VALUES ( ? )''', ( title_id, ) )
                print title_id, "skiped.(Fail)"
                fail_count = fail_count + 1
                continue
            
#            print "---------------------------------------------------"
#            print "author=", author
#            print "date  =", date
#            print "GS    =", gamestation
#            print "ID    =", title_id
#            print "售或徵求 =", content_list[0]
#            print "物品名稱 =", content_list[1]
#            print "遊戲分級 =", content_list[2]
#            print "語系版本 =", content_list[3]
#            print "目標價錢 =", content_list[4]
#            print "交易方式 =", content_list[5]
#            print "保存狀況 =", content_list[6]
#            print "所在地區 =", content_list[7]
#            print "其他附註 =", content_list[8]
#            print "---------------------------------------------------"

            # database 存入
            cur.execute('''INSERT OR IGNORE INTO Language (name) 
                    VALUES ( ? )''', ( content_list[3], ) )
            cur.execute('SELECT id FROM Language WHERE name = ? ', (content_list[3], ))
            language_id = cur.fetchone()[0]

            cur.execute('''INSERT OR IGNORE INTO Law_limited (name) 
                    VALUES ( ? )''', ( content_list[2], ) )
            cur.execute('SELECT id FROM Law_limited WHERE name = ? ', (content_list[2], ))
            law_limited_id = cur.fetchone()[0]

            cur.execute('''INSERT OR IGNORE INTO Gamestation (name) 
                    VALUES ( ? )''', ( gamestation, ) )
            cur.execute('SELECT id FROM Gamestation WHERE name = ? ', (gamestation, ))
            gamestation_id = cur.fetchone()[0]

            cur.execute('''INSERT OR IGNORE INTO SellORwanted (name) 
                    VALUES ( ? )''', ( content_list[0], ) )
            cur.execute('SELECT id FROM SellORwanted WHERE name = ? ', (content_list[0], ))
            sellORwanted_id = cur.fetchone()[0]

            cur.execute('''INSERT OR IGNORE INTO Author (name) 
                    VALUES ( ? )''', ( author, ) )
            cur.execute('SELECT id FROM Author WHERE name = ? ', (author, ))
            author_id = cur.fetchone()[0]

            cur.execute('''
            INSERT OR REPLACE INTO Content
                   ( titleID,        author_id,   sellORwanted_id, gamestation_id,  date,            item_name,
                     law_limited_id, language_id, price,           howtotrade,      status,          address,         message)
            VALUES ( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ? )''', 
                   ( title_id,       author_id,   sellORwanted_id, gamestation_id,  date,            content_list[1],
                     law_limited_id, language_id, content_list[4], content_list[5], content_list[6], content_list[7], content_list[8] ) )

            conn.commit()
            print title_id, "success!"
            is_read_database = False

            count = count + 1

        # 發現重複處理的文章後處置
        #if find_title_id: break

        # want be saved to database
        pre_page_save = pre_page

        # 爬進上一頁，直到第一頁
        if pre_page == "None":
            cur.execute('''DELETE FROM End_page''')
            cur.execute('''INSERT OR REPLACE INTO End_page(pre_page) 
                VALUES ( ? )''', ("None", ) )
            conn.commit()
            print "pre_page=", pre_page
            break
        pre_page = ptt_url[:18] + pre_page

        res = requests.get(pre_page) 
        soup = BeautifulSoup(res.text, "html.parser")
        print "DO", pre_page

    except:
        print ""
        print "Program interrupted!!!"
        print "NOW TitleID:", title_id
        print "NOW Page url:", pre_page_save
        print "NOW titles:", count

        cur.execute('''DELETE FROM End_page''')
        cur.execute('''INSERT OR REPLACE INTO End_page(pre_page) 
                VALUES ( ? )''', ( pre_page_save, ) )
        conn.commit()
        print "Record pre_page to database."
        exit()

print "Total pages:", page
print "Total titles:", count
print "Total fail pages", fail_count
print "Done!"
```

將爬到的資料都用字串存在sqlite資料庫裡，  有用關聯式的資料庫。

網頁爬蟲，爬 PTT gamesale 板，用sqlite 存文章資訊。

使用版本：

python 2.7.11

beautifulsoup4 4.5.3

pysqlite 2.8.3

內容有過濾特殊字串，

像是★、空白、大的空白、換行、發文提醒的範例文字，

最後也有過濾"【"、"："  --->用這些判斷區間，

像是 "物品名稱" 存字串由"："到下個"【"之間，

內容分：八個區段：

1. 物品名稱
2. 遊戲分級
3. 語系版本
4. 徵求價 or 售價
5. 交易方式
6. 保存狀況
7. 地區
8. 附註