# -*- coding: utf-8 -*-

import xml.etree.ElementTree as ET
import os.path
import fnmatch
import glob
import sys
import os

# Extract all XML Ground Truth files

path = os.getcwd()
path = os.path.dirname(path)
path = os.path.dirname(path)
path = os.path.dirname(path)
path = os.path.join(path, 'data', 'Table', 'WWT_xml', 'Ground Truth')

matches = []
for root, dirnames, filenames in os.walk(path):
    for filename in fnmatch.filter(filenames, '*.xml'):
        matches.append(os.path.join(root, filename))

# parse every xml file and output all GTs to a single txt file
column_gt_file = open('Column_Concept_GT.txt', 'w')
rel_gt_file = open('Relationship_GT.txt', 'w')

for xml_file in matches:
    if xml_file.find('?') != -1:
        continue
    root = ET.parse(xml_file).getroot()
    # deal with column concepts
    for i in range(len(root)):
        if root[i].tag == 'columnAnnotations':
            for j in range(len(root[i])):
                column_gt_file.write(xml_file[xml_file.find('annotation') + 10:] + ' ')
                column_gt_file.write(root[i][j].get('col') + ' ')
                for annos in root[i][j]:
                    column_gt_file.write(annos.get('name').encode('UTF-8') + ' ')
                column_gt_file.write('\n')
        elif root[i].tag == 'relationshipAnnotations':
            for j in range(len(root[i])):
                has_rel = False
                for rel in root[i][j]:
                    if rel.text.find('NULL') == -1:
                        has_rel = True
                        break
                if not has_rel:
                    continue
                rel_gt_file.write(xml_file[xml_file.find('annotation') + 10:] + ' ')
                rel_gt_file.write(root[i][j].get('col1') + ' ' + root[i][j].get('col2') + ' ')
                for rel in root[i][j]:
                    if rel.text.find('NULL') == -1:
                        rel_gt_file.write(rel.text + ' ')
                rel_gt_file.write('\n')

column_gt_file.close()
rel_gt_file.close()
