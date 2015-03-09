# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
    ]

    operations = [
        migrations.CreateModel(
            name='DjangoMigrations',
            fields=[
                ('id', models.IntegerField(serialize=False, primary_key=True)),
                ('app', models.CharField(max_length=255)),
                ('name', models.CharField(max_length=255)),
                ('applied', models.DateTimeField()),
            ],
            options={
                'db_table': 'django_migrations',
            },
            bases=(models.Model,),
        ),
        migrations.CreateModel(
            name='GoldAnnotation',
            fields=[
                ('table_id', models.IntegerField(serialize=False, primary_key=True)),
                ('col', models.IntegerField(null=True, blank=True)),
                ('gold_type', models.CharField(max_length=1000, blank=True)),
            ],
            options={
                'db_table': 'gold_annotation',
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
                ('table_id', models.IntegerField(serialize=False, primary_key=True)),
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
                ('table_id', models.IntegerField(serialize=False, primary_key=True)),
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
                ('table_id', models.IntegerField(serialize=False, primary_key=True)),
            ],
            options={
                'db_table': 'table_schema',
            },
            bases=(models.Model,),
        ),
    ]
