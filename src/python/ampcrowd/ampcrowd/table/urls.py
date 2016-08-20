from django.conf.urls import url
from table import views

urlpatterns = [
    url(r'^crowdtable/$', views.crowd_table, name = 'crowdtable'),
]
