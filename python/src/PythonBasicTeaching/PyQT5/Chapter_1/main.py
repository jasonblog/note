# -*- coding: utf-8 -*-

import sys
import os
import time
import subprocess

from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtWidgets import QMainWindow, QApplication, QListView, QFileDialog
from PyQt5.QtGui import QStandardItemModel, QStandardItem
from PyQt5.QtCore import pyqtSlot

from MarkQtUI import Ui_MainWindow

class Mark(QMainWindow, Ui_MainWindow):

    def __init__(self, parent=None):
        # 繼承Ui_MainWindow 也就是 MarkQtUI 內的 class
        super(Mark, self).__init__(parent)

        # 建立ui介面
        self.setupUi(self)
        # 這功能主要是點擊了這個按鈕要執行什麼？
        # self.Test_Button 這個 function 在 MarkQtUI.Ui_MainWindow內
        # 因為已經繼承了Ui_MainWindow，因此執行 self.Test_Button
        # 點擊了時候會套用下方的function test_button_clicked，會將值輸出
        # 如果沒有這行，點擊按鈕不會有任何的動作
        self.Test_Button.clicked.connect(self.test_button_clicked)


    def test_button_clicked(self):
        # 你看要他輸出什麼事情，這裡則是會在終端機印出 test
        print('test')

if __name__ == "__main__":
    
    # 第一行必備，系統呼叫
    app = QApplication(sys.argv)

    # 指定 TestFolder Class 會先執行__init__
    window = Mark()

    # 將GUI介面顯示出來
    window.show()

    # 關閉系統
    sys.exit(app.exec_())
