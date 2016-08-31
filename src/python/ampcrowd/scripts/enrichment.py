#!/usr/bin/env python
import httplib
import socket
import re
import ssl
import sys
import copy
import json
import urllib
import urllib2
import operator
import logging

from sets import Set
from argparse import ArgumentParser

logging.getLogger().setLevel(logging.INFO)

def send_request(data):
    # Send request
    params = {'data' : json.dumps(data)}
    url = 'https://lmxnb.top:8002/crowds/amt/tasks/'
    try:
        response = urllib2.urlopen(url,
                               urllib.urlencode(params))
    except urllib2.HTTPError as exc:
        logging.error("HTTPError occurred while reaching crowd_server")
        logging.error("Code: %i, Reason: %s", exc.code, exc.reason)
        logging.error("Server Response Below")
        logging.error(exc.read())
        sys.exit(1)
    res = json.loads(response.read())
    if res['status'] != 'ok' :
        print 'Got something wrong!!!!!!!!!!!!!!!!!!!!!!!!!!!!'

if __name__ == "__main__":

    prefix = "test_"
    # Initial data
    data = {}
    data['configuration'] = {}
    data['configuration']['task_type'] = 'kb'
    data['configuration']['task_batch_size'] = 10
    data['configuration']['num_assignments'] = 3
    data['configuration']['callback_url'] = 'www.google.com'
    data['configuration']['amt'] = {'sandbox' : True,
                                    'title' : 'Let\'s play with web tables',
                                    'description' : 'Assess statements about web tables.'}

    data['group_id'] = 'test'
    data['group_context'] = {}
    data['content'] = {}

    # Result Files
    file_name1 = '../../../../data/Result/enrichment/1.txt'
    file_name2 = '../../../../data/Result/enrichment/2.txt'
    file_name3 = '../../../../data/Result/enrichment/3.txt'

    # Data structures
    mp = {}
    maxk1 = 5000
    maxk2 = 3000
    maxk3 = 1000
    k = 0
    statements = Set()

    # 1.txt - fact triples
    f1 = open(file_name1, 'r')
    k = 0
    statements = Set()
    for line in f1.readlines():

        # empty line
        if len(line) == 1:
            continue

        # table_id line
        if line[0] == '#':
            tid = int(re.search(r'\d+', line).group())
            if not tid in mp.keys():
                mp[tid] = {'ids' : [], 'contents' : []}
        # statement line
        else:
            hash = line[line.find('________') + 8:]
            if not hash in statements and hash.find('null') == -1:
                statements.add(hash)
                # id
                k += 1
                id = prefix + '1_' + str(k)
                mp[tid]['ids'].append(id)
                # content
                r = int(line[0:line.find('________')])
                content = 'Row ' + str(r) + ': '
                cell1 = hash[0:hash.find('________')]
                hash = hash[hash.find('________') + 8:]
                rel = hash[0:hash.find('________')]
                hash = hash[hash.find('________') + 8:]
                cell2 = hash[0:-1]
                if rel[0] == '*':
                    rel = rel[3:-3]
                    cell0 = cell1
                    cell1 = cell2
                    cell2 = cell0
                content += '<b>' + cell1 + '</b> '
                content += '<font color = \"red\">' + rel + '</font> '
                content += '<b>' + cell2 + '</b>'
                mp[tid]['contents'].append(content.decode('latin-1'))

        # top k
        if k >= maxk1:
            break

    f1.close()

    # 2.txt - attr type pairs
    f2 = open(file_name2, 'r')
    k = 0
    statements = Set()
    for line in f2.readlines():

        # empty line
        if len(line) == 1:
            continue

        # table_id line
        if line[0] == '#':
            tid = int(re.search(r'\d+', line).group())
            if not tid in mp.keys():
                mp[tid] = {'ids' : [], 'contents' : []}            
        else:
            hash = line[line.find('________') + 8:]
            if not hash in statements and hash.find('null') == -1:
                statements.add(hash)
                # id
                k += 1
                id = prefix + '2_' + str(k)
                mp[tid]['ids'].append(id)
                # content
                r = int(line[0:line.find('________')])
                content = 'Row ' + str(r) + ': '
                content += '<b>' + hash[0:hash.find('________')] + '</b> '
                hash = hash[hash.find('________') + 8:]
                if hash.find('wikicategory_') == 0:
                    hash = hash[hash.find('_') + 1:]
                content += 'is one of <b>' + hash[0:-1] + '</b>'
                mp[tid]['contents'].append(content.decode('latin-1'))

        # top k
        if k >= maxk2:
            break

    f2.close()

    # 3.txt - entity type pairs
    f3 = open(file_name3, 'r')
    k = 0
    statements = Set()
    for line in f3.readlines():

        # empty line
        if len(line) == 1:
            continue

        # table_id line
        if line[0] == '#':
            ints = map(int, re.findall(r'\d+', line))
            tid = ints[0]
            r = ints[1]
            if not tid in mp.keys():
                mp[tid] = {'ids' : [], 'contents' : []}            
        else:
            hash = line
            if not hash in statements and hash.find('null') == -1:
                statements.add(hash)
                # id
                k += 1
                id = prefix + '3_' + str(k)
                mp[tid]['ids'].append(id)
                # content
                content = 'Row ' + str(r) + ': '
                content += '<b>' + hash[0:hash.find('________')] + '</b> '
                hash = hash[hash.find('________') + 8:]
                if hash.find('wikicategory_') == 0:
                    hash = hash[hash.find('_') + 1:]
                content += 'is one of <b>' + hash[0:-1] + '</b>'
                mp[tid]['contents'].append(content.decode('latin-1'))

        # top k
        if k >= maxk3:
            break

    f3.close()

    # send requests
    total_hits = 0
    for tid in mp.keys():

        cur_data = copy.deepcopy(data)
        cur_data['group_id'] = 'test_' + str(tid)
        cur_data['group_context']['table_url'] = '/table/crowdtable/?tableID=' + str(tid)

        for i in range(0, len(mp[tid]['ids'])):
            cur_data['content'][mp[tid]['ids'][i]] = mp[tid]['contents'][i]

        print 'Sending request with table_id = ' + str(tid)
        send_request(cur_data)
        total_hits += len(mp[tid]['ids']) / 10 + 1

    print total_hits
