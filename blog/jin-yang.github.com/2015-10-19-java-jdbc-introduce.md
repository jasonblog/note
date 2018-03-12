---
title: Java JDBC 驱动介绍
layout: post
comments: true
language: chinese
category: [program,misc]
keywords: java,jdbc
description: JDBC (Java Datebase Connectivity) 实际是一个独立于特定数据库管理系统、通用的 SQL 数据库存取和操作的公共接口，它是 JAVA 访问数据库的一种标准。也就是说它只是接口规范，具体实现是各数据库厂商提供的驱动程序。接下来，先看看 JDBC 的使用方法，然后看看其具体的实现原理。
---

JDBC (Java Datebase Connectivity) 实际是一个独立于特定数据库管理系统、通用的 SQL 数据库存取和操作的公共接口，它是 JAVA 访问数据库的一种标准。也就是说它只是接口规范，具体实现是各数据库厂商提供的驱动程序。

接下来，先看看 JDBC 的使用方法，然后看看其具体的实现原理。

<!-- more -->

## 简介

JDBC 作为 JAVA 的接口规范，为上层提供了统一的接口。

对于 Java 中的持久化方案，可以通过 JDBC 直接访问数据库，封装接口，或者直接使用第三方 O/R (Object Relational Mapping，对象关系映射) 工具，如 Hibernate、mybatis；这些工具都是对 JDBC 的封装。

在 JDBC 中常用的接口有：

* Java.sql.Driver 接口<br>JDBC 驱动程序需要实现的接口，供数据库厂商使用，不同数据库提供不用的实现；应用程序通过驱动程序管理器类 (java.sql.DriverManager) 去调用这些 Driver 实现。

* DriverManager类<br>用来创建连接，它本身就是一个创建Connection的工厂，设计的时候使用的就是Factory模式，给各数据库厂商提供接口，各数据库厂商需要实现它； Connection接口，根据提供的不同驱动产生不同的连接； Statement 接口，用来发送SQL语句；

* Resultset 接口<br>用来接收查询语句返回的查询结果。

接下来简单介绍 JDBC 的使用方法。

### 简单使用 JDBC 接口

JDBC 使用步骤大致包括了：A) 注册加载一个驱动；B) 创建数据库连接；C) 创建 statement，发送 SQL 语句；D) 执行 SQL 语句；E) 处理返回的结果；F) 关闭 statement 和 connection 。

#### 1. 加载与注册驱动

通过调用 Class 类的静态方法 forName()，向其传递要加载的 JDBC 驱动的类名。

{% highlight java %}
Class.forName("com.mysql.jdbc.Driver");             // 注册MYSQL数据库驱动器
Class.forName("oracle.jdbc.driver.OracleDriver");   // 注册ORACLE数据库驱动器
{% endhighlight %}

#### 2. 建立连接

通过调用 DriverManager 类的 getConnection() 方法建立到数据库的连接。

{% highlight java %}
Connection conn = DriverManager.getConnection(url, uid, pwd);
{% endhighlight %}

其中，JDBC URL 用于标识一个被注册的驱动程序，驱动程序管理器通过这个 URL 选择正确的驱动程序，从而建立到数据库的连接。

JDBC URL 的标准由三部分组成，各部分间用冒号 ```":"``` 分隔，格式为 ```协议:(子协议):(子名称)```；其中，JDBC URL 中的协议总是 jdbc；子协议用于标识一个数据库驱动程序；子名称是一种标识数据库的方法，子名称根据不同的子协议而变化，用子名称的目的是为了定位数据库提供足够的信息。

{% highlight text %}
URL: jdbc:mysql://localhost:3306/mydbname           // MySQL的JDBC
URL: jdbc:oracle:thin:@localhost:1521:mydbname      // Oracle的JDBC
{% endhighlight %}

注：上述的 URL 标示的方法是一个 JNI (Java Native Interface) 方式的命名，允许 Java 代码和其他语言写的代码进行交互。

#### 3. 访问数据库

数据库连接用于向数据库服务器发送命令以及 SQL 语句，在 java.sql 包中有 3 个接口分别定义了对数据库的调用的不同方式。
{% highlight java %}
/*--- Statement 对象用于执行静态的SQL语句，并且返回执行结果 */
Statement sm = conn.createStatement();                 // 创建该对象
sm.executeQuery(sql);                                  // 数据查询语句select
sm.executeUpdate(sql);                                 // 数据更新语句delete、update、insert、drop等

/*--- PreparedStatement 接口是Statement的子接口，它表示一条预编译过的SQL语句
 * 该对象所代表的SQL语句中的参数用问号表示，并调用PreparedStatement对象的setXXX()方法来设置这些参数
 */
String sql = "INSERT INTO user (id,name) VALUES (?,?)";
PreparedStatement ps = conn.prepareStatement(sql);     // 创建对象
ps.setInt(1, 1);                                       // 设置参数
ps.setString(2, "admin");
ResultSet rs = ps.executeQuery();                      // 查询操作
int c = ps.executeUpdate();                            // 更新操作


/*--- Callable Statement 当不直接使用 SQL 语句，而是调用数据库中的存储过程时，要用到该接口
 * 其中 CallabelStatement 是从 PreparedStatement 继承
 */
String sql = "{call insert_users(?,?)}";
CallableStatement st = conn.prepareCall(sql);          // 调用存储过程
st.setInt(1, 1);
st.setString(2, "admin");
st.execute();                                          // 执行SQL语句，可以是任何类型的SQL
{% endhighlight %}

代码中建议使用 PreparedStatement，而非 Statement；相比来说，前者的代码可读性和可维护性较高，可以提高 SQL 执行性能，更加安全（如防止 sql 注入）。

SQL 注入是利用某些系统没有对用户输入的数据进行充分的检查，而在用户输入数据中注入非法的 SQL 语句段或命令，从而利用系统的 SQL 引擎完成恶意行为的做法。

#### 4. 处理执行结果

查询语句，返回记录集 ResultSet；更新语句，返回数字，表示该更新影响的记录数。该对象以逻辑表格的形式封装了查询操作的结果集，相关接口由数据库驱动实现。

ResultSet 接口的常用方法：通过 ```next()``` 将游标往后移动一行，成功返回true，该对象维护了一个指向当前数据行的游标，初始的时候，游标在第一行之前，可以通过 ResultSet 对象的 ```next()``` 方法移动到下一行。

```getXXX(String name)```：返回当前游标下某个字段的值，如：```getInt("id")``` 或 ```getSting("name")```。

#### 5. 释放数据库连接

一般是在 finally 语句里面进行释放资源。

{% highlight java %}
rs.close();                                            // 关闭结果集
ps.close(); / st.close(); / sm.close();                // 相应地接口
conn.close();                                          // 关闭链接
{% endhighlight %}

下图是使用 JDBC 的通用流程。

![jdbc-basic-cycle]{: .pull-center}

相关的示例可以直接从 [JDBC 简单示例][jdbc-simple-example] 下载，或者参考如下代码：

{% highlight java %}
// javac GetConnection.java
// java -classpath .:/usr/share/java/mysql-connector-java.jar GetConnection

import java.sql.*;
import java.text.SimpleDateFormat;

public class GetConnection {
    public static void main(String[] args){
        try{
            // 调用Class.forName()方法加载驱动程序
            Class.forName("com.mysql.jdbc.Driver");
            System.out.println("成功加载MySQL驱动！");
        }catch(ClassNotFoundException e1){
            System.out.println("找不到MySQL驱动!");
            e1.printStackTrace();
        }

        String url="jdbc:mysql://localhost:3306/mysql";    // JDBC的URL
        // 调用DriverManager对象的getConnection()方法，获得一个Connection对象
        try {
            Connection conn;
            conn = DriverManager.getConnection(url,"root","password");
            Statement stmt = conn.createStatement(); // 创建Statement对象
            System.out.println("成功连接到数据库！");
            ResultSet rs = stmt.executeQuery("select now() now");
            while(rs.next()){
                //Retrieve by column name
                java.sql.Timestamp time = rs.getTimestamp("now");
                SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
                String str = sdf.format(time);
                System.out.println("NOW: " + str);
            }
            System.out.println("释放相关资源！");
            rs.close();
            stmt.close();
            conn.close();
        } catch (SQLException e){
            e.printStackTrace();
        }
    }
}
{% endhighlight %}

如果还没有安装 JDBC 可以通过如下的方式安装。

在 CentOS 中可以通过 ```yum install mysql-connector-java``` 命令安装 JDBC 驱动，此时，默认会保存在 ```/usr/share/java/mysql-connector-java.jar``` ，使用这个包需要包含全路径，可以设置 ```CLASSPATH``` 环境变量，或者在运行时通过 ```-classpath``` 指定：

{% highlight java %}
$ javac GetConnection
$ java -classpath .:/usr/share/java/mysql-connector-java.jar GetConnection
{% endhighlight %}

在 Eclipse 中可通过 ```[右健] -> Properties -> Java Build Path -> Libraries -> Add External JARs``` 。

当然，也可以从 [MySQL官网](http://www.mysql.com/products/connector/) 下载不同的 MySQL Connectors 版本。

### 批量处理

需要批量插入或者更新记录时，可以采用 Java 的批量更新机制，这一机制允许多条语句一次性提交给数据库批量处理，通常比单独提交处理更有效率。有如下两种方式：

{% highlight java %}
/*--- Statement批量处理 */
stmt.addBatch("insert into test value(1, 'foobar')"); // 添加需要批量处理的SQL语句或是参数
stmt.addBatch("insert into test value(2, 'oooops')");
stmt.addBatch("select * from test");
stmt.executeBatch();                                  // 执行批量处理语句，返回影响行数
stmt.clearBatch();                                    // 清除stmt中积攒的参数列表

/*--- PreparedStatement批量传参 */
PreparedStatement ps = conn.preparedStatement(sql);
for(int i=1; i<100000; i++) {
    ps.setInt(1, i);
    ps.setString(2, "name"+i);
    ps.setString(3, "email"+i);
    ps.addBatch();
    if((i+1)%1000==0){
        ps.executeBatch();   // 批量处理
        ps.clearBatch();     // 清空ps中积攒的sql
    }
}
{% endhighlight %}

通常会遇到两种批量执行 SQL 的情况：多条 SQL 语句批量处理；一个 SQL 语句的批量传参。实际上，上述的执行过程仍然是串行的，也就是说当第一条执行完成后，才会执行下一条，可以执行不同类型的 DML。批量执行的 SQL 最终接口为 ```executeBatchInternal()@PreparedStatement.class``` 。

{% highlight java %}
protected long[] executeBatchInternal() throws SQLException {
    ... ...
    try {
        if (!this.batchHasPlainStatements && this.connection.getRewriteBatchedStatements()) {

            if (canRewriteAsMultiValueInsertAtSqlLevel()) {
                return executeBatchedInserts(batchTimeout);
            }

            if (this.connection.versionMeetsMinimum(4, 1, 0) && !this.batchHasPlainStatements
                 && this.batchedArgs != null
                 && this.batchedArgs.size() > 3 /* cost of option setting rt-wise */) {
                return executePreparedBatchAsMultiStatement(batchTimeout);
            }
        }

        return executeBatchSerially(batchTimeout);
    } finally {
        this.statementExecuting.set(false);
        clearBatch();
    }
    ... ...
}
{% endhighlight %}

如上所述，JDBC 驱动默认情况下会无视 `executeBatch()` 语句，把期望批量执行的一组 SQL 语句拆散，一条一条地发给 MySQL 数据库，直接造成较低的性能。

而且在没有设置 autocommit 时，也是每条 SQL 提交一次。

只有把 `rewriteBatchedStatements` 参数置为 `true`，驱动才会帮你批量执行 SQL 。

{% highlight text %}
jdbc:mysql://ip:port/db?rewriteBatchedStatements=true
{% endhighlight %}

`rewriteBatchedStatements` 对于 `INSERT`、`UPDATE`、`DELETE` 都有效，只不过对 `INSERT` 会预先重排一下 SQL 语句。

{% highlight text %}
batchDelete(10条): 一次请求，内容: "delete from t where id = 1; delete from t where id = 2; ......"
batchUpdate(10条): 一次请求，内容: "update t set ... where id = 1; update t set ... where id = 2; ......"
batchInsert(10条): 一次请求，内容: "insert into t(...) values(...), (...), ..."
{% endhighlight %}

需要注意的是，即使 `rewriteBatchedStatements=true`，`batchDelete()` 和 `batchUpdate()` 也不一定会走批量。当 `batchSize<=3` 时，驱动会宁愿一条一条地执行 SQL 。

其它的操作如执行事务、获取元数据、批量执行等操作，以后再补充。

## 源码解析

如上所述，JDBC 提供了一个统一的接口，对于不同的驱动，除了加载驱动以及建立链接时的 URL 不同只外，其它的操作基本相同。

除了需要 JDBC 的源码之外，还需要 openjdk 的源码。

![jdbc-structure]{: .pull-center width="700"}

DriverManager 类，实际是在 rt.jar 包中，而其源码在 openjdk 中，对应 ```DriverManager.java``` 。

### 注册加载

应用程序首先通过如下程序加载驱动器。

{% highlight java %}
Class.forName("com.mysql.jdbc.Driver");
{% endhighlight %}

首先介绍一下什么是 ```Class.forName()``` 。

#### Class.forName()

简单来说 ```Class.forName(xxx.xx)``` 就是要求 JVM 查找并加载指定的类，在加载的同时会执行该类的静态代码段，其返回值是一个类。

而 ```newInstance()``` 就是根据类初始化一个实例，也就是说如下的两种方式，其效果是一样的：

{% highlight java %}
A a = (A)Class.forName("package.A").newInstance();
A a = new A();
{% endhighlight %}

```Class.forName().newInstance()``` 和 ```new()``` 区别是：A) 前者是一个方法，后者是一个关键字；B) 前者生成对象只能调用无参的构造函数，而后者没有限制。

使用 ```Class.forName()``` 的目的是为了动态加载类，如需要实例化对象，还需要调用 ```newInstance()``` 。而实际上，在我们的示例中，并没有调用 ```newInstance()```，Why？？？

如上所述，JVM 要先加载 class，而静态代码是和 class 绑定的，class 装载成功就表示执行了你的静态代码了，而且以后的实例化等操作将不会再执行这段静态代码了。

实际上，JDBC 规范中明确要求这个 Driver 类必须向 DriverManager 注册自己，例如，对于 MySQL 来说，在 ```src/com/mysql/jdbc/Driver.java``` 中，实现了 ```java.sql.Driver``` 接口，在初始化时有如下操作：

{% highlight java %}
public class Driver extends NonRegisteringDriver implements java.sql.Driver {
    static {
        try {
            java.sql.DriverManager.registerDriver(new Driver());
        } catch (SQLException E) {
            throw new RuntimeException("Can't register driver!");
        }
    }

    public Driver() throws SQLException {
        // Required for Class.forName().newInstance()
    }
}
{% endhighlight %}

也就是说，对于 JDBC 是否使用了 ```newInstance()```，两者是一样的。

### DriverManager

在继续讲解之前，可以看到，对应 DriverManager 类有一个静态匿名函数，也就是执行 ```loadInitialDrivers()``` 函数。

在 ```jdk/src/share/classes/java/sql/DriverManager.java``` 中，通过 ```registeredDrivers.addIfAbsent()``` 将新建的 DriverInfo 信息保存。

### 获取链接

实际上获取链接有多个接口，其入参是不同的，通常会使用如下的接口。在该函数中，如果正常，则会返回一个链接，否则会返回 NULL 。

{% highlight java %}
Drivermanager.getConnection(url, name, password);
{% endhighlight %}

而对于所有的接口，最终会调用如下的私有函数：

{% highlight java %}
private static Connection getConnection(
    String url, java.util.Properties info, Class caller) throws SQLException {
    ... ...
    for(DriverInfo aDriver : registeredDrivers) {
        Connection con = aDriver.driver.connect(url, info);
    }
    ... ...
}
{% endhighlight %}

也就是最终会调用 JDBC-MySQL Driver 的 ```connect()``` 函数。该函数先通过 ```parseURL()``` 解析 URL 中的信息，并保存在 Properties 中，该对象用于保存一些配置信息。

{% highlight java %}
// ### 1  @src/com/mysql/jdbc/NonRegisteringDriver.java
public java.sql.Connection connect(String url, Properties info) throws SQLException {
    ... ...                       // 对传入的URL进行一些判断，如负载均衡等

    // parseURL函数声明：public Properties parseURL(String url, Properties defaults)
    if ((props = parseURL(url, info)) == null) {
        return null;
    }
    ... ...
    try {                        // 解析完参数后返回Properties对象，然后会初始化链接
        Connection newConn = com.mysql.jdbc.ConnectionImpl.getInstance(
            host(props), port(props), props, database(props), url);
        return newConn;
    }
    ... ...
}

// ### 2
protected static Connection getInstance(...) throws SQLException {
    if (!Util.isJdbc4()) {       // 老接口
        return new ConnectionImpl(hostToConnectTo, portToConnectTo, info, databaseToConnectTo, url);
    }

    return (Connection) Util.handleNewInstance(JDBC_4_CONNECTION_CTOR,
        new Object[] { hostToConnectTo, Integer.valueOf(portToConnectTo), info, databaseToConnectTo, url }, null);
}
{% endhighlight %}

而 ```Util.handleNewInstance()``` 函数实际执行 ```Class.forName("com.mysql.jdbc.JDBC4Connection").newInstance()``` 。该函数会调用基类 ```class ConnectionImpl``` 的构造函数。

也就是说，对于 JDBC4 最终调用的仍然与老接口类似。

{% highlight java %}
public ConnectionImpl(...) throws SQLException {
    ... ...
    try {
        this.dbmd = getMetaData(false, false);    // 获取数据库信息
        initializeSafeStatementInterceptors();
        createNewIO(false);                       // 创建远程IO链接
        unSafeStatementInterceptors();
    }
    ... ...
}
{% endhighlight %}

### Socket的连接创建

连接创建是通过 ```ConnectionImp::createNewIO()``` 方法执行。

{% highlight java %}
public void createNewIO(boolean isForReconnect) throws SQLException {
    ... ...
    connectOneTryOnly()
    ... ...
}

private void connectOneTryOnly(...) throws SQLException {
    try {
        ... ...
        coreConnect(mergedProps);
    }
    ... ...
}

private void coreConnect(Properties mergedProps) throws SQLException, IOException {
    ... ...
    this.io = new MysqlIO(...);   // MysqlIO的创建，用来于服务器进行通信
    this.io.doHandshake(this.user, this.password, this.database);
    ... ...
}

public MysqlIO(...) {
    ... ...
    // 创建得到一个socket
    this.mysqlConnection = this.socketFactory.connect(this.host, this.port, props);
    ... ...
    // 创建input流
    if (this.connection.getUseReadAheadInput()) {
        ... ...
    }
    // 创建output流
    this.mysqlOutput = new BufferedOutputStream(this.mysqlConnection.getOutputStream(), 16384);
}
{% endhighlight %}

### 执行SQL

```createStatement()``` 有三类接口，最终都会调用如下接口。其中前两个参数为：对返回的结果集进行指定相应的模式功能，可参照 ResultSet 的常量设置。

{% highlight java %}
public java.sql.Statement createStatement(...) throws SQLException {
    checkClosed();
    // getLoadBalanceSafeProxy() 为相应的连接
    StatementImpl stmt = new StatementImpl(getLoadBalanceSafeProxy(), this.database);
    stmt.setResultSetType(resultSetType);
    stmt.setResultSetConcurrency(resultSetConcurrency);

    return stmt;
}

public java.sql.ResultSet executeQuery(String sql) throws SQLException {
    ... ...
    if (locallyScopedConn.getCacheResultSetMetadata()) { // 是否应用缓存ResultSetMeta
    }

    // ConnectionImpl中执行sql语句
    this.results = locallyScopedConn.execSQL(this, sql, -1, null,
            this.resultSetType, this.resultSetConcurrency,
            doStreaming,
            this.currentCatalog, cachedFields);
    ... ...
}

public synchronized ResultSetInternalMethods execSQL(...) {
    ... ...
    // 进入MysqlIO中执行查询操作
    return this.io.sqlQueryDirect(callingStatement, sql,
            encoding, null, maxRows, resultSetType,
            resultSetConcurrency, streamResults, catalog,
            cachedMetadata);
    ... ...
}

final ResultSetInternalMethods sqlQueryDirect(...) throws Exception {
    ... ...
    // 发送查询命与sql查询语句，并得到查询结果(socket处理)
    Buffer resultPacket = sendCommand(MysqlDefs.QUERY, null, queryPacket, false, null, 0);
    ... ...
    // 封装成ResultSet
    ResultSetInternalMethods rs = readAllResults(callingStatement, maxRows, resultSetType,
            resultSetConcurrency, streamResults, catalog, resultPacket,
            false, -1L, cachedMetadata);
    ... ...
}
{% endhighlight %}

很多接口还没有深入了解，有时间继续。

## 参考


<!--
http://www.mamicode.com/info-detail-965290.html         【JDBC发展史】从JDBC1.0到JDBC4.0
http://shmilyaw-hotmail-com.iteye.com/blog/1926513       ServiceLoader和DriverManager使用总结
http://wuquanyin1011.iteye.com/blog/1409455              MySQL JDBC 驱动源码分析
-->


[jdbc-basic-cycle]:       /images/java/jdba_basic_cycle.png                        "JDBC的基本调用流程"
[jdbc-structure]:         /images/java/jdbc_structure.png                          "JDBC框架整体架构"
[jdbc-simple-example]:    /reference/java/GetConnection.java                       "使用JDBC的简单示例"
