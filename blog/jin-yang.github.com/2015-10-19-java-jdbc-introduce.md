---
title: Java JDBC 驅動介紹
layout: post
comments: true
language: chinese
category: [program,misc]
keywords: java,jdbc
description: JDBC (Java Datebase Connectivity) 實際是一個獨立於特定數據庫管理系統、通用的 SQL 數據庫存取和操作的公共接口，它是 JAVA 訪問數據庫的一種標準。也就是說它只是接口規範，具體實現是各數據庫廠商提供的驅動程序。接下來，先看看 JDBC 的使用方法，然後看看其具體的實現原理。
---

JDBC (Java Datebase Connectivity) 實際是一個獨立於特定數據庫管理系統、通用的 SQL 數據庫存取和操作的公共接口，它是 JAVA 訪問數據庫的一種標準。也就是說它只是接口規範，具體實現是各數據庫廠商提供的驅動程序。

接下來，先看看 JDBC 的使用方法，然後看看其具體的實現原理。

<!-- more -->

## 簡介

JDBC 作為 JAVA 的接口規範，為上層提供了統一的接口。

對於 Java 中的持久化方案，可以通過 JDBC 直接訪問數據庫，封裝接口，或者直接使用第三方 O/R (Object Relational Mapping，對象關係映射) 工具，如 Hibernate、mybatis；這些工具都是對 JDBC 的封裝。

在 JDBC 中常用的接口有：

* Java.sql.Driver 接口<br>JDBC 驅動程序需要實現的接口，供數據庫廠商使用，不同數據庫提供不用的實現；應用程序通過驅動程序管理器類 (java.sql.DriverManager) 去調用這些 Driver 實現。

* DriverManager類<br>用來創建連接，它本身就是一個創建Connection的工廠，設計的時候使用的就是Factory模式，給各數據庫廠商提供接口，各數據庫廠商需要實現它； Connection接口，根據提供的不同驅動產生不同的連接； Statement 接口，用來發送SQL語句；

* Resultset 接口<br>用來接收查詢語句返回的查詢結果。

接下來簡單介紹 JDBC 的使用方法。

### 簡單使用 JDBC 接口

JDBC 使用步驟大致包括了：A) 註冊加載一個驅動；B) 創建數據庫連接；C) 創建 statement，發送 SQL 語句；D) 執行 SQL 語句；E) 處理返回的結果；F) 關閉 statement 和 connection 。

#### 1. 加載與註冊驅動

通過調用 Class 類的靜態方法 forName()，向其傳遞要加載的 JDBC 驅動的類名。

{% highlight java %}
Class.forName("com.mysql.jdbc.Driver");             // 註冊MYSQL數據庫驅動器
Class.forName("oracle.jdbc.driver.OracleDriver");   // 註冊ORACLE數據庫驅動器
{% endhighlight %}

#### 2. 建立連接

通過調用 DriverManager 類的 getConnection() 方法建立到數據庫的連接。

{% highlight java %}
Connection conn = DriverManager.getConnection(url, uid, pwd);
{% endhighlight %}

其中，JDBC URL 用於標識一個被註冊的驅動程序，驅動程序管理器通過這個 URL 選擇正確的驅動程序，從而建立到數據庫的連接。

JDBC URL 的標準由三部分組成，各部分間用冒號 ```":"``` 分隔，格式為 ```協議:(子協議):(子名稱)```；其中，JDBC URL 中的協議總是 jdbc；子協議用於標識一個數據庫驅動程序；子名稱是一種標識數據庫的方法，子名稱根據不同的子協議而變化，用子名稱的目的是為了定位數據庫提供足夠的信息。

{% highlight text %}
URL: jdbc:mysql://localhost:3306/mydbname           // MySQL的JDBC
URL: jdbc:oracle:thin:@localhost:1521:mydbname      // Oracle的JDBC
{% endhighlight %}

注：上述的 URL 標示的方法是一個 JNI (Java Native Interface) 方式的命名，允許 Java 代碼和其他語言寫的代碼進行交互。

#### 3. 訪問數據庫

數據庫連接用於向數據庫服務器發送命令以及 SQL 語句，在 java.sql 包中有 3 個接口分別定義了對數據庫的調用的不同方式。
{% highlight java %}
/*--- Statement 對象用於執行靜態的SQL語句，並且返回執行結果 */
Statement sm = conn.createStatement();                 // 創建該對象
sm.executeQuery(sql);                                  // 數據查詢語句select
sm.executeUpdate(sql);                                 // 數據更新語句delete、update、insert、drop等

/*--- PreparedStatement 接口是Statement的子接口，它表示一條預編譯過的SQL語句
 * 該對象所代表的SQL語句中的參數用問號表示，並調用PreparedStatement對象的setXXX()方法來設置這些參數
 */
String sql = "INSERT INTO user (id,name) VALUES (?,?)";
PreparedStatement ps = conn.prepareStatement(sql);     // 創建對象
ps.setInt(1, 1);                                       // 設置參數
ps.setString(2, "admin");
ResultSet rs = ps.executeQuery();                      // 查詢操作
int c = ps.executeUpdate();                            // 更新操作


/*--- Callable Statement 當不直接使用 SQL 語句，而是調用數據庫中的存儲過程時，要用到該接口
 * 其中 CallabelStatement 是從 PreparedStatement 繼承
 */
String sql = "{call insert_users(?,?)}";
CallableStatement st = conn.prepareCall(sql);          // 調用存儲過程
st.setInt(1, 1);
st.setString(2, "admin");
st.execute();                                          // 執行SQL語句，可以是任何類型的SQL
{% endhighlight %}

代碼中建議使用 PreparedStatement，而非 Statement；相比來說，前者的代碼可讀性和可維護性較高，可以提高 SQL 執行性能，更加安全（如防止 sql 注入）。

SQL 注入是利用某些系統沒有對用戶輸入的數據進行充分的檢查，而在用戶輸入數據中注入非法的 SQL 語句段或命令，從而利用系統的 SQL 引擎完成惡意行為的做法。

#### 4. 處理執行結果

查詢語句，返回記錄集 ResultSet；更新語句，返回數字，表示該更新影響的記錄數。該對象以邏輯表格的形式封裝了查詢操作的結果集，相關接口由數據庫驅動實現。

ResultSet 接口的常用方法：通過 ```next()``` 將遊標往後移動一行，成功返回true，該對象維護了一個指向當前數據行的遊標，初始的時候，遊標在第一行之前，可以通過 ResultSet 對象的 ```next()``` 方法移動到下一行。

```getXXX(String name)```：返回當前遊標下某個字段的值，如：```getInt("id")``` 或 ```getSting("name")```。

#### 5. 釋放數據庫連接

一般是在 finally 語句裡面進行釋放資源。

{% highlight java %}
rs.close();                                            // 關閉結果集
ps.close(); / st.close(); / sm.close();                // 相應地接口
conn.close();                                          // 關閉鏈接
{% endhighlight %}

下圖是使用 JDBC 的通用流程。

![jdbc-basic-cycle]{: .pull-center}

相關的示例可以直接從 [JDBC 簡單示例][jdbc-simple-example] 下載，或者參考如下代碼：

{% highlight java %}
// javac GetConnection.java
// java -classpath .:/usr/share/java/mysql-connector-java.jar GetConnection

import java.sql.*;
import java.text.SimpleDateFormat;

public class GetConnection {
    public static void main(String[] args){
        try{
            // 調用Class.forName()方法加載驅動程序
            Class.forName("com.mysql.jdbc.Driver");
            System.out.println("成功加載MySQL驅動！");
        }catch(ClassNotFoundException e1){
            System.out.println("找不到MySQL驅動!");
            e1.printStackTrace();
        }

        String url="jdbc:mysql://localhost:3306/mysql";    // JDBC的URL
        // 調用DriverManager對象的getConnection()方法，獲得一個Connection對象
        try {
            Connection conn;
            conn = DriverManager.getConnection(url,"root","password");
            Statement stmt = conn.createStatement(); // 創建Statement對象
            System.out.println("成功連接到數據庫！");
            ResultSet rs = stmt.executeQuery("select now() now");
            while(rs.next()){
                //Retrieve by column name
                java.sql.Timestamp time = rs.getTimestamp("now");
                SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
                String str = sdf.format(time);
                System.out.println("NOW: " + str);
            }
            System.out.println("釋放相關資源！");
            rs.close();
            stmt.close();
            conn.close();
        } catch (SQLException e){
            e.printStackTrace();
        }
    }
}
{% endhighlight %}

如果還沒有安裝 JDBC 可以通過如下的方式安裝。

在 CentOS 中可以通過 ```yum install mysql-connector-java``` 命令安裝 JDBC 驅動，此時，默認會保存在 ```/usr/share/java/mysql-connector-java.jar``` ，使用這個包需要包含全路徑，可以設置 ```CLASSPATH``` 環境變量，或者在運行時通過 ```-classpath``` 指定：

{% highlight java %}
$ javac GetConnection
$ java -classpath .:/usr/share/java/mysql-connector-java.jar GetConnection
{% endhighlight %}

在 Eclipse 中可通過 ```[右健] -> Properties -> Java Build Path -> Libraries -> Add External JARs``` 。

當然，也可以從 [MySQL官網](http://www.mysql.com/products/connector/) 下載不同的 MySQL Connectors 版本。

### 批量處理

需要批量插入或者更新記錄時，可以採用 Java 的批量更新機制，這一機制允許多條語句一次性提交給數據庫批量處理，通常比單獨提交處理更有效率。有如下兩種方式：

{% highlight java %}
/*--- Statement批量處理 */
stmt.addBatch("insert into test value(1, 'foobar')"); // 添加需要批量處理的SQL語句或是參數
stmt.addBatch("insert into test value(2, 'oooops')");
stmt.addBatch("select * from test");
stmt.executeBatch();                                  // 執行批量處理語句，返回影響行數
stmt.clearBatch();                                    // 清除stmt中積攢的參數列表

/*--- PreparedStatement批量傳參 */
PreparedStatement ps = conn.preparedStatement(sql);
for(int i=1; i<100000; i++) {
    ps.setInt(1, i);
    ps.setString(2, "name"+i);
    ps.setString(3, "email"+i);
    ps.addBatch();
    if((i+1)%1000==0){
        ps.executeBatch();   // 批量處理
        ps.clearBatch();     // 清空ps中積攢的sql
    }
}
{% endhighlight %}

通常會遇到兩種批量執行 SQL 的情況：多條 SQL 語句批量處理；一個 SQL 語句的批量傳參。實際上，上述的執行過程仍然是串行的，也就是說當第一條執行完成後，才會執行下一條，可以執行不同類型的 DML。批量執行的 SQL 最終接口為 ```executeBatchInternal()@PreparedStatement.class``` 。

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

如上所述，JDBC 驅動默認情況下會無視 `executeBatch()` 語句，把期望批量執行的一組 SQL 語句拆散，一條一條地發給 MySQL 數據庫，直接造成較低的性能。

而且在沒有設置 autocommit 時，也是每條 SQL 提交一次。

只有把 `rewriteBatchedStatements` 參數置為 `true`，驅動才會幫你批量執行 SQL 。

{% highlight text %}
jdbc:mysql://ip:port/db?rewriteBatchedStatements=true
{% endhighlight %}

`rewriteBatchedStatements` 對於 `INSERT`、`UPDATE`、`DELETE` 都有效，只不過對 `INSERT` 會預先重排一下 SQL 語句。

{% highlight text %}
batchDelete(10條): 一次請求，內容: "delete from t where id = 1; delete from t where id = 2; ......"
batchUpdate(10條): 一次請求，內容: "update t set ... where id = 1; update t set ... where id = 2; ......"
batchInsert(10條): 一次請求，內容: "insert into t(...) values(...), (...), ..."
{% endhighlight %}

需要注意的是，即使 `rewriteBatchedStatements=true`，`batchDelete()` 和 `batchUpdate()` 也不一定會走批量。當 `batchSize<=3` 時，驅動會寧願一條一條地執行 SQL 。

其它的操作如執行事務、獲取元數據、批量執行等操作，以後再補充。

## 源碼解析

如上所述，JDBC 提供了一個統一的接口，對於不同的驅動，除了加載驅動以及建立鏈接時的 URL 不同只外，其它的操作基本相同。

除了需要 JDBC 的源碼之外，還需要 openjdk 的源碼。

![jdbc-structure]{: .pull-center width="700"}

DriverManager 類，實際是在 rt.jar 包中，而其源碼在 openjdk 中，對應 ```DriverManager.java``` 。

### 註冊加載

應用程序首先通過如下程序加載驅動器。

{% highlight java %}
Class.forName("com.mysql.jdbc.Driver");
{% endhighlight %}

首先介紹一下什麼是 ```Class.forName()``` 。

#### Class.forName()

簡單來說 ```Class.forName(xxx.xx)``` 就是要求 JVM 查找並加載指定的類，在加載的同時會執行該類的靜態代碼段，其返回值是一個類。

而 ```newInstance()``` 就是根據類初始化一個實例，也就是說如下的兩種方式，其效果是一樣的：

{% highlight java %}
A a = (A)Class.forName("package.A").newInstance();
A a = new A();
{% endhighlight %}

```Class.forName().newInstance()``` 和 ```new()``` 區別是：A) 前者是一個方法，後者是一個關鍵字；B) 前者生成對象只能調用無參的構造函數，而後者沒有限制。

使用 ```Class.forName()``` 的目的是為了動態加載類，如需要實例化對象，還需要調用 ```newInstance()``` 。而實際上，在我們的示例中，並沒有調用 ```newInstance()```，Why？？？

如上所述，JVM 要先加載 class，而靜態代碼是和 class 綁定的，class 裝載成功就表示執行了你的靜態代碼了，而且以後的實例化等操作將不會再執行這段靜態代碼了。

實際上，JDBC 規範中明確要求這個 Driver 類必須向 DriverManager 註冊自己，例如，對於 MySQL 來說，在 ```src/com/mysql/jdbc/Driver.java``` 中，實現了 ```java.sql.Driver``` 接口，在初始化時有如下操作：

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

也就是說，對於 JDBC 是否使用了 ```newInstance()```，兩者是一樣的。

### DriverManager

在繼續講解之前，可以看到，對應 DriverManager 類有一個靜態匿名函數，也就是執行 ```loadInitialDrivers()``` 函數。

在 ```jdk/src/share/classes/java/sql/DriverManager.java``` 中，通過 ```registeredDrivers.addIfAbsent()``` 將新建的 DriverInfo 信息保存。

### 獲取鏈接

實際上獲取鏈接有多個接口，其入參是不同的，通常會使用如下的接口。在該函數中，如果正常，則會返回一個鏈接，否則會返回 NULL 。

{% highlight java %}
Drivermanager.getConnection(url, name, password);
{% endhighlight %}

而對於所有的接口，最終會調用如下的私有函數：

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

也就是最終會調用 JDBC-MySQL Driver 的 ```connect()``` 函數。該函數先通過 ```parseURL()``` 解析 URL 中的信息，並保存在 Properties 中，該對象用於保存一些配置信息。

{% highlight java %}
// ### 1  @src/com/mysql/jdbc/NonRegisteringDriver.java
public java.sql.Connection connect(String url, Properties info) throws SQLException {
    ... ...                       // 對傳入的URL進行一些判斷，如負載均衡等

    // parseURL函數聲明：public Properties parseURL(String url, Properties defaults)
    if ((props = parseURL(url, info)) == null) {
        return null;
    }
    ... ...
    try {                        // 解析完參數後返回Properties對象，然後會初始化鏈接
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

而 ```Util.handleNewInstance()``` 函數實際執行 ```Class.forName("com.mysql.jdbc.JDBC4Connection").newInstance()``` 。該函數會調用基類 ```class ConnectionImpl``` 的構造函數。

也就是說，對於 JDBC4 最終調用的仍然與老接口類似。

{% highlight java %}
public ConnectionImpl(...) throws SQLException {
    ... ...
    try {
        this.dbmd = getMetaData(false, false);    // 獲取數據庫信息
        initializeSafeStatementInterceptors();
        createNewIO(false);                       // 創建遠程IO鏈接
        unSafeStatementInterceptors();
    }
    ... ...
}
{% endhighlight %}

### Socket的連接創建

連接創建是通過 ```ConnectionImp::createNewIO()``` 方法執行。

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
    this.io = new MysqlIO(...);   // MysqlIO的創建，用來於服務器進行通信
    this.io.doHandshake(this.user, this.password, this.database);
    ... ...
}

public MysqlIO(...) {
    ... ...
    // 創建得到一個socket
    this.mysqlConnection = this.socketFactory.connect(this.host, this.port, props);
    ... ...
    // 創建input流
    if (this.connection.getUseReadAheadInput()) {
        ... ...
    }
    // 創建output流
    this.mysqlOutput = new BufferedOutputStream(this.mysqlConnection.getOutputStream(), 16384);
}
{% endhighlight %}

### 執行SQL

```createStatement()``` 有三類接口，最終都會調用如下接口。其中前兩個參數為：對返回的結果集進行指定相應的模式功能，可參照 ResultSet 的常量設置。

{% highlight java %}
public java.sql.Statement createStatement(...) throws SQLException {
    checkClosed();
    // getLoadBalanceSafeProxy() 為相應的連接
    StatementImpl stmt = new StatementImpl(getLoadBalanceSafeProxy(), this.database);
    stmt.setResultSetType(resultSetType);
    stmt.setResultSetConcurrency(resultSetConcurrency);

    return stmt;
}

public java.sql.ResultSet executeQuery(String sql) throws SQLException {
    ... ...
    if (locallyScopedConn.getCacheResultSetMetadata()) { // 是否應用緩存ResultSetMeta
    }

    // ConnectionImpl中執行sql語句
    this.results = locallyScopedConn.execSQL(this, sql, -1, null,
            this.resultSetType, this.resultSetConcurrency,
            doStreaming,
            this.currentCatalog, cachedFields);
    ... ...
}

public synchronized ResultSetInternalMethods execSQL(...) {
    ... ...
    // 進入MysqlIO中執行查詢操作
    return this.io.sqlQueryDirect(callingStatement, sql,
            encoding, null, maxRows, resultSetType,
            resultSetConcurrency, streamResults, catalog,
            cachedMetadata);
    ... ...
}

final ResultSetInternalMethods sqlQueryDirect(...) throws Exception {
    ... ...
    // 發送查詢命與sql查詢語句，並得到查詢結果(socket處理)
    Buffer resultPacket = sendCommand(MysqlDefs.QUERY, null, queryPacket, false, null, 0);
    ... ...
    // 封裝成ResultSet
    ResultSetInternalMethods rs = readAllResults(callingStatement, maxRows, resultSetType,
            resultSetConcurrency, streamResults, catalog, resultPacket,
            false, -1L, cachedMetadata);
    ... ...
}
{% endhighlight %}

很多接口還沒有深入瞭解，有時間繼續。

## 參考


<!--
http://www.mamicode.com/info-detail-965290.html         【JDBC發展史】從JDBC1.0到JDBC4.0
http://shmilyaw-hotmail-com.iteye.com/blog/1926513       ServiceLoader和DriverManager使用總結
http://wuquanyin1011.iteye.com/blog/1409455              MySQL JDBC 驅動源碼分析
-->


[jdbc-basic-cycle]:       /images/java/jdba_basic_cycle.png                        "JDBC的基本調用流程"
[jdbc-structure]:         /images/java/jdbc_structure.png                          "JDBC框架整體架構"
[jdbc-simple-example]:    /reference/java/GetConnection.java                       "使用JDBC的簡單示例"
