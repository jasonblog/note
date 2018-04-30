#E_7_12.py功能: 使用pandas套件讀寫excel檔案
import pandas as pd
from pandas import ExcelWriter
#運算敘述統計
df1=pd.read_excel('./file/oil_statistics.xlsx', sheetname='report1')
df2=df1.describe()
print(df2)
#開啟要寫入的檔案以及工作表
writer = ExcelWriter('./file/pandas_oil_statistics.xlsx')
df2.to_excel(writer,'Sheet2')
workbook = writer.book
worksheet = writer.sheets['Sheet2']
worksheet.conditional_format('A1:D9', {'type': '3_color_scale'})
#畫長條圖
chart = workbook.add_chart({'type': 'column'})
chart.add_series({'name': '=Sheet2!$B$1','categories': '=Sheet2!$A$2:$A$9',
'values': '=Sheet2!$B$2:$B$9'})
chart.add_series({'name': '=Sheet2!$C$1','categories': '=Sheet2!$A$2:$A$9',
'values': '=Sheet2!$C$2:$C$9'})
chart.add_series({'name': '=Sheet2!$D$1','categories': '=Sheet2!$A$2:$A$9',
'values': '=Sheet2!$D$2:$D$9'})
chart.set_x_axis({'name': 'Index'})
chart.set_y_axis({'name': 'Value', 'major_gridlines': {'visible': False}})
chart.set_legend({'position': 'top'})
worksheet.insert_chart('F2: W10', chart)
writer.save()
