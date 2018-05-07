from django.shortcuts import render
from django.http import HttpResponse

# Create your views here.
import pandas as pd
from E_14_2 import main as E_14_2_main
from E_16_1 import main as E_16_1_main
from django.db import connection

def E_16_1_Py(request):
	stock_id = request.GET['stock_id']
	trend_type = request.GET['trend_type']
	if(stock_id == "all"):
		tables = connection.introspection.table_names()
		result = pd.DataFrame()
		for table in tables:
			if(len(table) == 10 and table[:6] == 'stock_' and table[-4:].isdigit()):
				stkno = table[-4:]
				result = result.append(E_16_1_main(stkno, 'all', trend_type), ignore_index=True)
		response = result
	else:
		response = E_16_1_main(stock_id, 'one', trend_type)

	return HttpResponse(response.to_html(classes='', col_space=100))

def E_16_1(request):
	tables = connection.introspection.table_names()
	stock_names = []
	for table in tables:
		if(len(table) == 10 and table[:6] == 'stock_' and table[-4:].isdigit()):
			stock_names.append(table[-4:])
	return render(request,'E_16_1.html',locals())

def E_14_2_Py(request):
	stock_id = request.GET['stock_id']
	start_date = request.GET['start_date']
	end_date = request.GET['end_date']
	response = E_14_2_main(stock_id, start_date, end_date)
	return HttpResponse(response)