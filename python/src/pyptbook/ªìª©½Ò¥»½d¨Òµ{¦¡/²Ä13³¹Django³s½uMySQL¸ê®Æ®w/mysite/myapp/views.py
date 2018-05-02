
from myapp.models import Company
from django.shortcuts import render_to_response
from django.shortcuts import render
from django.shortcuts import redirect

def company(request):
    company = Company.objects.all()
    stockid = Company._meta.get_field('stockid').column
    abbreviation = Company._meta.get_field('abbreviation').column
    url = Company._meta.get_field('url').column
    industryname = Company._meta.get_field('industryname').column
    return render_to_response('company.html',locals())

def insert(request):
	return render(request,'insert.html')

def do_insert(request):
	stockid = request.POST['stockid']
	abbreviation = request.POST['abbreviation']
	url = request.POST['url']
	employees = request.POST['employees']
	capital = request.POST['capital']
	industryname = request.POST['industryname']
	listeddate = request.POST['listeddate']
	Company.objects.create(stockid=stockid, abbreviation=abbreviation, url=url, employees=employees, capital=capital, industryname=industryname, listeddate=listeddate)
	return redirect('/company/')

def detail(request, stockid):
	detail = Company.objects.get(stockid=stockid)
	return render(request,'detail.html', {'detail': detail})



def update(request, stockid):
	update = Company.objects.get(stockid=stockid)
	return render(request,'update.html', {'update': update})


def do_update(request):
	stockid = request.POST['stockid']
	abbreviation = request.POST['abbreviation']
	url = request.POST['url']
	employees = request.POST['employees']
	capital = request.POST['capital']
	industryname = request.POST['industryname']
	listeddate = request.POST['listeddate']
	do_update = Company.objects.filter(stockid=stockid)
	do_update.update(abbreviation=abbreviation)
	do_update.update(url=url)
	do_update.update(employees=employees)
	do_update.update(capital=capital)
	do_update.update(industryname=industryname)
	do_update.update(listeddate=listeddate)
	return redirect('/company/')


def delete(request, stockid):
	delete = Company.objects.get(stockid=stockid)
	return render(request,'delete.html', {'delete': delete})


def do_delete(request):
	stockid = request.POST['stockid']	
	do_delete = Company.objects.filter(stockid=stockid)
	do_delete.delete()
	return redirect('/company/')
