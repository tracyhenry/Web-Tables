from django.conf.urls import url
from display import views

urlpatterns = [
    #'',
    url(r'^showtable/$', views.show_table, name = 'showtable'),
    url(r'^crowdtable/$', views.crowd_table, name = 'crowdtable'),
    url(r'^showcell/$', views.show_cell, name = 'showcell'),
    url(r'^showrelation/$', views.show_relation, name = 'showrelation'),
    url(r'^relres/$', views.recv_rel_result, name = 'recvrelresult'),
    url(r'^showconcept/$', views.show_concept, name = 'showconcept'),
    url(r'^conres/$', views.recv_con_result, name = 'recvconresult'),
]

