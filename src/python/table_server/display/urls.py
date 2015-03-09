from django.conf.urls import patterns, url
from display import views

urlpatterns = patterns('',
    url(r'^showtable/$', views.showtable, name = 'showtable'),
)
