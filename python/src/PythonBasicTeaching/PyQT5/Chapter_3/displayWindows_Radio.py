# -*- coding: utf-8 -*-

import sys
import os
import time
import subprocess

from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtWidgets import QMainWindow, QApplication, QListView, QFileDialog
from PyQt5.QtGui import QStandardItemModel, QStandardItem
from PyQt5.QtCore import pyqtSlot
from MarkQtUI_Radio import Ui_MainWindow


class Mark(QMainWindow, Ui_MainWindow):

    def __init__(self, parent = None):
        # 繼承Ui_MainWindow 也就是 MarkQtUI 內的 class
        super(Mark, self).__init__(parent)
        # 建立ui介面
        self.setupUi(self)


if __name__ == "__main__":
    # 第一行必備，系統呼叫
    app = QApplication(sys.argv)

    # 指定 Mark Class 會先執行__init__
    window = Mark()

    # 將GUI介面顯示出來
    window.show()

    # 關閉系統
    sys.exit(app.exec_())
