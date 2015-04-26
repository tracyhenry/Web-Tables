# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('display', '0004_colrelation'),
    ]

    operations = [
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
    ]
