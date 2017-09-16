# -*- coding: utf-8 -*-
# 直接調用pyqt 不用designer

from PyQt4 import QtCore, QtGui
import datetime,json,sys,io
from BuyTicket import BuyTicket
from VPN import VPN

class MainWindow(QtGui.QMainWindow):
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)
        self.setWindowTitle(u'台鐵訂票助手')

        app_icon = QtGui.QIcon()
        app_icon.addFile('img/train.png', QtCore.QSize(16, 16))
        app_icon.addFile('img/train.png', QtCore.QSize(24, 24))
        app_icon.addFile('img/train.png', QtCore.QSize(32, 32))
        app_icon.addFile('img/train.png', QtCore.QSize(48, 48))
        app_icon.addFile('img/train.png', QtCore.QSize(256, 256))
        self.setWindowIcon(app_icon)

        self.form_widget = FormWidget(self)
        self.font_size = 12
        font_label_standard = QtGui.QFont('微軟正黑體', self.font_size)
        QtGui.QApplication.setFont(font_label_standard)
        # 加入form_widget 表單主體
        self.setCentralWidget(self.form_widget)

        # 加入vpn設定
        VPNAction = QtGui.QAction(u'VPN設定', self)
        VPNAction.triggered.connect(lambda:self.form_widget.showVPNdialog(self.form_widget))
        # 加入menu bar 幫助
        AboutAction = QtGui.QAction(u'關於', self)
        AboutAction.triggered.connect(self.form_widget.showAbout)


        menubar = self.menuBar()
        AboutMenu = menubar.addMenu(u'選單')
        AboutMenu.addAction(VPNAction)
        AboutMenu.addAction(AboutAction)





class FormWidget(QtGui.QWidget):

    def __init__(self, parent):
        super(FormWidget, self).__init__(parent)
        # 是否手動選擇VPN
        self.IsManuallyChooseVPN = False
        # 手動選出來的VPN列表 要傳遞給vpn class
        self.ManuallyVPNServer = None
        # VPN list的表頭
        self.VPNheader = None


        # ====================設定左邊layout 用來呈現訂票介面 裡面包含ticketInfo Layout====================
        self.leftLayout = QtGui.QGridLayout()

        # =============設定left layout的上方控件
        self.lbID = QtGui.QLabel( u"身份證字號：")
        self.lbID.setObjectName(u"lbID")
        self.leftLayout.addWidget(self.lbID,0,0)
        self.textID = QtGui.QLineEdit()
        self.textID.setObjectName(u"textID")
        reg_ex = QtCore.QRegExp("^[A-Za-z]{1}[12]{1}[0-9]{8}$")
        textID_validator = QtGui.QRegExpValidator(reg_ex, self)
        self.textID.setValidator(textID_validator)
        self.leftLayout.addWidget(self.textID,0,1)

        self.submitBtn = QtGui.QPushButton(u'啟動')
        self.submitBtn.setFont(QtGui.QFont('微軟正黑體', 20))
        self.submitBtn.setFixedSize(150,70)
        self.submitBtn.setIcon(QtGui.QIcon('img/power.png'))
        self.submitBtn.setIconSize(QtCore.QSize(40, 40))
        self.submitBtn.clicked.connect(lambda: self.Start(self))
        self.leftLayout.addWidget(self.submitBtn,0,3,2,2)

        self.vpnBtn = QtGui.QPushButton(u'開啟跳板')
        self.vpnBtn.setFixedSize(150, 40)
        self.vpnBtn.setIcon(QtGui.QIcon('img/vpn.png'))
        self.vpnBtn.setIconSize(QtCore.QSize(40, 40))
        self.vpnBtn.clicked.connect(lambda: self.OpenVPN(self))
        self.leftLayout.addWidget(self.vpnBtn, 2, 3, 2, 2)


        self.lb_StartStation = QtGui.QLabel(u'起站代碼')
        self.leftLayout.addWidget(self.lb_StartStation,1,0,1,1)
        self.cb_StartStation = ExtendedComboBox()
        self.cbStationAddItem(self.cb_StartStation)
        self.leftLayout.addWidget(self.cb_StartStation,1,1,1,1)

        self.lb_EndStation = QtGui.QLabel(u'到站代碼')
        self.leftLayout.addWidget(self.lb_EndStation,2,0,1,1)
        self.cb_EndStation = ExtendedComboBox()
        self.cbStationAddItem(self.cb_EndStation)
        self.leftLayout.addWidget(self.cb_EndStation,2,1,1,1)

        # =============ticketInfo Layout 用來放置去回程的控件
        self.TicketInfolayout = QtGui.QGridLayout()
        self.leftLayout.addLayout(self.TicketInfolayout,4,0,10,5)

        # =============設定位在ticketInfo裡的出發layout
        self.GoLayout = QtGui.QGroupBox()
        self.GoLayout.setTitle(u'【出發／單程】')
        vbox = QtGui.QGridLayout()
        self.lb_Go_Date = QtGui.QLabel(u'乘車日期')
        vbox.addWidget(self.lb_Go_Date,0,0,1,2)
        self.cb_Go_Date = QtGui.QComboBox()
        self.cbDateAddItem(self.cb_Go_Date)
        vbox.addWidget(self.cb_Go_Date,0,3,1,2)

        self.lb_Go_Num = QtGui.QLabel(u'訂票張數')
        vbox.addWidget(self.lb_Go_Num,1,0,1,2)
        self.cb_Go_Num = QtGui.QComboBox()
        self.cbNumAddItem(self.cb_Go_Num)
        vbox.addWidget(self.cb_Go_Num,1,3,1,1)

        self.lb_Go_Kind = QtGui.QLabel(u'車種')
        vbox.addWidget(self.lb_Go_Kind,2,0,1,2)
        self.cb_Go_Kind = QtGui.QComboBox()
        self.cbKindAddItem(self.cb_Go_Kind)
        vbox.addWidget(self.cb_Go_Kind,2,3,1,2)

        self.lb_Go_StartTime = QtGui.QLabel(u'起始時間')
        vbox.addWidget(self.lb_Go_StartTime, 3, 0, 1, 2)
        self.cb_Go_StartTime = QtGui.QComboBox()
        self.cbTimeAddItem(self.cb_Go_StartTime)
        vbox.addWidget(self.cb_Go_StartTime, 3, 3, 1, 1)

        self.lb_Go_EndTime = QtGui.QLabel(u'截止時間')
        vbox.addWidget(self.lb_Go_EndTime, 4, 0, 1, 2)
        self.cb_Go_EndTime = QtGui.QComboBox()
        self.cbTimeAddItem(self.cb_Go_EndTime)
        vbox.addWidget(self.cb_Go_EndTime, 4, 3, 1, 1)

        self.GoLayout.setLayout(vbox)
        self.TicketInfolayout.addWidget(self.GoLayout,0,0,1,3)

        # =============設定位在ticketInfo裡的回程layout
        self.BackLayout = QtGui.QGroupBox()
        self.BackLayout.setTitle(u'【回程】')
        self.BackLayout.setCheckable(True)
        backBox = QtGui.QGridLayout()

        self.lb_Back_Date = QtGui.QLabel(u'乘車日期')
        backBox.addWidget(self.lb_Back_Date,0,0,1,2)
        self.cb_Back_Date = QtGui.QComboBox()
        self.cbDateAddItem(self.cb_Back_Date)
        backBox.addWidget(self.cb_Back_Date,0,3,1,2)

        self.lb_Back_Num = QtGui.QLabel(u'訂票張數')
        backBox.addWidget(self.lb_Back_Num,1,0,1,2)
        self.cb_Back_Num = QtGui.QComboBox()
        self.cbNumAddItem(self.cb_Back_Num)
        backBox.addWidget(self.cb_Back_Num,1,3,1,1)

        self.lb_Back_Kind = QtGui.QLabel(u'車種')
        backBox.addWidget(self.lb_Back_Kind,2,0,1,2)
        self.cb_Back_Kind = QtGui.QComboBox()
        self.cbKindAddItem(self.cb_Back_Kind)
        backBox.addWidget(self.cb_Back_Kind,2,3,1,2)

        self.lb_Back_StartTime = QtGui.QLabel(u'起始時間')
        backBox.addWidget(self.lb_Back_StartTime, 3, 0, 1, 2)
        self.cb_Back_StartTime = QtGui.QComboBox()
        self.cbTimeAddItem(self.cb_Back_StartTime)
        backBox.addWidget(self.cb_Back_StartTime, 3, 3, 1, 1)

        self.lb_Back_EndTime = QtGui.QLabel(u'截止時間')
        backBox.addWidget(self.lb_Back_EndTime, 4, 0, 1, 2)
        self.cb_Back_EndTime = QtGui.QComboBox()
        self.cbTimeAddItem(self.cb_Back_EndTime)
        backBox.addWidget(self.cb_Back_EndTime, 4, 3, 1, 1)

        self.BackLayout.setLayout(backBox)
        self.TicketInfolayout.addWidget(self.BackLayout,0,3,1,3)





        # ====================設定右方layout====================
        self.rightLayout = QtGui.QGridLayout()
        self.lbPicBox = QtGui.QLabel(u'驗證碼：')
        self.rightLayout.addWidget(self.lbPicBox,0,0)
        self.captchaPic = QtGui.QLabel()
        self.captchaPic.setFixedSize(203,62)
        self.captchaPic.setStyleSheet(
            'border:1px solid rgb(0, 0, 0)'
        )
        self.rightLayout.addWidget(self.captchaPic,1,0,3,2)

        self.groupBoxGoResult = QtGui.QGroupBox()
        self.groupBoxGoResult.setStyleSheet("QGroupBox { background-color: rgb(255, 228,\
        224); border:1px solid rgb(122, 16, 0); }")
        self.groupBoxGoResult.setTitle(u'【去程訂票結果】')
        gorBox = QtGui.QVBoxLayout()
        self.Go_resultMsg = QtGui.QLabel()   #  用來顯示訂票結果
        self.Go_resultMsg.setStyleSheet('QLabel {color: red}')
        gorBox.addWidget(self.Go_resultMsg)
        self.groupBoxGoResult.setLayout(gorBox)
        self.rightLayout.addWidget(self.groupBoxGoResult,5,0,4,3)

        self.groupBoxBackResult = QtGui.QGroupBox()
        self.groupBoxBackResult.setStyleSheet("QGroupBox { background-color: rgb(255, 228,\
        224); border:1px solid rgb(122, 16, 0); }")
        self.groupBoxBackResult.setTitle(u'【回程訂票結果】')
        backrBox = QtGui.QVBoxLayout()
        self.Back_resultMsg =  QtGui.QLabel()  #  用來顯示訂票結果
        self.Back_resultMsg.setStyleSheet('QLabel {color: red}')
        backrBox.addWidget(self.Back_resultMsg)
        self.groupBoxBackResult.setLayout(backrBox)
        self.rightLayout.addWidget(self.groupBoxBackResult, 9, 0, 4, 3)


        # self.rightLayout.setRowMinimumHeight(4, 220)   #用來填補下面空白的部份


        # ==============設定下方log專用layout====================
        self.buttomLayout = QtGui.QGridLayout()
        self.lbLog = QtGui.QLabel(u'【狀態】')
        self.buttomLayout.addWidget(self.lbLog,0,0)
        self.clearBtn = QtGui.QPushButton(u'清除')
        self.clearBtn.clicked.connect(self.clearLog)
        self.buttomLayout.addWidget(self.clearBtn,0,4,1,1)
        self.textBrowser = QtGui.QPlainTextEdit()
        self.textBrowser.setReadOnly(True)
        self.buttomLayout.addWidget(self.textBrowser,1,0,1,5)





        # ====================主要layout======================
        self.mainLayout = QtGui.QGridLayout()
        self.mainLayout.addLayout(self.leftLayout,0,0)
        # 分隔線
        line = QtGui.QFrame()
        line.setFrameStyle(QtGui.QFrame.VLine| QtGui.QFrame.Sunken)
        self.mainLayout.addWidget(line,0,1)
        self.mainLayout.addLayout(self.rightLayout,0,2)
        # 分隔線
        hline = QtGui.QFrame()
        hline.setFrameStyle(QtGui.QFrame.HLine | QtGui.QFrame.Sunken)
        self.mainLayout.addWidget(hline,1,0,1,5)
        self.mainLayout.addLayout(self.buttomLayout,2,0,1,5)
        self.setLayout(self.mainLayout)

    # 將輸出訊息輸入到狀態視窗
    def logMsg(self,record):
        try:
            msg = unicode(record, 'utf-8')
        except:
            msg = record
        self.textBrowser.appendPlainText(msg)
    # 清除狀態欄
    def clearLog(self):
        self.textBrowser.clear()

    # 為訂票張數的combobox add item
    def cbNumAddItem(self, cb):
        cb.addItems([str(i) for i in range(1, 7)])


    # 為時間下拉選單產生時段
    def cbTimeAddItem(self, cb):
        cb.addItems([str(i).zfill(2) + ':00' for i in range(24)])
        cb.addItem('23:59')


    #  為車種下拉選單產生data
    def cbKindAddItem(self, cb):
        cb.addItem(u'全部車種', '*4')
        cb.addItem(u'自強號', '*1')
        cb.addItem(u'莒光號', '*2')
        cb.addItem(u'復興號', '*3')


    # 為日期下拉選單產生資料
    def cbDateAddItem(self, cb):
        date = datetime.datetime.now()
        strDate = date.date().strftime('%Y/%m/%d')
        dateOfWeek = ['一', '二', '三', '四', '五', '六', '日']
        cb.addItem(strDate + ' (' + unicode(dateOfWeek[date.date().weekday()],"utf-8") + ')', strDate + '-00')
        for i in range(14):
            date += datetime.timedelta(days=1)
            strDate = date.date().strftime('%Y/%m/%d')
            cb.addItem(strDate + ' (' + unicode(dateOfWeek[date.date().weekday()],"utf-8") + ')',
                       strDate + '-' + str(i + 1).zfill(2))


    # 產生車站下拉選單的資料
    def cbStationAddItem(self, cb):
        try:
            with io.open('station.json', 'r', encoding='utf8') as f:
                sdata = json.loads(f.read())
                # Sort it by station numbers
                sdata_sorted = sorted(
                    sdata, key=lambda s: s['ID']
                )
                for ii in range(len(sdata_sorted)):
                    cb.addItem(sdata_sorted[ii]['Station'], int(sdata_sorted[ii]['ID']))

        except IOError as ioerr:
            self.showMessage('查無車站json檔: ' + str(ioerr))

    # 顯示出對話視窗 要在connect裡傳參數可使用 lambda: self.showMessage(u'text you want to display')
    def showMessage(self,message=None):
        QtGui.QMessageBox.information(self, u"提示", message if message != None else u'發生錯誤!!')

    def showAbout(self):
        QtGui.QMessageBox.about(self,u"說明",
                                u"""
        <b>台鐵訂票程式</b>
        <p>Copyright © 2017 Lin Yich - All rights reserved.</p>
        <p>這程式能實現台鐵自動化訂票</p>
        <p>僅供學術交流用途，請勿從事非法活動</p>
        <p>需求環境：請參閱Readme.md</p>
                                """)
    # 將這視窗的內容pass到台鐵爬蟲 開始訂票程序
    def Start(self,mainWindow):
        do = BuyTicket(mainWindow)
        # disable開始按鈕
        self.submitBtn.setDisabled(True)
        do.Start()
        self.submitBtn.setDisabled(False)
    # 開啟VPN
    def OpenVPN(self,mainWindow):
        vpn = VPN(mainWindow)
        path = vpn.PathFromReg()
        if 'openvpn' in path.lower():
            vpn.ConnectVPN()
        else:
            QtGui.QMessageBox.warning(self, u"警告",u"尚未安裝OpenVPN，請參閱README.MD的說明")
    # 顯示vpn設定的對話框 並把值存檔下來
    def showVPNdialog(self,mainWindow):
        header,selectedValue, groupchecked, ok = VPNDialog.getVPNLists(mainWindow) #這段有點複雜 因為要取得dialog的回傳值 反正看得懂就好
        self.IsManuallyChooseVPN = groupchecked
        # 手動選出來的VPN列表 要傳遞給vpn class
        self.ManuallyVPNServer = selectedValue
        self.VPNheader = header


#========================================VPN設定對話框================================
class VPNDialog(QtGui.QDialog):
    def __init__(self, mainWindow, parent = None):
        super(VPNDialog, self).__init__(parent)

        layout = QtGui.QVBoxLayout(self)

        self.groupVpn = QtGui.QGroupBox()
        self.groupVpn.setTitle(u'【手動選擇VPN】 取消勾選則自動選擇最快Server')
        self.groupVpn.setCheckable(True)
        self.groupVpn.setMinimumSize(800,500)
        self.groupVpn.setChecked(mainWindow.IsManuallyChooseVPN) #從formWidget取得全域變數來設定groupBox的checked狀態
        # 取得vpn lists
        vpn = VPN(mainWindow)
        self.vpnLists,self.header = vpn.getVpnServerLists()
        vbox = QtGui.QVBoxLayout()
        self.TableView = QtGui.QTableView()
        self.TableView.setSelectionMode(QtGui.QAbstractItemView.SingleSelection) #只能選一個row
        self.TableView.setSelectionBehavior(QtGui.QAbstractItemView.SelectRows) #指定選擇整個row
        tm = MyTableModel(self.vpnLists, self.header, self)
        self.TableView.setModel(tm)

        header = self.TableView.horizontalHeader()
        header.setResizeMode(0, QtGui.QHeaderView.ResizeToContents)
        header.setResizeMode(1, QtGui.QHeaderView.ResizeToContents)
        header.setResizeMode(5, QtGui.QHeaderView.ResizeToContents)
        vbox.addWidget(self.TableView)
        self.groupVpn.setLayout(vbox)
        layout.addWidget(self.groupVpn)

        # 確認按鈕跟取消按鈕
        buttons = QtGui.QDialogButtonBox()
        action_btn = buttons.addButton(QtGui.QDialogButtonBox.Ok)
        action_btn.setText(u'確定')
        cancel_btn = buttons.addButton(QtGui.QDialogButtonBox.Cancel)
        cancel_btn.setText(u'取消')
        buttons.accepted.connect(self.accept)
        buttons.rejected.connect(self.reject)
        layout.addWidget(buttons)

    def getIschecked(self):
        return self.groupVpn.isChecked()

    def getSelectedValue(self):
        try:
            return self.vpnLists[self.TableView.selectionModel().selectedRows()[0].row()]
        except:
            return None
    def getHeader(self):
        return self.header

    @staticmethod
    def getVPNLists(mainWindow,parent = None):
        dialog = VPNDialog(mainWindow)
        dialog.setWindowTitle(u'手動選擇VPN')
        app_icon = QtGui.QIcon()
        app_icon.addFile('img/train.png', QtCore.QSize(16, 16))
        app_icon.addFile('img/train.png', QtCore.QSize(24, 24))
        app_icon.addFile('img/train.png', QtCore.QSize(32, 32))
        app_icon.addFile('img/train.png', QtCore.QSize(48, 48))
        app_icon.addFile('img/train.png', QtCore.QSize(256, 256))
        dialog.setWindowIcon(app_icon)
        result = dialog.exec_()
        groupchecked = dialog.getIschecked()
        selectedValue = dialog.getSelectedValue()
        header = dialog.getHeader()
        return (header,selectedValue,groupchecked, result == QtGui.QDialog.Accepted)

#========================================產生給QTableView用的資料================================
class MyTableModel(QtCore.QAbstractTableModel):
    def __init__(self, datain, headerdata, parent=None, *args):
        """ datain: a list of lists
            headerdata: a list of strings
        """
        QtCore.QAbstractTableModel.__init__(self, parent, *args)
        self.arraydata = datain
        self.headerdata = headerdata

    def rowCount(self, parent):
        return len(self.arraydata)

    def columnCount(self, parent):
        return len(self.arraydata[0])

    def data(self, index, role):
        if not index.isValid():
            return QtCore.QVariant()
        elif role != QtCore.Qt.DisplayRole:
            return QtCore.QVariant()
        return QtCore.QVariant(self.arraydata[index.row()][index.column()])

    def headerData(self, col, orientation, role):
        if orientation == QtCore.Qt.Horizontal and role == QtCore.Qt.DisplayRole:
            return QtCore.QVariant(self.headerdata[col])
        return QtCore.QVariant()

    def sort(self, Ncol, order):
        """Sort table by given column number.
        """
        self.emit(QtCore.SIGNAL("layoutAboutToBeChanged()"))
        self.arraydata = sorted(self.arraydata, key=lambda s: s[2])
        if order == QtCore.Qt.DescendingOrder:
            self.arraydata.reverse()
        self.emit(QtCore.SIGNAL("layoutChanged()"))


class ExtendedComboBox(QtGui.QComboBox):
    def __init__(self, parent=None):
        super(ExtendedComboBox, self).__init__(parent)

        self.setFocusPolicy(QtCore.Qt.StrongFocus)

        self.setEditable(True)

        # add a filter model to filter matching items

        self.pFilterModel = QtGui.QSortFilterProxyModel(self)

        self.pFilterModel.setFilterCaseSensitivity(QtCore.Qt.CaseInsensitive)

        self.pFilterModel.setSourceModel(self.model())

        # add a completer, which uses the filter model

        self.completer = QtGui.QCompleter(self.pFilterModel, self)

        # always show all (filtered) completions

        self.completer.setCompletionMode(QtGui.QCompleter.UnfilteredPopupCompletion)

        self.setCompleter(self.completer)

        # connect signals

        self.lineEdit().textEdited[unicode].connect(self.pFilterModel.setFilterFixedString)

        self.completer.activated.connect(self.on_completer_activated)

    # on selection of an item from the completer, select the corresponding item from combobox

    def on_completer_activated(self, text):
        if text:
            index = self.findText(text)

            self.setCurrentIndex(index)
    # 當lose focus時 判斷combobox輸入的text有沒有在item中
    def event(self, event):
        if event.type() == QtCore.QEvent.FocusOut:
            text = self.currentText()
            if text:
                index = self.findText(text)
                if index == -1:
                    self.setStyleSheet('QComboBox{border:1px solid red}')
                else:
                    self.setStyleSheet('QComboBox{border:1px solid gray}')


        return QtGui.QComboBox.event(self, event)

    # on model change, update the models of the filter and completer as well

    def setModel(self, model):
        super(ExtendedComboBox, self).setModel(model)

        self.pFilterModel.setSourceModel(model)

        self.completer.setModel(self.pFilterModel)

    # on model column change, update the model column of the filter and completer as well

    def setModelColumn(self, column):
        self.completer.setCompletionColumn(column)

        self.pFilterModel.setFilterKeyColumn(column)

        super(ExtendedComboBox, self).setModelColumn(column)



if __name__ == '__main__':
    app =QtGui.QApplication(sys.argv)
    MainWindow = MainWindow()
    # MainWindow.resize(600, 400)
    MainWindow.show()
    app.exec_()
