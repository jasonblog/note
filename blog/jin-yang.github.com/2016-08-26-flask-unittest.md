---
title: Flask 单元测试
layout: post
comments: true
language: chinese
category: [webserver]
keywords: flask,单元测试,unittest
description: 在 Python 中，有各种各样的测试框架，包括了 unittest、testtools、subunit、coverage、testrepository、nose、mox、mock、fixtures、discover 等等。而 unittest 是 Python 标准库中的一个模块，是其它框架和工具的基础。
---

在 Python 中，有各种各样的测试框架，包括了 unittest、testtools、subunit、coverage、testrepository、nose、mox、mock、fixtures、discover 等等。而 unittest 是 Python 标准库中的一个模块，是其它框架和工具的基础。

<!-- more -->

## unittest 简介

其时最好的资料是参考官方文档 [unittest — Unit testing framework](http://docs.python.org/2.7/library/unittest.html)，其中包含了一些常见的概念。

在开始就是介绍四个重要的概念：test fixture、test case、test suite、test runner。

一个 test case 实例就是一个测试用例，包括了一套完整的测试流程：测试前准备环境的搭建 (setUp)，执行测试代码 (run)，以及测试后环境的还原 (tearDown)。多个测试用例组合到一起，就是 test suite；当然 test suite 可能会是 test suite 以及 test case 的组合。


<!--
test loader 是用来加载 test Case到TestSuite中的，其中有几个loadTestsFrom__()方法，就是从各个地方寻找TestCase，创建它们的实例，然后add到TestSuite中，再返回一个TestSuite实例。
TextTestRunner是来执行测试用例的，其中的run(test)会执行TestSuite/TestCase中的run(result)方法。
测试的结果会保存到TextTestResult实例中，包括运行了多少测试用例，成功了多少，失败了多少等信息。


这样整个流程就清楚了，首先是要写好TestCase，然后由TestLoader加载TestCase到TestSuite，然后由TextTestRunner来运行TestSuite，运行的结果保存在TextTestResult中，整个过程集成在unittest.main模块中。

现在已经涉及到了test case, test suite, test runner这三个概念了，还有test fixture没有提到，那什么是test fixture呢？？在TestCase的docstring中有这样一段话：

Test authors should subclass TestCase for their own tests. Construction and deconstruction of the test's environment ('fixture') can be implemented by overriding the 'setUp' and 'tearDown' methods respectively.

可见，对一个测试用例环境的搭建和销毁，是一个fixture，通过覆盖TestCase的setUp()和tearDown()方法来实现。这个有什么用呢？比如说在这个测试用例中需要访问数据库，那么可以在setUp()中建立数据库连接以及进行一些初始化，在tearDown()中清除在数据库中产生的数据，然后关闭连接。注意tearDown的过程很重要，要为以后的TestCase留下一个干净的环境。关于fixture，还有一个专门的库函数叫做fixtures，功能更加强大，以后会介绍到。

至此，概念和流程基本清楚了，下面通过简单的例子再来实践一下，就拿unittest文档上的例子吧：




The test case and test fixture concepts are supported through the TestCase and FunctionTestCase classes; the former should be used when creating new tests, and the latter can be used when integrating existing test code with a unittest-driven framework. When building test fixtures using TestCase, the setUp() and tearDown() methods can be overridden to provide initialization and cleanup for the fixture. With FunctionTestCase, existing functions can be passed to the constructor for these purposes. When the test is run, the fixture initialization is run first; if it succeeds, the cleanup method is run after the test has been executed, regardless of the outcome of the test. Each instance of the TestCase will only be used to run a single test method, so a new fixture is created for each test.

Test suites are implemented by the TestSuite class. This class allows individual tests and test suites to be aggregated; when the suite is executed, all tests added directly to the suite and in “child” test suites are run.

A test runner is an object that provides a single method, run(), which accepts a TestCase or TestSuite object as a parameter, and returns a result object. The class TestResult is provided for use as the result object. unittest provides the TextTestRunner as an example test runner which reports test results on the standard error stream by default. Alternate runners can be implemented for other environments (such as graphical environments) without any need to derive from a specific class.

-->

接下来直接看一个示例，用来测试 random 库中的部分函数。

{% highlight python %}
# -*- coding:utf-8 -*-
import random
import unittest

class TestSequenceFunctions(unittest.TestCase):
    def setUp(self):          # 初始化设置
        self.seq = range(10)
    def tearDown(self):       # 测试完成后的清理
        pass
    def test_shuffle(self):   # 将序列随机化，需要保证没有丢失元素
        random.shuffle(self.seq)
        self.seq.sort()
        self.assertEqual(self.seq, range(10))
        # should raise an exception for an immutable sequence
        self.assertRaises(TypeError, random.shuffle, (1,2,3))
    def test_choice(self):    # 随机选择其中一个元素
        element = random.choice(self.seq)
        self.assertTrue(element in self.seq)
    def test_sample(self):    # 选择n个随机且独立的元素
        with self.assertRaises(ValueError):
            random.sample(self.seq, 20)
        for element in random.sample(self.seq, 5):
            self.assertTrue(element in self.seq)

if __name__ == '__main__':
    unittest.main()
{% endhighlight %}

<!--
[python] view plain copy
在CODE上查看代码片派生到我的代码片

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

getTestCaseNames()是从TestCase这个类中找所有以“test”开头的方法，然后注意第9行，在构造TestSuite对象时，其参数使用了一个map方法，即对testCaseNames中的每一个元素，使用testCaseClass为其构造对象，其结果是一个TestCase的对象集合，可以用下面的代码来分步说明：

[python] view plain copy
在CODE上查看代码片派生到我的代码片

    testcases = []
    for name in testCaeNames:
        testcases.append(TestCase(name))
    loaded_suite = self.suiteClass(tuple(testcases))


可见，对每一个以test开头的方法，都为其构建了一个TestCase对象，值得注意的是，如果没有定义test开头的方法，而是将测试代码写到了一个名为runTest的方法中，那么会为该runTest方法构建TestCase对象，如果定义了test开头的方法，就会忽略runTest方法。

至此，基本就清楚了，每一个以test开头的方法，都会为其构建TestCase对象，也就是说TestSequenceFunctions类中其实定义了三个TestCase，之所以写成这样，是为了方便，因为这几个测试用例的fixture是相同的，如果每一个测试用例单独写成一个TestCase的话，会有很多的冗余代码。
-->



## flask 单元测试

### 简单测试

假设当我们访问应用的根 URL 时应该显示 "Helle World\n"，新增了一个新的测试用例来测试:

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

注意，我们的调试函数都是以 test 开头的，也就是 unittest 定义的标准，用于测试的函数并运行它们。

{% highlight python %}
# -*- coding:utf-8 -*-
import os
import foobar
import unittest

class FoobarTestCase(unittest.TestCase):
    def setUp(self):
        foobar.app.config['TESTING'] = True
        self.app = foobar.app.test_client()
        foobar.init()             # 调用应用中的命令
    def tearDown(self):
        pass
    def test_check_entry(self):
        rv = self.app.get('/')
        assert '401 Unauthorized' in rv.data

if __name__ == '__main__':
    unittest.main()
{% endhighlight %}

在如上的测试用例中，通过使用 self.app.get 向应用的指定 URL 发送一个 HTTP GET 请求，该函数调用会返回一个 Flask.response_class 对象；然后，可以使用 data 属性来检查应用的返回值。


### 登录和注销

很多应用的功能必须登录以后才能使用，因此必须测试应用的登录和注销，因为登录和注销后会重定向到别的页面，因此必须告诉客户端使用 follow_redirects 追踪重定向。

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

实际上 Python 中使用比较多的是 nose 以及 pytest，据说 nose 要更加好用一些，不过在 flask 示例中使用的是 pytest，为了便于参考，我们还是直接使用 pytest 。


<!---
https://pypi.python.org/pypi/pytest/
http://dormousehole.readthedocs.io/en/latest/testing.html
-->


## 参考

在源码的 examples 中，有个 minitwit 示例，使用 pytest 进行测试，可以参考。


{% highlight python %}
{% endhighlight %}
