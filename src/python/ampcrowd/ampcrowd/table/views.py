from django.shortcuts import render

# Create your views here.
from django.views.decorators.clickjacking import xframe_options_exempt
from django.views.decorators.http import require_GET
from django.http import HttpResponse

from models import *
from misc import *

@require_GET
def crowd_table(request):

    # Get table id
    table_id = int(request.GET.get('tableID'))

    context = get_table_context(table_id)
    if context == {}:
        return HttpResponse("No such table!")

    return render(request, 'crowdtable.html', context)
