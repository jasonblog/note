import scrapy
from constellation.items import ConstellationItem
import time
import re

today_time = time.strftime("%Y-%m-%d", time.localtime())


class constellationSpider(scrapy.Spider):
    name = 'constellation'
    allowed_domains = ['http://astro.click108.com.tw/']
    start_urls = ['http://astro.click108.com.tw/daily_0.php?iAstro=0&iAcDay=%s' % today_time,
                  'http://astro.click108.com.tw/daily_1.php?iAstro=1&iAcDay=%s' % today_time,
                  'http://astro.click108.com.tw/daily_2.php?iAstro=2&iAcDay=%s' % today_time,
                  'http://astro.click108.com.tw/daily_3.php?iAstro=3&iAcDay=%s' % today_time,
                  'http://astro.click108.com.tw/daily_4.php?iAstro=4&iAcDay=%s' % today_time,
                  'http://astro.click108.com.tw/daily_5.php?iAstro=5&iAcDay=%s' % today_time,
                  'http://astro.click108.com.tw/daily_6.php?iAstro=6&iAcDay=%s' % today_time,
                  'http://astro.click108.com.tw/daily_7.php?iAstro=7&iAcDay=%s' % today_time,
                  'http://astro.click108.com.tw/daily_8.php?iAstro=8&iAcDay=%s' % today_time,
                  'http://astro.click108.com.tw/daily_9.php?iAstro=9&iAcDay=%s' % today_time,
                  'http://astro.click108.com.tw/daily_10.php?iAstro=10&iAcDay=%s' % today_time,
                  'http://astro.click108.com.tw/daily_11.php?iAstro=11&iAcDay=%s' % today_time
                  ]

    def parse(self, response):
        item = ConstellationItem()
        target = response.css(".TODAY_CONTENT")  # 取得我們要抓取資料的區塊
        dr = re.compile(r'<[^>]+>', re.S)  # 正規表示法
        for tag in target:
            item['name'] = tag.css("h3::text")[0].get()  # 針對要抓取的區塊內，在取得h3，::text代表說取得文字
            item['OverallFortuneTitle'] = tag.css('span::text')[0].get()  # 針對要抓取的區塊內，在取得span的第0的元素，::text代表說取得文字
            item['LoveFortuneTitle'] = tag.css('span::text')[1].get()  # 針對要抓取的區塊內，在取得span的第1的元素，::text代表說取得文字
            item['CareerFortuneTitle'] = tag.css('span::text')[2].get()  # 針對要抓取的區塊內，在取得span的第2的元素，::text代表說取得文字
            item['WealthFortuneTitle'] = tag.css('span::text')[3].get()  # 針對要抓取的區塊內，在取得span的第3的元素，::text代表說取得文字
            item['OverallFortuneContent'] = tag.css('p::text')[1].get()  # 針對要抓取的區塊內，在取得p的第1個元素，::text代表說取得文字
            item['LoveFortuneContent'] = tag.css('p::text')[3].get()  # 針對要抓取的區塊內，在取得p的第3個元素，::text代表說取得文字

            CareerFortuneContent = tag.css('p')[5].get()
            getCareerFortuneContent = dr.sub('', CareerFortuneContent)
            WealthFortuneContent = tag.css('p')[7].get()
            getWealthFortuneContent = dr.sub('', WealthFortuneContent)
            item['CareerFortuneContent'] = getCareerFortuneContent
            item['WealthFortuneContent'] = getWealthFortuneContent
            yield item
