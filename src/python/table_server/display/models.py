from __future__ import unicode_literals

from django.db import models


class DjangoMigrations(models.Model):
    id = models.IntegerField(primary_key=True)
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
    id = models.IntegerField(null=True)

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

class YagoEntity(models.Model):
    id = models.IntegerField(primary_key=True)
    value = models.CharField(max_length=255)

    class Meta:
        db_table = 'yago_entity'

class FuzzyMatch(models.Model):
    id = models.IntegerField(primary_key=True)
    row = models.IntegerField()
    col = models.IntegerField()
    table_id = models.IntegerField()
    entity_id = models.IntegerField()

    class Meta:
        db_table = 'fuzzy_match'

class YagoConcept(models.Model):
    id = models.IntegerField(primary_key=True)
    value = models.CharField(max_length=255)

    class Meta:
        db_table = 'yago_concept'

class YagoType(models.Model):
    id = models.IntegerField(primary_key=True)
    entity_id = models.IntegerField()
    concept_id = models.IntegerField()

    class Meta:
        db_table = 'yago_type'

