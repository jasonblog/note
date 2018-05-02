
from django.conf.urls import url
from django.contrib import admin
from myapp import views


urlpatterns = [
    url(r'^admin/', admin.site.urls),
    url(r'^company/$',views.company),
    url(r'^company/insert/$',views.insert),
    url(r'^do_insert/$',views.do_insert),
    url(r'^company/detail/(?P<stockid>\d+)/$',views.detail),
    url(r'^company/update/(?P<stockid>\d+)/$',views.update),
    url(r'^do_update/$',views.do_update),
    url(r'^company/delete/(?P<stockid>\d+)/$',views.delete),
    url(r'^do_delete/$',views.do_delete),

]
