
# -*- coding: utf-8 -*-
"""
Created on Thu Jun 22 09:11:04 2017
1.自Google finance抓取台股日資料(任意期間)
2.存入EXCEL檔案
@author: 蔡進金, 林萍珍
"""
import pandas as pd
import datetime
import os.path
 
class GetGoogleFinance():
	def __init__(self,stkno,startdate,enddate,refresh=True):
    	self.sdate=startdate
    	self.edate=enddate
        S_date=datetime.datetime.strftime(startdate,'%Y%m%d')
        E_date=datetime.datetime.strftime(enddate,'%Y%m%d')
        self.fn=str(stkno)+'_'+S_date+'_'+E_date+'.xlsx'
    	self.stkno=stkno
    	self.refresh=refresh
    	
	def savetoexcel(self,data,sheetname):
    	'''
    	若資料不存在，則不寫入檔案
    	'''
    	if   len(data) != 0:
        	writer=pd.ExcelWriter(self.fn)
            data.to_excel(writer,sheetname,index=False)
        	writer.save()
 
	def getstock(self,asc=False):
    	if not self.refresh and os.path.exists(self.fn):	#不重置 &檔案已存在
        	Data=pd.read_excel(self.fn)
            Data[['open','high','low','close','volume']]=\
            Data[['open','high','low','close','volume']].astype(float)
            Data[['dates']]=Data[['dates']].astype(object)
    	else:
        	s_yy=self.sdate.year
        	s_mm='{:%b}'.format(self.sdate)
        	s_dd=self.sdate.day
        	e_yy=self.edate.year
        	e_mm='{:%b}'.format(self.edate)
        	e_dd=self.edate.day
        	Data=pd.DataFrame()
        	for startrow in range(1,10200,200):
            	try:
                    #url='https://www.google.com/finance/historical?q=TPE:{}&startdate={}+{}%2C+{}&enddate={}+{}%2C+{}&num=200&ei=91FUWbDaMIuk0AS8mKzQDg&start={}'\
                	url='https://finance.google.com/finance/historical?q=TPE:{}&startdate={}+{}%2C+{}&enddate={}+{}%2C+{}&num=200&ei=1zPaWcjPMc3E0QTivbGwBw&start={}'\
                    .format(self.stkno,s_mm,s_dd,s_yy,e_mm,e_dd,e_yy,startrow)
                	#print(url)
                	data=pd.read_html(url)[2]
                    data=data.drop(data.columns[[0]],axis=0)
                    Data=Data.append(data,ignore_index=True)
            	except Exception as err:
                	print(err)
             	   break
        	if  len(Data)   != 0:    	#有取得資料才進行轉換
                Data.columns=['dates','open','high','low','close','volume']
            	Data=Data.replace('-','0')
                Data[['open','high','low','close','volume']]=\
        	    Data[['open','high','low','close','volume']].astype(float)
            	for i in range(len(Data)):
                        Data['dates'].loc[i]=datetime.datetime.strptime\
                        (Data['dates'].loc[i], "%b %d, %Y").date()
      	      if asc==True:
                    Data=Data.sort_index(by=['dates'],ascending=[True])
        	
    	return Data
	
if __name__ == "__main__":
	'''
	Get data from google finance
	'''   
	stkno='1102'
    start = datetime.datetime(2015,9,1)
	end = datetime.datetime(2017,10,1)
	#增加參數，True則重新抓取資料，
	#         False則檢查檔案存在與否，若不存在再上網抓取資料。
    Gdata=GetGoogleFinance(stkno,start,end,False)
 
	Data =Gdata.getstock(True)
	'''
	Save data to excel
	'''
	if  len(Data)  != 0:
    	Gdata.savetoexcel(Data,stkno) 
	else:
    	print('{} 抓取的資料不存在!'.format(stkno))
