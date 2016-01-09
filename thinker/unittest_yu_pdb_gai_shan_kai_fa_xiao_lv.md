# unittest 與 pdb 改善開發效率


近年來，軟體工程界開始體認到現實的不完美，接受人腦的極限和不完美，轉而往發展簡單、實際的方法。如: XP 、 unit-test driven。其中，以 unit test 的技術最廣為採用，其原因來自其效果立即、明顯、低門檻， programmer 可以馬上感受到好處。 Python 也和其它許許多多的語言一樣，隨著潮流推出許多 unit test 的工具。其中最熟為人的，大概就是 unittest 和 doctest 這兩個隨 Python 散佈的工具。
不論使用哪一種工具， unit test 工具能在開發初期，迅速發掘潛在的 bug ，並不斷的反饋給 programmer ，使 programmer 能快速界定出 bug 的內容，並加以修正。這種反饋、修正的過程，程式碼的品質能獲得快速的改善，避免問題的擴散。在開發的中、後期， unit test 又能讓我們持續的對新加入或修改的程式進行品質的確認，維持程式碼的品質。

然而， unittest 雖然能測出錯誤的存在，但 programmer 並無法只依這些錯誤訊息就進行修正，programmer 還需要其它的資訊。這個資訊，通常就是程式目前現狀，還有現狀與正確狀況間的差異。這些現狀的資訊，需要透過檢視發生錯誤時的記憶體狀況。這時就需要祭出 pdb 來幫忙了。

例如:
```py
001	import sys
002	sys.stdout.write('give me a number (0 or 1): ')
003	sys.stdout.flush()
004	n=sys.stdin.readline()
005	a = ['hello', 'world']
006	print a[n]
```
很簡單的一個程式。但進行測試時，不論你輸入 0 或 1，都會看到
```c
jetjs$ python test.py
give me a number: 1
Traceback (most recent call last):
  File "test.py", line 6, in <module>
    print strings[n]
TypeError: list indices must be integers
```
這時， interpreter 告訴你在第 6 行出錯。這個程式很小，大概還可以直接從程式碼看出問題在哪，但程式變大時，或許你根本弄不清 n 是從哪來的，又為什麼出錯。 Interpreter 只告訴你必需用整數當 index ，但現在的 n 不是整數又是什麼？

這時或許心裡充滿了懷疑，最實際的做法就是檢查 n 的內容。使用 pdb
```py
jetjs$ python -m pdb test.py
> /usr/home/thinker/progm/jetjs/test.py(1)<module>()
-> import sys
(Pdb) b 6
Breakpoint 1 at /usr/home/thinker/progm/jetjs/test.py:6
(Pdb) r
give me a number: 1
> /usr/home/thinker/progm/jetjs/test.py(6)<module>()
-> print strings[n]
(Pdb) print repr(n)
'1\n'
(Pdb) 
```
第一行是呼叫 pdb 對 test.py 這個程式進行除錯的指令
```py
python -m pdb test.py
```
interpreter 告問我們第 6 行出錯，於是進入 pdb 之後，我們在第 6 行設定 break point
```py
b 6
```
，然後讓程式開始執行
```py
r
```
。在我們輸入數字，程式執行到第 6 行時，程式會被中斷，再度進入 pdb ，這時就可以檢視 n 的內容
```py
print repr(n)
```
，發覺 n 是一個字串。於是我們就瞭解到，本來應該是整數的 n 目前是一個字串。會發生這狀況的原因，是因為 readline() 傳回來的就是字串，而我們必需先將字串轉成整數才能正確的執行。
開發程式時，除了利用 unit test 幫助我們發現錯誤之外， programmer 最好還是熟悉 debugger 的操作，以快速的瞭解目前的狀況，才能進行修正。 unit test 再加上 debugger 的幫助，能讓 programmer 的工作效率更加提高，不用再對程式碼修修改改，就能瞭解程式的狀態。這樣的好工具，為什麼學校都不教授呢？或許學校的老師們大多不寫程式吧!! :p

(關於 pdb 請參考 http://docs.python.org/lib/module-pdb.html )