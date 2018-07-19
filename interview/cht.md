# CHT


## 職缺技能

- Java、Restful API,Node.js、Python、JavaScript、 CSS、Bootstrap、HTML AWD/RWD

```sh
1. Java 、 JSP/Servlet 、 JPA 、 MVC 架構, Struts/Spring 框架,Restful API 設計開發經驗 1 年以上


2. 具備資料庫 : MySQL、MongoDB、MSSQL 設計、 面
交控系統後
管理經驗
端開發
議
3. 熟悉 Linux/Unix 環境
4. 熟悉 Hadoop、HDFS、Map/Reduce、Hive、Hbase、Spark、ETL 等相關開發經驗尤佳
```

以 Python 為程式語言開發 Apache Spark 應用程式存取 Hadoop HDFS 上的分散式大數據,
此架構具有多重優點: Python 語言簡明易學具高度生產力; `基於記憶體 (in-memory) 運算的 Apache Spark 叢集運算框架可進行高效能之大數據運算`;


而 Hadoop HDFS 則為大數據提供`彈性, 可靠, 與易擴容之分散式檔案系統`

Spark 2.0 主要功能有四 :
```
(1) Spark SQL DataFrame : 可使用 SQL 語法執行數據分析
(2) Spark Streaming : 可處理即時資料串流
(3) Graphx : 分散式圖形處理架構, 可用圖表運算
(4) Spark MLlib : 可擴充之機器學習函式庫
(5) Spark ML pipeline : 為 ML 每一階段建立 pipeline 流程, 減少程式設計負擔
```

其中 Spark ML pipeline 是 Spark 從 Python Scikit-learn 模組得到啟發而設計之架構. 
Spark 資料處理方式有`三種` :
```
(1) RDD
(2) DataFrame
(3) Spark SQL
```


Python Spark 機器學習 API 有兩個 : MLlib 與 ML Pipeline, 前者是搭配 RDD, 後者則是搭配 DataFrame :

```
(1) Spark MLlib : RDD-based 機器學習 API
(2) Spark ML Pipeline : DataFrame-based 機器學習 API
Spark DataFrame 與 Pandas DataFrame 可以互相轉換, Python 開發者可將資料轉為 Spark DataFrame 後使用 Spark ML Pipeline 進行訓練與預測, 然後轉回 Pandas DataFrame, 這樣就可以利用 Python 豐富的視覺化工具如 Matplotlib 等呈現結果. 
Spark DataFrame 提供 API 可輕易讀取其他資料來源, 例如 Hadoop, JSON 等, 也可以透過 JDBC 讀取關聯式資料庫如 MySQL 等. 
````

###大數據的 3V :
```
(1) Volumn : 大量資料
(2) Variety : 多樣化資料
(3) Velocity : 資料流速快
```


Hadoop 是 Apache 的大量資料儲存與處理平臺, 源自於 Doug Cutting 與 Mike Cafarelia 於 2002 年起始的 Nutch 專案, 2004 年導入 Google 的 MapReduce 分散是技術, 於 2006 年改名為 Hadoop 專案. Hadoop 具有如下特性 :

```
(1) 可擴展性 : 分散式儲存與運算, 只要增加新的資料節點伺服器即可擴充
(2) 經濟性 : 一般等級伺服器即可架構高效能叢集
(3) 彈性 : 可儲存各種形式與來源之資料 (無 Schema)
(4) 可靠性 : 每個資料都有兩個副本 (分散式架構)
```
### HDFS


`HDFS(Hadoop Distributed File System)` 分散式檔案系統由 NameNode 與多個 DataNode 組成, `ode 負責管理維護 HDFS 目錄系統, 控制檔案讀寫動作, DataNode 則負責儲存資料. DataNode 可擴充至成千上萬個. 檔案在儲存時會被切割成多個區塊, 每個區塊會有兩個副本 (總共), 也可以在組態設定中指定要建立幾個副本, 當某個區塊毀損時, NameNode 會自動搜尋其他 DataNode 上的副本來回復資料. HDFS 具備機架感知功能, 一份資料的三個副本會分別存放於同機架的不同節點, 以及不同機架的不同節點以避免資料遺失, 即使機架或節點故障仍可保證回復資料. 

### MapReduce
MapReduce 中的 Map 是將工作分割成許多小工作交給不同伺服器執行; 而 Reduce 則是指將所有伺服器的執行結果彙整後傳會最終結果. 利用 MapReduce 可在上千臺伺服器上平行處理巨量資料. Hadoop 的新版 MapReduce 架構稱為 YARN (Yet Another Resource Negotiator), 是效率更高的運算資源管理核心. 


```sh
1.手機怎麼跟後端資料傳送 
格式  協定 

2. MVC

3. APP ~ Linux/Windows 檢測

4 CVP

5. 應用優勢

車聯網
```