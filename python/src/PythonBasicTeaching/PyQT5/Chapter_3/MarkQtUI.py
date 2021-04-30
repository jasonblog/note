# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'mainwindow.ui'
#
# Created by: PyQt5 UI code generator 5.14.1
#
# WARNING! All changes made in this file will be lost!


from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(800, 600)
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.displayListWidget = QtWidgets.QListWidget(self.centralwidget)
        self.displayListWidget.setGeometry(QtCore.QRect(50, 120, 256, 192))
        self.displayListWidget.setObjectName("displayListWidget")
        self.verticalLayoutWidget = QtWidgets.QWidget(self.centralwidget)
        self.verticalLayoutWidget.setGeometry(QtCore.QRect(49, 10, 261, 109))
        self.verticalLayoutWidget.setObjectName("verticalLayoutWidget")
        self.verticalLayout_2 = QtWidgets.QVBoxLayout(self.verticalLayoutWidget)
        self.verticalLayout_2.setContentsMargins(0, 0, 0, 0)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.clickTimeButton = QtWidgets.QPushButton(self.verticalLayoutWidget)
        self.clickTimeButton.setObjectName("clickTimeButton")
        self.verticalLayout_2.addWidget(self.clickTimeButton)
        self.clickTestButton = QtWidgets.QPushButton(self.verticalLayoutWidget)
        self.clickTestButton.setObjectName("clickTestButton")
        self.verticalLayout_2.addWidget(self.clickTestButton)
        self.clickDateButton = QtWidgets.QPushButton(self.verticalLayoutWidget)
        self.clickDateButton.setObjectName("clickDateButton")
        self.verticalLayout_2.addWidget(self.clickDateButton)
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtWidgets.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 800, 22))
        self.menubar.setObjectName("menubar")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtWidgets.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow"))
        self.clickTimeButton.setText(_translate("MainWindow", "現在時間是？"))
        self.clickTestButton.setText(_translate("MainWindow", "嗨起來"))
        self.clickDateButton.setText(_translate("MainWindow", "今天日期是?"))
