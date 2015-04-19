from django.shortcuts import render

# Create your views here.
from django.views.decorators.clickjacking import xframe_options_exempt
from django.views.decorators.http import require_GET, require_POST
from django.views.decorators.csrf import csrf_exempt
from django.http import HttpResponse
from random import randint
from sets import Set

from models import *

@require_GET
def show_table(request):

    # Get table id
    table_id = int(request.GET.get('tableID'))

    # Random
    if table_id == -1:
        while True:
            table_id = randint(0, 6000);
            if TableCells.objects.filter(table_id = table_id).count() > 0:
                break

    # Get the set of table cells
    cells =  TableCells.objects.filter(table_id = table_id)
    if cells.count() == 0:
        return HttpResponse('No such table!')

    # Get the schema of this table
    schemas = TableSchema.objects.filter(table_id = table_id)

    # Ground Truth
    ground_truths = GoldAnnotation.objects.filter(table_id = table_id)

    # Entity Column
    entity_col = TableSchema.objects.filter(table_id = table_id).filter(is_entity = 1)
    if entity_col.count() == 0:
        entity_col = -1
    else:
        entity_col = entity_col[0].att_id

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

    # Row Lists
    row_list = []
    for i in range(0, N):
        row_list.append({"row" : i, "cell" : []})

    table_matches = FuzzyMatch.objects.filter(table_id = cells[0].table_id)
    have_match = [[False for j in range(M)] for i in range(N)]
    in_database = [[False for j in range(M)] for i in range(N)]
    for match in table_matches:
        have_match[match.row][match.col] = True

    for cell in cells:
        row_list[cell.row]["cell"].append({"col" : cell.col, "value" : cell.value, "matched" : have_match[cell.row][cell.col]})
        in_database[cell.row][cell.col] = True

    for i in range(0, N):
        for j in range(0, M):
            if in_database[i][j] == False:
                row_list[i]["cell"].append({"col" : j, "value" : "null", "matched" : False})

    # Schema Lists
    schema_list = []
    for col in schemas:
        schema_list.append({"col" : col.att_id, "value" : col.att_name})

    # Ground Truth Lists
    gt_list = []
    for col in ground_truths:
        gt_list.append({"col" : col.col, "value" : col.gold_type})    
    for i in range(0, M):
        appeared = False
        for col in ground_truths:
            if col.col == i:
                appeared = True
        if appeared == False:
            gt_list.append({"col" : i, "value" : "NO_GROUND_TRUTH"})

    return render(request, 'showtable.html', {'row_list' : row_list,
                                              'table_id' : str(table_id),
                                              'table_url' : table_url,
                                              'gt_list' : gt_list,
                                              'entity_col' : entity_col,
                                              'schema_list' : schema_list})

@require_POST
@csrf_exempt
def show_cell(request): 

    table_id = request.POST.get('table_id')
    row = request.POST.get('row')
    col = request.POST.get('col')

    matches = FuzzyMatch.objects.filter(table_id = table_id, row = row, col = col)
    cur_cell = TableCells.objects.filter(table_id = table_id, row = row, col = col)

    ans = "\n--------------\n Cell ID: " + str(cur_cell[0].id) + "\n--------------\n"

    # no matches
    if matches.count() == 0:
        return HttpResponse(ans + "No Matches!")

    # matches
    for match in matches:

        ans = ans + YagoEntity.objects.filter(id = match.entity_id)[0].value + ":\n"
        types = YagoType.objects.filter(entity_id = match.entity_id)
        for cur_type in types:
            ans = ans + YagoConcept.objects.filter(id = cur_type.concept_id)[0].value + "\n"

        ans = ans + "\n--------------------------------\n"

    return HttpResponse(ans)
