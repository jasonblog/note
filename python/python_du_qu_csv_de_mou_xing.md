# Python 讀取csv的某行


假設CSV文件內容如下：

```sh
No.,Name,Age,Score
1,Apple,12,98
2,Ben,13,97
3,Celia,14,96
4,Dave,15,95
```

保存為A.csv，利用Python自帶的 csv模塊 ，有兩種方法可以實現提取其中的一行：

##方法一：reader

第一種方法使用reader函數，接收一個可迭代的對象（比如csv文件），能返回一個生成器，就可以從其中解析出csv的內容：比如下面的代碼可以讀取csv的全部內容，以行為單位：

```py
import csv
with open('A.csv','rb') as csvfile:
    reader = csv.reader(csvfile)
    rows = [row for row in reader]
print rows
```
得到：

```sh
[['No.', 'Name', 'Age', 'Score'],
['1', 'Apple', '12', '98'],
['2', 'Ben', '13', '97'],
['3', 'Celia', '14', '96'],
['4', 'Dave', '15', '95']]
```

要提取其中第二行，可以用下面的代碼：

```py
import csv
with open('A.csv','rb') as csvfile:
    reader = csv.reader(csvfile)
    for i,rows in enumerate(reader):
        if i == 2:
        row = rows
print row 
```

得到：

```sh
['2', 'Ben', '13', '97']
```

這種方法是通用的方法，要事先知道行號，比如Ben的記錄在第2行，而不能根據』Ben'這個名字查詢。這時可以採用第二種方法：
##方法二：DictReader

第二種方法是使用DictReader，和reader函數類似，接收一個可迭代的對象，能返回一個生成器，但是返回的每一個單元格都放在一個字典的值內，而這個字典的鍵則是這個單元格的標題（即列頭）。用下面的代碼可以看到DictReader的結構：

```py
import csv
with open('A.csv','rb') as csvfile:
    reader = csv.DictReader(csvfile)
    rows = [row for row in reader]
print rows
```

得到：

```sh
[{'Age': '12', 'No.': '1', 'Score': '98', 'Name': 'Apple'},
{'Age': '13', 'No.': '2', 'Score': '97', 'Name': 'Ben'},
{'Age': '14', 'No.': '3', 'Score': '96', 'Name': 'Celia'},
{'Age': '15', 'No.': '4', 'Score': '95', 'Name': 'Dave'}]
```

如果我們想用DictReader讀取csv的某一列，就可以用列的標題查詢：

```py
import csv
with open('A.csv','rb') as csvfile:
    reader = csv.DictReader(csvfile)
    for row in reader:
        if row['Name']=='Ben':
        print row
```

就得到：

```sh
{'Age': '13', 'No.': '2', 'Score': '97', 'Name': 'Ben'}
```

可見，DictReader很適合讀取csv的的行（記錄）。