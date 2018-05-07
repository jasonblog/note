from django.shortcuts import render
from E_14_2 import main as E_14_2_main
from django.http import HttpResponse

def E_14_2(request):
	return render(request,'E_14_2.html',{})

def E_14_2_Py(request):
	stock_id = request.GET['stock_id']
	start_date = request.GET['start_date']
	end_date = request.GET['end_date']
	response = E_14_2_main(stock_id, start_date, end_date)
	return HttpResponse(response)
