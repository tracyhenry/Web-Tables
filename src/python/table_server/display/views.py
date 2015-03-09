from django.shortcuts import render

# Create your views here.
from django.views.decorators.clickjacking import xframe_options_exempt
from django.views.decorators.http import require_GET, require_POST
from django.http import HttpResponse


from models import *

@require_GET
def showtable(request):

    # Get table id
    table_id = int(request.GET.get('tableID'))

    # Get the set of table cells
    cells =  TableCells.objects.filter(table_id = table_id)
    if cells.count() == 0:
        return HttpResponse('No such table!')

    # Get the url of this table
    table_url = cells[0].table_url

    return HttpResponse(table_url)

