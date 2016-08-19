from django.shortcuts import render

# Create your views here.
from django.views.decorators.clickjacking import xframe_options_exempt
from django.views.decorators.http import require_GET, require_POST
from django.views.decorators.csrf import csrf_exempt
from django.http import HttpResponse
from random import randint
from sets import Set

from models import *
from misc import *

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

    context = get_table_context(table_id)
    if context == {}:
        return HttpResponse("No such table!")

    return render(request, 'showtable.html', context)

@require_GET
def crowd_table(request):

    # Get table id
    table_id = int(request.GET.get('tableID'))

    context = get_table_context(table_id)
    if context == {}:
        return HttpResponse("No such table!")

    return render(request, 'crowdtable.html', context)

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

@require_GET
def show_relation(request):

    result_id = request.GET.get('id')
    res = ColRelation.objects.filter(id = result_id)

    if res.count() == 0:
        return HttpResponse("No such result!")

    # table context
    res = res[0]
    context = get_table_context(res.table_id)

    # add result-related things into contxet
    context['result_id'] = result_id
    context['result_col'] = res.col_id
    context['result_relation'] = res.relation
    context['result_verdict'] = res.verdict

    return render(request, 'showrelation.html', context)

@require_POST
@csrf_exempt
def recv_rel_result(request):

    result_id = request.POST.get('result_id')
    verdict = request.POST.get('verdict')

    res = ColRelation.objects.filter(id = result_id)[0]
    res.verdict = verdict
    res.save()

    return HttpResponse("Result received!")

@require_GET
def show_concept(request):

    result_id= request.GET.get('id')
    result_set = RecConcept.objects.filter(res_id = result_id)

    if result_set.count() == 0:
        return HttpResponse("No such result!")

    # table context
    context = get_table_context(result_set[0].table_id)

    # add result_related things into context
    context['result_id'] = result_id
    context['row_id'] = result_set[0].row_id
    context['result_set'] = []

    for res in result_set:
        context['result_set'].append({"rank" : res.rank, "concept" : res.concept})

    return render(request, 'showconcept.html', context)

@require_POST
@csrf_exempt
def recv_con_result(request):

    result_id = request.POST.get('result_id')
    rank = request.POST.get('rank')
    verdict = request.POST.get('verdict')

    res = RecConcept.objects.filter(res_id = result_id, rank = rank)[0]
    res.verdict = verdict
    res.save()

    return HttpResponse("Result Received!")
