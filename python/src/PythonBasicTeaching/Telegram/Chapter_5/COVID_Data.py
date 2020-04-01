# -*- coding: utf-8 -*-
import json
import os
from urllib.request import urlretrieve, urlopen
import time

Covid19Dict = {}


def GlobalData():
    """
    回傳全球的狀況
    """
    data = urlopen("https://covid19dashboard.cdc.gov.tw/dash2").read()
    data = json.loads(data.decode())
    print('全球 COVID-19 最新狀況: %s' % data)
    return data


def NewTaiwanData():
    """
    回傳 最新的消息
    """
    data = urlopen("https://covid19dashboard.cdc.gov.tw/dash3").read()
    data = json.loads(data.decode())
    print('台灣 COVID-19 最新狀況: %s' % data)
    return data


def TaiwanDataRecord():
    """
    回傳 20201/1～現在的發病日狀況，更新較慢～
    :return:
    """
    data = urlopen("https://covid19dashboard.cdc.gov.tw/dash5").read()
    data = json.loads(data.decode())
    print('台灣 COVID-19 發病狀況: %s' % data)
    return data


def main():
    """
    將取得到的三個dict合併起來回傳
    """
    getGlobalDict = {'GlobalData': GlobalData()}
    getNewTaiwanData = {'NewTaiwanData': NewTaiwanData()}
    getTaiwanDataRecord = {'TaiwanDataRecord': TaiwanDataRecord()}
    Covid19Dict.update(getGlobalDict)
    Covid19Dict.update(getNewTaiwanData)
    Covid19Dict.update(getTaiwanDataRecord)
    return Covid19Dict

