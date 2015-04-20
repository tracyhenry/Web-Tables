from models import *

def get_table_context(table_id):

    # Get the set of table cells
    cells = TableCells.objects.filter(table_id = table_id)
    if cells.count() == 0:
        return {}

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
        row_list[cell.row]["cell"].append({"col" : cell.col,
                                           "value" : cell.value,
                                           "matched" : have_match[cell.row][cell.col]})
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

    ans = {'row_list' : row_list,
           'table_id' : str(table_id),
           'table_url' : table_url,
           'gt_list' : gt_list,
           'entity_col' : entity_col,
           'schema_list' : schema_list}
    return ans
