#E_7_10.py功能: 讀取excel (*.xls)檔案
import xlrd
import xlwt
#read excel file
li1=[]
data=xlrd.open_workbook('./file/index.xls')
table=data.sheets()[0]
print('表格內共有 %d 列' %(table.nrows))
print('表格內共有 %d 行' %(table.ncols))
print('原始資料內容 = ')
for i in range(0,table.nrows):
    print(table.cell(i,0).value)
    li1.append(table.cell(i,0).value)
#write excel file
wb = xlwt.Workbook()
ws = wb.add_sheet('ClosePrice')
print('修改後的資料內容 = ')
for i in range(0,table.nrows):
    print(li1[i]+10000)
    ws.write(i, 0, li1[i]+10000)
wb.save('./file/example.xls')
