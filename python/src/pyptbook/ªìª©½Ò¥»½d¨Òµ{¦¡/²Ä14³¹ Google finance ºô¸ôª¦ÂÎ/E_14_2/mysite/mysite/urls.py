
from django.conf.urls import url
from django.contrib import admin
from myapp import views


urlpatterns = [
    url(r'^admin/', admin.site.urls),
    url(r'^E_14_2/$',views.E_14_2),
    url(r'^E_14_2_Py/$',views.E_14_2_Py),    

]
