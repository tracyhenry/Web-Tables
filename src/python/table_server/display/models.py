# This is an auto-generated Django model module.
# You'll have to do the following manually to clean this up:
#   * Rearrange models' order
#   * Make sure each model has one field with primary_key=True
#   * Remove `managed = False` lines if you wish to allow Django to create, modify, and delete the table
# Feel free to rename the models, but don't rename db_table values or field names.
#
# Also note: You'll have to insert the output of 'django-admin.py sqlcustom [app_label]'
# into your database.
from __future__ import unicode_literals

from django.db import models


class DjangoMigrations(models.Model):
    id = models.IntegerField(primary_key=True)  # AutoField?
    app = models.CharField(max_length=255)
    name = models.CharField(max_length=255)
    applied = models.DateTimeField()

    class Meta:
        db_table = 'django_migrations'


class GoldAnnotation(models.Model):
    table_id = models.IntegerField(primary_key=True)
    col = models.IntegerField(blank=True, null=True)
    gold_type = models.CharField(max_length=1000, blank=True)

    class Meta:
        db_table = 'gold_annotation'


class TableCells(models.Model):
    table_url = models.CharField(max_length=500, blank=True)
    row = models.IntegerField(blank=True, null=True)
    col = models.IntegerField(blank=True, null=True)
    value = models.TextField(blank=True)
    table_id = models.IntegerField(primary_key=True)

    class Meta:
        db_table = 'table_cells'


class TableContext(models.Model):
    table_url = models.CharField(max_length=500, blank=True)
    content = models.TextField(blank=True)
    table_id = models.IntegerField(primary_key=True)

    class Meta:
        db_table = 'table_context'


class TableSchema(models.Model):
    table_url = models.CharField(max_length=500, blank=True)
    att_id = models.IntegerField(blank=True, null=True)
    att_name = models.CharField(max_length=2000, blank=True)
    att_type = models.CharField(max_length=10, blank=True)
    is_entity = models.IntegerField(blank=True, null=True)
    table_id = models.IntegerField(primary_key=True)

    class Meta:
        db_table = 'table_schema'
