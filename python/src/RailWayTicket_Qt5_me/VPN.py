# encoding: utf-8
import requests, os, sys, tempfile, subprocess, base64, time, platform

"""
用來抓取速度最快的VPN
init:country參數 用來指定特定國家的VPN或不指定

members:
self.index:vpn gate CSV的國家全名或國家縮寫欄位的index
self.country:指定的國家
"""


class VPN:

    """
    初始化時 指派一個國家 可為縮寫或全名
    會依據縮寫或全名 指派index變數 用來指向array的全名或縮寫欄位
    日本 全:Japan  縮:Jp
    韓國 全:Korea Republic of  縮:KR
    墨西哥 全:Mexico  縮:MK
    美國   全:United States  縮:US
    中國   全:China  縮:CN
    .....etc
    """

    def __init__(self,mainWindow, country=''):
        # pyqt視窗的實例化物件
        self.mainWindow = mainWindow
        self.country = country

        path=self.PathFromReg()
        if 'openvpn' not in path.lower():
            raise '尚未安裝openVPN!!'

        if len(country) == 0:
            self.index = -1  # 如果沒指派國家 index為-1
        elif len(country) == 2:
            self.index = 6  # 國家名稱縮寫
        elif len(country) > 2:
            self.index = 5  # 國家名稱全名
        else:
            self.mainWindow.logMsg('請指定國家(全名或縮寫) 或不輸入，預設判斷全部資料')
            exit(1)

    #  連接上最快速的VPN
    def ConnectVPN(self):

        # 如果有從dialog手動選擇vpn 就用手動選的vpn 否則預設自動取得
        if self.mainWindow.IsManuallyChooseVPN and self.mainWindow.ManuallyVPNServer is not None:
            winner = self.mainWindow.ManuallyVPNServer
            labels = self.mainWindow.VPNheader
        else:
            # 取得vpn server列表
            supported,labels = self.getVpnServerLists()
            # 依照總分欄位 排序servers 取出最快的server
            winner = supported[0]

        self.mainWindow.logMsg("\n== 選擇的伺服器 ==")
        #  [:-1]是指不取最後一欄
        pairs = list(zip(labels, winner))[:-1]
        for (l, d) in pairs[:4]:
            self.mainWindow.logMsg(l + ': ' + d)

        self.mainWindow.logMsg(pairs[4][0] + ': ' + str(float(pairs[4][1]) / 10**6) + ' MBps')
        self.mainWindow.logMsg("國家: " + pairs[5][1])

        self.mainWindow.logMsg("\n啟動VPN中...")

        #  Debug用  將vpn config檔寫入txt檔
        # f = open(r'C:\Users\vi000\Desktop\vpn config.txt', 'w', encoding='UTF-8')
        # f.write(str(base64.b64decode(winner[-1])).replace(r"\r\n", "\n"))
        # f.close()

        #  Windows專用 儲存open vpn config檔
        #  需要用系統管理員權限執行CMD 才能正確執行
        if platform.system().upper() == "WINDOWS":
            _, path = tempfile.mkstemp()
            f = open(path, 'w')
            f.write(base64.b64decode(winner[-1]).decode('utf-8').replace(r"\r\n", "\n"))
            f.close()
            cmd = "openvpn --config "+path
            self.process = subprocess.Popen(cmd, shell=True)
        elif platform.system().upper() == "LINUX":
            _, path = tempfile.mkstemp()
            f = open(path, 'w')
            f.write(base64.b64decode(winner[-1]))
            f.write('\nscript-security 2\nup /etc/openvpn/update-resolv-conf\ndown /etc/openvpn/update-resolv-conf')
            f.close()

            self.process = subprocess.Popen(['sudo', 'openvpn', '--config', path])

    def getVpnServerLists(self):
        try:
            self.mainWindow.logMsg("====開始取得VPN清單====")
            vpn_data = requests.get('http://www.vpngate.net/api/iphone/').text.replace('\r','')
            servers = [line.split(',') for line in vpn_data.split('\n')]
            labels = servers[1]
            labels[0] = labels[0][1:]
            servers = [s for s in servers[2:] if len(s) > 1]
        except Exception, e:
            print(e)
            self.mainWindow.logMsg('無法取得VPN資料')
            exit(1)

        if self.index != -1:
            desired = [s for s in servers if self.country.lower() in s[self.index].lower()]
        else:
            desired = servers
        found = len(desired)
        self.mainWindow.logMsg('找到 ' + str(found) + ' 個伺服器  國家: ' + self.country
              if len(self.country) > 0
              else '找到 ' + str(found) + ' 個伺服器')
        if found == 0:
            exit(1)

        supported = [s for s in desired if len(s[-1]) > 0]
        self.mainWindow.logMsg(str(len(supported)) + ' 個伺服器支援 OpenVPN')
        return sorted(supported, key=lambda s: s[2], reverse=True),labels

    #  只有Linux會用到
    def disConnect(self):
            try:
                self.process.kill()
            except:
                pass
            while self.process.poll() != 0:
                time.sleep(1)
            self.mainWindow.logMsg('\nVPN 中止')

    #  確認VPN有無連線上
    def testVPN(self):
        r = requests.get('http://www.icanhazip.com')
        self.mainWindow.logMsg("icanhazip回傳的IP是" + r.text)

    #  回傳環境變數 用來判斷有沒有安裝open vpn
    #  path = PathFromReg()
    #  print('openvpn' in path.lower())
    def PathFromReg(self):
        pass
        ''' 
        loc = r'SYSTEM\CurrentControlSet\Control\Session Manager\Environment'
        reg = _winreg.ConnectRegistry(None, _winreg.HKEY_LOCAL_MACHINE)
        key = _winreg.OpenKey(reg, loc)
        n_val = _winreg.QueryInfoKey(key)[1]
        for i in range(n_val):
            val = _winreg.EnumValue(key, i)
            if val[0] == 'Path':
                return val[1]
                '''

