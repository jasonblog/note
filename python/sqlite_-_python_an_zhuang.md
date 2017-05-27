# SQLite - Python 安裝


<div class="primary">
				
					<div class="archive_info">
	
		<div id="content">
					</div>
				<h1>SQLite - <span class="color_h1">Python</span></h1>

<h2>安裝</h2>
<p>SQLite3 可使用 sqlite3 模塊與 Python 進行集成。sqlite3 模塊是由 Gerhard Haring 編寫的。它提供了一個與 PEP 249 描述的 DB-API 2.0 規範兼容的 SQL 接口。您不需要單獨安裝該模塊，因為 Python 2.5.x 以上版本默認自帶了該模塊。</p>
<p>為了使用 sqlite3 模塊，您首先必須創建一個表示數據庫的連接對象，然後您可以有選擇地創建光標對象，這將幫助您執行所有的 SQL 語句。</p>
<h2>Python sqlite3 模塊 API</h2>
<p>以下是重要的 sqlite3 模塊程序，可以滿足您在 Python 程序中使用 SQLite 數據庫的需求。如果您需要了解更多細節，請查看 Python sqlite3 模塊的官方文檔。</p>
<table class="reference notranslate">
<tbody><tr><th width="5%">序號</th><th>API &amp; 描述</th></tr>
<tr><td>1</td><td><b>sqlite3.connect(database [,timeout ,other optional arguments])</b><br>
<p>該 API 打開一個到 SQLite 數據庫文件 database 的鏈接。您可以使用 ":memory:" 來在 RAM 中打開一個到 database 的數據庫連接，而不是在磁盤上打開。如果數據庫成功打開，則返回一個連接對象。</p>
<p>當一個數據庫被多個連接訪問，且其中一個修改了數據庫，此時 SQLite 數據庫被鎖定，直到事務提交。timeout 參數表示連接等待鎖定的持續時間，直到發生異常斷開連接。timeout 參數默認是 5.0（5 秒）。</p>
<p>如果給定的數據庫名稱 filename 不存在，則該調用將創建一個數據庫。如果您不想在當前目錄中創建數據庫，那麼您可以指定帶有路徑的文件名，這樣您就能在任意地方創建數據庫。</p>
</td></tr>
<tr><td>2</td><td><b>connection.cursor([cursorClass])</b><br>
<p>該例程創建一個 <b>cursor</b>，將在 Python 數據庫編程中用到。該方法接受一個單一的可選的參數 cursorClass。如果提供了該參數，則它必須是一個擴展自 sqlite3.Cursor 的自定義的 cursor 類。</p>
</td></tr>
<tr><td>3</td><td><b>cursor.execute(sql [, optional parameters])</b><br>
<p>該例程執行一個 SQL 語句。該 SQL 語句可以被參數化（即使用佔位符代替 SQL 文本）。sqlite3 模塊支持兩種類型的佔位符：問號和命名佔位符（命名樣式）。</p>
<p>例如：cursor.execute("insert into people values (?, ?)", (who, age))</p>
</td></tr>
<tr><td>4</td><td><b>connection.execute(sql [, optional parameters])</b><br>
<p>該例程是上面執行的由光標（cursor）對象提供的方法的快捷方式，它通過調用光標（cursor）方法創建了一箇中間的光標對象，然後通過給定的參數調用光標的 execute 方法。</p>
</td></tr>
<tr><td>5</td><td><b>cursor.executemany(sql, seq_of_parameters)</b><br>
<p>該例程對 seq_of_parameters 中的所有參數或映射執行一個 SQL 命令。</p>
</td></tr>
<tr><td>6</td><td><b>connection.executemany(sql[, parameters])</b><br>
<p>該例程是一個由調用光標（cursor）方法創建的中間的光標對象的快捷方式，然後通過給定的參數調用光標的 executemany 方法。</p>
</td></tr>
<tr><td>7</td><td><b>cursor.executescript(sql_script)</b><br>
<p>該例程一旦接收到腳本，會執行多個 SQL 語句。它首先執行 COMMIT 語句，然後執行作為參數傳入的 SQL 腳本。所有的 SQL 語句應該用分號（;）分隔。</p>
</td></tr>
<tr><td>8</td><td><b>connection.executescript(sql_script)</b><br>
<p>該例程是一個由調用光標（cursor）方法創建的中間的光標對象的快捷方式，然後通過給定的參數調用光標的 executescript 方法。</p>
</td></tr>
<tr><td>9</td><td><b>connection.total_changes()</b><br>
<p>該例程返回自數據庫連接打開以來被修改、插入或刪除的數據庫總行數。</p>
</td></tr>
<tr><td>10</td><td><b>connection.commit()</b><br>
<p>該方法提交當前的事務。如果您未調用該方法，那麼自您上一次調用 commit() 以來所做的任何動作對其他數據庫連接來說是不可見的。</p>
</td></tr>
<tr><td>11</td><td><b>connection.rollback()</b><br>
<p>該方法回滾自上一次調用 commit() 以來對數據庫所做的更改。</p>
</td></tr>
<tr><td>12</td><td><b>connection.close()</b><br>
<p>該方法關閉數據庫連接。請注意，這不會自動調用 commit()。如果您之前未調用 commit() 方法，就直接關閉數據庫連接，您所做的所有更改將全部丟失！</p>
</td></tr>
<tr><td>13</td><td><b>cursor.fetchone()</b><br>
<p>該方法獲取查詢結果集中的下一行，返回一個單一的序列，當沒有更多可用的數據時，則返回 None。</p>
</td></tr>
<tr><td>14</td><td><b>cursor.fetchmany([size=cursor.arraysize])</b><br>
<p>該方法獲取查詢結果集中的下一行組，返回一個列表。當沒有更多的可用的行時，則返回一個空的列表。該方法嘗試獲取由 size 參數指定的儘可能多的行。</p>
</td></tr><tr><td>15</td><td><b>cursor.fetchall()</b><br>
<p>該例程獲取查詢結果集中所有（剩餘）的行，返回一個列表。當沒有可用的行時，則返回一個空的列表。</p>
</td></tr></tbody></table>
<h2>連接數據庫</h2>
<p>下面的 Python 代碼顯示瞭如何連接到一個現有的數據庫。如果數據庫不存在，那麼它就會被創建，最後將返回一個數據庫對象。</p>
<pre class="prettyprint prettyprinted"><span class="com">#!/usr/bin/python</span><span class="pln">

</span><span class="kwd">import</span><span class="pln"> sqlite3

conn </span><span class="pun">=</span><span class="pln"> sqlite3</span><span class="pun">.</span><span class="pln">connect</span><span class="pun">(</span><span class="str">'test.db'</span><span class="pun">)</span><span class="pln">

</span><span class="kwd">print</span><span class="pln"> </span><span class="str">"Opened database successfully"</span><span class="pun">;</span></pre>
<p>在這裡，您也可以把數據庫名稱複製為特定的名稱 <b>:memory:</b>，這樣就會在 RAM 中創建一個數據庫。現在，讓我們來運行上面的程序，在當前目錄中創建我們的數據庫 <b>test.db</b>。您可以根據需要改變路徑。保存上面代碼到 sqlite.py 文件中，並按如下所示執行。如果數據庫成功創建，那麼會顯示下面所示的消息：</p>
<pre class="result prettyprint prettyprinted"><span class="pln">$chmod </span><span class="pun">+</span><span class="pln">x sqlite</span><span class="pun">.</span><span class="pln">py
$</span><span class="pun">./</span><span class="pln">sqlite</span><span class="pun">.</span><span class="pln">py
</span><span class="typ">Open</span><span class="pln"> database successfully</span></pre>
<h2>創建表</h2>
<p>下面的 Python 代碼段將用於在先前創建的數據庫中創建一個表：</p>
<pre class="prettyprint prettyprinted"><span class="com">#!/usr/bin/python</span><span class="pln">

</span><span class="kwd">import</span><span class="pln"> sqlite3

conn </span><span class="pun">=</span><span class="pln"> sqlite3</span><span class="pun">.</span><span class="pln">connect</span><span class="pun">(</span><span class="str">'test.db'</span><span class="pun">)</span><span class="pln">
</span><span class="kwd">print</span><span class="pln"> </span><span class="str">"Opened database successfully"</span><span class="pun">;</span><span class="pln">

conn</span><span class="pun">.</span><span class="pln">execute</span><span class="pun">(</span><span class="str">'''CREATE TABLE COMPANY
       (ID INT PRIMARY KEY     NOT NULL,
       NAME           TEXT    NOT NULL,
       AGE            INT     NOT NULL,
       ADDRESS        CHAR(50),
       SALARY         REAL);'''</span><span class="pun">)</span><span class="pln">
</span><span class="kwd">print</span><span class="pln"> </span><span class="str">"Table created successfully"</span><span class="pun">;</span><span class="pln">

conn</span><span class="pun">.</span><span class="pln">close</span><span class="pun">()</span></pre>
<p>上述程序執行時，它會在 <b>test.db</b> 中創建 COMPANY 表，並顯示下面所示的消息：</p>
<pre class="result prettyprint prettyprinted"><span class="typ">Opened</span><span class="pln"> database successfully
</span><span class="typ">Table</span><span class="pln"> created successfully</span></pre>
<h2>INSERT 操作</h2>
<p>下面的 Python 程序顯示瞭如何在上面創建的 COMPANY 表中創建記錄：</p>
<pre class="prettyprint prettyprinted"><span class="com">#!/usr/bin/python</span><span class="pln">

</span><span class="kwd">import</span><span class="pln"> sqlite3

conn </span><span class="pun">=</span><span class="pln"> sqlite3</span><span class="pun">.</span><span class="pln">connect</span><span class="pun">(</span><span class="str">'test.db'</span><span class="pun">)</span><span class="pln">
</span><span class="kwd">print</span><span class="pln"> </span><span class="str">"Opened database successfully"</span><span class="pun">;</span><span class="pln">

conn</span><span class="pun">.</span><span class="pln">execute</span><span class="pun">(</span><span class="str">"INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) \
      VALUES (1, 'Paul', 32, 'California', 20000.00 )"</span><span class="pun">);</span><span class="pln">

conn</span><span class="pun">.</span><span class="pln">execute</span><span class="pun">(</span><span class="str">"INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) \
      VALUES (2, 'Allen', 25, 'Texas', 15000.00 )"</span><span class="pun">);</span><span class="pln">

conn</span><span class="pun">.</span><span class="pln">execute</span><span class="pun">(</span><span class="str">"INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) \
      VALUES (3, 'Teddy', 23, 'Norway', 20000.00 )"</span><span class="pun">);</span><span class="pln">

conn</span><span class="pun">.</span><span class="pln">execute</span><span class="pun">(</span><span class="str">"INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) \
      VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 )"</span><span class="pun">);</span><span class="pln">

conn</span><span class="pun">.</span><span class="pln">commit</span><span class="pun">()</span><span class="pln">
</span><span class="kwd">print</span><span class="pln"> </span><span class="str">"Records created successfully"</span><span class="pun">;</span><span class="pln">
conn</span><span class="pun">.</span><span class="pln">close</span><span class="pun">()</span></pre>
<p>上述程序執行時，它會在 COMPANY 表中創建給定記錄，並會顯示以下兩行：</p>
<pre class="result prettyprint prettyprinted"><span class="typ">Opened</span><span class="pln"> database successfully
</span><span class="typ">Records</span><span class="pln"> created successfully</span></pre>
<h2>SELECT 操作</h2>
<p>下面的 Python 程序顯示瞭如何從前面創建的 COMPANY 表中獲取並顯示記錄：</p>
<pre class="prettyprint prettyprinted"><span class="com">#!/usr/bin/python</span><span class="pln">

</span><span class="kwd">import</span><span class="pln"> sqlite3

conn </span><span class="pun">=</span><span class="pln"> sqlite3</span><span class="pun">.</span><span class="pln">connect</span><span class="pun">(</span><span class="str">'test.db'</span><span class="pun">)</span><span class="pln">
</span><span class="kwd">print</span><span class="pln"> </span><span class="str">"Opened database successfully"</span><span class="pun">;</span><span class="pln">

cursor </span><span class="pun">=</span><span class="pln"> conn</span><span class="pun">.</span><span class="pln">execute</span><span class="pun">(</span><span class="str">"SELECT id, name, address, salary  from COMPANY"</span><span class="pun">)</span><span class="pln">
</span><span class="kwd">for</span><span class="pln"> row </span><span class="kwd">in</span><span class="pln"> cursor</span><span class="pun">:</span><span class="pln">
   </span><span class="kwd">print</span><span class="pln"> </span><span class="str">"ID = "</span><span class="pun">,</span><span class="pln"> row</span><span class="pun">[</span><span class="lit">0</span><span class="pun">]</span><span class="pln">
   </span><span class="kwd">print</span><span class="pln"> </span><span class="str">"NAME = "</span><span class="pun">,</span><span class="pln"> row</span><span class="pun">[</span><span class="lit">1</span><span class="pun">]</span><span class="pln">
   </span><span class="kwd">print</span><span class="pln"> </span><span class="str">"ADDRESS = "</span><span class="pun">,</span><span class="pln"> row</span><span class="pun">[</span><span class="lit">2</span><span class="pun">]</span><span class="pln">
   </span><span class="kwd">print</span><span class="pln"> </span><span class="str">"SALARY = "</span><span class="pun">,</span><span class="pln"> row</span><span class="pun">[</span><span class="lit">3</span><span class="pun">],</span><span class="pln"> </span><span class="str">"\n"</span><span class="pln">

</span><span class="kwd">print</span><span class="pln"> </span><span class="str">"Operation done successfully"</span><span class="pun">;</span><span class="pln">
conn</span><span class="pun">.</span><span class="pln">close</span><span class="pun">()</span></pre>
<p>上述程序執行時，它會產生以下結果：</p>
<pre class="result prettyprint prettyprinted"><span class="typ">Opened</span><span class="pln"> database successfully
ID </span><span class="pun">=</span><span class="pln">  </span><span class="lit">1</span><span class="pln">
NAME </span><span class="pun">=</span><span class="pln">  </span><span class="typ">Paul</span><span class="pln">
ADDRESS </span><span class="pun">=</span><span class="pln">  </span><span class="typ">California</span><span class="pln">
SALARY </span><span class="pun">=</span><span class="pln">  </span><span class="lit">20000.0</span><span class="pln">

ID </span><span class="pun">=</span><span class="pln">  </span><span class="lit">2</span><span class="pln">
NAME </span><span class="pun">=</span><span class="pln">  </span><span class="typ">Allen</span><span class="pln">
ADDRESS </span><span class="pun">=</span><span class="pln">  </span><span class="typ">Texas</span><span class="pln">
SALARY </span><span class="pun">=</span><span class="pln">  </span><span class="lit">15000.0</span><span class="pln">

ID </span><span class="pun">=</span><span class="pln">  </span><span class="lit">3</span><span class="pln">
NAME </span><span class="pun">=</span><span class="pln">  </span><span class="typ">Teddy</span><span class="pln">
ADDRESS </span><span class="pun">=</span><span class="pln">  </span><span class="typ">Norway</span><span class="pln">
SALARY </span><span class="pun">=</span><span class="pln">  </span><span class="lit">20000.0</span><span class="pln">

ID </span><span class="pun">=</span><span class="pln">  </span><span class="lit">4</span><span class="pln">
NAME </span><span class="pun">=</span><span class="pln">  </span><span class="typ">Mark</span><span class="pln">
ADDRESS </span><span class="pun">=</span><span class="pln">  </span><span class="typ">Rich</span><span class="pun">-</span><span class="typ">Mond</span><span class="pln">
SALARY </span><span class="pun">=</span><span class="pln">  </span><span class="lit">65000.0</span><span class="pln">

</span><span class="typ">Operation</span><span class="pln"> </span><span class="kwd">done</span><span class="pln"> successfully</span></pre>
<h2>UPDATE 操作</h2>
<p>下面的 Python 代碼顯示瞭如何使用 UPDATE 語句來更新任何記錄，然後從 COMPANY 表中獲取並顯示更新的記錄：</p>
<pre class="prettyprint prettyprinted"><span class="com">#!/usr/bin/python</span><span class="pln">

</span><span class="kwd">import</span><span class="pln"> sqlite3

conn </span><span class="pun">=</span><span class="pln"> sqlite3</span><span class="pun">.</span><span class="pln">connect</span><span class="pun">(</span><span class="str">'test.db'</span><span class="pun">)</span><span class="pln">
</span><span class="kwd">print</span><span class="pln"> </span><span class="str">"Opened database successfully"</span><span class="pun">;</span><span class="pln">

conn</span><span class="pun">.</span><span class="pln">execute</span><span class="pun">(</span><span class="str">"UPDATE COMPANY set SALARY = 25000.00 where ID=1"</span><span class="pun">)</span><span class="pln">
conn</span><span class="pun">.</span><span class="pln">commit</span><span class="pun">()</span><span class="pln">
</span><span class="kwd">print</span><span class="pln"> </span><span class="str">"Total number of rows updated :"</span><span class="pun">,</span><span class="pln"> conn</span><span class="pun">.</span><span class="pln">total_changes

cursor </span><span class="pun">=</span><span class="pln"> conn</span><span class="pun">.</span><span class="pln">execute</span><span class="pun">(</span><span class="str">"SELECT id, name, address, salary  from COMPANY"</span><span class="pun">)</span><span class="pln">
</span><span class="kwd">for</span><span class="pln"> row </span><span class="kwd">in</span><span class="pln"> cursor</span><span class="pun">:</span><span class="pln">
   </span><span class="kwd">print</span><span class="pln"> </span><span class="str">"ID = "</span><span class="pun">,</span><span class="pln"> row</span><span class="pun">[</span><span class="lit">0</span><span class="pun">]</span><span class="pln">
   </span><span class="kwd">print</span><span class="pln"> </span><span class="str">"NAME = "</span><span class="pun">,</span><span class="pln"> row</span><span class="pun">[</span><span class="lit">1</span><span class="pun">]</span><span class="pln">
   </span><span class="kwd">print</span><span class="pln"> </span><span class="str">"ADDRESS = "</span><span class="pun">,</span><span class="pln"> row</span><span class="pun">[</span><span class="lit">2</span><span class="pun">]</span><span class="pln">
   </span><span class="kwd">print</span><span class="pln"> </span><span class="str">"SALARY = "</span><span class="pun">,</span><span class="pln"> row</span><span class="pun">[</span><span class="lit">3</span><span class="pun">],</span><span class="pln"> </span><span class="str">"\n"</span><span class="pln">

</span><span class="kwd">print</span><span class="pln"> </span><span class="str">"Operation done successfully"</span><span class="pun">;</span><span class="pln">
conn</span><span class="pun">.</span><span class="pln">close</span><span class="pun">()</span></pre>
<p>上述程序執行時，它會產生以下結果：</p>
<pre class="result prettyprint prettyprinted"><span class="typ">Opened</span><span class="pln"> database successfully
</span><span class="typ">Total</span><span class="pln"> number of rows updated </span><span class="pun">:</span><span class="pln"> </span><span class="lit">1</span><span class="pln">
ID </span><span class="pun">=</span><span class="pln">  </span><span class="lit">1</span><span class="pln">
NAME </span><span class="pun">=</span><span class="pln">  </span><span class="typ">Paul</span><span class="pln">
ADDRESS </span><span class="pun">=</span><span class="pln">  </span><span class="typ">California</span><span class="pln">
SALARY </span><span class="pun">=</span><span class="pln">  </span><span class="lit">25000.0</span><span class="pln">

ID </span><span class="pun">=</span><span class="pln">  </span><span class="lit">2</span><span class="pln">
NAME </span><span class="pun">=</span><span class="pln">  </span><span class="typ">Allen</span><span class="pln">
ADDRESS </span><span class="pun">=</span><span class="pln">  </span><span class="typ">Texas</span><span class="pln">
SALARY </span><span class="pun">=</span><span class="pln">  </span><span class="lit">15000.0</span><span class="pln">

ID </span><span class="pun">=</span><span class="pln">  </span><span class="lit">3</span><span class="pln">
NAME </span><span class="pun">=</span><span class="pln">  </span><span class="typ">Teddy</span><span class="pln">
ADDRESS </span><span class="pun">=</span><span class="pln">  </span><span class="typ">Norway</span><span class="pln">
SALARY </span><span class="pun">=</span><span class="pln">  </span><span class="lit">20000.0</span><span class="pln">

ID </span><span class="pun">=</span><span class="pln">  </span><span class="lit">4</span><span class="pln">
NAME </span><span class="pun">=</span><span class="pln">  </span><span class="typ">Mark</span><span class="pln">
ADDRESS </span><span class="pun">=</span><span class="pln">  </span><span class="typ">Rich</span><span class="pun">-</span><span class="typ">Mond</span><span class="pln">
SALARY </span><span class="pun">=</span><span class="pln">  </span><span class="lit">65000.0</span><span class="pln">

</span><span class="typ">Operation</span><span class="pln"> </span><span class="kwd">done</span><span class="pln"> successfully</span></pre>
<h2>DELETE 操作</h2>
<p>下面的 Python 代碼顯示瞭如何使用 DELETE 語句刪除任何記錄，然後從 COMPANY 表中獲取並顯示剩餘的記錄：</p>
<pre class="prettyprint prettyprinted"><span class="com">#!/usr/bin/python</span><span class="pln">

</span><span class="kwd">import</span><span class="pln"> sqlite3

conn </span><span class="pun">=</span><span class="pln"> sqlite3</span><span class="pun">.</span><span class="pln">connect</span><span class="pun">(</span><span class="str">'test.db'</span><span class="pun">)</span><span class="pln">
</span><span class="kwd">print</span><span class="pln"> </span><span class="str">"Opened database successfully"</span><span class="pun">;</span><span class="pln">

conn</span><span class="pun">.</span><span class="pln">execute</span><span class="pun">(</span><span class="str">"DELETE from COMPANY where ID=2;"</span><span class="pun">)</span><span class="pln">
conn</span><span class="pun">.</span><span class="pln">commit</span><span class="pun">()</span><span class="pln">
</span><span class="kwd">print</span><span class="pln"> </span><span class="str">"Total number of rows deleted :"</span><span class="pun">,</span><span class="pln"> conn</span><span class="pun">.</span><span class="pln">total_changes

cursor </span><span class="pun">=</span><span class="pln"> conn</span><span class="pun">.</span><span class="pln">execute</span><span class="pun">(</span><span class="str">"SELECT id, name, address, salary  from COMPANY"</span><span class="pun">)</span><span class="pln">
</span><span class="kwd">for</span><span class="pln"> row </span><span class="kwd">in</span><span class="pln"> cursor</span><span class="pun">:</span><span class="pln">
   </span><span class="kwd">print</span><span class="pln"> </span><span class="str">"ID = "</span><span class="pun">,</span><span class="pln"> row</span><span class="pun">[</span><span class="lit">0</span><span class="pun">]</span><span class="pln">
   </span><span class="kwd">print</span><span class="pln"> </span><span class="str">"NAME = "</span><span class="pun">,</span><span class="pln"> row</span><span class="pun">[</span><span class="lit">1</span><span class="pun">]</span><span class="pln">
   </span><span class="kwd">print</span><span class="pln"> </span><span class="str">"ADDRESS = "</span><span class="pun">,</span><span class="pln"> row</span><span class="pun">[</span><span class="lit">2</span><span class="pun">]</span><span class="pln">
   </span><span class="kwd">print</span><span class="pln"> </span><span class="str">"SALARY = "</span><span class="pun">,</span><span class="pln"> row</span><span class="pun">[</span><span class="lit">3</span><span class="pun">],</span><span class="pln"> </span><span class="str">"\n"</span><span class="pln">

</span><span class="kwd">print</span><span class="pln"> </span><span class="str">"Operation done successfully"</span><span class="pun">;</span><span class="pln">
conn</span><span class="pun">.</span><span class="pln">close</span><span class="pun">()</span></pre>
<p>上述程序執行時，它會產生以下結果：</p>
<pre class="result prettyprint prettyprinted"><span class="typ">Opened</span><span class="pln"> database successfully
</span><span class="typ">Total</span><span class="pln"> number of rows deleted </span><span class="pun">:</span><span class="pln"> </span><span class="lit">1</span><span class="pln">
ID </span><span class="pun">=</span><span class="pln">  </span><span class="lit">1</span><span class="pln">
NAME </span><span class="pun">=</span><span class="pln">  </span><span class="typ">Paul</span><span class="pln">
ADDRESS </span><span class="pun">=</span><span class="pln">  </span><span class="typ">California</span><span class="pln">
SALARY </span><span class="pun">=</span><span class="pln">  </span><span class="lit">20000.0</span><span class="pln">

ID </span><span class="pun">=</span><span class="pln">  </span><span class="lit">3</span><span class="pln">
NAME </span><span class="pun">=</span><span class="pln">  </span><span class="typ">Teddy</span><span class="pln">
ADDRESS </span><span class="pun">=</span><span class="pln">  </span><span class="typ">Norway</span><span class="pln">
SALARY </span><span class="pun">=</span><span class="pln">  </span><span class="lit">20000.0</span><span class="pln">

ID </span><span class="pun">=</span><span class="pln">  </span><span class="lit">4</span><span class="pln">
NAME </span><span class="pun">=</span><span class="pln">  </span><span class="typ">Mark</span><span class="pln">
ADDRESS </span><span class="pun">=</span><span class="pln">  </span><span class="typ">Rich</span><span class="pun">-</span><span class="typ">Mond</span><span class="pln">
SALARY </span><span class="pun">=</span><span class="pln">  </span><span class="lit">65000.0</span><span class="pln">

</span><span class="typ">Operation</span><span class="pln"> </span><span class="kwd">done</span><span class="pln"> successfully</span></pre>												<div class="scroll-top"><a href="javascript:scroll(0,0)">返回頂部</a></div>
				
			 </div>
	        	        	 		<div class="clear"></div>
		</div>