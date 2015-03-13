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

    # Calculate N and M
    N = 0
    M = 0
    for cell in cells:
        N = max(N, cell.row)
        M = max(M, cell.col)
    N += 1
    M += 1

    # Lists
    row_list = []
    for i in range(0, N):
        row_list.append({"row" : i, "cell" : []})
    for cell in cells:
        row_list[cell.row]["cell"].append({"col" : cell.col, "value" : cell.value})


    return render(request, 'showtable.html', {'row_list' : row_list,
                                              'table_url' : table_url})

