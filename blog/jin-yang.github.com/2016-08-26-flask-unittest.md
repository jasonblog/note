---
title: Flask 單元測試
layout: post
comments: true
language: chinese
category: [webserver]
keywords: flask,單元測試,unittest
description: 在 Python 中，有各種各樣的測試框架，包括了 unittest、testtools、subunit、coverage、testrepository、nose、mox、mock、fixtures、discover 等等。而 unittest 是 Python 標準庫中的一個模塊，是其它框架和工具的基礎。
---

在 Python 中，有各種各樣的測試框架，包括了 unittest、testtools、subunit、coverage、testrepository、nose、mox、mock、fixtures、discover 等等。而 unittest 是 Python 標準庫中的一個模塊，是其它框架和工具的基礎。

<!-- more -->

## unittest 簡介

其時最好的資料是參考官方文檔 [unittest — Unit testing framework](http://docs.python.org/2.7/library/unittest.html)，其中包含了一些常見的概念。

在開始就是介紹四個重要的概念：test fixture、test case、test suite、test runner。

一個 test case 實例就是一個測試用例，包括了一套完整的測試流程：測試前準備環境的搭建 (setUp)，執行測試代碼 (run)，以及測試後環境的還原 (tearDown)。多個測試用例組合到一起，就是 test suite；當然 test suite 可能會是 test suite 以及 test case 的組合。


<!--
test loader 是用來加載 test Case到TestSuite中的，其中有幾個loadTestsFrom__()方法，就是從各個地方尋找TestCase，創建它們的實例，然後add到TestSuite中，再返回一個TestSuite實例。
TextTestRunner是來執行測試用例的，其中的run(test)會執行TestSuite/TestCase中的run(result)方法。
測試的結果會保存到TextTestResult實例中，包括運行了多少測試用例，成功了多少，失敗了多少等信息。


這樣整個流程就清楚了，首先是要寫好TestCase，然後由TestLoader加載TestCase到TestSuite，然後由TextTestRunner來運行TestSuite，運行的結果保存在TextTestResult中，整個過程集成在unittest.main模塊中。

現在已經涉及到了test case, test suite, test runner這三個概念了，還有test fixture沒有提到，那什麼是test fixture呢？？在TestCase的docstring中有這樣一段話：

Test authors should subclass TestCase for their own tests. Construction and deconstruction of the test's environment ('fixture') can be implemented by overriding the 'setUp' and 'tearDown' methods respectively.

可見，對一個測試用例環境的搭建和銷燬，是一個fixture，通過覆蓋TestCase的setUp()和tearDown()方法來實現。這個有什麼用呢？比如說在這個測試用例中需要訪問數據庫，那麼可以在setUp()中建立數據庫連接以及進行一些初始化，在tearDown()中清除在數據庫中產生的數據，然後關閉連接。注意tearDown的過程很重要，要為以後的TestCase留下一個乾淨的環境。關於fixture，還有一個專門的庫函數叫做fixtures，功能更加強大，以後會介紹到。

至此，概念和流程基本清楚了，下面通過簡單的例子再來實踐一下，就拿unittest文檔上的例子吧：




The test case and test fixture concepts are supported through the TestCase and FunctionTestCase classes; the former should be used when creating new tests, and the latter can be used when integrating existing test code with a unittest-driven framework. When building test fixtures using TestCase, the setUp() and tearDown() methods can be overridden to provide initialization and cleanup for the fixture. With FunctionTestCase, existing functions can be passed to the constructor for these purposes. When the test is run, the fixture initialization is run first; if it succeeds, the cleanup method is run after the test has been executed, regardless of the outcome of the test. Each instance of the TestCase will only be used to run a single test method, so a new fixture is created for each test.

Test suites are implemented by the TestSuite class. This class allows individual tests and test suites to be aggregated; when the suite is executed, all tests added directly to the suite and in “child” test suites are run.

A test runner is an object that provides a single method, run(), which accepts a TestCase or TestSuite object as a parameter, and returns a result object. The class TestResult is provided for use as the result object. unittest provides the TextTestRunner as an example test runner which reports test results on the standard error stream by default. Alternate runners can be implemented for other environments (such as graphical environments) without any need to derive from a specific class.

-->

接下來直接看一個示例，用來測試 random 庫中的部分函數。

{% highlight python %}
# -*- coding:utf-8 -*-
import random
import unittest

class TestSequenceFunctions(unittest.TestCase):
    def setUp(self):          # 初始化設置
        self.seq = range(10)
    def tearDown(self):       # 測試完成後的清理
        pass
    def test_shuffle(self):   # 將序列隨機化，需要保證沒有丟失元素
        random.shuffle(self.seq)
        self.seq.sort()
        self.assertEqual(self.seq, range(10))
        # should raise an exception for an immutable sequence
        self.assertRaises(TypeError, random.shuffle, (1,2,3))
    def test_choice(self):    # 隨機選擇其中一個元素
        element = random.choice(self.seq)
        self.assertTrue(element in self.seq)
    def test_sample(self):    # 選擇n個隨機且獨立的元素
        with self.assertRaises(ValueError):
            random.sample(self.seq, 20)
        for element in random.sample(self.seq, 5):
            self.assertTrue(element in self.seq)

if __name__ == '__main__':
    unittest.main()
{% endhighlight %}

<!--
[python] view plain copy
在CODE上查看代碼片派生到我的代碼片

    def loadTestsFromTestCase(self, testCaseClass):
        """Return a suite of all tests cases contained in testCaseClass"""
        if issubclass(testCaseClass, suite.TestSuite):
            raise TypeError("Test cases should not be derived from TestSuite." \
                                " Maybe you meant to derive from TestCase?")
        testCaseNames = self.getTestCaseNames(testCaseClass)
        if not testCaseNames and hasattr(testCaseClass, 'runTest'):
            testCaseNames = ['runTest']
        loaded_suite = self.suiteClass(map(testCaseClass, testCaseNames))
        return loaded_suite

getTestCaseNames()是從TestCase這個類中找所有以“test”開頭的方法，然後注意第9行，在構造TestSuite對象時，其參數使用了一個map方法，即對testCaseNames中的每一個元素，使用testCaseClass為其構造對象，其結果是一個TestCase的對象集合，可以用下面的代碼來分步說明：

[python] view plain copy
在CODE上查看代碼片派生到我的代碼片

    testcases = []
    for name in testCaeNames:
        testcases.append(TestCase(name))
    loaded_suite = self.suiteClass(tuple(testcases))


可見，對每一個以test開頭的方法，都為其構建了一個TestCase對象，值得注意的是，如果沒有定義test開頭的方法，而是將測試代碼寫到了一個名為runTest的方法中，那麼會為該runTest方法構建TestCase對象，如果定義了test開頭的方法，就會忽略runTest方法。

至此，基本就清楚了，每一個以test開頭的方法，都會為其構建TestCase對象，也就是說TestSequenceFunctions類中其實定義了三個TestCase，之所以寫成這樣，是為了方便，因為這幾個測試用例的fixture是相同的，如果每一個測試用例單獨寫成一個TestCase的話，會有很多的冗餘代碼。
-->



## flask 單元測試

### 簡單測試

假設當我們訪問應用的根 URL 時應該顯示 "Helle World\n"，新增了一個新的測試用例來測試:

<!--
class FlaskrTestCase(unittest.TestCase):

    def setUp(self):
        self.db_fd, flaskr.app.config['DATABASE'] = tempfile.mkstemp()
        self.app = flaskr.app.test_client()
        flaskr.init_db()

    def tearDown(self):
        os.close(self.db_fd)
        os.unlink(flaskr.app.config['DATABASE'])

    def test_empty_db(self):
        rv = self.app.get('/')
        assert 'No entries here so far' in rv.data
-->

注意，我們的調試函數都是以 test 開頭的，也就是 unittest 定義的標準，用於測試的函數並運行它們。

{% highlight python %}
# -*- coding:utf-8 -*-
import os
import foobar
import unittest

class FoobarTestCase(unittest.TestCase):
    def setUp(self):
        foobar.app.config['TESTING'] = True
        self.app = foobar.app.test_client()
        foobar.init()             # 調用應用中的命令
    def tearDown(self):
        pass
    def test_check_entry(self):
        rv = self.app.get('/')
        assert '401 Unauthorized' in rv.data

if __name__ == '__main__':
    unittest.main()
{% endhighlight %}

在如上的測試用例中，通過使用 self.app.get 嚮應用的指定 URL 發送一個 HTTP GET 請求，該函數調用會返回一個 Flask.response_class 對象；然後，可以使用 data 屬性來檢查應用的返回值。


### 登錄和註銷

很多應用的功能必須登錄以後才能使用，因此必須測試應用的登錄和註銷，因為登錄和註銷後會重定向到別的頁面，因此必須告訴客戶端使用 follow_redirects 追蹤重定向。

{% highlight python %}
def login(self, username, password):
    return self.app.post('/login', data=dict(
        username=username,
        password=password
    ), follow_redirects=True)
def logout(self):
    return self.app.get('/logout', follow_redirects=True)

def test_login_logout(self):
    rv = self.login('admin', 'default')
    assert 'You were logged in' in rv.data
    rv = self.logout()
    assert 'You were logged out' in rv.data
    rv = self.login('adminx', 'default')
    assert 'You should log in' in rv.data
{% endhighlight %}



## 使用 pytest

實際上 Python 中使用比較多的是 nose 以及 pytest，據說 nose 要更加好用一些，不過在 flask 示例中使用的是 pytest，為了便於參考，我們還是直接使用 pytest 。


<!---
https://pypi.python.org/pypi/pytest/
http://dormousehole.readthedocs.io/en/latest/testing.html
-->


## 參考

在源碼的 examples 中，有個 minitwit 示例，使用 pytest 進行測試，可以參考。


{% highlight python %}
{% endhighlight %}
