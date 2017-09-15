# -*- coding: utf-8 -*-
'''
批次命名分割出來的小驗證碼並存檔
'''
from PyQt5 import QtCore, QtGui
from PyQt5.QtWidgets import (QApplication, QCheckBox, QColorDialog, QDialog,
                QErrorMessage, QFileDialog, QFontDialog, QFrame, QGridLayout,
                        QInputDialog, QLabel, QLineEdit, QMessageBox, QPushButton,
                        QMainWindow, QWidget, QComboBox, QCompleter, QGroupBox,
                        QVBoxLayout, QPlainTextEdit, QAction)


from PIL import Image as pilIm
import sys,random,os,requests
from StringIO import StringIO
from Image import Image
import os.path as path
import re



class RenameWindow(QDialog):
    def __init__(self, parent=None):
        super(RenameWindow, self).__init__(parent)
        self.originCaptcha = None
        self.setWindowTitle(u'重新命名驗證碼')
        self.grid = QGridLayout(self)

        self.captchaPic = smallPicBox(203,62)
        self.grid.addWidget(self.captchaPic,0,0,3,2)
        self.changePicBtn = QPushButton(u'換圖')
        self.changePicBtn.clicked.connect(self.GetImage)
        self.grid.addWidget(self.changePicBtn,0,2,3,3)
        # 加入分割後驗證碼圖片的pictureBox
        width,height = 60,60
        self.pic1 = smallPicBox(width,height)
        self.grid.addWidget(self.pic1,4,0,3,2)
        self.input1 = QLineEdit()
        self.input1.returnPressed.connect(self.saveImg)
        self.grid.addWidget(self.input1,4,1,3,2)

        self.pic2 = smallPicBox(width,height)
        self.grid.addWidget(self.pic2,7,0,3,2)
        self.input2 = QLineEdit()
        self.input2.returnPressed.connect(self.saveImg)
        self.grid.addWidget(self.input2,7,1,3,2)

        self.pic3 = smallPicBox(width,height)
        self.grid.addWidget(self.pic3,10,0,3,2)
        self.input3 = QLineEdit()
        self.input3.returnPressed.connect(self.saveImg)
        self.grid.addWidget(self.input3,10,1,3,2)

        self.pic4 = smallPicBox(width,height)
        self.grid.addWidget(self.pic4,13,0,3,2)
        self.input4 = QLineEdit()
        self.input4.returnPressed.connect(self.saveImg)
        self.grid.addWidget(self.input4,13,1,3,2)

        self.pic5 = smallPicBox(width,height)
        self.grid.addWidget(self.pic5,16,0,3,2)
        self.input5 = QLineEdit()
        self.input5.returnPressed.connect(self.saveImg)
        self.grid.addWidget(self.input5,16,1,3,2)

        self.pic6 = smallPicBox(width,height)
        self.grid.addWidget(self.pic6,19,0,3,2)
        self.input6 = QLineEdit()
        self.input6.returnPressed.connect(self.saveImg)
        self.grid.addWidget(self.input6,19,1,3,2)

        self.setTabOrder(self.pic1, self.pic2)
        self.setTabOrder(self.pic2, self.pic3)
        self.setTabOrder(self.pic3, self.pic4)
        self.setTabOrder(self.pic4, self.pic5)
        self.setTabOrder(self.pic5, self.pic6)

        self.pixBoxs = [self.pic1, self.pic2, self.pic3, self.pic4, self.pic5, self.pic6]
        self.inputBoxs = [self.input1,self.input2,self.input3,self.input4,self.input5,self.input6]

        # 取得驗證碼
        self.GetImage()


    def GetImage(self):
        # 清空圖片和文字輸入框
        for i, obj in enumerate(self.inputBoxs):
            obj.clear()
        for i,obj in enumerate(self.pixBoxs):
            obj.clear()

        # 取得驗證碼stream
        s = requests.Session()
        req = s.get('http://railway.hinet.net/ImageOut.jsp')

        im = pilIm.open(StringIO(req.content)).convert('RGB')
        io = StringIO()
        im.save(io, format='png')
        qimg = QtGui.QImage.fromData(io.getvalue())
        self.captchaPic.qimage = qimg
        # 原始驗證碼
        self.captchaPic.setPixmap(QtGui.QPixmap(qimg))

        Img = Image(req.content)
        # 取得處理完後的圖片
        self.imgarr = Img.StartProcess()
        print self.imgarr
        # 用來儲存轉成QPixmap的圖片 用來存檔
        self.PixMaparr = []
        for index,img in enumerate(self.imgarr):
            try:
                height, width,channel = img.shape
                bytes = 3*width
                qimg = QImage(img.data, width, height,bytes, QImage.Format_RGB888)
                pixmap = QPixmap(qimg)
                self.pixBoxs[index].qimage = qimg
                self.pixBoxs[index].setPixmap(pixmap)
                self.PixMaparr.append(pixmap)
            except:
                print 'aaaaaaaaaaaaaaaaaaaaaaaaaa'
                pass

    def saveImg(self):

        if len(self.imgarr) > 0:
            for index,pixmap in enumerate(self.PixMaparr):
                name = self.inputBoxs[index].text()
                # 如果輸入的檔名不為空 就存檔
                if name:
                    # 取得圖片的流水號
                    fullname = self.GetFileSeqName(name)
                    pixmap.save(fullname)
        # 重新取得驗證碼
        self.GetImage()

        self.input1.setFocus(True)

    def GetFileSeqName(self,inputName):
        savePath = r"CaptchaTest"
        for root, dirs, files in os.walk(savePath):
            indices = [i for i, x in enumerate(files) if x.startswith(inputName)]
            print(len(indices))

        fullname = savePath +'/'+ inputName + '_' + str(len(indices)+1) +'.png'
        print fullname
        return fullname


class smallPicBox(QLabel):

    def __init__(self, width,height,parent=None):
        super(smallPicBox, self).__init__(parent)
        self.qimage = None # 用來存放qimage 點擊圖片時就能下載
        self.setFixedSize(width,height)
        self.setStyleSheet(
            'border:1px solid rgb(0, 0, 0)'
        )
    # 點擊驗證碼的圖片就將原始驗證碼存檔
    def mouseReleaseEvent(self, event):
        savePath = r"FailCaptcha"
        for root, dirs, files in os.walk(savePath):
            indices = [i for i, x in enumerate(files)]
            print(len(indices))

        fullname = savePath +'/' + str(len(indices)+1) +'.png'
        if self.qimage:
            self.qimage.save(fullname)


if __name__ == '__main__':
    app = QApplication(sys.argv)
    RenameWindow = RenameWindow()
    # MainWindow.resize(600, 400)
    RenameWindow.show()
    app.exec_()
