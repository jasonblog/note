from bs4 import BeautifulSoup
import pandas as pd

# <table bgcolor="#d2d2d2" class="solid_1_padding_4_1_tbl" id="tblStockList" style="font-size:10pt;line-height:16px;" width="100%">
table = BeautifulSoup(open('./aa.html','r').read()).find_all("table", attrs={'class':'solid_1_padding_4_1_tbl','id':'tblStockList'})
print(table)
#df = pd.read_html(str(table))

#print(df)
