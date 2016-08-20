from models import *

def get_table_context(table_id):

    # Get the set of table cells
    cells = TableCells.objects.filter(table_id = table_id)
    if cells.count() == 0:
        return {}

    # Get the schema of this table
    schemas = TableSchema.objects.filter(table_id = table_id)

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

    in_database = [[False for j in range(M)] for i in range(N)]

    for cell in cells:
        row_list[cell.row]["cell"].append({"col" : cell.col,
                                           "value" : cell.value,})
        in_database[cell.row][cell.col] = True

    for i in range(0, N):
        for j in range(0, M):
            if in_database[i][j] == False:
                row_list[i]["cell"].append({"col" : j, "value" : "null"})

    # Schema Lists
    schema_list = []
    for col in schemas:
        schema_list.append({"col" : col.att_id, "value" : col.att_name})

    ans = {'row_list' : row_list,
           'table_url' : table_url,
           'schema_list' : schema_list}
    return ans
