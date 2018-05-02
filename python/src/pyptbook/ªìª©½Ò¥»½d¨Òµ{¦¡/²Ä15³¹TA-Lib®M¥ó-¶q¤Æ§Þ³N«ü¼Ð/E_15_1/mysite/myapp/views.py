from django.shortcuts import render
from django.http import HttpResponse
from E_14_2 import main as E_14_2_main
from E_15_1 import main as E_15_1_main
from django.db import connection

def E_15_1(request):
	tables = connection.introspection.table_names()
	stock_names = []
	for table in tables:
		if(len(table) == 10 and table[:6] == 'stock_' and table[-4:].isdigit()):
			stock_names.append(table[-4:])
	return render(request,'E_15_1.html',locals())


def E_15_1_Py(request):
	stock_id = request.GET['stock_id']
	response = E_15_1_main(stock_id)
	return HttpResponse(response)

def E_14_2_Py(request):
	stock_id = request.GET['stock_id']
	start_date = request.GET['start_date']
	end_date = request.GET['end_date']
	response = E_14_2_main(stock_id, start_date, end_date)
	return HttpResponse(response)