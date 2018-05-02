from django.conf.urls import url
from django.contrib import admin
from myapp import views
urlpatterns = [
	url(r'^admin/', admin.site.urls),
	url(r'^go/$', views.pylinkweb),  #新增這一列
	url(r'^fv/$', views.deposits),   #新增這一列 
	url(r'^result/$', views.result),   #新增這一列     
]
