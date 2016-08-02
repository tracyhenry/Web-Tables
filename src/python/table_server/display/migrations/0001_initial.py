# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
    ]

    operations = [
        migrations.CreateModel(
            name='ColRelation',
            fields=[
                ('id', models.IntegerField(serialize=False, primary_key=True)),
                ('table_id', models.IntegerField()),
                ('col_id', models.IntegerField()),
                ('relation', models.CharField(max_length=255)),
                ('verdict', models.IntegerField()),
            ],
            options={
                'db_table': 'col_relation',
            },
            bases=(models.Model,),
        ),
        migrations.CreateModel(
            name='FuzzyMatch',
            fields=[
                ('id', models.IntegerField(serialize=False, primary_key=True)),
                ('row', models.IntegerField()),
                ('col', models.IntegerField()),
                ('table_id', models.IntegerField()),
                ('entity_id', models.IntegerField()),
            ],
            options={
                'db_table': 'fuzzy_match',
            },
            bases=(models.Model,),
        ),
        migrations.CreateModel(
            name='GoldAnnotation',
            fields=[
                ('table_id', models.IntegerField(null=True)),
                ('col', models.IntegerField(null=True, blank=True)),
                ('gold_type', models.CharField(max_length=1000, blank=True)),
                ('id', models.IntegerField(serialize=False, primary_key=True)),
            ],
            options={
                'db_table': 'gold_annotation',
            },
            bases=(models.Model,),
        ),
        migrations.CreateModel(
            name='RecConcept',
            fields=[
                ('id', models.IntegerField(serialize=False, primary_key=True)),
                ('res_id', models.IntegerField()),
                ('table_id', models.IntegerField()),
                ('row_id', models.IntegerField()),
                ('rank', models.IntegerField()),
                ('concept', models.CharField(max_length=255)),
                ('verdict', models.IntegerField()),
            ],
            options={
                'db_table': 'rec_concept',
            },
            bases=(models.Model,),
        ),
        migrations.CreateModel(
            name='TableCells',
            fields=[
                ('table_url', models.CharField(max_length=500, blank=True)),
                ('row', models.IntegerField(null=True, blank=True)),
                ('col', models.IntegerField(null=True, blank=True)),
                ('value', models.TextField(blank=True)),
                ('table_id', models.IntegerField(null=True)),
                ('id', models.IntegerField(serialize=False, primary_key=True)),
            ],
            options={
                'db_table': 'table_cells',
            },
            bases=(models.Model,),
        ),
        migrations.CreateModel(
            name='TableContext',
            fields=[
                ('table_url', models.CharField(max_length=500, blank=True)),
                ('content', models.TextField(blank=True)),
                ('table_id', models.IntegerField(null=True)),
                ('id', models.IntegerField(serialize=False, primary_key=True)),
            ],
            options={
                'db_table': 'table_context',
            },
            bases=(models.Model,),
        ),
        migrations.CreateModel(
            name='TableSchema',
            fields=[
                ('table_url', models.CharField(max_length=500, blank=True)),
                ('att_id', models.IntegerField(null=True, blank=True)),
                ('att_name', models.CharField(max_length=2000, blank=True)),
                ('att_type', models.CharField(max_length=10, blank=True)),
                ('is_entity', models.IntegerField(null=True, blank=True)),
                ('table_id', models.IntegerField(null=True)),
                ('id', models.IntegerField(serialize=False, primary_key=True)),
            ],
            options={
                'db_table': 'table_schema',
            },
            bases=(models.Model,),
        ),
        migrations.CreateModel(
            name='YagoConcept',
            fields=[
                ('id', models.IntegerField(serialize=False, primary_key=True)),
                ('value', models.CharField(max_length=255)),
            ],
            options={
                'db_table': 'yago_concept',
            },
            bases=(models.Model,),
        ),
        migrations.CreateModel(
            name='YagoEntity',
            fields=[
                ('id', models.IntegerField(serialize=False, primary_key=True)),
                ('value', models.CharField(max_length=255)),
            ],
            options={
                'db_table': 'yago_entity',
            },
            bases=(models.Model,),
        ),
        migrations.CreateModel(
            name='YagoType',
            fields=[
                ('id', models.IntegerField(serialize=False, primary_key=True)),
                ('entity_id', models.IntegerField()),
                ('concept_id', models.IntegerField()),
            ],
            options={
                'db_table': 'yago_type',
            },
            bases=(models.Model,),
        ),
    ]
