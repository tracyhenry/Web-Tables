from django.conf.urls import patterns, url
from display import views

urlpatterns = patterns('',
    url(r'^showtable/$', views.show_table, name = 'showtable'),
    url(r'^showcell/$', views.show_cell, name = 'showcell'),
    url(r'^showrelation/$', views.show_relation, name = 'showrelation'),
    url(r'^relres/$', views.recv_rel_result, name = 'recvrelresult'),
)
