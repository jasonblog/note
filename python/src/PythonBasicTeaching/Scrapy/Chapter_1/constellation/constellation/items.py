# -*- coding: utf-8 -*-

# Define here the models for your scraped items
#
# See documentation in:
# https://doc.scrapy.org/en/latest/topics/items.html

import scrapy


class ConstellationItem(scrapy.Item):
    # define the fields for your item here like:
    # 星座名稱： "今日牡羊座解析"
    name = scrapy.Field()

    # 整體運勢 標題
    OverallFortuneTitle = scrapy.Field()
    # 整體運勢 內容
    OverallFortuneContent = scrapy.Field()
    # 愛情運勢 標題
    LoveFortuneTitle = scrapy.Field()
    # 愛情運勢 內容
    LoveFortuneContent = scrapy.Field()
    # 事業運勢 標題
    CareerFortuneTitle = scrapy.Field()
    # 事業運勢 內容
    CareerFortuneContent = scrapy.Field()
    # 財運運勢 標題
    WealthFortuneTitle = scrapy.Field()
    # 財運運勢 內容
    WealthFortuneContent = scrapy.Field()



