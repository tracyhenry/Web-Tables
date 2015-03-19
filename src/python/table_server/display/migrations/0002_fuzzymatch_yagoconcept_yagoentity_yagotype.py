# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('display', '0001_initial'),
    ]

    operations = [
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
