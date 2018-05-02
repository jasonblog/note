from django.shortcuts import render
from django.http import HttpResponse
from E_14_2 import main as E_14_2_main
from E_15_2 import main as E_15_2_main
from django.db import connection
import pandas as pd


def E_15_2(request):
	tables = connection.introspection.table_names()
	stock_names = []
	for table in tables:
		if(len(table) == 10 and table[:6] == 'stock_' and table[-4:].isdigit()):
			stock_names.append(table[-4:])
	return render(request,'E_15_2.html',locals())

def E_15_2_Py(request):
	stock_id = request.GET['stock_id']
	if(stock_id == "all"):
		tables = connection.introspection.table_names()
		dfSMA = pd.DataFrame()
		dfBBand = pd.DataFrame()
		#set DataFrame
		for index, table in enumerate(tables):
			if(len(table) == 10 and table[:6] == 'stock_' and table[-4:].isdigit()):
				stkno = table[-4:]
				df2= pd.DataFrame([[stkno,'','','','']],columns=['id','count','roi','winrate','winfactor'])
				dfSMA = dfSMA.append(df2, ignore_index=True)
				dfBBand = dfBBand.append(df2, ignore_index=True)
		#將每一股股票進行回測
		for index in range(len(dfBBand.index)):
			print(dfBBand.get_value(index, 'id'))
			result = E_15_2_main(dfBBand.get_value(index, 'id'), dfSMA, dfBBand, index)
		response = result
	else:
		dfSMA = pd.DataFrame(columns=['id','count','roi','winrate','winfactor'],index=[0])
		dfBBand = pd.DataFrame(columns=['id','count','roi','winrate','winfactor'],index=[0])
		dfSMA['id'] = stock_id
		dfBBand['id'] = stock_id
		response = E_15_2_main(stock_id, dfSMA, dfBBand, 0)

	return HttpResponse(response)


def E_14_2_Py(request):
	stock_id = request.GET['stock_id']
	start_date = request.GET['start_date']
	end_date = request.GET['end_date']
	response = E_14_2_main(stock_id, start_date, end_date)
	return HttpResponse(response)