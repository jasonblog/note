# encoding: utf-8
import requests
from PIL import Image
from Image import Image as CVImg
from PyQt4 import QtCore, QtGui
from StringIO import StringIO
import re,kerasInitModel
import numpy as np
from retrying import retry
import Queue ,time, threading, datetime

lock = threading.Lock()

class BuyTicket:
    def __init__(self, mainWindow):
        # 載入模型
        self.model = kerasInitModel.LoadModel()
        self.model.load_weights("model/model.h5")

        self.mainWindow = mainWindow
        # 身份證字號
        self.ID = str(mainWindow.textID.text())
        # 是否為來回票
        self.IsTwoWay = mainWindow.BackLayout.isChecked()
        # 去程開始車站
        self.sStation = self.GetComboboxValue(mainWindow.cb_StartStation).zfill(3)
        #去程終點車站
        self.eStation = self.GetComboboxValue(mainWindow.cb_EndStation).zfill(3)

        # 去程日期
        self.Go_Date = self.GetComboboxValue(mainWindow.cb_Go_Date)
        # 去程開始時間
        self.Go_sTime = unicode(mainWindow.cb_Go_StartTime.currentText())
        #去程結束時間
        self.Go_eTime = unicode(mainWindow.cb_Go_EndTime.currentText())

        # 去程車種
        self.Go_Kind = self.GetComboboxValue(mainWindow.cb_Go_Kind)
        # 去程票數
        self.Go_Num = unicode(mainWindow.cb_Go_Num.currentText())

        # 回程日期
        self.Back_Date = self.GetComboboxValue(mainWindow.cb_Back_Date)
        # 回程開始時間
        self.Back_sTime = unicode(mainWindow.cb_Back_StartTime.currentText())
        # 回程結束時間
        self.Back_eTime = unicode(mainWindow.cb_Back_EndTime.currentText())
        # 回程車種
        self.Back_Kind = self.GetComboboxValue(mainWindow.cb_Back_Kind)
        # 回程票數
        self.Back_Num = unicode(mainWindow.cb_Back_Num.currentText())
        # 去程是否完成訂票流程
        self.IsGoSuccess = False
        # 回程是否完成訂票流程
        self.IsBackSuccess = False
        # http請求的表頭
        self.headers = {'Content-Type': 'application/x-www-form-urlencoded',
                   'Referer': 'http://railway.hinet.net/ctkind2.htm'
                   }

    # 由MainWindow的button click事件呼叫Start() 再進行訂票
    def Start(self):
        # 開始三個執行緒
        thd1 = threading.Thread(target=self.FirstRequest(), name='Thd1', args=())
        thd2 = threading.Thread(target=self.FirstRequest(), name='Thd2', args=())
        thd3 = threading.Thread(target=self.FirstRequest(), name='Thd3', args=())

        thd1.start()
        thd2.start()
        thd3.start()

        # Wait for all threads to terminate.
        while thd1.is_alive() or thd2.is_alive() or thd3.is_alive():
            time.sleep(1)

        # 第一次請求 主要取得session
    @retry(stop_max_attempt_number=10)
    def FirstRequest(self):
        # 如果訂票還沒完成 就清空訂票資訊
        if not self.IsGoSuccess:
            self.mainWindow.Go_resultMsg.setText(unicode('', "utf-8"))
        if not self.IsBackSuccess:
            self.mainWindow.Back_resultMsg.setText(unicode('', "utf-8"))
        # 用來中止線程 如果去回程有一個還沒訂完票 就再執行thread
        if not self.IsGoSuccess or not self.IsBackSuccess:

            # self.PrintAllVariable()
            # ==================
            # 輸入基本資料頁
            # ==================
            url = 'http://railway.hinet.net/check_ctkind2.jsp'
            # # 取得post的參數
            data = self.GetQueryData()
            s = requests.Session()
            result = s.post(url, data=data, headers=self.headers)
            result.encoding = 'big5'
            result.raise_for_status()
            # print(result.text)

            self.SecondRequest(s)


    # 第二次請求 取得驗證碼並解析
    @retry(stop_max_attempt_number=10)
    def SecondRequest(self,s):

        if not self.IsGoSuccess or not self.IsBackSuccess:

            # =====================
            # 填寫驗證碼頁面
            # =====================
            # 取得驗證碼圖片
            req = s.get('http://railway.hinet.net/ImageOut.jsp')
            req.raise_for_status()
            im = Image.open(StringIO(req.content)).convert('RGB')
            io = StringIO()
            im.save(io, format='png')
            qimg = QtGui.QImage.fromData(io.getvalue())
            self.mainWindow.captchaPic.setPixmap(QtGui.QPixmap(qimg))
            self.mainWindow.logMsg('\n解析驗證碼中．．．')
            QtGui.QApplication.processEvents()
            x = CVImg(req.content)
            imgs = x.StartProcess()  # 取得處理完後的驗證碼圖片陣列

            # 將圖片陣列轉成keras可處理格式
            data = np.empty((len(imgs), 50, 50, 3), dtype="float32")
            for index, img in enumerate(imgs):
                arr = np.asarray(img, dtype="float32") / 255.0  # 將黑白圖片轉成1,0陣列 原本是0,255
                data[index, :, :, :] = arr
            lock.acquire()
            classes = self.model.predict_classes(data)
            lock.release()
            result = []
            letters = list('0123456789')
            for c in classes:
                result.append(letters[c])
            answer = ''.join(result).upper()
            self.mainWindow.logMsg('驗證碼解答: ' + answer + '\n')

            # num, ok = QtGui.QInputDialog.getText(self.mainWindow, u"驗證碼", u"請輸入驗證碼")

            if len(answer) >= 5:
                self.ThirdRequest(s,answer)
            else:
                self.SecondRequest(s)

    # 第三次請求 執行訂票動作
    @retry(stop_max_attempt_number=10)
    def ThirdRequest(self,s,answer):
        # ===============================
        # 來回票訂票結果
        # ===============================

        # 去程/單程訂票結果
        if not self.IsGoSuccess: # 判斷是否完成訂票
            dateType = self.checkDateType(self.Go_Date)
            if dateType == 'ctkind':
                url = 'http://railway.hinet.net/ctkind11.jsp'
            elif dateType == 'order_kind':
                url = 'http://railway.hinet.net/order_kind1.jsp'
            # 如果是單程票 returnTicket是0 若是雙程票 returnTicket=1
            data = self.GetQueryData(type=2, returnTicket=1 if self.IsTwoWay else 0, randInput=answer)
            # print(data)
            result = s.get(url, params=data, headers=self.headers)
            result.encoding = 'big5-hkscs'
            result.raise_for_status()
            #  過濾出結果頁的html訊息
            GoreturnMsg = self.htmlRegexMatchResult(result.text, dateType)

            lock.acquire()

            self.mainWindow.Go_resultMsg.setText(unicode(GoreturnMsg, "utf-8"))
            self.mainWindow.logMsg('去程訂票結果：\n'+unicode(GoreturnMsg, "utf-8"))
            # 如果回傳訊息是驗證碼錯誤以外的訊息 就將flag設為True
            if GoreturnMsg is not ReturnMsg.captchaErr:
                self.IsGoSuccess = True

            lock.release()
            # self.mainWindow.logMsg(result.text)
            # self.mainWindow.logMsg('====================================\n')

        # 有勾選雙程票 且未完成訂票才要跑這段
        if self.IsTwoWay and not self.IsBackSuccess:
            #  回程訂票結果
            dateType = self.checkDateType(self.Back_Date)
            if dateType == 'ctkind':
                url = 'http://railway.hinet.net/ctkind11.jsp'
            elif dateType == 'order_kind':
                url = 'http://railway.hinet.net/order_kind1.jsp'
            data2 = self.GetQueryData(type=2, returnTicket=2, randInput=answer)
            result = s.get(url, params=data2, headers=self.headers)
            result.encoding = 'big5-hkscs'
            result.raise_for_status()
            # self.mainWindow.logMsg(result.text)
            #  過濾出結果頁的html訊息
            BackreturnMsg = self.htmlRegexMatchResult(result.text, dateType)
            lock.acquire()
            self.mainWindow.Back_resultMsg.setText(unicode(BackreturnMsg, "utf-8"))
            self.mainWindow.logMsg('去程訂票結果：\n'+unicode(BackreturnMsg, "utf-8"))
            # 如果回傳訊息是驗證碼錯誤以外的訊息 就將flag設為True
            if GoreturnMsg is not ReturnMsg.captchaErr:
                self.IsBackSuccess = True
            lock.release()

        # 如果其中一個訂票沒完成 就再跑一次
        if not self.IsGoSuccess or not self.IsBackSuccess:
            self.SecondRequest(s)

    # ======================================其他method==========================================

    # 印出所有參數 Debug用
    def PrintAllVariable(self):
        attrs = vars(self)
        self.mainWindow.logMsg(', \n'.join("%s: %s" % item for item in attrs.items()))

    # 取得combobox的value
    def GetComboboxValue(self, cb):
        return str(cb.itemData(cb.currentIndex()).toPyObject())

    # 傳回post或get需要的參數
    # type 1:訂來回票 包括去回程的參數 第一個頁面用的
    #      2:result頁裡的iframe用的 依據returnTicket判斷是單程、回程、去程  1:去程 2:回程 0:單程
    def GetQueryData(self,type=1,returnTicket=0,randInput=None):
        data = {}
        if type == 1:
            data = {"person_id": self.ID,#身份證字號
                    "from_station": self.sStation,#起站
                    "to_station":self.eStation,#迄站
                    "getin_date":self.Go_Date, #去程乘車日期
                    "getin_date2":self.Back_Date,#回程乘車日期
                    "order_qty_str":self.Go_Num,#去程訂票張數
                    "order_qty_str2":self.Back_Num,#回程訂票張數
                    "train_type":self.Go_Kind,#去程車種
                    "train_type2":self.Back_Kind,#回程車種
                    "getin_start_dtime":self.Go_sTime,#去程起始時間
                    "getin_start_dtime2":self.Back_sTime,#回程起始時間
                    "getin_end_dtime":self.Go_eTime,#去程截止時間
                    "getin_end_dtime2":self.Back_eTime,#回程截止時間
                    "returnTicket": returnTicket
                   }
        elif type == 2:
            data = {"person_id": self.ID,  # 身份證字號
                    "from_station": self.sStation,  # 起站
                    "to_station": self.eStation,  # 迄站
                    "getin_date": self.Go_Date if returnTicket is not 2 else self.Back_Date,  # 去程乘車日期
                    "order_qty_str": self.Go_Num if returnTicket is not 2 else self.Back_Num,  # 去程訂票張數
                    "train_type": self.Go_Kind if returnTicket is not 2 else self.Back_Kind,  # 去程車種
                    "getin_start_dtime": self.Go_sTime if returnTicket is not 2 else self.Back_sTime,  # 去程起始時間
                    "getin_end_dtime": self.Go_eTime if returnTicket is not 2 else self.Back_eTime,  # 去程截止時間
                    "returnTicket": returnTicket,
                    "randInput": randInput
                    }
        # 避免request在get時會將網址encode
        strdata = "&".join("%s=%s" % (k, v) for k, v in data.items())
        return strdata

    # 台鐵結果頁會依據日期的區間 來get不同的網址 回傳的html也會不太一樣
    # 所以要判斷是前十天還是後五天
    # 傳入的格式 yyyy/mm/dd-aa    aa是index流水號 11(含)之前的是一個網址 之後是另一個網址
    def checkDateType(self,date):
        p = re.compile(r'\d{4}/\d{2}/\d{2}-(\d*)')
        m = p.match(date)
        if m.group(1).isdigit():
            if int(m.group(1)) > 11:
                return 'ctkind'
            else:
                return 'order_kind'
        else:
            return '非數字'



    # 輸入結果頁面的html 回傳result message
    # 日期流水號在11之前跟之後 回傳的html不一樣 所以要分開判斷
    def htmlRegexMatchResult(self,html,dateType):
        if html.find(u"亂數號碼錯誤") > -1:
            result = ReturnMsg.captchaErr
        elif html.find(u"身分證字號錯誤") > -1:
            result = ReturnMsg.IdErr
        elif html.find(u"此期間訂票額滿") > -1:
            result = ReturnMsg.NoSeat
        elif html.find(u"該車種已訂票額滿") > -1:
            result = ReturnMsg.NoTrain
        elif html.find(u'訂票日期錯誤或內容格式錯誤') > -1:
            result = ReturnMsg.DateErr
        elif html.find(u"您的車票已訂到") > -1:
            if dateType == 'order_kind':
                regex = r"<span id='spanOrderCode'[^>]*>(?P<code>\d*).*車次：</span> <span class='hv1 blue01 bold01'>(?P<trainNumber>\d*).*車種：</span> <span class='hv1 blue01 bold01'>(?P<kind>[自強|莒光|復興]*)"
            elif dateType == 'ctkind':
                regex = r"<span id=\"spanOrderCode\" [^>]*>(?P<code>\d*)[^車次]*車次：</span> <span[^>]*>(?P<trainNumber>\d*)[^車]*車種：</span> <span[^>]*>\s*(?P<kind>[自強|莒光|復興]*)"
            try:
                html = html.decode('utf-8')
            except:
                pass
            match = re.search(regex.decode('utf-8'), html)

            result = ReturnMsg.success(match)
        else:
            result = ReturnMsg.NoReturn

        return result


class ReturnMsg:
    captchaErr = "驗證碼錯誤"
    IdErr = "身份證字號錯誤"
    NoSeat = "此期間訂票額滿，\n或無指定條件之車次"
    NoTrain = "【該時段、該車種已訂票額滿】\n─ 請改訂其他時段、車種乘車票"
    DateErr = "訂票日期錯誤或內容格式錯誤"
    NoReturn = "查無回傳資料"
    @staticmethod
    def success(match):
        if match is not None:
            return str.format("您的車票已訂到\n電腦代碼:{} \n車次:{}  車種:{}",
                                match.group('code'), match.group('trainNumber'),
                                match.group('kind').encode('utf-8'))
        else:
            return "您的車票已訂到"


